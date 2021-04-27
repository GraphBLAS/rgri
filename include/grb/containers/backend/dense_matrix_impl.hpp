
#pragma once

#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/dense_matrix.hpp>

namespace grb {

template <typename T,
          typename I,
          typename Allocator>
dense_matrix_impl_iterator<dense_matrix_impl_<T, I, Allocator>>
dense_matrix_impl_<T, I, Allocator>::find(index_t index) {
  return iterator(*this, index[0]*n() + index[1]);
}

template <typename T,
          typename I,
          typename Allocator>
dense_matrix_impl_<T, I, Allocator>::dense_matrix_impl_(const grb::import_matrix_type_<T, I>& matrix)
 : m_(matrix.m_), n_(matrix.m_)
{
  assign_tuples(matrix.matrix_);
}

// NOTE: tuples must contain unique indices inside the bounds of the matrix.
template <typename T,
          typename I,
          typename Allocator>
void
dense_matrix_impl_<T, I, Allocator>::assign_tuples(const tuples_type& tuples)
{
  values_.assign(m()*n(), T());

  for (const auto& tuple : tuples) {
    value_type value = std::get<0>(tuple);
    index_type i = std::get<1>(tuple);
    index_type j = std::get<2>(tuple);

    values_[i*n() + j] = value;
  }
}


// NOTE: new_tuples must be unique indices that do not occur
//       in the sparse matrix.
template <typename T,
          typename I,
          typename Allocator>
void
dense_matrix_impl_<T, I, Allocator>::insert_tuples(tuples_type& new_tuples) {
  for (const auto& tuple : new_tuples) {
    value_type value = std::get<0>(tuple);
    index_type i = std::get<1>(tuple);
    index_type j = std::get<2>(tuple);

    values_[i*n() + j] = value;
  }
}

template <typename T,
          typename I,
          typename Allocator>
dense_matrix_impl_<T, I, Allocator>::dense_matrix_impl_(index_t shape)
 : m_(shape[0]), n_(shape[1]) {
  values_.assign(m()*n(), T());
}

} // end grb