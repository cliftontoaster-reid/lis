#include <gtest/gtest.h>

#include <glaze/json/read.hpp>

#include "fchat/c2s.hpp"
#include "fchat/meta.hpp"

using DiceExpr = fchat::c2s::RLL::DiceExpr;
using Dice = fchat::c2s::RLL::Dice;

namespace {

auto make(std::vector<std::variant<Dice, uint16_t>> parts) -> DiceExpr {
  DiceExpr ret;
  ret.value = std::move(parts);
  return ret;
}

auto make_insane(std::size_t n) -> fchat::c2s::RLL::DiceExpr {
  std::vector<std::variant<Dice, uint16_t>> parts;
  parts.reserve(n + 1);
  for (std::size_t i = 0; i < n; ++i)
    parts.emplace_back(Dice{.mul = 1, .sides = 6, .add = 0});  // d6 × n
  parts.emplace_back(uint16_t{5});  // single number, at the end
  fchat::c2s::RLL::DiceExpr e;
  e.value = std::move(parts);
  return e;
}

template <typename E>
void expect_enum_roundtrip(E value, std::string_view json_str) {
  std::string out;
  EXPECT_FALSE(glz::write_json(value, out)) << "write_json failed";
  EXPECT_EQ(out, json_str);
  E read{};
  EXPECT_FALSE(glz::read_json(read, json_str))
      << "read_json failed for " << json_str;
  EXPECT_EQ(read, value);
}

}  // namespace

TEST(Meta_c2s_IDN, WritesMethodAsTicket) {
  fchat::c2s::IDN idn{};  // default-constructed is enough;
  idn.account = "acc";    // the getter ignores the object anyway
  idn.ticket = "tkt";
  idn.character = "char";

  std::string json;
  const auto ec = glz::write_json(idn, json);
  ASSERT_FALSE(ec) << "write_json failed";

  // "method" is a write-only getter in the meta; glz writes it as the first
  // key in compact JSON, so assert it is present in the serialized output.
  EXPECT_NE(json.find(R"("method":"ticket")"), std::string::npos);
}

TEST(Meta_c2s_IDN, RoundTripFields) {
  fchat::c2s::IDN out{};
  out.account = "acc";
  out.ticket = "tkt";
  out.character = "char";
  // cname/cversion left at their defaults (LIS_NAME / LIS_VERSION).

  std::string json;
  ASSERT_FALSE(glz::write_json(out, json)) << "write_json failed";

  fchat::c2s::IDN in{};
  ASSERT_FALSE(glz::read_json(in, json)) << "read_json failed";

  EXPECT_EQ(in.account, "acc");
  EXPECT_EQ(in.ticket, "tkt");
  EXPECT_EQ(in.character, "char");
  EXPECT_EQ(in.cname, out.cname);  // defaults preserved through round-trip
  EXPECT_EQ(in.cversion, out.cversion);
}

TEST(Meta_c2s_IDN, MethodIgnoredOnRead) {
  // "method" is a write-only getter in the meta; a payload that still carries
  // it (as the client emits) must parse without error, and the real fields
  // must populate. This locks in graceful handling of the extra key.
  const std::string json = R"({
    "method": "ticket",
    "account": "acc",
    "ticket": "tkt",
    "character": "char"
  })";

  fchat::c2s::IDN in{};
  EXPECT_FALSE(glz::read_json(in, json))
      << "read_json failed despite the write-only 'method' key";

  EXPECT_EQ(in.account, "acc");
  EXPECT_EQ(in.ticket, "tkt");
  EXPECT_EQ(in.character, "char");
}

TEST(Meta_c2s_RLL_DiceExpr, StringVariantWritesRaw) {
  fchat::c2s::RLL::DiceExpr expr;
  expr.value = std::string{"bottle"};  // selects the string alternative

  std::string json;
  ASSERT_FALSE(glz::write_json(expr, json)) << "write_json failed";

  // The custom meta writes the raw string directly, so the serialized output
  // is a JSON string literal "bottle" — NOT an object {"value":"bottle"}.
  EXPECT_EQ(json, R"("bottle")");
}

TEST(Meta_c2s_RLL_DiceExpr, SingleDice_Default) {
  auto expr = make({Dice{.mul = 1, .sides = 6, .add = 0}});

  std::string out;
  const auto ec = glz::write_json(expr, out);
  EXPECT_FALSE(ec);
  EXPECT_EQ(out, "\"d6\"");
}

