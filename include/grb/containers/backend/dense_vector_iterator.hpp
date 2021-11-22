#pragma once

#include <grb/containers/backend/dense_vector.hpp>

namespace grb {

template <typename T,
          typename I = std::size_t,
          typename Allocator = std::allocator<T>>
class dense_vector;

template <typename T,
          typename I,
          typename Allocator>
class dense_vector_iterator {
private:

  template <typename U>
  struct const_if_impl_ {
    template <typename V>
    struct constify {
      using type = std::add_const_t<V>;
    };
  };

  template <typename U>
  requires(!std::is_const_v<U>)
  struct const_if_impl_<U> {
    template <typename V>
    struct constify {
      using type = V;
    };
  };

  template <typename U, typename V>
  using const_if_t = typename const_if_impl_<U>:: template constify<V>::type;

public:
  using backend_type = dense_vector<std::remove_const_t<T>, I, Allocator>;
  using backend_type_constified = const_if_t<T, backend_type>;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using scalar_type = T;
  using index_type = I;

  using key_type = grb::index<I>;
  using map_type = T;

  using value_type = grb::vector_entry<T, index_type>;
  using iterator = dense_vector_iterator;
  using const_iterator = dense_vector_iterator<std::add_const_t<T>,
                                               index_type, Allocator>;

  using reference = grb::vector_ref<T, I>;
  using const_reference = grb::vector_ref<std::add_const_t<T>, I>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  using iterator_category = std::forward_iterator_tag;

  dense_vector_iterator(backend_type_constified& vector, index_type index)
    : vector_(&vector), index_(index) {
    	fast_forward();
  }

  // reference operator*() const noexcept
  reference operator*() const noexcept
  requires(!std::is_const_v<T>)
  {
    return reference(index_, vector_->data_[index_]);
  }

  // const_reference operator*() const noexcept
  const_reference operator*() const noexcept
  requires(std::is_const_v<T>)
  {
    return const_reference(index_, vector_->data_[index_]);
  }

  void fast_forward() noexcept {
    while (!vector_->flags_[index_] && index_ < vector_->flags_.size()) {
    	index_++;
    }
  }

  void increment() noexcept {
    index_++;
    fast_forward();
  }

  dense_vector_iterator& operator++() noexcept {
    increment();
    return *this;
  }

  dense_vector_iterator operator++(int) noexcept {
    dense_vector_iterator other = *this;
    increment();
    return other;
  }

  bool operator<(dense_vector_iterator other) const noexcept {
    return index_ < other.index_;
  }

  bool operator<=(dense_vector_iterator other) const noexcept {
    return index_ <= other.index_;
  }

  bool operator>(dense_vector_iterator other) const noexcept {
    return index_ > other.index_;
  }

  bool operator>=(dense_vector_iterator other) const noexcept {
    return index_ >= other.index_;
  }


  bool operator==(const dense_vector_iterator& other) const noexcept = default;
  bool operator!=(const dense_vector_iterator& other) const noexcept  = default;

  dense_vector_iterator() = default;
  ~dense_vector_iterator() = default;
  dense_vector_iterator(const dense_vector_iterator&) = default;
  dense_vector_iterator(dense_vector_iterator&&) = default;
  dense_vector_iterator& operator=(const dense_vector_iterator&) = default;
  dense_vector_iterator& operator=(dense_vector_iterator&&) = default;

private:


  backend_type_constified* vector_;
  index_type index_;
};

} // end grb
