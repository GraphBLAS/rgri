#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <ranges>
#include <any>

namespace grb {

using any = std::any;

template <typename Container>
struct container_traits {
  using size_type = std::ranges::range_size_t<Container>;
  using difference_type = std::ranges::range_difference_t<Container>;
  using iterator = std::ranges::iterator_t<Container>;
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = typename std::tuple_element<0, value_type>::type;
  using map_type = typename std::tuple_element<1, value_type>::type;
  using scalar_type = typename std::tuple_element<1, value_type>::type;
  using index_type = typename std::tuple_element<0, key_type>::type;
  using reference = std::ranges::range_reference_t<Container>;
};

// TODO: depricate *_scalar_type_t in favor of *scalar_t

template <typename Matrix>
using matrix_scalar_type_t = typename std::remove_cvref_t<Matrix>::scalar_type;

template <typename Matrix>
using matrix_index_type_t = typename std::remove_cvref_t<Matrix>::index_type;

template <typename Matrix>
using matrix_scalar_t = typename std::remove_cvref_t<Matrix>::scalar_type;

template <typename Matrix>
using matrix_index_t = typename std::remove_cvref_t<Matrix>::index_type;

namespace {

template <typename T>
concept has_shape_method = requires(T t) { {t.shape()} -> std::same_as<typename std::tuple_element<0, std::ranges::range_value_t<T>>::type>; };

template <typename T>
concept has_find_method = requires(T t) { {t.find(std::declval<typename container_traits<T>::key_type>())} -> std::same_as<typename container_traits<T>::iterator>; };

}  // end

template <has_shape_method T>
constexpr auto shape(T&& t) {
  return std::forward<T>(t).shape();
}

template <has_find_method T>
constexpr auto find(T&& t, typename grb::container_traits<T>::key_type key) {
  return std::forward<T>(t).find(key);
}

} // end grb