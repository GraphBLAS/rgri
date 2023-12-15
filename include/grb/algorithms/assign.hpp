#pragma once

#include <type_traits>

namespace grb {

template <grb::MatrixRange B,
          grb::MutableMatrixRange<grb::matrix_scalar_t<B>> A>
void assign(A&& a, B&& b) {
  if (grb::shape(a) != grb::shape(b)) {
    throw grb::invalid_argument("assign: dimensions of a and b do not match.");
  }

  a.clear();
  a.insert(std::ranges::begin(b), std::ranges::end(b));
}

template <typename T, grb::MutableMatrixRange<T> A>
void assign(A&& a, const T& value) {

  grb::full_matrix<T, grb::matrix_index_t<A>> other(grb::shape(a), value);

  assign(a, other);
}

template <grb::VectorRange B,
          grb::MutableVectorRange<grb::vector_scalar_t<B>> A>
void assign(A&& a, B&& b) {
  if (grb::shape(a) != grb::shape(b)) {
    throw grb::invalid_argument("assign: dimensions of a and b do not match.");
  }

  a.clear();
  a.insert(std::ranges::begin(b), std::ranges::end(b));
}

template <typename T, grb::MutableVectorRange<T> A>
void assign(A&& a, const T& value) {
  for (std::size_t i = 0; i < a.shape(); i++) {
    a[i] = value;
  }
}

} // namespace grb
