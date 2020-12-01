#pragma once

namespace grb {

template <typename MatrixType>
class matrix_reference {
public:
  using value_type = typename MatrixType::value_type;
  using index_type = typename MatrixType::index_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;

  matrix_reference(MatrixType& matrix, size_type row, index_type idx)
    : matrix_(matrix),
      row_(row),
      idx_(idx) {
  }

  matrix_reference& operator=(const matrix_reference& other) {
    value_reference() = other.value_reference();
    return *this;
  }

  matrix_reference& operator=(const value_type& value) {
    value_reference() = value;
    return *this;
  }

  value_type& value_reference() noexcept {
    return matrix_.values_[idx_];
  }

  const value_type& value_reference() const noexcept {
    return matrix_.values_[idx_];
  }

  operator value_type() const noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
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
void swap(grb::matrix_reference<MatrixType> a, grb::matrix_reference<MatrixType> b) {
  using value_type = typename grb::matrix_reference<MatrixType>::value_type;
  value_type value = a;
  a = b.value_reference();
  b = value;
}

} // end std
