#pragma once

#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/detail/matrix_traits.hpp>
#include <grb/util/index.hpp>
#include <variant>

namespace grb {

template <typename Iterator, typename Fn>
class filter_accessor {
public:
  using scalar_type = decltype(std::declval<Fn>()(*std::declval<Iterator>()));
  using index_type =
      typename __detail::get_index_type<std::iter_value_t<Iterator>>::type;
  using difference_type = std::iter_difference_t<Iterator>;

  using reference = std::iter_reference_t<Iterator>;

  using value_type = std::iter_value_t<Iterator>;

  using iterator_accessor = filter_accessor;
  using const_iterator_accessor = filter_accessor;
  using nonconst_iterator_accessor = filter_accessor;

  using iterator_category = std::forward_iterator_tag;

  filter_accessor() = default;
  ~filter_accessor() = default;
  filter_accessor(const filter_accessor&) = default;
  filter_accessor(filter_accessor&&) = default;

  filter_accessor& operator=(const filter_accessor& other) {
    iter_ = other.iter_;
    return *this;
  }

  filter_accessor& operator=(filter_accessor&& other) {
    iter_ = other.iter_;
    return *this;
  }

  filter_accessor(Iterator iter, Iterator end, Fn fn)
      : iter_(iter), end_(end), fn_(fn) {
    fast_forward();
  }

  filter_accessor& operator++() noexcept {
    ++iter_;
    fast_forward();
    return *this;
  }

  void fast_forward() {
    while (iter_ != end_ && !std::get<Fn>(fn_)(*iter_)) {
      ++iter_;
    }
  }

  bool operator==(const filter_accessor& other) const noexcept {
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
using filter_iterator =
    grb::detail::iterator_adaptor<filter_accessor<Iterator, Fn>>;

template <typename ContainerType, std::copy_constructible Fn>
  requires(grb::MatrixRange<ContainerType> || grb::VectorRange<ContainerType>)
class filter_view
    : public std::ranges::view_interface<filter_view<ContainerType, Fn>> {
public:
  using container_type = std::decay_t<ContainerType>;

  using index_type = container_index_t<container_type>;
  using scalar_type = decltype(std::declval<Fn>()(
      std::declval<container_value_t<container_type>>()));
  using size_type = container_size_t<container_type>;
  using difference_type = container_difference_t<container_type>;

  using key_type = container_key_t<container_type>;

  using iterator = filter_iterator<
      decltype(std::declval<std::ranges::views::all_t<ContainerType>>()
                   .base()
                   .begin()),
      Fn>;
  using const_iterator = iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  filter_view(ContainerType matrix, Fn fn) : matrix_(matrix), fn_(fn) {}

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
    if (iter == base().end() || !fn_(*iter)) {
      return end();
    } else {
      return iterator(base().find(key), base().end(), fn_);
    }
  }

  decltype(auto) base() const noexcept {
    return matrix_.base();
  }

private:
  std::ranges::views::all_t<ContainerType> matrix_;
  Fn fn_;
};

template <grb::MatrixRange MatrixType, std::copy_constructible Fn>
filter_view(MatrixType&&, Fn) -> filter_view<MatrixType, Fn>;

template <grb::VectorRange VectorType, std::copy_constructible Fn>
filter_view(VectorType&&, Fn) -> filter_view<VectorType, Fn>;

} // namespace grb
