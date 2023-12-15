#pragma once

#include <functional>
#include <grb/algorithms/assign.hpp>
#include <grb/containers/views/views.hpp>
#include <grb/detail/concepts.hpp>
#include <grb/detail/detail.hpp>
#include <type_traits>
#include <utility>

namespace grb {

/// Multiply a matrix times a vector
template <MatrixRange A, VectorRange B,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::vector_scalar_t<B>>
              Combine = grb::multiplies<>,
          BinaryOperator<grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>>
              Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>>
auto multiply(A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine(), M&& mask = M{}) {
  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::vector_scalar_t<B>;

  using a_index_type = grb::matrix_index_t<A>;
  using b_index_type = grb::vector_index_t<B>;

  using c_scalar_type = decltype(combine(std::declval<a_scalar_type>(),
                                         std::declval<b_scalar_type>()));

  using c_index_type = grb::bigger_integral_t<a_index_type, b_index_type>;

  grb::vector<c_scalar_type, c_index_type> c(a.shape()[0]);

  for (auto&& [a_index, a_v] : a) {
    auto&& [i, k] = a_index;

    auto iter = b.find(k);

    if (iter != b.end()) {
      auto&& [_, b_v] = *iter;

      auto iter = mask.find(i);

      if (iter != mask.end()) {
        auto&& [_, value] = *iter;
        if (bool(value)) {
          auto combined_v = combine(a_v, b_v);
          auto&& [insert_iter, success] = c.insert({i, combined_v});
          if (!success) {
            auto&& [_, c_ref] = *insert_iter;
            c_scalar_type c_v = c_ref;
            c_ref = reduce(c_v, combined_v);
          }
        }
      }
    }
  }

  return c;
}

/// Multiply two matrices
template <MatrixRange A, MatrixRange B,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::matrix_scalar_t<B>>
              Combine = grb::multiplies<>,
          BinaryOperator<grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>>
              Reduce = grb::plus<>,
          MaskMatrixRange M = grb::full_matrix_mask<>>
auto multiply(A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{},
              M&& mask = grb::full_matrix_mask()) {
  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::matrix_scalar_t<B>;

  using a_index_type = grb::matrix_index_t<A>;
  using b_index_type = grb::matrix_index_t<B>;

  using c_scalar_type = decltype(combine(std::declval<a_scalar_type>(),
                                         std::declval<b_scalar_type>()));

  using c_index_type = grb::bigger_integral_t<a_index_type, b_index_type>;

  grb::matrix<c_scalar_type, c_index_type> c(
      grb::index<c_index_type>(a.shape()[0], b.shape()[1]));

  for (auto&& [a_index, a_v] : a) {
    auto&& [i, k] = a_index;

    for (c_index_type j = 0; j < b.shape()[1]; j++) {
      auto iter = b.find(grb::index<b_index_type>(k, j));

      if (iter != b.end()) {
        auto&& [b_index, b_v] = *iter;

        if constexpr (std::is_same_v<std::decay_t<M>,
                                     grb::full_matrix_mask<>>) {
          c[grb::index<c_index_type>(i, j)] =
              reduce(c[grb::index<c_index_type>(i, j)], combine(a_v, b_v));
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

template <VectorRange A, VectorRange B,
          BinaryOperator<grb::vector_scalar_t<A>, grb::vector_scalar_t<B>>
              Combine = grb::multiplies<>,
          Monoid<grb::combine_result_t<A, B, Combine>> Reduce = grb::plus<>>
auto multiply(A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{}) {
  using a_value_type = grb::container_scalar_t<A>;
  using b_value_type = grb::container_scalar_t<B>;

  using combine_type = grb::combine_result_t<A, B, Combine>;

  combine_type rv = grb::monoid_traits<Reduce, combine_type>::identity();

  for (auto&& [index, value] : a) {
    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;
      auto result = combine(static_cast<a_value_type>(value),
                            static_cast<b_value_type>(b_value));

      rv = reduce(rv, result);
    }
  }
  return rv;
}

template <VectorRange A, MatrixRange B,
          BinaryOperator<grb::vector_scalar_t<A>, grb::matrix_scalar_t<B>>
              Combine = grb::multiplies<>,
          BinaryOperator<grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>>
              Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>>
auto multiply(A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine(), M&& mask = M{}) {
  return multiply(grb::transpose(std::forward<B>(b)), std::forward<A>(a),
                  std::forward<Reduce>(reduce), std::forward<Combine>(combine),
                  std::forward<M>(mask));
}

namespace {

template <typename C, typename A, typename B, typename Combine, typename Reduce,
          typename M, typename Accumulate>
void multiply_impl_(C&& c, A&& a, B&& b, Reduce&& reduce, Combine&& combine,
                    M&& mask, Accumulate&& acc, bool merge) {
  auto z = multiply(a, b, reduce, combine, mask);

  if (merge) {
    grb::assign(c, ewise_union(c, z, acc));
  } else {
    grb::assign(c, ewise_union(c, z, acc, grb::views::structure(z)));
  }
}

} // namespace

// Matrix matrix

template <MatrixRange A, MatrixRange B,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::matrix_scalar_t<B>>
              Combine = grb::multiplies<>,
          BinaryOperator<grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>>
              Reduce = grb::plus<>,
          MaskMatrixRange M = grb::full_matrix_mask<>,
          MutableMatrixRange<grb::combine_result_t<A, B, Combine>> C,
          BinaryOperator<grb::matrix_scalar_t<C>,
                         grb::combine_result_t<A, B, Combine>>
              Accumulate = grb::take_right<>>
void multiply(C&& c, A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{}, M&& mask = M{},
              Accumulate&& acc = Accumulate{}, bool merge = false) {
  multiply_impl_(c, a, b, reduce, combine, mask, acc, merge);
}

// Matrix vector

template <MatrixRange A, VectorRange B,
          BinaryOperator<grb::matrix_scalar_t<A>, grb::vector_scalar_t<B>>
              Combine = grb::multiplies<>,
          BinaryOperator<grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>>
              Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>,
          MutableVectorRange<grb::combine_result_t<A, B, Combine>> C,
          BinaryOperator<grb::vector_scalar_t<C>,
                         grb::combine_result_t<A, B, Combine>>
              Accumulate = grb::take_right<>>
void multiply(C&& c, A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{}, M&& mask = M{},
              Accumulate&& acc = Accumulate{}, bool merge = false) {
  multiply_impl_(c, a, b, reduce, combine, mask, acc, merge);
}

// Vector matrix

template <VectorRange A, MatrixRange B,
          BinaryOperator<grb::vector_scalar_t<A>, grb::matrix_scalar_t<B>>
              Combine = grb::multiplies<>,
          BinaryOperator<grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>,
                         grb::combine_result_t<A, B, Combine>>
              Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>,
          MutableVectorRange<grb::combine_result_t<A, B, Combine>> C,
          BinaryOperator<grb::vector_scalar_t<C>,
                         grb::combine_result_t<A, B, Combine>>
              Accumulate = grb::take_right<>>
void multiply(C&& c, A&& a, B&& b, Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{}, M&& mask = M{},
              Accumulate&& acc = Accumulate{}, bool merge = false) {
  multiply_impl_(c, a, b, reduce, combine, mask, acc, merge);
}

} // namespace grb
