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

  std::vector<float> vec(matrix.size(), 2);
  std::copy(vec.begin(), vec.end(), matrix.begin());

  total_sum = 0;
  for (auto elem : matrix) {
    float value = elem;
    grb::index_t idx = elem;
    total_sum += value;
  }

  printf("After std::copy, total sum was %f, average value %f\n", total_sum, total_sum / matrix.size());

  size_t n_elements = 0;
  for (auto elem : matrix) {
    n_elements++;
    elem = matrix.size() - n_elements;
  }

  printf("Before sorting:\n");
  print_matrix(matrix);

  auto a = *matrix.begin();
  auto b = *(matrix.begin() + 1);

  std::sort(matrix.begin(), matrix.end(), [](auto a, auto b) {
                                            float a_v = a;
                                            float b_v = b;
                                            return a_v < b_v;
                                          });

  printf("After sorting:\n");
  print_matrix(matrix);

  return 0;
}
