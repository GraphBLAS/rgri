#pragma once

#include <algorithm>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <grb/grb.hpp>
#include <numeric>
#include <random>
#include <vector>

template <typename T>
auto get_distribution(T min, T max) {
  if constexpr (std::is_same_v<bool, T>) {
    return std::bernoulli_distribution();
  } else if constexpr (std::is_integral_v<T>) {
    return std::uniform_int_distribution<T>(min, max);
  } else if constexpr (std::is_floating_point_v<T>) {
    return std::uniform_real_distribution<T>(min, max);
  }
}

template <bool is_monoid, typename T, typename Fn, typename ReferenceFn>
void test_op(std::size_t num_values, T init, Fn&& op, ReferenceFn&& std_op,
             T min = std::numeric_limits<T>::min(),
             T max = std::numeric_limits<T>::max()) {
  std::vector<T> vec(num_values);
  std::random_device rd;
  std::mt19937 gen(rd());

  auto d = get_distribution(min, max);

  std::for_each(vec.begin(), vec.end(), [&](auto&& v) { v = d(gen); });

  if constexpr (is_monoid) {
    static_assert(grb::is_monoid_v<Fn, T>);
    constexpr T identity = grb::monoid_traits<Fn, T>::identity();
    init = identity;
  }

  std::vector<T> vec2 = vec;

  std::accumulate(vec.begin(), vec.end(), init, op);
  std::accumulate(vec2.begin(), vec2.end(), init, std_op);

  for (size_t i = 0; i < vec.size(); i++) {
    REQUIRE(vec[i] == vec2[i]);
  }
}

TEMPLATE_TEST_CASE("basic operator test plus 1", "[template]", int, size_t,
                   float, double, bool) {
  using T = TestType;
  std::size_t num_values = 1000;

  test_op<true>(num_values, T(0), grb::plus(), std::plus());
  test_op<true>(num_values, T(0), grb::plus<T>(), std::plus<T>());
  test_op<true>(num_values, T(0), grb::plus<T, T>(), std::plus<T>());
  test_op<true>(num_values, T(0), grb::plus<T, T, T>(), std::plus<T>());
}

TEMPLATE_TEST_CASE("basic operator test minus 1", "[template]", int, size_t,
                   float, double, bool) {
  using T = TestType;
  std::size_t num_values = 1000;

  test_op<false>(num_values, T(0), grb::minus(), std::minus());
  test_op<false>(num_values, T(0), grb::minus<T>(), std::minus<T>());
  test_op<false>(num_values, T(0), grb::minus<T, T>(), std::minus<T>());
  test_op<false>(num_values, T(0), grb::minus<T, T, T>(), std::minus<T>());
}

TEMPLATE_TEST_CASE("basic operator test multiplies 1", "[template]", int,
                   size_t, float, double, bool) {
  using T = TestType;
  std::size_t num_values = 1000;

  test_op<true>(num_values, T(0), grb::multiplies(), std::multiplies());
  test_op<true>(num_values, T(0), grb::multiplies<T>(), std::multiplies<T>());
  test_op<true>(num_values, T(0), grb::multiplies<T, T>(),
                std::multiplies<T>());
  test_op<true>(num_values, T(0), grb::multiplies<T, T, T>(),
                std::multiplies<T>());
}

TEMPLATE_TEST_CASE("basic operator test divides 1", "[template]", int, size_t,
                   float, double) {
  using T = TestType;
  std::size_t num_values = 1000;

  test_op<false>(num_values, T(1), grb::divides(), std::divides());
  test_op<false>(num_values, T(1), grb::divides<T>(), std::divides<T>());
  test_op<false>(num_values, T(1), grb::divides<T, T>(), std::divides<T>());
  test_op<false>(num_values, T(1), grb::divides<T, T, T>(), std::divides<T>());
}

TEMPLATE_TEST_CASE("basic operator test modulus 1", "[template]", int, size_t) {
  using T = TestType;
  std::size_t num_values = 1000;

  test_op<false>(num_values, T(1), grb::modulus(), std::modulus());
  test_op<false>(num_values, T(1), grb::modulus<T>(), std::modulus<T>());
  test_op<false>(num_values, T(1), grb::modulus<T, T>(), std::modulus<T>());
  test_op<false>(num_values, T(1), grb::modulus<T, T, T>(), std::modulus<T>());
}
