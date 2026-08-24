include(FetchContent)

# Configure IXWebSocket options before declaring
set(USE_TLS ON CACHE BOOL "Enable TLS/SSL in IXWebSocket" FORCE)
set(USE_OPEN_SSL ON CACHE BOOL "Use OpenSSL for TLS" FORCE)

FetchContent_Declare(
  ixwebsocket
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
  GIT_TAG        v12.0.1
)

FetchContent_MakeAvailable(ixwebsocket)

# Alias for consistent target naming matching your SQLite style
if(TARGET ixwebsocket AND NOT TARGET IXWebSocket::IXWebSocket)
  add_library(IXWebSocket::IXWebSocket ALIAS ixwebsocket)
endif()
