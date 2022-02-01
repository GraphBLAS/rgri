#pragma once

#include <grb/detail/matrix_traits.hpp>
#include <grb/detail/monoid_traits.hpp>

namespace grb {

template <typename AMatrixType,
          typename BMatrixType,
          typename Fn,
          typename MaskType = grb::full_mask<>>
auto ewise_mult(AMatrixType&& a, BMatrixType&& b, Fn&& fn,
	              MaskType&& mask = grb::full_mask()) {
	assert(a.shape() == b.shape());

	using a_value_type = matrix_scalar_type_t<AMatrixType>;
	using b_value_type = matrix_scalar_type_t<BMatrixType>;

	using index_type = grb::bigger_integral_t<matrix_index_type_t<AMatrixType>,
	                                          matrix_index_type_t<BMatrixType>>;

	using c_value_type = decltype(std::forward<Fn>(fn)(std::declval<a_value_type>(), std::declval<b_value_type>()));

  grb::matrix<c_value_type, index_type> c(a.shape());

  for (auto&& [index, a_value] : a) {

    if constexpr(!std::is_same_v<std::decay_t<MaskType>, grb::full_mask<>>) {
    	auto mask_iter = mask.find(index);
    	if (mask_iter == mask.end() || !bool(std::get<1>(*mask_iter))) {
    		continue;
    	}
    }

  	auto iter = b.find(index);

  	if (iter != b.end()) {
  		auto&& [_, b_value] = *iter;

  		c.insert({index, std::forward<Fn>(fn)(a_value, b_value)});
	  }
  }

  return c;
}

template <typename AMatrixType,
          typename BMatrixType,
          typename Fn,
          typename T,
          typename MaskType = grb::full_mask<>>
auto ewise_add(AMatrixType&& a, BMatrixType&& b, Fn&& fn, T&& default_value,
	             MaskType&& mask = grb::full_mask<>()) {
	assert(a.shape() == b.shape());

	using a_value_type = matrix_scalar_type_t<AMatrixType>;
	using b_value_type = matrix_scalar_type_t<BMatrixType>;

	using index_type = grb::bigger_integral_t<matrix_index_type_t<AMatrixType>,
	                                          matrix_index_type_t<BMatrixType>>;

	using c_value_type = decltype(std::forward<Fn>(fn)(std::declval<a_value_type>(), std::declval<b_value_type>()));

	std::vector<bool> index_map(a.shape()[0]*a.shape()[1], false);

  grb::matrix<c_value_type, index_type> c(a.shape());

  size_t num_matched = 0;

	for (auto&& [index, a_value] : a) {

    if constexpr(!std::is_same_v<std::decay_t<MaskType>, grb::full_mask<>>) {
    	auto mask_iter = mask.find(index);
    	if (mask_iter == mask.end() || !bool(std::get<1>(*mask_iter))) {
    		continue;
    	}
    }

  	auto iter = b.find(index);

  	if (iter != b.end()) {
  		auto&& [_, b_value] = *iter;

  		c.insert({index, std::forward<Fn>(fn)(a_value, b_value)});
  		index_map[index[0]*a.shape()[1] + index[1]] = true;
  		num_matched++;
  	} else {
  		c.insert({index, std::forward<Fn>(fn)(a_value, default_value)});
  	}
	}

	if (num_matched < b.size()) {
		for (auto&& [index, b_value] : b) {
			c.insert({index, std::forward<Fn>(fn)(default_value, b_value)});
		}
	}

	return c;
}


template <typename AMatrixType,
          typename BMatrixType,
          typename Fn>
auto ewise_add(AMatrixType&& a, BMatrixType&& b, Fn&& fn)
requires(std::is_same_v<matrix_scalar_type_t<AMatrixType>,
	                      matrix_scalar_type_t<BMatrixType>> &&
	       grb::is_monoid_v<Fn, matrix_scalar_type_t<AMatrixType>>)
{
	return ewise_add(std::forward<AMatrixType>(a),
						       std::forward<BMatrixType>(b),
						       std::forward<Fn>(fn),
						       grb::monoid_traits<Fn, matrix_scalar_type_t<AMatrixType>>::identity());
}

} // end grb