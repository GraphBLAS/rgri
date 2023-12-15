#pragma once

#include <experimental/mdspan>
#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/util/index.hpp>

namespace grb {

template <typename MDSpanType>
class mdspan_matrix_accessor {
public:
  using mdspan_type = std::remove_cvref_t<MDSpanType>;

  using scalar_type = typename mdspan_type::value_type;
  using scalar_reference = typename mdspan_type::reference;

  using index_type = typename mdspan_type::index_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  using difference_type = std::ptrdiff_t;
  using iterator_accessor = mdspan_matrix_accessor;
  using const_iterator_accessor = mdspan_matrix_accessor;
  using nonconst_iterator_accessor = mdspan_matrix_accessor;
  using iterator_category = std::random_access_iterator_tag;

  constexpr mdspan_matrix_accessor() noexcept = default;
  constexpr ~mdspan_matrix_accessor() noexcept = default;
  constexpr mdspan_matrix_accessor(const mdspan_matrix_accessor&) noexcept =
      default;
  constexpr mdspan_matrix_accessor&
  operator=(const mdspan_matrix_accessor&) noexcept = default;

  constexpr mdspan_matrix_accessor(mdspan_type mdspan,
                                   grb::index<index_type> index) noexcept
      : mdspan_(mdspan), i_(index[0]), j_(index[1]) {}

  constexpr mdspan_matrix_accessor&
  operator+=(difference_type offset) noexcept {
    auto global_idx = i_ * mdspan_.extent(1) + j_;
    auto new_idx = global_idx + offset;
    i_ = new_idx / mdspan_.extent(1);
    j_ = new_idx % mdspan_.extent(1);
    return *this;
  }

  constexpr mdspan_matrix_accessor& operator++() noexcept {
    j_++;
    if (j_ >= mdspan_.extent(1)) {
      i_++;
      j_ = 0;
    }
    return *this;
  }

  constexpr mdspan_matrix_accessor& operator--() noexcept {
    if (j_ > 0) {
      j_--;
    } else {
      i_--;
      j_ = mdspan_.extent(1) - 1;
    }
    return *this;
  }

  constexpr difference_type
  operator-(const const_iterator_accessor& other) const noexcept {
    difference_type row_diff = difference_type(i_) - difference_type(other.i_);
    difference_type col_diff = difference_type(j_) - difference_type(other.j_);

    return row_diff * mdspan_.extent(1) + col_diff;
  }

  constexpr bool
  operator==(const const_iterator_accessor& other) const noexcept {
    return i_ == other.i_ && j_ == other.j_;
  }

  constexpr bool
  operator<(const const_iterator_accessor& other) const noexcept {
    if (i_ < other.i_) {
      return true;
    } else if (i_ == other.i_) {
      return j_ < other.j_;
    } else {
      return false;
    }
  }

  constexpr reference operator*() const noexcept {
    return reference({i_, j_}, mdspan_(i_, j_));
  }

private:
  index_type i_;
  index_type j_;
  mdspan_type mdspan_;
};

template <typename MDSpanType>
using mdspan_matrix_iterator =
    grb::detail::iterator_adaptor<mdspan_matrix_accessor<MDSpanType>>;

template <typename... Args>
class mdspan_matrix_view {
public:
  using mdspan_type = std::experimental::mdspan<Args...>;

  using scalar_type = typename mdspan_type::value_type;
  using scalar_reference = typename mdspan_type::reference;

  using index_type = typename mdspan_type::index_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  using key_type = grb::index<index_type>;

  using difference_type = std::ptrdiff_t;

  using iterator = grb::mdspan_matrix_iterator<mdspan_type>;

  mdspan_matrix_view(mdspan_type mdspan) : mdspan_(mdspan) {}

  auto begin() {
    return iterator(mdspan_, key_type{0, 0});
  }

  auto end() {
    return iterator(mdspan_, key_type{mdspan_.extent(0), 0});
  }

  auto find(const key_type& key) {
    return iterator(mdspan_, key);
  }

  auto insert(const value_type& entry) {
    auto&& [index, value] = entry;
    return iterator(mdspan_, index);
  }

  key_type shape() {
    return key_type(mdspan_.extent(0), mdspan_.extent(1));
  }

  auto size() {
    return mdspan_.size();
  }

  template <typename O>
  auto insert_or_assign(const key_type& key, O&& obj) {
    mdspan_(key[0], key[1]) = std::forward<O>(obj);
    return iterator(mdspan_, key);
  }

  scalar_reference operator[](const key_type& key) {
    return mdspan_(key[0], key[1]);
  }

  decltype(auto) base() noexcept {
    return mdspan_;
  }

private:
  mdspan_type mdspan_;
};

template <typename... Args>
mdspan_matrix_view(std::experimental::mdspan<Args...>)
    -> mdspan_matrix_view<Args...>;

} // namespace grb

namespace grb {

namespace views {

template <typename... Args>
auto tag_invoke(grb::tag_t<grb::views::all>,
                std::experimental::mdspan<Args...> m)
  requires(m.rank() == 2)
{
  return grb::mdspan_matrix_view(m);
}

} // namespace views

} // namespace grb
