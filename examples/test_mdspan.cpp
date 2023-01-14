#include <grb/grb.hpp>
#include <experimental/mdspan>
#include <grb/detail/iterator_adaptor.hpp>

namespace grb {

template <typename MDSpanType>
class mdspan_matrix_accessor {
public:
  using mdspan_type = std::remove_cvref_t<MDSpanType>;

  using scalar_type = typename mdspan_type::value_type;
  using scalar_reference = typename mdspan_type::reference;

  using index_type = typename mdspan_type::index_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  using difference_type = std::ptrdiff_t;
  using iterator_accessor = mdspan_matrix_accessor;
  using const_iterator_accessor = mdspan_matrix_accessor;
  using nonconst_iterator_accessor = mdspan_matrix_accessor;
  using iterator_category = std::random_access_iterator_tag;

  constexpr mdspan_matrix_accessor() noexcept = default;
  constexpr ~mdspan_matrix_accessor() noexcept = default;
  constexpr mdspan_matrix_accessor(const mdspan_matrix_accessor&) noexcept = default;
  constexpr mdspan_matrix_accessor& operator=(const mdspan_matrix_accessor&) noexcept = default;

  constexpr mdspan_matrix_accessor(mdspan_type mdspan, grb::index<index_type> index) noexcept
    : mdspan_(mdspan), i_(index[0]), j_(index[1]) { }

  constexpr mdspan_matrix_accessor& operator+=(difference_type offset) noexcept {
    auto global_idx = i_*mdspan_.extent(1) + j_;
    auto new_idx = global_idx + offset;
    i_ = new_idx / mdspan_.extent(1);
    j_ = new_idx % mdspan_.extent(1);
    return *this;
  }

  constexpr mdspan_matrix_accessor& operator++() noexcept {
    j_++;
    if (j_ >= mdspan_.extent(1)) {
      i_++;
      j_ = 0;
    }
    return *this;
  }

  constexpr mdspan_matrix_accessor& operator--() noexcept {
    if (j_ > 0) {
      j_--;
    } else {
      i_--;
      j_ = mdspan_.extent(1) - 1;
    }
    return *this;
  }

  constexpr difference_type operator-(const const_iterator_accessor& other) const noexcept {
    difference_type row_diff = difference_type(i_) - difference_type(other.i_);
    difference_type col_diff = difference_type(j_) - difference_type(other.j_);

    return row_diff*mdspan_.extent(1) + col_diff;
  }

  constexpr bool operator==(const const_iterator_accessor& other) const noexcept {
    return i_ == other.i_ && j_ == other.j_;
  }

  constexpr bool operator<(const const_iterator_accessor& other) const noexcept {
    if (i_ < other.i_) {
      return true;
    } else if (i_ == other.i_) {
      return j_ < other.j_;
    } else {
      return false;
    }
  }

  constexpr reference operator*() const noexcept {
    return reference({i_, j_}, mdspan_(i_, j_));
  }

private:
  index_type i_;
  index_type j_;
  mdspan_type mdspan_;
};

template <typename MDSpanType>
using mdspan_matrix_iterator = grb::detail::iterator_adaptor<mdspan_matrix_accessor<MDSpanType>>;

template <grb::MatrixRange M>
class matrix_view {
public:

  using index_type = grb::matrix_index_t<M>;
  using key_type = typename grb::container_traits<M>::key_type;
  using value_type = std::ranges::range_value_t<M>;
  using scalar_type = grb::matrix_scalar_t<M>;
  using scalar_reference = decltype(grb::get<1>(std::declval<std::ranges::range_reference_t<M>>()));

  matrix_view(M matrix) : matrix_(matrix) {}

  auto begin() {
    return std::ranges::begin(base());
  }

  auto end() {
    return std::ranges::end(base());
  }

  auto find(const key_type& key) {
    return grb::find(base(), key);
  }

  auto insert(const value_type& entry) {
    return grb::insert(base(), entry);
  }

  auto size() {
    return std::ranges::size(base());
  }

  auto shape() {
    return grb::shape(base());
  }

  template <typename O>
  auto insert_or_assign(const key_type& key, O&& obj) {
    return grb::insert_or_assign(key, std::forward<O>(obj));
  }

  scalar_reference operator[](const key_type& key) {
    auto&& [iter, inserted] = insert(value_type(key, scalar_type{}));
    return grb::get<1>(*iter);
  }

