
#pragma once

#include <concepts>
#include <limits>
#include <tuple>

#include <grb/detail/concepts.hpp>
#include <grb/detail/detect_index.hpp>

namespace grb {

template <std::integral T = std::size_t>
class index {
public:
  using index_type = T;

  using first_type = T;
  using second_type = T;

  constexpr index_type operator[](index_type dim) const noexcept {
    if (dim == 0) {
      return first;
    } else {
      return second;
    }
  }

  template <std::integral U>
    requires(std::numeric_limits<U>::max() >= std::numeric_limits<T>::max())
  constexpr operator index<U>() const noexcept {
    return index<U>(first, second);
  }

  constexpr index(index_type first, index_type second)
      : first(first), second(second) {}

  template <grb::TupleLike<T, T> Tuple>
    requires(!__detail::instantiation_of_index_v<Tuple>)
  constexpr index(Tuple tuple)
      : first(grb::get<0>(tuple)), second(grb::get<1>(tuple)) {}

  constexpr bool operator==(const index&) const noexcept = default;

  template <std::size_t Index>
  constexpr T get() const noexcept
    requires(Index <= 1)
  {
    if constexpr (Index == 0) {
      return first;
    }
    if constexpr (Index == 1) {
      return second;
    }
  }

  index() = default;
  ~index() = default;
  index(const index&) = default;
  index& operator=(const index&) = default;
  index(index&&) = default;
  index& operator=(index&&) = default;

  index_type first;
  index_type second;
};

} // namespace grb

namespace std {

template <std::size_t Index, std::integral I>
struct tuple_element<Index, grb::index<I>>
    : tuple_element<Index, std::tuple<I, I>> {};

template <std::integral I>
struct tuple_size<grb::index<I>> : integral_constant<std::size_t, 2> {};

template <std::size_t Index, std::integral I>
inline constexpr I get(grb::index<I> index)
  requires(Index <= 1)
{
  if constexpr (Index == 0) {
    return index.first;
  }
  if constexpr (Index == 1) {
    return index.second;
  }
}

} // namespace std
