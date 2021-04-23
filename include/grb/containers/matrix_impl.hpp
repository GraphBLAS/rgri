
#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include <regex.h>

namespace grb {

/*
template <typename T,
          typename I,
          typename Allocator>
matrix_iterator<matrix<T, I, Allocator>>
matrix<T, I, Allocator>::find(index_t index) {
  size_type row = index[0];
  for (index_type idx = rowptr_[row]; idx < rowptr_[row+1]; idx++) {
    if (colind_[idx] == index[1]) {
      return matrix_iterator<matrix>(*this, row, idx);
    }
  }
  return end();
}
*/

} // end grb