  decltype(auto) base() noexcept {
    if constexpr(std::ranges::view<std::decay_t<M>>) {
      return matrix_;
    } else {
      return matrix_.base();
    }
  }

private:
  std::ranges::views::all_t<M> matrix_;
};

template <typename M>
matrix_view(M&& m) -> matrix_view<M>;

template <typename... Args>
class mdspan_matrix_view {
public:
  using mdspan_type = std::experimental::mdspan<Args...>;

  using scalar_type = typename mdspan_type::value_type;
  using scalar_reference = typename mdspan_type::reference;

  using index_type = typename mdspan_type::index_type;

  using value_type = grb::matrix_entry<scalar_type, index_type>;
  using reference = grb::matrix_ref<scalar_type, index_type, scalar_reference>;

  using key_type = grb::index<index_type>;

  using difference_type = std::ptrdiff_t;

  using iterator = grb::mdspan_matrix_iterator<mdspan_type>;

  mdspan_matrix_view(mdspan_type mdspan) : mdspan_(mdspan) {}

  auto begin() {
    return iterator(mdspan_, key_type{0, 0});
  }

  auto end() {
    return iterator(mdspan_, key_type{mdspan_.extent(0), 0});
  }

  auto find(const key_type& key) {
    return iterator(mdspan_, key);
  }

  auto insert(const value_type& entry) {
    return grb::insert(mdspan_, entry);
  }

  auto shape() {
    return grb::shape(mdspan_);
  }

  auto size() {
    return mdspan_.size();
  }

  template <typename O>
  auto insert_or_assign(const key_type& key, O&& obj) {
    return grb::insert_or_assign(key, std::forward<O>(obj));
  }

  scalar_reference operator[](const key_type& key) {
    auto&& [iter, inserted] = insert(value_type(key, scalar_type{}));
    return grb::get<1>(*iter);
  }

  decltype(auto) base() noexcept {
    return mdspan_;
  }

private:
  mdspan_type mdspan_;
};

template <typename... Args>
mdspan_matrix_view(std::experimental::mdspan<Args...>) -> mdspan_matrix_view<Args...>;

namespace views {

inline constexpr struct all_fn_ {
  template <typename O>
  auto operator()(O&& x) const
  requires(grb::is_tag_invocable_v<all_fn_, O> ||
           grb::MatrixRange<O>)
  {
    if constexpr(grb::is_tag_invocable_v<all_fn_, O>) {
      return grb::tag_invoke(*this, std::forward<O>(x));
    } else if constexpr(grb::MatrixRange<O>) {
      return grb::matrix_view(std::forward<O>(x));
    }
  }
} all{};

} // end views

} // end grb

namespace std {

namespace experimental {

template <typename... Args>
auto tag_invoke(grb::tag_t<grb::views::all>, std::experimental::mdspan<Args...> m)
requires(m.rank() == 2)
{
  return grb::mdspan_matrix_view(m);
}

template <typename... Args>
auto tag_invoke(grb::tag_t<grb::size>, std::experimental::mdspan<Args...> m)
{
  return m.size();
}

template <typename... Args>
auto tag_invoke(grb::tag_t<grb::shape>, std::experimental::mdspan<Args...> m)
requires(m.rank() == 2)
{

  using index_type = decltype(m)::index_type;
  return grb::index<index_type>(m.extent(0), m.extent(1));
}

template <typename... Args>
auto tag_invoke(grb::tag_t<grb::insert_or_assign>, std::experimental::mdspan<Args...> m)
requires(m.rank() == 2)
{

  using index_type = decltype(m)::index_type;
  return grb::index<index_type>(m.extent(0), m.extent(1));
}

} // end experimental


} // end std

int main(int argc, char** argv) {
  grb::matrix<float, int> a("data/chesapeake.mtx");

  grb::matrix<float, std::size_t, grb::dense> b("data/chesapeake.mtx");

  std::vector<float> data(b.shape()[0]*b.shape()[1], 0);

  std::experimental::mdspan b_span(data.data(), b.shape()[0], b.shape()[1]);

  for (auto&& [index, value] : b) {
    auto&& [i, j] = index;
    b_span(i, j) = value;
  }

  for (size_t i = 0; i < b.shape()[0]; i++) {
    for (size_t j = 0; j < b.shape()[1]; j++) {
      std::cout << b_span(i, j) << ", ";
    }
    std::cout << std::endl;
  }

  auto c = grb::multiply(a, b);

  auto shape = grb::shape(b_span);

  std::cout << shape[0] << ", " << shape[1] << std::endl;

  auto v = grb::views::all(b_span);

  grb::print(v, "v");

  return 0;
}
