#include <iostream>
#include <grb/grb.hpp>

int main(int argc, char** argv) {
  grb::matrix<float, int> matrix("chesapeake.mtx");

  grb::index_t shape = matrix.shape();

  printf("Loaded a %lu x %lu matrix with %lu nonzeros.\n",
         shape[0], shape[1], matrix.size());

  return 0;
}
