#pragma once

#include <grb/containers/views/matrix_view.hpp>
#include <grb/detail/concepts.hpp>
#include <grb/detail/tag_invoke.hpp>

namespace grb {

namespace views {

inline constexpr struct all_fn_ {
  template <typename O>
  auto operator()(O&& x) const
    requires(grb::is_tag_invocable_v<all_fn_, O> || grb::MatrixRange<O>)
  {
    if constexpr (grb::is_tag_invocable_v<all_fn_, O>) {
      return grb::tag_invoke(*this, std::forward<O>(x));
    } else if constexpr (grb::MatrixRange<O>) {
      return grb::matrix_view(std::forward<O>(x));
    }
  }
} all{};

template <std::ranges::viewable_range R>
using all_t = decltype(views::all(std::declval<R>()));

} // namespace views

} // namespace grb
