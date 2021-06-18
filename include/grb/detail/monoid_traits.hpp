
#pragma once

namespace grb {

// Check whether binary operator `Fn` has
// an identity for the type `T`.
template <typename Fn, typename T>
struct has_identity {
  template <typename U>
	static constexpr
	decltype(U:: template identity<T>(), bool())
	test_identity(int) {
	  return true;
	}

	template <typename U>
	static constexpr bool test_identity(...) {
	  return false;
	}

	static constexpr bool value = test_identity<Fn>(int());
};

// Check whether binary operator `Fn` has
// an identity for the type `T`.
template <typename Fn, typename T>
inline constexpr bool has_identity_v = has_identity<Fn, T>::value;

template <typename Fn, typename T, typename Enable = void>
class monoid_traits;

template <typename T>
class monoid_traits<std::plus<T>, T>
{
public:

	static constexpr T identity() noexcept {
		return T(0);
	}
};

template <typename Fn, typename T>
class monoid_traits<Fn, T, std::enable_if_t<grb::has_identity_v<Fn, T>>>
{
public:
	static T identity() {
		return Fn:: template identity<T>();
	}
};

} // end grb