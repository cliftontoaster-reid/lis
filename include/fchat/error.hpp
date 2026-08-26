#pragma once

#include <glaze/core/context.hpp>
#include <glaze/core/reflect.hpp>
#include <stdexcept>
#include <string_view>

namespace fchat {

class parse_error : public std::runtime_error {
 public:
  glz::error_code code;

  parse_error(glz::error_ctx ctx, std::string_view buffer)
      : std::runtime_error(glz::format_error(ctx, buffer)), code(ctx.ec) {}
};

}  // namespace fchat