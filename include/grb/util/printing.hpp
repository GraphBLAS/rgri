#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <grb/containers/matrix.hpp>
#include <grb/containers/vector.hpp>
#include <grb/containers/views/transpose_matrix.hpp>

namespace grb {

/// Print the GraphBLAS matrix object `matrix` to standard output,
/// optionally adding the string `label` to label the output.
template <typename... Args>
void print(const grb::matrix<Args...>& matrix, std::string label = "") {
  std::cout << matrix.shape()[0] << " x " << matrix.shape()[1] <<
               " matrix with " << matrix.size() << " stored values";
  if (label != "") {
    std::cout << " \"" << label << "\"";
  }
  std::cout << std::endl;

  for (auto&& tuple : matrix) {
    auto [i, j] = tuple.index();
    auto value = tuple.value();

    std::cout << "(" << i << ", " << j << "): " << value << std::endl;
  }
}

template <typename... Args>
void print(const grb::transpose_matrix_view<Args...>& matrix, std::string label = "") {
  std::cout << matrix.shape()[0] << " x " << matrix.shape()[1] <<
               " matrix with " << matrix.size() << " stored values";
  if (label != "") {
    std::cout << " \"" << label << "\"";
  }
  std::cout << std::endl;

  for (auto&& tuple : matrix) {
    auto [i, j] = tuple.index();
    auto value = tuple.value();

    std::cout << "(" << i << ", " << j << "): " << value << std::endl;
  }
}

template <typename... Args>
void print(const grb::transform_matrix_view<Args...>& matrix, std::string label = "") {
  std::cout << matrix.shape()[0] << " x " << matrix.shape()[1] <<
               " matrix with " << matrix.size() << " stored values";
  if (label != "") {
    std::cout << " \"" << label << "\"";
  }
  std::cout << std::endl;

  for (auto&& tuple : matrix) {
    auto [i, j] = tuple.index();
    auto value = tuple.value();

    std::cout << "(" << i << ", " << j << "): " << value << std::endl;
  }
}

/// Print the GraphBLAS matrix object `matrix` to standard output,
/// optionally adding the string `label` to label the output.
template <typename... Args>
void print(const grb::vector<Args...>& vector, std::string label = "") {
  std::cout << vector.shape()
            << " dimension vector with " << vector.size() << " stored values";
  if (label != "") {
    std::cout << " \"" << label << "\"";
  }
  std::cout << std::endl;

  for (auto&& [i, v] : vector) {
    std::cout << "(" << i << "): " << v << std::endl;
  }
}

};