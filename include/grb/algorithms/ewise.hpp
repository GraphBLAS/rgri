#pragma once

#include <grb/detail/matrix_traits.hpp>
#include <grb/detail/monoid_traits.hpp>

namespace grb {

template <MatrixRange A,
          MatrixRange B,
          BinaryOperator<grb::matrix_scalar_type_t<A>,
                         grb::matrix_scalar_type_t<B>> Combine,
          MaskMatrixRange M = grb::full_mask<>
          >
auto ewise_intersection(A&& a, B&& b, Combine&& combine, M&& mask = M{})
{
  if (a.shape()[0] != b.shape()[0] || a.shape()[1] != b.shape()[1]) {
    throw grb::invalid_argument("ewise_intersection: Dimensions of matrices are incompatible.");
  }

  if (mask.shape()[0] < a.shape()[0] || mask.shape()[1] < a.shape()[1]) {
    throw grb::invalid_argument("ewise_intersection: Mask has smaller dimensions than matrices.");
  }

  using a_scalar_type = grb::matrix_scalar_t<A>;
  using b_scalar_type = grb::matrix_scalar_t<B>;
  using c_scalar_type = decltype(std::forward<Combine>(combine)(std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  using index_type = grb::bigger_integral_t<grb::matrix_index_t<A>, grb::matrix_index_t<B>>;

  grb::matrix<c_scalar_type, index_type> c(a.shape());

  for (auto&& [index, a_value] : a) {
    if constexpr(!std::is_same_v<std::remove_cvref_t<M>, grb::full_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(grb::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;
      c.insert({index, std::forward<Combine>(combine)(static_cast<a_scalar_type>(a_value), static_cast<b_scalar_type>(b_value))});
    }
  }

  return c;
}

template <typename AMatrixType,
          typename BMatrixType,
          typename Fn,
          typename MaskType = grb::full_mask<>>
auto ewise_mult(AMatrixType&& a, BMatrixType&& b, Fn&& fn,
                MaskType&& mask = grb::full_mask()) {
  assert(a.shape() == b.shape());

  using a_value_type = matrix_scalar_type_t<AMatrixType>;
  using b_value_type = matrix_scalar_type_t<BMatrixType>;

  using index_type = grb::bigger_integral_t<matrix_index_type_t<AMatrixType>,
                                            matrix_index_type_t<BMatrixType>>;

  using c_value_type = decltype(std::forward<Fn>(fn)(std::declval<a_value_type>(), std::declval<b_value_type>()));

  grb::matrix<c_value_type, index_type> c(a.shape());

  for (auto&& [index, a_value] : a) {

    if constexpr(!std::is_same_v<std::decay_t<MaskType>, grb::full_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(std::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;

      c.insert({index, std::forward<Fn>(fn)(a_value, b_value)});
    }
  }

  return c;
}

template <typename AMatrixType,
          typename BMatrixType,
          typename Fn,
          typename T,
          typename MaskType = grb::full_mask<>>
auto ewise_add(AMatrixType&& a, BMatrixType&& b, Fn&& fn, T&& default_value,
               MaskType&& mask = grb::full_mask<>()) {
  assert(a.shape() == b.shape());

  using a_value_type = matrix_scalar_type_t<AMatrixType>;
  using b_value_type = matrix_scalar_type_t<BMatrixType>;

  using index_type = grb::bigger_integral_t<matrix_index_type_t<AMatrixType>,
                                            matrix_index_type_t<BMatrixType>>;

  using c_value_type = decltype(std::forward<Fn>(fn)(std::declval<a_value_type>(), std::declval<b_value_type>()));

  std::vector<bool> index_map(a.shape()[0]*a.shape()[1], false);

  grb::matrix<c_value_type, index_type> c(a.shape());

  size_t num_matched = 0;

  for (auto&& [index, a_value] : a) {

    if constexpr(!std::is_same_v<std::decay_t<MaskType>, grb::full_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(std::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;

      c.insert({index, std::forward<Fn>(fn)(a_value, b_value)});
      index_map[index[0]*a.shape()[1] + index[1]] = true;
      num_matched++;
    } else {
      c.insert({index, std::forward<Fn>(fn)(a_value, default_value)});
    }
  }

  if (num_matched < b.size()) {
    for (auto&& [index, b_value] : b) {
      c.insert({index, std::forward<Fn>(fn)(default_value, b_value)});
    }
  }

  return c;
}


template <typename AMatrixType,
          typename BMatrixType,
          typename Fn>
auto ewise_add(AMatrixType&& a, BMatrixType&& b, Fn&& fn)
requires(std::is_same_v<matrix_scalar_type_t<AMatrixType>,
                        matrix_scalar_type_t<BMatrixType>> &&
         grb::is_monoid_v<Fn, matrix_scalar_type_t<AMatrixType>>)
{
  return ewise_add(std::forward<AMatrixType>(a),
                   std::forward<BMatrixType>(b),
                   std::forward<Fn>(fn),
                   grb::monoid_traits<Fn, matrix_scalar_type_t<AMatrixType>>::identity());
}

template <typename AVectorType,
          typename BVectorType,
          typename Fn,
          typename T,
          typename MaskType = grb::full_mask<>>
auto ewise_add_v(AVectorType&& a, BVectorType&& b, Fn&& fn, T&& default_value,
               MaskType&& mask = grb::full_mask<>()) {
  assert(a.shape() == b.shape());

  using a_value_type = matrix_scalar_type_t<AVectorType>;
  using b_value_type = matrix_scalar_type_t<BVectorType>;

  using index_type = grb::bigger_integral_t<matrix_index_type_t<AVectorType>,
                                            matrix_index_type_t<BVectorType>>;

  using c_value_type = decltype(std::forward<Fn>(fn)(std::declval<a_value_type>(), std::declval<b_value_type>()));

  std::vector<bool> index_map(a.shape(), false);

  grb::vector<c_value_type, index_type> c(a.shape());

  size_t num_matched = 0;

  for (auto&& [index, a_value] : a) {

    if constexpr(!std::is_same_v<std::decay_t<MaskType>, grb::full_mask<>>) {
      auto mask_iter = mask.find(index);
      if (mask_iter == mask.end() || !bool(std::get<1>(*mask_iter))) {
        continue;
      }
    }

    auto iter = b.find(index);

    if (iter != b.end()) {
      auto&& [_, b_value] = *iter;

      c.insert({index, std::forward<Fn>(fn)(a_value, b_value)});
      index_map[index] = true;
      num_matched++;
    } else {
      c.insert({index, std::forward<Fn>(fn)(a_value, default_value)});
    }
  }

  if (num_matched < b.size()) {
    for (auto&& [index, b_value] : b) {
      c.insert({index, std::forward<Fn>(fn)(default_value, b_value)});
    }
  }

  return c;
}

} // end grb