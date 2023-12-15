#pragma once

#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/util/index.hpp>
#include <iterator>

namespace grb {

template <typename T, typename I, typename TIter = T*, typename IIter = I*>
class csr_matrix_row_accessor {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using scalar_type = std::iter_value_t<TIter>;
  using scalar_reference = std::iter_reference_t<TIter>;

  using index_type = I;

  using value_type = grb::matrix_entry<scalar_type, I>;

  using reference = grb::matrix_ref<T, I, scalar_reference>;

  using iterator_category = std::random_access_iterator_tag;

  using iterator_accessor = csr_matrix_row_accessor;
  using const_iterator_accessor = iterator_accessor;
  using nonconst_iterator_accessor = iterator_accessor;

  using key_type = grb::index<I>;

  constexpr csr_matrix_row_accessor() noexcept = default;
  constexpr ~csr_matrix_row_accessor() noexcept = default;
  constexpr csr_matrix_row_accessor(const csr_matrix_row_accessor&) noexcept =
      default;
  constexpr csr_matrix_row_accessor&
  operator=(const csr_matrix_row_accessor&) noexcept = default;

  constexpr csr_matrix_row_accessor(TIter values, IIter colind, index_type row,
                                    size_type idx) noexcept
      : values_(values), colind_(colind), row_(row), idx_(idx) {}

  constexpr csr_matrix_row_accessor&
  operator+=(difference_type offset) noexcept {
    idx_ += offset;
    return *this;
  }

  constexpr bool operator==(const iterator_accessor& other) const noexcept {
    return idx_ == other.idx_;
  }

  constexpr difference_type
  operator-(const iterator_accessor& other) const noexcept {
    return difference_type(idx_) - difference_type(other.idx_);
  }

  constexpr bool operator<(const iterator_accessor& other) const noexcept {
    return idx_ < other.idx_;
  }

  constexpr reference operator*() const noexcept {
    return reference(key_type(row_, colind_[idx_]), values_[idx_]);
  }

private:
  TIter values_;
  IIter colind_;
  index_type row_;
  size_type idx_;
};

template <typename T, typename I, typename TIter, typename IIter>
using csr_matrix_row_iterator =
    grb::detail::iterator_adaptor<csr_matrix_row_accessor<T, I, TIter, IIter>>;

template <typename T, typename I, typename TIter, typename IIter>
class csr_matrix_row_view {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using scalar_type = T;
  using index_type = I;

  using scalar_reference = std::iter_reference_t<TIter>;

  using key_type = grb::index<I>;
  using map_type = T;

  using iterator = csr_matrix_row_iterator<T, I, TIter, IIter>;

  csr_matrix_row_view(TIter values, IIter colind, index_type row,
                      size_type size)
      : values_(values), colind_(colind), row_(row), size_(size) {}

  scalar_reference operator[](size_type idx) {
    return values_[idx];
  }

  iterator begin() const {
    return iterator(values_, colind_, row_, 0);
  }

  iterator end() const {
    return iterator(values_, colind_, row_, size_);
  }

  size_type size() const noexcept {
    return size_;
  }

  TIter values_;
  IIter colind_;
  index_type row_;
  size_type size_;
};

template <std::random_access_iterator TIter, std::random_access_iterator IIter,
          typename... Args>
csr_matrix_row_view(TIter, IIter, Args&&...)
    -> csr_matrix_row_view<std::iter_value_t<TIter>, std::iter_value_t<IIter>,
                           TIter, IIter>;

template <typename T, typename I, typename TIter = T*, typename IIter = I*>
class csr_matrix_view_accessor {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using scalar_type = std::iter_value_t<TIter>;
  using scalar_reference = std::iter_reference_t<TIter>;

  using index_type = I;

  using value_type = grb::matrix_entry<scalar_type, I>;

  using reference = grb::matrix_ref<T, I, scalar_reference>;

  using iterator_category = std::random_access_iterator_tag;

  using iterator_accessor = csr_matrix_view_accessor;
  using const_iterator_accessor = iterator_accessor;
  using nonconst_iterator_accessor = iterator_accessor;

  using key_type = grb::index<I>;

  constexpr csr_matrix_view_accessor() noexcept = default;
  constexpr ~csr_matrix_view_accessor() noexcept = default;
  constexpr csr_matrix_view_accessor(const csr_matrix_view_accessor&) noexcept =
      default;
  constexpr csr_matrix_view_accessor&
  operator=(const csr_matrix_view_accessor&) noexcept = default;

