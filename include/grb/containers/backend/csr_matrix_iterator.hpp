
#pragma once

#include <iterator>

namespace grb {

template <typename MatrixType>
class csr_matrix_impl_iterator {
public:
  using value_type = typename MatrixType::value_type;
  using index_type = typename MatrixType::index_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;
  using reference = typename MatrixType::reference;
  using pointer = csr_matrix_impl_iterator;

  using iterator_category = std::random_access_iterator_tag;


  csr_matrix_impl_iterator(MatrixType& matrix) : matrix_(matrix) {
    fast_forward();
  }

  csr_matrix_impl_iterator(MatrixType& matrix, size_type row, index_type idx)
    : matrix_(matrix),
      row_(row),
      idx_(idx) {
    fast_forward();
  }

  csr_matrix_impl_iterator& operator=(const csr_matrix_impl_iterator& other) {
    row_ = other.row_;
    idx_ = other.idx_;
    return *this;
  }

  csr_matrix_impl_iterator& operator++() {
    increment();
    return *this;
  }

  csr_matrix_impl_iterator& operator--() {
    decrement();
    return *this;
  }

  bool operator<(const csr_matrix_impl_iterator& other) {
    return idx_ < other.idx_;
  }

  csr_matrix_impl_iterator operator+(const difference_type offset) {
    csr_matrix_impl_iterator other(matrix_, row_, idx_);
    difference_type n = offset;
    if (n > 0) {
      while (n > 0) {
        other.increment();
        n--;
      }
    }
    if (n < 0) {
      while (n < 0) {
        other.decrement();
        n++;
      }
    }
    return other;
  }

  csr_matrix_impl_iterator operator-(const difference_type offset) {
    csr_matrix_impl_iterator other(matrix_, row_, idx_);
    return other + (-offset);
  }

  difference_type operator-(const csr_matrix_impl_iterator& other) const noexcept {
    return difference_type(idx_) - difference_type(other.idx_);
  }

  void increment() {
    idx_++;
    if (idx_ >= matrix_.rowptr_[row_+1]) {
      row_++;
      idx_ = matrix_.rowptr_[row_];
    }
  }

  void decrement() {
    if (idx_ > 0) {
      idx_--;
      if (idx_ < matrix_.rowptr_[row_]) {
        row_--;
        idx_ = matrix_.rowptr_[row_+1]-1;
      }
    }
  }

  bool operator==(const csr_matrix_impl_iterator& other) const noexcept {
    return &matrix_ == &other.matrix_ && row_ == other.row_ && idx_ == other.idx_;
  }

  bool operator!=(const csr_matrix_impl_iterator& other) const noexcept {
    return !(*this == other);
  }

  reference operator*() noexcept {
    return reference(matrix_, row_, idx_);
  }

  void fast_forward() noexcept {
    while (matrix_.rowptr_[row_] == matrix_.rowptr_[row_+1] && row_ < matrix_.m_) {
        row_++;
    }
  }

 private:
   index_type row_ = 0;
   index_type idx_ = 0;

   MatrixType& matrix_;
};

} // end grb
