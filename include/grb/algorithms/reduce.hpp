#pragma once

#include <grb/containers/views/views.hpp>
#include <grb/detail/concepts.hpp>
#include <grb/detail/detail.hpp>

namespace grb {

template <MatrixRange A,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::matrix_scalar_t<A>,
                         grb::matrix_scalar_t<A>>
              Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>>
auto reduce(A&& a, Reduce&& reduce = Reduce{}, M&& mask = M{}) {
  using T = grb::matrix_scalar_t<A>;
  using I = grb::matrix_index_t<A>;

  grb::vector<T, I> v(grb::shape(a)[0]);

  for (auto&& [idx, a_v] : a) {
    T value = a_v;
    auto&& [row, col] = idx;

    if (mask.find(row) != mask.end()) {
      auto iter = v.find(row);

      if (iter != v.end()) {
        auto&& [_, v_v] = *iter;
        value = reduce(value, v_v);
      }

      v.insert_or_assign(row, value);
    }
  }

  return v;
}

} // namespace grb
