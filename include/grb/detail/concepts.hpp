#pragma once

#include "matrix_traits.hpp"
#include "get.hpp"

namespace grb {

template <typename T, std::size_t I, typename U = grb::any>
concept TupleElementGettable = requires(T tuple) {
                                 {grb::get<I>(tuple)} -> std::convertible_to<U>;
                               };
template <typename T, typename... Args>
concept TupleLike =
  requires {
    typename std::tuple_size<std::remove_cvref_t<T>>::type;
    requires std::same_as<std::remove_cvref_t<decltype(std::tuple_size_v<std::remove_cvref_t<T>>)>, std::size_t>;
  } &&
  sizeof...(Args) == std::tuple_size_v<std::remove_cvref_t<T>> &&
  []<std::size_t... I>(std::index_sequence<I...>) {
    return (TupleElementGettable<T, I, Args> && ...);
  }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>());

template <typename Entry, typename T, typename I>
concept MatrixEntry = TupleLike<Entry, grb::any, grb::any> &&
                      requires(Entry entry) { {grb::get<0>(entry)} -> TupleLike<I, I>; } &&
                      requires(Entry entry) { {grb::get<1>(entry)} -> std::convertible_to<T>; };

template <typename Entry, typename T, typename I, typename U>
concept MutableMatrixEntry = MatrixEntry<Entry, T, I> &&
                             std::indirectly_writable<decltype(std::get<1>(std::declval<Entry>())), U>;


template <typename M>
concept MatrixRange = std::ranges::sized_range<M> &&
  requires(M matrix) {
    typename grb::matrix_scalar_type_t<M>;
    typename grb::matrix_index_type_t<M>;
    {std::declval<std::ranges::range_value_t<std::remove_cvref_t<M>>>()}
      -> MatrixEntry<grb::matrix_scalar_type_t<M>,
                     grb::matrix_index_type_t<M>>;
    {grb::shape(matrix)} -> TupleLike<grb::matrix_index_type_t<M>,
                                  grb::matrix_index_type_t<M>>;
    {grb::find(matrix, {grb::matrix_index_type_t<M>{}, grb::matrix_index_type_t<M>{}})} -> std::convertible_to<std::ranges::iterator_t<M>>;
  };
  
template <typename M>
concept MaskMatrixRange = MatrixRange<M> &&
                          std::is_convertible_v<grb::matrix_scalar_type_t<M>, bool>;

} // end grb