#pragma once

#include <grb/containers/views/filter_matrix.hpp>

namespace grb {

namespace views {

template <std::copy_constructible F>
class filter_adapter_closure {
public:
  filter_adapter_closure(F fn) : fn_(fn) {}

  template <MatrixRange M>
    requires(std::ranges::viewable_range<M>)
  auto operator()(M&& m) const {
    return filter_view(std::forward<M>(m), fn_);
  }

  template <VectorRange V>
    requires(std::ranges::viewable_range<V>)
  auto operator()(V&& v) const {
    return filter_view(std::forward<V>(v), fn_);
  }

  template <MatrixRange M>
    requires(std::ranges::viewable_range<M>)
  friend auto operator|(M&& m, const filter_adapter_closure& closure) {
    return closure(std::forward<M>(m));
  }

  template <VectorRange V>
    requires(std::ranges::viewable_range<V>)
  friend auto operator|(V&& v, const filter_adapter_closure& closure) {
    return closure(std::forward<V>(v));
  }

private:
  F fn_;
};

class filter_fn_ {
public:
  template <MatrixRange M, std::copy_constructible F>
    requires(std::ranges::viewable_range<M>)
  auto operator()(M&& m, F&& f) const {
    return filter_adapter_closure(std::forward<F>(f))(std::forward<M>(m));
  }

  template <VectorRange V, std::copy_constructible F>
    requires(std::ranges::viewable_range<V>)
  auto operator()(V&& v, F&& f) const {
    return filter_adapter_closure(std::forward<F>(f))(std::forward<V>(v));
  }

  template <std::copy_constructible F>
  auto operator()(F&& fn) const {
    return filter_adapter_closure(std::forward<F>(fn));
  }
};

inline constexpr auto filter = filter_fn_{};

} // namespace views

} // namespace grb