  constexpr csr_matrix_view_accessor(TIter values, IIter rowptr, IIter colind,
                                     size_type idx, index_type row,
                                     size_type row_dim) noexcept
      : values_(values), rowptr_(rowptr), colind_(colind), idx_(idx), row_(row),
        row_dim_(row_dim) {
    fast_forward_row();
  }

  // Given that `idx_` has just been advanced to an element
  // possibly in a new row, advance `row_` to find the new row.
  // That is:
  // Advance `row_` until idx_ >= rowptr_[row_] && idx_ < rowptr_[row_+1]
  void fast_forward_row() noexcept {
    while (row_ < row_dim_ - 1 && idx_ >= rowptr_[row_ + 1]) {
      row_++;
    }
  }

  // Given that `idx_` has just been retreated to an element
  // possibly in a previous row, retreat `row_` to find the new row.
  // That is:
  // Retreat `row_` until idx_ >= rowptr_[row_] && idx_ < rowptr_[row_+1]
  void fast_backward_row() noexcept {
    while (idx_ < rowptr_[row_]) {
      row_--;
    }
  }

  constexpr csr_matrix_view_accessor&
  operator+=(difference_type offset) noexcept {
    idx_ += offset;
    if (offset < 0) {
      fast_backward_row();
    } else {
      fast_forward_row();
    }
    return *this;
  }

  constexpr bool operator==(const iterator_accessor& other) const noexcept {
    return idx_ == other.idx_;
  }

  constexpr difference_type
  operator-(const iterator_accessor& other) const noexcept {
    return difference_type(idx_) - difference_type(other.idx_);
  }

  constexpr bool operator<(const iterator_accessor& other) const noexcept {
    return idx_ < other.idx_;
  }

  constexpr reference operator*() const noexcept {
    return reference(key_type(row_, colind_[idx_]), values_[idx_]);
  }

private:
  TIter values_;
  IIter rowptr_;
  IIter colind_;
  size_type idx_;
  index_type row_;
  size_type row_dim_;
};

template <typename T, typename I, typename TIter, typename IIter>
using csr_matrix_view_iterator =
    grb::detail::iterator_adaptor<csr_matrix_view_accessor<T, I, TIter, IIter>>;

template <typename T, typename I, typename TIter = T*, typename IIter = I*>
class csr_matrix_view {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using scalar_reference = std::iter_reference_t<TIter>;

  using scalar_type = T;
  using index_type = I;

  using key_type = grb::index<I>;
  using map_type = T;

  using iterator = csr_matrix_view_iterator<T, I, TIter, IIter>;

  csr_matrix_view(TIter values, IIter rowptr, IIter colind, key_type shape,
                  size_type nnz)
      : values_(values), rowptr_(rowptr), colind_(colind), shape_(shape),
        nnz_(nnz) {}

  key_type shape() const noexcept {
    return shape_;
  }

  size_type size() const noexcept {
    return nnz_;
  }

  iterator begin() const {
    return iterator(values_, rowptr_, colind_, 0, 0, shape()[1]);
  }

  iterator end() const {
    return iterator(values_, rowptr_, colind_, nnz_, shape()[1], shape()[1]);
  }

  auto values_data() const {
    return values_;
  }

  auto rowptr_data() const {
    return rowptr_;
  }

  auto colind_data() const {
    return colind_;
  }

  iterator find(const key_type& key) const {
    index_type i = key[0];
    for (index_type j_ptr = rowptr_[i]; j_ptr < rowptr_[i + 1]; j_ptr++) {
      if (colind_[j_ptr] == key[1]) {
        return iterator(values_, rowptr_, colind_, j_ptr, i, shape()[1]);
      }
    }
    return end();
  }

  auto row(index_type row_index) const {
    auto offset = rowptr_[row_index];
    auto size = rowptr_[row_index + 1] - offset;
    return csr_matrix_row_view(values_ + offset, colind_ + offset, row_index,
                               size);
  }

  auto rows() const {
    auto row_indices =
        std::ranges::views::iota(index_type(0), index_type(shape()[0]));

    return row_indices |
           std::ranges::views::transform(
               [*this](index_type row_index) { return row(row_index); });
  }

private:
  TIter values_;
  IIter rowptr_;
  IIter colind_;
  size_type nnz_;

  key_type shape_;
};

template <typename TIter, typename IIter, typename... Args>
csr_matrix_view(TIter, IIter, IIter, Args&&...)
    -> csr_matrix_view<std::iter_value_t<TIter>, std::iter_value_t<IIter>,
                       TIter, IIter>;

} // namespace grb