TEST(Meta_c2s_RLL_DiceExpr, DiceWithMultiplier) {
  auto expr = make({Dice{.mul = 2, .sides = 10, .add = 0}});

  std::string out;
  const auto ec = glz::write_json(expr, out);
  EXPECT_FALSE(ec);
  EXPECT_EQ(out, "\"2d10\"");
}

TEST(Meta_c2s_RLL_DiceExpr, DiceWithAdd) {
  auto expr = make({Dice{.mul = 1, .sides = 20, .add = 3}});

  std::string out;
  const auto ec = glz::write_json(expr, out);
  EXPECT_FALSE(ec);
  EXPECT_EQ(out, "\"d20+3\"");
}

TEST(Meta_c2s_RLL_DiceExpr, DiceWithNegativeAdd) {
  auto expr = make({Dice{.mul = 3, .sides = 6, .add = -2}});

  std::string out;
  const auto ec = glz::write_json(expr, out);
  EXPECT_FALSE(ec);
  EXPECT_EQ(out, "\"3d6+-2\"");
}

TEST(Meta_c2s_RLL_DiceExpr, NumberTerm) {
  auto expr = make({Dice{.mul = 1, .sides = 6, .add = 0}, uint16_t{5}});

  std::string out;
  const auto ec = glz::write_json(expr, out);
  EXPECT_FALSE(ec);
  EXPECT_EQ(out, "\"d6+5\"");
}

TEST(Meta_c2s_RLL_DiceExpr, MixedChain) {
  auto expr = make({
      Dice{.mul = 2, .sides = 6, .add = 0},
      Dice{.mul = 1, .sides = 4, .add = 1},
      uint16_t{3},
  });

  std::string out;
  const auto ec = glz::write_json(expr, out);
  EXPECT_FALSE(ec);
  EXPECT_EQ(out, "\"2d6+d4+1+3\"");
}

TEST(Meta_c2s_RLL_DiceExpr, InsanelyLong) {
  constexpr std::size_t N = 20'000;
  auto e = make_insane(N);

  std::string out;
  ASSERT_FALSE(glz::write_json(e, out));

  // Build the expected stream the same way the writer does:
  // "d6" + "+d6"*(N-1) + "+5", JSON-quoted.
  std::string expected = "\"d6";
  for (std::size_t i = 1; i < N; ++i) {
    expected += "+d6";
  }
  expected += "+5\"";

  EXPECT_EQ(out, expected);
  EXPECT_EQ(out.front(), '"');
  EXPECT_EQ(out.back(), '"');
  // N dice + 1 number = N+1 terms => exactly N '+' separators.
  EXPECT_EQ(std::count(out.begin(), out.end(), '+'), static_cast<long>(N));
  EXPECT_EQ(out.substr(1, 2), "d6");
  EXPECT_EQ(out.substr(out.size() - 2, 2), "5\"");
}

TEST(Meta_c2s_RLL_DiceExpr, ReadStringBackToVariant) {
  // The custom writer is ONE-WAY: it emits a human-readable string, not a
  // structured value. Reading that string back therefore selects the
  // std::string alternative of the variant (it is NOT reparsed into Dice
  // terms). This pins that behavior so it cannot regress silently.
  const auto round_trips_to_string = [](std::string_view expected) {
    fchat::c2s::RLL::DiceExpr out;
    out.value = std::string{expected};

    std::string json;
    ASSERT_FALSE(glz::write_json(out, json)) << "write_json failed";
    // write_json emits a JSON string literal -> the value wrapped in quotes.
    ASSERT_EQ(json, std::string{"\""} + std::string{expected} + "\"");

    fchat::c2s::RLL::DiceExpr in{};
    ASSERT_FALSE(glz::read_json(in, json)) << "read_json failed";
    ASSERT_TRUE(std::holds_alternative<std::string>(in.value));
    EXPECT_EQ(std::get<std::string>(in.value), expected);
  };

  round_trips_to_string("bottle");
  round_trips_to_string("2d6+d4+1+3");
}

// ----- ChannelMode -----------------------------------------------------------
TEST(Meta_ChannelMode, EnumerateBothWays) {
  expect_enum_roundtrip(fchat::ChannelMode::CHAT, "\"chat\"");
  expect_enum_roundtrip(fchat::ChannelMode::ADS, "\"ads\"");
  expect_enum_roundtrip(fchat::ChannelMode::BOTH, "\"both\"");
}
TEST(Meta_ChannelMode, InvalidValueErrors) {
  fchat::ChannelMode v{};
  EXPECT_TRUE(glz::read_json(v, "\"bogus\""));
}

