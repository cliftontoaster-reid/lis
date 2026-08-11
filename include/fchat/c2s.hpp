#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "fchat/common.hpp"

namespace fchat {

namespace c2s {

struct ACB {
  std::string character;
};

struct AOP {
  std::string character;
};

struct AWC {
  std::string character;
};

struct BRO {
  std::string message;
};

struct CBL {
  std::string channel;
};

struct CBU {
  std::string character;
  std::string channel;
};

struct CCR {
  std::string channel;
};

struct CDS {
  std::string channel;
  std::string description;
};

struct CHA {};

struct CIU {
  std::string channel;
  std::string character;
};

struct CKU {
  std::string channel;
  std::string character;
};

struct COA {
  std::string channel;
  std::string character;
};

struct COL {
  std::string character;
};

struct COR {
  std::string channel;
  std::string character;
};

struct CRC {
  std::string channel;
};

struct CSO {
  std::string character;
  std::string channel;
};

struct CTU {
  std::string channel;
  std::string character;
  std::uint8_t length;
};

struct CUB {
  std::string channel;
  std::string character;
};

struct DOP {
  std::string character;
};

struct FKS {
  std::vector<uint16_t> kinks;
  std::vector<std::string> genders;
  std::vector<std::string> orientations;
  std::vector<std::string> languages;
  std::vector<std::string> furryprefs;
  std::vector<std::string> roles;
};

struct IDN {
  std::string account;
  std::string ticket;
  std::string character;
  std::string cname = LIS_NAME;
  std::string cversion = LIS_VERSION;
};

struct IGN {
  enum class Action {
    ADD,
    DELETE,
    NOTIFY,
    LIST,
  };

  Action action{Action::LIST};
  std::optional<std::string> character;
};

struct JCH {
  std::string channel;
};

struct KIC {
  std::string channel;
};

struct KIK {
  std::string character;
};

struct KIN {
  std::string character;
};

struct LCH {
  std::string channel;
};

struct LRP {
  std::string channel;
  std::string message;
};

struct MSG {
  std::string channel;
  std::string message;
};

struct ORS {};
struct PIN {};

struct PRI {
  std::string recipient;
  std::string message;
};

struct PRO {
  std::string character;
};

struct RLL {
  struct Dice {
    std::int16_t mul = 1;
    std::int16_t sides;
    std::int16_t add = 0;
  };

  struct DiceExpr {
    std::variant<std::vector<std::variant<Dice, uint16_t>>, std::string> value;
  };

  DiceExpr dice;
  std::string channel;
};

struct RLD {
  std::string save;
};

struct RMO {
  std::string channel;
  ChannelMode mode{ChannelMode::BOTH};
};

struct RST {
  enum class Status {
    PUBLIC,
    PRIVATE,
  };

  std::string channel;
  Status status;
};

struct RWD {
  std::string character;
};

struct SFC {
  std::string action = "report";
  uint32_t logid{0};
  std::string report;
  std::string tab;
};

struct STA {
  enum class Status { ONLINE, LOOKING, BUSY, DND, IDLE, AWAY, CROWN };

  Status status;
  std::string statusmsg;
};

struct TMO {
  std::string character;
  uint8_t time;
  std::string reason;
};

struct TPN {
  enum class Status { CLEAR, PAUSED, TYPING };

  std::string character;
  Status status;
};

struct UNB {
  std::string character;
};

struct UPT {};

}  // namespace c2s

}  // namespace fchat