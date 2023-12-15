#pragma once

#include <grb/detail/detail.hpp>
#include <grb/detail/matrix_traits.hpp>
#include <grb/detail/monoid_traits.hpp>

namespace grb {

template <
    MatrixRange A, MatrixRange B,
    BinaryOperator<grb::matrix_scalar_t<A>, grb::matrix_scalar_t<B>> Combine,
    MaskMatrixRange M = grb::full_matrix_mask<>>
auto ewise_intersection(A&& a, B&& b, Combine&& combine, M&& mask = M{}) {
  if (a.shape()[0] != b.shape()[0] || a.shape()[1] != b.shape()[1]) {
    throw grb::invalid_argument(
        "ewise_intersection: Dimensions of matrices are incompatible.");
  }

  if (mask.shape()[0] < a.shape()[0] || mask.shape()[1] < a.shape()[1]) {
    throw grb::invalid_argument(
        "ewise_intersection: Mask has smaller dimensions than matrices.");
  }

  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::matrix_scalar_t<B>;
  using c_scalar_type = decltype(std::forward<Combine>(combine)(
      std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using index_type =
      grb::bigger_integral_t<grb::matrix_index_t<A>, grb::matrix_index_t<B>>;

  grb::matrix<c_scalar_type, index_type> c(a.shape());

  for (auto&& [index, a_value] : a) {
    if constexpr (!std::is_same_v<std::remove_cvref_t<M>,
                                  grb::full_matrix_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;
      c.insert({index, std::forward<Combine>(combine)(
                           static_cast<a_scalar_type>(a_value),
                           static_cast<b_scalar_type>(b_value))});
    }
  }

  return c;
}

template <
    MatrixRange A, MatrixRange B,
    BinaryOperator<grb::matrix_scalar_t<A>, grb::matrix_scalar_t<B>> Combine,
    MaskMatrixRange M = grb::full_matrix_mask<>>
auto ewise_union(A&& a, B&& b, Combine&& combine, M&& mask = M{}) {
  if (a.shape()[0] != b.shape()[0] || a.shape()[1] != b.shape()[1]) {
    throw grb::invalid_argument(
        "ewise_intersection: Dimensions of matrices are incompatible.");
  }

  if (mask.shape()[0] < a.shape()[0] || mask.shape()[1] < a.shape()[1]) {
    throw grb::invalid_argument(
        "ewise_intersection: Mask has smaller dimensions than matrices.");
  }

  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::matrix_scalar_t<B>;
  using c_scalar_type = decltype(std::forward<Combine>(combine)(
      std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using index_type =
      grb::bigger_integral_t<grb::matrix_index_t<A>, grb::matrix_index_t<B>>;

  grb::matrix<c_scalar_type, index_type> c(a.shape());

  size_t num_matched = 0;

  for (auto&& [index, a_value] : a) {
    if constexpr (!std::is_same_v<std::remove_cvref_t<M>,
                                  grb::full_matrix_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;

      c.insert({index, std::forward<Combine>(combine)(
                           static_cast<a_scalar_type>(a_value),
                           static_cast<b_scalar_type>(b_value))});
      ++num_matched;
    } else {
      c.insert({index, static_cast<a_scalar_type>(a_value)});
    }
  }

  if (num_matched < b.size()) {
    for (auto&& [index, b_value] : b) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
      c.insert({index, b_value});
    }
  }

  return c;
}

template <
    VectorRange A, VectorRange B,
    BinaryOperator<grb::vector_scalar_t<A>, grb::vector_scalar_t<B>> Combine,
    MaskVectorRange M = grb::full_vector_mask<>>
auto ewise_intersection(A&& a, B&& b, Combine&& combine, M&& mask = M{}) {
  if (a.shape() != b.shape()) {
    throw grb::invalid_argument(
        "ewise_intersection: Dimensions of vectors are incompatible.");
  }

  if (mask.shape() < a.shape()) {
    throw grb::invalid_argument(
        "ewise_intersection: Mask has smaller dimensions than vectors.");
  }

  using a_scalar_type = grb::vector_scalar_t<A>;
  using b_scalar_type = grb::vector_scalar_t<B>;
  using c_scalar_type = decltype(std::forward<Combine>(combine)(
      std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using index_type =
      grb::bigger_integral_t<grb::vector_index_t<A>, grb::vector_index_t<B>>;

  grb::vector<c_scalar_type, index_type> c(a.shape());

  for (auto&& [index, a_value] : a) {
    if constexpr (!std::is_same_v<std::remove_cvref_t<M>,
                                  grb::full_vector_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;
      c.insert({index, std::forward<Combine>(combine)(
                           static_cast<a_scalar_type>(a_value),
                           static_cast<b_scalar_type>(b_value))});
    }
  }

  return c;
}

template <
    VectorRange A, VectorRange B,
    BinaryOperator<grb::vector_scalar_t<A>, grb::vector_scalar_t<B>> Combine,
    MaskVectorRange M = grb::full_vector_mask<>>
auto ewise_union(A&& a, B&& b, Combine&& combine, M&& mask = M{}) {
  if (a.shape() != b.shape()) {
    throw grb::invalid_argument("ewise_intersection: Dimensions of vectors (" +
                                std::to_string(a.shape()) + " and " +
                                std::to_string(b.shape()) +
                                ") are incompatible.");
  }

  if (mask.shape() < a.shape()) {
    throw grb::invalid_argument(
        "ewise_intersection: Mask has smaller dimensions than vectors.");
  }

  using a_scalar_type = grb::vector_scalar_t<A>;
  using b_scalar_type = grb::vector_scalar_t<B>;
  using c_scalar_type = decltype(std::forward<Combine>(combine)(
      std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using index_type =
      grb::bigger_integral_t<grb::vector_index_t<A>, grb::vector_index_t<B>>;

  grb::vector<c_scalar_type, index_type> c(a.shape());

  size_t num_matched = 0;

  for (auto&& [index, a_value] : a) {
    if constexpr (!std::is_same_v<std::remove_cvref_t<M>,
                                  grb::full_vector_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;

      c.insert({index, std::forward<Combine>(combine)(
                           static_cast<a_scalar_type>(a_value),
                           static_cast<b_scalar_type>(b_value))});
      ++num_matched;
    } else {
      c.insert({index, static_cast<a_scalar_type>(a_value)});
    }
  }

  if (num_matched < b.size()) {
    for (auto&& [index, b_value] : b) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
      c.insert({index, b_value});
    }
  }

  return c;
}

} // namespace grb
