#pragma once

#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/concepts.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/util/index.hpp>

#include <ranges>

namespace grb {

template <typename Iterator>
class transpose_matrix_accessor {
public:
  using scalar_type =
      std::remove_cvref_t<decltype(grb::get<1>(*std::declval<Iterator>()))>;
  using index_type = std::remove_cvref_t<decltype(grb::get<0>(
      grb::get<0>(*std::declval<Iterator>())))>;
  using difference_type = typename Iterator::difference_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = value_type;

  using iterator_accessor = transpose_matrix_accessor;
  using const_iterator_accessor = transpose_matrix_accessor;
  using nonconst_iterator_accessor = transpose_matrix_accessor;

  using iterator_category = typename Iterator::iterator_category;

  transpose_matrix_accessor() noexcept = default;
  ~transpose_matrix_accessor() noexcept = default;
  transpose_matrix_accessor(const transpose_matrix_accessor&) noexcept =
      default;
  transpose_matrix_accessor&
  operator=(const transpose_matrix_accessor&) noexcept = default;

  transpose_matrix_accessor(Iterator iter) : iter_(iter) {}

  transpose_matrix_accessor& operator++() noexcept {
    iter_++;
    return *this;
  }

  transpose_matrix_accessor& operator+=(difference_type offset) noexcept
    requires(std::is_same_v<iterator_category, std::random_access_iterator_tag>)
  {
    iter_ += offset;
    return *this;
  }

  bool operator==(const transpose_matrix_accessor& other) const noexcept {
    return iter_ == other.iter_;
  }

  reference operator*() const noexcept {
    auto&& [index, value] = *iter_;
    return reference({index[1], index[0]}, value);
  }

  difference_type
  operator-(const transpose_matrix_accessor& other) const noexcept
    requires(std::is_same_v<iterator_category, std::random_access_iterator_tag>)
  {
    return iter_ - other.iter_;
  }

private:
  Iterator iter_;
};

template <typename Iterator>
using transpose_matrix_iterator =
    grb::detail::iterator_adaptor<transpose_matrix_accessor<Iterator>>;

template <typename MatrixType>
class transpose_matrix_view
    : public std::ranges::view_interface<transpose_matrix_view<MatrixType>> {
public:
  using matrix_type = std::decay_t<MatrixType>;

  using index_type = typename matrix_type::index_type;
  using scalar_type = typename matrix_type::scalar_type;
  using size_type = typename matrix_type::size_type;
  using difference_type = typename matrix_type::difference_type;

  using iterator =
      transpose_matrix_iterator<typename matrix_type::const_iterator>;
  using const_iterator = iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  using key_type = typename matrix_type::key_type;

  transpose_matrix_view(const MatrixType& matrix) : matrix_(matrix) {}

  grb::index<index_type> shape() const noexcept {
    return {matrix_.shape()[1], matrix_.shape()[0]};
  }

  size_type size() const noexcept {
    return matrix_.size();
  }

  iterator begin() const noexcept {
    return iterator(matrix_.begin());
  }

  iterator end() const noexcept {
    return iterator(matrix_.end());
  }

  iterator find(key_type key) const noexcept {
    return iterator(matrix_.find({key[1], key[0]}));
  }

private:
  const MatrixType& matrix_;
};

template <typename MatrixType>
auto transpose(const MatrixType& matrix) {
  return transpose_matrix_view<MatrixType>(matrix);
}

} // namespace grb
