
#include <gtest/gtest.h>

#include "fchat/c2s.hpp"

#define TEST_C2S(OPCODE, JSON, CHECK)                                          \
  TEST(C2S_Deserialize, Deserialize_##OPCODE##_Returns_##OPCODE) {             \
    const std::string json = JSON;                                             \
    const auto cmd =                                                           \
        fchat::c2s::deserialize_command(fchat::C2SOpCode::OPCODE, json);       \
    ASSERT_TRUE(std::holds_alternative<fchat::c2s::OPCODE>(cmd));              \
    CHECK(std::get<fchat::c2s::OPCODE>(cmd));                                  \
  }                                                                            \
  TEST(C2S_Serialize, Serialize_##OPCODE##_RoundTrips) {                       \
    const auto cmd =                                                           \
        fchat::c2s::deserialize_command(fchat::C2SOpCode::OPCODE, JSON);       \
    const std::string json = fchat::c2s::serialize_command(cmd);               \
    const auto reparsed =                                                      \
        fchat::c2s::deserialize_command(fchat::C2SOpCode::OPCODE, json);       \
    ASSERT_TRUE(std::holds_alternative<fchat::c2s::OPCODE>(reparsed));         \
    CHECK(std::get<fchat::c2s::OPCODE>(reparsed));                             \
  }                                                                            \
  TEST(C2S_Errors, Deserialize_##OPCODE##_MalformedThrows) {                   \
    EXPECT_THROW(                                                              \
        fchat::c2s::deserialize_command(fchat::C2SOpCode::OPCODE, "not json"), \
        fchat::parse_error);                                                   \
  }

TEST_C2S(ACB, R"({"character":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Bob"); })

TEST_C2S(AOP, R"({"character":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Bob"); })

TEST_C2S(AWC, R"({"character":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Bob"); })

TEST_C2S(BRO, R"({"message":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.message, "Bob"); })

TEST_C2S(CBL, R"({"channel":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.channel, "Bob"); })

TEST_C2S(CBU, R"({"character": "Charlie", "channel":"Bob"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.character, "Charlie");
           EXPECT_EQ(cmd.channel, "Bob");
         })

TEST_C2S(CCR, R"({"channel":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.channel, "Bob"); })

TEST_C2S(
    CDS,
    R"({"channel":"Bob", "description": "Lorem ipsum labore nisi sunt"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.channel, "Bob");
      EXPECT_EQ(cmd.description, "Lorem ipsum labore nisi sunt");
    })

TEST_C2S(CIU, R"({"channel":"Bob", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(CKU, R"({"channel":"Bob", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(COA, R"({"channel":"Bob", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(COL, R"({"character": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Charlie"); })

TEST_C2S(COR, R"({"channel":"Bob", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(CRC, R"({"channel":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.channel, "Bob"); })

TEST_C2S(CSO, R"({"channel":"Bob", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(CTU, R"({"channel":"Bob", "character": "Charlie", "length": 69})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
           EXPECT_EQ(cmd.length, 69);
         })

TEST_C2S(CUB, R"({"channel":"Bob", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(DOP, R"({"character": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Charlie"); })

TEST_C2S(FKS, R"({
  "kinks": [1, 2, 3],
  "genders": ["Male", "Female"],
  "orientations": ["Bisexual"],
  "languages": ["English"],
  "furryprefs": ["Dolphin"],
  "roles": ["Dominant", "Submissive"]
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.kinks.size(), 3u);
           EXPECT_EQ(cmd.kinks[0], 1);
           EXPECT_EQ(cmd.kinks[1], 2);
           EXPECT_EQ(cmd.kinks[2], 3);
           EXPECT_EQ(cmd.genders.size(), 2u);
           EXPECT_EQ(cmd.genders[0], "Male");
           EXPECT_EQ(cmd.genders[1], "Female");
           EXPECT_EQ(cmd.orientations.size(), 1u);
           EXPECT_EQ(cmd.orientations[0], "Bisexual");
           EXPECT_EQ(cmd.languages.size(), 1u);
           EXPECT_EQ(cmd.languages[0], "English");
           EXPECT_EQ(cmd.furryprefs.size(), 1u);
           EXPECT_EQ(cmd.furryprefs[0], "Dolphin");
           EXPECT_EQ(cmd.roles.size(), 2u);
           EXPECT_EQ(cmd.roles[0], "Dominant");
           EXPECT_EQ(cmd.roles[1], "Submissive");
         })

TEST_C2S(IDN, R"({
  "account": "Bob",
  "ticket": "tik",
  "character": "BobDaFox",
  "cname": "Lis",
  "cversion": "0.0.0"
})",
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.account, "Bob");
           EXPECT_EQ(cmd.ticket, "tik");
           EXPECT_EQ(cmd.character, "BobDaFox");
           EXPECT_EQ(cmd.cname, "Lis");
           EXPECT_EQ(cmd.cversion, "0.0.0");
         })

TEST_C2S(IGN, R"({"action":"add", "character": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.action, fchat::c2s::IGN::Action::ADD);
           EXPECT_EQ(cmd.character, "Charlie");
         })

TEST_C2S(JCH, R"({"channel": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.channel, "Charlie"); })

TEST_C2S(KIC, R"({"channel": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.channel, "Charlie"); })

TEST_C2S(KIK, R"({"character": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Charlie"); })

TEST_C2S(KIN, R"({"character": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Charlie"); })

