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

template <typename T>
void print_vector(const std::vector<T>& v) {
	size_t index = 0;
  for (auto& elem : v) {
    std::cout << std::setw(4) << index << ": " << std::setw(5) << elem << std::endl;
    index++;
  }
}

template <typename T, typename I>
void spmv(grb::matrix<T, I>& a, grb::vector<float>& b, grb::vector<float>& c) {
	for (auto elem : a) {
		grb::index_t idx = elem;
		T value = elem;

		c[idx[0]] += value * b[idx[1]];
	}
}

int main(int argc, char** argv) {
  grb::matrix<float, int> matrix("data/chesapeake.mtx");

  grb::vector<float> v(matrix.shape()[1], 1);
  grb::vector<float> result(matrix.shape()[0], 0);

  spmv(matrix, v, result);

  print_vector(result);

  return 0;
}
