#include <grb/grb.hpp>
#include <nwgraph/edge_list.hpp>

template <grb::MatrixRange M>
void matrix(M&&) {}

int main(int argc, char** argv) {
  nw::graph::edge_list<nw::graph::directedness::undirected, float> graph(0);

  grb::matrix<float, int> a("data/chesapeake.mtx");

  for (auto&& [index, v] : a) {
    auto&& [i, j] = index;
    graph.push_back(i, j, v);
  }

  grb::nwgraph_view view(graph);

  for (auto&& [idx, v] : view) {
    auto&& [i, j] = idx;
    std::cout << i << ", " << j << ": " << v << std::endl;
  }

  return 0;
}