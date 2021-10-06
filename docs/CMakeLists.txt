#
# Doxygen
#

find_package(Doxygen REQUIRED doxygen)

# Non-default Doxygen settings. Review carefully.
set(DOXYGEN_COLS_IN_ALPHA_INDEX 5)
set(DOXYGEN_CASE_SENSE_NAMES NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_HTML_DYNAMIC_MENUS YES)
set(DOXYGEN_OPTIMIZE_OUTPUT_SLICE NO)
set(DOXYGEN_OUTPUT_TEXT_DIRECTION None)
set(DOXYGEN_XML_NS_MEMB_FILE_SCOPE NO)
set(DOXYGEN_TOC_INCLUDE_HEADINGS 0)

# Doxygen inputs are exactly the source files for RGRI (which is header-only)
get_property(rgri_headers TARGET RGRI PROPERTY SOURCES)

doxygen_add_docs(
    docs-doxygen
    ${rgri_headers}
    ALL
    USE_STAMP_FILE
    WORKING_DIRECTORY "${RGRI_SOURCE_DIR}/include"
)

#
# Sphinx
#

find_program(SPHINX_EXECUTABLE sphinx-build REQUIRED)

add_custom_command(
    OUTPUT sphinx/index.html
    COMMAND ${SPHINX_EXECUTABLE} -b html -Dbreathe_projects.RGRI=${CMAKE_CURRENT_BINARY_DIR}/xml ${CMAKE_CURRENT_SOURCE_DIR} sphinx
    DEPENDS
        index.rst
        conf.py
        # Sphinx uses Doxygen XML output, so re-run command if stamp file updated
        ${CMAKE_CURRENT_BINARY_DIR}/docs-doxygen.stamp
)

add_custom_target(docs-sphinx ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sphinx/index.html)
add_dependencies(docs-sphinx docs-doxygen)

#
# Meta-target
#

add_custom_target(docs ALL)
add_dependencies(docs docs-sphinx docs-doxygen)

