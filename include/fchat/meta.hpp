#pragma once

#include <format>
#include <glaze/core/opts.hpp>
#include <glaze/glaze.hpp>

#include "c2s.hpp"
#include "fchat/common.hpp"
#include "fchat/s2c.hpp"

template <>
struct glz::meta<fchat::c2s::IDN> {
  static constexpr auto value = glz::object(
      "method", [](auto&&) { return "ticket"; },  //
      "account", &fchat::c2s::IDN::account,       //
      "ticket", &fchat::c2s::IDN::ticket,         //
      "character", &fchat::c2s::IDN::character,   //
      "cname", &fchat::c2s::IDN::cname,           //
      "cversion", &fchat::c2s::IDN::cversion      //
  );
};

template <>
struct glz::meta<fchat::c2s::RLL::DiceExpr> {
  static constexpr auto custom_write = true;
  static constexpr auto custom_read = true;
};

namespace glz::detail {
template <>
struct to<JSON, fchat::c2s::RLL::DiceExpr> {
  template <auto Opts>
  static void op(const fchat::c2s::RLL::DiceExpr& value,
                 auto&&... args) noexcept {
    std::string out;
    if (auto* str = std::get_if<std::string>(&value.value)) {
      out = *str;
    } else if (auto* vec = std::get_if<
                   std::vector<std::variant<fchat::c2s::RLL::Dice, uint16_t>>>(
                   &value.value)) {
      std::string formatted;
      bool first = true;
      for (const auto& item : *vec) {
        if (auto* d = std::get_if<fchat::c2s::RLL::Dice>(&item)) {
          if (!first) formatted += "+";
          if (d->mul != 1) {
            formatted += std::format("{}d{}", d->mul, d->sides);
          } else {
            formatted += std::format("d{}", d->sides);
          }
          if (d->add != 0) {
            formatted += std::format("+{}", d->add);
          }
        } else if (auto* num = std::get_if<uint16_t>(&item)) {
          if (!first) formatted += "+";
          formatted += std::to_string(*num);
        }
        first = false;
      }
      out = formatted;
    }
    write<JSON>::op<Opts>(out, args...);
  }
};

template <>
struct from<JSON, fchat::c2s::RLL::DiceExpr> {
  template <auto Opts>
  static void op(fchat::c2s::RLL::DiceExpr& value, auto&&... args) {
    std::string s;
    read<JSON>::op<Opts>(s, args...);
    value.value = std::move(s);
  }
};
}  // namespace glz::detail

template <>
struct glz::meta<fchat::ChannelMode> {
  using enum fchat::ChannelMode;
  static constexpr auto value =
      glz::enumerate("chat", CHAT, "ads", ADS, "both", BOTH);
};

template <>
struct glz::meta<fchat::c2s::STA::Status> {
  using enum fchat::c2s::STA::Status;
  static constexpr auto value =
      glz::enumerate("online", ONLINE, "looking", LOOKING, "busy", BUSY, "dnd",
                     DND, "idle", IDLE, "away", AWAY, "crown", CROWN);
};

template <>
struct glz::meta<fchat::c2s::TPN::Status> {
  using enum fchat::c2s::TPN::Status;
  static constexpr auto value =
      glz::enumerate("clear", CLEAR, "paused", PAUSED, "typing", TYPING);
};

template <>
struct glz::meta<fchat::c2s::IGN::Action> {
  using enum fchat::c2s::IGN::Action;
  static constexpr auto value = glz::enumerate("add", ADD, "delete", DELETE,
                                               "notify", NOTIFY, "list", LIST);
};

template <>
struct glz::meta<fchat::c2s::RST::Status> {
  using enum fchat::c2s::RST::Status;
  static constexpr auto value =
      glz::enumerate("public", PUBLIC, "private", PRIVATE);
};

template <>
struct glz::meta<fchat::s2c::CBU> {
  using T = fchat::s2c::CBU;
  static constexpr auto value = glz::object(
      "operator", &T::op, "channel", &T::channel, "character", &T::character);
};

template <>
struct glz::meta<fchat::s2c::CKU> {
  using T = fchat::s2c::CKU;
  static constexpr auto value = glz::object(
      "operator", &T::op, "channel", &T::channel, "character", &T::character);
};

template <>
struct glz::meta<fchat::s2c::CTU> {
  using T = fchat::s2c::CTU;
  static constexpr auto value = glz::object(
      "operator", &T::op, "channel", &T::channel, "character", &T::character);
};

