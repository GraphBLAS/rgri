#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/csr_matrix.hpp>
#include <iostream>

int main(int argc, char** argv) {
  auto data = grb::read_MatrixMarket<float, size_t>("data/chesapeake.mtx");

  for (const auto& tuple : data.matrix_) {
    std::cout << "(" << std::get<1>(tuple) << ", " << std::get<2>(tuple) << "): " << std::get<0>(tuple) << std::endl;
  }

  grb::csr_matrix_impl_<float, size_t, std::allocator<float>> matrix(data);

  return 0;
}
