#include <iostream>
#include <grb/grb.hpp>

int main(int argc, char** argv) {
  grb::matrix<float, int> matrix("data/chesapeake.mtx");

  grb::index_t shape = matrix.shape();

  printf("Loaded a %lu x %lu matrix with %lu nonzeros.\n",
         shape[0], shape[1], matrix.size());

  printf("Assigning each matrix element to 12.\n");
  for (auto elem : matrix) {
    elem = 12;
  }

  float total_sum = 0;
  for (auto elem : matrix) {
    float value = elem;
    grb::index_t idx = elem;
    total_sum += value;
  }

  printf("Total sum was %f, average value %f\n", total_sum, total_sum / matrix.size());

  return 0;
}
