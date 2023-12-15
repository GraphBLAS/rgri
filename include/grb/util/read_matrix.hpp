#pragma once

#include <grb/containers/matrix.hpp>

namespace grb {

template <typename T, std::integral I = std::size_t,
          typename Hint = grb::sparse, typename Allocator = std::allocator<T>>
grb::matrix<T, I, Hint, Allocator> read_matrix(std::string file_path) {
  return grb::matrix<T, I, Hint, Allocator>(file_path);
}

} // namespace grb
