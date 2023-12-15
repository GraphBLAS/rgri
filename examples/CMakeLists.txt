function(download_data url file_name)
  set(DATASET_ARCHIVE ${CMAKE_BINARY_DIR}/data/${file_name})

  file(DOWNLOAD
       ${url}
       ${DATASET_ARCHIVE})

 string(REPLACE
        ".tar.gz" ""
        DATSET_DIR
        ${DATASET_ARCHIVE})

 file(ARCHIVE_EXTRACT INPUT
      ${DATASET_ARCHIVE}
      ${DATASET_DIR})
endfunction()

download_data(https://suitesparse-collection-website.herokuapp.com/MM/DIMACS10/chesapeake.tar.gz
              chesapeake.tar.gz)

function(add_example example_name)
  add_executable(${example_name} ${example_name}.cpp)
  target_link_libraries(${example_name} rgri fmt)
endfunction()

add_example(assign_example)
add_example(basic_ewise)
add_example(basic_matrix)
add_example(basic_multiply)
add_example(ewise_example)
add_example(matrix_stdalgs)
add_example(multiply_example)
add_example(views_example)

add_subdirectory(algorithms)
