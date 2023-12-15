
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
      : matrix_(matrix), idx_(idx) {
    fast_forward();
  }

  dense_matrix_impl_iterator&
  operator=(const dense_matrix_impl_iterator& other) {
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

  // TODO: these methods can be optimized.
  dense_matrix_impl_iterator operator+(const difference_type offset) {
    dense_matrix_impl_iterator iter(*this);
    if (offset > 0) {
      for (size_t i = 0; i < offset; i++) {
        ++iter;
      }
    }
    if (offset < 0) {
      for (difference_type i = 0; i >= offset; i--) {
        --iter;
      }
    }
    return iter;
  }

  dense_matrix_impl_iterator operator-(const difference_type offset) {
    return dense_matrix_impl_iterator(matrix_, idx_) + (-offset);
  }

  // TODO: this can be optimized.
  difference_type
  operator-(const dense_matrix_impl_iterator& other) const noexcept {
    auto min = std::min(*this, other);
    auto max = std::max(*this, other);
    size_t i = 0;
    while (min != max) {
      ++min;
      i++;
    }
    return difference_type(idx_) - difference_type(other.idx_);
  }

  void increment() {
    idx_++;
    fast_forward();
  }

  void decrement() {
    idx_--;
    fast_backward();
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

  void fast_forward() noexcept {
    while (idx_ < matrix_.m() * matrix_.n() && !matrix_.flags_[idx_]) {
      idx_++;
    }
  }

  void fast_backward() noexcept {
    while (idx_ > 0 && !matrix_.flags_[idx_]) {
      idx_--;
    }
  }

private:
  size_type idx_ = 0;

  MatrixType& matrix_;
};

} // namespace grb
