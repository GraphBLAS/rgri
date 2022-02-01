#pragma once

#include <ranges>

namespace grb {

template <typename MatrixType>
using indices_view = std::ranges::elements_view<MatrixType, 0>;

template <typename MatrixType>
using values_view = std::ranges::elements_view<MatrixType, 1>;

namespace views {

inline constexpr auto indices = std::ranges::views::elements<0>;
inline constexpr auto values = std::ranges::views::elements<1>;

} // end views

} // end grb

#include <grb/containers/views/ones_view.hpp>
