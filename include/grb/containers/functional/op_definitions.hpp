#pragma once

#include <limits>
#include <type_traits>

namespace grb {

template <typename Fn, typename T = void>
class unary_op_impl_ {
public:
  constexpr T operator()(const T& value) const {
    return Fn{}(value);
  }
};

template <typename Fn>
class unary_op_impl_<Fn, void> {
public:
  template <typename T>
  constexpr auto operator()(T&& value) const {
    return Fn{}(std::forward<T>(value));
  }
};

template <typename Fn, typename T = void, typename U = T, typename V = void>
class binary_op_impl_ {
public:
  constexpr V operator()(const T& lhs, const U& rhs) const {
    return Fn{}(lhs, rhs);
  }

  template <typename X>
  static constexpr X identity()
  requires(has_identity_template_v<Fn, X>)
  {
    return Fn:: template identity<X>();
  }
};

template <typename Fn, typename T, typename U>
class binary_op_impl_<Fn, T, U, void> {
public:
  constexpr auto operator()(const T& lhs, const U& rhs) const {
    return Fn{}(lhs, rhs);
  }

  static constexpr T identity()
  requires(std::is_same_v<T, U> &&
           has_identity_template_v<Fn, T>)
  {
    return Fn:: template identity<T>();
  }
};

template <typename Fn>
class binary_op_impl_<Fn, void, void, void>
{
public:
  template <typename T, typename U>
  constexpr auto operator()(T&& lhs, U&& rhs) const
  {
    return Fn{}(lhs, rhs);
  }

  template <typename T>
  static constexpr T identity()
  requires(has_identity_template_v<Fn, T>)
  {
    return Fn:: template identity<T>();
  }
};

class plus_impl_ {
public:
  template <typename T, typename U>
  constexpr auto operator()(T&& lhs, U&& rhs) const {
    return std::forward<T>(lhs) + std::forward<U>(rhs);
  }

  template <typename T>
  static constexpr T identity() {
    return T(0);
  }
};

/// Binary Operator to perform subtraction. Uses the `-` operator.
struct minus_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return a - b;
  }
};

/// Binary Operator to perform multiplication. Uses the `*` operator.
struct multiplies_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return a * b;
  }

  template <typename T>
  static constexpr T identity() {
    return T(1);
  }
};

/// Binary Operator to perform division. Uses the `/` operator.
struct divides_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return a / b;
  }
};

// omit std::functional comparisons

/// Binary Operator to perform max, returning the greater of the two values,
/// or the first element if they are equal.  Uses the `<` operator.
struct max_impl_ {
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
struct min_impl_ {
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
struct modulus_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return a % b;
  }
};

/// The binary operator `grb::plus`, which forms a monoid
/// on integral types.
template <typename T = void, typename U = T, typename V = void>
using plus = binary_op_impl_<plus_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using minus = binary_op_impl_<minus_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using multiplies = binary_op_impl_<multiplies_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using times = multiplies<T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using divides = binary_op_impl_<divides_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using max = binary_op_impl_<max_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using min = binary_op_impl_<min_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using modulus = binary_op_impl_<modulus_impl_, T, U, V>;

// Unary operators

struct negate_impl_ {
  template <typename T>
  constexpr auto operator()(const T& a) const {
    return -a;
  }
};

struct logical_not_impl_ {
  template <typename T>
  constexpr auto operator()(const T& a) const {
    return !a;
  }
};

template <typename T = void>
using negate = unary_op_impl_<negate_impl_, T>;

template <typename T = void>
using logical_not = unary_op_impl_<logical_not_impl_, T>;

// Logical operators

struct logical_and_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return a && b;
  }

  template <typename T>
  static constexpr T identity() {
    return T(true);
  }
};

struct logical_or_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return a || b;
  }

  template <typename T>
  static constexpr T identity() {
    return T(false);
  }
};

struct logical_xor_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return (a || b) && !(a && b);
  }

  template <typename T>
  static constexpr T identity() {
    return T(false);
  }
};

struct logical_xnor_impl_ {
  template <typename T, typename U>
  constexpr auto operator()(const T& a, const U& b) const {
    return !((a || b) && !(a && b));
  }
};

template <typename T = void, typename U = T, typename V = void>
using logical_and = binary_op_impl_<logical_and_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using logical_or = binary_op_impl_<logical_or_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using logical_xor = binary_op_impl_<logical_xor_impl_, T, U, V>;

template <typename T = void, typename U = T, typename V = void>
using logical_xnor = binary_op_impl_<logical_xnor_impl_, T, U, V>;

} // end grb
