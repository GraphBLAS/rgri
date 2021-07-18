cmake_minimum_required(VERSION 3.19...3.21)
project(RGRI VERSION 0.1.0)

enable_testing()

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

option(RGRI_DOCS "Build Doxygen and Sphinx docs" ${is_top_level})
option(RGRI_TESTS "Build tests" ${is_top_level})
option(RGRI_PACKAGING "Include CMake install/packaging rules" ${is_top_level})

add_subdirectory(include)

if (RGRI_DOCS)
    add_subdirectory(docs)
endif ()

if (RGRI_TESTS)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif ()

if (RGRI_PACKAGING)
    add_subdirectory(packaging)
endif ()
