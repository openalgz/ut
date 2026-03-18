cmake_minimum_required(VERSION 3.31) 

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b") 

include(cmake/prelude.cmake)

project(
  ut
  VERSION 1.2.0
  LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

option(UT_ENABLE_MODULES "Enable modules with import std" OFF)
option(UT_COMPILE_TIME "Enable compile time features" OFF)

if(UT_ENABLE_MODULES)
  if(NOT CMAKE_GENERATOR MATCHES "Visual Studio") ## Support should be added with 4.4, todo: check when available
    set(CMAKE_CXX_MODULE_STD 1)
  endif()
endif()

if(UT_ENABLE_MODULES)
  add_library(${PROJECT_NAME}_${PROJECT_NAME} STATIC)
  set(UT_LIB_TYPE PUBLIC)
else()
  add_library(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE)
  set(UT_LIB_TYPE INTERFACE)
endif()
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_${PROJECT_NAME})

if(PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
endif()

target_compile_features(${PROJECT_NAME}_${PROJECT_NAME} ${UT_LIB_TYPE} cxx_std_23)

if(UT_ENABLE_MODULES)
  target_sources(${PROJECT_NAME}_${PROJECT_NAME}
    PUBLIC
      FILE_SET modules TYPE CXX_MODULES
      BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
      FILES
        "${PROJECT_SOURCE_DIR}/modules/ut.ixx"
  )

  target_compile_definitions(${PROJECT_NAME}_${PROJECT_NAME} PRIVATE UT_ENABLE_MODULES)
endif()

if(UT_COMPILE_TIME)
  target_compile_definitions(${PROJECT_NAME}_${PROJECT_NAME} ${UT_LIB_TYPE} UT_COMPILE_TIME)
endif()

if(MSVC)
  string(REGEX MATCH "\/cl(.exe)?$" matched_cl ${CMAKE_CXX_COMPILER})
  if(matched_cl)
    # for a C++ standards compliant preprocessor, not needed for clang-cl
    target_compile_options(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE "/Zc:preprocessor" /GL /permissive- /Zc:lambda)
    target_link_options(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE /LTCG /INCREMENTAL:NO)
  endif()
endif()

set_property(TARGET ${PROJECT_NAME}_${PROJECT_NAME} PROPERTY EXPORT_NAME ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}_${PROJECT_NAME} ${warning_guard}
  INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()
