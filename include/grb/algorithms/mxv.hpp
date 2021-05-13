#pragma once

namespace grb {
	
// C = mask(alpha*C + AB) U ~mask(C)

template <typename MatrixType,
          typename VectorType1,
          typename VectorType2,
          typename MaskType,
          typename SemiringType,
          typename AccumulatorType,
          typename AlphaType = typename MatrixType::value_type>
void mxv(const MatrixType& a, const VectorType1& b, VectorType2& c,
         const MaskType& mask,
         const SemiringType& semiring,
         const AccumulatorType& accumulator,
         AlphaType alpha = 1,
         bool merge = true) {
  using matrix_value_type = typename MatrixType::value_type;
  using b_vector_value_type = typename VectorType1::value_type;
  using c_vector_value_type = typename VectorType2::value_type;

  VectorType2 output_vector(c.shape());

  for (auto ref : a) {
    matrix_value_type matrix_value = ref.value();
    grb::index_t index = ref.index();
    auto [i, k] = index;

    auto b_iter = b.find(k);
    if (mask.find(i) != mask.end() && b_iter != b.end()) {
      b_vector_value_type vector_value = (*b_iter).value();

      auto v = semiring.combine(matrix_value, vector_value);

      if (output_vector.find(i) == output_vector.end()) {
        output_vector[i] = v;
      } else {
        output_vector[i] = semiring.reduce(output_vector[i].value(), v);
      }
    }
  }

  for (auto ref : c) {
    c_vector_value_type value = ref.value();
    size_t i = ref.index()[0];
    // If variable is in mask, add alpha*value
	  if (mask.find(i) != mask.end()) {
      if (alpha != 0) {
	      output_vector[i] = accumulator(output_vector[i].value(), alpha*value);
	    }
    } else {
    	if (merge) {
    		output_vector[i] = value;
    	}
    }
  }

  for (auto ref : mask) {
  	auto mask_value = ref.value();
  	size_t i = ref.index()[0];
  	if (output_vector.find(i) != output_vector.end()) {
  		output_vector[i] = mask_value*output_vector[i];
  	}
  }

}

} // end grb