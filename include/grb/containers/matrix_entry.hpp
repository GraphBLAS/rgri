#pragma once

#include <concepts>
#include <grb/util/index.hpp>
#include <limits>
#include <type_traits>

#include <grb/detail/concepts.hpp>

namespace grb {

template <typename T, typename I = std::size_t>
class matrix_entry {
public:
  using index_type = I;
  using map_type = T;

  matrix_entry(grb::index<I> index, const map_type& value)
      : value_(value), index_(index) {}
  matrix_entry(grb::index<I> index, map_type&& value)
      : value_(std::move(value)), index_(index) {}

  template <typename U>
    requires(std::is_constructible_v<T, U>)
  matrix_entry(grb::index<I> index, U&& value)
      : value_(std::forward<U>(value)), index_(index) {}

  template <typename Entry>
  matrix_entry(Entry&& entry)
      : index_(grb::get<0>(entry)), value_(grb::get<1>(entry)) {}

  template <std::size_t Index>
    requires(Index <= 1)
  auto get() const noexcept {
    if constexpr (Index == 0) {
      return index();
    }
    if constexpr (Index == 1) {
      return value();
    }
  }

  operator std::pair<std::pair<I, I>, T>() const noexcept {
    return {{index_[0], index_[1]}, value_};
  }

  grb::index<I> index() const noexcept {
    return index_;
  }

  map_type value() const noexcept {
    return value_;
  }

  template <std::integral U>
    requires(!std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator matrix_entry<T, U>() const noexcept {
    return matrix_entry<T, U>(index_, value_);
  }

  template <std::integral U>
    requires(!std::is_const_v<T> && !std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator matrix_entry<std::add_const_t<T>, U>() const noexcept {
    return matrix_entry<std::add_const_t<T>, U>(index_, value_);
  }

  bool operator<(const matrix_entry& other) const noexcept {
    if (index()[0] < other.index()[0]) {
      return true;
    } else if (index()[0] == other.index()[0] &&
               index()[1] < other.index()[1]) {
      return true;
    }
    return false;
  }

  matrix_entry() = default;
  ~matrix_entry() = default;

  matrix_entry(const matrix_entry&) = default;
  matrix_entry(matrix_entry&&) = default;
  matrix_entry& operator=(const matrix_entry&) = default;
  matrix_entry& operator=(matrix_entry&&) = default;

private:
  grb::index<I> index_;
  map_type value_;
};

} // namespace grb

namespace std {

template <typename T, typename I>
  requires(!std::is_const_v<T>)
void swap(grb::matrix_entry<T, I> a, grb::matrix_entry<T, I> b) {
  grb::matrix_entry<T, I> other = a;
  a = b;
  b = other;
}

template <std::size_t Index, typename T, typename I>
struct tuple_element<Index, grb::matrix_entry<T, I>>
    : tuple_element<Index, std::tuple<grb::index<I>, T>> {};

template <typename T, typename I>
struct tuple_size<grb::matrix_entry<T, I>> : integral_constant<size_t, 2> {};

} // namespace std

namespace grb {

template <typename T, typename I = std::size_t, typename TRef = T&>
class matrix_ref {
public:
  using scalar_type = T;
  using index_type = I;

  using key_type = grb::index<I>;
  using map_type = T;

  using scalar_reference = TRef;

  using value_type = grb::matrix_entry<T, I>;

  matrix_ref(grb::index<I> index, scalar_reference value)
      : index_(index), value_(value) {}

  operator value_type() const noexcept {
    return value_type(index_, value_);
  }

  operator std::pair<std::pair<I, I>, T>() const noexcept {
    return {{index_[0], index_[1]}, value_};
  }

  template <std::size_t Index>
    requires(Index <= 1)
  decltype(auto) get() const noexcept {
    if constexpr (Index == 0) {
      return index();
    }
    if constexpr (Index == 1) {
      return value();
    }
  }

  grb::index<I> index() const noexcept {
    return index_;
  }

  scalar_reference value() const noexcept {
    return value_;
  }

  template <std::integral U>
    requires(!std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator matrix_ref<T, U, TRef>() const noexcept {
    return matrix_ref<T, U, TRef>(index_, value_);
  }

  template <std::integral U>
    requires(!std::is_const_v<T> && !std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator matrix_ref<std::add_const_t<T>, U, TRef>() const noexcept {
    return matrix_ref<std::add_const_t<T>, U, TRef>(index_, value_);
  }

  bool operator<(matrix_entry<T, I> other) const noexcept {
    if (index()[0] < other.index()[0]) {
      return true;
    } else if (index()[0] == other.index()[0] &&
               index()[1] < other.index()[1]) {
      return true;
    }
    return false;
  }

  matrix_ref() = delete;
  ~matrix_ref() = default;

  matrix_ref(const matrix_ref&) = default;
  matrix_ref& operator=(const matrix_ref&) = delete;
  matrix_ref(matrix_ref&&) = default;
  matrix_ref& operator=(matrix_ref&&) = default;

private:
  grb::index<I> index_;
  scalar_reference value_;
};

} // namespace grb

namespace std {

template <typename T, typename I, typename TRef>
  requires(!std::is_const_v<T>)
void swap(grb::matrix_ref<T, I, TRef> a, grb::matrix_ref<T, I, TRef> b) {
  grb::matrix_entry<T, I> other = a;
  a = b;
  b = other;
}

template <std::size_t Index, typename T, typename I, typename TRef>
struct tuple_element<Index, grb::matrix_ref<T, I, TRef>>
    : tuple_element<Index, std::tuple<grb::index<I>, TRef>> {};

template <typename T, typename I, typename TRef>
struct tuple_size<grb::matrix_ref<T, I, TRef>>
    : integral_constant<std::size_t, 2> {};

template <std::size_t Index, typename T, typename I, typename TRef>
inline decltype(auto) get(grb::matrix_ref<T, I, TRef> ref)
  requires(Index <= 1)
{
  if constexpr (Index == 0) {
    return ref.index();
  }
  if constexpr (Index == 1) {
    return ref.value();
  }
}

template <std::size_t Index, typename T, typename I>
inline decltype(auto) get(grb::matrix_entry<T, I> entry)
  requires(Index <= 1)
{
  if constexpr (Index == 0) {
    return entry.index();
  }
  if constexpr (Index == 1) {
    return entry.value();
  }
}

} // namespace std
