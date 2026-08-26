
#include <gtest/gtest.h>

#include "fchat/s2c.hpp"

#define TEST_S2C(OPCODE, JSON, CHECK)                                          \
  TEST(S2C_Deserialize, Deserialize_##OPCODE##_Returns_##OPCODE) {             \
    const std::string json = JSON;                                             \
    const auto cmd =                                                           \
        fchat::s2c::deserialize_command(fchat::S2COpCode::OPCODE, json);       \
    ASSERT_TRUE(std::holds_alternative<fchat::s2c::OPCODE>(cmd));              \
    CHECK(std::get<fchat::s2c::OPCODE>(cmd));                                  \
  }                                                                            \
  TEST(S2C_Serialize, Serialize_##OPCODE##_RoundTrips) {                       \
    const auto cmd =                                                           \
        fchat::s2c::deserialize_command(fchat::S2COpCode::OPCODE, JSON);       \
    const std::string json = fchat::s2c::serialize_command(cmd);               \
    const auto reparsed =                                                      \
        fchat::s2c::deserialize_command(fchat::S2COpCode::OPCODE, json);       \
    ASSERT_TRUE(std::holds_alternative<fchat::s2c::OPCODE>(reparsed));         \
    CHECK(std::get<fchat::s2c::OPCODE>(reparsed));                             \
  }                                                                            \
  TEST(S2C_Errors, Deserialize_##OPCODE##_MalformedThrows) {                   \
    EXPECT_THROW(                                                              \
        fchat::s2c::deserialize_command(fchat::S2COpCode::OPCODE, "not json"), \
        fchat::parse_error);                                                   \
  }

TEST_S2C(ADL, R"({"ops":["bob", "charlie"]})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.ops.size(), 2);
           EXPECT_EQ(cmd.ops[0], "bob");
           EXPECT_EQ(cmd.ops[1], "charlie");
         })

TEST_S2C(AOP, R"({"character":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Bob"); })

TEST_S2C(BRO, R"({"message":"Bob"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.message, "Bob"); })

TEST_S2C(CDS, R"({"channel":"Bob", "description": "Charlie"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.description, "Charlie");
         })

TEST_S2C(CHA, R"({
  "channels": [
    {
      "name": "Bob",
      "mode": "both",
      "characters": 69
    },
    {
      "name": "TV",
      "mode": "ads",
      "characters": 0
    }
  ]
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channels.size(), 2);
           EXPECT_EQ(cmd.channels[0].name, "Bob");
           EXPECT_EQ(cmd.channels[0].mode, fchat::ChannelMode::BOTH);
           EXPECT_EQ(cmd.channels[0].characters, 69);

           EXPECT_EQ(cmd.channels[1].name, "TV");
           EXPECT_EQ(cmd.channels[1].mode, fchat::ChannelMode::ADS);
           EXPECT_EQ(cmd.channels[1].characters, 0);
         })

TEST_S2C(CIU, R"({"sender":"Bob", "title": "Charlie", "name": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.sender, "Bob");
           EXPECT_EQ(cmd.title, "Charlie");
           EXPECT_EQ(cmd.name, "Francis");
         })

TEST_S2C(
    CBU,
    R"({"operator":"Bob", "channel": "Charlie", "character": "Francis"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.op, "Bob");
      EXPECT_EQ(cmd.channel, "Charlie");
      EXPECT_EQ(cmd.character, "Francis");
    })

TEST_S2C(
    CKU,
    R"({"operator":"Bob", "channel": "Charlie", "character": "Francis"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.op, "Bob");
      EXPECT_EQ(cmd.channel, "Charlie");
      EXPECT_EQ(cmd.character, "Francis");
    })

TEST_S2C(COA,
         R"({"channel": "Charlie", "character": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Charlie");
           EXPECT_EQ(cmd.character, "Francis");
         })

TEST_S2C(COL,
         R"({
  "channel": "Bob",
  "oplist": [
    "Charlie",
    "Francis"
  ]
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.oplist.size(), 2);
           EXPECT_EQ(cmd.oplist[0], "Charlie");
           EXPECT_EQ(cmd.oplist[1], "Francis");
         })

TEST_S2C(CON,
         R"({"count": 69})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.count, 69); })

TEST_S2C(COR,
         R"({"channel": "Charlie", "character": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Charlie");
           EXPECT_EQ(cmd.character, "Francis");
         })

