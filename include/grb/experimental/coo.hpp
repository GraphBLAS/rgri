#include <vector>
#include <tuple>

template <typename T, typename I>
using spmat_base_type_ = std::vector<std::tuple<T, I, I>>;

template <typename T, typename I>
struct grb_coo_impl_ {
	using value_type = T;
	using index_type = I;
	using size_type = I;
	using difference_type = std::ptrdiff_t;

	std::vector<std::tuple<value_type, index_type, index_type>> values_;
};