
#pragma once

#include <grb/containers/backend/dense_matrix.hpp>
#include <grb/util/matrix_io.hpp>

namespace grb {

template <typename T, typename I, typename Allocator>
dense_matrix_impl_iterator<dense_matrix_impl_<T, I, Allocator>>
dense_matrix_impl_<T, I, Allocator>::find(index_t index) const {
  size_t idx = index[0] * n() + index[1];
  if (flags_[idx]) {
    return iterator(*const_cast<dense_matrix_impl_*>(this), idx);
  } else {
    return end();
  }
}

template <typename T, typename I, typename Allocator>
dense_matrix_impl_<T, I, Allocator>::dense_matrix_impl_(
    const grb::import_matrix_type_<T, I>& matrix)
    : m_(matrix.m_), n_(matrix.m_), nnz_(0) {
  assign_tuples(matrix.matrix_);
}

// NOTE: tuples must contain unique indices inside the bounds of the matrix.
template <typename T, typename I, typename Allocator>
void dense_matrix_impl_<T, I, Allocator>::assign_tuples(
    const tuples_type& tuples) {
  values_.assign(m() * n(), T());
  flags_.assign(m() * n(), false);

  insert_tuples(tuples);
}

// NOTE: new_tuples must be unique indices that do not occur
//       in the sparse matrix.
template <typename T, typename I, typename Allocator>
void dense_matrix_impl_<T, I, Allocator>::insert_tuples(
    const tuples_type& new_tuples) {
  for (const auto& tuple : new_tuples) {
    value_type value = std::get<0>(tuple);
    index_type i = std::get<1>(tuple);
    index_type j = std::get<2>(tuple);

    values_[i * n() + j] = value;
    flags_[i * n() + j] = true;
  }
  nnz_ += new_tuples.size();
}

template <typename T, typename I, typename Allocator>
template <typename InputIt>
void dense_matrix_impl_<T, I, Allocator>::insert(InputIt first, InputIt last) {
  size_t inserted = 0;
  for (auto iter = first; iter != last; iter++) {
    index_type i = std::get<0>(*iter);
    index_type j = std::get<1>(*iter);
    value_type value = std::get<2>(*iter);

    if (!flags_[i * n() + j]) {
      inserted++;
      flags_[i * n() + j] = true;
      values_[i * n() + j] = value;
    }
  }
  nnz_ += inserted;
}

template <typename T, typename I, typename Allocator>
dense_matrix_impl_<T, I, Allocator>::dense_matrix_impl_(index_t shape)
    : m_(shape[0]), n_(shape[1]), nnz_(0) {
  values_.assign(m() * n(), T());
  flags_.assign(m() * n(), false);
}

} // namespace grb
