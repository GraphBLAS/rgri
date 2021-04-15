
#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "matrix_reference.hpp"
#include "matrix_iterator.hpp"

namespace grb {

template <typename T,
          typename I = std::size_t,
          typename Allocator = std::allocator<T>>
class matrix {
public:
  using value_type = T;
  using index_type = I;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = matrix_iterator<matrix>;

  matrix() = default;
  matrix(const matrix&) = default;
  matrix& operator=(const matrix&) = default;

  matrix(std::string fname) {
    load_matrix(fname);
  }

  index_t shape() const noexcept {
    return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

  iterator begin() {
    return matrix_iterator<matrix>(*this, 0, rowptr_[0]);
  }

  iterator end() {
    return matrix_iterator<matrix>(*this, m_, rowptr_[m_]);
  }

  iterator find(index_t index);

  void load_matrix(const std::string& fname, bool one_indexed = true);

private:
  size_t m_ = 0;
  size_t n_ = 0;

  size_t nnz_ = 0;

  std::vector<value_type> values_;
  std::vector<index_type> rowptr_;
  std::vector<index_type> colind_;

  friend class matrix_iterator<matrix>;
  friend class matrix_reference<matrix>;
};

} // end grb

#include "matrix_impl.hpp"