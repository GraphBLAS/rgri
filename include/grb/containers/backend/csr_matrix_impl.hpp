
#pragma once

#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/csr_matrix.hpp>

namespace grb {

template <typename T,
          typename I,
          typename Allocator>
csr_matrix_impl_iterator<csr_matrix_impl_<T, I, Allocator>>
csr_matrix_impl_<T, I, Allocator>::find(index_t index) const {
  size_type row = index[0];
  for (index_type idx = rowptr_[row]; idx < rowptr_[row+1]; idx++) {
    if (colind_[idx] == index[1]) {
      return iterator(*const_cast<csr_matrix_impl_*>(this), row, idx);
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
  assign_tuples(matrix.matrix_);
}

// NOTE: tuples must contain unique indices inside the bounds of the matrix.
template <typename T,
          typename I,
          typename Allocator>
void
csr_matrix_impl_<T, I, Allocator>::assign_tuples(const tuples_type& tuples)
{
  nnz_ = tuples.size();
  rowptr_.resize(m()+1);
  colind_.resize(nnz());
  values_.resize(nnz());

  rowptr_[0] = 0;
  
  size_type r = 0;
  size_type c = 0;
  for (const auto& tuple : tuples) {
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

// NOTE: tuples must contain unique indices inside the bounds of the matrix.
template <typename T,
          typename I,
          typename Allocator>
void
csr_matrix_impl_<T, I, Allocator>::assign_tuples(const std::vector<std::tuple<index_type, index_type, value_type>>& tuples)
{
  nnz_ = tuples.size();
  rowptr_.resize(m()+1);
  colind_.resize(nnz());
  values_.resize(nnz());

  rowptr_[0] = 0;
  
  size_type r = 0;
  size_type c = 0;
  for (const auto& tuple : tuples) {
    value_type v = std::get<2>(tuple);
    size_type i = std::get<0>(tuple);
    size_type j = std::get<1>(tuple);

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


// NOTE: new_tuples must be unique indices that do not occur
//       in the sparse matrix.
template <typename T,
          typename I,
          typename Allocator>
void
csr_matrix_impl_<T, I, Allocator>::insert_tuples(tuples_type& new_tuples) {
  // a < b
  auto sort_fn = [](auto a, auto b) {
                   if (std::get<1>(a) < std::get<1>(b)) {
                     return true;
                   }
                   else if (std::get<1>(a) == std::get<1>(b)) {
                     if (std::get<2>(a) < std::get<2>(b)) {
                      return true;
                     }
                   }
                   return false;
                 };

  std::sort(new_tuples.begin(), new_tuples.end(), sort_fn);

  tuples_type current_tuples;
  current_tuples.reserve(nnz());
  for (auto iter = begin(); iter != end(); ++iter) {
    float value = *iter;
    index_t idx = *iter;
    current_tuples.push_back({value, idx[0], idx[1]});
  }

  std::sort(current_tuples.begin(), current_tuples.end(), sort_fn);

  tuples_type output_tuples(current_tuples.size() + new_tuples.size());

  std::merge(current_tuples.begin(), current_tuples.end(),
             new_tuples.begin(), new_tuples.end(),
             output_tuples.begin(), sort_fn);

  assign_tuples(output_tuples);
}

// Note: undefined (?) which element will be inserted if equal elements already exist
template <typename T,
          typename I,
          typename Allocator>
template <typename InputIt>
void
csr_matrix_impl_<T, I, Allocator>::insert(InputIt first, InputIt last) {
  // a < b
  auto sort_fn = [](auto a, auto b) {
                   if (std::get<0>(a) < std::get<0>(b)) {
                     return true;
                   }
                   else if (std::get<0>(a) == std::get<0>(b)) {
                     if (std::get<1>(a) < std::get<1>(b)) {
                      return true;
                     }
                   }
                   return false;
                 };

  std::vector<std::tuple<index_type, index_type, value_type>> merged_tuples(nnz() + (last - first));
  std::set_union(begin(), end(), first, last, merged_tuples.begin(), sort_fn);

  assign_tuples(merged_tuples);
}

template <typename T,
          typename I,
          typename Allocator>
csr_matrix_impl_<T, I, Allocator>::csr_matrix_impl_(index_t shape)
 : m_(shape[0]), n_(shape[1]), nnz_(0) {
  rowptr_.resize(m()+1, 0);
}

} // end grb