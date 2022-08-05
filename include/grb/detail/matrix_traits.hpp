#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <ranges>
#include <any>

namespace grb {

using any = std::any;

template <typename T, std::size_t I>
concept method_gettable = requires(T tuple) {
  {tuple. template get<I>()};
};

template <typename T, std::size_t I>
concept std_gettable = requires(T tuple) {
  {std::get<I>(tuple)};
};

template <std::size_t I, typename T>
inline constexpr decltype(auto) get(T&& tuple)
requires(method_gettable<T, I>)
{
  return std::forward<T>(tuple). template get<I>();
}

template <std::size_t I, typename T>
inline constexpr decltype(auto) get(T&& tuple)
requires(!method_gettable<T, I> && std_gettable<T, I>)
{
  return std::get<I>(std::forward<T>(tuple));
}

template <typename Matrix>
using matrix_scalar_type_t = typename std::remove_cvref_t<Matrix>::scalar_type;

template <typename Matrix>
using matrix_index_type_t_impl_ = typename std::remove_cvref_t<Matrix>::index_type;

template <typename Matrix>
requires(std::is_integral_v<matrix_index_type_t_impl_<Matrix>>)
using matrix_index_type_t = matrix_index_type_t_impl_<Matrix>;

template <typename Matrix>
using matrix_index_type_t_impl_ = typename std::remove_cvref_t<Matrix>::index_type;

template <typename Tuple, std::size_t... Is>
inline auto get_as_tuple(std::index_sequence<Is...> idx_seq) {
  return std::tuple<std::tuple_element_t<Is, Tuple>...>{std::tuple_element_t<Is, Tuple>()...};
}

template <typename Tuple, std::size_t N>
concept TuplelikeSize = requires(Tuple tuple) {
  {std::tuple_size<std::remove_cvref_t<Tuple>>{}} -> std::derived_from<std::integral_constant<size_t, N>>;
  {get_as_tuple<std::remove_cvref_t<Tuple>>(std::make_index_sequence<N>())};
};

template <typename Tuple, typename... Args>
concept Tuplelike = requires(Tuple tuple) {
  {std::tuple_size<std::remove_cvref_t<Tuple>>{}} -> std::derived_from<std::integral_constant<size_t, sizeof...(Args)>>;
  {get_as_tuple<std::remove_cvref_t<Tuple>>(std::make_index_sequence<sizeof...(Args)>())} -> std::same_as<std::tuple<Args...>>;
};

template <typename T, std::size_t I, typename U = grb::any>
concept is_tuple_element =
  (requires(T t) { {t. template get<I>()} -> std::convertible_to<std::tuple_element_t<I, T>>; } &&
   requires(T t) { {t. template get<I>()} -> std::convertible_to<U>; }) ||
  (requires(T t) { {std::get<I>(t)} -> std::convertible_to<std::tuple_element_t<I, T>>; } &&
   requires(T t) { {std::get<I>(t)} -> std::convertible_to<U>; });

template <typename T>
concept tuple_like = requires {
                       typename std::tuple_size<T>::type;
                       requires std::same_as<std::remove_cvref_t<decltype(std::tuple_size_v<T>)>, std::size_t>;
                     } && []<std::size_t... I>(std::index_sequence<I...>) {
                      return (is_tuple_element<T, I> && ...); }(std::make_index_sequence<std::tuple_size_v<T>>());

template <typename T, typename... Args>
concept TupleLike = requires {
                       typename std::tuple_size<T>::type;
                       requires std::same_as<std::remove_cvref_t<decltype(std::tuple_size_v<T>)>, std::size_t>;
                     } &&
                     sizeof...(Args) == std::tuple_size_v<T> &&
                     []<std::size_t... I>(std::index_sequence<I...>) {
                      return (is_tuple_element<T, I, Args> && ...); }(std::make_index_sequence<std::tuple_size_v<T>>());

template <typename ValueType, typename T, typename I>
concept MatrixValueType = TuplelikeSize<ValueType, 2> &&
                          requires(ValueType value) {
                            {grb::get<0>(value)} -> Tuplelike<I, I>;
                            {grb::get<1>(value)} -> std::convertible_to<T>;
                            {I{}} -> std::integral;
                          };

template <typename Entry, typename T, typename I>
concept MatrixEntry = is_tuple_element<Entry, 1, T> &&
                      (requires(T t) { {t. template get<0>()} -> TupleLike<I, I>; } ||
                       requires(T t) { {std::get<0>(t)} -> TupleLike<I, I>; });

template <typename Entry, typename T, typename I>
concept MutableMatrixEntry = is_tuple_element<Entry, 1, T&> &&
                             (requires(T t) { {t. template get<0>()} -> TupleLike<I, I>; } ||
                              requires(T t) { {std::get<0>(t)} -> TupleLike<I, I>; });


template <typename Matrix>
concept MatrixRange = std::ranges::sized_range<Matrix> &&
  requires(Matrix matrix) {
    typename grb::matrix_scalar_type_t<Matrix>;
    typename grb::matrix_index_type_t<Matrix>;
    {matrix.shape()} -> Tuplelike<grb::matrix_index_type_t<Matrix>,
                                  grb::matrix_index_type_t<Matrix>>;
    {std::declval<typename std::ranges::range_value_t<std::remove_cvref_t<Matrix>>>()}
      -> MatrixValueType<grb::matrix_scalar_type_t<Matrix>,
                         grb::matrix_index_type_t<Matrix>>;
  };

} // end grb