#pragma once

#include <ranges>

namespace grb {

/*
template <typename MatrixType>
using indices_view = std::ranges::elements_view<MatrixType, 0>;

template <typename MatrixType>
using values_view = std::ranges::elements_view<MatrixType, 1>;
*/

namespace views {

inline constexpr auto indices =
    std::ranges::views::transform([](auto&& e) { return grb::get<0>(e); });
inline constexpr auto values =
    std::ranges::views::transform([](auto&& e) { return grb::get<1>(e); });

} // namespace views

} // namespace grb

#include <grb/containers/views/all.hpp>
#include <grb/containers/views/complement_view.hpp>
#include <grb/containers/views/filter.hpp>
#include <grb/containers/views/full_matrix_view.hpp>
#include <grb/containers/views/full_vector_view.hpp>
#include <grb/containers/views/masked_view.hpp>
#include <grb/containers/views/submatrix_view.hpp>
#include <grb/containers/views/traditional_formats/csr_matrix_view.hpp>
#include <grb/containers/views/transform.hpp>
#include <grb/containers/views/transpose_matrix.hpp>

#if __has_include(<experimental/mdspan>)
#include <grb/containers/views/third_party/mdspan_view.hpp>
#endif

#if USE_NWGRAPH
#include <grb/containers/views/third_party/nwgraph_view.hpp>
#endif
