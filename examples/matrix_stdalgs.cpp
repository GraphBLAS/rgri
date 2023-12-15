#include <algorithm>
#include <grb/grb.hpp>
#include <iostream>

int main(int argc, char** argv) {
  grb::matrix<float, size_t> matrix("chesapeake/chesapeake.mtx");

  std::cout << "Loaded " << matrix.shape()[0] << " x " << matrix.shape()[1]
            << " matrix with " << matrix.size() << " nonzeros." << std::endl;

  auto iterator = matrix.begin();

  float sum = 0;

  std::for_each(matrix.begin(), matrix.end(), [](auto matrix_ref) {
    auto&& [index, value] = matrix_ref;
    value += 2;
  });

  std::cout << "After adding two to each value:" << std::endl;

  grb::print(matrix);

  return 0;
}
