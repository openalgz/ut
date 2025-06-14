cmake_minimum_required(VERSION 3.24)

include(cmake/prelude.cmake)

project(
    ut
    VERSION 1.0.0
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_library(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_${PROJECT_NAME})

if (MSVC)
   string(REGEX MATCH "\/cl(.exe)?$" matched_cl ${CMAKE_CXX_COMPILER})
   if (matched_cl)
      # for a C++ standards compliant preprocessor, not needed for clang-cl
      target_compile_options(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE "/Zc:preprocessor" /GL /permissive- /Zc:lambda)
      target_link_options(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE /LTCG /INCREMENTAL:NO)
   endif()
endif()

set_property(TARGET ${PROJECT_NAME}_${PROJECT_NAME} PROPERTY EXPORT_NAME ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(
   ${PROJECT_NAME}_${PROJECT_NAME} ${warning_guard}
    INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
endif()