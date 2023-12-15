
#pragma once

#include <grb/detail/iterator_adaptor.hpp>

namespace grb {

/*
   TODO: there are a ton of optimizations that could be applied here.
*/

template <grb::VectorRange V>
class complement_vector_view_accessor {
public:
  using scalar_type = bool;
  using index_type = grb::vector_index_t<V>;
  using difference_type = typename container_traits<V>::difference_type;

  using value_type = grb::vector_entry<scalar_type, index_type>;
  using reference = value_type;

  using iterator_accessor = complement_vector_view_accessor;
  using const_iterator_accessor = complement_vector_view_accessor;
  using nonconst_iterator_accessor = complement_vector_view_accessor;

  using iterator_category = std::forward_iterator_tag;

  complement_vector_view_accessor() noexcept = default;
  ~complement_vector_view_accessor() noexcept = default;
  complement_vector_view_accessor(
      const complement_vector_view_accessor&) noexcept = default;
  complement_vector_view_accessor&
  operator=(const complement_vector_view_accessor&) noexcept = default;

  complement_vector_view_accessor(const V& vector, index_type index)
      : vector_(&vector), index_(index) {
    fast_forward();
  }

  // Return whether the current index is present in the complement view.
  bool index_present() const {
    auto iter = vector_->find(index_);
    if (iter == vector_->end()) {
      return true;
    } else if (!static_cast<bool>(grb::get<1>(*iter))) {
      return true;
    } else {
      return false;
    }
  }

  void fast_forward() {
    while (index_ < vector_->shape() && !index_present()) {
      ++index_;
    }
  }

  complement_vector_view_accessor& operator++() noexcept {
    ++index_;
    fast_forward();
    return *this;
  }

  bool operator==(const complement_vector_view_accessor& other) const noexcept {
    return vector_ == other.vector_ && index_ == other.index_;
  }

  bool operator!=(const complement_vector_view_accessor& other) const noexcept {
    return !(*this == other);
  }

  value_type operator*() const noexcept {
    return value_type(index_, true);
  }

private:
  const V* vector_;
  index_type index_;
};

template <grb::VectorRange V>
using complement_vector_view_iterator =
    grb::detail::iterator_adaptor<complement_vector_view_accessor<V>>;

template <typename V>
class complement_view;

template <grb::VectorRange V>
class complement_view<V> {
public:
  using scalar_type = bool;
  using index_type = grb::vector_index_t<V>;
  using value_type = grb::vector_entry<scalar_type, index_type>;
  using size_type = typename container_traits<V>::size_type;
  using difference_type = typename container_traits<V>::difference_type;
  using key_type = typename container_traits<V>::key_type;

  using iterator = complement_vector_view_iterator<V>;
  using reference = value_type;

  complement_view(const V& vector) : vector_(vector) {}

  std::size_t size() const {
    std::size_t vec_size = shape() - vector_.size();
    for (auto&& [_, value] : vector_) {
      if (!static_cast<bool>(value)) {
        vec_size++;
      }
    }
    return vec_size;
  }

  iterator begin() const {
    return iterator(vector_, 0);
  }

  iterator end() const {
    return iterator(vector_, shape());
  }

  auto shape() const {
    return vector_.shape();
  }

  iterator find(key_type key) const {
    auto iter = vector_.find(key);

    if (iter == vector_.end() || !static_cast<bool>(grb::get<1>(*iter))) {
      return iterator(vector_, key);
    } else {
      return end();
    }
  }

private:
  const V& vector_;
};

template <grb::MatrixRange M>
class complement_matrix_view_accessor {
public:
  using scalar_type = bool;
  using index_type = grb::matrix_index_t<M>;
  using key_type = typename container_traits<M>::key_type;
  using difference_type = typename container_traits<M>::difference_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = value_type;

  using iterator_accessor = complement_matrix_view_accessor;
  using const_iterator_accessor = complement_matrix_view_accessor;
  using nonconst_iterator_accessor = complement_matrix_view_accessor;

  using iterator_category = std::forward_iterator_tag;

  complement_matrix_view_accessor() noexcept = default;
  ~complement_matrix_view_accessor() noexcept = default;
  complement_matrix_view_accessor(
      const complement_matrix_view_accessor&) noexcept = default;
  complement_matrix_view_accessor&
  operator=(const complement_matrix_view_accessor&) noexcept = default;

  complement_matrix_view_accessor(const M& matrix, index_type i, index_type j)
      : matrix_(&matrix), i_(i), j_(j) {
    fast_forward();
  }

  // Return whether the current index is present in the complement view.
  bool index_present() const {
    auto iter = matrix_->find(index());
    if (iter == matrix_->end()) {
      return true;
    } else if (!static_cast<bool>(grb::get<1>(*iter))) {
      return true;
    } else {
      return false;
    }
  }

  void fast_forward() {
    while (linearized(index()) < linearized(matrix_->shape()) &&
           !index_present()) {
      increment();
    }
  }

  complement_matrix_view_accessor& operator++() noexcept {
    increment();
    fast_forward();
    return *this;
  }

  bool operator==(const complement_matrix_view_accessor& other) const noexcept {
    return matrix_ == other.matrix_ && index() == other.index();
  }

  bool operator!=(const complement_matrix_view_accessor& other) const noexcept {
    return !(*this == other);
  }

  value_type operator*() const noexcept {
    return value_type(index(), true);
  }

private:
  void increment() noexcept {
    if (j_ > matrix_->shape()[1] - 1) {
      i_++;
      j_ = 0;
    } else {
      j_++;
    }
  }

  auto index() const noexcept {
    return key_type(i_, j_);
  }

  auto linearized(key_type idx) const noexcept {
    return idx[0] * idx[1];
  }

private:
  const M* matrix_;
  index_type i_, j_;
};

template <grb::MatrixRange M>
using complement_matrix_view_iterator =
    grb::detail::iterator_adaptor<complement_matrix_view_accessor<M>>;

template <grb::MatrixRange M>
class complement_view<M> {
public:
  using scalar_type = bool;
  using index_type = grb::matrix_index_t<M>;
  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using size_type = typename container_traits<M>::size_type;
  using difference_type = typename container_traits<M>::difference_type;
  using key_type = typename container_traits<M>::key_type;

  using iterator = complement_matrix_view_iterator<M>;
  using reference = value_type;

  complement_view(const M& matrix) : matrix_(matrix) {}

  std::size_t size() const {
    std::size_t matrix_size = shape()[0] * shape()[1] - matrix_.size();
    for (auto&& [_, value] : matrix_) {
      if (!static_cast<bool>(value)) {
        matrix_size++;
      }
    }
    return matrix_size;
  }

  iterator begin() const {
    return iterator(matrix_, 0, 0);
  }

  iterator end() const {
    return iterator(matrix_, shape()[0], shape()[1]);
  }

  auto shape() const {
    return matrix_.shape();
  }

  iterator find(key_type key) const {
    auto iter = matrix_.find(key);

    if (iter == matrix_.end() || !static_cast<bool>(grb::get<1>(*iter))) {
      return iterator(matrix_, key[0], key[1]);
    } else {
      return end();
    }
  }

private:
  const M& matrix_;
};

template <grb::MatrixRange M>
complement_view(M&& m) -> complement_view<std::remove_cvref_t<M>>;

template <grb::VectorRange V>
complement_view(V&& v) -> complement_view<std::remove_cvref_t<V>>;

} // namespace grb
