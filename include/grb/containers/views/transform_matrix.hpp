#pragma once

#include <grb/util/index.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <utility>

namespace grb {

template <typename Iterator, typename Fn>
class transform_matrix_accessor {
public:
  using scalar_type = typename Iterator::scalar_type;
  using index_type = typename Iterator::index_type;
  using difference_type = typename Iterator::difference_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_entry<scalar_type, index_type>;

  using iterator_accessor = transform_matrix_accessor;
  using const_iterator_accessor = transform_matrix_accessor;
  using nonconst_iterator_accessor = transform_matrix_accessor;

  using iterator_category = typename Iterator::iterator_category;

  transform_matrix_accessor() noexcept = default;
  ~transform_matrix_accessor() noexcept = default;
  transform_matrix_accessor(const transform_matrix_accessor&) noexcept = default;
  transform_matrix_accessor& operator=(const transform_matrix_accessor&) noexcept = default;

  transform_matrix_accessor(Iterator iter, Fn fn) : iter_(iter), fn_(fn) {}

  transform_matrix_accessor& operator++() noexcept {
    iter_++;
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
    return reference(index, fn_(*iter_));
  }

private:
  Iterator iter_;
  Fn fn_;
};

template <typename Iterator, typename Fn>
using transform_matrix_iterator = grb::detail::iterator_adaptor<transform_matrix_accessor<Iterator, Fn>>;

template <typename MatrixType,
          typename Fn>
class transform_matrix_view {
public:

  using matrix_type = MatrixType;

  using index_type = typename matrix_type::index_type;
  using scalar_type = std::remove_cvref_t<decltype(std::declval<Fn>()(std::declval<typename matrix_type::value_type>()))>;
  using size_type = typename matrix_type::size_type;
  using difference_type = typename matrix_type::difference_type;

  using key_type = typename matrix_type::key_type;

  using iterator = transform_matrix_iterator<typename matrix_type::const_iterator, Fn>;

  transform_matrix_view(const MatrixType& matrix, Fn fn) : matrix_(matrix), fn_(fn) {}

  grb::index<index_type> shape() const noexcept {
    return {matrix_.shape()[0], matrix_.shape()[1]};
  }

  size_type size() const noexcept {
    return matrix_.size();
  }

  iterator begin() const noexcept {
    return iterator(matrix_.begin(), fn_);
  }

  iterator end() const noexcept {
    return iterator(matrix_.end(), fn_);
  }

  scalar_type operator[](grb::index<index_type> index) const noexcept {
    return fn_(static_cast<scalar_type>(matrix_[index]));
  }

  iterator find(key_type key) const noexcept {
    return iterator(matrix_.find(key), fn_);
  }

private:
  const MatrixType& matrix_;
  Fn fn_;
};

template <typename MatrixType, typename Fn>
auto transform(MatrixType& matrix, Fn fn) {
  return transform_matrix_view<MatrixType, Fn>(matrix, fn);
}

} // end grb