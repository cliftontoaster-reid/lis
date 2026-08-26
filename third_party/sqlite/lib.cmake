# third_party/sqlite/lib.cmake
include(FetchContent)

# Fetch official SQLite amalgamation (single C file for inline compiler optimization)
FetchContent_Declare(
  sqlite3
  URL https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(sqlite3)

if(NOT TARGET sqlite3)
  add_library(sqlite3 STATIC "${sqlite3_SOURCE_DIR}/sqlite3.c")
  target_include_directories(sqlite3 PUBLIC "${sqlite3_SOURCE_DIR}")

  target_compile_definitions(sqlite3 PRIVATE
    SQLITE_THREADSAFE=1             # Multi-threading support
    SQLITE_DEFAULT_MEMSTATUS=0      # Strip global memory tracking overhead
    SQLITE_OMIT_DEPRECATED          # Strip legacy APIs
    SQLITE_DQS=0                    # Disable double-quoted string literals
    SQLITE_CORE
  )

  target_link_libraries(sqlite3 PRIVATE ${CMAKE_DL_LIBS})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../mark_system.cmake)
lis_mark_system(sqlite3)