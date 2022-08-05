#pragma once

#include <grb/util/index.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/detail/matrix_traits.hpp>

namespace grb {

template <typename Iterator, typename Fn>
class filter_matrix_accessor {
public:
  using difference_type = typename Iterator::difference_type;

  using value_type = std::remove_cvref_t<decltype(*std::declval<Iterator>())>;

  using scalar_type = std::remove_cvref_t<decltype(grb::get<1>(*std::declval<Iterator>()))>;
  using index_type = std::remove_cvref_t<decltype(grb::get<0>(grb::get<0>(*std::declval<Iterator>())))>;

  using reference = value_type;

  using iterator_accessor = filter_matrix_accessor;
  using const_iterator_accessor = filter_matrix_accessor;
  using nonconst_iterator_accessor = filter_matrix_accessor;

  using iterator_category = std::forward_iterator_tag;

  filter_matrix_accessor() noexcept = default;
  ~filter_matrix_accessor() noexcept = default;
  filter_matrix_accessor(const filter_matrix_accessor&) noexcept = default;
  filter_matrix_accessor& operator=(const filter_matrix_accessor&) noexcept = default;

  filter_matrix_accessor(Iterator iter, Iterator end, Fn fn) : iter_(iter), end_(end), fn_(fn) {
    fast_forward();
  }

  filter_matrix_accessor& operator++() noexcept {
    ++iter_;
    fast_forward();
    return *this;
  }

  void fast_forward() {
    while (iter_ != end_ && !fn_(*iter_)) {
      iter_++;
    }
  }

  bool operator==(const filter_matrix_accessor& other) const noexcept {
    return iter_ == other.iter_;
  }

  reference operator*() const noexcept {
    return *iter_;
  }

private:
  Iterator iter_;
  Iterator end_;
  Fn fn_;
};

template <typename Iterator, typename Fn>
using filter_matrix_iterator = grb::detail::iterator_adaptor<filter_matrix_accessor<Iterator, Fn>>;

template <typename MatrixType, typename Fn>
class filter_matrix_view {
public:

  using matrix_type = std::decay_t<MatrixType>;

  using index_type = typename matrix_type::index_type;
  using scalar_type = typename matrix_type::scalar_type;
  using size_type = typename matrix_type::size_type;
  using difference_type = typename matrix_type::difference_type;

  using iterator = filter_matrix_iterator<typename matrix_type::const_iterator, Fn>;
  using const_iterator = iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  using key_type = typename matrix_type::key_type;

  filter_matrix_view(const MatrixType& matrix, Fn fn) : matrix_(matrix), fn_(fn) {}

  auto shape() const noexcept {
    return matrix_.shape();
  }

  size_type size() const noexcept {
    auto iter = begin();
    std::size_t size_ = 0;
    while (iter != end()) {
      ++iter;
      ++size_;
    }
    return size_;
  }

  iterator begin() const noexcept {
    return iterator(matrix_.begin(), matrix_.end(), fn_);
  }

  iterator end() const noexcept {
    return iterator(matrix_.end(), matrix_.end(), fn_);
  }

  iterator find(key_type key) const noexcept {
    auto iter = matrix_.find(key);
    if (fn_(*iter)) {
      return iterator(iter, matrix_.end(), fn_);
    } else {
      return end();
    }
  }

private:
  const MatrixType& matrix_;
  Fn fn_;
};

template <typename MatrixType, typename Fn>
auto filter(const MatrixType& matrix, Fn fn) {
  return filter_matrix_view<MatrixType, Fn>(matrix, fn);
}

} // end grb