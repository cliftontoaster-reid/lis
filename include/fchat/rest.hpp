#pragma once

#include <cstdint>
#include <ctime>
#include <glaze/core/common.hpp>  // glz::object, glz::merge, glz::enumerate
#include <glaze/core/meta.hpp>     // glz::meta
#include <glaze/json/read.hpp>     // glz::read_json
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "util.hpp"

namespace fchat::rest {

struct EmptyPayload {
  void encode_to(std::string& out) const {}
  [[nodiscard]] static auto validate() -> std::string_view { return ""; }
};

template <typename Payload>
struct AuthedEnvelope {
  std::string_view account;
  std::string_view ticket;
  Payload payload;

  [[nodiscard]] auto validate() const -> std::string_view {
    if (account.empty()) { return "account missing"; }
    if (ticket.empty()) { return "ticket missing"; }
    return payload.validate();
  }

  void encode_to(std::string& out) const {
    const auto prev_size = out.size();
    payload.encode_to(out);

    bool first = (out.size() == prev_size);  // NOLINT(misc-const-correctness)
    fchat::util::append_kv(out, "account", first, account);
    fchat::util::append_kv(out, "ticket", first, ticket);
  }
};

template <typename T>
struct ResponseEnvelope {
  std::string_view error;
  T data{};

  [[nodiscard]] constexpr auto ok() const noexcept -> bool {
    return error.empty();
  }
};

// =============================================================================
// GET API TICKET ENDPOINT
// =============================================================================
namespace get_api_ticket {

struct UrlArgs {
  std::string_view account;
  std::string_view password;