TEST_S2C(CSO,
         R"({"channel": "Charlie", "character": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Charlie");
           EXPECT_EQ(cmd.character, "Francis");
         })

TEST_S2C(
    CTU,
    R"({"channel": "Charlie", "operator": "Bob", "character": "Francis"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.channel, "Charlie");
      EXPECT_EQ(cmd.character, "Francis");
      EXPECT_EQ(cmd.op, "Bob");
    })

TEST_S2C(DOP,
         R"({"character": "Francis"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Francis"); })

TEST_S2C(ERR,
         R"({"number": 10, "message": "Francis"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.message, "Francis"); })

TEST_S2C(FKS,
         R"({
  "characters": [
    "Bob",
    "Charlie"
  ],
  "kinks": [
    "Door",
    "Chair"
  ]
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.characters.size(), 2);
           EXPECT_EQ(cmd.characters[0], "Bob");
           EXPECT_EQ(cmd.characters[1], "Charlie");

           EXPECT_EQ(cmd.kinks.size(), 2);
           EXPECT_EQ(cmd.kinks[0], "Door");
           EXPECT_EQ(cmd.kinks[1], "Chair");
         })

TEST_S2C(HLO,
         R"({"message": "Francis"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.message, "Francis"); })

TEST_S2C(ICH,
         R"({
  "users": [
    {"identity": "Bob"},
    {"identity": "Charlie"}
  ],
  "channel": "Bob",
  "mode": "both"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.users.size(), 2);
           EXPECT_EQ(cmd.users[0], "Bob");
           EXPECT_EQ(cmd.users[1], "Charlie");

           EXPECT_EQ(cmd.channel, "Bob");
           EXPECT_EQ(cmd.mode, fchat::ChannelMode::BOTH);
         })

TEST_S2C(IDN,
         R"({"character": "Francis"})",  //
         [](const auto& cmd) -> auto { EXPECT_EQ(cmd.character, "Francis"); })

TEST_S2C(
    JCH,
    R"({"channel": "Charlie", "title": "Bob", "character": {"identity": "Francis"}})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.channel, "Charlie");
      EXPECT_EQ(cmd.title, "Bob");
      EXPECT_EQ(cmd.character, "Francis");
    })

TEST_S2C(KID,
         R"({
  "type": "custom",
  "message": "Bob",
  "key": [0, 1],
  "value": [0, 1]
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.type, fchat::s2c::KID::KidType::CUSTOM);
           EXPECT_EQ(cmd.message, "Bob");

           EXPECT_EQ(cmd.key.size(), 2);
           EXPECT_EQ(cmd.key[0], 0);
           EXPECT_EQ(cmd.key[1], 1);

           EXPECT_EQ(cmd.value.size(), 2);
           EXPECT_EQ(cmd.value[0], 0);
           EXPECT_EQ(cmd.value[1], 1);
         })

TEST_S2C(LCH,
         R"({"channel": "Charlie", "character": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.channel, "Charlie");
           EXPECT_EQ(cmd.character, "Francis");
         })

TEST_S2C(LIS,
         R"({
  "characters": [
    [
      "Alice",
      "Female",
      "online",
      "Coding C++23"
    ],
    [
      "Bob",
      "Male",
      "away",
      "BRB, getting coffee"
    ]
  ]
})",  //
         [](const auto& cmd) -> auto {
           ASSERT_EQ(cmd.characters.size(), 2);

           EXPECT_EQ(cmd.characters[0].name, "Alice");
           EXPECT_EQ(cmd.characters[0].gender, "Female");
           EXPECT_EQ(cmd.characters[0].status, "online");
           EXPECT_EQ(cmd.characters[0].status_message, "Coding C++23");

           EXPECT_EQ(cmd.characters[1].name, "Bob");
           EXPECT_EQ(cmd.characters[1].gender, "Male");
           EXPECT_EQ(cmd.characters[1].status, "away");
           EXPECT_EQ(cmd.characters[1].status_message, "BRB, getting coffee");
         })

TEST_S2C(NLN,
         R"({"identity": "Charlie", "gender": "Bob", "status": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.identity, "Charlie");
           EXPECT_EQ(cmd.gender, "Bob");
           EXPECT_EQ(cmd.status, "Francis");
         })

