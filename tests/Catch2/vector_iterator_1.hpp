#pragma once

#include <ranges>
#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <grb/grb.hpp>

TEMPLATE_PRODUCT_TEST_CASE(
    "basic vector iterator tests 1", "[matrix][template]", (grb::vector),
    ((float, int, grb::sparse), (float, size_t, grb::sparse),
     (float, int, grb::dense), (float, size_t, grb::dense))) {

  using value_type = typename TestType::value_type;
  using iterator = typename TestType::iterator;
  using const_iterator = typename TestType::const_iterator;

  static_assert(std::forward_iterator<iterator>);
  static_assert(std::forward_iterator<const_iterator>);

  std::vector<std::string> fnames = {"chesapeake/chesapeake.mtx"};
  for (size_t i = 0; i < fnames.size(); i++) {
  }
}
