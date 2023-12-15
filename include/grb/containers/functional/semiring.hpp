#pragma once

namespace grb {

template <typename Reduce, typename Combine>
class semiring {
public:
  using reduce_type = Reduce;
  using combine_type = Combine;

  semiring(Reduce reduce, Combine combine)
      : combine_(combine), reduce_(reduce) {}

  template <typename T, typename U>
  auto combine(const T& a, const U& b) const {
    return combine_(a, b);
  }

  template <typename T, typename U>
  auto reduce(const T& a, const U& b) const {
    return reduce_(a, b);
  }

  Combine combine_;
  Reduce reduce_;
};

template <typename ReduceOp, typename CombineOp>
auto make_semiring(ReduceOp reduce, CombineOp combine) {
  return semiring<ReduceOp, CombineOp>(reduce, combine);
}

} // namespace grb
