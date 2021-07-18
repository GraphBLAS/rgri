include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(RGRI_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/cmake/RGRI"
    CACHE PATH "Install directory for CMake package files")

install(
    TARGETS RGRI
    EXPORT RGRI
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    EXPORT RGRI
    DESTINATION "${RGRI_INSTALL_CMAKEDIR}"
    NAMESPACE RGRI::
    FILE RGRIConfig.cmake
    COMPONENT RGRI_Development
)

install(
    DIRECTORY "${RGRI_SOURCE_DIR}/include/grb"
    TYPE INCLUDE
    COMPONENT RGRI_Development
)

write_basic_package_version_file(
    RGRIConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/RGRIConfigVersion.cmake"
    DESTINATION "${RGRI_INSTALL_CMAKEDIR}"
    COMPONENT RGRI_Development
)

if (RGRI_DOCS)
    install(
        DIRECTORY "${RGRI_BINARY_DIR}/docs/sphinx/"
        TYPE DOC
        COMPONENT RGRI_Documentation
    )
endif ()

