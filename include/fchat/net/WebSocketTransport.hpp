#pragma once

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "fchat/detail/mpsc.hpp"

namespace fchat::net {

class WebSocketTransport {
 public:
  struct WebSocketInitArgs {
    std::string_view user;
    std::string_view password;
    std::string_view token;
    std::time_t tokenExpiresAt;
  };
  using NetInitFunc = void (*)(ix::WebSocket& ctx, WebSocketInitArgs& args);

  WebSocketTransport();
  WebSocketTransport(NetInitFunc initFunc);

  WebSocketTransport(const WebSocketTransport& other);
  auto operator=(const WebSocketTransport& other) -> WebSocketTransport&;

  WebSocketTransport(WebSocketTransport&& other) noexcept;
  auto operator=(WebSocketTransport&& other) noexcept -> WebSocketTransport&;

  ~WebSocketTransport();

  enum class State : std::uint8_t {
    CLOSED,
    EARLY,
    READY,
  };

 private:
  ix::WebSocket websocket_;
  State connectionState_;

  NetInitFunc init_;
  std::optional<std::string> uri_;

  detail::MpscQueue<std::string> read_queue_;
  detail::MpscQueue<std::string> write_queue_;

  void connect(std::string_view uri);
};

}  // namespace fchat::net