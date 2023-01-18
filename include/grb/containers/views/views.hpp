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

#include <grb/containers/views/all.hpp>
#include <grb/containers/views/full_matrix_view.hpp>
#include <grb/containers/views/full_vector_view.hpp>
#include <grb/containers/views/transpose_matrix.hpp>
#include <grb/containers/views/transform_matrix.hpp>
#include <grb/containers/views/filter_matrix.hpp>
#include <grb/containers/views/submatrix_view.hpp>
#include <grb/containers/views/complement_view.hpp>
#include <grb/containers/views/masked_view.hpp>
#include <grb/containers/views/traditional_formats/csr_matrix_view.hpp>

#if __has_include(<experimental/mdspan>)
#include <grb/containers/views/third_party/mdspan_view.hpp>
#endif