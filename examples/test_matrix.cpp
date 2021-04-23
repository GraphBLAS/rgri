#include <grb/grb.hpp>
#include <iostream>
#include <algorithm>

int main(int argc, char** argv) {
  grb::matrix<float, size_t> matrix("data/chesapeake.mtx");

  std::cout << "Loaded " << matrix.shape()[0] << " x " << matrix.shape()[1] << " matrix with " << matrix.size() << " nonzeros." << std::endl;

  auto iterator = matrix.begin();

  size_t i = 0;
  for (auto iterator = matrix.begin(); iterator != matrix.end(); ++iterator) {
  	printf("%lu\n", i++);
  }

  for (auto element : matrix) {
    float value = element;
    grb::index_t idx = element;
    std::cout << "(" << idx[0] << ", " << idx[1] << "): " << value << std::endl;
  }

  std::for_each(matrix.begin(), matrix.end(), [](auto ref) {
  	float& v = ref.value();
  	v += 2;
  });

  for (auto element : matrix) {
    float value = element;
    grb::index_t idx = element;
    std::cout << "(" << idx[0] << ", " << idx[1] << "): " << value << std::endl;
  }

  return 0;
}
