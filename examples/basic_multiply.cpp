#include <iostream>
#include <grb/grb.hpp>
#include <iomanip>
#include <algorithm>

template <typename MatrixType>
void print_matrix(MatrixType& matrix) {
  for (auto elem : matrix) {
    float value = elem;
    grb::index_t idx = elem;
    std::cout << "(" << std::setw(2) << idx[0] << ", " << std::setw(2) << idx[1] << "): ";
    std::cout << std::setprecision(3) << std::setw(5) << value << std::endl;
  }
}

int main(int argc, char** argv) {
  grb::matrix<float, int> a("data/chesapeake.mtx");
  grb::matrix<float, int> b("data/chesapeake.mtx");

  auto c = grb::multiply(a, b);

  std::cout << grb::sum(c) << std::endl;

  return 0;
}
