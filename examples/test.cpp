#include <grb/grb.hpp>

int main(int argc, char** argv) {
	grb::matrix<float, int, grb::sparse> matrix({1024, 1024});

  matrix[{12, 13}] = 12;
  matrix[{34, 12}] = 12;
  matrix[{0, 2}] = 0.1;
  matrix[{123, 123}] = 14;
  matrix[{141, 191}] = 12;

  grb::print(matrix, "Original matrix:");

  auto transpose_matrix = grb::transpose(matrix);

  grb::print(transpose_matrix, "Transposed matrix:");

  return 0;
}