// ----- c2s::STA::Status ------------------------------------------------------
TEST(Meta_c2s_STA_Status, EnumerateBothWays) {
  using E = fchat::c2s::STA::Status;
  expect_enum_roundtrip(E::ONLINE, "\"online\"");
  expect_enum_roundtrip(E::LOOKING, "\"looking\"");
  expect_enum_roundtrip(E::BUSY, "\"busy\"");
  expect_enum_roundtrip(E::DND, "\"dnd\"");
  expect_enum_roundtrip(E::IDLE, "\"idle\"");
  expect_enum_roundtrip(E::AWAY, "\"away\"");
  expect_enum_roundtrip(E::CROWN, "\"crown\"");  // gap closed in meta.hpp
}
TEST(Meta_c2s_STA_Status, InvalidValueErrors) {
  fchat::c2s::STA::Status v{};
  EXPECT_TRUE(glz::read_json(v, "\"nope\""));
}

// ----- c2s::TPN::Status ------------------------------------------------------
TEST(Meta_c2s_TPN_Status, EnumerateBothWays) {
  using E = fchat::c2s::TPN::Status;
  expect_enum_roundtrip(E::CLEAR, "\"clear\"");
  expect_enum_roundtrip(E::PAUSED, "\"paused\"");
  expect_enum_roundtrip(E::TYPING, "\"typing\"");
}
TEST(Meta_c2s_TPN_Status, InvalidValueErrors) {
  fchat::c2s::TPN::Status v{};
  EXPECT_TRUE(glz::read_json(v, "\"zzz\""));
}

// ----- s2c::KID::KidType -----------------------------------------------------
TEST(Meta_s2c_KID_KidType, EnumerateBothWays) {
  using E = fchat::s2c::KID::KidType;
  expect_enum_roundtrip(E::START, "\"start\"");
  expect_enum_roundtrip(E::CUSTOM, "\"custom\"");
  expect_enum_roundtrip(E::END, "\"end\"");
}
TEST(Meta_s2c_KID_KidType, InvalidValueErrors) {
  fchat::s2c::KID::KidType v{};
  EXPECT_TRUE(glz::read_json(v, "\"??\""));
}

// ----- s2c::IGN::Action ------------------------------------------------------
TEST(Meta_s2c_IGN_Action, EnumerateBothWays) {
  using E = fchat::s2c::IGN::Action;
  expect_enum_roundtrip(E::INIT, "\"init\"");
  expect_enum_roundtrip(E::ADD, "\"add\"");
  expect_enum_roundtrip(E::DELETE, "\"delete\"");
  expect_enum_roundtrip(E::NOTIFY, "\"notify\"");
}
TEST(Meta_s2c_IGN_Action, InvalidValueErrors) {
  fchat::s2c::IGN::Action v{};
  EXPECT_TRUE(glz::read_json(v, "\"wat\""));
}

// ----- s2c::PRD::ProfileType -------------------------------------------------
TEST(Meta_s2c_PRD_ProfileType, EnumerateBothWays) {
  using E = fchat::s2c::PRD::ProfileType;
  expect_enum_roundtrip(E::START, "\"start\"");
  expect_enum_roundtrip(E::INFO, "\"info\"");
  expect_enum_roundtrip(E::SELECT, "\"select\"");
  expect_enum_roundtrip(E::END, "\"end\"");
}
TEST(Meta_s2c_PRD_ProfileType, InvalidValueErrors) {
  fchat::s2c::PRD::ProfileType v{};
  EXPECT_TRUE(glz::read_json(v, "\"???\""));
}

// ----- s2c op/channel/character (CBU, CKU, CTU)
// -------------------------------
namespace {

template <typename T>
void expect_op_channel_character(std::string_view op, std::string_view channel,
                                 std::string_view character) {
  std::string o{op};
  std::string c{channel};
  std::string ch{character};
  T out{.op = o, .channel = c, .character = ch};
  std::string json;
  ASSERT_FALSE(glz::write_json(out, json)) << "write_json failed";
  ASSERT_NE(json.find(R"("operator")"), std::string::npos);
  ASSERT_EQ(json.find(R"("op")"), std::string::npos);

  T in{};
  ASSERT_FALSE(glz::read_json(in, json)) << "read_json failed";
  EXPECT_EQ(std::string(in.op), std::string(op));
  EXPECT_EQ(std::string(in.channel), std::string(channel));
  EXPECT_EQ(std::string(in.character), std::string(character));
}

}  // namespace

