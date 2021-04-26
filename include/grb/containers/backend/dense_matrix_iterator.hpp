
#pragma once

#include <iterator>

namespace grb {

template <typename MatrixType>
class dense_matrix_impl_iterator {
public:
  using value_type = typename MatrixType::value_type;
  using index_type = typename MatrixType::index_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;
  using reference = typename MatrixType::reference;
  using pointer = dense_matrix_impl_iterator;

  using iterator_category = std::random_access_iterator_tag;


  dense_matrix_impl_iterator(MatrixType& matrix) : matrix_(matrix) {}

  dense_matrix_impl_iterator(MatrixType& matrix, size_type idx)
    : matrix_(matrix), idx_(idx) {}

  dense_matrix_impl_iterator& operator=(const dense_matrix_impl_iterator& other) {
    idx_ = other.idx_;
    return *this;
  }

  dense_matrix_impl_iterator& operator++() {
    increment();
    return *this;
  }

  dense_matrix_impl_iterator& operator--() {
    decrement();
    return *this;
  }

  bool operator<(const dense_matrix_impl_iterator& other) {
    return idx_ < other.idx_;
  }

  dense_matrix_impl_iterator operator+(const difference_type offset) {
    return dense_matrix_impl_iterator(matrix_, idx_ + offset);
  }

  dense_matrix_impl_iterator operator-(const difference_type offset) {
    return dense_matrix_impl_iterator(matrix_, idx_) + (-offset);
  }

  difference_type operator-(const dense_matrix_impl_iterator& other) const noexcept {
    return difference_type(idx_) - difference_type(other.idx_);
  }

  void increment() {
    idx_++;
  }

  void decrement() {
    idx_--;
  }

  bool operator==(const dense_matrix_impl_iterator& other) const noexcept {
    return &matrix_ == &other.matrix_ && idx_ == other.idx_;
  }

  bool operator!=(const dense_matrix_impl_iterator& other) const noexcept {
    return !(*this == other);
  }

  reference operator*() noexcept {
    return reference(matrix_, idx_);
  }

private:
  size_type idx_ = 0;

  MatrixType& matrix_;
};

} // end grb
