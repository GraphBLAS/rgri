
#pragma once

#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/csr_matrix.hpp>

namespace grb {

template <typename T,
          typename I,
          typename Allocator>
csr_matrix_impl_iterator<csr_matrix_impl_<T, I, Allocator>>
csr_matrix_impl_<T, I, Allocator>::find(index_t index) {
  size_type row = index[0];
  for (index_type idx = rowptr_[row]; idx < rowptr_[row+1]; idx++) {
    if (colind_[idx] == index[1]) {
      return iterator(*this, row, idx);
    }
  }
  return end();
}

template <typename T,
          typename I,
          typename Allocator>
csr_matrix_impl_<T, I, Allocator>::csr_matrix_impl_(const grb::import_matrix_type_<T, I>& matrix)
  : m_(matrix.m_), n_(matrix.m_), nnz_(matrix.nnz_)
{
  rowptr_.resize(m()+1);
  colind_.resize(nnz());
  values_.resize(nnz());

  rowptr_[0] = 0;
  
  size_type r = 0;
  size_type c = 0;
  for (const auto& tuple : matrix.matrix_) {
    value_type v = std::get<0>(tuple);
    size_type i = std::get<1>(tuple);
    size_type j = std::get<2>(tuple);

    values_[c] = v;
    colind_[c] = j;

    while (r < i) {
      if (r+1 > m_) {
        // TODO better exception
        throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
      }
      rowptr_[r+1] = c;
      r++;
    }
    c++;

    if (c > nnz_) {
      // TODO better exception
      throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
    }
  }

  for ( ; r < m_; r++) {
    rowptr_[r+1] = nnz_;
  }
}

} // end grb