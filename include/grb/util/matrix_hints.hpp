#pragma once

#include <grb/containers/backend/csr_matrix.hpp>
#include <grb/containers/backend/dense_matrix.hpp>
#include <grb/detail/pack_includes.hpp>

namespace grb {

struct sparse {};
struct dense {};
struct row {};
struct column {};

template <typename... Hints>
struct compose {
	template <typename T>
	struct includes {
	  static constexpr bool value = pack_includes<T, Hints...>::value;
	};
};

template <typename T, typename Enabler = void>
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

template <typename... Hints>
struct pick_backend_type<compose<Hints...>,
                         std::enable_if_t<compose<Hints...>:: template includes<grb::sparse>::value>
                         >
{
  template <typename... Args>
  using type = grb::csr_matrix_impl_<Args...>;
};

template <typename... Hints>
struct pick_backend_type<compose<Hints...>,
                         std::enable_if_t<
                           compose<Hints...>:: template includes<grb::dense>::value &&
                           !compose<Hints...>:: template includes<grb::sparse>::value
                                         >
                         >
{
  template <typename... Args>
  using type = grb::csr_matrix_impl_<Args...>;
};

// Current logic: if one of the matrices is dense,
// pick dense. Otherwise, sparse.
template <typename T, typename U, typename Enabler = void>
struct pick_ewise_hint {
	using type = grb::sparse;
};

template <typename T, typename U>
struct pick_ewise_hint<T, U,
                       std::enable_if_t<compose<T, U>:: template includes<grb::dense>::value>
                       >
{
	using type = grb::dense;
};

} // end grb