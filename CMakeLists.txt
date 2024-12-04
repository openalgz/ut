cmake_minimum_required(VERSION 3.30)

include(cmake/prelude.cmake)

project(
    ut
    VERSION 0.0.4
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# Detect compiler support for C++20 modules flags
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("/interface" COMPILER_SUPPORTS_MSVC_MODULES)
check_cxx_compiler_flag("-fmodules" COMPILER_SUPPORTS_FMODULES)

# Initialize UT_ENABLE_MODULES to OFF
set(UT_ENABLE_MODULES OFF)

# Check if either MSVC or FMODULES flags are supported
if (COMPILER_SUPPORTS_MSVC_MODULES OR COMPILER_SUPPORTS_FMODULES)
    # Determine the appropriate module flag
    if (COMPILER_SUPPORTS_MSVC_MODULES)
        set(MODULE_FLAG "/interface")
    else()
        set(MODULE_FLAG "-fmodules")
    endif()

    # Create a temporary directory for the test
    set(TEST_COMPILE_DIR "${CMAKE_BINARY_DIR}/CMakeTmp/module_test")

    file(MAKE_DIRECTORY "${TEST_COMPILE_DIR}")

    # Create a minimal CMakeLists.txt for the test
    file(WRITE "${TEST_COMPILE_DIR}/CMakeLists.txt" "
        cmake_minimum_required(VERSION 3.30)
        project(ModuleTest LANGUAGES CXX)
        set(CMAKE_CXX_STANDARD 20)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        add_library(ModuleTest OBJECT test_module.cppm)
        target_compile_options(ModuleTest PRIVATE ${MODULE_FLAG})
    ")

    # Create the test module source file
    file(WRITE "${TEST_COMPILE_DIR}/test_module.cppm" "module;")

    # Attempt to compile the test module
    try_compile(
        MODULE_COMPILES             # Variable to store the result
        "${TEST_COMPILE_DIR}"       # Directory for the build
        "${TEST_COMPILE_DIR}/test_module.cppm"  # Source file
        # Pass the module flag via CMAKE_CXX_FLAGS
        CMAKE_FLAGS
            -DCMAKE_CXX_FLAGS=${MODULE_FLAG}
            -DCMAKE_CXX_STANDARD=20
            -DCMAKE_CXX_STANDARD_REQUIRED=ON
        OUTPUT_VARIABLE MODULE_COMPILE_OUTPUT  # Capture compile output (optional)
    )

    # Optionally, you can output the compile results for debugging
    # message(STATUS "Module compile output: ${MODULE_COMPILE_OUTPUT}")

    # Set UT_ENABLE_MODULES based on the compilation result
    if (MODULE_COMPILES)
        set(UT_ENABLE_MODULES ON)
    else()
        set(UT_ENABLE_MODULES OFF)
        message(STATUS "C++20 modules are not supported by the compiler.")
    endif()

    # Clean up the temporary directory (optional)
    # file(REMOVE_RECURSE "${TEST_COMPILE_DIR}")
endif()

# Conditionally add the library target based on module support
if (UT_ENABLE_MODULES)
    # Use OBJECT library when modules are supported
    add_library(${PROJECT_NAME}_${PROJECT_NAME} OBJECT)
    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_${PROJECT_NAME})

    # Set the C++ standard and module requirements
    target_compile_features(${PROJECT_NAME}_${PROJECT_NAME} PUBLIC cxx_std_23)

    # Add the module interface source file
    target_sources(${PROJECT_NAME}_${PROJECT_NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include/ut/ut.cppm
    )

    # Ensure that consumers can locate the module interface
    target_include_directories(${PROJECT_NAME}_${PROJECT_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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

    # Specify that C++23 is required
    target_compile_features(${PROJECT_NAME}_${PROJECT_NAME} INTERFACE cxx_std_23)

    # Use traditional include directories
    target_include_directories(${PROJECT_NAME}_${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
endif()

set_property(TARGET ${PROJECT_NAME}_${PROJECT_NAME} PROPERTY EXPORT_NAME ${PROJECT_NAME})

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
endif()
