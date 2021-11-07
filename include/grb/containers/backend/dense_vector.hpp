#pragma once

#include <vector>
#include <grb/containers/vector_entry.hpp>
#include <grb/containers/backend/dense_vector_iterator.hpp>

namespace grb {

template <typename T,
          typename I,
          typename Allocator>
struct dense_vector {
public:
	using index_type = I;
	using value_type = grb::vector_entry<T, I>;

	using key_type = I;
	using map_type = T;

	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using allocator_type = Allocator;

  using iterator = dense_vector_iterator<T, index_type, allocator_type>;

  using const_iterator = dense_vector_iterator<std::add_const_t<T>, index_type, allocator_type>;

  using reference = grb::vector_ref<T, index_type>;
  using const_reference = grb::vector_ref<std::add_const_t<T>, index_type>;

  using pointer = iterator;
  using const_pointer = const_iterator;

	using v_reference = typename std::vector<T, allocator_type>::reference;

	dense_vector() = default;

	dense_vector(size_type shape) {
		data_.resize(shape);
		flags_.resize(shape, false);
	}

	size_type shape() const noexcept {
		return data_.size();
	}

	size_type size() const noexcept {
		return nnz_;
	}

	v_reference operator[](I index) noexcept {
		if (!flags_[index]) {
			data_[index] = T();
			flags_[index] = true;
		}
		return data_[index];
	}

	iterator begin() noexcept {
		return iterator(*this, 0);
	}

	iterator end() noexcept {
		return iterator(*this, shape());
	}

private:
  using bool_allocator_type = typename std::allocator_traits<allocator_type>::rebind_alloc<bool>;
  friend iterator;
  friend const_iterator;

	std::vector<T, allocator_type> data_;
	std::vector<bool, bool_allocator_type> flags_;
	size_t nnz_ = 0;
};

} // end grb