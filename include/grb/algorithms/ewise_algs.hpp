#pragma once

namespace grb {

template <typename AMatrixType,
          typename BMatrixType,
          typename BinaryOp,
          typename Accumulator>
auto ewise(const AMatrixType& a,
	         const BMatrixType& b,
	         const BinaryOp& op,
	         const Accumulator& acc) {
	if (a.shape() != b.shape()) {
		throw grb::invalid_argument("ewise: matrix dimensions do not match.");
	}
	using a_value_type = typename AMatrixType::value_type;
	using b_value_type = typename BMatrixType::value_type;

	using c_value_type = decltype(op(std::declval<a_value_type>(), std::declval<b_value_type>()));

	using c_hint = typename grb::pick_ewise_hint<typename AMatrixType::hint_type, typename BMatrixType::hint_type>::type;

	// TODO: something better for c_index_type, allocator_type
	using c_index_type = typename AMatrixType::index_type;

	using c_allocator_type = typename AMatrixType::allocator_type;

	grb::matrix<c_value_type, c_index_type, c_hint, c_allocator_type> c(a.shape());

	ewise(a, b, c, op, acc);

	return c;
}

template <typename AMatrixType,
          typename BMatrixType,
          typename CMatrixType,
          typename BinaryOp,
          typename Accumulator>
void ewise(const AMatrixType& a,
	         const BMatrixType& b,
	         CMatrixType& c,
	         const BinaryOp& op,
	         const Accumulator& acc) {
	if (a.shape() != b.shape() || b.shape() != c.shape()) {
		throw grb::invalid_argument("ewise: matrix dimensions do not match.");
	}
	using a_value_type = typename AMatrixType::value_type;
	using b_value_type = typename BMatrixType::value_type;
	using c_value_type = typename CMatrixType::value_type;

  for (auto ref : a) {
  	auto [i, j] = ref.index();
  	a_value_type a_value = ref.value();
  	auto b_iter = b.find({i, j});
  	if (b_iter != b.end()) {
  		b_value_type b_value = *b_iter;
  		a_value = op(a_value, b_value);
  	}
  	auto c_iter = c.find({i, j});
  	if (c_iter == c.end()) {
  		c[{i, j}] = a_value;
  	} else {
  		c_value_type c_value = *c_iter;
  		*c_iter = acc(c_value, a_value);
  	}
  }
}

} // end grb