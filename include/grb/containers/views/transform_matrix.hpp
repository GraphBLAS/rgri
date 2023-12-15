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
class transform_matrix_accessor {
public:
  using scalar_type = decltype(std::declval<Fn>()(*std::declval<Iterator>()));
  using index_type =
      typename __detail::get_index_type<std::iter_value_t<Iterator>>::type;
  using difference_type = std::iter_difference_t<Iterator>;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_entry<scalar_type, index_type>;

  using iterator_accessor = transform_matrix_accessor;
  using const_iterator_accessor = transform_matrix_accessor;
  using nonconst_iterator_accessor = transform_matrix_accessor;

  using iterator_category =
      typename std::iterator_traits<Iterator>::iterator_category;

  transform_matrix_accessor() = default;
  ~transform_matrix_accessor() = default;
  transform_matrix_accessor(const transform_matrix_accessor&) = default;
  transform_matrix_accessor(transform_matrix_accessor&&) = default;

  transform_matrix_accessor& operator=(const transform_matrix_accessor& other) {
    iter_ = other.iter_;
    return *this;
  }

  transform_matrix_accessor& operator=(transform_matrix_accessor&& other) {
    iter_ = other.iter_;
    return *this;
  }

  transform_matrix_accessor(Iterator iter, Fn fn) : iter_(iter), fn_(fn) {}

  transform_matrix_accessor& operator++() noexcept {
    ++iter_;
    return *this;
  }

  transform_matrix_accessor& operator+=(difference_type offset) noexcept
    requires(std::is_same_v<iterator_category, std::random_access_iterator_tag>)
  {
    iter_ += offset;
    return *this;
  }

  bool operator==(const transform_matrix_accessor& other) const noexcept {
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
using transform_matrix_iterator =
    grb::detail::iterator_adaptor<transform_matrix_accessor<Iterator, Fn>>;

template <grb::MatrixRange MatrixType, std::copy_constructible Fn>
class transform_matrix_view : public std::ranges::view_interface<
                                  transform_matrix_view<MatrixType, Fn>> {
public:
  using matrix_type = std::decay_t<MatrixType>;

  using index_type = container_index_t<matrix_type>;
  using scalar_type = decltype(std::declval<Fn>()(
      std::declval<container_value_t<matrix_type>>()));
  using size_type = container_size_t<matrix_type>;
  using difference_type = container_difference_t<matrix_type>;

  using key_type = container_key_t<matrix_type>;

  using iterator = transform_matrix_iterator<
      decltype(std::declval<std::ranges::views::all_t<MatrixType>>()
                   .base()
                   .begin()),
      Fn>;
  using const_iterator = iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  transform_matrix_view(MatrixType&& matrix, Fn fn)
      : matrix_(matrix), fn_(fn) {}

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
    return matrix_.base();
  }

private:
  std::ranges::views::all_t<MatrixType> matrix_;
  Fn fn_;
};

template <grb::MatrixRange MatrixType, std::copy_constructible Fn>
transform_matrix_view(MatrixType&&, Fn)
    -> transform_matrix_view<MatrixType, Fn>;

} // namespace grb
