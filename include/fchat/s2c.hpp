#pragma once

#include <cstdint>
#include <glaze/core/context.hpp>
#include <string_view>
#include <vector>

#include "fchat/c2s.hpp"
#include "fchat/common.hpp"
#include "fchat/error.hpp"

namespace fchat::s2c {

template <S2COpCode Op>
struct opcode_traits;

struct ADL {
  std::vector<std::string_view> ops;
};
template <>
struct opcode_traits<fchat::S2COpCode::ADL> {
  using type = ADL;
};

struct AOP {
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::AOP> {
  using type = AOP;
};

struct BRO {
  std::string_view message;
};
template <>
struct opcode_traits<fchat::S2COpCode::BRO> {
  using type = BRO;
};

struct CDS {
  std::string_view channel;
  std::string_view description;
};
template <>
struct opcode_traits<fchat::S2COpCode::CDS> {
  using type = CDS;
};

struct CHA {
  struct Channel {
    std::string_view name;
    ChannelMode mode = ChannelMode::BOTH;
    uint32_t characters{};
  };

  std::vector<Channel> channels;
};
template <>
struct opcode_traits<fchat::S2COpCode::CHA> {
  using type = CHA;
};

struct CIU {
  std::string_view sender;
  std::string_view title;
  std::string_view name;
};
template <>
struct opcode_traits<fchat::S2COpCode::CIU> {
  using type = CIU;
};

struct CBU {
  std::string_view op;
  std::string_view channel;
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::CBU> {
  using type = CBU;
};

struct CKU {
  std::string_view op;
  std::string_view channel;
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::CKU> {
  using type = CKU;
};

struct COA {
  std::string_view character;
  std::string_view channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::COA> {
  using type = COA;
};

struct COL {
  std::string_view channel;
  std::vector<std::string_view> oplist;
};
template <>
struct opcode_traits<fchat::S2COpCode::COL> {
  using type = COL;
};

struct CON {
  std::uint32_t count;
};
template <>
struct opcode_traits<fchat::S2COpCode::CON> {
  using type = CON;
};

struct COR {
  std::string_view character;
  std::string_view channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::COR> {
  using type = COR;
};

struct CSO {
  std::string_view character;
  std::string_view channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::CSO> {
  using type = CSO;
};

struct CTU {
  std::string_view op;
  std::string_view channel;
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::CTU> {
  using type = CTU;
};

struct DOP {
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::DOP> {
  using type = DOP;
};

struct ERR {
  ErrorCode number;
  std::string_view message;
};
template <>
struct opcode_traits<fchat::S2COpCode::ERR> {
  using type = ERR;
};

struct FKS {
  std::vector<std::string_view> characters;
  std::vector<std::string_view> kinks;
};
template <>
struct opcode_traits<fchat::S2COpCode::FKS> {
  using type = FKS;
};

struct FLN {
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::FLN> {
  using type = FLN;
};

struct HLO {
  std::string_view message;
};
template <>
struct opcode_traits<fchat::S2COpCode::HLO> {
  using type = HLO;
};

struct ICH {
  std::vector<std::string_view> users;
  std::string_view channel;
  ChannelMode mode;
};
template <>
struct opcode_traits<fchat::S2COpCode::ICH> {
  using type = ICH;
};

struct IDN {
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::IDN> {
  using type = IDN;
};

struct JCH {
  std::string_view channel;
  std::string_view character;
  std::string_view title;
};
template <>
struct opcode_traits<fchat::S2COpCode::JCH> {
  using type = JCH;
};

struct KID {
  enum class KidType : std::int8_t { START, CUSTOM, END };