template <>
struct glz::meta<fchat::s2c::ICH> {
  using T = fchat::s2c::ICH;
  struct user_box {
    std::string_view identity;
  };
  static constexpr auto read_users = [](T& obj, std::vector<user_box>& val) {
    obj.users.clear();
    obj.users.reserve(val.size());
    for (const auto& box : val) {
      obj.users.push_back(box.identity);
    }
  };
  static constexpr auto write_users = [](const T& obj) {
    std::vector<user_box> out;
    out.reserve(obj.users.size());
    for (const auto& u : obj.users) {
      out.push_back(user_box{u});
    }
    return out;
  };
  static constexpr auto value =
      glz::object("users", glz::custom<read_users, write_users>,  //
                  "channel", &T::channel, "mode", &T::mode);
};

template <>
struct glz::meta<fchat::s2c::JCH> {
  using T = fchat::s2c::JCH;
  struct identity_box {
    std::string_view identity;
  };
  static constexpr auto read_character = [](T& obj, identity_box& val) {
    obj.character = val.identity;
  };
  static constexpr auto write_character = [](const T& obj) {
    return identity_box{obj.character};
  };
  static constexpr auto value =
      glz::object("channel", &T::channel,                                     //
                  "character", glz::custom<read_character, write_character>,  //
                  "title", &T::title);
};

template <>
struct glz::meta<fchat::s2c::KID::KidType> {
  using enum fchat::s2c::KID::KidType;
  static constexpr auto value =
      glz::enumerate("start", START, "custom", CUSTOM, "end", END);
};

template <>
struct glz::meta<fchat::s2c::LIS::CharacterInfo> {
  static constexpr auto value =
      glz::array(&fchat::s2c::LIS::CharacterInfo::name,
                 &fchat::s2c::LIS::CharacterInfo::gender,
                 &fchat::s2c::LIS::CharacterInfo::status,
                 &fchat::s2c::LIS::CharacterInfo::status_message);
};

template <>
struct glz::meta<fchat::s2c::IGN::Action> {
  using enum fchat::s2c::IGN::Action;
  static constexpr auto value = glz::enumerate("init", INIT,      //
                                               "add", ADD,        //
                                               "delete", DELETE,  //
                                               "notify", NOTIFY   //
  );
};

template <>
struct glz::meta<fchat::s2c::PRD::ProfileType> {
  using enum fchat::s2c::PRD::ProfileType;
  static constexpr auto value = glz::enumerate("start", START,    //
                                               "info", INFO,      //
                                               "select", SELECT,  //
                                               "end", END         //
  );
};

template <>
struct glz::meta<fchat::s2c::RLL> {
  static constexpr std::string_view tag = "type";
  static constexpr auto ids = std::array{"dice", "bottle"};
};

template <>
struct glz::meta<fchat::s2c::SFC> {
  static constexpr std::string_view tag = "action";
  static constexpr auto ids = std::array{"confirm", "report"};
};

struct VarTmp {
  std::string variable;
  glz::raw_json value;  // captures the raw JSON of "value"
};

template <>
struct glz::meta<fchat::s2c::VAR> {
  static constexpr auto custom_read = true;  // write stays default (reflection)
};

template <>
struct glz::detail::from<glz::JSON, fchat::s2c::VAR> {
  template <auto Opts>
  static void op(fchat::s2c::VAR& value, auto&&... args) {
    VarTmp tmp{};
    read<JSON>::op<Opts>(tmp, args...);  // read {variable, value}
    value.variable = tmp.variable;
    std::string_view s = tmp.value.str;

    if (tmp.variable == "permissions") {
      // server sends a QUOTED number; quoted_num only here
      int ii{};
      if (!glz::read<glz::opts{.quoted_num = true}>(ii, s)) {
        value.value = ii;
        return;
      }
    } else {
      // normal path: int -> double -> vector, NO quoted_num
      int ii{};
      if (!glz::read<glz::opts{}>(ii, s)) {
        value.value = ii;
        return;
      }
      double dd{};
      if (!glz::read<glz::opts{}>(dd, s)) {
        value.value = dd;
        return;
      }
      const auto p = s.find_first_not_of(" \t\r\n");
      if (p != std::string_view::npos && s[p] == '[') {
        std::vector<std::string> v{};
        if (!glz::read<glz::opts{}>(v, s)) {
          value.value = std::move(v);
          return;
        }
      }
    }
    if constexpr (sizeof...(args) >= 1)
      std::get<0>(std::tie(args...)).error = glz::error_code::syntax_error;
  }
};
