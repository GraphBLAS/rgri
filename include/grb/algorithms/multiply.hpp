#pragma once

#include <type_traits>
#include <functional>
#include <utility>
#include <grb/detail/detail.hpp>

namespace grb {

/// Multiply two matrices
template <typename AMatrixType,
          typename BMatrixType,
          typename ReduceFn = std::plus<>,
          typename CombineFn = std::multiplies<>,
          typename MaskType = grb::full_mask
          >
auto multiply(AMatrixType&& a,
	            BMatrixType&& b,
	            ReduceFn&& reduce = ReduceFn(),
	            CombineFn&& combine = CombineFn(),
	            MaskType&& mask = grb::full_mask()) {
	using a_matrix = std::decay_t<AMatrixType>;
	using b_matrix = std::decay_t<BMatrixType>;

	using a_scalar_type = typename a_matrix::scalar_type;
	using b_scalar_type = typename b_matrix::scalar_type;

	using a_index_type = typename a_matrix::index_type;
	using b_index_type = typename b_matrix::index_type;

	using c_scalar_type = decltype(combine(std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

	using c_index_type = grb::bigger_integral_t<a_index_type, b_index_type>;

	grb::matrix<c_scalar_type, c_index_type> c({a.shape()[0], b.shape()[1]});

	for (auto&& [a_index, a_v] : a) {
		auto&& [i, k] = a_index;

		for (c_scalar_type j = 0; j < b.shape()[1]; j++) {
			auto iter = b.find({k, j});

			if (iter != b.end()) {
				auto&& [b_index, b_v] = *iter;

        if constexpr(std::is_same_v<std::decay_t<MaskType>, grb::full_mask>) {
				  c[{i, j}] = reduce(c[{i, j}], combine(a_v, b_v));
			  } else {
			  	auto iter = mask.find({i, j});
			  	if (iter != mask.end()) {
			  		auto&& [index, value] = *iter;
			  		if (bool(value)) {
				      c[{i, j}] = reduce(c[{i, j}], combine(a_v, b_v));
				    }
			  	}
			  }
			}
		}
	}

	return c;
}

} // end grb