TEST(Meta_s2c_CBU, KeyAndRoundTrip) {
  expect_op_channel_character<fchat::s2c::CBU>("mod", "main", "Bob");
}
TEST(Meta_s2c_CKU, KeyAndRoundTrip) {
  expect_op_channel_character<fchat::s2c::CKU>("mod", "main", "Bob");
}
TEST(Meta_s2c_CTU, KeyAndRoundTrip) {
  expect_op_channel_character<fchat::s2c::CTU>("mod", "main", "Bob");
}

// ----- s2c::ICH
// ---------------------------------------------------------------
TEST(Meta_s2c_ICH, CustomUsersReader) {
  const std::string json =
      R"({"users":[{"identity":"A"},{"identity":"B"}],"channel":"X","mode":"both"})";
  fchat::s2c::ICH in{};
  ASSERT_FALSE(glz::read_json(in, json)) << "read_json failed";
  ASSERT_EQ(in.users.size(), 2u);
  EXPECT_EQ(std::string(in.users[0]), "A");
  EXPECT_EQ(std::string(in.users[1]), "B");
  EXPECT_EQ(std::string(in.channel), "X");
  EXPECT_EQ(in.mode, fchat::ChannelMode::BOTH);
}
TEST(Meta_s2c_ICH, MalformedUsersErrors) {
  // A malformed "users" value (not an array of objects) makes the whole
  // message fail to parse. The users field is left in its default (empty)
  // state rather than retaining partial data.
  const std::string json =
      R"({"users":"not_an_array","channel":"X","mode":"both"})";
  fchat::s2c::ICH in{};
  const auto ec = glz::read_json(in, json);
  EXPECT_TRUE(ec);
  EXPECT_TRUE(in.users.empty());
}

// ----- s2c::JCH
// ---------------------------------------------------------------
TEST(Meta_s2c_JCH, CustomCharacterReader) {
  const std::string json =
      R"({"channel":"C","character":{"identity":"Bob"},"title":"T"})";
  fchat::s2c::JCH in{};
  ASSERT_FALSE(glz::read_json(in, json)) << "read_json failed";
  EXPECT_EQ(std::string(in.channel), "C");
  EXPECT_EQ(std::string(in.character), "Bob");
  EXPECT_EQ(std::string(in.title), "T");
}
TEST(Meta_s2c_JCH, MalformedCharacterErrors) {
  const std::string json =
      R"({"channel":"C","character":"not_an_object","title":"T"})";
  fchat::s2c::JCH in{};  // character defaults to empty string_view
  const auto ec = glz::read_json(in, json);
  EXPECT_TRUE(ec);                    // whole message fails to parse
  EXPECT_TRUE(in.character.empty());  // no partial data retained
}

// ----- s2c::LIS::CharacterInfo (array, not object)
// ----------------------------
TEST(Meta_s2c_LIS_CharacterInfo, ArrayNotObject) {
  std::string n = "Alice", g = "F", s = "online", m = "hi";
  fchat::s2c::LIS::CharacterInfo ci{
      .name = n, .gender = g, .status = s, .status_message = m};
  std::string json;
  ASSERT_FALSE(glz::write_json(ci, json)) << "write_json failed";
  EXPECT_EQ(json, R"(["Alice","F","online","hi"])");
}
TEST(Meta_s2c_LIS_CharacterInfo, ReadArrayInOrder) {
  const std::string json = R"(["Alice","Female","online","hello"])";
  fchat::s2c::LIS::CharacterInfo ci{};
  ASSERT_FALSE(glz::read_json(ci, json)) << "read_json failed";
  EXPECT_EQ(std::string(ci.name), "Alice");
  EXPECT_EQ(std::string(ci.gender), "Female");
  EXPECT_EQ(std::string(ci.status), "online");
  EXPECT_EQ(std::string(ci.status_message), "hello");
}
TEST(Meta_s2c_LIS_CharacterInfo, RoundTrip) {
  std::string n = "Alice", g = "F", s = "online", m = "hi";
  fchat::s2c::LIS::CharacterInfo out{
      .name = n, .gender = g, .status = s, .status_message = m};
  std::string json;
  ASSERT_FALSE(glz::write_json(out, json));
  fchat::s2c::LIS::CharacterInfo in{};
  ASSERT_FALSE(glz::read_json(in, json));
  EXPECT_EQ(std::string(in.name), "Alice");
  EXPECT_EQ(std::string(in.gender), "F");
  EXPECT_EQ(std::string(in.status), "online");
  EXPECT_EQ(std::string(in.status_message), "hi");
}

