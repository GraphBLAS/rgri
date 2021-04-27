#pragma once

// TODO: write concept defining Matrix, Vector types.

namespace grb {

template <typename MatrixType, typename VectorType1, typename VectorType2>
void mxv(const MatrixType& a, const VectorType1& b, VectorType2& c) {
	for (auto iter = a.begin(); iter != a.end(); iter++) {
		float value = *iter;
		auto [i, j] = *iter;

		c[i] += value*b[j];
	}
}

} // end grb