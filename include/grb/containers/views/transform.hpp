#pragma once

#include <grb/containers/views/transform_matrix.hpp>
#include <grb/containers/views/transform_vector.hpp>

namespace grb {

namespace views {

template <std::copy_constructible F>
class transform_adapter_closure {
public:
  transform_adapter_closure(F fn) : fn_(fn) {}

  template <MatrixRange M>
    requires(std::ranges::viewable_range<M>)
  auto operator()(M&& m) const {
    return transform_matrix_view(std::forward<M>(m), fn_);
  }

  template <VectorRange V>
    requires(std::ranges::viewable_range<V>)
  auto operator()(V&& v) const {
    return transform_vector_view(std::forward<V>(v), fn_);
  }

  template <MatrixRange M>
    requires(std::ranges::viewable_range<M>)
  friend auto operator|(M&& m, const transform_adapter_closure& closure) {
    return closure(std::forward<M>(m));
  }

  template <VectorRange V>
    requires(std::ranges::viewable_range<V>)
  friend auto operator|(V&& v, const transform_adapter_closure& closure) {
    return closure(std::forward<V>(v));
  }

private:
  F fn_;
};

class transform_fn_ {
public:
  template <MatrixRange M, std::copy_constructible F>
    requires(std::ranges::viewable_range<M>)
  auto operator()(M&& m, F&& f) const {
    return transform_adapter_closure(std::forward<F>(f))(std::forward<M>(m));
  }

  template <VectorRange V, std::copy_constructible F>
    requires(std::ranges::viewable_range<V>)
  auto operator()(V&& v, F&& f) const {
    return transform_adapter_closure(std::forward<F>(f))(std::forward<V>(v));
  }

  template <std::copy_constructible F>
  auto operator()(F&& fn) const {
    return transform_adapter_closure(std::forward<F>(fn));
  }
};

inline constexpr auto transform = transform_fn_{};

template <typename ContainerType>
  requires(std::ranges::viewable_range<ContainerType>)
auto structure(ContainerType&& c) {
  auto always_true = [](auto&&) -> bool { return true; };
  return grb::views::transform(std::forward<ContainerType>(c), always_true);
}

} // namespace views

} // namespace grb
