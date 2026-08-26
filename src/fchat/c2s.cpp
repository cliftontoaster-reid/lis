#include "fchat/c2s.hpp"

#include <glaze/json/write.hpp>
#include <string_view>
#include <variant>

#include "fchat/common.hpp"
#include "fchat/meta.hpp"

namespace fchat::c2s {

auto serialize_command(const Commands& cmd) -> std::string {
  std::string out;
  std::visit(
      [&out](const auto& payload) -> auto {
        auto err = glz::write_json(payload, out);
        if (err) {
          throw std::runtime_error("Serialization failed");
        }
      },
      cmd);
  return out;
}

auto deserialize_command(C2SOpCode opcode, std::string_view json) -> Commands {
  switch (opcode) {
    case C2SOpCode::ACB:
      return parse_payload<C2SOpCode::ACB>(json);
    case C2SOpCode::AOP:
      return parse_payload<C2SOpCode::AOP>(json);
    case C2SOpCode::AWC:
      return parse_payload<C2SOpCode::AWC>(json);
    case C2SOpCode::BRO:
      return parse_payload<C2SOpCode::BRO>(json);
    case C2SOpCode::CBL:
      return parse_payload<C2SOpCode::CBL>(json);
    case C2SOpCode::CBU:
      return parse_payload<C2SOpCode::CBU>(json);
    case C2SOpCode::CCR:
      return parse_payload<C2SOpCode::CCR>(json);
    case C2SOpCode::CDS:
      return parse_payload<C2SOpCode::CDS>(json);
    case C2SOpCode::CHA:
      return parse_payload<C2SOpCode::CHA>(json);
    case C2SOpCode::CIU:
      return parse_payload<C2SOpCode::CIU>(json);
    case C2SOpCode::CKU:
      return parse_payload<C2SOpCode::CKU>(json);
    case C2SOpCode::COA:
      return parse_payload<C2SOpCode::COA>(json);
    case C2SOpCode::COL:
      return parse_payload<C2SOpCode::COL>(json);
    case C2SOpCode::COR:
      return parse_payload<C2SOpCode::COR>(json);
    case C2SOpCode::CRC:
      return parse_payload<C2SOpCode::CRC>(json);
    case C2SOpCode::CSO:
      return parse_payload<C2SOpCode::CSO>(json);
    case C2SOpCode::CTU:
      return parse_payload<C2SOpCode::CTU>(json);
    case C2SOpCode::CUB:
      return parse_payload<C2SOpCode::CUB>(json);
    case C2SOpCode::DOP:
      return parse_payload<C2SOpCode::DOP>(json);
    case C2SOpCode::FKS:
      return parse_payload<C2SOpCode::FKS>(json);
    case C2SOpCode::IDN:
      return parse_payload<C2SOpCode::IDN>(json);
    case C2SOpCode::IGN:
      return parse_payload<C2SOpCode::IGN>(json);
    case C2SOpCode::JCH:
      return parse_payload<C2SOpCode::JCH>(json);
    case C2SOpCode::KIC:
      return parse_payload<C2SOpCode::KIC>(json);
    case C2SOpCode::KIK:
      return parse_payload<C2SOpCode::KIK>(json);
    case C2SOpCode::KIN:
      return parse_payload<C2SOpCode::KIN>(json);
    case C2SOpCode::LCH:
      return parse_payload<C2SOpCode::LCH>(json);
    case C2SOpCode::LRP:
      return parse_payload<C2SOpCode::LRP>(json);
    case C2SOpCode::MSG:
      return parse_payload<C2SOpCode::MSG>(json);
    case C2SOpCode::ORS:
      return parse_payload<C2SOpCode::ORS>(json);
    case C2SOpCode::PIN:
      return parse_payload<C2SOpCode::PIN>(json);
    case C2SOpCode::PRI:
      return parse_payload<C2SOpCode::PRI>(json);
    case C2SOpCode::PRO:
      return parse_payload<C2SOpCode::PRO>(json);
    case C2SOpCode::RLD:
      return parse_payload<C2SOpCode::RLD>(json);
    case C2SOpCode::RLL:
      return parse_payload<C2SOpCode::RLL>(json);
    case C2SOpCode::RMO:
      return parse_payload<C2SOpCode::RMO>(json);
    case C2SOpCode::RST:
      return parse_payload<C2SOpCode::RST>(json);
    case C2SOpCode::RWD:
      return parse_payload<C2SOpCode::RWD>(json);
    case C2SOpCode::SFC:
      return parse_payload<C2SOpCode::SFC>(json);
    case C2SOpCode::STA:
      return parse_payload<C2SOpCode::STA>(json);
    case C2SOpCode::TMO:
      return parse_payload<C2SOpCode::TMO>(json);
    case C2SOpCode::TPN:
      return parse_payload<C2SOpCode::TPN>(json);
    case C2SOpCode::UNB:
      return parse_payload<C2SOpCode::UNB>(json);
    case C2SOpCode::UPT:
      return parse_payload<C2SOpCode::UPT>(json);
    default:
      throw std::runtime_error("Unknown opcode");
  }
}

}  // namespace fchat::c2s