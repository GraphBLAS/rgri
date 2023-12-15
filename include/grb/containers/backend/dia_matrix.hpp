#pragma once

#include <climits>
#include <grb/containers/backend/dia_matrix_iterator.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/util/index.hpp>
#include <grb/util/matrix_io.hpp>
#include <limits>
#include <map>
#include <vector>

namespace grb {

template <typename T, std::integral I = std::size_t,
          typename Allocator = std::allocator<T>>
class dia_matrix {
public:
  using scalar_type = T;
  using index_type = I;
  using value_type = grb::matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;
  using index_allocator_type = typename std::allocator_traits<
      allocator_type>::template rebind_alloc<index_type>;

  using iterator = dia_matrix_iterator<T, index_type>;

  using const_iterator = dia_matrix_iterator<std::add_const_t<T>, index_type>;

  using reference = grb::matrix_ref<T, index_type>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, index_type>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  dia_matrix(grb::index<I> shape) : m_(shape[0]), n_(shape[1]) {}

  grb::index<I> shape() const noexcept {
    return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

  dia_matrix() = default;
  ~dia_matrix() = default;
  dia_matrix(const dia_matrix&) = default;
  dia_matrix(dia_matrix&&) = default;
  dia_matrix& operator=(const dia_matrix&) = default;
  dia_matrix& operator=(dia_matrix&&) = default;

  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    for (auto iter = first; iter != last; ++iter) {
      insert(*iter);
    }
  }

  std::pair<iterator, bool> insert(const value_type& value) {
    auto&& [index, v] = value;
    auto&& [i, j] = index;
    index_type diagonal =
        (difference_type(j) - difference_type(i)) + shape()[0] - 1;
    index_type idx = std::min(i, j);
    // index_type count = std::min(m_, n_) - std::abs(difference_type(i) -
    // difference_type(j));
    index_type count = std::min(m_ - i, n_ - j) + idx;

    auto iter = diagonals_.find(diagonal);
    if (iter == diagonals_.end()) {
      auto [it, _] = diagonals_.insert(
          {diagonal, {std::vector<T>(count), std::vector<bool>(count, false)}});
      iter = it;
    }

    auto&& [vec, bvec] = iter->second;

    if (!bvec[idx]) {
      vec[idx] = v;
      bvec[idx] = true;
      nnz_++;
      return {iterator(idx, m_, n_, iter, diagonals_.end()), true};
    } else {
      return {iterator(idx, m_, n_, iter, diagonals_.end()), false};
    }
  }

  template <typename M>
  std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
    auto&& [i, j] = k;
    index_type diagonal =
        (difference_type(j) - difference_type(i)) + shape()[0] - 1;
    index_type idx = std::min(i, j);
    // index_type count = std::min(m_, n_) - std::abs(difference_type(i) -
    // difference_type(j));
    index_type count = std::min(m_ - i, n_ - j) + idx;

    auto iter = diagonals_.find(diagonal);
    if (iter == diagonals_.end()) {
      auto [it, b] = diagonals_.insert(
          {diagonal,
           {std::vector<T>(count, false), std::vector<bool>(count, false)}});
      iter = it;
    }

    auto&& [vec, bvec] = iter->second;
    vec[idx] = std::forward<M>(obj);

    if (!bvec[idx]) {
      bvec[idx] = true;
      nnz_++;
      return {iterator(idx, m_, n_, iter, diagonals_.end()), true};
    } else {
      return {iterator(idx, m_, n_, iter, diagonals_.end()), false};
    }
  }

  iterator begin() noexcept {
    return iterator(0, m_, n_, diagonals_.begin(), diagonals_.end());
  }

  const_iterator begin() const noexcept {
    return const_iterator(0, m_, n_, diagonals_.begin(), diagonals_.end());
  }

  iterator end() noexcept {
    return iterator(0, m_, n_, diagonals_.end(), diagonals_.end());
  }

  const_iterator end() const noexcept {
    return const_iterator(0, m_, n_, diagonals_.end(), diagonals_.end());
  }

  std::size_t nbytes() const noexcept {
    size_t size_bytes = 0;
    for (auto&& diagonal : diagonals_) {
      auto&& [_, arrays] = diagonal;
      auto&& [values, bvec] = arrays;
      if constexpr (!std::is_same_v<scalar_type, bool>) {
        size_bytes += values.size() * sizeof(scalar_type);
      } else {
        size_bytes += (values.size() + CHAR_BIT - 1) / CHAR_BIT;
      }
      size_bytes += (bvec.size() + CHAR_BIT - 1) / CHAR_BIT;
    }
    return size_bytes;
  }

private:
  index_type m_ = 0;
  index_type n_ = 0;
  size_type nnz_ = 0;

  std::map<index_type, std::pair<std::vector<T>, std::vector<bool>>> diagonals_;
};

} // namespace grb
