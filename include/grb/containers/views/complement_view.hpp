
#pragma once

namespace grb {

/*
   TODO: there are a ton of optimizations that could be applied here.
*/

template <typename MatrixType>
class complement_matrix_view_reference {
public:
  using value_type = bool;
  using index_type = typename MatrixType::index_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;

  complement_matrix_view_reference(const MatrixType& matrix, size_t idx,
                                   const std::vector<bool>& complement)
    : matrix_(matrix), idx_(idx), complement_(complement) {}

  operator std::tuple<index_type, index_type, bool>() const noexcept {
    return {idx_ / matrix_.shape()[1], idx_ % matrix_.shape()[1], complement_[idx_]};
  }

  grb::index_t index() const noexcept {
    return {idx_ / matrix_.shape()[1], idx_ % matrix_.shape()[1]};
  }

  bool value() const noexcept {
    return complement_[idx_];
  }

private:
  const MatrixType& matrix_;
  size_t idx_;
  const std::vector<bool>& complement_;
};

template <typename MatrixType>
class complement_matrix_view_iterator {
public:
	using value_type = bool;
	using index_type = typename MatrixType::index_type;
	using size_type = typename MatrixType::size_type;
	using difference_type = typename MatrixType::difference_type;

  using reference = complement_matrix_view_reference<MatrixType>;

  complement_matrix_view_iterator(const MatrixType& matrix, size_type idx)
    : matrix_(matrix), idx_(idx) {
    get_compl_();
  }

  complement_matrix_view_iterator& operator++() {
    do {
      idx_++;
    } while (idx_ < matrix_.shape()[0]*matrix_.shape()[1]
             && static_cast<bool>(complement_[idx_]) == false);
  	return *this;
  }

  complement_matrix_view_iterator& operator--() {
    do {
      idx_--;
    } while (idx_ >= 0
             && static_cast<bool>(complement_[idx_]) == false);
    return *this;
  }

  bool operator==(const complement_matrix_view_iterator& other) const {
    if (&matrix_ == &other.matrix_ && idx_ == other.idx_) {
      return true;
    } else {
      return false;
    }
  }

  bool operator!=(const complement_matrix_view_iterator& other) const {
    return !(*this == other);
  }

  reference operator*() const noexcept {
    return complement_matrix_view_reference<MatrixType>(matrix_, idx_, complement_);
  }

  void get_compl_() {
  	if (complement_.size() > 0) {
      for (size_t i = 0; i < complement_.size(); i++) {
        complement_[i] = true;
      }
  	}
  	complement_.resize(matrix_.shape()[0]*matrix_.shape()[1], true);

  	for (auto&& [i, j, value] : matrix_) {
  		if (static_cast<bool>(value) == true) {
  			complement_[i*matrix_.shape()[1] + j] = false;
  		}
  	}
  }

 private:

  const MatrixType& matrix_;
  size_type idx_;
  std::vector<bool> complement_;

  friend reference;
};

template <typename MatrixType>
class complement_matrix_view {
public:
  using value_type = typename MatrixType::value_type;
  using index_type = typename MatrixType::index_type;
  using allocator_type = typename MatrixType::allocator_type;
  using size_type = typename MatrixType::size_type;
  using difference_type = typename MatrixType::difference_type;

  using iterator = complement_matrix_view_iterator<MatrixType>;
  using reference = complement_matrix_view_reference<MatrixType>;

  complement_matrix_view(const MatrixType& matrix) : matrix_(matrix) {}

  iterator begin() const {
    return iterator(matrix_, 0);
  }

  iterator end() const {
    return iterator(matrix_, matrix_.shape()[0]*matrix_.shape()[1]);
  }

  index_t shape() const noexcept {
  	return matrix_.shape();
  }

  // TODO: should we try to "skip over" stored values equal to true?
  size_type size() {
    size_t num_elem = matrix_.shape()[0]*matrix_.shape()[1] - matrix_.size();
    for (auto&& [i, j, v] : matrix_) {
      if (static_cast<bool>(v) == false) {
        num_elem++;
      }
    }
  	return num_elem;
  }

private:

	const MatrixType& matrix_;

  friend iterator;
  friend reference;
};

} // end grb