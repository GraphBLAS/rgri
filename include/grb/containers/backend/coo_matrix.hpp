#pragma once

namespace grb {

template <typename T,
          typename I>
class coo_matrix {
public:
  using value_type = grb::matrix_entry<T, I>;
  using index_type = I;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

	using key_type = grb::index<I>;
	using map_type = T;

  using backend_type = std::vector<value_type>;

  using iterator = typename backend_type::iterator;
  using const_iterator = typename backend_type::const_iterator;

  using reference = grb::matrix_ref<T, I>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, I>;

  coo_matrix(grb::index<I> shape) : shape_(shape) {}

  grb::index<I> shape() const noexcept {
    return shape_;
  }

  size_type size() const noexcept {
  	return tuples_.size();
  }

  iterator begin() noexcept {
    return tuples_.begin();
  }

  const_iterator begin() const noexcept {
    return tuples_.begin();
  }

  iterator end() noexcept {
    return tuples_.end();
  }

  const_iterator end() const noexcept {
    return tuples_.end();
  }

  std::pair<iterator, bool>
  insert(value_type&& value) {
  	auto&& [insert_index, insert_value] = value;
  	for (auto iter = begin(); iter != end(); ++iter) {
  		auto&& [index, v] = *iter;
  		if (index == insert_index) {
  			return {iter, false};
  		}
  	}
  	tuples_.push_back(value);
  	return {--tuples_.end(), true};
  }

  template <class M>
  std::pair<iterator, bool>
  insert_or_assign(key_type k, M&& obj) {
  	for (auto iter = begin(); iter != end(); ++iter) {
  		auto&& [index, v] = *iter;
  		if (index == k) {
  			v = std::forward<M>(obj);
  			return {iter, false};
  		}
  	}
  	tuples_.push_back({k, std::forward<M>(obj)});
  	return {--tuples_.end(), true};
  }

  iterator find(key_type key) noexcept {
  	return std::find_if(begin(), end(), [&](auto&& v) {
  		                                   auto&& [i, v_] = v;
  		                                   return i == key;
  		                                  });
  }

  const_iterator find(key_type key) const noexcept {
  	return std::find_if(begin(), end(), [&](auto&& v) {
  		                                    auto&& [i, v_] = v;
  		                                    return i == key;
  		                                  });
  }

  coo_matrix() = default;
  ~coo_matrix() = default;
  coo_matrix(const coo_matrix&) = default;
  coo_matrix(coo_matrix&&) = default;
  coo_matrix& operator=(const coo_matrix&) = default;
  coo_matrix& operator=(coo_matrix&&) = default;

private:
  grb::index<I> shape_;
  backend_type tuples_;
};

} // end grb