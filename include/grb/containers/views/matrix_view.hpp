#pragma once

#include <grb/detail/concepts.hpp>

namespace grb {

template <grb::MatrixRange M>
class matrix_view {
public:
  using index_type = grb::matrix_index_t<M>;
  using key_type = typename grb::container_traits<M>::key_type;
  using value_type = std::ranges::range_value_t<M>;
  using scalar_type = grb::matrix_scalar_t<M>;
  using scalar_reference =
      decltype(grb::get<1>(std::declval<std::ranges::range_reference_t<M>>()));

  matrix_view(M matrix) : matrix_(matrix) {}

  auto begin() {
    return std::ranges::begin(base());
  }

  auto end() {
    return std::ranges::end(base());
  }

  auto find(const key_type& key) {
    return grb::find(base(), key);
  }

  auto insert(const value_type& entry) {
    return grb::insert(base(), entry);
  }

  auto size() {
    return std::ranges::size(base());
  }

  auto shape() {
    return grb::shape(base());
  }

  template <typename O>
  auto insert_or_assign(const key_type& key, O&& obj) {
    return grb::insert_or_assign(key, std::forward<O>(obj));
  }

  scalar_reference operator[](const key_type& key) {
    auto&& [iter, inserted] = insert(value_type(key, scalar_type{}));
    return grb::get<1>(*iter);
  }

  decltype(auto) base() noexcept {
    if constexpr (std::ranges::view<std::decay_t<M>>) {
      return matrix_;
    } else {
      return matrix_.base();
    }
  }

private:
  std::ranges::views::all_t<M> matrix_;
};

template <typename M>
matrix_view(M&& m) -> matrix_view<M>;

} // namespace grb
