#include <grb/detail/monoid_traits.hpp>
#include <grb/detail/concepts.hpp>
#include <optional>

namespace grb {

template <VectorRange A,
          VectorRange B,
          BinaryOperator<grb::vector_scalar_t<A>, grb::vector_scalar_t<B>> Combine = grb::multiplies<>,
          BinaryOperator<grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>> Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>>
auto multiply(A&& a,
              B&& b,
              Reduce&& reduce = Reduce{},
              Combine&& combine = Combine{},
              M&& mask = M{})
{
  using a_scalar_type = grb::vector_scalar_t<A>;
  using b_scalar_type = grb::vector_scalar_t<B>;

  using a_index_type = grb::vector_index_t<A>;
  using b_index_type = grb::vector_index_t<B>;

  using c_scalar_type = decltype(combine(std::declval<a_scalar_type>(), std::declval<b_scalar_type>()));

  c_scalar_type c;
  bool contains = false;

  for (auto&& [a_index, a_v] : a) {
    auto i = a_index;

    auto iter = b.find(i);

    if (iter != b.end()) {
      auto&& [_, b_v] = *iter;

      if constexpr(std::is_same_v<std::decay_t<M>, grb::full_vector_mask<>>) {
        if (!contains) {
          c = combine(a_v, b_v);
          contains = true;
        } else {
          c = reduce(c, combine(a_v, b_v));
        }
      } else {
        auto iter = mask.find(i);

        if (iter != mask.end()) {
          auto&& [_, value] = *iter;
          if (bool(value)) {
            if (!contains) {
              contains = true;
              c = combine(a_v, b_v);
            } else {
              c = reduce(c, combine(a_v, b_v));
            }
          }
        }
      }
    }
  }

  std::optional<c_scalar_type> c_optional;
  if (contains) {
    c_optional = c;
  }
  return c_optional;
}

template <VectorRange A,
          VectorRange B,
          BinaryOperator<grb::vector_scalar_t<A>, grb::vector_scalar_t<B>> Combine = grb::multiplies<>,
          BinaryOperator<grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>,
                         grb::elementwise_return_type_t<A, B, Combine>> Reduce = grb::plus<>,
          MaskVectorRange M = grb::full_vector_mask<>>
auto dot(A&& a,
         B&& b,
         Reduce&& reduce = Reduce{},
         Combine&& combine = Combine{},
         M&& mask = M{}) {
  return multiply(std::forward<A>(a), std::forward<B>(b), std::forward<Reduce>(reduce),
                  std::forward<Combine>(combine), std::forward<M>(mask));
}

} // end grb