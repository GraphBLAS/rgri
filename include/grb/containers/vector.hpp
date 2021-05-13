
#pragma once

#include <grb/grb.hpp>
#include <vector>

namespace grb {

template <typename T,
          typename I = std::size_t,
          typename Hint = grb::dense,
          typename Allocator = std::allocator<T>>
class vector {
public:
	using value_type = T;
	using index_type = I;
	using allocator_type = Allocator;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using hint_type = Hint;

	using backend_type = grb::matrix<value_type, index_type, hint_type, allocator_type>;

	using iterator = typename backend_type::iterator;
	using const_iterator = typename backend_type::const_iterator;

	using reference = typename backend_type::reference;
	using const_reference = typename backend_type::const_reference;

	vector() = default;
	vector(const vector&) = default;
	vector& operator=(const vector&) = default;

	vector(size_type dimension) : backend_matrix_({1, dimension}) {}

	size_type shape() const noexcept {
		return backend_matrix_.shape()[1];
	}

	size_type size() const noexcept {
		return backend_matrix_.size();
	}

	iterator begin() {
		return backend_matrix_.begin();
	}

	iterator end() {
		return backend_matrix_.end();
	}

	const_iterator begin() const {
		return backend_matrix_.begin();
	}

	const_iterator end() const {
		return backend_matrix_.end();
	}

	iterator find(size_type index) {
		return backend_matrix_.find({1, index});
	}

	const_iterator find(size_type index) const {
		return backend_matrix_.find({1, index});
	}

	reference operator[](size_type index) {
		return backend_matrix_[{1, index}];
	}

	reference at(size_type index) {
		return backend_matrix_.at({1, index});
	}

private:
	backend_type backend_matrix_;
};

} // end grb