TEST_S2C(
    IGN,
    R"({"action": "add", "characters": ["Bob"], "character": "Francis"})",  //
    [](const auto& cmd) -> auto {
      EXPECT_EQ(cmd.action, fchat::s2c::IGN::Action::ADD);
      EXPECT_EQ(cmd.characters.size(), 1);
      EXPECT_EQ(cmd.characters[0], "Bob");
      EXPECT_EQ(cmd.character, "Francis");
    })

TEST_S2C(FRL,
         R"({
  "characters": [
    "Charlie",
    "Francis"
  ]
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.characters.size(), 2);
           EXPECT_EQ(cmd.characters[0], "Charlie");
           EXPECT_EQ(cmd.characters[1], "Francis");
         })

TEST_S2C(ORS,
         R"({
  "channels": [
    {
      "name": "General",
      "characters": 150,
      "title": "Welcome to the main channel!"
    },
    {
      "name": "Development",
      "characters": 42,
      "title": "C++23 discussion & debugging"
    }
  ]
})",  //
         [](const auto& cmd) -> auto {
           ASSERT_EQ(cmd.channels.size(), 2);

           EXPECT_EQ(cmd.channels[0].name, "General");
           EXPECT_EQ(cmd.channels[0].characters, 150);
           EXPECT_EQ(cmd.channels[0].title, "Welcome to the main channel!");

           EXPECT_EQ(cmd.channels[1].name, "Development");
           EXPECT_EQ(cmd.channels[1].characters, 42);
           EXPECT_EQ(cmd.channels[1].title, "C++23 discussion & debugging");
         })

TEST_S2C(PIN,
         R"({})",  //
         [](const auto& cmd) -> auto {})

TEST_S2C(PRD,
         R"({
  "type": "info",
  "message": "User profile loaded",
  "key": "user_id",
  "value": "12345"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.type, fchat::s2c::PRD::ProfileType::INFO);
           EXPECT_EQ(cmd.message, "User profile loaded");
           EXPECT_EQ(cmd.key, "user_id");
           EXPECT_EQ(cmd.value, "12345");
         })

TEST_S2C(PRI,
         R"({
  "character": "INFO",
  "message": "User profile loaded"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.character, "INFO");
           EXPECT_EQ(cmd.message, "User profile loaded");
         })

TEST_S2C(MSG,
         R"({
  "character": "INFO",
  "message": "User profile loaded",
  "channel": "Bob"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.character, "INFO");
           EXPECT_EQ(cmd.message, "User profile loaded");
           EXPECT_EQ(cmd.channel, "Bob");
         })

TEST_S2C(LRP,
         R"({
  "character": "INFO",
  "message": "User profile loaded",
  "channel": "Bob"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.character, "INFO");
           EXPECT_EQ(cmd.message, "User profile loaded");
           EXPECT_EQ(cmd.channel, "Bob");
         })

TEST(S2C_Deserialize, Deserialize_RLL_ReturnsRllDice) {
  const std::string json = R"({
    "type":"dice","channel":"#c","character":"Bob","message":"hi",
    "results":[1,2],"rolls":["1d6"],"endresult":3})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, json);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::RLL>(cmd));
  const auto& rll = std::get<fchat::s2c::RLL>(cmd);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::RllDice>(rll));
  const auto& d = std::get<fchat::s2c::RllDice>(rll);
  EXPECT_EQ(d.channel, "#c");
  EXPECT_EQ(d.character, "Bob");
  EXPECT_EQ(d.message, "hi");
  EXPECT_EQ(d.type, "dice");
  ASSERT_EQ(d.results.size(), 2);
  EXPECT_EQ(d.results[0], 1);
  EXPECT_EQ(d.rolls[0], "1d6");
  EXPECT_EQ(d.endresult, 3);
}

TEST(S2C_Deserialize, Deserialize_RLL_ReturnsRllBottle) {
  const std::string json = R"({
    "type":"bottle","channel":"#c","character":"Bob","message":"spin",
    "target":"Charlie"})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, json);
  const auto& rll = std::get<fchat::s2c::RLL>(cmd);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::RllBottle>(rll));
  const auto& b = std::get<fchat::s2c::RllBottle>(rll);
  EXPECT_EQ(b.channel, "#c");
  EXPECT_EQ(b.target, "Charlie");
  EXPECT_EQ(b.type, "bottle");
}

