#pragma once

#include <nwgraph/edge_list.hpp>

namespace grb {

template <typename T>
class nwgraph_view;

template <typename G>
requires(nw::graph::edge_list_graph<std::remove_reference_t<G>>)
class nwgraph_view<G> {
public:

  using index_type = nw::graph::vertex_id_t<std::remove_cvref_t<G>>;
  using key_type = grb::index<index_type>;
  using size_type = std::ranges::range_size_t<G>;
  using difference_type = std::ranges::range_difference_t<G>;

  using scalar_reference = decltype(grb::get<2>(std::declval<std::ranges::range_reference_t<G>>()));
  using scalar_type = std::remove_cvref_t<scalar_reference>;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  nwgraph_view(G graph, key_type shape)
    : shape_(shape), graph_(std::forward<G>(graph)) {}

  nwgraph_view(G graph)
    : graph_(std::forward<G>(graph)) {
    auto num_vertices = nw::graph::num_vertices(base());
    shape_ = key_type(num_vertices, num_vertices);
  }

  size_type size() const {
    return std::ranges::size(base());
  }

  key_type shape() const {
    return shape_;
  }

  auto find(const key_type& key) const {
    for (auto iter = begin(); iter != end(); ++iter) {
      auto&& [index, _] = *iter;
      if (index == key) {
        return iter;
      }
    }
    return end();
  }

  auto begin() const {
    return matrix_range().begin();
  }

  auto end() const {
    return matrix_range().end();
  }

private:
  auto matrix_range() const {
    auto&& g = base();
    return g
     | std::views::transform([&](auto&& entry) {
                               auto&& [i, j, v] = entry;
                               return reference(key_type(nw::graph::source(g, entry),
                                                         nw::graph::target(g, entry)),
                                                v);
                             });
  }

  decltype(auto) base() const {
    return graph_.base();
  }

private:
  key_type shape_;
  std::ranges::views::all_t<G> graph_;
};

template <typename G, typename T>
nwgraph_view(G&&, T&&) -> nwgraph_view<G>;

template <typename G>
nwgraph_view(G&&) -> nwgraph_view<G>;

namespace views {

template <typename G>
auto tag_invoke(grb::tag_t<grb::views::all>, G&& graph)
requires(nw::graph::edge_list_graph<std::remove_reference_t<G>>)
{
  return grb::nwgraph_view(std::forward<G>(graph));
}

} // end views

} // end grb



