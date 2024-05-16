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
