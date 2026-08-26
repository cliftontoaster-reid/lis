#pragma once

#include <curl/curl.h>

#include <glaze/json/read.hpp>
#include <string>
#include <string_view>

namespace fchat::net {

struct CurlGlobalGuard {
  CurlGlobalGuard();
  ~CurlGlobalGuard();

  CurlGlobalGuard(const CurlGlobalGuard&) = delete;
  auto operator=(const CurlGlobalGuard&) -> CurlGlobalGuard& = delete;
  CurlGlobalGuard(CurlGlobalGuard&&) = delete;
  auto operator=(CurlGlobalGuard&&) -> CurlGlobalGuard& = delete;
};

struct HttpResponse {
  long status_code{0};
  CURLcode curl_code{CURLE_OK};
  std::string_view body;

  [[nodiscard]] auto ok() const noexcept -> bool;
};

class HttpClient {
 public:
  static auto post(const char* url, std::string_view payload,
                   std::string& response_buffer) -> HttpResponse;
  static auto post(const char* url, std::string_view payload) -> HttpResponse;
  template <typename Endpoint>
  static auto post(const char* url, const Endpoint::UrlArgs& url_args,
                   const Endpoint::FormData& form_data) -> Endpoint::Response {
    using Response = Endpoint::Response;

    if (auto err = url_args.validate(); !err.empty()) {
      return Response{.error = err};
    }
    if (auto err = form_data.validate(); !err.empty()) {
      return Response{.error = err};
    }

    thread_local std::string target_url;
    thread_local std::string payload_buf;
    thread_local std::string response_buf;

    target_url.assign(url);
    url_args.encode_to(target_url);

    payload_buf.clear();
    form_data.encode_to(payload_buf);

    auto res = HttpClient::post(target_url.c_str(), payload_buf, response_buf);
    if (!res.ok()) {
      return Response{.error = "HTTP request failed"};
    }

    Response response{};
    if (auto err = glz::read_json(response, res.body)) {
      return Response{.error = "Failed to parse JSON response"};
    }

    return response;
  }

  static auto get(const char* url, std::string& response_buffer)
      -> HttpResponse;
  static auto get(const char* url) -> HttpResponse;
  template <typename Endpoint>
  static auto get(const char* url, const Endpoint::UrlArgs& url_args)
      -> Endpoint::Response {
    using Response = Endpoint::Response;

    if (auto err = url_args.validate(); !err.empty()) {
      return Response{.error = err};
    }

    thread_local std::string target_url;
    thread_local std::string response_buf;

    target_url.assign(url);
    url_args.encode_to(target_url);

    auto res = HttpClient::get(target_url.c_str(), response_buf);
    if (!res.ok()) {
      return Response{.error = "HTTP request failed"};
    }

    Response response{};
    if (auto err = glz::read_json(response, res.body)) {
      return Response{.error = "Failed to parse JSON response"};
    }

    return response;
  }
};

}  // namespace fchat::net
