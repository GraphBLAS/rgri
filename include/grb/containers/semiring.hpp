#pragma once

namespace grb {

template <typename Combine, typename Reduce>
class semiring {
public:

  semiring(Combine combine, Reduce reduce)
    : combine_(combine), reduce_(reduce) {}

  template <typename T, typename U>
  auto combine(T a, U b) const {
    return combine_(a, b);
  }

  template <typename T, typename U>
  auto reduce(T a, U b) const {
    return reduce_(a, b);
  }

  Combine combine_;
  Reduce reduce_;
};

template <typename BinaryFunction1, typename BinaryFunction2>
auto make_semiring(BinaryFunction1 combine, BinaryFunction2 reduce) {
  return semiring<BinaryFunction1, BinaryFunction2>(combine, reduce);
}

} // end grb