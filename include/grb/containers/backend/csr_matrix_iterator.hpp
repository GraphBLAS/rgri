#pragma once

#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/spanner.hpp>
#include <span>
#include <type_traits>

namespace grb {

// NOTES:
// 1. This csr_matrix_iterator implements the std::random_access_iterator
//    concept.  HOWEVER, random access is not guaranteed in constant time,
//    since "fast forwarding" may be necessary if there are empty rows.
//    Random access is guaranteed in O(m) time, where m is the number of
//    rows in the sparse matrix.  In practice, random access is done in
//    close to constant time.
// 2. This iterator currently holds five data elements:
//    - std::span<T> with values
//    - std::span<I> with column indices
//    - std::span<I> with row ptrs
//    - I with current row
//    - I with index
//    Each span contains a value_type* pointer and a size.
//    We could reduce this to three pointers and also remove
//    a branch in fast_forward_row and fast_backward_row by storing
//    std::numeric_limits<I>::max() in location m + 2 (or m + 1?) in
//    the row_ptr array.

template <typename T, typename I, typename TIter, typename TConstIter,
          typename IIter>
class csr_matrix_iterator {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // using element_type = T;
  using scalar_type = T;
  using index_type = I;
  using map_type = T;

  using backend_iterator =
      std::conditional_t<!std::is_const_v<T>, TIter, TConstIter>;

  using value_type = grb::matrix_entry<T, index_type>;
  using iterator = csr_matrix_iterator;
  using const_iterator = csr_matrix_iterator<std::add_const_t<T>, index_type,
                                             TIter, TConstIter, IIter>;

  using nonconst_iterator =
      csr_matrix_iterator<std::remove_const_t<T>, index_type, TIter, TConstIter,
                          IIter>;

  using scalar_reference = decltype(*std::declval<TIter>());
  using const_scalar_reference = decltype(*std::declval<TConstIter>());

  using reference = grb::matrix_ref<T, I, scalar_reference>;
  using const_reference =
      grb::matrix_ref<std::add_const_t<T>, I, const_scalar_reference>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  using iterator_category = std::random_access_iterator_tag;

  template <std::ranges::random_access_range R1,
            std::ranges::random_access_range R2,
            std::ranges::random_access_range R3>
  csr_matrix_iterator(index_type row, index_type index, R1&& values,
                      R2&& rowptr, R3&& colind)
      : row_(row), index_(index), values_(values.begin(), values.end()),
        rowptr_(rowptr.begin(), rowptr.end()),
        colind_(colind.begin(), colind.end()) {
    fast_forward_row();
  }

  operator const_iterator() const noexcept {
    return const_iterator(row_, index_, values_, rowptr_, colind_);
  }

  // reference operator*() const noexcept
  reference operator*() const noexcept
    requires(!std::is_const_v<T>)
  {
    return reference({row_, colind_[index_]}, values_[index_]);
  }

  // const_reference operator*() const noexcept
  const_reference operator*() const noexcept
    requires(std::is_const_v<T>)
  {
    // return const_reference(row_, index_, values_, rowptr_, colind_);
    return const_reference({row_, colind_[index_]}, values_[index_]);
  }

  // Given that `index_` has just been advanced to an element
  // possibly in a new row, advance `row_` to find the new row.
  // That is:
  // Advance `row_` until index_ >= rowptr_[row_] && index_ < rowptr_[row_+1]
  void fast_forward_row() noexcept {
    while (row_ < rowptr_.size() - 1 && index_ >= rowptr_[row_ + 1]) {
      row_++;
    }
  }

  // Given that `index_` has just been retreated to an element
  // possibly in a previous row, retreat `row_` to find the new row.
  // That is:
  // Retreat `row_` until index_ >= rowptr_[row_] && index_ < rowptr_[row_+1]
  void fast_backward_row() noexcept {
    while (index_ < rowptr_[row_]) {
      row_--;
    }
  }

  void increment() noexcept {
    index_++;
    fast_forward_row();
  }

  void decrement() noexcept {
    index_--;
    fast_backward_row();
  }

  iterator& operator++() noexcept {
    increment();
    return *this;
  }

  iterator operator++(int) noexcept {
    iterator other = *this;
    increment();
    return other;
  }

  iterator& operator--() noexcept {
    decrement();
    return *this;
  }

  iterator operator--(int) noexcept {
    iterator other = *this;
    decrement();
    return other;
  }

  iterator& operator+=(difference_type n) noexcept {
    index_ += n;
    if (n < 0) {
      fast_backward_row();
    } else {
      fast_forward_row();
    }
    return *this;
  }

  iterator& operator-=(difference_type n) noexcept {
    index_ -= n;
    if (n > 0) {
      fast_backward_row();
    } else {
      fast_forward_row();
    }
    return *this;
  }

  iterator operator+(difference_type n) const noexcept {
    iterator other = *this;
    other += n;
    return other;
  }

  iterator operator-(difference_type n) const noexcept {
    iterator other = *this;
    other -= n;
    return other;
  }

  auto operator[](size_type n) const noexcept {
    return *(*this + n);
  }

  difference_type operator-(const_iterator other) const noexcept {
    return index_ - other.index_;
  }

  bool operator<(iterator other) const noexcept {
    return index_ < other.index_;
  }

  bool operator<(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return index_ < other.index_;
  }

  bool operator<=(iterator other) const noexcept {
    return index_ <= other.index_;
  }

  bool operator<=(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return index_ <= other.index_;
  }

  bool operator>(iterator other) const noexcept {
    return index_ > other.index_;
  }

  bool operator>(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return index_ > other.index_;
  }

  bool operator>=(iterator other) const noexcept {
    return index_ >= other.index_;
  }

  bool operator>=(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return index_ >= other.index_;
  }

  bool operator==(iterator other) const noexcept {
    return values_.begin() == other.values_.begin() &&
           rowptr_.begin() == other.rowptr_.begin() &&
           colind_.begin() == other.colind_.begin() && row_ == other.row_ &&
           index_ == other.index_;
  }

  bool operator==(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return values_.begin() == other.values_.begin() &&
           rowptr_.begin() == other.rowptr_.begin() &&
           colind_.begin() == other.colind_.begin() && row_ == other.row_ &&
           index_ == other.index_;
  }

  bool operator!=(iterator other) const noexcept {
    return !(*this == other);
  }

  bool operator!=(const_iterator other) const noexcept
    requires(!std::is_same_v<iterator, const_iterator>)
  {
    return !(*this == other);
  }

  csr_matrix_iterator() = default;
  ~csr_matrix_iterator() = default;
  csr_matrix_iterator(const csr_matrix_iterator&) = default;
  csr_matrix_iterator(csr_matrix_iterator&&) = default;
  csr_matrix_iterator& operator=(const csr_matrix_iterator&) = default;
  csr_matrix_iterator& operator=(csr_matrix_iterator&&) = default;

  friend csr_matrix_iterator operator+(difference_type n, iterator iter) {
    return iter + n;
  }

  friend const_iterator;
  friend nonconst_iterator;

private:
  using const_index_type = std::add_const_t<index_type>;

  grb::detail::spanner<backend_iterator> values_;
  grb::detail::spanner<IIter> rowptr_;
  grb::detail::spanner<IIter> colind_;

  index_type row_;
  index_type index_;
};

} // namespace grb
