#pragma once

namespace grb {

template <typename BackendReferenceType,
          typename ValueTransform,
          typename IndexTransform>
class const_transform_matrix_view_reference;

template <typename BackendReferenceType,
          typename ValueTransform,
          typename IndexTransform>
class transform_matrix_view_reference {
public:
  using backend_value_type = typename BackendReferenceType::value_type;

  using value_transform_type = ValueTransform;
  using index_transform_type = IndexTransform;

  using value_type = decltype(std::declval<ValueTransform>()(std::declval<backend_value_type>()));

  using index_type = typename BackendReferenceType::index_type;
  using size_type = typename BackendReferenceType::size_type;
  using difference_type = typename BackendReferenceType::difference_type;

  using backend_reference = BackendReferenceType;

  using reference = transform_matrix_view_reference;
  using const_reference = const_transform_matrix_view_reference<backend_reference, value_transform_type, index_transform_type>;

  transform_matrix_view_reference(backend_reference ref,
                                  const value_transform_type& value_transform,
                                  const index_transform_type& index_transform)
    : ref_(ref),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  transform_matrix_view_reference& operator=(const transform_matrix_view_reference& other) {
    ref_ = other.ref_;
    value_transform_ = other.value_transform_;
    index_transform_ = other.index_transform_;
    return *this;
  }

  value_type value_reference() const noexcept {
    return value_transform_(ref_.value_reference());
  }

  operator value_type() const noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
  	return index_transform_(ref_.index());
  }

  value_type value() const noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return *this;
  }

private:

  backend_reference ref_;
  const value_transform_type& value_transform_;
  const index_transform_type& index_transform_;

  friend const_reference;
};

template <typename BackendReferenceType,
          typename ValueTransform,
          typename IndexTransform>
class const_transform_matrix_view_reference {
public:
  using backend_value_type = typename BackendReferenceType::value_type;

  using value_type = decltype(std::declval<ValueTransform>()(std::declval<backend_value_type>()));

  using value_transform_type = ValueTransform;
  using index_transform_type = IndexTransform;

  using index_type = typename BackendReferenceType::index_type;
  using size_type = typename BackendReferenceType::size_type;
  using difference_type = typename BackendReferenceType::difference_type;

  using backend_reference = BackendReferenceType;

  using reference = transform_matrix_view_reference<backend_reference, value_transform_type, index_transform_type>;
  using const_reference = const_transform_matrix_view_reference;

  const_transform_matrix_view_reference(backend_reference ref,
                                        const value_transform_type& value_transform,
                                        const index_transform_type& index_transform)
    : ref_(ref),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  const_transform_matrix_view_reference& operator=(const const_transform_matrix_view_reference& other) {
    ref_ = other.ref_;
    value_transform_ = other.index_transform_;
    index_transform_ = other.value_transform_;
    return *this;
  }

  value_type value_reference() const noexcept {
    return value_transform_(ref_.value_reference());
  }

  operator value_type() const noexcept {
    return value_reference();
  }

  operator index_t() const noexcept {
    return index_transform_(ref_.index());
  }

  value_type value() const noexcept {
    return value_reference();
  }

  index_t index() const noexcept {
    return *this;
  }

private:

  backend_reference ref_;
  const value_transform_type& value_transform_;
  const index_transform_type& index_transform_;

  friend reference;
};

} // end grb

namespace std {

template <typename MatrixType, typename ValueTransform, typename IndexTransform>
void swap(grb::transform_matrix_view_reference<MatrixType, ValueTransform, IndexTransform> a, grb::transform_matrix_view_reference<MatrixType, ValueTransform, IndexTransform> b) {
  using value_type = typename grb::transform_matrix_view_reference<MatrixType, ValueTransform, IndexTransform>::value_type;
  value_type value = a;
  a = b.value_reference();
  b = value;
}

} // end std
