#include "fchat/s2c.hpp"

#include <glaze/json/write.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

#include "fchat/common.hpp"
#include "fchat/meta.hpp"

namespace fchat::s2c {

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

auto deserialize_command(S2COpCode opcode, std::string_view json) -> Commands {
  switch (opcode) {
    case S2COpCode::ADL:
      return parse_payload<S2COpCode::ADL>(json);
    case S2COpCode::AOP:
      return parse_payload<S2COpCode::AOP>(json);
    case S2COpCode::BRO:
      return parse_payload<S2COpCode::BRO>(json);
    case S2COpCode::CBU:
      return parse_payload<S2COpCode::CBU>(json);
    case S2COpCode::CDS:
      return parse_payload<S2COpCode::CDS>(json);
    case S2COpCode::CHA:
      return parse_payload<S2COpCode::CHA>(json);
    case S2COpCode::CIU:
      return parse_payload<S2COpCode::CIU>(json);
    case S2COpCode::CKU:
      return parse_payload<S2COpCode::CKU>(json);
    case S2COpCode::COA:
      return parse_payload<S2COpCode::COA>(json);
    case S2COpCode::COL:
      return parse_payload<S2COpCode::COL>(json);
    case S2COpCode::CON:
      return parse_payload<S2COpCode::CON>(json);
    case S2COpCode::COR:
      return parse_payload<S2COpCode::COR>(json);
    case S2COpCode::CSO:
      return parse_payload<S2COpCode::CSO>(json);
    case S2COpCode::CTU:
      return parse_payload<S2COpCode::CTU>(json);
    case S2COpCode::DOP:
      return parse_payload<S2COpCode::DOP>(json);
    case S2COpCode::ERR:
      return parse_payload<S2COpCode::ERR>(json);
    case S2COpCode::FKS:
      return parse_payload<S2COpCode::FKS>(json);
    case S2COpCode::FLN:
      return parse_payload<S2COpCode::FLN>(json);
    case S2COpCode::HLO:
      return parse_payload<S2COpCode::HLO>(json);
    case S2COpCode::FRL:
      return parse_payload<S2COpCode::FRL>(json);
    case S2COpCode::ICH:
      return parse_payload<S2COpCode::ICH>(json);
    case S2COpCode::IDN:
      return parse_payload<S2COpCode::IDN>(json);
    case S2COpCode::IGN:
      return parse_payload<S2COpCode::IGN>(json);
    case S2COpCode::JCH:
      return parse_payload<S2COpCode::JCH>(json);
    case S2COpCode::KID:
      return parse_payload<S2COpCode::KID>(json);
    case S2COpCode::LCH:
      return parse_payload<S2COpCode::LCH>(json);
    case S2COpCode::LIS:
      return parse_payload<S2COpCode::LIS>(json);
    case S2COpCode::LRP:
      return parse_payload<S2COpCode::LRP>(json);
    case S2COpCode::MSG:
      return parse_payload<S2COpCode::MSG>(json);
    case S2COpCode::NLN:
      return parse_payload<S2COpCode::NLN>(json);
    case S2COpCode::ORS:
      return parse_payload<S2COpCode::ORS>(json);
    case S2COpCode::PIN:
      return parse_payload<S2COpCode::PIN>(json);
    case S2COpCode::PRD:
      return parse_payload<S2COpCode::PRD>(json);
    case S2COpCode::PRI:
      return parse_payload<S2COpCode::PRI>(json);
    case S2COpCode::RLL:
      return parse_payload<S2COpCode::RLL>(json);
    case S2COpCode::RMO:
      return parse_payload<S2COpCode::RMO>(json);
    case S2COpCode::RTB:
      return parse_payload<S2COpCode::RTB>(json);
    case S2COpCode::SFC:
      return parse_payload<S2COpCode::SFC>(json);
    case S2COpCode::STA:
      return parse_payload<S2COpCode::STA>(json);
    case S2COpCode::SYS:
      return parse_payload<S2COpCode::SYS>(json);
    case S2COpCode::TPN:
      return parse_payload<S2COpCode::TPN>(json);
    case S2COpCode::UPT:
      return parse_payload<S2COpCode::UPT>(json);
    case S2COpCode::VAR:
      return parse_payload<S2COpCode::VAR>(json);
    default:
      throw std::runtime_error("Unknown opcode");
  }
}

}  // namespace fchat::s2c