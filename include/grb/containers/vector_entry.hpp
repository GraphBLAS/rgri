#pragma once

namespace grb {

template <typename T, typename I = std::size_t>
class vector_entry {
public:
  using index_type = I;
  using map_type = T;

  vector_entry(index_type index, const map_type& value)
      : index_(index), value_(value) {}
  vector_entry(index_type index, map_type&& value)
      : index_(index), value_(std::move(value)) {}

  template <typename U>
    requires(std::is_constructible_v<T, U>)
  vector_entry(grb::index<I> index, U&& value)
      : value_(std::forward<U>(value)), index_(index) {}

  template <typename U>
  vector_entry(std::pair<U, T> tuple)
    requires(std::numeric_limits<I>::max() >= std::numeric_limits<U>::max())
      : value_(tuple.second), index_(tuple.first) {}

  template <std::size_t Index>
  auto get() const noexcept {
    if constexpr (Index == 0) {
      return index();
    }
    if constexpr (Index == 1) {
      return value();
    }
  }

  operator std::pair<I, T>() const noexcept {
    return {index_, value_};
  }

  index_type index() const noexcept {
    return index_;
  }

  map_type value() const noexcept {
    return value_;
  }

  template <std::integral U>
    requires(!std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator vector_entry<T, U>() const noexcept {
    return vector_entry<T, U>(index_, value_);
  }

  template <std::integral U>
    requires(!std::is_const_v<T> && !std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator vector_entry<std::add_const_t<T>, U>() const noexcept {
    return vector_entry<std::add_const_t<T>, U>(index_, value_);
  }

  bool operator<(const vector_entry& other) const noexcept {
    return index() < other.index();
  }

  vector_entry() = default;
  ~vector_entry() = default;

  vector_entry(const vector_entry&) = default;
  vector_entry& operator=(const vector_entry&) = default;
  vector_entry(vector_entry&&) = default;
  vector_entry& operator=(vector_entry&&) = default;

private:
  index_type index_;
  map_type value_;
};

} // namespace grb

namespace std {

template <typename T, typename I>
  requires(!std::is_const_v<T>)
void swap(grb::vector_entry<T, I> a, grb::vector_entry<T, I> b) {
  grb::vector_entry<T, I> other = a;
  a = b;
  b = other;
}

template <std::size_t Index, typename T, typename I>
struct tuple_element<Index, grb::vector_entry<T, I>>
    : tuple_element<Index, std::tuple<I, T>> {};

template <typename T, typename I>
struct tuple_size<grb::vector_entry<T, I>> : integral_constant<size_t, 2> {};

} // namespace std

namespace grb {

template <typename T, typename I = std::size_t, typename TRef = T&>
class vector_ref {
public:
  using scalar_type = T;
  using index_type = I;
  using value_type = grb::vector_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using scalar_reference = TRef;

  vector_ref(index_type index, scalar_reference value)
      : index_(index), value_(value) {}

  operator value_type() const noexcept {
    return value_type(index_, value_);
  }

  operator std::pair<I, T>() const noexcept {
    return {index_, value_};
  }

  template <std::size_t Index>
  decltype(auto) get() noexcept {
    if constexpr (Index == 0) {
      return index();
    }
    if constexpr (Index == 1) {
      return value();
    }
  }

  template <std::size_t Index>
  decltype(auto) get() const noexcept {
    if constexpr (Index == 0) {
      return index();
    }
    if constexpr (Index == 1) {
      return value();
    }
  }

  index_type index() const noexcept {
    return index_;
  }

  scalar_reference value() const noexcept {
    return value_;
  }

  template <std::integral U>
    requires(!std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator vector_ref<T, U>() const noexcept {
    return vector_ref<T, U>(index_, value_);
  }

  template <std::integral U>
    requires(!std::is_const_v<T> && !std::is_same_v<I, U> &&
             std::numeric_limits<U>::max() >= std::numeric_limits<I>::max())
  operator vector_ref<std::add_const_t<T>, U>() const noexcept {
    return vector_ref<std::add_const_t<T>, U>(index_, value_);
  }

  bool operator<(vector_entry<T, I> other) const noexcept {
    return index() < other.index();
  }

  vector_ref() = delete;
  ~vector_ref() = default;

  vector_ref(const vector_ref&) = default;
  vector_ref& operator=(const vector_ref&) = delete;
  vector_ref(vector_ref&&) = default;
  vector_ref& operator=(vector_ref&&) = delete;

private:
  index_type index_;
  scalar_reference value_;
};

} // namespace grb

namespace std {

template <typename T, typename I, typename TRef>
  requires(!std::is_const_v<T>)
void swap(grb::vector_ref<T, I, TRef> a, grb::vector_ref<T, I, TRef> b) {
  grb::vector_entry<T, I> other = a;
  a = b;
  b = other;
}

template <std::size_t Index, typename T, typename I, typename TRef>
struct tuple_element<Index, grb::vector_ref<T, I, TRef>>
    : tuple_element<Index, std::tuple<I, TRef>> {};

template <typename T, typename I, typename TRef>
struct tuple_size<grb::vector_ref<T, I, TRef>> : integral_constant<size_t, 2> {
};

} // namespace std
