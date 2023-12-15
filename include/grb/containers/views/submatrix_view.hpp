
#pragma once

#include "filter_matrix.hpp"
#include <grb/containers/matrix_entry.hpp>
#include <grb/detail/iterator_adaptor.hpp>
#include <grb/detail/matrix_traits.hpp>
#include <grb/util/index.hpp>

namespace grb {

template <typename MatrixType>
class submatrix_view {
private:
  struct in_matrix;

public:
  using matrix_type = std::decay_t<MatrixType>;
  // using filter_matrix_type =
  // decltype(grb::filter_matrix_view(std::declval<const MatrixType&>(),
  // in_matrix<std::ranges::range_value_t<matrix_type>)); using
  // filter_matrix_type = grb::filter_matrix_view<MatrixType,
  // decltype(in_matrix<std::ranges::range_value_t<matrix_type>)>;
  using filter_matrix_type = grb::filter_view<matrix_type, in_matrix>;

  using index_type = typename matrix_type::index_type;
  using scalar_type = typename matrix_type::scalar_type;
  using size_type = typename matrix_type::size_type;
  using difference_type = typename matrix_type::difference_type;

  using iterator = typename filter_matrix_type::iterator;
  using const_iterator = typename filter_matrix_type::const_iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  using key_type = typename matrix_type::key_type;

  submatrix_view(const MatrixType& matrix, grb::index<index_type> rows,
                 grb::index<index_type> columns)
      : rows_(rows), columns_(columns),
        filtered_matrix_(matrix, in_matrix(*this)) {}

  grb::index<index_type> shape() const noexcept {
    return {rows_[1] - rows_[0], columns_[1] - columns_[0]};
  }

  size_type size() const noexcept {
    return filtered_matrix_.size();
  }

  iterator begin() const noexcept {
    return filtered_matrix_.begin();
  }

  iterator end() const noexcept {
    return filtered_matrix_.end();
  }

  iterator find(key_type key) const noexcept {
    return filtered_matrix_.find(key);
  }

private:
  struct in_matrix {
    template <typename Entry>
    bool operator()(Entry&& entry) const {
      auto&& [index, _] = entry;
      if (index[0] >= matrix_->rows_[0] && index[0] < matrix_->rows_[1]) {
        if (index[1] >= matrix_->columns_[0] &&
            index[1] < matrix_->columns_[1]) {
          return true;
        }
      }
      return false;
    }

    in_matrix(const submatrix_view& matrix) : matrix_(&matrix) {}
    in_matrix() = default;

    const submatrix_view* matrix_;
  };

  grb::index<index_type> rows_;
  grb::index<index_type> columns_;
  filter_matrix_type filtered_matrix_;
};

} // namespace grb
