#pragma once

namespace grb {

// Binary operators
struct plus {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a + b;
	}

  template <typename T>
	static constexpr T identity() {
		return T(0);
	}
};

struct minus {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a - b;
	}
};

struct multiplies {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a * b;
	}

	template <typename T>
	static constexpr T identity() {
		return T(1);
	}
};

using times = multiplies;

struct divides {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a / b;
	}
};

// omit std::functional comparisons

struct max {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		if (a < b) {
			return b;
		} else {
			return a;
		}
	}

	template <typename T>
	static constexpr T identity() {
		return std::min(std::numeric_limits<T>::lowest(), -std::numeric_limits<T>::infinity());
	}
};

struct min {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		if (b < a) {
			return b;
		} else {
			return a;
		}
	}

	template <typename T>
	static constexpr T identity() {
		return std::max(std::numeric_limits<T>::max(), std::numeric_limits<T>::infinity());
	}
};

struct modulus {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a % b;
	}
};

// Unary operators

struct negate {
	template <typename T>
	constexpr auto operator()(const T& a) const {
		return -a;
	}
};

struct logical_not {
	template <typename T>
	constexpr auto operator()(const T& a) const {
		return !a;
	}
};

// Logical operators

struct logical_and {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a && b;
	}

	template <typename T>
	static constexpr T identity() {
		return T(true);
	}
};

struct logical_or {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a || b;
	}

	template <typename T>
	static constexpr T identity() {
		return T(false);
	}
};

struct logical_xor {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return (a || b) && !(a && b);
	}

	template <typename T>
	static constexpr T identity() {
		return T(false);
	}
};

struct logical_xnor {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return !((a || b) && !(a && b));
	}
};

} // end grb
