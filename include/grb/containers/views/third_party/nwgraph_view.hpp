#pragma once

#include <nwgraph/edge_list.hpp>
#include <ranges>

namespace grb {

/*
template <std::ranges::viewable_range R>
auto enumerate(R&& r) {
  using W = std::ranges::range_size_t<R>;
  return std::ranges::views::zip(std::ranges::views::iota(W{0}),
                                 std::forward<R>(r));
}

class enumerate_adapter_closure {
public:
  enumerate_adapter_closure() {}

  template <std::ranges::viewable_range R>
  auto operator()(R&& r) const {
    return enumerate(std::forward<R>(r));
  }

  template <std::ranges::viewable_range R>
  friend auto operator|(R&& r, const enumerate_adapter_closure& closure) {
    return enumerate(std::forward<R>(r));
  }
};

auto enumerate() {
  return enumerate_adapter_closure();
}
*/

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

  using scalar_reference =
      decltype(grb::get<2>(std::declval<std::ranges::range_reference_t<G>>()));
  using scalar_type = std::remove_cvref_t<scalar_reference>;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  nwgraph_view(G graph, key_type shape)
      : shape_(shape), graph_(std::forward<G>(graph)) {}

  nwgraph_view(G graph) : graph_(std::forward<G>(graph)) {
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
    return g | std::views::transform([&](auto&& entry) {
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

template <std::ranges::viewable_range G>
class nwgraph_adjacency_list_view_accessor {
public:
  using index_type = nw::graph::vertex_id_t<std::remove_cvref_t<G>>;
  using key_type = grb::index<index_type>;
  using size_type = std::ranges::range_size_t<G>;
  using difference_type = std::ranges::range_difference_t<G>;

  using scalar_reference =
      decltype(grb::get<1>(std::declval<std::ranges::range_reference_t<
                               std::ranges::range_reference_t<G>>>()));

  using scalar_type = std::remove_cvref_t<scalar_reference>;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  using iterator_category = std::forward_iterator_tag;

  using iterator_accessor = nwgraph_adjacency_list_view_accessor;
  using const_iterator_accessor = nwgraph_adjacency_list_view_accessor;
  using nonconst_iterator_accessor = nwgraph_adjacency_list_view_accessor;

  constexpr nwgraph_adjacency_list_view_accessor() noexcept = default;
  constexpr ~nwgraph_adjacency_list_view_accessor() noexcept = default;
  constexpr nwgraph_adjacency_list_view_accessor(
      const nwgraph_adjacency_list_view_accessor&) noexcept = default;
  constexpr nwgraph_adjacency_list_view_accessor&
  operator=(const nwgraph_adjacency_list_view_accessor&) noexcept = default;

  using inner_iterator = std::ranges::iterator_t<std::ranges::range_value_t<G>>;

  constexpr nwgraph_adjacency_list_view_accessor(G graph, std::size_t row,
                                                 inner_iterator col) noexcept
      : graph_(&graph), row_(row), col_(col) {}

  constexpr bool operator==(const iterator_accessor& other) const noexcept {
    return row_ == other.row_ && col_ == other.col_;
  }

  constexpr iterator_accessor& operator++() {
    ++col_;
    if (col_ == (*graph_)[row_].end() /* && row_ != graph_->size()-1 */) {
      ++row_;
      if (row_ != graph_->size() - 1) {
        col_ = (*graph_)[row_].begin();
      }
    }
    return *this;
  }

  constexpr reference operator*() const noexcept {
    auto&& [j, v] = *col_;
    return reference(key_type(row_, j), v);
  }

private:
  std::remove_reference_t<G>* graph_;
  size_type row_;
  inner_iterator col_;
};

template <typename G>
using nwgraph_adjacency_list_view_iterator =
    grb::detail::iterator_adaptor<nwgraph_adjacency_list_view_accessor<G>>;

template <typename G>
  requires(nw::graph::adjacency_list_graph<std::remove_reference_t<G>>)
class nwgraph_view<G> {
public:
  using index_type = nw::graph::vertex_id_t<std::remove_cvref_t<G>>;
  using key_type = grb::index<index_type>;
  using size_type = std::ranges::range_size_t<G>;
  using difference_type = std::ranges::range_difference_t<G>;

  using scalar_reference =
      decltype(grb::get<1>(std::declval<std::ranges::range_reference_t<
                               std::ranges::range_reference_t<G>>>()));

  using scalar_type = std::remove_cvref_t<scalar_reference>;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  using iterator = nwgraph_adjacency_list_view_iterator<G>;

  nwgraph_view(G graph, key_type shape)
      : shape_(shape), graph_(std::forward<G>(graph)) {}

  nwgraph_view(G graph) : graph_(std::forward<G>(graph)) {
    auto num_vertices = nw::graph::num_vertices(base());
    shape_ = key_type(num_vertices, num_vertices);
  }

  size_type size() const {
    size_type size_ = 0;
    for (auto&& row : base()) {
      size_ += std::ranges::size(row);
    }
    return size_;
  }

  key_type shape() const {
    return shape_;
  }

  auto find(const key_type& key) const {
    auto&& row = base()[key[0]];
    for (auto iter = row.begin(); iter != row.end(); ++iter) {
      auto&& [j, _] = *iter;
      if (j == key[1]) {
        return iterator(base(), key[0], iter);
      }
    }
    return end();
  }

  iterator begin() const {
    return iterator(base(), 0, base()[0].begin());
  }

  iterator end() const {
    return iterator(base(), graph_.size(), base()[graph_.size() - 1].end());
    // return iterator(base(), graph_.size()-1, base()[graph_.size()-1].end());
  }

private:
  decltype(auto) base() const {
    return graph_.base();
  }

  /*
    static inline auto matrix_view(auto&& graph) {
      return graph
             | enumerate()
             | std::views::transform(
                 [](auto&& row_entry) {
                   auto&& [i, row] = row_entry;
                   return row
                          | std::views::transform(
                              [=](auto&& entry) {
                                auto&& [j, v] = entry;
                                return reference(key_type(i, j), v);
                              });
                 })
             | std::views::join;
    }
    */

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

template <typename G>
auto tag_invoke(grb::tag_t<grb::views::all>, G&& graph)
  requires(nw::graph::adjacency_list_graph<std::remove_reference_t<G>>)
{
  return grb::nwgraph_view(std::forward<G>(graph));
}

} // namespace views

} // namespace grb
