
#pragma once

#include <grb/grb.hpp>
#include <grb/containers/backend/dense_vector.hpp>
#include <numeric>

namespace grb {

template <typename T,
          std::integral I = std::size_t,
          typename Hint = grb::dense,
          typename Allocator = std::allocator<T>>
class vector {
public:
	/// Type of scalar values stored in the matrix
	using scalar_type = T;
	/// Type used to store indices
	using index_type = I;

	using value_type = grb::vector_entry<T, I>;

	using key_type = I;
	using map_type = T;

	using size_type = std::size_t;
	using difference_Type = std::ptrdiff_t;

  /// Allocator type
	using allocator_type = Allocator;

  // TODO: implement sparse, allow selecting sparse.
	using backend_type = grb::dense_vector<T, I, Allocator>;

	using iterator = typename backend_type::iterator;
	using const_iterator = typename backend_type::const_iterator;

	using reference = typename backend_type::reference;
	using const_reference = typename backend_type::const_reference;

	using pointer = iterator;
	using const_pointer = const_iterator;

	using scalar_reference = typename backend_type::scalar_reference;

	vector(I shape) : backend_(shape) {}
	vector(I shape, const Allocator& allocator) : backend_(shape, allocator) {}

  /// Shape of the vector
	I shape() const noexcept {
		return backend_.shape();
	}

  /// Number of values stored in the vector
	size_type size() const noexcept {
		return backend_.size();
	}

  /// Iterator to the beginning
	iterator begin() noexcept {
		return backend_.begin();
	}

  /// Iterator to the end
	iterator end() noexcept {
		return backend_.end();
	}

  /// Const iterator to the beginning
	const_iterator begin() const noexcept {
		return backend_.begin();
	}

  /// Const iterator to the end
	const_iterator end() const noexcept {
		return backend_.end();
	}

	iterator find(key_type index) noexcept {
		return backend_.find(index);
	}

	const_iterator find(key_type index) const noexcept {
		return backend_.find(index);
	}

	scalar_reference operator[](map_type index) {
		return backend_[index];
	}

	template <typename InputIt>
	void insert(InputIt first, InputIt last) {
		for (auto iter = first; iter != last; ++iter) {
			insert(*iter);
		}
	}

	std::pair<iterator, bool> insert(const value_type& value) {
		return backend_.insert(value);
	}

	template <typename M>
	std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
		return backend_.insert_or_assign(k, std::forward<M>(obj));
	}

  void clear() {
    vector other(shape());
    *this = std::move(other);
  }

	void reshape(I shape) {
		backend_.reshape(shape);
	}

	vector() = default;
	vector(const Allocator& allocator) : backend_(allocator) {}
	
	~vector() = default;
	vector(const vector&) = default;
	vector& operator=(const vector&) = default;
	vector(vector&&) = default;
	vector& operator=(vector&&) = default;

private:
	backend_type backend_;
};

} // end grb