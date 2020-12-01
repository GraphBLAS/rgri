
#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace grb {

template <typename T,
          typename I = std::uint64_t,
          typename Allocator = std::allocator<T>>
class matrix {
public:
  using value_type = T;
  using index_type = I;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  matrix() = default;
  // matrix(const matrix&) = default;
  // matrix& operator=(const matrix&) = default;

  matrix(const std::string& fname) {
    load_matrix(fname);
  }

  index_t shape() const noexcept {
    return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

  void load_matrix(const std::string& fname, bool one_indexed = true);

private:
  size_t m_ = 0;
  size_t n_ = 0;

  size_t nnz_ = 0;

  std::vector<value_type> values_;
  std::vector<index_type> rowptr_;
  std::vector<index_type> colind_;
};

} // end grb

#include "matrix_impl.hpp"