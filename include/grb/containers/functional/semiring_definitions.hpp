#pragma once

#include "op_definitions.hpp"
#include "semiring.hpp"

namespace grb {

template <typename Reduce, typename Combine>
struct standard_semiring {
  using reduce_type = Reduce;
  using combine_type = Combine;

  template <typename T, typename U>
  auto combine(const T& a, const U& b) const {
    return combine_(a, b);
  }

  template <typename T, typename U>
  auto reduce(const T& a, const U& b) const {
    return reduce_(a, b);
  }

  Reduce reduce_;
  Combine combine_;
};

using plus_multiplies = standard_semiring<grb::plus<>, grb::multiplies<>>;
using plus_times = plus_multiplies;

using min_plus = standard_semiring<grb::min<>, grb::plus<>>;

using max_plus = standard_semiring<grb::max<>, grb::plus<>>;

using min_multiplies = standard_semiring<grb::min<>, grb::multiplies<>>;
using min_times = min_multiplies;

using min_max = standard_semiring<grb::min<>, grb::max<>>;

using max_min = standard_semiring<grb::max<>, grb::min<>>;

using max_multiplies = standard_semiring<grb::max<>, grb::multiplies<>>;
using max_times = max_multiplies;

using plus_min = standard_semiring<grb::plus<>, grb::min<>>;

// Logical semirings

using lor_land = standard_semiring<grb::logical_or<>, grb::logical_and<>>;
using land_lor = standard_semiring<grb::logical_and<>, grb::logical_or<>>;
using lxor_land = standard_semiring<grb::logical_xor<>, grb::logical_and<>>;
using lxnor_lor = standard_semiring<grb::logical_xnor<>, grb::logical_or<>>;

} // namespace grb
