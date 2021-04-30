#pragma once

#include <grb/experimental/transform_matrix_view.hpp>

namespace grb {

namespace experimental {

template <typename T>
T identity(T value) {
	return value;
}

template <typename T>
T value_one(T value) {
	return 1;
}

} // end experimental

// TODO: type of ValueTransform should be ValueTransform&&

template <typename MatrixType, typename ValueTransform>
auto transform(MatrixType& matrix, const ValueTransform& value_transform) {
	return grb::experimental::transform_matrix_view(
		       matrix,
		       value_transform,
		       grb::experimental::identity<grb::index_t>,
		       grb::experimental::identity<grb::index_t>);
}

template <typename MatrixType>
auto structure(MatrixType& matrix) {
	using value_type = typename MatrixType::value_type;
	return grb::experimental::transform_matrix_view(
		       matrix,
		       grb::experimental::value_one<value_type>,
		       grb::experimental::identity<grb::index_t>,
		       grb::experimental::identity<grb::index_t>);
}

} // end grb