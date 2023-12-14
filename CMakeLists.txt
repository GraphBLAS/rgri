cmake_minimum_required (VERSION 3.20)
project(rgri)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCS "Build Sphinx documentation" OFF)

add_subdirectory(include)

if (is_top_level)
  include(FetchContent)
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