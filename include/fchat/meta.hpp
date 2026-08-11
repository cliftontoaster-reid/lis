#pragma once

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
  using T = fchat::c2s::RLL::DiceExpr;
  static constexpr auto value = [](auto&& self, auto&& write_val) {
    // If it's a raw string (like "bottle"), write it directly
    if (auto* str = std::get_if<std::string_view>(&self.value)) {
      write_val = *str;
    } else if (auto* vec = std::get_if<
                   std::vector<std::variant<fchat::c2s::RLL::Dice, uint16_t>>>(
                   &self.value)) {
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
      write_val = formatted;
    }
  };
};

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
                     DND, "idle", IDLE, "away", AWAY);
};

template <>
struct glz::meta<fchat::c2s::TPN::Status> {
  using enum fchat::c2s::TPN::Status;
  static constexpr auto value =
      glz::enumerate("clear", CLEAR, "paused", PAUSED, "typing", TYPING);
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
  static constexpr auto value = glz::object(
      "users",
      [](T& obj, auto&& val) {
        struct user_box {
          std::string_view identity;
        };

        std::vector<user_box> boxes;

        if (!glz::read_json(boxes, val)) {
          obj.users.clear();
          obj.users.reserve(boxes.size());
          for (const auto& box : boxes) {
            obj.users.push_back(box.identity);
          }
        }
      },
      "channel", &T::channel, "mode", &T::mode);
};

template <>
struct glz::meta<fchat::s2c::JCH> {
  using T = fchat::s2c::JCH;
  static constexpr auto value = glz::object(
      "channel", &T::channel, "character",
      [](T& obj, auto&& val) {
        struct identity_box {
          std::string_view identity;
        };
        identity_box box{};
        if (!glz::read_json(box, val)) {
          obj.character = box.identity;
        }
      },
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