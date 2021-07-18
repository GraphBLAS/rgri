set(examples
    basic_matrix.cpp
    basic_multiply.cpp
    test.cpp
    test_basic.cpp
    test_matrix.cpp
    test_mxv.cpp
)

foreach (source IN LISTS examples)
  get_filename_component(name "${source}" NAME_WE)

  add_executable(RGRI_${name} ${source})
  target_link_libraries(RGRI_${name} PRIVATE RGRI::RGRI)

  add_test(
      NAME ${name}
      COMMAND RGRI_${name}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
endforeach ()
