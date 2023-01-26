#pragma once

#include <grb/grb.hpp>
#include <nwgraph/edge_list.hpp>
#include <nwgraph/adjacency.hpp>

template <grb::MatrixRange M>
auto get_edge_list(M&& a) {
  nw::graph::edge_list<nw::graph::directedness::directed, float> graph(0);

  for (auto&& [index, v] : a) {
    auto&& [i, j] = index;
    graph.push_back(i, j, v);
  }

  return graph;
}

template <grb::MatrixRange M>
auto get_adjacency_list(M&& a) {
  nw::graph::indexed_struct_of_arrays<int, int, float> graph(a.shape()[0]);

  graph.open_for_push_back();
  for (auto&& [index, v] : a) {
    auto&& [i, j] = index;
    graph.push_back(i, j, v);
  }
  graph.close_for_push_back();

  return graph;
}

template <typename T, typename I>
auto get_csr_view(const grb::matrix<T, I, grb::sparse>& a) {
  auto values = a.backend_.values_.data();
  auto rowptr = a.backend_.rowptr_.data();
  auto colind = a.backend_.colind_.data();
  auto shape = a.shape();
  int nnz = a.size();

  return grb::csr_matrix_view(values, rowptr, colind, shape, nnz);
}
