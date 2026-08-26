#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace fchat::util {

inline void url_encode_append(std::string_view input, std::string& out) {
  static constexpr std::string_view kHexDigits = "0123456789ABCDEF";
  static constexpr unsigned char kLowNibbleMask = 0x0F;

  out.reserve(out.size() + input.size());

  for (const unsigned char byte : input) {
    if ((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') ||
        (byte >= '0' && byte <= '9') || byte == '-' || byte == '_' ||
        byte == '.' || byte == '~') {
      out.push_back(static_cast<char>(byte));
    } else if (byte == ' ') {
      out.push_back('+');
    } else {
      out.push_back('%');
      out.push_back(kHexDigits.at(byte >> 4));
      out.push_back(kHexDigits.at(byte & kLowNibbleMask));
    }
  }
}

inline void append_kv(std::string& out, std::string_view key, bool& first,
                      std::string_view value) {
  if (!first) {
    out.push_back('&');
  }
  first = false;

  url_encode_append(key, out);
  out.push_back('=');
  url_encode_append(value, out);
}

template <typename... Args>
void build_form_encoded(std::string& out, Args&&... kvs) {
  bool first = true;
  auto emit = [&](auto&& kv_pair) -> auto {
    append_kv(out, kv_pair.first, first, kv_pair.second);
  };
  (emit(std::forward<Args>(kvs)), ...);
}

}  // namespace fchat::util
