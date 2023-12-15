#pragma once

#include <grb/containers/backend/dense_vector_iterator.hpp>
#include <grb/containers/vector_entry.hpp>
#include <numeric>
#include <vector>

namespace grb {

template <typename T, typename I, typename Allocator>
struct dense_vector {
public:
  using index_type = I;
  using value_type = grb::vector_entry<T, I>;

  using key_type = I;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;

  using bool_allocator_type = typename std::allocator_traits<
      allocator_type>::template rebind_alloc<bool>;

  using iterator = dense_vector_iterator<
      T, index_type, typename std::vector<T, allocator_type>::iterator,
      typename std::vector<T, allocator_type>::const_iterator,
      typename std::vector<bool, bool_allocator_type>::const_iterator>;

  using const_iterator = dense_vector_iterator<
      std::add_const_t<T>, index_type,
      typename std::vector<T, allocator_type>::iterator,
      typename std::vector<T, allocator_type>::const_iterator,
      typename std::vector<bool, bool_allocator_type>::const_iterator>;

  using reference = grb::vector_ref<T, index_type>;
  using const_reference = grb::vector_ref<std::add_const_t<T>, index_type>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  using scalar_reference = typename std::vector<T, allocator_type>::reference;

  dense_vector(I shape) {
    data_.resize(shape);
    flags_.resize(shape, false);
  }

  dense_vector(I shape, const Allocator& allocator)
      : data_(allocator), flags_(allocator) {
    data_.resize(shape);
    flags_.resize(shape, false);
  }

  size_type shape() const noexcept {
    return data_.size();
  }

  size_type size() const noexcept {
    return nnz_;
  }

  scalar_reference operator[](I index) noexcept {
    if (!flags_[index]) {
      data_[index] = T();
      flags_[index] = true;
      nnz_++;
    }
    return data_[index];
  }

  iterator begin() noexcept {
    return iterator(data_, flags_, 0);
  }

  const_iterator begin() const noexcept {
    return const_iterator(data_, flags_, 0);
  }

  iterator end() noexcept {
    return iterator(data_, flags_, shape());
  }

  const_iterator end() const noexcept {
    return const_iterator(data_, flags_, shape());
  }

  std::pair<iterator, bool> insert(const value_type& value) {
    auto&& [idx, v] = value;
    if (flags_[idx]) {
      return {iterator(data_, flags_, idx), false};
    } else {
      nnz_++;
      flags_[idx] = true;
      data_[idx] = v;
      return {iterator(data_, flags_, idx), true};
    }
  }

  template <typename M>
  std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
    if (flags_[k]) {
      data_[k] = std::forward<M>(obj);
      return {iterator(data_, flags_, k), false};
    } else {
      nnz_++;
      flags_[k] = true;
      data_[k] = std::forward<M>(obj);
      return {iterator(data_, flags_, k), true};
    }
  }

  iterator find(key_type key) noexcept {
    if (flags_[key]) {
      return iterator(data_, flags_, key);
    } else {
      return end();
    }
  }

  const_iterator find(key_type key) const noexcept {
    if (flags_[key]) {
      return const_iterator(data_, flags_, key);
    } else {
      return end();
    }
  }

  void reshape(I shape) {
    bool smaller = shape < this->shape();
    data_.resize(shape);
    flags_.resize(shape, false);
    if (smaller) {
      nnz_ = std::reduce(flags_.begin(), flags_.end(), size_t(0));
    }
  }

  dense_vector() = default;

  dense_vector(const Allocator& allocator)
      : data_(allocator), flags_(allocator) {}

  ~dense_vector() = default;
  dense_vector(const dense_vector&) = default;
  dense_vector& operator=(const dense_vector&) = default;

  dense_vector(dense_vector&& other)
      : data_(std::move(other.data_)), flags_(std::move(other.flags_)),
        nnz_(other.nnz_) {
    other.nnz_ = 0;
  }

  dense_vector& operator=(dense_vector&& other) {
    data_ = std::move(other.data_);
    flags_ = std::move(other.flags_);
    nnz_ = other.nnz_;
    other.nnz_ = 0;
    return *this;
  }

private:
  friend iterator;
  friend const_iterator;

  std::vector<T, allocator_type> data_;
  std::vector<bool, bool_allocator_type> flags_;
  size_t nnz_ = 0;
};

} // namespace grb