  KidType type;
  std::string_view message;
  std::vector<int> key;
  std::vector<int> value;
};
template <>
struct opcode_traits<fchat::S2COpCode::KID> {
  using type = KID;
};

struct LCH {
  std::string_view channel;
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::LCH> {
  using type = LCH;
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
template <>
struct opcode_traits<fchat::S2COpCode::LIS> {
  using type = LIS;
};

struct NLN {
  std::string_view identity;
  std::string_view gender;
  std::string_view status;
};
template <>
struct opcode_traits<fchat::S2COpCode::NLN> {
  using type = NLN;
};

struct IGN {
  enum class Action : std::int8_t { INIT, ADD, DELETE, NOTIFY };

  Action action{};
  std::vector<std::string_view> characters;
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::IGN> {
  using type = IGN;
};

struct FRL {
  std::vector<std::string_view> characters;
};
template <>
struct opcode_traits<fchat::S2COpCode::FRL> {
  using type = FRL;
};

struct ORS {
  struct ChannelInfo {
    std::string_view name;
    int characters{};
    std::string_view title;
  };

  std::vector<ChannelInfo> channels;
};
template <>
struct opcode_traits<fchat::S2COpCode::ORS> {
  using type = ORS;
};

struct PIN {};
template <>
struct opcode_traits<fchat::S2COpCode::PIN> {
  using type = PIN;
};

struct PRD {
  enum class ProfileType : std::int8_t { START, INFO, SELECT, END };

  ProfileType type{};
  std::string_view message;
  std::string_view key;
  std::string_view value;
};
template <>
struct opcode_traits<fchat::S2COpCode::PRD> {
  using type = PRD;
};

struct PRI {
  std::string_view character;
  std::string_view message;
};
template <>
struct opcode_traits<fchat::S2COpCode::PRI> {
  using type = PRI;
};

struct MSG {
  std::string_view character;
  std::string_view message;
  std::string_view channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::MSG> {
  using type = MSG;
};

struct LRP {
  std::string_view character;
  std::string_view message;
  std::string_view channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::LRP> {
  using type = LRP;
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
template <>
struct opcode_traits<fchat::S2COpCode::RLL> {
  using type = RLL;
};

struct RMO {
  ChannelMode mode;
  std::string_view channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::RMO> {
  using type = RMO;
};

struct RTB {
  std::string_view type;
  std::string_view character;
};
template <>
struct opcode_traits<fchat::S2COpCode::RTB> {
  using type = RTB;
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
template <>
struct opcode_traits<fchat::S2COpCode::SFC> {
  using type = SFC;
};

struct STA {
  std::string_view status;
  std::string_view character;
  std::string_view statusmsg;
};
template <>
struct opcode_traits<fchat::S2COpCode::STA> {
  using type = STA;
};

struct SYS {
  std::string_view message;
  std::optional<std::string_view> channel;
};
template <>
struct opcode_traits<fchat::S2COpCode::SYS> {
  using type = SYS;
};

using fchat::c2s::TPN;
template <>
struct opcode_traits<fchat::S2COpCode::TPN> {
  using type = TPN;
};

struct UPT {
  std::time_t time{};
  std::time_t starttime{};
  std::string_view startstring;
  int accepted{};
  int channels{};
  int users{};
  int maxusers{};
};
template <>
struct opcode_traits<fchat::S2COpCode::UPT> {
  using type = UPT;
};

struct VAR {
  std::string variable;
  std::variant<int, double, std::vector<std::string>> value;
};
template <>
struct opcode_traits<fchat::S2COpCode::VAR> {
  using type = VAR;
};

using Commands =
    std::variant<ADL, AOP, BRO, CBU, CDS, CHA, CIU, CKU, COA, COL, CON, COR,
                 CSO, CTU, DOP, ERR, FKS, FLN, HLO, FRL, ICH, IDN, IGN, JCH,
                 KID, LCH, LIS, LRP, MSG, NLN, ORS, PIN, PRD, PRI, RLL, RMO,
                 RTB, SFC, STA, SYS, TPN, UPT, VAR>;

template <S2COpCode Op>
auto parse_payload(std::string_view json) -> Commands {
  typename opcode_traits<Op>::type payload{};

  if (auto err = glz::read_json(payload, json)) {
    throw fchat::parse_error{err, json};
  }

  return payload;
}
auto serialize_command(const Commands& cmd) -> std::string;

auto deserialize_command(S2COpCode opcode, std::string_view json) -> Commands;

}  // namespace fchat::s2c
