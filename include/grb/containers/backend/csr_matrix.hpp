
#pragma once

#include <climits>
#include <grb/containers/backend/coo_matrix.hpp>
#include <grb/containers/backend/csr_matrix_iterator.hpp>
#include <grb/containers/matrix_entry.hpp>
#include <grb/experimental/sycl_tools/vector.hpp>
#include <grb/util/index.hpp>
#include <grb/util/matrix_io.hpp>
#include <limits>
#include <vector>

namespace grb {

template <typename T, std::integral I = std::size_t,
          typename Allocator = std::allocator<T>>
class csr_matrix {
private:
  template <typename... Args>
  using vector_type = shp::vector<Args...>;

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

  using iterator = csr_matrix_iterator<
      T, index_type, typename vector_type<T, Allocator>::iterator,
      typename vector_type<T, Allocator>::const_iterator,
      typename vector_type<I, index_allocator_type>::const_iterator>;

  using const_iterator = csr_matrix_iterator<
      std::add_const_t<T>, index_type,
      typename vector_type<T, Allocator>::iterator,
      typename vector_type<T, Allocator>::const_iterator,
      typename vector_type<I, index_allocator_type>::const_iterator>;

  using reference = grb::matrix_ref<T, index_type>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, index_type>;

  using scalar_reference = typename vector_type<T, allocator_type>::reference;

  using pointer = iterator;
  using const_pointer = const_iterator;

  iterator begin() noexcept {
    return iterator(0, 0, values_, rowptr_, colind_);
  }

  const_iterator begin() const noexcept {
    return const_iterator(0, 0, values_, rowptr_, colind_);
  }

  iterator end() noexcept {
    return iterator(shape()[0], size(), values_, rowptr_, colind_);
  }

  const_iterator end() const noexcept {
    return const_iterator(shape()[0], size(), values_, rowptr_, colind_);
  }

  grb::index<I> shape() const noexcept {
    return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

  template <typename InputIt>
  void insert(InputIt first, InputIt last);

  std::pair<iterator, bool> insert(const value_type& value);

  template <class M>
  std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj);

  iterator find(key_type key) noexcept;
  const_iterator find(key_type key) const noexcept;

  void reshape(grb::index<I> shape) {
    bool all_inside = true;
    for (auto&& [index, v] : *this) {
      auto&& [i, j] = index;
      if (!(i < shape[0] && j < shape[1])) {
        all_inside = false;
        break;
      }
    }

    if (all_inside) {
      auto last_idx = rowptr_[this->shape()[0]];
      rowptr_.resize(shape[0] + 1, last_idx);
      m_ = shape[0];
      n_ = shape[1];
      return;
    } else {
      grb::coo_matrix<T, I> new_tuples(shape);
      for (auto&& [index, v] : *this) {
        auto&& [i, j] = index;
        if (i < shape[0] && j < shape[1]) {
          new_tuples.insert({index, v});
        }
      }
      m_ = shape[0];
      n_ = shape[1];
      assign_tuples(new_tuples.begin(), new_tuples.end());
    }
  }

  csr_matrix(grb::index<I> shape);
  csr_matrix(grb::index<I> shape, const Allocator& allocator);

  csr_matrix(const Allocator& allocator) : allocator_(allocator) {}

  csr_matrix() = default;
  ~csr_matrix() = default;
  csr_matrix(const csr_matrix&) = default;
  csr_matrix& operator=(const csr_matrix&) = default;

  std::size_t nbytes() const noexcept {
    std::size_t size_bytes = 0;
    size_bytes += rowptr_.size() * sizeof(index_type);
    size_bytes += colind_.size() * sizeof(index_type);
    if constexpr (!std::is_same_v<scalar_type, bool>) {
      size_bytes += values_.size() * sizeof(scalar_type);
    } else {
      size_bytes += (values_.size() + CHAR_BIT - 1) / CHAR_BIT;
    }
    return size_bytes;
  }

  csr_matrix(csr_matrix&& other)
      : rowptr_(std::move(other.rowptr_)), colind_(std::move(other.colind_)),
        values_(std::move(other.values_)), m_(other.m_), n_(other.n_),
        nnz_(other.nnz_) {
    other.m_ = 0;
    other.n_ = 0;
    other.nnz_ = 0;
    other.rowptr_ =
        vector_type<index_type, index_allocator_type>({0}, allocator_);
  }

  csr_matrix& operator=(csr_matrix&& other) {
    rowptr_ = std::move(other.rowptr_);
    colind_ = std::move(other.colind_);
    values_ = std::move(other.values_);
    m_ = other.m_;
    other.m_ = 0;
    n_ = other.n_;
    other.n_ = 0;
    nnz_ = other.nnz_;
    other.nnz_ = 0;
    return *this;
  }

private:
  template <typename InputIt>
  void assign_tuples(InputIt first, InputIt last);

  index_type m_ = 0;
  index_type n_ = 0;
  size_type nnz_ = 0;

  Allocator allocator_;