// ----- s2c::RLL (tagged variant)
// ----------------------------------------------
TEST(Meta_s2c_RLL, TaggedDiceVariant) {
  const std::string json =
      R"({"type":"dice","channel":"c","character":"Bob","message":"m","results":[1,2],"rolls":["a","b"],"endresult":5})";
  fchat::s2c::RLL v{};
  ASSERT_FALSE(glz::read_json(v, json)) << "read_json failed";
  auto* d = std::get_if<fchat::s2c::RllDice>(&v);
  ASSERT_NE(d, nullptr);
  EXPECT_EQ(std::string(d->channel), "c");
  EXPECT_EQ(std::string(d->character), "Bob");
  EXPECT_EQ(d->endresult, 5);
  ASSERT_EQ(d->results.size(), 2u);
  EXPECT_EQ(d->results[0], 1);
}
TEST(Meta_s2c_RLL, TaggedBottleVariant) {
  const std::string json =
      R"({"type":"bottle","channel":"c","character":"Bob","message":"m","target":"T"})";
  fchat::s2c::RLL v{};
  ASSERT_FALSE(glz::read_json(v, json)) << "read_json failed";
  auto* b = std::get_if<fchat::s2c::RllBottle>(&v);
  ASSERT_NE(b, nullptr);
  EXPECT_EQ(std::string(b->target), "T");
  EXPECT_EQ(std::string(b->channel), "c");
}
TEST(Meta_s2c_RLL, UnknownTagErrors) {
  const std::string json = R"({"type":"nope"})";
  fchat::s2c::RLL v{};
  EXPECT_TRUE(glz::read_json(v, json));
}
TEST(Meta_s2c_RLL, RoundTrip) {
  fchat::s2c::RllBottle b{
      .channel = "c", .character = "Bob", .message = "m", .target = "T"};
  fchat::s2c::RLL v = b;
  std::string json;
  ASSERT_FALSE(glz::write_json(v, json)) << "write_json failed";
  EXPECT_NE(json.find(R"("type":"bottle")"), std::string::npos);
  fchat::s2c::RLL back{};
  ASSERT_FALSE(glz::read_json(back, json)) << "read_json failed";
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::RllBottle>(back));
  EXPECT_EQ(std::string(std::get<fchat::s2c::RllBottle>(back).target), "T");
}

// ----- s2c::SFC (tagged variant)
// ----------------------------------------------
TEST(Meta_s2c_SFC, TaggedConfirmVariant) {
  const std::string json =
      R"({"action":"confirm","moderator":"m","character":"c","timestamp":"t"})";
  fchat::s2c::SFC v{};
  ASSERT_FALSE(glz::read_json(v, json)) << "read_json failed";
  auto* p = std::get_if<fchat::s2c::SfcConfirm>(&v);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(std::string(p->moderator), "m");
  EXPECT_EQ(std::string(p->character), "c");
}
TEST(Meta_s2c_SFC, TaggedReportVariant) {
  const std::string json =
      R"({"action":"report","callid":7,"report":"r","timestamp":"t","character":"c","logid":3})";
  fchat::s2c::SFC v{};
  ASSERT_FALSE(glz::read_json(v, json)) << "read_json failed";
  auto* p = std::get_if<fchat::s2c::SfcReport>(&v);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->callid, 7);
  EXPECT_EQ(p->logid, 3);
  EXPECT_EQ(std::string(p->report), "r");
}
TEST(Meta_s2c_SFC, UnknownActionErrors) {
  const std::string json = R"({"action":"weird"})";
  fchat::s2c::SFC v{};
  EXPECT_TRUE(glz::read_json(v, json));
}
TEST(Meta_s2c_SFC, RoundTrip) {
  fchat::s2c::SfcReport r{.report = "r", .character = "c", .logid = 3};
  fchat::s2c::SFC v = r;
  std::string json;
  ASSERT_FALSE(glz::write_json(v, json)) << "write_json failed";
  EXPECT_NE(json.find(R"("action":"report")"), std::string::npos);
  fchat::s2c::SFC back{};
  ASSERT_FALSE(glz::read_json(back, json)) << "read_json failed";
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::SfcReport>(back));
  EXPECT_EQ(std::string(std::get<fchat::s2c::SfcReport>(back).report), "r");
}
