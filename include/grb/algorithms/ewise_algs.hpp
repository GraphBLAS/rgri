#pragma once

namespace grb {

/// Applies the binary operator `op` element-wise to GraphBLAS matrices
/// `a` and `b`.  Any valid binary function, including GraphBLAS's built-in
/// binary operators, can be used.  Binary values which are present in one matrix
/// but not the other will be preserved in the result, meaning that this operation
/// provides a *union* of the elements.
template <typename AMatrixType,
          typename BMatrixType,
          typename BinaryOp>
auto ewise(const AMatrixType& a,
	         const BMatrixType& b,
	         const BinaryOp& op) {
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

	ewise(a, b, c, op, grb::plus{});

	return c;
}

/// Apply the binary operator `op` elementwise to matrices `a` and `b`,
/// accumulating the results into the matrix `c` using binary operator `acc`.
/// Elements in only one matrix will be preserved in the output matrix `c`,
/// meaning this operation provides a *union* of the elements.
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

  // For all indices in a [i, j], perform
  // value = a[i, j]
  // if [i, j] in b:
  //   value = op(value, b[i, j])
  // c[i, j] = acc(c[i, j], value)
  // Takes care of all indices in a.
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

  // For all indices [i, j] in b but NOT in a,
  // do c[i, j] = acc(c[i, j], b[i, j])
  for (auto ref : b) {
  	auto [i, j] = ref.index();
  	b_value_type b_value = ref.value();
  	if (a.find({i, j}) == a.end()) {
  		auto c_iter = c.find({i, j});
  		if (c_iter == c.end()) {
  			c[{i, j}] = b_value;
  		} else {
  			c_value_type c_value = *c_iter;
  			*c_iter = acc(c_value, b_value);
  		}
  	}
  }
}

} // end grb
