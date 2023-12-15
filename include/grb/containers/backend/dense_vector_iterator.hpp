#pragma once

#include <grb/containers/backend/dense_vector_iterator.hpp>
#include <grb/containers/vector_entry.hpp>
#include <grb/detail/spanner.hpp>
#include <ranges>

namespace grb {

template <typename T, typename I, typename TIter, typename TConstIter,
          typename BIter>
class dense_vector_iterator {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using scalar_type = T;
  using index_type = I;

  using key_type = grb::index<I>;
  using map_type = T;

  using backend_iterator =
      std::conditional_t<!std::is_const_v<T>, TIter, TConstIter>;

  using value_type = grb::vector_entry<T, index_type>;
  using iterator = dense_vector_iterator;
  using const_iterator = dense_vector_iterator<std::add_const_t<T>, index_type,
                                               TIter, TConstIter, BIter>;

  using nonconst_iterator =
      dense_vector_iterator<std::remove_const_t<T>, index_type, TIter,
                            TConstIter, BIter>;

  using scalar_reference = decltype(*std::declval<TIter>());
  using const_scalar_reference = decltype(*std::declval<TConstIter>());

  using reference = grb::vector_ref<T, I, scalar_reference>;
  using const_reference =
      grb::vector_ref<std::add_const_t<T>, I, const_scalar_reference>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  using iterator_category = std::forward_iterator_tag;

  template <std::ranges::random_access_range R1,
            std::ranges::random_access_range R2>
  dense_vector_iterator(R1&& data, R2&& flags, index_type index)
      : data_(data), flags_(flags), index_(index) {
    fast_forward();
  }

  reference operator*() const noexcept
    requires(!std::is_const_v<T>)
  {
    return reference(index_, data_[index_]);
  }

  const_reference operator*() const noexcept
    requires(std::is_const_v<T>)
  {
    return const_reference(index_, data_[index_]);
  }

  void fast_forward() noexcept {
    while (!flags_[index_] && index_ < flags_.size()) {
      index_++;
    }
  }

  void increment() noexcept {
    index_++;
    fast_forward();
  }

  dense_vector_iterator& operator++() noexcept {
    increment();
    return *this;
  }

  dense_vector_iterator operator++(int) noexcept {
    dense_vector_iterator other = *this;
    increment();
    return other;
  }

  bool operator<(dense_vector_iterator other) const noexcept {
    return index_ < other.index_;
  }

  bool operator<=(dense_vector_iterator other) const noexcept {
    return index_ <= other.index_;
  }

  bool operator>(dense_vector_iterator other) const noexcept {
    return index_ > other.index_;
  }

  bool operator>=(dense_vector_iterator other) const noexcept {
    return index_ >= other.index_;
  }

  bool operator==(iterator other) const noexcept {
    return index_ == other.index_;
  }

  bool operator==(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return index_ == other.index_;
  }

  bool operator!=(iterator other) const noexcept {
    return !(*this == other);
  }

  bool operator!=(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return !(*this == other);
  }

  dense_vector_iterator() = default;
  ~dense_vector_iterator() = default;
  dense_vector_iterator(const dense_vector_iterator&) = default;
  dense_vector_iterator(dense_vector_iterator&&) = default;
  dense_vector_iterator& operator=(const dense_vector_iterator&) = default;
  dense_vector_iterator& operator=(dense_vector_iterator&&) = default;

private:
  grb::detail::spanner<backend_iterator> data_;
  grb::detail::spanner<BIter> flags_;

  index_type index_;
};

} // namespace grb
