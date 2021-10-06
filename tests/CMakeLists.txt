find_package(Catch2 REQUIRED)
include(Catch)

add_executable(RGRI_tests 
    tests.cpp
    algorithms_1.hpp
    matrix_methods_1.hpp
    matrix_methods_2.hpp
    matrix_methods_3.hpp
)
target_link_libraries(RGRI_tests PRIVATE RGRI::RGRI Catch2::Catch2)

catch_discover_tests(RGRI_tests WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
