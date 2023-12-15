cmake_minimum_required (VERSION 3.20)
project(rgri)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCS "Build Sphinx documentation" OFF)
option(ENABLE_BINSPARSE "Enable binsparse file parsing" OFF)

add_subdirectory(include)

include(FetchContent)

if (ENABLE_BINSPARSE)
  FetchContent_Declare(
    binsparse
    GIT_REPOSITORY https://github.com/GraphBLAS/binsparse-reference-impl.git
    GIT_TAG main)
  FetchContent_MakeAvailable(binsparse)

  target_link_libraries(rgri INTERFACE binsparse)
endif()

if (is_top_level)
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1)
  FetchContent_MakeAvailable(fmt)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
  )
  FetchContent_MakeAvailable(Catch2)

  add_subdirectory(examples)
  add_subdirectory(tests)

  if (BUILD_DOCS)
    add_subdirectory(docs)
  endif()
endif()