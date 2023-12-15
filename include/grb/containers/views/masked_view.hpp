#pragma once

namespace grb {

template <MatrixRange MatrixType, MaskMatrixRange MaskType>
class masked_view
    : public std::ranges::view_interface<masked_view<MatrixType, MaskType>> {
private:
  struct in_matrix;

public:
  using matrix_type = std::decay_t<MatrixType>;
  using mask_type = std::decay_t<MaskType>;
  using filter_matrix_type = grb::filter_view<const matrix_type&, in_matrix>;

  using index_type = typename matrix_type::index_type;
  using scalar_type = typename matrix_type::scalar_type;
  using size_type = typename matrix_type::size_type;
  using difference_type = typename matrix_type::difference_type;

  using iterator = typename filter_matrix_type::iterator;
  using const_iterator = typename filter_matrix_type::const_iterator;

  using value_type = std::remove_cvref_t<decltype(*std::declval<iterator>())>;

  using key_type = typename matrix_type::key_type;

  masked_view(const MatrixType& matrix, const MaskType& mask)
      : mask_(&mask), filtered_matrix_(matrix, in_matrix(*this)) {}

  grb::index<index_type> shape() const noexcept {
    return filtered_matrix_.shape();
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

      auto& mask = *matrix_->mask_;
      auto iter = mask.find(index);
      if (iter != mask.end()) {
        auto&& [_, value] = *iter;
        if (bool(value)) {
          return true;
        }
      }
      return false;
    }

    in_matrix(const masked_view& matrix) : matrix_(&matrix) {}
    in_matrix() = default;

    const masked_view* matrix_;
  };

  const mask_type* mask_;
  filter_matrix_type filtered_matrix_;
};

namespace views {

template <grb::MaskMatrixRange M>
class mask_adaptor_closure {
public:
  mask_adaptor_closure(const M& mask) : mask_(mask) {}

  template <grb::MatrixRange Matrix>
  auto operator()(Matrix&& matrix) const {
    return masked_view(std::forward<Matrix>(matrix), mask_);
  }

  template <grb::MatrixRange Matrix>
  friend auto operator|(Matrix&& matrix, const mask_adaptor_closure& closure) {
    return closure(std::forward<Matrix>(matrix));
  }

private:
  const M& mask_;
};

template <grb::MaskMatrixRange M>
auto mask(M&& mask) {
  return mask_adaptor_closure(std::forward<M>(mask));
}

template <grb::MatrixRange Matrix, grb::MaskMatrixRange M>
auto mask(Matrix&& matrix, M&& mask) {
  return masked_view(std::forward<Matrix>(matrix), std::forward<M>(mask));
}

} // namespace views

} // namespace grb
