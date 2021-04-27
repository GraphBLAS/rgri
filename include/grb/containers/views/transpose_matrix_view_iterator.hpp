
#pragma once

#include "transpose_matrix_view_reference.hpp"

namespace grb {

template <typename BackendIteratorType>
class const_transpose_matrix_view_iterator;

template <typename BackendIteratorType>
class transpose_matrix_view_iterator {
public:
  using value_type = typename BackendIteratorType::value_type;
  using index_type = typename BackendIteratorType::index_type;
  using size_type = typename BackendIteratorType::size_type;
  using difference_type = typename BackendIteratorType::difference_type;

  using backend_iterator = BackendIteratorType;
  using backend_reference = typename BackendIteratorType::reference;

  using pointer = transpose_matrix_view_iterator;

  using iterator = transpose_matrix_view_iterator;
  using const_iterator = const_transpose_matrix_view_iterator<backend_iterator>;

  using reference = transpose_matrix_view_reference<backend_reference>;

  using iterator_category = typename BackendIteratorType::iterator_category;

  transpose_matrix_view_iterator(BackendIteratorType iterator) : iterator_(iterator) {}

  transpose_matrix_view_iterator(const transpose_matrix_view_iterator& other) : iterator_(other.iterator_) {}

  transpose_matrix_view_iterator& operator=(const transpose_matrix_view_iterator& other) {
    iterator_ = other.iterator_;
    return *this;
  }

  transpose_matrix_view_iterator& operator++() {
    ++iterator_;
    return *this;
  }

  transpose_matrix_view_iterator& operator--() {
    --iterator_;
    return *this;
  }

  bool operator<(const transpose_matrix_view_iterator& other) {
    return iterator_ < other.iterator_;
  }

  transpose_matrix_view_iterator operator+(const difference_type offset) {
    return transpose_matrix_view_iterator(iterator_ + offset);
  }

  transpose_matrix_view_iterator operator-(const difference_type offset) {
    return transpose_matrix_view_iterator(iterator_ - offset);
  }

  difference_type operator-(const transpose_matrix_view_iterator& other) const noexcept {
    return iterator_ - other.iterator_;
  }

  bool operator==(const transpose_matrix_view_iterator& other) const noexcept {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const transpose_matrix_view_iterator& other) const noexcept {
    return iterator_ != other.iterator_;
  }

  reference operator*() noexcept {
    return reference(*iterator_);
  }

private:

  BackendIteratorType iterator_;

  friend const_iterator;
};

template <typename BackendIteratorType>
class const_transpose_matrix_view_iterator {
public:
  using value_type = typename BackendIteratorType::value_type;
  using index_type = typename BackendIteratorType::index_type;
  using size_type = typename BackendIteratorType::size_type;
  using difference_type = typename BackendIteratorType::difference_type;

  using backend_iterator = BackendIteratorType;
  using backend_reference = typename BackendIteratorType::reference;
  using backend_const_reference = typename BackendIteratorType::const_reference;

  using pointer = transpose_matrix_view_iterator<backend_iterator>;
  using const_pointer = const_transpose_matrix_view_iterator;

  using iterator = transpose_matrix_view_iterator<backend_iterator>;
  using const_iterator = const_transpose_matrix_view_iterator;

  using reference = transpose_matrix_view_reference<backend_reference>;
  using const_reference = const_transpose_matrix_view_reference<backend_const_reference>;

  using iterator_category = typename BackendIteratorType::iterator_category;

  const_transpose_matrix_view_iterator(BackendIteratorType iterator) : iterator_(iterator) {}

  const_transpose_matrix_view_iterator(const const_transpose_matrix_view_iterator& other) : iterator_(other.iterator_) {}
  const_transpose_matrix_view_iterator(const iterator& other) : iterator_(other.iterator_) {}

  const_transpose_matrix_view_iterator& operator=(const const_transpose_matrix_view_iterator& other) {
    iterator_ = other.iterator_;
    return *this;
  }

  const_transpose_matrix_view_iterator& operator++() {
    ++iterator_;
    return *this;
  }

  const_transpose_matrix_view_iterator& operator--() {
    --iterator_;
    return *this;
  }

  bool operator<(const const_transpose_matrix_view_iterator& other) {
    return iterator_ < other.iterator_;
  }

  const_transpose_matrix_view_iterator operator+(const difference_type offset) {
    return const_transpose_matrix_view_iterator(iterator_ + offset);
  }

  const_transpose_matrix_view_iterator operator-(const difference_type offset) {
    return const_transpose_matrix_view_iterator(iterator_ - offset);
  }

  difference_type operator-(const const_transpose_matrix_view_iterator& other) const noexcept {
    return iterator_ - other.iterator_;
  }

  bool operator==(const const_transpose_matrix_view_iterator& other) const noexcept {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const const_transpose_matrix_view_iterator& other) const noexcept {
    return iterator_ != other.iterator_;
  }

  const_reference operator*() noexcept {
    return const_reference(*iterator_);
  }

private:

  BackendIteratorType iterator_;

  friend iterator;
};

} // end grb