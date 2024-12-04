enable_language(CXX)

cmake_minimum_required(VERSION 3.30)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

include(CTest)
if(BUILD_TESTING)
  # Uncomment and configure tests as needed
  # add_subdirectory(tests)
endif()

# Gather source files for the dev-mode executable
# Since the library is module-based, you might not need to gather additional source files
# If main.cpp exists and is part of the executable, include it directly

set(sources "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Organize source files into IDE-friendly groups
source_group(TREE "${PROJECT_SOURCE_DIR}/src" PREFIX sources FILES ${sources})

add_executable(${PROJECT_NAME}_ide ${sources})

# Link against the 'ut' library
target_link_libraries(${PROJECT_NAME}_ide PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})

# Organize targets into folders within the IDE
set_target_properties(${PROJECT_NAME}_${PROJECT_NAME} ${PROJECT_NAME}_ide PROPERTIES FOLDER ProjectTargets)

# Add tests for the executable
add_test(NAME ${PROJECT_NAME}_ide COMMAND ${PROJECT_NAME}_ide)
