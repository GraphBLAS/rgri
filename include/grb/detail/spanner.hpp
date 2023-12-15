#pragma once

#include <iterator>

namespace grb {

namespace detail {

template <std::random_access_iterator Iter>
class spanner {
public:
  using value_type = std::iter_value_t<Iter>;
  using size_type = std::size_t;
  using difference_type = std::iter_difference_t<Iter>;
  using reference = std::iter_reference_t<Iter>;
  using iterator = Iter;

  template <std::ranges::random_access_range R>
  spanner(R&& r) : begin_(std::ranges::begin(r)), end_(std::ranges::end(r)) {}
  spanner(Iter first, Iter last) : begin_(first), end_(last) {}
  spanner(Iter first, std::size_t count) : begin_(first), end_(first + count) {}

  spanner() = default;
  spanner(const spanner&) noexcept = default;
  spanner& operator=(const spanner&) noexcept = default;

  std::size_t size() const noexcept {
    return end() - begin();
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  Iter begin() const noexcept {
    return begin_;
  }

  Iter end() const noexcept {
    return end_;
  }

  reference operator[](size_type index) const {
    return *(begin() + index);
  }

  spanner first(size_type n) const {
    return spanner(begin(), begin() + n);
  }

  spanner last(size_type n) const {
    return spanner(end() - n, end());
  }

  spanner subspan(std::size_t offset, std::size_t count) const {
    return spanner(begin() + offset, begin() + offset + count);
  }

private:
  Iter begin_, end_;
};

template <std::ranges::random_access_range R>
spanner(R&&) -> spanner<std::ranges::iterator_t<R>>;

} // namespace detail
} // namespace grb
