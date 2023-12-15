cmake_minimum_required (VERSION 3.20)
project(rgri)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-O3 -march=native)

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

  add_compile_options(-DENABLE_BINSPARSE)

  # Check whether compiler supports ranges and std::ranges::zip.
  # If not, use range-v3 instead of standard library's ranges.
  include(CheckCXXSymbolExists)
  check_cxx_symbol_exists(__cpp_lib_ranges "ranges" HAS_RANGES)
  check_cxx_symbol_exists(__cpp_lib_ranges_zip "ranges" HAS_ZIP)

  if (HAS_RANGES AND HAS_ZIP)
    set(USE_RANGE_V3 OFF)
    message(STATUS "Standard library supports both ranges and std::views::zip, using standard library.")
  else()
    set(USE_RANGE_V3 ON)
    message(STATUS "NOTE: Standard library does not include ranges and/or std::views::zip. Using range-v3.")
  endif()

  # range-v3
  if (USE_RANGE_V3)
    FetchContent_Declare(
      range-v3
      GIT_REPOSITORY https://github.com/BenBrock/range-v3.git
      GIT_TAG 5300fe3)
    FetchContent_MakeAvailable(range-v3)
    target_link_libraries(rgri INTERFACE range-v3)
  endif()
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
