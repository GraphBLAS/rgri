#pragma once

#include <grb/containers/backend/csr_matrix.hpp>
#include <grb/containers/backend/dense_matrix.hpp>

namespace grb {

struct sparse {};
struct dense {};

template <typename T>
struct pick_backend_type;

template <>
struct pick_backend_type<sparse> {
	template <typename... Args>
	using type = grb::csr_matrix_impl_<Args...>;
};

template <>
struct pick_backend_type<dense> {
	template <typename... Args>
	using type = grb::dense_matrix_impl_<Args...>;
};

template <typename T, typename U>
struct pick_ewise_hint {
	using type = grb::dense;
};

// TODO: once hints are expanded, will need
//       to check whether hint *contains* sparse,
//       dense.  Also properly handle dense.
template <>
struct pick_ewise_hint<grb::sparse, grb::sparse> {
	using type = grb::sparse;
};

} // end grb