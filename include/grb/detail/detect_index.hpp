#pragma once

#include <concepts>

namespace grb {

template <std::integral T>
class index;

namespace __detail {

template <typename T>
struct instantiation_of_index {
  static constexpr bool value = false;
};

template <typename T>
struct instantiation_of_index<index<T>> {
  static constexpr bool value = true;
};

template <typename T>
inline constexpr bool instantiation_of_index_v =
    instantiation_of_index<T>::value;

} // namespace __detail

} // namespace grb
