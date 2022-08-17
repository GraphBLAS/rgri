
#pragma once

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
  complement_vector_view_accessor(const complement_vector_view_accessor&) noexcept = default;
  complement_vector_view_accessor& operator=(const complement_vector_view_accessor&) noexcept = default;

  complement_vector_view_accessor(const V& vector, index_type index) : vector_(&vector), index_(index) {
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

  value_type operator*() const noexcept {
    return value_type(index_, true);
  }

private:
  const V* vector_;
  index_type index_;
};

template <grb::VectorRange V>
using complement_view_iterator = grb::detail::iterator_adaptor<complement_vector_view_accessor<V>>;

template <grb::VectorRange V>
class complement_view {
public:
  using scalar_type = bool;
  using index_type = grb::vector_index_t<V>;
  using value_type = grb::vector_entry<scalar_type, index_type>;
  using size_type = typename container_traits<V>::size_type;
  using difference_type = typename container_traits<V>::difference_type;
  using key_type = typename container_traits<V>::key_type;

  using iterator = complement_view_iterator<V>;
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

    if (iter == vector_.end() && !static_cast<bool>(grb::get<1>(*iter))) {
      return iterator(vector_, key);
    }  else {
      return end();
    }
  }

private:
  const V& vector_;
};

} // end grb