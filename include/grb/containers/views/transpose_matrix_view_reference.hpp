#pragma once

namespace grb {

template <typename BackendReferenceType>
class const_transpose_matrix_view_reference;

template <typename BackendReferenceType>
class transpose_matrix_view_reference {
public:
  using value_type = typename BackendReferenceType::value_type;
  using index_type = typename BackendReferenceType::index_type;
  using size_type = typename BackendReferenceType::size_type;
  using difference_type = typename BackendReferenceType::difference_type;

  using backend_reference = BackendReferenceType;

  using reference = transpose_matrix_view_reference;
  using const_reference = const_transpose_matrix_view_reference<backend_reference>;

  transpose_matrix_view_reference(backend_reference ref) : ref_(ref) {}

  transpose_matrix_view_reference& operator=(const transpose_matrix_view_reference& other) {
    ref_ = other.ref_;
    return *this;
  }

  transpose_matrix_view_reference& operator=(const value_type& value) {
    ref_ = value;
    return *this;
  }

  value_type& value_reference() noexcept {
    return ref_.value_reference();
  }

  const value_type& value_reference() const noexcept {
    return ref_;
  }

  operator value_type() noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
  	auto [i, j] = ref_.index();
    return {j, i};
  }

  value_type& value() noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return *this;
  }

private:

  backend_reference ref_;

  friend const_reference;
};

template <typename BackendReferenceType>
class const_transpose_matrix_view_reference {
public:
  using value_type = typename BackendReferenceType::value_type;
  using index_type = typename BackendReferenceType::index_type;
  using size_type = typename BackendReferenceType::size_type;
  using difference_type = typename BackendReferenceType::difference_type;

  using backend_reference = BackendReferenceType;

  using reference = transpose_matrix_view_reference<backend_reference>;
  using const_reference = const_transpose_matrix_view_reference;

  const_transpose_matrix_view_reference(backend_reference ref) : ref_(ref) {}

  const_transpose_matrix_view_reference& operator=(const const_transpose_matrix_view_reference& other) {
    ref_ = other.ref_;
    return *this;
  }

  const_transpose_matrix_view_reference& operator=(const value_type& value) {
    ref_ = value;
    return *this;
  }

  value_type value_reference() const noexcept {
    return ref_.value_reference();
  }

  operator value_type() const noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
  	auto [i, j] = ref_.index();
    return {j, i};
  }

  value_type value() const noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return *this;
  }

private:

  backend_reference ref_;

  friend reference;
};

} // end grb

namespace std {

template <typename MatrixType>
void swap(grb::transpose_matrix_view_reference<MatrixType> a, grb::transpose_matrix_view_reference<MatrixType> b) {
  using value_type = typename grb::transpose_matrix_view_reference<MatrixType>::value_type;
  value_type value = a;
  a = b.value_reference();
  b = value;
}

} // end std
