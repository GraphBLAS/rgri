#pragma once

#include <grb/util/index.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/detail/matrix_traits.hpp>

namespace grb {

template <typename Iterator, typename Fn>
class filter_matrix_accessor {
public:
  using scalar_type = decltype(std::declval<Fn>()(*std::declval<Iterator>()));
  using index_type = typename __detail::get_index_type<std::iter_value_t<Iterator>>::type;
  using difference_type = std::iter_difference_t<Iterator>;

  using reference = std::iter_reference_t<Iterator>;

  using value_type = std::iter_value_t<Iterator>;

  using iterator_accessor = filter_matrix_accessor;
  using const_iterator_accessor = filter_matrix_accessor;
  using nonconst_iterator_accessor = filter_matrix_accessor;

  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

  filter_matrix_accessor() = default;
  ~filter_matrix_accessor() = default;
  filter_matrix_accessor(const filter_matrix_accessor&) = default;
  filter_matrix_accessor(filter_matrix_accessor&&) = default;

  filter_matrix_accessor& operator=(const filter_matrix_accessor& other) {
    iter_ = other.iter_;
    return *this;
  }

  filter_matrix_accessor& operator=(filter_matrix_accessor&& other) {
    iter_ = other.iter_;
    return *this;
  }

  filter_matrix_accessor(Iterator iter, Iterator end, Fn fn) : iter_(iter), end_(end), fn_(fn) {}

  filter_matrix_accessor& operator++() noexcept {
    do {
      ++iter_;
    } while (iter_ != end_ && !std::get<Fn>(fn_)(*iter_));
    return *this;
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
  std::variant<char, Fn> fn_;
};

template <typename Iterator, typename Fn>
using filter_matrix_iterator = grb::detail::iterator_adaptor<filter_matrix_accessor<Iterator, Fn>>;

template <grb::MatrixRange MatrixType,
          std::copy_constructible Fn>
class filter_matrix_view : public std::ranges::view_interface<filter_matrix_view<MatrixType, Fn>> {
public:

  using matrix_type = std::decay_t<MatrixType>;

  using index_type = container_index_t<matrix_type>;
  using scalar_type = decltype(std::declval<Fn>()(std::declval<container_value_t<matrix_type>>()));
  using size_type = container_size_t<matrix_type>;
  using difference_type = container_difference_t<matrix_type>;

  using key_type = container_key_t<matrix_type>;

  using iterator = filter_matrix_iterator<decltype(std::declval<std::ranges::views::all_t<MatrixType>>().base().begin()), Fn>;
  using const_iterator = iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  filter_matrix_view(MatrixType&& matrix, Fn fn) : matrix_(matrix), fn_(fn) {}

  auto shape() const noexcept {
    return base().shape();
  }

  size_type size() const noexcept {
    return std::ranges::distance(begin(), end());
  }

  iterator begin() const noexcept {
    return iterator(base().begin(), base().end(), fn_);
  }

  iterator end() const noexcept {
    return iterator(base().end(), base().end(), fn_);
  }

  iterator find(key_type key) const noexcept {
    auto iter = base().find(key);
    if (iter != base().end()) {
      return iterator(base().find(key), base().end(), fn_);
    } else {
      return end();
    }
  }

  decltype(auto) base() const noexcept {
    return matrix_.base();
  }

private:
  std::ranges::views::all_t<MatrixType> matrix_;
  Fn fn_;
};

template <grb::MatrixRange MatrixType,
          std::copy_constructible Fn>
filter_matrix_view(MatrixType&&, Fn) -> filter_matrix_view<MatrixType, Fn>;

} // end grb