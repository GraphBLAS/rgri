#pragma once

#include <binsparse/binsparse.hpp>
#include <grb/containers/backend/coo_matrix.hpp>

#include <grb/detail/ranges.hpp>

namespace grb {

template <typename T, typename I = std::size_t>
inline coo_matrix<T, I> binsparse_read_coo(std::string file_path) {
  auto coo = ::binsparse::read_coo_matrix<T, I>(file_path);

  __ranges::subrange values(coo.values, coo.values + coo.nnz);
  __ranges::subrange rowind(coo.rowind, coo.rowind + coo.nnz);
  __ranges::subrange colind(coo.colind, coo.colind + coo.nnz);

  auto tuples =
      __ranges::views::zip(__ranges::views::zip(rowind, colind), values);

  coo_matrix<T, I> matrix({coo.m, coo.n});

  matrix.insert(tuples.begin(), tuples.end());

  std::allocator<T>{}.deallocate(coo.values, coo.nnz);
  std::allocator<I>{}.deallocate(coo.rowind, coo.nnz);
  std::allocator<I>{}.deallocate(coo.colind, coo.nnz);

  return matrix;
}

} // namespace grb
