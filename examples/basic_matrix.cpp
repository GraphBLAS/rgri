#include <grb/grb.hpp>
#include <cstdio>

int main(int argc, char** argv) {
  grb::matrix<float> matrix("data/chesapeake.mtx");

  size_t m = matrix.shape()[0];
  size_t n = matrix.shape()[1];
  size_t nnz = matrix.size();

  printf("Read in a file that is %lu x %lu dimensions with %lu stored values.\n",
         m, n, nnz);

  grb::print(matrix, "chesapeake");

  float sum_values = 0;
  size_t sum_indices = 0;
  for (auto&& [index, value] : matrix) {
    auto&& [i, j] = index;

    sum_values += value;
    sum_indices += i + j;
  }

  return 0;
}
