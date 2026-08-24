file(GLOB LIS_TEST_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(lis_tests ${LIS_TEST_SOURCES})

target_link_libraries(lis_tests PRIVATE
  Lis::lis
  GTest::gtest_main
)

target_include_directories(lis_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(lis_tests)

if(LIS_BUILD_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(lis_tests PRIVATE --coverage)
  target_link_options(lis_tests PRIVATE --coverage)

  find_program(GCOVR_EXECUTABLE gcovr)
  if(GCOVR_EXECUTABLE)
    add_custom_target(coverage
      COMMAND ${CMAKE_CTEST_COMMAND} --output-junit ${CMAKE_BINARY_DIR}/junit.xml --output-on-failure
      COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/coverage/html
      COMMAND ${GCOVR_EXECUTABLE}
        -r ${PROJECT_SOURCE_DIR}
        --object-directory ${CMAKE_BINARY_DIR}
        --txt-metric branch
        --exclude-unreachable-branches
        --exclude-throw-branches
        --filter ${PROJECT_SOURCE_DIR}/src
        --filter ${PROJECT_SOURCE_DIR}/include
        --html-details ${CMAKE_BINARY_DIR}/coverage/html/index.html
        --xml ${CMAKE_BINARY_DIR}/coverage/coverage.xml
        --json ${CMAKE_BINARY_DIR}/coverage/coverage.json
      COMMAND ${CMAKE_COMMAND} -E echo
        "Coverage reports written to ${CMAKE_BINARY_DIR}/coverage"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS lis_tests
      COMMENT "Generating coverage reports"
      VERBATIM
    )
  endif()
endif()