TEST(S2C_Serialize, Serialize_RLL_RllDiceRoundTrips) {
  const std::string json =
      R"({"type":"dice","channel":"#c","character":"Bob","message":"hi","results":[1,2],"rolls":["1d6"],"endresult":3})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, json);
  const std::string out = fchat::s2c::serialize_command(cmd);
  EXPECT_NE(out.find(R"("type":"dice")"), std::string::npos);
  const auto reparsed =
      fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, out);
  const auto& rll = std::get<fchat::s2c::RLL>(reparsed);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::RllDice>(rll));
  EXPECT_EQ(std::get<fchat::s2c::RllDice>(rll).endresult, 3);
}

TEST(S2C_Serialize, Serialize_RLL_RllBottleRoundTrips) {
  const std::string json =
      R"({"type":"bottle","channel":"#c","character":"Bob","message":"spin","target":"Charlie"})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, json);
  const std::string out = fchat::s2c::serialize_command(cmd);
  EXPECT_NE(out.find(R"("type":"bottle")"), std::string::npos);
  const auto reparsed =
      fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, out);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::RllBottle>(
      std::get<fchat::s2c::RLL>(reparsed)));
}

TEST(S2C_Errors, Deserialize_RLL_MalformedThrows) {
  EXPECT_THROW(
      fchat::s2c::deserialize_command(fchat::S2COpCode::RLL, "not json"),
      fchat::parse_error);
}
TEST(S2C_Errors, Deserialize_RLL_UnknownTypeThrows) {
  EXPECT_THROW(fchat::s2c::deserialize_command(fchat::S2COpCode::RLL,
                                               R"({"type":"bogus"})"),
               fchat::parse_error);
}

TEST_S2C(RMO,
         R"({"mode": "both", "channel": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.mode, fchat::ChannelMode::BOTH);
           EXPECT_EQ(cmd.channel, "Francis");
         })

TEST_S2C(RTB,
         R"({"type": "both", "character": "Francis"})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.type, "both");
           EXPECT_EQ(cmd.character, "Francis");
         })

TEST(S2C_Deserialize, Deserialize_SFC_ReturnsSfcConfirm) {
  const std::string json = R"({
    "action":"confirm","moderator":"Bob","character":"Charlie","timestamp":"123"})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, json);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::SFC>(cmd));
  const auto& sfc = std::get<fchat::s2c::SFC>(cmd);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::SfcConfirm>(sfc));
  const auto& c = std::get<fchat::s2c::SfcConfirm>(sfc);
  EXPECT_EQ(c.action, "confirm");
  EXPECT_EQ(c.moderator, "Bob");
  EXPECT_EQ(c.character, "Charlie");
  EXPECT_EQ(c.timestamp, "123");
}

TEST(S2C_Deserialize, Deserialize_SFC_ReturnsSfcReport) {
  const std::string json = R"({
    "action":"report","callid":7,"report":"spam","timestamp":"456","character":"Charlie","logid":9})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, json);
  const auto& sfc = std::get<fchat::s2c::SFC>(cmd);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::SfcReport>(sfc));
  const auto& r = std::get<fchat::s2c::SfcReport>(sfc);
  EXPECT_EQ(r.action, "report");
  EXPECT_EQ(r.callid, 7);
  EXPECT_EQ(r.report, "spam");
  EXPECT_EQ(r.logid, 9);
}

TEST(S2C_Serialize, Serialize_SFC_ConfirmRoundTrips) {
  const std::string json =
      R"({"action":"confirm","moderator":"Bob","character":"Charlie","timestamp":"123"})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, json);
  const std::string out = fchat::s2c::serialize_command(cmd);
  EXPECT_NE(out.find(R"("action":"confirm")"), std::string::npos);
  const auto reparsed =
      fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, out);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::SfcConfirm>(
      std::get<fchat::s2c::SFC>(reparsed)));
}

TEST(S2C_Serialize, Serialize_SFC_ReportRoundTrips) {
  const std::string json =
      R"({"action":"report","callid":7,"report":"spam","timestamp":"456","character":"Charlie","logid":9})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, json);
  const std::string out = fchat::s2c::serialize_command(cmd);
  EXPECT_NE(out.find(R"("action":"report")"), std::string::npos);
  const auto reparsed =
      fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, out);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::SfcReport>(
      std::get<fchat::s2c::SFC>(reparsed)));
}

