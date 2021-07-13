#pragma once

namespace grb {

// Binary operators

/// Binary Operator to perform addition. Uses the `+` operator.
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

/// Binary Operator to perform subtraction. Uses the `-` operator.
struct minus {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a - b;
	}
};

/// Binary Operator to perform multiplication. Uses the `*` operator.
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

/// Binary Operator to perform multiplication.  Uses the `*` operator.
using times = multiplies;

/// Binary Operator to perform division. Uses the `/` operator.
struct divides {
	template <typename T, typename U>
	constexpr auto operator()(const T& a, const U& b) const {
		return a / b;
	}
};

// omit std::functional comparisons

/// Binary Operator to perform max, returning the greater of the two values,
/// or the first element if they are equal.  Uses the `<` operator.
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

/// Binary Operator to perform min, returning the lesser of the two values,
/// or the first element if they are equal.  Uses the `<` operator.
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

/// Binary Operator to perform modulus, uses the `%` operator.
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
