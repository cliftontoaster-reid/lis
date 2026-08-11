#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "fchat/c2s.hpp"
#include "fchat/common.hpp"

namespace fchat {

namespace s2c {

struct ADL {
  std::vector<std::string_view> ops;
};

struct AOP {
  std::string_view character;
};

struct BRO {
  std::string_view message;
};

struct CDS {
  std::string_view channel;
  std::string_view description;
};

struct CHA {
  struct Channel {
    std::string_view name;
    ChannelMode mode;
    uint32_t characters;
  };

  std::vector<Channel> channels;
};

struct CIU {
  std::string_view sender;
  std::string_view title;
  std::string_view name;
};

struct CBU {
  std::string_view op;
  std::string_view channel;
  std::string_view character;
};

struct CKU {
  std::string_view op;
  std::string_view channel;
  std::string_view character;
};

struct COA {
  std::string_view character;
  std::string_view channel;
};

struct COL {
  std::string_view channel;
  std::vector<std::string_view> oplist;
};

struct CON {
  std::uint32_t count;
};

struct COR {
  std::string_view character;
  std::string_view channel;
};

struct CSO {
  std::string_view character;
  std::string_view channel;
};

struct CTU {
  std::string_view op;
  std::string_view channel;
  std::string_view character;
};

struct DOP {
  std::string_view character;
};

struct ERR {
  ErrorCode number;
  std::string_view message;
};

struct FKS {
  std::vector<std::string_view> characters;
  std::vector<std::string_view> kinks;
};

struct FLN {
  std::string_view character;
};

struct HLO {
  std::string_view message;
};

struct ICH {
  std::vector<std::string_view> users;
  std::string_view channel;
  ChannelMode mode;
};

struct IDN {
  std::string_view character;
};

struct JCH {
  std::string_view channel;
  std::string_view character;
  std::string_view title;
};

struct KID {
  enum class KidType { START, CUSTOM, END };

  KidType type;
  std::string_view message;
  std::vector<int> key;
  std::vector<int> value;
};

struct LCH {
  std::string_view channel;
  std::string_view character;
};

struct LIS {
  struct CharacterInfo {
    std::string_view name;
    std::string_view gender;
    std::string_view status;
    std::string_view status_message;
  };

  std::vector<CharacterInfo> characters;
};

struct NLN {
  std::string_view identity;
  std::string_view gender;
  std::string_view status;
};

struct IGN {
  enum class Action { INIT, ADD, DELETE, NOTIFY };

  Action action{};
  std::vector<std::string_view> characters;
  std::string_view character;
};

struct FRL {
  std::vector<std::string_view> characters;
};

struct ORS {
  struct ChannelInfo {
    std::string_view name;
    int characters{};
    std::string_view title;
  };

  std::vector<ChannelInfo> channels;
};

struct PIN {};

struct PRD {
  enum class ProfileType { START, INFO, SELECT, END };

  ProfileType type{};
  std::string_view message;
  std::string_view key;
  std::string_view value;
};

struct PRI {
  std::string_view character;
  std::string_view message;
};

struct MSG {
  std::string_view character;
  std::string_view message;
  std::string_view channel;
};

struct LRP {
  std::string_view character;
  std::string_view message;
  std::string_view channel;
};

struct RllDice {
  std::string_view channel;
  std::string_view character;
  std::string_view message;
  std::string_view type{"dice"};
  std::vector<int> results;
  std::vector<std::string> rolls;
  int endresult{};
};

struct RllBottle {
  std::string_view channel;
  std::string_view character;
  std::string_view message;
  std::string_view type{"bottle"};
  std::string_view target;
};

using RLL = std::variant<RllDice, RllBottle>;

struct RMO {
  ChannelMode mode;
  std::string_view channel;
};

struct RTB {
  std::string_view type;
  std::string_view character;
};

struct SfcConfirm {
  std::string_view action{"confirm"};
  std::string_view moderator;
  std::string_view character;
  std::string_view timestamp;
};

struct SfcReport {
  std::string_view action{"report"};
  int callid{};
  std::string_view report;
  std::string_view timestamp;
  std::string_view character;
  int logid{};
};

using SFC = std::variant<SfcConfirm, SfcReport>;

struct STA {
  std::string_view status;
  std::string_view character;
  std::string_view statusmsg;
};

struct SYS {
  std::string_view message;
  std::optional<std::string_view> channel;
};

using fchat::c2s::TPN;

struct UPT {
  std::time_t time{};
  std::time_t starttime{};
  std::string_view startstring;
  int accepted{};
  int channels{};
  int users{};
  int maxusers{};
};

struct VAR {
  std::string variable;
  std::variant<int, double, std::vector<std::string>> value;
};

}  // namespace s2c

}  // namespace fchat