
#pragma once

#include <iterator>

namespace grb {

template <typename BackendIteratorType>
class matrix_iterator {
public:
  using value_type = typename BackendIteratorType::value_type;
  using index_type = typename BackendIteratorType::index_type;
  using size_type = typename BackendIteratorType::size_type;
  using difference_type = typename BackendIteratorType::difference_type;
  using backend_reference = typename BackendIteratorType::reference;
  using reference = matrix_reference<backend_reference>;
  using iterator_category = typename BackendIteratorType::iterator_category;
  using backend_iterator = BackendIteratorType;

  matrix_iterator(BackendIteratorType iterator) : iterator_(iterator) {}

  matrix_iterator(const matrix_iterator& other) : iterator_(other.iterator_) {}

  matrix_iterator& operator=(const matrix_iterator& other) {
    iterator_ = other.iterator_;
    return *this;
  }

  matrix_iterator& operator++() {
    ++iterator_;
    return *this;
  }

  matrix_iterator& operator--() {
    --iterator_;
    return *this;
  }

  bool operator<(const matrix_iterator& other) {
    return iterator_ < other.iterator_;
  }

  matrix_iterator operator+(const difference_type offset) {
    return matrix_iterator(iterator_ + offset);
  }

  matrix_iterator operator-(const difference_type offset) {
    return matrix_iterator(iterator_ - offset);
  }

  difference_type operator-(const matrix_iterator& other) const noexcept {
    return iterator_ - other.iterator_;
  }

  bool operator==(const matrix_iterator& other) const noexcept {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const matrix_iterator& other) const noexcept {
    return iterator_ != other.iterator_;
  }

  reference operator*() noexcept {
    return reference(*iterator_);
  }

 private:

   BackendIteratorType iterator_;
};

} // end grb
