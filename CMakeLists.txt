cmake_minimum_required(VERSION 3.31)

# CMake gates `import std` behind an opt-in UUID that it changes as the feature
# evolves, so a value hardcoded for one release makes every other release fail.
# It also has to be set before CXX is enabled, hence its position above
# project(). Values are published in CMake's Help/dev/experimental.rst; add a
# row when a new release changes the gate.
set(_ut_import_std_gates
  "3.31=0e5b6991-d74f-4b3d-a41c-cf096e0b2508"
  "4.0=a9e1cf81-9932-4810-974b-6eccaf14e457"
  "4.1=d0edc3af-4c50-42ea-a356-e2862fe7a444"
  "4.2=d0edc3af-4c50-42ea-a356-e2862fe7a444"
  "4.3=451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
  "4.4=f35a9ac6-8463-4d38-8eec-5d6008153e7d"
)

set(UT_IMPORT_STD_GATE "")
foreach(_ut_gate IN LISTS _ut_import_std_gates)
  if(_ut_gate MATCHES "^${CMAKE_MAJOR_VERSION}\\.${CMAKE_MINOR_VERSION}=(.+)$")
    set(UT_IMPORT_STD_GATE "${CMAKE_MATCH_1}")
    break()
  endif()
endforeach()
unset(_ut_gate)
unset(_ut_import_std_gates)

# Leave the variable unset on an unknown release: a wrong UUID is reported as a
# confusing "set to incorrect value" warning, whereas an absent one lets the
# UT_ENABLE_MODULES branch below explain what actually needs updating.
#
# Defer to a value the consumer put in the cache, so
# `-DCMAKE_EXPERIMENTAL_CXX_IMPORT_STD=...` is an escape hatch when a table row is
# wrong or missing. A plain set() here would shadow the cache entry `-D` creates
# and silently discard it.
#
# Test the cache specifically rather than DEFINED: when this project is consumed
# via add_subdirectory() or FetchContent, a parent that hardcodes a single UUID as
# an ordinary variable -- the pattern this table replaced -- would otherwise beat
# the table on every release and pass its staleness on to us. A cache entry is a
# deliberate override; a parent's ordinary variable usually is not.
#
# Assigning to the cache here would also restore the override, but it would pin the
# UUID in CMakeCache.txt for everyone, so upgrading CMake in an existing build tree
# would keep using the previous release's value and quietly ignore the table -- the
# failure this table exists to prevent.
if(UT_IMPORT_STD_GATE AND NOT DEFINED CACHE{CMAKE_EXPERIMENTAL_CXX_IMPORT_STD})
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "${UT_IMPORT_STD_GATE}")
endif()

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
  # Report on the gate value actually in effect. Each branch describes a different
  # cause, so they cannot be collapsed: an unlisted release needs a new table row,
  # while a suppressed or overridden value is something the consumer did and only
  # they can undo.
  if(NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD AND NOT UT_IMPORT_STD_GATE)
    message(WARNING
        "CMake ${CMAKE_VERSION} is newer than any release listed in the `import std` "
        "gate table at the top of CMakeLists.txt, so UT_ENABLE_MODULES will likely "
        "fail to configure. Add this release's CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "
        "value from CMake's Help/dev/experimental.rst to that table, or pass the "
        "value directly with -DCMAKE_EXPERIMENTAL_CXX_IMPORT_STD=<uuid>."
    )
  elseif(NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
    message(WARNING
        "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD was supplied with an empty or false value, "
        "which suppresses the gate this project selected for CMake ${CMAKE_VERSION} "
        "(${UT_IMPORT_STD_GATE}), so UT_ENABLE_MODULES will likely fail to configure. "
        "Pass a UUID, or drop the override with -UCMAKE_EXPERIMENTAL_CXX_IMPORT_STD."
    )
  elseif(UT_IMPORT_STD_GATE AND
         NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD STREQUAL UT_IMPORT_STD_GATE)
    # Cached overrides outlive the command line that introduced them, so say so
    # rather than let a stale one look like the table's own choice.
    message(STATUS
        "Using supplied CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "
        "(${CMAKE_EXPERIMENTAL_CXX_IMPORT_STD}) instead of the value this project "
        "lists for CMake ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} "
        "(${UT_IMPORT_STD_GATE}). Pass -UCMAKE_EXPERIMENTAL_CXX_IMPORT_STD to restore it."
    )
  endif()
  # `import std` remains Ninja-only: the Visual Studio generators cannot build
  # BMIs for IMPORTED targets, and the std module is modeled as one. This is a
  # standing limitation rather than a pending release -- still documented as such
  # in CMake 4.4, under "import std Support" and "Limitations" in
  # cmake-cxxmodules(7). Drop this guard only once that text changes; MSVC itself
  # has been capable since toolset 14.36.
  if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
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
