#pragma once

#include <cstdint>
#include <glaze/core/context.hpp>
#include <glaze/core/opts.hpp>
#include <glaze/core/read.hpp>
#include <glaze/json/read.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "fchat/common.hpp"
#include "fchat/error.hpp"

namespace fchat::c2s {

template <C2SOpCode Op>
struct opcode_traits;

struct ACB {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::ACB> {
  using type = ACB;
};

struct AOP {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::AOP> {
  using type = AOP;
};

struct AWC {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::AWC> {
  using type = AWC;
};

struct BRO {
  std::string message;
};
template <>
struct opcode_traits<fchat::C2SOpCode::BRO> {
  using type = BRO;
};

struct CBL {
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CBL> {
  using type = CBL;
};

struct CBU {
  std::string character;
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CBU> {
  using type = CBU;
};

struct CCR {
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CCR> {
  using type = CCR;
};

struct CDS {
  std::string channel;
  std::string description;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CDS> {
  using type = CDS;
};

struct CHA {};
template <>
struct opcode_traits<fchat::C2SOpCode::CHA> {
  using type = CHA;
};

struct CIU {
  std::string channel;
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CIU> {
  using type = CIU;
};

struct CKU {
  std::string channel;
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CKU> {
  using type = CKU;
};

struct COA {
  std::string channel;
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::COA> {
  using type = COA;
};

struct COL {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::COL> {
  using type = COL;
};

struct COR {
  std::string channel;
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::COR> {
  using type = COR;
};

struct CRC {
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CRC> {
  using type = CRC;
};

struct CSO {
  std::string character;
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CSO> {
  using type = CSO;
};

struct CTU {
  std::string channel;
  std::string character;
  std::uint8_t length;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CTU> {
  using type = CTU;
};

struct CUB {
  std::string channel;
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::CUB> {
  using type = CUB;
};

struct DOP {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::DOP> {
  using type = DOP;
};

struct FKS {
  std::vector<uint16_t> kinks;
  std::vector<std::string> genders;
  std::vector<std::string> orientations;
  std::vector<std::string> languages;
  std::vector<std::string> furryprefs;
  std::vector<std::string> roles;
};
template <>
struct opcode_traits<fchat::C2SOpCode::FKS> {
  using type = FKS;
};

struct IDN {
  std::string account;
  std::string ticket;
  std::string character;
  std::string cname = LIS_NAME;
  std::string cversion = LIS_VERSION;
};
template <>
struct opcode_traits<fchat::C2SOpCode::IDN> {
  using type = IDN;
};

struct IGN {
  enum class Action : std::int8_t {
    ADD,
    DELETE,
    NOTIFY,
    LIST,
  };

  Action action{Action::LIST};
  std::optional<std::string> character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::IGN> {
  using type = IGN;
};

struct JCH {
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::JCH> {
  using type = JCH;
};

struct KIC {
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::KIC> {
  using type = KIC;
};

struct KIK {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::KIK> {
  using type = KIK;
};

struct KIN {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::KIN> {
  using type = KIN;
};

struct LCH {
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::LCH> {
  using type = LCH;
};

struct LRP {
  std::string channel;
  std::string message;
};
template <>
struct opcode_traits<fchat::C2SOpCode::LRP> {
  using type = LRP;
};

struct MSG {
  std::string channel;
  std::string message;
};
template <>
struct opcode_traits<fchat::C2SOpCode::MSG> {
  using type = MSG;
};

struct ORS {};
template <>
struct opcode_traits<fchat::C2SOpCode::ORS> {
  using type = ORS;
};

struct PIN {};
template <>
struct opcode_traits<fchat::C2SOpCode::PIN> {
  using type = PIN;
};

struct PRI {
  std::string recipient;
  std::string message;
};
template <>
struct opcode_traits<fchat::C2SOpCode::PRI> {
  using type = PRI;
};

struct PRO {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::PRO> {
  using type = PRO;
};

struct RLL {
  struct Dice {
    std::int16_t mul = 1;
    std::int16_t sides{};
    std::int16_t add = 0;
  };

  struct DiceExpr {
    std::variant<std::vector<std::variant<Dice, uint16_t>>, std::string> value;
  };

  DiceExpr dice;
  std::string channel;
};
template <>
struct opcode_traits<fchat::C2SOpCode::RLL> {
  using type = RLL;
};

struct RLD {
  std::string save;
};
template <>
struct opcode_traits<fchat::C2SOpCode::RLD> {
  using type = RLD;
};

struct RMO {
  std::string channel;
  ChannelMode mode{ChannelMode::BOTH};
};
template <>
struct opcode_traits<fchat::C2SOpCode::RMO> {
  using type = RMO;
};

struct RST {
  enum class Status : std::int8_t {
    PUBLIC,
    PRIVATE,
  };

  std::string channel;
  Status status;
};
template <>
struct opcode_traits<fchat::C2SOpCode::RST> {
  using type = RST;
};

struct RWD {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::RWD> {
  using type = RWD;
};

struct SFC {
  std::string action = "report";
  uint32_t logid{0};
  std::string report;
  std::string tab;
};
template <>
struct opcode_traits<fchat::C2SOpCode::SFC> {
  using type = SFC;
};

struct STA {
  enum class Status : std::int8_t {
    ONLINE,
    LOOKING,
    BUSY,
    DND,
    IDLE,
    AWAY,
    CROWN
  };

  Status status;
  std::string statusmsg;
};
template <>
struct opcode_traits<fchat::C2SOpCode::STA> {
  using type = STA;
};

struct TMO {
  std::string character;
  uint8_t time;
  std::string reason;
};
template <>
struct opcode_traits<fchat::C2SOpCode::TMO> {
  using type = TMO;
};

struct TPN {
  enum class Status : std::int8_t { CLEAR, PAUSED, TYPING };

  std::string character;
  Status status;
};
template <>
struct opcode_traits<fchat::C2SOpCode::TPN> {
  using type = TPN;
};

struct UNB {
  std::string character;
};
template <>
struct opcode_traits<fchat::C2SOpCode::UNB> {
  using type = UNB;
};

struct UPT {};
template <>
struct opcode_traits<fchat::C2SOpCode::UPT> {
  using type = UPT;
};

using Commands =
    std::variant<ACB, AOP, AWC, BRO, CBL, CBU, CCR, CDS, CHA, CIU, CKU, COA,
                 COL, COR, CRC, CSO, CTU, CUB, DOP, FKS, IDN, IGN, JCH, KIC,
                 KIK, KIN, LCH, LRP, MSG, ORS, PIN, PRI, PRO, RLD, RLL, RMO,
                 RST, RWD, SFC, STA, TMO, TPN, UNB, UPT>;

template <C2SOpCode Op>
auto parse_payload(std::string_view json) -> Commands {
  typename opcode_traits<Op>::type payload{};

  if (auto err = glz::read_json(payload, json)) {
    throw fchat::parse_error{err, json};
  }

  return payload;
}
auto serialize_command(const Commands& cmd) -> std::string;

auto deserialize_command(C2SOpCode opcode, std::string_view json) -> Commands;

}  // namespace fchat::c2s
