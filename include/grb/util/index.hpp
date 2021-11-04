
#pragma once

#include <tuple>
#include <concepts>
#include <limits>

namespace grb {

template <typename T = std::size_t>
requires(std::is_integral_v<T> && !std::is_reference_v<T>)
class index {
public:
  using index_type = T;

  using first_type = T;
  using second_type = T;

  index_type operator[](index_type dim) const noexcept {
    if (dim == 0) {
      return first;
    } else {
      return second;
    }
  }

  template <std::integral U>
  requires(std::numeric_limits<U>::max() >= std::numeric_limits<T>::max())
  operator index<U>() const noexcept {
    return index<U>(first, second);
  }

  index(index_type first, index_type second) : first(first), second(second) {}

  bool operator==(const index&) const noexcept = default;

  index() = default;
  ~index() = default;
  index(const index&) = default;
  index& operator=(const index&) = default;
  index(index&&) = default;
  index& operator=(index&&) = default;

  index_type first;
  index_type second;
};

struct index_t {
  using size_type = std::size_t;

  operator std::pair<size_type, size_type>() const noexcept {
    return std::make_pair(i, j);
  }

  operator std::tuple<size_type, size_type>() const noexcept {
    return std::make_tuple(i, j);
  }

  bool operator==(index_t other) {
    return i == other.i && j == other.j;
  }

  bool operator!=(index_t other) {
    return !(*this == other);
  }

  size_type operator[](size_type idx) const noexcept {
    if (idx == 0) {
      return i;
    } else {
      return j;
    }
  }

  size_type first() const noexcept {
    return i;
  }

  size_type second() const noexcept {
    return j;
  }

  size_type i, j;
};

} // end grb

namespace std {

template <std::size_t I, typename T>
size_t get(grb::index<T> idx)
requires(I <= 1)
{
  return idx[I];
}

} // end std

namespace std {

template <std::size_t I>
size_t get(grb::index_t idx) {
  static_assert(I <= 1, "std::get() called on index_t with an index greater than 1");

  return idx[I];
}

} // end std
