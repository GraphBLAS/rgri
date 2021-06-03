
#pragma once

namespace grb {

template <typename T>
struct is_monoid {
  template <typename U>
	static constexpr
	decltype(U::identity(), bool())
	test_get(int) {
	  return true;
	}

	template <typename U>
	static constexpr bool test_get(...) {
	  return false;
	}

	static constexpr bool value = test_get<T>(int());
};

template <typename T>
inline constexpr bool is_monoid_v = is_monoid<T>::value;

template <typename Fn, typename Enable = void>
class monoid_traits;

template <typename T>
class monoid_traits<std::plus<T>>
{
public:
	static constexpr T identity() noexcept {
		return T(0);
	}
};

template <typename Fn>
class monoid_traits<Fn, std::enable_if_t<grb::is_monoid_v<Fn>>>
{
public:
	template <typename U>
	static constexpr U identity() noexcept {
		return Fn:: template identity<U>();
	}
};

} // end grb