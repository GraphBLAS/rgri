#pragma once

#include <grb/containers/matrix.hpp>
#include <vector>

namespace grb {

template <grb::MatrixRange M, std::integral I>
auto permute(M&& m, const std::vector<I>& permutation) {
  using T = grb::matrix_scalar_t<M>;
  using I2 = grb::matrix_index_t<M>;

  grb::matrix<T, I2> o({permutation.size(), permutation.size()});


  std::vector<std::vector<I>> proj(std::max(m.shape()[0], m.shape()[1]));

  for (std::size_t i = 0; i < proj.size(); i++) {
    proj[i].push_back({});
  }

  for (std::size_t i = 0; i < permutation.size(); i++) {
    proj[permutation[i]].push_back(i);
  }

  for (auto&& [idx, v] : m) {
    auto&& [i, j] = idx;

    for (auto&& i_ : proj[i]) {
      for (auto&& j_ : proj[j]) {
        o.insert({{I2(i_), I2(j_)}, v});
      }
    }
  }

  return o;
}

template <grb::MatrixRange M, std::integral I>
auto permute(M&& m,
             const std::vector<I>& row_permutation,
             const std::vector<I>& column_permutation) {
  using T = grb::matrix_scalar_t<M>;
  using I2 = grb::matrix_index_t<M>;

  grb::matrix<T, I2> o({row_permutation.size(), column_permutation.size()});

  std::vector<std::vector<I>> row_proj(m.shape()[0]);
  std::vector<std::vector<I>> col_proj(m.shape()[1]);

  for (std::size_t i = 0; i < row_proj.size(); i++) {
    row_proj[i].push_back({});
  }

  for (std::size_t i = 0; i < col_proj.size(); i++) {
    col_proj[i].push_back({});
  }

  for (std::size_t i = 0; i < row_permutation.size(); i++) {
    row_proj[row_permutation[i]].push_back(i);
  }

  for (std::size_t i = 0; i < column_permutation.size(); i++) {
    col_proj[column_permutation[i]].push_back(i);
  }

  for (auto&& [idx, v] : m) {
    auto&& [i, j] = idx;

    for (auto&& i_ : row_proj[i]) {
      for (auto&& j_ : col_proj[j]) {
        o.insert({{I2(i_), I2(j_)}, v});
      }
    }
  }

  return o;
}

} // end grb