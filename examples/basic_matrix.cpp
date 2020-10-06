#include <grb/grb.hpp>

int main(int argc, char** argv) {
  grb::matrix<float, int> matrix;

  auto shape = matrix.shape();

  return 0;
}
