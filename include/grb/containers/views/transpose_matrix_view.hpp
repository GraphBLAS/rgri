
#pragma once

#include "transpose_matrix_view_iterator.hpp"
#include "transpose_matrix_view_reference.hpp"

namespace grb {

template <typename MatrixType>
class transpose_matrix_view {
public:
  using value_type = typename MatrixType::value_type;
  using index_type = typename MatrixType::index_type;
  using allocator_type = typename MatrixType::allocator_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;

  using backend_iterator = typename MatrixType::iterator;
  using backend_const_iterator = typename MatrixType::const_iterator;

  using iterator = transpose_matrix_view_iterator<backend_iterator>;
  using const_iterator = const_transpose_matrix_view_iterator<backend_const_iterator>;

  using reference = typename MatrixType::reference;
  using const_reference = typename MatrixType::const_reference;

  using tuples_type = typename MatrixType::tuples_type;

  using hint_type = typename MatrixType::hint_type;

  transpose_matrix_view(MatrixType& matrix) : matrix_(matrix) {}

  index_t shape() const noexcept {
  	return {matrix_.shape()[1], matrix_.shape()[0]};
  }

  size_type size() const noexcept {
  	return matrix_.size();
  }

  iterator begin() {
    return iterator(matrix_.begin());
  }

  iterator end() {
    return iterator(matrix_.end());
  }

  const_iterator begin() const {
    return const_iterator(matrix_.begin());
  }

  const_iterator end() const {
    return const_iterator(matrix_.end());
  }

  index_t transpose_index(index_t index) const noexcept {
    return index_t{index[1], index[0]};
  }

  iterator find(index_t index) {
    return iterator(matrix_.find(transpose_index(index)));
  }

  reference operator[](index_t index) {
    return reference(matrix_[transpose_index(index)]);
  }

  reference at(index_t index) {
    return reference(matrix_.at(transpose_index(index)));
  }

private:
	MatrixType& matrix_;
};

template <typename MatrixType>
transpose_matrix_view<MatrixType>
transpose(MatrixType& matrix) {
	return transpose_matrix_view<MatrixType>(matrix);
}

} // end grb