
#pragma once

#include <utility>

#include "transform_matrix_view_iterator.hpp"
#include "transform_matrix_view_reference.hpp"

namespace grb {

template <typename MatrixType,
          typename ValueTransform,
          typename IndexTransform,
          typename ShapeTransform>
class transform_matrix_view {
public:
  using backend_value_type = typename MatrixType::value_type;

  // value_type is the return type of ValueTransform
  using value_type = decltype(std::declval<ValueTransform>()(std::declval<backend_value_type>()));

  // NOTE: index_type does not change

  using value_transform_type = ValueTransform;
  using index_transform_type = IndexTransform;
  using shape_transform_type = ShapeTransform;

  using index_type = typename MatrixType::index_type;
  using allocator_type = typename MatrixType::allocator_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;

  using backend_iterator = typename MatrixType::iterator;
  using backend_const_iterator = typename MatrixType::const_iterator;

  using iterator = transform_matrix_view_iterator<backend_iterator, value_transform_type, index_transform_type>;
  using const_iterator = const_transform_matrix_view_iterator<backend_const_iterator, value_transform_type, index_transform_type>;

  using backend_reference = typename MatrixType::reference;
  using backend_const_reference = typename MatrixType::const_reference;

  using reference = transform_matrix_view_reference<backend_reference, value_transform_type, index_transform_type>;
  using const_reference = const_transform_matrix_view_reference<backend_const_reference, value_transform_type, index_transform_type>;

  // TODO: are these the correct types for value, index transforms?
  transform_matrix_view(MatrixType& matrix,
                        const ValueTransform& value_transform,
                        const IndexTransform& index_transform,
                        const ShapeTransform& shape_transform)
    : matrix_(matrix), value_transform_(value_transform),
      index_transform_(index_transform), shape_transform_(shape_transform) {}

  index_t shape() const noexcept {
  	return shape_transform_({matrix_.shape()[1], matrix_.shape()[0]});
  }

  size_type size() const noexcept {
  	return matrix_.size();
  }

  iterator begin() {
    return iterator(matrix_.begin(), value_transform_, index_transform_);
  }

  iterator end() {
    return iterator(matrix_.end(), value_transform_, index_transform_);
  }

  const_iterator begin() const {
    return const_iterator(matrix_.begin(), value_transform_, index_transform_);
  }

  const_iterator end() const {
    return const_iterator(matrix_.end(), value_transform_, index_transform_);
  }


private:
	MatrixType& matrix_;
  const ValueTransform& value_transform_;
  const IndexTransform& index_transform_;
  const ShapeTransform& shape_transform_;
};

} // end grb