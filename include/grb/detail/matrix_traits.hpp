#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <ranges>
#include <any>

namespace grb {

using any = std::any;

namespace {

template <typename Container>
struct get_index_type {
private:
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = typename std::tuple_element<0, value_type>::type;
public:
  using type = key_type;
};

template <typename Container>
requires requires { typename std::tuple_element<0, typename std::tuple_element<0, std::ranges::range_value_t<Container>>::type>::type; }
struct get_index_type<Container>
{
private:
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = typename std::tuple_element<0, value_type>::type;
public:
  using type = typename std::tuple_element<0, key_type>::type;
};
}

template <typename Container>
struct container_traits {
  using size_type = std::ranges::range_size_t<Container>;
  using difference_type = std::ranges::range_difference_t<Container>;
  using iterator = std::ranges::iterator_t<Container>;
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = typename std::tuple_element<0, value_type>::type;
  using map_type = typename std::tuple_element<1, value_type>::type;
  using scalar_type = typename std::tuple_element<1, value_type>::type;
  using index_type = typename get_index_type<Container>::type;
  using reference = std::ranges::range_reference_t<Container>;
};

template <typename Container>
struct matrix_traits {
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

template <typename Container>
struct vector_traits {
  using size_type = std::ranges::range_size_t<Container>;
  using difference_type = std::ranges::range_difference_t<Container>;
  using iterator = std::ranges::iterator_t<Container>;
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = typename std::tuple_element<0, value_type>::type;
  using map_type = typename std::tuple_element<1, value_type>::type;
  using scalar_type = typename std::tuple_element<1, value_type>::type;
  using index_type = key_type;
  using reference = std::ranges::range_reference_t<Container>;
};

// TODO: depricate *_scalar_type_t in favor of *scalar_t

template <typename T>
using container_scalar_t = typename container_traits<T>::scalar_type;

template <typename T>
using container_index_t = typename container_traits<T>::index_type;

template <typename T>
using matrix_scalar_t = typename matrix_traits<T>::scalar_type;

template <typename T>
using matrix_index_t = typename matrix_traits<T>::index_type;

template <typename T>
using vector_scalar_t = typename vector_traits<T>::scalar_type;

template <typename T>
using vector_index_t = typename vector_traits<T>::index_type;

template <typename T>
using vector_scalar_type_t = vector_scalar_t<T>;

template <typename T>
using vector_index_type_t = vector_index_t<T>;

template <typename Matrix>
using matrix_scalar_type_t = typename std::remove_cvref_t<Matrix>::scalar_type;

template <typename Matrix>
using matrix_index_type_t = typename std::remove_cvref_t<Matrix>::index_type;

template <typename A, typename B, typename Fn>
using elementwise_return_type_t = std::invoke_result_t<Fn, container_scalar_t<A>, container_scalar_t<B>>;

namespace {

template <typename T>
concept has_matrix_shape = requires(T t) { {t.shape()} -> std::same_as<typename std::tuple_element<0, std::ranges::range_value_t<T>>::type>; };

template <typename T>
concept has_vector_shape = requires(T t) { {t.shape()} -> std::same_as<std::ranges::range_value_t<T>>; };

template <typename T>
concept has_find_method = requires(T t) { {t.find(std::declval<typename container_traits<T>::key_type>())} -> std::same_as<typename container_traits<T>::iterator>; };

}  // end

template <has_matrix_shape T>
constexpr auto shape(T&& t) {
  return std::forward<T>(t).shape();
}

template <has_vector_shape T>
constexpr auto shape(T&& t) {
  return std::forward<T>(t).shape();
}

template <has_find_method T>
constexpr auto find(T&& t, typename grb::container_traits<T>::key_type key) {
  return std::forward<T>(t).find(key);
}

} // end grb