TEST(S2C_Defaults, SfcConfirm_ActionDefaultsToConfirm) {
  fchat::s2c::SfcConfirm c{};
  EXPECT_EQ(c.action, "confirm");
  const std::string out =
      fchat::s2c::serialize_command(fchat::s2c::Commands{fchat::s2c::SFC{c}});
  EXPECT_NE(out.find(R"("action":"confirm")"), std::string::npos);
}
TEST(S2C_Defaults, SfcReport_ActionDefaultsToReport) {
  fchat::s2c::SfcReport r{};
  EXPECT_EQ(r.action, "report");
  const std::string out =
      fchat::s2c::serialize_command(fchat::s2c::Commands{fchat::s2c::SFC{r}});
  EXPECT_NE(out.find(R"("action":"report")"), std::string::npos);
}

TEST(S2C_Errors, Deserialize_SFC_MalformedThrows) {
  EXPECT_THROW(
      fchat::s2c::deserialize_command(fchat::S2COpCode::SFC, "not json"),
      fchat::parse_error);
}
TEST(S2C_Errors, Deserialize_SFC_UnknownActionThrows) {
  EXPECT_THROW(fchat::s2c::deserialize_command(fchat::S2COpCode::SFC,
                                               R"({"action":"bogus"})"),
               fchat::parse_error);
}

TEST_S2C(STA,
         R"({
  "status": "INFO",
  "character": "User profile loaded",
  "statusmsg": "Bob"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.status, "INFO");
           EXPECT_EQ(cmd.character, "User profile loaded");
           EXPECT_EQ(cmd.statusmsg, "Bob");
         })

TEST_S2C(SYS,
         R"({
  "message": "INFO",
  "channel": "Bob"
})",  //
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.message, "INFO");
           EXPECT_EQ(cmd.channel, "Bob");
         })

TEST_S2C(UPT,
         R"({
           "time": 1000,
           "starttime": 500,
           "startstring": "2020-01-01",
           "accepted": 1,
           "channels": 2,
           "users": 3,
           "maxusers": 4
         })",
         [](const auto& cmd) -> auto {
           EXPECT_EQ(cmd.time, 1000);
           EXPECT_EQ(cmd.starttime, 500);
           EXPECT_EQ(cmd.startstring, "2020-01-01");
           EXPECT_EQ(cmd.accepted, 1);
           EXPECT_EQ(cmd.channels, 2);
           EXPECT_EQ(cmd.users, 3);
           EXPECT_EQ(cmd.maxusers, 4);
         })

TEST_S2C(VAR, R"({"variable":"x","value":5})", [](const auto& cmd) -> auto {
  EXPECT_EQ(cmd.variable, "x");
  ASSERT_TRUE(std::holds_alternative<int>(cmd.value));
  EXPECT_EQ(std::get<int>(cmd.value), 5);
})

TEST(S2C_Deserialize, Deserialize_VAR_DoubleValue) {
  const std::string json = R"({"variable":"x","value":3.14})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::VAR, json);
  ASSERT_TRUE(std::holds_alternative<fchat::s2c::VAR>(cmd));
  const auto& v = std::get<fchat::s2c::VAR>(cmd);
  EXPECT_EQ(v.variable, "x");
  ASSERT_TRUE(std::holds_alternative<double>(v.value));
  EXPECT_DOUBLE_EQ(std::get<double>(v.value), 3.14);
}

TEST(S2C_Deserialize, Deserialize_VAR_VectorValue) {
  const std::string json = R"({"variable":"x","value":["a","b"]})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::VAR, json);
  const auto& v = std::get<fchat::s2c::VAR>(cmd);
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(v.value));
  EXPECT_EQ(std::get<std::vector<std::string>>(v.value).size(), 2);
}

TEST(S2C_Serialize, Serialize_VAR_DoubleRoundTrips) {
  const std::string json = R"({"variable":"x","value":3.14})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::VAR, json);
  const std::string out = fchat::s2c::serialize_command(cmd);
  const auto reparsed =
      fchat::s2c::deserialize_command(fchat::S2COpCode::VAR, out);
  ASSERT_TRUE(std::holds_alternative<double>(
      std::get<fchat::s2c::VAR>(reparsed).value));
}

TEST(S2C_Serialize, Serialize_VAR_VectorRoundTrips) {
  const std::string json = R"({"variable":"x","value":["a","b"]})";
  const auto cmd = fchat::s2c::deserialize_command(fchat::S2COpCode::VAR, json);
  const std::string out = fchat::s2c::serialize_command(cmd);
  const auto reparsed =
      fchat::s2c::deserialize_command(fchat::S2COpCode::VAR, out);
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(
      std::get<fchat::s2c::VAR>(reparsed).value));
}