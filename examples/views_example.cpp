#include <grb/grb.hpp>

int main(int argc, char** argv) {
  grb::matrix<float> matrix({10, 10});

  matrix[{2, 3}] = 12;
  matrix[{7, 1}] = 12;
  matrix[{5, 5}] = 12;
  matrix[{2, 7}] = 12;
  matrix[{9, 6}] = 12;

  grb::print(matrix, "original matrix");

  auto matrix_t = grb::transpose(matrix);

  grb::print(matrix_t, "matrix transposed");

  auto matrix_tr = grb::transform(matrix, [](auto&& entry) {
                                            auto&& [idx, value] = entry;
                                            auto&& [i, j] = idx;
                                            return i + j / 10.0f;
                                          });

  grb::print(matrix_tr, "matrix transformed");

  return 0;
}