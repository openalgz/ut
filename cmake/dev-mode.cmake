enable_language(CXX)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

include(CTest)
if(BUILD_TESTING)
  #add_subdirectory(tests)
endif()

# Done in developer mode only, so users won't be bothered by this :)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/*.hpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX headers FILES ${headers})

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/src" PREFIX sources FILES ${sources})

add_executable(${PROJECT_NAME}_ide ${sources} ${headers})

target_link_libraries(${PROJECT_NAME}_ide PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}_${PROJECT_NAME} ${PROJECT_NAME}_ide PROPERTIES FOLDER ProjectTargets)

add_test(NAME ${PROJECT_NAME}_ide COMMAND ${PROJECT_NAME}_ide)

# UT_RUN feature tests
add_executable(ut_run_tests "${PROJECT_SOURCE_DIR}/tests/ut_run_tests.cpp")
target_link_libraries(ut_run_tests PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})

# Test: No filter - all 5 tests should run
add_test(NAME ut_run_no_filter COMMAND ut_run_tests)
set_tests_properties(ut_run_no_filter PROPERTIES
   PASS_REGULAR_EXPRESSION "tests: 5 \\(5 passed"
)

# Test: Single test filter
add_test(NAME ut_run_single COMMAND ut_run_tests)
set_tests_properties(ut_run_single PROPERTIES
   ENVIRONMENT "UT_RUN=alpha"
   PASS_REGULAR_EXPRESSION "tests: 1 \\(1 passed"
)

# Test: Single test with space in name
add_test(NAME ut_run_single_with_space COMMAND ut_run_tests)
set_tests_properties(ut_run_single_with_space PROPERTIES
   ENVIRONMENT "UT_RUN=delta test"
   PASS_REGULAR_EXPRESSION "tests: 1 \\(1 passed"
)

# Test: Array filter with two tests
add_test(NAME ut_run_array_two COMMAND ut_run_tests)
set_tests_properties(ut_run_array_two PROPERTIES
   ENVIRONMENT "UT_RUN=[alpha,beta]"
   PASS_REGULAR_EXPRESSION "tests: 2 \\(2 passed"
)

# Test: Array filter with three tests
add_test(NAME ut_run_array_three COMMAND ut_run_tests)
set_tests_properties(ut_run_array_three PROPERTIES
   ENVIRONMENT "UT_RUN=[alpha,beta,gamma]"
   PASS_REGULAR_EXPRESSION "tests: 3 \\(3 passed"
)

# Test: Array filter with tests containing spaces
add_test(NAME ut_run_array_with_spaces COMMAND ut_run_tests)
set_tests_properties(ut_run_array_with_spaces PROPERTIES
   ENVIRONMENT "UT_RUN=[delta test,epsilon test]"
   PASS_REGULAR_EXPRESSION "tests: 2 \\(2 passed"
)

# Test: Non-matching filter - no tests should run
add_test(NAME ut_run_no_match COMMAND ut_run_tests)
set_tests_properties(ut_run_no_match PROPERTIES
   ENVIRONMENT "UT_RUN=nonexistent"
   PASS_REGULAR_EXPRESSION "tests: 0 \\(0 passed"
)