TEST_C2S(LCH, R"({"channel": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.channel, "Charlie"); })

TEST_C2S(
    LRP,
    R"({"channel": "Charlie", "message": "Lorem ipsum labore nisi sunt"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.channel, "Charlie");
      EXPECT_EQ(cmd.message, "Lorem ipsum labore nisi sunt");
    })

TEST_C2S(
    MSG,
    R"({"channel": "Charlie", "message": "Lorem ipsum labore nisi sunt"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.channel, "Charlie");
      EXPECT_EQ(cmd.message, "Lorem ipsum labore nisi sunt");
    })

TEST_C2S(ORS,
         R"({})",  //
         [](const auto& cmd) -> auto {})

TEST_C2S(PIN,
         R"({})",  //
         [](const auto& cmd) -> auto {})

TEST_C2S(
    PRI,
    R"({"recipient": "Charlie", "message": "Lorem ipsum labore nisi sunt"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.recipient, "Charlie");
      EXPECT_EQ(cmd.message, "Lorem ipsum labore nisi sunt");
    })

TEST_C2S(PRO, R"({"character":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Bob"); })

TEST_C2S(RLL, R"({"dice":"2d6+3","channel":"#foo"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_TRUE(std::holds_alternative<std::string>(cmd.dice.value));
           EXPECT_EQ(std::get<std::string>(cmd.dice.value), "2d6+3");
           EXPECT_EQ(cmd.channel, "#foo");
         })

TEST_C2S(RLD, R"({"save":"no"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.save, "no"); })

TEST_C2S(RMO,
         R"({"channel": "Charlie", "mode": "both"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Charlie");
           EXPECT_EQ(cmd.mode, fchat::ChannelMode::BOTH);
         })

TEST_C2S(RST,
         R"({"channel": "Charlie", "status": "public"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Charlie");
           EXPECT_EQ(cmd.status, fchat::c2s::RST::Status::PUBLIC);
         })

TEST_C2S(RWD,
         R"({})",  //
         [](const auto& cmd) -> auto {})

TEST_C2S(SFC,
         R"({
  "action": "report",
  "logid": 6969,
  "report": "Sussy baka",
  "tab": "silly goose"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.action, "report");
           EXPECT_EQ(cmd.logid, 6969);
           EXPECT_EQ(cmd.report, "Sussy baka");
           EXPECT_EQ(cmd.tab, "silly goose");
         })

TEST_C2S(STA,
         R"({"statusmsg": "Charlie", "status": "online"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.statusmsg, "Charlie");
           EXPECT_EQ(cmd.status, fchat::c2s::STA::Status::ONLINE);
         })

TEST_C2S(TMO,
         R"({"character": "Charlie", "time": 55, "reason": "Sussy baka"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.character, "Charlie");
           EXPECT_EQ(cmd.time, 55);
           EXPECT_EQ(cmd.reason, "Sussy baka");
         })

TEST_C2S(TPN,
         R"({"character": "Charlie", "status": "clear"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.character, "Charlie");
           EXPECT_EQ(cmd.status, fchat::c2s::TPN::Status::CLEAR);
         })

TEST_C2S(UNB,
         R"({"character": "Charlie"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Charlie"); })

TEST_C2S(UPT,
         R"({})",  //
         [](const auto& cmd) -> auto {})

