#include <grb/grb.hpp>
#include <nwgraph/adjacency.hpp>
#include <nwgraph/edge_list.hpp>

int main(int argc, char** argv) {
  nw::graph::edge_list<nw::graph::directedness::directed, float> graph(0);

  grb::matrix<float, int> a("data/chesapeake.mtx");

  for (auto&& [index, v] : a) {
    auto&& [i, j] = index;
    graph.push_back(i, j, v);
  }

  // grb::nwgraph_view view(graph);
  auto view = grb::views::all(graph);

  auto c = grb::multiply(view, a);

  // grb::print(view, "View of NWGraph edge_list");

  nw::graph::indexed_struct_of_arrays<int, int, float> csr_graph(a.shape()[0]);

  csr_graph.open_for_push_back();
  for (auto&& [index, v] : a) {
    auto&& [i, j] = index;
    csr_graph.push_back(i, j, v);
  }
  csr_graph.close_for_push_back();

  // grb::nwgraph_view view2(csr_graph);
  auto view2 = grb::views::all(csr_graph);

  std::cout << view2.shape()[0] << " x " << view2.shape()[1] << std::endl;
  std::cout << view2.size() << " nnz" << std::endl;

  grb::print(view2);

  /*
    for (std::size_t i = 0; i < csr_graph.size(); ++i) {
      auto&& row = csr_graph[i];
      for (auto&& entry : row) {
        auto j = nw::graph::target(csr_graph, entry);

        auto&& [_, v] = entry;
        std::cout << i << ", " << j << ": " << v << std::endl;
      }
    }
    */

  return 0;
}