  void encode_to(std::string& out) const {
    out.push_back('?');

    util::build_form_encoded(out,                             //
                             std::pair{"account", account},   //
                             std::pair{"password", password}  //
    );
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (account.empty() || password.empty()) {
      return "Account and password cannot be empty for auth.";
    }

    return "";
  }
};

using FormData = EmptyPayload;

struct Payload {
  std::vector<std::string_view> characters;
  std::string_view default_character;
  std::string_view ticket;
  std::vector<std::pair<std::string_view, std::string_view>> friends;
  std::vector<std::string_view> bookmarks;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace get_api_ticket

// =============================================================================
// BOOKMARK ADD ENDPOINT
// =============================================================================
namespace bookmark_add {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view name;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out, std::pair{"name", name});
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (name.empty()) {
      return "Name cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;
using Payload = EmptyPayload;
using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace bookmark_add

// =============================================================================
// BOOKMARK LIST ENDPOINT
// =============================================================================
namespace bookmark_list {

using UrlArgs = EmptyPayload;
using FormData = AuthedEnvelope<EmptyPayload>;

struct Payload {
  std::vector<std::string_view> characters;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace bookmark_list

// =============================================================================
// BOOKMARK REMOVE ENDPOINT
// =============================================================================
namespace bookmark_remove {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view name;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out, std::pair{"name", name});
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (name.empty()) {
      return "Name cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;
using Payload = EmptyPayload;
using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace bookmark_remove

// =============================================================================
// CHARACTER DATA ENDPOINT
// =============================================================================
namespace character_data {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view name;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out, std::pair{"name", name});
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (name.empty()) {
      return "Name cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;

struct Payload {
  struct ProfileSettings {
    bool customs_first;
    bool show_friends;
    bool guestbook;
    bool prevent_bookmarks;
    bool is_public;
  };
  enum class KinkOption : std::uint8_t {
    NO,
    MAYBE,
    YES,
    FAVE,
  };
  struct CustomKink {
    std::string_view name;
    std::string_view description;
    KinkOption choice;
    std::vector<uint32_t> children;
  };
  struct Image {
    uint64_t image_id;
    std::string_view extension;
    uint32_t height;
    uint32_t width;
    std::string_view description;
    uint32_t sort_order;
  };
  struct Character {
    uint32_t id{};
    std::string_view name;
  };

  uint32_t id;
  std::string_view name;
  std::string_view description;
  uint32_t views;
  bool customs_first;
  std::string_view custom_title;
  bool is_self;
  std::vector<std::string_view> badges;
  std::time_t created_at;
  std::time_t updated_at;
  std::map<uint32_t, KinkOption> kinks;
  std::map<uint32_t, CustomKink> custom_kinks;
  std::map<uint32_t, std::variant<uint32_t, std::string_view>> infotags;
  std::vector<Image> images;
  std::vector<Character> character_list;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace character_data

// =============================================================================
// CHARACTER LIST ENDPOINT
// =============================================================================
namespace character_list {

using UrlArgs = EmptyPayload;
using FormData = AuthedEnvelope<EmptyPayload>;

struct Payload {
  std::vector<std::string_view> characters;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace character_list

// =============================================================================
// GROUP LIST ENDPOINT
// =============================================================================
namespace group_list {

using UrlArgs = EmptyPayload;
using FormData = AuthedEnvelope<EmptyPayload>;

struct Payload {
  std::vector<std::string_view> groups;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace group_list

// =============================================================================
// IGNORE LIST ENDPOINT
// =============================================================================
namespace ignore_list {

using UrlArgs = EmptyPayload;
using FormData = AuthedEnvelope<EmptyPayload>;

struct Payload {
  std::vector<std::string_view> ignores;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace ignore_list

// =============================================================================
// INFO LIST ENDPOINT
// =============================================================================
namespace info_list {

using UrlArgs = EmptyPayload;
using FormData = AuthedEnvelope<EmptyPayload>;

struct Payload {
  struct Item {
    enum class Type : std::uint8_t { TEXT, LIST };
    uint32_t id;
    std::string_view name;
    Type type;
    std::optional<std::vector<std::string_view>> list;
  };
  struct InfoGroup {
    std::string_view group;
    std::vector<Item> items;
  };

  std::map<int, InfoGroup> info;
  std::string_view error;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace info_list

// =============================================================================
// KINK LIST ENDPOINT
// =============================================================================
namespace kink_list {

using UrlArgs = EmptyPayload;
using FormData = EmptyPayload;

struct Payload {
  struct KinkItem {
    uint32_t kink_id;
    std::string_view name;
    std::string_view description;
  };
  struct KinkGroup {
    std::string_view group;
    std::vector<KinkItem> items;
  };

  std::map<int, KinkGroup> kinks;
  std::string_view error;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace kink_list

// =============================================================================
// MAPPING LIST ENDPOINT
// =============================================================================
namespace mapping_list {

using UrlArgs = EmptyPayload;
using FormData = EmptyPayload;

struct Payload {
  struct Kink {
    std::string_view id;
    std::string_view name;
    std::string_view description;
    std::string_view group_id;
  };
  struct KinkGroup {
    std::string_view id;
    std::string_view name;
  };
  struct Infotag {
    std::string_view id;
    std::string_view name;
    std::string_view type;
    std::string_view list;
    std::string_view group_id;
  };
  struct InfotagGroup {
    std::string_view id;
    std::string_view name;
  };
  struct ListItem {
    std::string_view id;
    std::string_view name;
    std::string_view value;
  };

  std::vector<Kink> kinks;
  std::vector<KinkGroup> kink_groups;
  std::vector<Infotag> infotags;
  std::vector<InfotagGroup> infotag_groups;
  std::vector<ListItem> listitems;
  std::string_view error;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace mapping_list

// =============================================================================
// FRIEND LIST ENDPOINT
// =============================================================================
namespace friend_list {

using UrlArgs = EmptyPayload;
using FormData = AuthedEnvelope<EmptyPayload>;

struct Friend {
  std::string_view source;
  std::string_view dest;
  std::uint64_t last_online;
};

struct Payload {
  std::vector<Friend> friends;
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace friend_list

// =============================================================================
// FRIEND REMOVE ENDPOINT
// =============================================================================
namespace friend_remove {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view source_name;
  std::string_view dest_name;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out,                                    //
                             std::pair{"source_name", source_name},  //
                             std::pair{"dest_name", dest_name}       //
    );
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (source_name.empty()) {
      return "Source name cannot be empty.";
    }
    if (dest_name.empty()) {
      return "Destination name cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;
using Payload = EmptyPayload;
using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace friend_remove

// =============================================================================
// REQUEST ACCEPT ENDPOINT
// =============================================================================
namespace request_accept {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view request_id;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out, std::pair{"request_id", request_id});
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (request_id.empty()) {
      return "Request ID cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;
using Payload = EmptyPayload;
using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace request_accept

// =============================================================================
// REQUEST DENY ENDPOINT
// =============================================================================
namespace request_deny {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view request_id;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out, std::pair{"request_id", request_id});
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (request_id.empty()) {
      return "Request ID cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;
using Payload = EmptyPayload;
using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace request_deny

// =============================================================================
// REQUEST CANCEL ENDPOINT
// =============================================================================
namespace request_cancel {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view request_id;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out, std::pair{"request_id", request_id});
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (request_id.empty()) {
      return "Request ID cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;
using Payload = EmptyPayload;
using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace request_cancel

// =============================================================================
// REQUEST SEND 2 ENDPOINT
// =============================================================================
namespace request_send {

using UrlArgs = EmptyPayload;

struct RequestPayload {
  std::string_view source;
  std::string_view target;

  void encode_to(std::string& out) const {
    util::build_form_encoded(out,                          //
                             std::pair{"source", source},  //
                             std::pair{"target", target}   //
    );
  }

  [[nodiscard]] auto validate() const -> std::string_view {
    if (source.empty()) {
      return "Source cannot be empty.";
    }
    if (target.empty()) {
      return "Target cannot be empty.";
    }
    return "";
  }
};

using FormData = AuthedEnvelope<RequestPayload>;

struct Payload {
  struct Character {
    uint32_t id;
    std::string_view name;
  };
  struct Request {
    uint32_t id{};
    Character source{};
    Character target{};
    std::time_t createdAt{};
  };
};

using Response = ResponseEnvelope<Payload>;

struct Endpoint {
  using UrlArgs = UrlArgs;
  using FormData = FormData;
  using Payload = Payload;
  using Response = Response;
};

}  // namespace request_send

}  // namespace fchat::rest

// =============================================================================
// GLAZE REFLECTION
// =============================================================================

template <typename T>
struct glz::meta<fchat::rest::ResponseEnvelope<T>> {
  using R = fchat::rest::ResponseEnvelope<T>;
  static constexpr auto value = glz::object(  //
      "error", &R::error,                     //
      glz::merge{&R::data}                    //
  );
};

template <typename Payload>
struct glz::meta<fchat::rest::AuthedEnvelope<Payload>> {
  using T = fchat::rest::AuthedEnvelope<Payload>;
  static constexpr auto value = glz::merge(glz::object(                 //
                                               "account", &T::account,  //
                                               "ticket", &T::ticket),   //
                                           &T::payload);
};

template <>
struct glz::meta<fchat::rest::get_api_ticket::Payload> {
  using T = fchat::rest::get_api_ticket::Payload;
  static constexpr auto value = glz::object(
      "characters", &T::characters,                //
      "default_character", &T::default_character,  //
      "ticket", &T::ticket,                        //
      "friends",
      [](T& obj, auto&& val) -> void {
        struct friend_box {
          std::string_view dest_name;
          std::string_view source_name;
        };
        std::vector<friend_box> boxes{};
        if (!glz::read_json(boxes, val)) {
          obj.friends.clear();
          obj.friends.reserve(boxes.size());
          for (auto&& entry : boxes) {
            obj.friends.emplace_back(entry.dest_name, entry.source_name);
          }
        }
      },
      "bookmarks",
      [](T& obj, auto&& val) -> void {
        struct bookmark_box {
          std::string_view name;
        };
        std::vector<bookmark_box> boxes{};
        if (!glz::read_json(boxes, val)) {
          obj.bookmarks.clear();
          obj.bookmarks.reserve(boxes.size());
          for (auto&& entry : boxes) {
            obj.bookmarks.push_back(entry.name);
          }
        }
      });
};

template <>
struct glz::meta<fchat::rest::character_data::Payload::KinkOption> {
  using enum fchat::rest::character_data::Payload::KinkOption;
  static constexpr auto value = glz::enumerate("no", NO,        //
                                               "maybe", MAYBE,  //
                                               "yes", YES,      //
                                               "fave", FAVE     //
  );
};

template <>
struct glz::meta<fchat::rest::info_list::Payload::Item::Type> {
  using enum fchat::rest::info_list::Payload::Item::Type;
  static constexpr auto value = glz::enumerate("text", TEXT,  //
                                               "list", LIST   //
  );
};