TEST(C2S_Defaults, IDN_CNameCVersionDefaults) {
  const std::string json =
      R"({"account":"Bob","ticket":"tik","character":"BobDaFox"})";
  const auto cmd = fchat::c2s::deserialize_command(fchat::C2SOpCode::IDN, json);
  ASSERT_TRUE(std::holds_alternative<fchat::c2s::IDN>(cmd));
  const auto& idn = std::get<fchat::c2s::IDN>(cmd);
  EXPECT_EQ(idn.account, "Bob");
  EXPECT_EQ(idn.ticket, "tik");
  EXPECT_EQ(idn.character, "BobDaFox");
  EXPECT_EQ(idn.cname, LIS_NAME);
  EXPECT_EQ(idn.cversion, LIS_VERSION);
}

TEST(C2S_Defaults, IDN_MethodIsAlwaysTicket) {
  const fchat::c2s::Commands cmd{fchat::c2s::IDN{
      .account = "Bob", .ticket = "tik", .character = "BobDaFox"}};
  const std::string json = fchat::c2s::serialize_command(cmd);
  EXPECT_NE(json.find(R"("method":"ticket")"), std::string::npos);
}

TEST(C2S_Defaults, RMO_ModeDefaultsToBoth) {
  const fchat::c2s::Commands cmd{fchat::c2s::RMO{.channel = "Bob"}};
  const std::string json = fchat::c2s::serialize_command(cmd);
  EXPECT_NE(json.find(R"("mode":"both")"), std::string::npos);
}

TEST(C2S_Defaults, SFC_ModeDefaultsToBoth) {
  const fchat::c2s::Commands cmd{
      fchat::c2s::SFC{.logid = 69, .report = "No", .tab = "Way"}};
  const std::string json = fchat::c2s::serialize_command(cmd);
  EXPECT_NE(json.find(R"("action":"report")"), std::string::npos);
}

#define TEST_ENUM(OP, MEMBER, ENUM_TYPE, VALUE)                             \
  TEST(C2S_Enums, OP##_##MEMBER##_##VALUE) {                                \
    fchat::c2s::OP obj{};                                                   \
    obj.MEMBER = ENUM_TYPE::VALUE;                                          \
    const fchat::c2s::Commands cmd{obj};                                    \
    const std::string json = fchat::c2s::serialize_command(cmd);            \
    const auto reparsed =                                                   \
        fchat::c2s::deserialize_command(fchat::C2SOpCode::OP, json);        \
    ASSERT_TRUE(std::holds_alternative<fchat::c2s::OP>(reparsed));          \
    EXPECT_EQ(std::get<fchat::c2s::OP>(reparsed).MEMBER, ENUM_TYPE::VALUE); \
  }

TEST_ENUM(STA, status, fchat::c2s::STA::Status, ONLINE)
TEST_ENUM(STA, status, fchat::c2s::STA::Status, LOOKING)
TEST_ENUM(STA, status, fchat::c2s::STA::Status, BUSY)
TEST_ENUM(STA, status, fchat::c2s::STA::Status, DND)
TEST_ENUM(STA, status, fchat::c2s::STA::Status, IDLE)
TEST_ENUM(STA, status, fchat::c2s::STA::Status, AWAY)
TEST_ENUM(STA, status, fchat::c2s::STA::Status, CROWN)

TEST_ENUM(TPN, status, fchat::c2s::TPN::Status, CLEAR)
TEST_ENUM(TPN, status, fchat::c2s::TPN::Status, PAUSED)
TEST_ENUM(TPN, status, fchat::c2s::TPN::Status, TYPING)

TEST_ENUM(RST, status, fchat::c2s::RST::Status, PUBLIC)
TEST_ENUM(RST, status, fchat::c2s::RST::Status, PRIVATE)

TEST_ENUM(IGN, action, fchat::c2s::IGN::Action, ADD)
TEST_ENUM(IGN, action, fchat::c2s::IGN::Action, DELETE)
TEST_ENUM(IGN, action, fchat::c2s::IGN::Action, NOTIFY)
TEST_ENUM(IGN, action, fchat::c2s::IGN::Action, LIST)

TEST_ENUM(RMO, mode, fchat::ChannelMode, CHAT)
TEST_ENUM(RMO, mode, fchat::ChannelMode, ADS)
TEST_ENUM(RMO, mode, fchat::ChannelMode, BOTH)
