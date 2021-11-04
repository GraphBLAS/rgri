
#pragma once

#include <grb/util/index.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/csr_matrix_iterator.hpp>
#include <limits>
#include <climits>

namespace grb {

template <typename T,
          typename I = std::size_t,
          typename Allocator = std::allocator<T>>
requires(std::is_integral_v<I>)
class csr_matrix {
public:
  using index_type = I;
  using value_type = grb::matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;
  using index_allocator_type = typename std::allocator_traits<allocator_type>::rebind_alloc<index_type>;

  using iterator = csr_matrix_iterator<T,
                                       index_type>;

  using const_iterator = csr_matrix_iterator<std::add_const_t<T>,
                                             index_type>;

  using reference = grb::matrix_ref<T, index_type>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, index_type>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  iterator begin() noexcept {
    return iterator(0, 0, values_, rowptr_, colind_);
  }

  const_iterator begin() const noexcept {
    return const_iterator(0, 0, values_, rowptr_, colind_);
  }

  iterator end() noexcept {
    return iterator(shape()[0], size(), values_, rowptr_, colind_);
  }

  const_iterator end() const noexcept {
    return const_iterator(shape()[0], size(), values_, rowptr_, colind_);
  }

  grb::index<I> shape() const noexcept {
    return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

  template <typename InputIt>
  void insert(InputIt first, InputIt last);

  std::pair<iterator, bool> insert(value_type&& value);

  template <class M>
  std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj);

  iterator find(key_type key) noexcept;
  const_iterator find(key_type key) const noexcept;

  csr_matrix(grb::index<I> shape);

  csr_matrix() = default;
  ~csr_matrix() = default;
  csr_matrix(const csr_matrix&) = default;
  csr_matrix(csr_matrix&&) = default;
  csr_matrix& operator=(const csr_matrix&) = default;
  csr_matrix& operator=(csr_matrix&&) = default;

private:

  template <typename InputIt>
  void assign_tuples(InputIt first, InputIt last);

  index_type m_ = 0;
  index_type n_ = 0;
  size_type nnz_ = 0;

