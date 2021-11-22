
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

} // end grb

namespace std {

template <std::size_t I, typename T>
size_t get(grb::index<T> idx)
requires(I <= 1)
{
  return idx[I];
}

} // end std
