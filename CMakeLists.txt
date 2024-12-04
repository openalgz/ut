cmake_minimum_required(VERSION 3.30)

include(cmake/prelude.cmake)

project(
    ut
    VERSION 0.0.4
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF) 

option(ut_ENABLE_MODULES "Use C++20 Module" ON)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/ut.in")
    if (ut_ENABLE_MODULES)
        set(UT_HEADER [=[
export module ut;
import std;
import std.compat;
]=] CACHE INTERNAL "" FORCE)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/ut.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/ut.cppm" @ONLY)  
    else()
        set(UT_HEADER [=[
#pragma once
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string_view>
]=]
        CACHE INTERNAL "" FORCE)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/ut.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/ut.hpp" @ONLY)
    endif()
else()
    message(FATAL_ERROR "Missing ${PROJECT_NAME} Input: ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/ut.in")
endif()

# Conditionally add the library target based on module support
if (ut_ENABLE_MODULES)
    # Define a STATIC library when modules are supported
    add_library(${PROJECT_NAME}_${PROJECT_NAME} STATIC)
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_${PROJECT_NAME})

    # Set the C++ standard and module requirements
    target_compile_features(${PROJECT_NAME}_${PROJECT_NAME} PUBLIC cxx_std_20)

    # Add module interface files using FILE_SET
    target_sources(${PROJECT_NAME}_${PROJECT_NAME}
        PRIVATE
            FILE_SET CXX_MODULES
                FILES
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/ut/ut.cppm
    )

    # Ensure that consumers can locate the module interface
    target_include_directories(${PROJECT_NAME}_${PROJECT_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

    # Optionally, define any compiler-specific flags for modules
    if (COMPILER_SUPPORTS_MSVC_MODULES)
        target_compile_options(${PROJECT_NAME}_${PROJECT_NAME} PRIVATE "/interface")
    elseif (COMPILER_SUPPORTS_FMODULES)
        target_compile_options(${PROJECT_NAME}_${PROJECT_NAME} PRIVATE "-fmodules")
    endif()

else()
    # Use INTERFACE library when modules are not supported
    add_library(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE)
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_${PROJECT_NAME})

    # Define UT_DISABLE_MODULE
    target_compile_definitions(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE UT_DISABLE_MODULE)

    # Specify that C++20 is required
    target_compile_features(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE cxx_std_20)

    # Use traditional include directories
    target_include_directories(${PROJECT_NAME}_${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )
endif()

set_property(TARGET ${PROJECT_NAME}_${PROJECT_NAME} PROPERTY EXPORT_NAME ${PROJECT_NAME})

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
endif()
