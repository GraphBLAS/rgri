cmake_minimum_required(VERSION 3.0.0)
project(cpp_doxygen_sphinx VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_FLAGS "-O3 -std=gnu++17")

include_directories("../include")
add_library(cpp_doxygen_sphinx "src/cpp_doxygen_sphinx.cpp")

set(PROJECT_INCLUDE_DIR "../include/grb")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(Doxygen)

if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs_doxygen/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
    add_custom_target( docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

