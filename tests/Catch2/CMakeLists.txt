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

enable_testing()

add_executable(tests tests.cpp)

target_link_libraries(tests PRIVATE rgri Catch2::Catch2WithMain)
