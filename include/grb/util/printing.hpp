#pragma once

#include <iostream>
#include <iomanip>
#include <string>

namespace grb {

/// Print the GraphBLAS matrix object `matrix` to standard output,
/// optionally adding the string `label` to label the output.
template <typename MatrixType>
void print(MatrixType&& matrix, std::string label = "") {
  std::cout << matrix.shape()[0] << " x " << matrix.shape()[1] <<
               " matrix with " << matrix.size() << " nnz";
  if (label != "") {
    std::cout << " \"" << label << "\"";
  }
  std::cout << std::endl;

  for (const auto& tuple : matrix) {
    auto [i, j] = tuple.index();
    auto value = tuple.value();

    std::cout << "(" << i << ", " << j << "): " << value << std::endl;
  }
}

};