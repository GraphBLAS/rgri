#pragma once

#include <type_traits>
#include <functional>
#include <utility>
#include <grb/detail/detail.hpp>
#include <grb/containers/views/views.hpp>
#include <grb/detail/concepts.hpp>

namespace grb {

// TODO (new): bring in concepts to allow overloading.


template <MatrixRange A,
          VectorRange B,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::vector_scalar_t<B>> Combine = grb::multiplies<>,
          BinaryOperator<grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>> Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>>
auto multiply(A&& a,
                 B&& b,
                 Reduce&& reduce = Reduce{},
                 Combine&& combine = Combine(),
                 M&& mask = M{}) {
  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::vector_scalar_t<B>;

  using a_index_type = grb::matrix_index_t<A>;
  using b_index_type = grb::vector_index_t<B>;

  using c_scalar_type = decltype(combine(std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using c_index_type = grb::bigger_integral_t<a_index_type, b_index_type>;

  grb::vector<c_scalar_type, c_index_type> c(a.shape()[0]);

  for (auto&& [a_index, a_v] : a) {
    auto&& [i, k] = a_index;

    auto iter = b.find(k);

    if (iter != b.end()) {
      auto&& [b_index, b_v] = *iter;

      if constexpr(std::is_same_v<std::decay_t<M>, grb::full_vector_mask<>>) {
        c[i] = reduce(c[i], combine(a_v, b_v));
      } else {
        auto iter = mask.find(i);

        if (iter != mask.end()) {
          auto&& [_, value] = *iter;
          if (bool(value)) {
            c[i] = reduce(c[i], combine(a_v, b_v));
          }
        }
      }
    }
  }

  return c;
}


/// Multiply two matrices
template <MatrixRange A,
          MatrixRange B,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::matrix_scalar_t<B>> Combine = grb::multiplies<>,
          BinaryOperator<grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>> Reduce = grb::plus<>,
          MaskMatrixRange M = grb::full_matrix_mask<>>
auto multiply(A&& a,
              B&& b,
              Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{},
              M&& mask = grb::full_matrix_mask()) {
  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::matrix_scalar_t<B>;

  using a_index_type = grb::matrix_index_t<A>;
  using b_index_type = grb::matrix_index_t<B>;

  using c_scalar_type = decltype(combine(std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using c_index_type = grb::bigger_integral_t<a_index_type, b_index_type>;

  grb::matrix<c_scalar_type, c_index_type> c({a.shape()[0], b.shape()[1]});

  for (auto&& [a_index, a_v] : a) {
    auto&& [i, k] = a_index;

    for (c_index_type j = 0; j < b.shape()[1]; j++) {
      auto iter = b.find({k, j});

      if (iter != b.end()) {
        auto&& [b_index, b_v] = *iter;

        if constexpr(std::is_same_v<std::decay_t<M>, grb::full_matrix_mask<>>) {
          c[{i, j}] = reduce(c[{i, j}], combine(a_v, b_v));
        } else {
          auto iter = mask.find({i, j});
          if (iter != mask.end()) {
            auto&& [_, value] = *iter;
            if (bool(value)) {
              c[{i, j}] = reduce(c[{i, j}], combine(a_v, b_v));
            }
          }
        }
      }
    }
  }

  return c;
}

} // end grb