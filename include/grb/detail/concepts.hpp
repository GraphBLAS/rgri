#pragma once

#include <ranges>

#include "cpos.hpp"
#include "get.hpp"
#include "matrix_traits.hpp"

namespace grb {

template <typename T, std::size_t I, typename U = grb::any>
concept TupleElementGettable = requires(T tuple) {
  { grb::get<I>(tuple) } -> std::convertible_to<U>;
};
template <typename T, typename... Args>
concept TupleLike =
    requires {
      typename std::tuple_size<std::remove_cvref_t<T>>::type;
      requires std::same_as<
          std::remove_cvref_t<
              decltype(std::tuple_size_v<std::remove_cvref_t<T>>)>,
          std::size_t>;
    } && sizeof...(Args) == std::tuple_size_v<std::remove_cvref_t<T>> &&
    []<std::size_t... I>(std::index_sequence<I...>) {
      return (TupleElementGettable<T, I, Args> && ...);
    }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>());

template <typename Entry, typename T, typename I>
concept MatrixEntry =
    TupleLike<Entry, grb::any, grb::any> && requires(Entry entry) {
      { grb::get<0>(entry) } -> TupleLike<I, I>;
    } && requires(Entry entry) {
      { grb::get<1>(entry) } -> std::convertible_to<T>;
    };

template <typename Entry, typename T, typename I, typename U>
concept MutableMatrixEntry =
    MatrixEntry<Entry, T, I> &&
    std::is_assignable_v<decltype(grb::get<1>(std::declval<Entry>())), U>;

template <typename M>
concept MatrixRange = std::ranges::sized_range<M> && requires(M matrix) {
  typename container_traits<std::remove_cvref_t<M>>;
  // typename grb::matrix_scalar_t<M>;
  // typename grb::matrix_index_t<M>;
  {
    std::declval<std::ranges::range_value_t<M>>()
  } -> MatrixEntry<grb::matrix_scalar_t<M>, grb::matrix_index_t<M>>;
  {
    grb::shape(matrix)
  } -> TupleLike<grb::matrix_index_t<M>, grb::matrix_index_t<M>>;
  {
    grb::find(matrix, {grb::matrix_index_t<M>{}, grb::matrix_index_t<M>{}})
  } -> std::convertible_to<std::ranges::iterator_t<M>>;
};

template <typename M, typename T>
concept MutableMatrixRange =
    grb::MatrixRange<M> &&
    grb::MutableMatrixEntry<std::ranges::range_reference_t<M>,
                            grb::matrix_scalar_t<M>, grb::matrix_index_t<M>,
                            T> &&
    requires(M matrix, T value) {
      {
        grb::insert(
            matrix,
            {{grb::matrix_index_t<M>{}, grb::matrix_index_t<M>{}}, value})
      } -> std::same_as<std::pair<std::ranges::iterator_t<M>, bool>>;
    } && std::is_constructible_v<grb::matrix_scalar_t<M>, T>;

template <typename M>
concept MaskMatrixRange =
    MatrixRange<M> && std::is_convertible_v<grb::matrix_scalar_t<M>, bool>;

template <typename Entry, typename T, typename I>
concept VectorEntry =
    TupleLike<Entry, grb::any, grb::any> && requires(Entry entry) {
      { grb::get<0>(entry) } -> std::integral;
    } && requires(Entry entry) {
      { grb::get<1>(entry) } -> std::convertible_to<T>;
    };

template <typename Entry, typename T, typename I, typename U>
concept MutableVectorEntry =
    VectorEntry<Entry, T, I> &&
    std::is_assignable_v<decltype(grb::get<1>(std::declval<Entry>())), U>;

template <typename V>
concept VectorRange = std::ranges::sized_range<V> && requires(V vector) {
  typename grb::vector_scalar_t<V>;
  typename grb::vector_index_t<V>;
  {
    std::declval<std::ranges::range_value_t<V>>()
  } -> VectorEntry<grb::vector_scalar_t<V>, grb::vector_index_t<V>>;
  { grb::shape(vector) } -> std::same_as<grb::vector_index_t<V>>;
  {
    grb::find(vector, grb::vector_index_t<V>{})
  } -> std::convertible_to<std::ranges::iterator_t<V>>;
};

template <typename V, typename T>
concept MutableVectorRange =
    VectorRange<V> &&
    MutableVectorEntry<std::ranges::range_reference_t<V>,
                       grb::vector_scalar_t<V>, grb::vector_index_t<V>, T> &&
    requires(V vector, T value) {
      {
        grb::insert(vector,
                    {grb::vector_index_t<V>{}, grb::vector_scalar_t<V>{}})
      } -> std::same_as<std::pair<std::ranges::iterator_t<V>, bool>>;
    } && std::is_constructible_v<grb::vector_scalar_t<V>, T>;

template <typename M>
concept MaskVectorRange =
    VectorRange<M> && std::is_convertible_v<grb::vector_scalar_t<M>, bool>;

} // namespace grb
