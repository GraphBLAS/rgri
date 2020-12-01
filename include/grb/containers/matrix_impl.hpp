
#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include <regex.h>

namespace grb {

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

// TODO: this Matrix Market reader is experimental, and should be replaced.

template <typename T,
          typename I,
          typename Allocator>
void matrix<T, I, Allocator>::load_matrix(const std::string& fname, bool one_indexed) {
   std::ifstream f;

   f.open(fname.c_str());

   if (!f.is_open()) {
     throw std::runtime_error("CSRMatrix::read_MatrixMarket cannot open " + fname);
   }

   std::string buf;

   bool outOfComments = false;

   while (!outOfComments) {
     getline(f, buf);
     regex_t regex;
     int reti;

     reti = regcomp(&regex, "^%", 0);
     reti = regexec(&regex, buf.c_str(), 0, NULL, 0);

     if (reti == REG_NOMATCH) {
       outOfComments = true;
     }
   }

   size_t m, n, nnz;
   sscanf(buf.c_str(), "%lu %lu %lu", &m, &n, &nnz);

   m_ = m;
   n_ = n;
   nnz_ = nnz;

   values_.resize(nnz_);
   rowptr_.resize(m_+1);
   colind_.resize(nnz_);

   rowptr_[0] = 0;
   size_t r = 0;
   size_t c = 0;
   size_t i0 = 0;
   size_t j0 = 0;
   while (getline(f, buf)) {
     size_t i, j;
     double v;
     sscanf(buf.c_str(), "%lu %lu %lf", &i, &j, &v);
     if (one_indexed) {
       i--;
       j--;
     }

     if ((i == i0 && j < j0) || i < i0) {
       throw std::runtime_error("CSRMatrix::read_MatrixMarket " + fname + " is not sorted.");
     }
     i0 = i;
     j0 = j;

     assert(c < nnz_);

     values_[c] = v;
     colind_[c] = j;

     while (r < i) {
       if (r+1 >= m_+1) {
         printf("Trying to write to %lu >= %lu\n", r+1, m_+1);
         printf("Trying to write to %lu >= %lu indices %lu, %lu\n",
                r+1, m_+1,
                i, j);
         fflush(stdout);
       }
       assert(r+1 < m_+1);
       rowptr_[r+1] = c;
       r++;
     }
     c++;
   }

   for ( ; r < m_; r++) {
     rowptr_[r+1] = nnz_;
   }

   f.close();
}

} // end grb