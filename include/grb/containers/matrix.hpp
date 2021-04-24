
#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "matrix_reference.hpp"
#include "matrix_iterator.hpp"
#include "backend/csr_matrix.hpp"

namespace grb {

template <typename T,
          typename I = std::size_t,
          typename Allocator = std::allocator<T>>
class matrix {
public:
  using value_type = T;
  using index_type = I;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using backend_type = csr_matrix_impl_<value_type, index_type, allocator_type>;
  using backend_reference = typename backend_type::reference;
  using backend_iterator = typename backend_type::iterator;

  using iterator = matrix_iterator<backend_iterator>;
  using reference = matrix_reference<backend_reference>;

  using tuples_type = std::vector<std::tuple<value_type, index_type, index_type>>;

  matrix() = default;
  matrix(const matrix&) = default;
  matrix& operator=(const matrix&) = default;

  matrix(index_t dimensions) : backend_matrix_(dimensions) {}

  matrix(std::string fname) : backend_matrix_(read_MatrixMarket<T, I>(fname)) {}

  index_t shape() const noexcept {
    return {backend_matrix_.m(), backend_matrix_.n()};
  }

  size_type size() const noexcept {
    return backend_matrix_.nnz();
  }

  iterator begin() {
    return iterator(backend_matrix_.begin());
  }

  iterator end() {
    return iterator(backend_matrix_.end());
  }

  iterator find(index_t index) {
    return backend_matrix_.find(index);
  }

  reference operator[](index_t index) {
    auto iter = find(index);
    if (iter == end()) {
      tuples_type new_tuple = {{value_type{}, index[0], index[1]}};
      backend_matrix_.insert_tuples(new_tuple);
      iter = find(index);
    }
    return *iter;
  }

private:
  backend_type backend_matrix_;

  friend iterator;
  friend reference;
};

} // end grb

#include "matrix_impl.hpp"