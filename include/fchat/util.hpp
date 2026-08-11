#pragma once

#include <string>

namespace fchat {

namespace util {

inline void url_encode_append(std::string_view input, std::string& out) {
  static constexpr char hex_digits[] = "0123456789ABCDEF";

  // Pre-reserve capacity to prevent intermediate reallocations
  out.reserve(out.size() + input.size());

  for (const unsigned char c : input) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
        c == '~') {
      out.push_back(static_cast<char>(c));
    } else if (c == ' ') {
      out.push_back('+');
    } else {
      out.push_back('%');
      out.push_back(hex_digits[c >> 4]);
      out.push_back(hex_digits[c & 0x0F]);
    }
  }
}

inline void append_kv(std::string& out, bool& first, std::string_view key,
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
  (append_kv(out, first, kvs.first, kvs.second), ...);
}

}  // namespace util

}  // namespace fchat
