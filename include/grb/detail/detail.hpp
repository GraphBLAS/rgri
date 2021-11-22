#pragma once

#include <type_traits>
#include <limits>

namespace grb {

struct full_mask {};

template <std::integral I, std::integral U>
struct bigger_integral_impl_ {
	using type = I;
};

template <std::integral I, std::integral U>
requires(std::numeric_limits<I>::max() < std::numeric_limits<U>::max())
struct bigger_integral_impl_<I, U> {
	using type = U;
};

template <std::integral I, std::integral U>
using bigger_integral_t = typename bigger_integral_impl_<I, U>::type;

} // end grb