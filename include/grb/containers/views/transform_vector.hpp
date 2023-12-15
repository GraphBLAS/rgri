#pragma once

#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/concepts.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/util/index.hpp>
#include <tuple>
#include <type_traits>
#include <variant>

namespace grb {

template <typename Iterator, typename Fn>
class transform_vector_accessor {
public:
  using scalar_type = decltype(std::declval<Fn>()(*std::declval<Iterator>()));
  using index_type =
      typename __detail::get_index_type<std::iter_value_t<Iterator>>::type;
  using difference_type = std::iter_difference_t<Iterator>;

  using value_type = grb::vector_entry<scalar_type, index_type>;
  using reference = grb::vector_entry<scalar_type, index_type>;

  using iterator_accessor = transform_vector_accessor;
  using const_iterator_accessor = transform_vector_accessor;
  using nonconst_iterator_accessor = transform_vector_accessor;

  using iterator_category =
      typename std::iterator_traits<Iterator>::iterator_category;

  transform_vector_accessor() = default;
  ~transform_vector_accessor() = default;
  transform_vector_accessor(const transform_vector_accessor&) = default;
  transform_vector_accessor(transform_vector_accessor&&) = default;

  transform_vector_accessor& operator=(const transform_vector_accessor& other) {
    iter_ = other.iter_;
    return *this;
  }

  transform_vector_accessor& operator=(transform_vector_accessor&& other) {
    iter_ = other.iter_;
    return *this;
  }

  transform_vector_accessor(Iterator iter, Fn fn) : iter_(iter), fn_(fn) {}

  transform_vector_accessor& operator++() noexcept {
    ++iter_;
    return *this;
  }

  transform_vector_accessor& operator+=(difference_type offset) noexcept
    requires(std::is_same_v<iterator_category, std::random_access_iterator_tag>)
  {
    iter_ += offset;
    return *this;
  }

  bool operator==(const transform_vector_accessor& other) const noexcept {
    return iter_ == other.iter_;
  }

  reference operator*() const noexcept {
    auto&& [index, _] = *iter_;
    return reference(index, fn()(*iter_));
  }

  const Fn& fn() const noexcept {
    return std::get<Fn>(fn_);
  }

private:
  Iterator iter_;
  std::variant<char, Fn> fn_;
};

template <typename Iterator, typename Fn>
using transform_vector_iterator =
    grb::detail::iterator_adaptor<transform_vector_accessor<Iterator, Fn>>;

template <grb::VectorRange VectorType, std::copy_constructible Fn>
class transform_vector_view : public std::ranges::view_interface<
                                  transform_vector_view<VectorType, Fn>> {
public:
  using vector_type = std::decay_t<VectorType>;

  using index_type = container_index_t<vector_type>;
  using scalar_type = decltype(std::declval<Fn>()(
      std::declval<container_value_t<vector_type>>()));
  using size_type = container_size_t<vector_type>;
  using difference_type = container_difference_t<vector_type>;

  using key_type = container_key_t<vector_type>;

  using iterator = transform_vector_iterator<
      decltype(std::declval<std::ranges::views::all_t<VectorType>>()
                   .base()
                   .begin()),
      Fn>;
  using const_iterator = iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  transform_vector_view(VectorType&& vector, Fn fn)
      : vector_(vector), fn_(fn) {}

  auto shape() const noexcept {
    return base().shape();
  }

  size_type size() const noexcept {
    return base().size();
  }

  iterator begin() const noexcept {
    return iterator(base().begin(), fn_);
  }

  iterator end() const noexcept {
    return iterator(base().end(), fn_);
  }

  iterator find(key_type key) const noexcept {
    return iterator(base().find(key), fn_);
  }

  decltype(auto) base() const noexcept {
    return vector_.base();
  }

private:
  std::ranges::views::all_t<VectorType> vector_;
  Fn fn_;
};

template <grb::VectorRange VectorType, std::copy_constructible Fn>
transform_vector_view(VectorType&&, Fn)
    -> transform_vector_view<VectorType, Fn>;

} // namespace grb
