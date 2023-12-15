
#pragma once

#include <grb/containers/backend/coo_matrix.hpp>
#include <grb/containers/backend/csr_matrix.hpp>
#include <grb/containers/backend/dia_matrix.hpp>

#include <grb/containers/matrix_entry.hpp>
#include <grb/util/matrix_hints.hpp>

namespace grb {

/// A GraphBLAS matrix object.
/// 1. Template parameter `T` specifies the type of values stored in the matrix.
/// 2. `I` is an integer type used to record the indices of stored elements.
/// 3. `Hint` is a hint as to what backend data structure should be used to
///    store the matrix elements, and can be either `grb::sparse` or
///    `grb::dense`.
/// 4. `Allocator` is the C++ allocator used to allocate memory.
template <typename T, std::integral I = std::size_t,
          typename Hint = grb::sparse, typename Allocator = std::allocator<T>>
class matrix {
public:
  /// Type of scalar elements stored in the matrix.
  using scalar_type = T;

  /// Type used to reference the indices of elements stored in the matrix.
  using index_type = I;

  /// A tuple-like type containing:
  /// 1. A tuple-like type with two `index_type` elements storing the row and
  /// column
  ///    index of the element.
  /// 2. An element of type `scalar_type`.
  using value_type = matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  /// Allocator type
  using allocator_type = Allocator;

  /// A large unsigned integral type
  using size_type = std::size_t;

  /// A large signed integral type
  using difference_type = std::ptrdiff_t;

  using hint_type = Hint;

  using backend_type =
      typename pick_backend_type<Hint>::template type<T, I, Allocator>;

  using iterator = typename backend_type::iterator;
  using const_iterator = typename backend_type::const_iterator;

  using reference = typename backend_type::reference;
  using const_reference = typename backend_type::const_reference;

  using scalar_reference = typename backend_type::scalar_reference;

  /// Construct an empty matrix of dimension `shape[0]` x `shape[1]`
  matrix(grb::index<I> shape) : backend_(shape) {}

  matrix(grb::index<I> shape, const Allocator& allocator)
      : backend_(shape, allocator) {}

  matrix(std::initializer_list<I> shape)
      : backend_({*shape.begin(), *(shape.begin() + 1)}) {}

  matrix(std::initializer_list<I> shape, const Allocator& allocator)
      : backend_({*shape.begin(), *(shape.begin() + 1)}, allocator) {}

  /// Construct a matrix from the Matrix Market file stored at location
  /// `file_path`.
  matrix(std::string file_path) {
    auto tuples = grb::mmread<T, I>(file_path);
    reshape(tuples.shape());
    insert(tuples.begin(), tuples.end());
  }

  matrix(std::string file_path, const Allocator& allocator)
      : backend_(allocator) {
    auto tuples = grb::mmread<T, I>(file_path);
    reshape(tuples.shape());
    insert(tuples.begin(), tuples.end());
  }

  /// Dimensions of the matrix
  grb::index<I> shape() const noexcept {
    return backend_.shape();
  }

  /// Number of stored values in the matrix
  size_type size() const noexcept {
    return backend_.size();
  }

  /// Whether the matrix is empty
  bool empty() const noexcept {
    return size() == 0;
  }

  /// Iterator to the beginning
  iterator begin() noexcept {
    return backend_.begin();
  }

  /// Const iterator to the beginning
  const_iterator begin() const noexcept {
    return backend_.begin();
  }

  /// Iterator to the end
  iterator end() noexcept {
    return backend_.end();
  }

  /// Const iterator to the end
  const_iterator end() const noexcept {
    return backend_.end();
  }

  /// Insert elements in the range [first, last).
  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    backend_.insert(first, last);
  }

  void clear() {
    matrix other(shape());
    *this = std::move(other);
  }

  /// Insert the matrix_entry `value` into the matrix.
  /// If no element is already present at the index, insert
  /// the element and return a pair containing an iterator to
  /// the newly inserted element and the boolean value `true`.
  /// If an element already exists at the index, return a pair
  /// containing an iterator to the element that prevented
  /// inserting, along with the boolean value `false`.
  std::pair<iterator, bool> insert(const value_type& value) {
    return backend_.insert(std::move(value));
  }

  template <class M>
  std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
    return backend_.insert_or_assign(k, std::forward<M>(obj));
  }

  iterator find(key_type key) noexcept {
    return backend_.find(key);
  }

  const_iterator find(key_type key) const noexcept {
    return backend_.find(key);
  }

  /// Reshape the matrix dimensions to be `shape[0]` x `shape[1]`.
  /// Any elements outside the new shape will be deleted.
  void reshape(grb::index<I> shape) {
    return backend_.reshape(shape);
  }

  scalar_reference operator[](grb::index<I> index) {
    auto [iterator, inserted] = insert({index, T()});
    auto&& [i_, value] = *iterator;
    return value;
  }

  matrix() = default;
  matrix(const Allocator& allocator) : backend_(allocator) {}

  matrix(const matrix&) = default;
  matrix(matrix&&) = default;
  matrix& operator=(const matrix&) = default;
  matrix& operator=(matrix&&) = default;

private:
  backend_type backend_;
};

} // namespace grb
