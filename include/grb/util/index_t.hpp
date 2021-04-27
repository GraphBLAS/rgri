
#pragma once

#include <tuple>

namespace grb {

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

template <std::size_t I>
size_t get(grb::index_t idx) {
  static_assert(I <= 1, "std::get() called on index_t with an index greater than 1");

  return idx[I];
}

} // end std