  vector_type<index_type, index_allocator_type> rowptr_ =
      vector_type<index_type, index_allocator_type>({0}, allocator_);
  vector_type<index_type, index_allocator_type> colind_ =
      vector_type<index_type, index_allocator_type>(allocator_);
  vector_type<T, allocator_type> values_ =
      vector_type<T, allocator_type>(allocator_);
};

template <typename T, std::integral I, typename Allocator>
csr_matrix<T, I, Allocator>::csr_matrix(index<I> shape,
                                        const Allocator& allocator)
    : m_(shape[0]), n_(shape[1]), nnz_(0), allocator_(allocator) {
  if (shape[0] + 1 > std::numeric_limits<I>::max() ||
      shape[1] + 1 > std::numeric_limits<I>::max()) {
    throw std::runtime_error(
        "grb::matrix({" + std::to_string(shape[0]) + std::to_string(shape[1]) +
        "}: dimensions too big for " + std::to_string(sizeof(I) * CHAR_BIT) +
        "-bit integer");
  }
  rowptr_.resize(shape[0] + 1, 0);
}

template <typename T, std::integral I, typename Allocator>
csr_matrix<T, I, Allocator>::csr_matrix(index<I> shape)
    : m_(shape[0]), n_(shape[1]), nnz_(0) {
  if (shape[0] + 1 > std::numeric_limits<I>::max() ||
      shape[1] + 1 > std::numeric_limits<I>::max()) {
    throw std::runtime_error(
        "grb::matrix({" + std::to_string(shape[0]) + std::to_string(shape[1]) +
        "}: dimensions too big for " + std::to_string(sizeof(I) * CHAR_BIT) +
        "-bit integer");
  }
  rowptr_.resize(shape[0] + 1, 0);
}

// Refill the contents of the matrix so that it only contains the *sorted*
// tuples stored in `tuples`.  Tuples must be within the bounds of the current
// dimensions of the matrix.
template <typename T, std::integral I, typename Allocator>
template <typename InputIt>
void csr_matrix<T, I, Allocator>::assign_tuples(InputIt first, InputIt last) {
  nnz_ = last - first;
  rowptr_.resize(shape()[0] + 1);
  colind_.resize(nnz_);
  values_.resize(nnz_);

  rowptr_[0] = 0;

  size_type r = 0;
  size_type c = 0;
  for (auto iter = first; iter != last; ++iter) {
    auto&& [index, value] = *iter;
    auto&& [i, j] = index;

    values_[c] = value;
    colind_[c] = j;

    while (r < i) {
      if (r + 1 > m_) {
        // TODO: exception?
        // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
      }
      rowptr_[r + 1] = c;
      r++;
    }
    c++;

    if (c > nnz_) {
      // TODO: exception?
      // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
    }
  }

  for (; r < m_; r++) {
    rowptr_[r + 1] = nnz_;
  }
}

// Currently assumes NO DUPLICATES in range first -> last
template <typename T, std::integral I, typename Allocator>
template <typename InputIt>
void csr_matrix<T, I, Allocator>::insert(InputIt first, InputIt last) {
  // a < b
  auto sort_fn = [](const auto& a, const auto& b) {
    auto&& [a_index, a_value] = a;
    auto&& [b_index, b_value] = b;
    auto&& [a_i, a_j] = a_index;
    auto&& [b_i, b_j] = b_index;
    if (a_i < b_i) {
      return true;
    } else if (a_i == b_i) {
      if (a_j < b_j) {
        return true;
      }
    }
    return false;
  };

  using tuple_type = value_type;
  vector_type<tuple_type> my_sorted_indices(begin(), end());

  vector_type<tuple_type> sorted_indices_toadd(first, last);
  std::ranges::sort(my_sorted_indices, sort_fn);
  std::ranges::sort(sorted_indices_toadd, sort_fn);

  vector_type<tuple_type> output_indices(my_sorted_indices.size() +
                                         sorted_indices_toadd.size());

  auto new_last =
      std::set_union(my_sorted_indices.begin(), my_sorted_indices.end(),
                     sorted_indices_toadd.begin(), sorted_indices_toadd.end(),
                     output_indices.begin(), sort_fn);
  /*
    vector_type<std::tuple<index_type, index_type, value_type>>
    merged_tuples(nnz() + (last - first)); std::set_union(begin(), end(), first,
    last, merged_tuples.begin(), sort_fn);
    */

  assign_tuples(output_indices.begin(), new_last);
}

// NOTE: if sorted, you could exit this for loop early
template <typename T, std::integral I, typename Allocator>

typename csr_matrix<T, I, Allocator>::iterator
csr_matrix<T, I, Allocator>::find(key_type key) noexcept {
  index_type i = key[0];
  for (index_type j_ptr = rowptr_[i]; j_ptr < rowptr_[i + 1]; j_ptr++) {
    if (colind_[j_ptr] == key[1]) {
      return iterator(i, j_ptr, values_, rowptr_, colind_);
    }
  }
  return end();
}

template <typename T, std::integral I, typename Allocator>
typename csr_matrix<T, I, Allocator>::const_iterator
csr_matrix<T, I, Allocator>::find(key_type key) const noexcept {
  index_type i = key[0];
  for (index_type j_ptr = rowptr_[i]; j_ptr < rowptr_[i + 1]; j_ptr++) {
    if (colind_[j_ptr] == key[1]) {
      return const_iterator(i, j_ptr, values_, rowptr_, colind_);
    }
  }
  return end();
}

template <typename T, std::integral I, typename Allocator>
std::pair<typename csr_matrix<T, I, Allocator>::iterator, bool>
csr_matrix<T, I, Allocator>::insert(
    const typename csr_matrix<T, I, Allocator>::value_type& value) {
  auto&& [idx, v] = value;
  auto iter = find(idx);
  if (iter != end()) {
    return {iter, false};
  } else {
    insert(&value, &value + 1);
    auto iter = find(idx);
    return {iter, true};
  }
}

template <typename T, std::integral I, typename Allocator>
template <class M>
std::pair<typename csr_matrix<T, I, Allocator>::iterator, bool>
csr_matrix<T, I, Allocator>::insert_or_assign(
    csr_matrix<T, I, Allocator>::key_type k, M&& obj) {
  auto iter = find(k);

  if (iter != end()) {
    auto&& [index, value] = *iter;
    value = std::forward<M>(obj);
    return {iter, false};
  } else {
    auto&& [iter, flag] = insert({k, std::forward<M>(obj)});
    return {iter, true};
  }
}

} // namespace grb
