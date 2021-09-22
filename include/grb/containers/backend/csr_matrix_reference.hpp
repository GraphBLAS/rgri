#pragma once

#include <grb/util/index_t.hpp>

namespace grb {

template <typename MatrixType>
class csr_matrix_impl_reference {
public:
  using value_type = typename MatrixType::value_type;
  using index_type = typename MatrixType::index_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;

  csr_matrix_impl_reference(MatrixType& matrix, size_type row, index_type idx)
    : matrix_(matrix),
      row_(row),
      idx_(idx) {
  }

  csr_matrix_impl_reference& operator=(const csr_matrix_impl_reference& other) {
    value_reference() = other.value_reference();
    return *this;
  }

  csr_matrix_impl_reference& operator=(const value_type& value) {
    value_reference() = value;
    return *this;
  }

  decltype(auto) value_reference() noexcept {
    return matrix_.values_[idx_];
  }

  decltype(auto) value_reference() const noexcept {
    return matrix_.values_[idx_];
  }

  operator value_type() noexcept {
    return value_reference();
  }

  operator value_type() const noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
    return index();
  }

  decltype(auto) value() noexcept {
    return value_reference();
  }

  decltype(auto) value() const noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return {row_, (size_type) matrix_.colind_[idx_]};
  }

 private:
   size_type row_ = 0;
   index_type idx_ = 0;

   MatrixType& matrix_;
};

} // end grb

namespace std {

template <typename MatrixType>
void swap(grb::csr_matrix_impl_reference<MatrixType> a, grb::csr_matrix_impl_reference<MatrixType> b) {
  using value_type = typename grb::csr_matrix_impl_reference<MatrixType>::value_type;
  value_type value = a;
  a = b.value_reference();
  b = value;
}

} // end std
