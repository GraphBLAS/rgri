#pragma once

#include <limits>
#include <type_traits>

#include <grb/containers/vector_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/util/index.hpp>

namespace grb {

template <typename T, std::integral I>
class full_vector_accessor {
public:
  using value_type = grb::vector_entry<T, I>;
  using difference_type = std::ptrdiff_t;
  using iterator_accessor = full_vector_accessor;
  using const_iterator_accessor = full_vector_accessor;
  using nonconst_iterator_accessor = full_vector_accessor;
  using reference = grb::vector_entry<T, I>;
  using iterator_category = std::random_access_iterator_tag;

  constexpr full_vector_accessor() noexcept = default;
  constexpr ~full_vector_accessor() noexcept = default;
  constexpr full_vector_accessor(const full_vector_accessor&) noexcept =
      default;
  constexpr full_vector_accessor&
  operator=(const full_vector_accessor&) noexcept = default;

  constexpr full_vector_accessor(I i, I n, T value) noexcept
      : i_(i), n_(n), value_(value) {}

  constexpr full_vector_accessor& operator+=(difference_type offset) noexcept {
    i_ += offset;
    return *this;
  }

  constexpr difference_type
  operator-(const const_iterator_accessor& other) const noexcept {
    return difference_type(i_) - difference_type(other.i_);
  }

  constexpr bool
  operator==(const const_iterator_accessor& other) const noexcept {
    return i_ == other.i_;
  }

  constexpr bool
  operator<(const const_iterator_accessor& other) const noexcept {
    return i_ < other.i_;
  }

  constexpr reference operator*() const noexcept {
    return grb::vector_entry<T, I>(i_, value_);
  }

private:
  I i_;
  I n_;
  T value_;
};

template <typename T, std::integral I = std::size_t>
using full_vector_iterator =
    grb::detail::iterator_adaptor<full_vector_accessor<T, I>>;

template <typename T, typename I = std::size_t>
class full_vector {
public:
  using scalar_type = T;

  using index_type = I;

  using value_type = grb::vector_entry<T, I>;

  using key_type = I;
  using map_type = T;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  using scalar_reference = scalar_type;

  using iterator = full_vector_iterator<T, I>;
  using const_iterator = iterator;

  full_vector(I shape = std::numeric_limits<I>::max(), T value = T())
      : shape_(shape), value_(value) {}

  iterator begin() const noexcept {
    return iterator(0, shape_, value_);
  }

  iterator end() const noexcept {
    return iterator(shape_, shape_, value_);
  }

  I shape() const noexcept {
    return shape_;
  }

  size_type size() const noexcept {
    return shape();
  }

  scalar_reference operator[](I index) const noexcept {
    return value_;
  }

  iterator find(key_type key) const noexcept {
    if constexpr (std::is_signed_v<I>) {
      if (key < 0) {
        return end();
      }
    }

    if (key < shape()) {
      return iterator(key, shape(), value_);
    } else {
      return end();
    }
  }

private:
  T value_;
  I shape_;
};

template <typename I = std::size_t>
class full_vector_mask : public full_vector<bool, I> {
public:
  full_vector_mask(I shape = std::numeric_limits<I>::max())
      : full_vector<bool, I>(shape, true) {}
};

template <typename I = std::size_t>
class empty_vector_mask : public full_vector<bool, I> {
public:
  empty_vector_mask(I shape = std::numeric_limits<I>::max())
      : full_vector<bool, I>(shape, false) {}
};

} // namespace grb
