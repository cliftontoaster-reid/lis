# third_party/gtest/lib.cmake
include(FetchContent)

# Keep GoogleTest build minimal
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.18.0
)

FetchContent_MakeAvailable(googletest)
