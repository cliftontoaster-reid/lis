# third_party/glaze/lib.cmake
include(FetchContent)

# Options to keep Glaze build minimal
set(glaze_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(glaze_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG        v4.2.1  # Replace with latest stable release tag
)

FetchContent_MakeAvailable(glaze)