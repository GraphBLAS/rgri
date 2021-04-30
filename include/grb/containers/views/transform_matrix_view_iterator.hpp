
#pragma once

#include "transform_matrix_view_reference.hpp"

namespace grb {

template <typename BackendIteratorType, typename ValueTransform, typename IndexTransform>
class const_transform_matrix_view_iterator;

template <typename BackendIteratorType, typename ValueTransform, typename IndexTransform>
class transform_matrix_view_iterator {
public:
  using backend_value_type = typename BackendIteratorType::value_type;

  using value_transform_type = ValueTransform;
  using index_transform_type = IndexTransform;

  using value_type = decltype(std::declval<ValueTransform>()(std::declval<backend_value_type>()));

  using index_type = typename BackendIteratorType::index_type;
  using size_type = typename BackendIteratorType::size_type;
  using difference_type = typename BackendIteratorType::difference_type;

  using backend_iterator = BackendIteratorType;
  using backend_reference = typename BackendIteratorType::reference;

  using pointer = transform_matrix_view_iterator;

  using iterator = transform_matrix_view_iterator;
  using const_iterator = const_transform_matrix_view_iterator<backend_iterator, value_transform_type, index_transform_type>;

  using reference = transform_matrix_view_reference<backend_reference, value_transform_type, index_transform_type>;
  using const_reference = const_transform_matrix_view_reference<backend_reference, value_transform_type, index_transform_type>;

  using iterator_category = typename BackendIteratorType::iterator_category;

  transform_matrix_view_iterator(BackendIteratorType iterator,
                                 const ValueTransform& value_transform,
                                 const IndexTransform& index_transform)
    : iterator_(iterator),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  transform_matrix_view_iterator(const transform_matrix_view_iterator& other,
                                 const ValueTransform& value_transform,
                                 const IndexTransform& index_transform)
    : iterator_(other.iterator_),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  transform_matrix_view_iterator& operator=(const transform_matrix_view_iterator& other) {
    iterator_ = other.iterator_;
    value_transform_ = other.value_transform_;
    index_transform_ = other.index_transform_;
    return *this;
  }

  transform_matrix_view_iterator& operator++() {
    ++iterator_;
    return *this;
  }

  transform_matrix_view_iterator& operator--() {
    --iterator_;
    return *this;
  }

  bool operator<(const transform_matrix_view_iterator& other) {
    return iterator_ < other.iterator_;
  }

  transform_matrix_view_iterator operator+(const difference_type offset) {
    return transform_matrix_view_iterator(iterator_ + offset, value_transform_, index_transform_);
  }

  transform_matrix_view_iterator operator-(const difference_type offset) {
    return transform_matrix_view_iterator(iterator_ - offset, value_transform_, index_transform_);
  }

  difference_type operator-(const transform_matrix_view_iterator& other) const noexcept {
    return iterator_ - other.iterator_;
  }

  bool operator==(const transform_matrix_view_iterator& other) const noexcept {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const transform_matrix_view_iterator& other) const noexcept {
    return iterator_ != other.iterator_;
  }

  reference operator*() noexcept {
    return reference(*iterator_, value_transform_, index_transform_);
  }

private:

  BackendIteratorType iterator_;
  const ValueTransform& value_transform_;
  const IndexTransform& index_transform_;

  friend const_iterator;
};

template <typename BackendIteratorType,
          typename ValueTransform,
          typename IndexTransform>
class const_transform_matrix_view_iterator {
public:
  using backend_value_type = typename BackendIteratorType::value_type;

  using value_transform_type = ValueTransform;
  using index_transform_type = IndexTransform;

  using value_type = decltype(std::declval<ValueTransform>()(std::declval<backend_value_type>()));

  using index_type = typename BackendIteratorType::index_type;
  using size_type = typename BackendIteratorType::size_type;
  using difference_type = typename BackendIteratorType::difference_type;

  using backend_iterator = BackendIteratorType;
  using backend_reference = typename BackendIteratorType::reference;
  using const_backend_reference = typename BackendIteratorType::const_reference;

  using pointer = transform_matrix_view_iterator<BackendIteratorType, ValueTransform, IndexTransform>;

  using iterator = pointer;
  using const_iterator = const_transform_matrix_view_iterator<backend_iterator, value_transform_type, index_transform_type>;

  using reference = transform_matrix_view_reference<backend_reference, value_transform_type, index_transform_type>;
  using const_reference = const_transform_matrix_view_reference<const_backend_reference, value_transform_type, index_transform_type>;

  using iterator_category = typename BackendIteratorType::iterator_category;

  const_transform_matrix_view_iterator(BackendIteratorType iterator,
                                 const ValueTransform& value_transform,
                                 const IndexTransform& index_transform)
    : iterator_(iterator),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  const_transform_matrix_view_iterator(const const_transform_matrix_view_iterator& other,
                                       const ValueTransform& value_transform,
                                       const IndexTransform& index_transform)
    : iterator_(other.iterator_),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  const_transform_matrix_view_iterator(const iterator& other,
                                       const ValueTransform& value_transform,
                                       const IndexTransform& index_transform)
    : iterator_(other.iterator_),
      value_transform_(value_transform),
      index_transform_(index_transform) {}

  const_transform_matrix_view_iterator& operator=(const const_transform_matrix_view_iterator& other) {
    iterator_ = other.iterator_;
    value_transform_ = other.value_transform_;
    index_transform_ = other.index_transform_;
    return *this;
  }

  const_transform_matrix_view_iterator& operator++() {
    ++iterator_;
    return *this;
  }

  const_transform_matrix_view_iterator& operator--() {
    --iterator_;
    return *this;
  }

  bool operator<(const const_transform_matrix_view_iterator& other) {
    return iterator_ < other.iterator_;
  }

  const_transform_matrix_view_iterator operator+(const difference_type offset) {
    return const_transform_matrix_view_iterator(iterator_ + offset, value_transform_, index_transform_);
  }

  const_transform_matrix_view_iterator operator-(const difference_type offset) {
    return const_transform_matrix_view_iterator(iterator_ - offset, value_transform_, index_transform_);
  }

  difference_type operator-(const const_transform_matrix_view_iterator& other) const noexcept {
    return iterator_ - other.iterator_;
  }

  bool operator==(const const_transform_matrix_view_iterator& other) const noexcept {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const const_transform_matrix_view_iterator& other) const noexcept {
    return iterator_ != other.iterator_;
  }

  const_reference operator*() noexcept {
    return const_reference(*iterator_, value_transform_, index_transform_);
  }

private:

  BackendIteratorType iterator_;
  const ValueTransform& value_transform_;
  const IndexTransform& index_transform_;

  friend iterator;
};

} // end grb