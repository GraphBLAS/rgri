#pragma once

#include <type_traits>
#include <concepts>
#include <utility>
#include <ranges>
#include <any>
#include <grb/detail/tag_invoke.hpp>

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
  using key_type = std::remove_cvref_t<typename std::tuple_element<0, value_type>::type>;
  using map_type = std::remove_cvref_t<typename std::tuple_element<1, value_type>::type>;
  using scalar_type = std::remove_cvref_t<typename std::tuple_element<1, value_type>::type>;
  using index_type = std::remove_cvref_t<typename get_index_type<Container>::type>;
  using reference = std::ranges::range_reference_t<Container>;
};

template <typename Container>
struct matrix_traits {
  using size_type = std::ranges::range_size_t<Container>;
  using difference_type = std::ranges::range_difference_t<Container>;
  using iterator = std::ranges::iterator_t<Container>;
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = std::remove_cvref_t<typename std::tuple_element<0, value_type>::type>;
  using map_type = std::remove_cvref_t<typename std::tuple_element<1, value_type>::type>;
  using scalar_type = std::remove_cvref_t<typename std::tuple_element<1, value_type>::type>;
  using index_type = std::remove_cvref_t<typename std::tuple_element<0, key_type>::type>;
  using reference = std::ranges::range_reference_t<Container>;
};

template <typename Container>
struct vector_traits {
  using size_type = std::ranges::range_size_t<Container>;
  using difference_type = std::ranges::range_difference_t<Container>;
  using iterator = std::ranges::iterator_t<Container>;
  using value_type = std::ranges::range_value_t<Container>;
  using key_type = std::remove_cvref_t<typename std::tuple_element<0, value_type>::type>;
  using map_type = std::remove_cvref_t<typename std::tuple_element<1, value_type>::type>;
  using scalar_type = std::remove_cvref_t<typename std::tuple_element<1, value_type>::type>;
  using index_type = key_type;
  using reference = std::ranges::range_reference_t<Container>;
};

// TODO: depricate *_scalar_type_t in favor of *scalar_t

template <typename T>
using container_scalar_t = typename container_traits<T>::scalar_type;

template <typename T>
using container_index_t = typename container_traits<T>::index_type;

template <typename T>
using container_value_t = typename container_traits<T>::value_type;

template <typename T>
using container_key_t = typename container_traits<T>::key_type;

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

template <typename T>
concept has_insert_method = requires(T t) { {t.insert({std::declval<typename container_traits<T>::key_type>(), std::declval<container_scalar_t<T>>()})}; };

template <typename T, typename M>
concept has_insert_or_assign_method = requires(T t, M obj) { {t.insert_or_assign(std::declval<typename container_traits<T>::key_type>(), std::forward<M>(obj))}; };

}  // end

inline constexpr struct shape_fn_ {
  template <typename T>
  auto operator()(T&& x) const
    requires(grb::is_tag_invocable_v<shape_fn_, T> ||
             has_matrix_shape<T> ||
             has_vector_shape<T>)
    {
    if constexpr(grb::is_tag_invocable_v<shape_fn_, T>) {
      return grb::tag_invoke(*this, std::forward<T>(x));
    } else if constexpr(has_matrix_shape<T>) {
      return std::forward<T>(x).shape();
    } else if constexpr(has_vector_shape<T>) {
      return std::forward<T>(x).shape();
    }
  }
} shape{};

inline constexpr struct size_fn_ {
  template <typename T>
  auto operator()(T&& x) const
  requires(grb::is_tag_invocable_v<size_fn_, T> ||
           std::ranges::sized_range<T>)
  {
    if constexpr(grb::is_tag_invocable_v<size_fn_, T>) {
      return grb::tag_invoke(*this, std::forward<T>(x));
    } else if constexpr(std::ranges::sized_range<T>) {
      return std::ranges::size(std::forward<T>(x));
    }
  }
} size{};

inline constexpr struct find_fn_ {
  template <typename T>
  auto operator()(T&& x, typename grb::container_traits<T>::key_type key) const
  requires(grb::is_tag_invocable_v<find_fn_, T, typename grb::container_traits<T>::key_type> ||
           has_find_method<T>)
  {
    if constexpr(grb::is_tag_invocable_v<find_fn_, T, typename grb::container_traits<T>::key_type>) {
      return grb::tag_invoke(*this, std::forward<T>(x), key);
    } else if constexpr(has_find_method<T>) {
      return std::forward<T>(x).find(key);
    }
  }
} find{};

inline constexpr struct insert_fn_ {
  template <typename T>
  auto operator()(T&& x, const container_value_t<T>& entry) const
  requires(grb::is_tag_invocable_v<insert_fn_, T, const container_value_t<T>&> ||
           has_insert_method<T>)
  {
    if constexpr(grb::is_tag_invocable_v<insert_fn_, T, const container_value_t<T>&>) {
      return grb::tag_invoke(*this, std::forward<T>(x), entry);
    } else if constexpr(has_insert_method<T>) {
      return std::forward<T>(x).insert(entry);
    }
  }
} insert{};

inline constexpr struct insert_or_assign_fn_ {
  template <typename T, typename M>
  auto operator()(T&& x, typename grb::container_traits<T>::key_type key, M&& obj) const
  requires(grb::is_tag_invocable_v<insert_or_assign_fn_, T, typename grb::container_traits<T>::key_type, M> ||
           has_insert_or_assign_method<T, M>)
  {
    if constexpr(grb::is_tag_invocable_v<insert_or_assign_fn_, T, typename grb::container_traits<T>::key_type, M>) {
      return grb::tag_invoke(*this, std::forward<T>(x), key, std::forward<M>(obj));
    } else if constexpr(has_insert_or_assign_method<T, M>) {
      return std::forward<T>(x).insert_or_assign(key, std::forward<M>(obj));
    }
  }
} insert_or_assign{};

} // end grb