  std::vector<index_type, index_allocator_type> rowptr_;
  std::vector<index_type, index_allocator_type> colind_;
  std::vector<T, allocator_type> values_;
};

template <typename T,
          typename I,
          typename Allocator>
csr_matrix<T, I, Allocator>::csr_matrix(index<I> shape)
 : m_(shape[0]), n_(shape[1]), nnz_(0) {
  if (shape[0]+1 > std::numeric_limits<I>::max() ||
      shape[1]+1 > std::numeric_limits<I>::max()) {
    throw std::runtime_error("grb::matrix({"                    +
                             std::to_string(shape[0])           +
                             std::to_string(shape[1])           +
                             "}: dimensions too big for "       +
                             std::to_string(sizeof(I)*CHAR_BIT) +
                             "-bit integer");
  }
  rowptr_.resize(shape[0]+1, 0);
}

// Refill the contents of the matrix so that it only contains the *sorted*
// tuples stored in `tuples`.  Tuples must be within the bounds of the current
// dimensions of the matrix.
template <typename T,
          typename I,
          typename Allocator>
template <typename InputIt>
void
csr_matrix<T, I, Allocator>::assign_tuples(InputIt first, InputIt last)
{
  nnz_ = last - first;
  rowptr_.resize(shape()[0]+1);
  colind_.resize(nnz_);
  values_.resize(nnz_);

  rowptr_[0] = 0;
  
  size_type r = 0;
  size_type c = 0;
  for (auto iter = first; iter != last; ++iter) {
    auto&& [index, value] = *iter;
    auto&& [i, j] = index;

    values_[c] = value;
    colind_[c] = j;

    while (r < i) {
      if (r+1 > m_) {
        // TODO: exception?
        // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
      }
      rowptr_[r+1] = c;
      r++;
    }
    c++;

    if (c > nnz_) {
      // TODO: exception?
      // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
    }
  }

  for ( ; r < m_; r++) {
    rowptr_[r+1] = nnz_;
  }
}


// Currently assumes NO DUPLICATES in range first -> last
template <typename T,
          typename I,
          typename Allocator>
template <typename InputIt>
void csr_matrix<T, I, Allocator>::insert(InputIt first, InputIt last) {
  // a < b
  auto sort_fn = [](const auto& a, const auto& b) {
                   auto&& [a_index, a_value] = a;
                   auto&& [b_index, b_value] = b;
                   auto&& [a_i, a_j] = a_index;
                   auto&& [b_i, b_j] = b_index;
                   if (a_i < b_i) {
                     return true;
                   }
                   else if (a_i == b_i) {
                     if (a_j < b_j) {
                      return true;
                     }
                   }
                   return false;
                 };

  using tuple_type = std::pair<std::pair<I, I>, T>;
  std::vector<tuple_type> my_sorted_indices(begin(), end());

  using input_tuple_type = std::iter_value_t<InputIt>;
  std::vector<input_tuple_type> sorted_indices_toadd(first, last);
  std::ranges::sort(my_sorted_indices, sort_fn);
  std::ranges::sort(sorted_indices_toadd, sort_fn);

  std::vector<grb::matrix_entry<T, I>> output_indices(my_sorted_indices.size() + sorted_indices_toadd.size());

  auto new_last = std::set_union(
                      my_sorted_indices.begin(), my_sorted_indices.end(),
                      sorted_indices_toadd.begin(), sorted_indices_toadd.end(),
                      output_indices.begin(), sort_fn);
/*
  std::vector<std::tuple<index_type, index_type, value_type>> merged_tuples(nnz() + (last - first));
  std::set_union(begin(), end(), first, last, merged_tuples.begin(), sort_fn);
  */

  assign_tuples(output_indices.begin(), new_last);
}


// NOTE: if sorted, you could exit this for loop early
template <typename T,
          typename I,
          typename Allocator>

typename csr_matrix<T, I, Allocator>::iterator
csr_matrix<T, I, Allocator>::find(key_type key) noexcept
{
  index_type i = key[0];
  for (index_type j_ptr = rowptr_[i]; j_ptr < rowptr_[i+1]; j_ptr++) {
    if (colind_[j_ptr] == key[1]) {
      return iterator(i, j_ptr, values_, rowptr_, colind_);
    }
  }
  return end();
}

template <typename T,
          typename I,
          typename Allocator>
typename csr_matrix<T, I, Allocator>::const_iterator
csr_matrix<T, I, Allocator>::find(key_type key) const noexcept
{
  index_type i = key[0];
  for (index_type j_ptr = rowptr_[i]; j_ptr < rowptr_[i+1]; j_ptr++) {
    if (colind_[j_ptr] == key[1]) {
      return const_iterator(i, j_ptr, values_, rowptr_, colind_);
    }
  }
  return end();
}

template <typename T,
          typename I,
          typename Allocator>
std::pair<typename csr_matrix<T, I, Allocator>::iterator, bool>
csr_matrix<T, I, Allocator>::insert(typename csr_matrix<T, I, Allocator>::value_type&& value) {
  auto&& [idx, v] = value;
  auto iter = find(idx);
  if (iter != end()) {
    return {iter, false};
  } else {
    insert(&value, &value + 1);
    auto iter = find(idx);
    return {iter, true};
  }
}

template <typename T,
          typename I,
          typename Allocator>
template <class M>
std::pair<typename csr_matrix<T, I, Allocator>::iterator, bool>
csr_matrix<T, I, Allocator>::insert_or_assign(csr_matrix<T, I, Allocator>::key_type k, M&& obj) {
  auto iter = find(k);

  if (iter != end()) {
    auto&& [index, value] = *iter;
    value = std::forward<M>(obj);
    return {iter, false};
  } else {
    auto&& [iter, flag] = insert({k, std::forward<M>(obj)});
    return {iter, true};
  }
}

} // end grb
