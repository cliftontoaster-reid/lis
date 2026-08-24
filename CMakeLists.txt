cmake_minimum_required(VERSION 3.15)
project(Lis VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared libraries (.so / .dll)" OFF)
set(LIS_BUILD_SHARED ${BUILD_SHARED_LIBS})

option(LIS_BUILD_TESTS "Build unit tests" ON)

option(LIS_BUILD_COVERAGE "Build with coverage instrumentation and generate reports" OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------------------------
# Include Third-Party Manifests (these may overwrite BUILD_SHARED_LIBS internally)
# ------------------------------------------------------------------------------
include(third_party/glaze/lib.cmake)
include(third_party/IXWebSocket/lib.cmake)
include(third_party/libcurl/lib.cmake)
include(third_party/sqlite/lib.cmake)

if(LIS_BUILD_TESTS)
  include(third_party/gtest/lib.cmake)
  enable_testing()
  add_subdirectory(tests)
endif()

# ------------------------------------------------------------------------------
# Define Main Library Target
# ------------------------------------------------------------------------------
set(LIS_SOURCES
  src/Command.cpp
)

if(LIS_BUILD_SHARED)
  add_library(lis SHARED ${LIS_SOURCES})
else()
  add_library(lis STATIC ${LIS_SOURCES})
endif()

set_target_properties(lis PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)

include(GenerateExportHeader)
generate_export_header(lis
  EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/exports/lis/export.hpp"
  EXPORT_MACRO_NAME LIS_EXPORT
)

target_compile_definitions(lis PRIVATE
  LIS_NAME=\"${PROJECT_NAME}\"
  LIS_VERSION=\"${PROJECT_VERSION}\"

  $<$<PLATFORM_ID:Windows>:LIS_REQUIRES_NET_INIT>
)

add_library(Lis::lis ALIAS lis)

target_include_directories(lis
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/exports>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(LIS_LINK_LIBRARIES
  glaze::glaze
  IXWebSocket::IXWebSocket
  libcurl
  sqlite3
)

target_link_libraries(lis
  PUBLIC
    ${LIS_LINK_LIBRARIES}
)

if(LIS_BUILD_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(lis PRIVATE --coverage)
endif()