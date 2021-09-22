#pragma once

namespace grb {

template <typename BackendReferenceType>
class const_matrix_reference;

template <typename BackendReferenceType>
class matrix_reference {
public:
  using value_type = typename BackendReferenceType::value_type;
  using index_type = typename BackendReferenceType::index_type;
  using size_type = typename BackendReferenceType::size_type;
  using difference_type = typename BackendReferenceType::difference_type;

  using backend_reference = BackendReferenceType;

  using reference = matrix_reference;
  using const_reference = const_matrix_reference<backend_reference>;

  using value_reference_type = decltype(std::declval<backend_reference>().value());

  matrix_reference(backend_reference ref) : ref_(ref) {}

  matrix_reference& operator=(const matrix_reference& other) {
    ref_ = other.ref_;
    return *this;
  }

  matrix_reference& operator=(const value_type& value) {
    ref_ = value;
    return *this;
  }

  value_reference_type value_reference() noexcept {
    return ref_.value_reference();
  }

  operator value_type() noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
    return ref_;
  }

  value_reference_type value() noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return *this;
  }

  operator std::tuple<index_type, index_type, value_reference_type>() noexcept {
    return {index()[0], index()[1], value()};
  }

  template <std::size_t Index>
  decltype(auto) get() {
    if constexpr(Index == 0) { return index()[0]; }
    if constexpr(Index == 1) { return index()[1]; }
    if constexpr(Index == 2) { return value(); }
  }

private:

  backend_reference ref_;

  friend const_reference;
};

template <typename BackendReferenceType>
class const_matrix_reference {
public:
  using value_type = typename BackendReferenceType::value_type;
  using index_type = typename BackendReferenceType::index_type;
  using size_type = typename BackendReferenceType::size_type;
  using difference_type = typename BackendReferenceType::difference_type;

  using backend_reference = BackendReferenceType;

  using reference = matrix_reference<backend_reference>;
  using const_reference = const_matrix_reference;

  const_matrix_reference(backend_reference ref) : ref_(ref) {}

  const_matrix_reference& operator=(const const_matrix_reference& other) {
    ref_ = other.ref_;
    return *this;
  }

  value_type value_reference() const noexcept {
    return ref_;
  }

  operator value_type() const noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
    return ref_;
  }

  value_type value() const noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return *this;
  }

  operator std::tuple<index_type, index_type, value_type>() const noexcept {
    return {index()[0], index()[1], value()};
  }

  template <std::size_t Index>
  decltype(auto) get() const noexcept {
    if constexpr(Index == 0) { return index()[0]; }
    if constexpr(Index == 1) { return index()[1]; }
    if constexpr(Index == 2) { return value(); }
  }

private:

  backend_reference ref_;

  friend reference;
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

template <std::size_t Index, typename BackendReferenceType>
struct tuple_element<Index, grb::matrix_reference<BackendReferenceType>>
  : tuple_element<Index, std::tuple<size_t, size_t, typename grb::matrix_reference<BackendReferenceType>::value_type>>
{
};

template<typename BackendReferenceType>
struct tuple_size<grb::matrix_reference<BackendReferenceType>>
    : integral_constant<size_t, 3> {};

template <std::size_t Index, typename BackendReferenceType>
struct tuple_element<Index, grb::const_matrix_reference<BackendReferenceType>>
  : tuple_element<Index, std::tuple<size_t, size_t, typename grb::const_matrix_reference<BackendReferenceType>::value_type>>
{
};

template<typename BackendReferenceType>
struct tuple_size<grb::const_matrix_reference<BackendReferenceType>>
    : integral_constant<size_t, 3> {};

} // end std
