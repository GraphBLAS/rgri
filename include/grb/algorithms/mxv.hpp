#pragma once

namespace grb {

/// Multiply the GraphBLAS matrix `a` by the matrix `b`
/// using conventional matrix multiplication with the `*`
/// and `+` operators, returning another matrix `c`.
template <typename AMatrixType, typename BMatrixType>
auto multiply(const AMatrixType& a, const BMatrixType& b) {
  using a_value_type = typename AMatrixType::value_type;
  using b_value_type = typename BMatrixType::value_type;

  size_t M = a.shape()[0];
  size_t K = a.shape()[1];
  size_t N = b.shape()[1];

  if (a.shape()[1] != b.shape()[0]) {
    throw grb::invalid_argument(
        "Dimensions of matrices given to grb::multiply() are not compatible.");
  }

  using c_value_type = decltype(a_value_type() * b_value_type());
  using index_type = typename AMatrixType::index_type;
  // TODO: handle hint_types other than `grb::sparse`, `grb::dense`
  using backend_type =
      typename grb::pick_ewise_hint<typename AMatrixType::hint_type,
                                    typename BMatrixType::hint_type>::type;

  grb::matrix<c_value_type, index_type, backend_type> c({M, N});

  for (auto ref : a) {
    for (size_t j = 0; j < N; j++) {
      a_value_type a_value = ref.value();
      auto [i, k] = ref.index();
      auto iter = b.find({k, j});
      if (iter != b.end()) {
        b_value_type b_value = *iter;
        c[{i, j}] = c[{i, j}] + a_value * b_value;
      }
    }
  }
  return c;
}

/// Compute the sum of all elements in the matrix `matrix`,
/// using the conventional `+` operator.
template <typename MatrixType>
auto sum(const MatrixType& matrix) {
  using value_type = typename MatrixType::value_type;
  value_type x = 0;
  for (auto ref : matrix) {
    x += ref.value();
  }
  return x;
}

// C = mask(alpha*C + AB) U ~mask(C)

template <typename MatrixType, typename VectorType1, typename VectorType2,
          typename MaskType, typename SemiringType, typename AccumulatorType,
          typename AlphaType = typename MatrixType::value_type>
void mxv(const MatrixType& a, const VectorType1& b, VectorType2& c,
         const MaskType& mask, const SemiringType& semiring,
         const AccumulatorType& accumulator, AlphaType alpha = 1,
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
        output_vector[i] = accumulator(output_vector[i].value(), alpha * value);
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
      output_vector[i] = mask_value * output_vector[i];
    }
  }
}

template <typename AMatrixType, typename BMatrixType>
size_t multiply_flops(const AMatrixType& a, const BMatrixType& b) {
  size_t M = a.shape()[0];
  size_t K = a.shape()[1];
  size_t N = b.shape()[1];

  if (a.shape()[1] != b.shape()[0]) {
    throw grb::invalid_argument(
        "Dimensions of matrices given to grb::multiply() are not compatible.");
  }

  std::vector<size_t> a_nnz(K);
  std::vector<size_t> b_nnz(K);

  for (auto ref : a) {
    auto [i, k] = ref.index();
    a_nnz[k]++;
  }

  for (auto ref : b) {
    auto [k, j] = ref.index();
    b_nnz[k]++;
  }

  size_t n_flops = 0;
  for (size_t i = 0; i < K; i++) {
    n_flops += a_nnz[i] * b_nnz[i];
  }

  return n_flops;
}

} // namespace grb
