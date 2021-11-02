
#pragma once

#include <grb/util/index.hpp>
#include <grb/containers/matrix_element.hpp>
#include <grb/util/matrix_io.hpp>
#include <grb/containers/backend/csr_matrix_iterator.hpp>
#include <limits>
#include <climits>

namespace grb {

template <typename T,
          typename I = std::size_t,
          typename Allocator = std::allocator<T>>
requires(std::is_integral_v<I>)
class csr_matrix {
public:
  // using element_type = T;
  using index_type = I;
  using value_type = grb::matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using allocator_type = Allocator;
  using index_allocator_type = typename std::allocator_traits<allocator_type>::rebind_alloc<index_type>;

  using iterator = csr_matrix_iterator<T,
                                       index_type>;

  using const_iterator = csr_matrix_iterator<std::add_const_t<T>,
                                             index_type>;

/*
  using reference = csr_matrix_reference<T,
                                         index_type>;
  using const_reference = csr_matrix_reference<std::add_const_t<T>,
                                               std::add_const_t<index_type>>;
                                               */

  using reference = grb::matrix_ref<T, index_type>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, index_type>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  const_iterator begin() const noexcept {
    return const_iterator(0, 0, values_, rowptr_, colind_);
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

  csr_matrix(grb::index<I> shape);

  csr_matrix() = default;
  ~csr_matrix() = default;
  csr_matrix(const csr_matrix&) = default;
  csr_matrix(csr_matrix&&) = default;
  csr_matrix& operator=(const csr_matrix&) = default;
  csr_matrix& operator=(csr_matrix&&) = default;

private:

  template <typename InputIt>
  void assign_tuples(InputIt first, InputIt last);

  index_type m_ = 0;
  index_type n_ = 0;
  size_type nnz_ = 0;

  std::vector<index_type, index_allocator_type> rowptr_;
  std::vector<index_type, index_allocator_type> colind_;
  std::vector<T, allocator_type> values_;
};

template <typename T,
          typename I,
          typename Allocator>
csr_matrix<T, I, Allocator>::csr_matrix(index<I> shape)
 : m_(shape[0]), n_(shape[1]), nnz_(0) {
  if (shape[0]+1 > std::numeric_limits<I>::max() ||
      shape[1]+1 > std::numeric_limits<I>::max()) {
    throw std::runtime_error("grb::matrix({"                    +
                             std::to_string(shape[0])           +
                             std::to_string(shape[1])           +
                             "}: dimensions too big for "       +
                             std::to_string(sizeof(I)*CHAR_BIT) +
                             "-bit integer");
  }
  rowptr_.resize(shape[0]+1, 0);
}

// Refill the contents of the matrix so that it only contains the tuples stored
// in `tuples`.  Tuples must be within the bounds of the current dimensions of
// the matrix.
template <typename T,
          typename I,
          typename Allocator>
template <typename InputIt>
void
csr_matrix<T, I, Allocator>::assign_tuples(InputIt first, InputIt last)
{
  nnz_ = last - first;
  rowptr_.resize(shape()[0]+1);
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
      if (r+1 > m_) {
        // TODO: exception?
        // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
      }
      rowptr_[r+1] = c;
      r++;
    }
    c++;

    if (c > nnz_) {
      // TODO: exception?
      // throw std::runtime_error("csr_matrix_impl_: given invalid matrix");
    }
  }

  for ( ; r < m_; r++) {
    rowptr_[r+1] = nnz_;
  }
}


template <typename T,
          typename I,
          typename Allocator>
template <typename InputIt>
void csr_matrix<T, I, Allocator>::insert(InputIt first, InputIt last) {
  // a < b
  /*
  auto sort_fn = [](auto a, auto b) {
                   if (std::get<0>(a) < std::get<0>(b)) {
                     return true;
                   }
                   else if (std::get<0>(a) == std::get<0>(b)) {
                     if (std::get<1>(a) < std::get<1>(b)) {
                      return true;
                     }
                   }
                   return false;
                 };

  std::vector<std::tuple<index_type, index_type, value_type>> merged_tuples(nnz() + (last - first));
  std::set_union(begin(), end(), first, last, merged_tuples.begin(), sort_fn);
  */

  assign_tuples(first, last);
}

} // end grb

/*
#include <grb/containers/backend/csr_matrix_iterator.hpp>
#include <grb/containers/backend/csr_matrix_reference.hpp>

namespace grb {

template <typename T,
          typename I,
          typename Allocator>
struct csr_matrix_impl_ {
  using value_type = T;
  using index_type = I;

  using allocator_type = Allocator;
  using index_allocator_type = typename std::allocator_traits<allocator_type>::rebind_alloc<index_type>;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = csr_matrix_impl_iterator<csr_matrix_impl_>;
  using reference = csr_matrix_impl_reference<csr_matrix_impl_>;

  using tuples_type = std::vector<std::tuple<value_type, index_type, index_type>>;

  csr_matrix_impl_() : m_(0), n_(0), nnz_(0) {}

  csr_matrix_impl_(index_t shape);

  csr_matrix_impl_(const import_matrix_type_<value_type, index_type>& matrix);

  void assign_tuples(const tuples_type& tuples);
  void assign_tuples(const std::vector<std::tuple<index_type, index_type, value_type>>& tuples);

  size_type m() const {
    return m_;
  }

  size_type n() const {
    return n_;
  }

  size_type nnz() const {
    return nnz_;
  }

  iterator begin() const {
    // TODO: I should perhaps not do this.
    return iterator(*const_cast<csr_matrix_impl_*>(this), 0, rowptr_[0]);
  }

  iterator end() const {
    // TODO: I should perhaps not do this.
    return iterator(*const_cast<csr_matrix_impl_*>(this), m_, rowptr_[m_]);
  }

  iterator find(index_t idx) const;

  void insert_tuples(tuples_type& tuples);

  template <typename InputIt>
  void insert(InputIt first, InputIt last);

  size_type m_, n_;
  size_type nnz_;

  std::vector<index_type, index_allocator_type> rowptr_;
  std::vector<index_type, index_allocator_type> colind_;
  std::vector<value_type, allocator_type> values_;

  friend iterator;
  friend reference;
};

} // end grb

#include <grb/containers/backend/csr_matrix_impl.hpp>

*/