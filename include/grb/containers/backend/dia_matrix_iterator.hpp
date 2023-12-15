#pragma once

#include <map>
#include <vector>

template <typename T, typename I>
class dia_matrix_iterator {
public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // using element_type = T;
  using index_type = I;
  using map_type = T;

  using value_type = grb::matrix_entry<T, index_type>;
  using iterator = dia_matrix_iterator;
  using const_iterator = dia_matrix_iterator<std::add_const_t<T>, index_type>;

  using reference = grb::matrix_ref<T, I>;
  using const_reference = grb::matrix_ref<std::add_const_t<T>, I>;

  using pointer = iterator;
  using const_pointer = const_iterator;

  using iterator_category = std::forward_iterator_tag;

  dia_matrix_iterator(
      index_type idx, index_type m, index_type n,
      typename std::map<index_type,
                        std::pair<std::vector<T>, std::vector<bool>>>::iterator
          diagonal,
      typename std::map<index_type,
                        std::pair<std::vector<T>, std::vector<bool>>>::iterator
          end)
      : idx_(idx), m_(m), n_(n), diagonal_(diagonal), end_(end) {
    fast_forward();
  }

  void fast_forward() {
    index_type diagonal_count = diagonal_->second.second.size();
    if (idx_ >= diagonal_count) {
      ++diagonal_;
      idx_ = 0;
    }
    while (diagonal_ != end_ && !bool(diagonal_->second.second[idx_])) {
      idx_++;
      index_type diagonal_count = diagonal_->second.second.size();
      if (idx_ >= diagonal_count) {
        ++diagonal_;
        idx_ = 0;
      }
    }
  }

  dia_matrix_iterator& operator++() noexcept {
    idx_++;
    fast_forward();
    return *this;
  }

  dia_matrix_iterator operator++(int) noexcept {
    dia_matrix_iterator other = *this;
    idx_++;
    fast_forward();
    return other;
  }

  reference operator*() noexcept {
    difference_type i, j;

    difference_type v =
        difference_type(m_) - 1 - difference_type(diagonal_->first);
    if (v >= 0) {
      i = v;
      j = 0;
    } else {
      i = 0;
      j = std::abs(v);
    }

    i += idx_;
    j += idx_;

    return reference({index_type(i), index_type(j)},
                     diagonal_->second.first[idx_]);
  }

  dia_matrix_iterator() = default;
  ~dia_matrix_iterator() = default;
  dia_matrix_iterator(const dia_matrix_iterator&) = default;
  dia_matrix_iterator(dia_matrix_iterator&&) = default;
  dia_matrix_iterator& operator=(const dia_matrix_iterator&) = default;
  dia_matrix_iterator& operator=(dia_matrix_iterator&&) = default;

  bool operator==(const dia_matrix_iterator&) const = default;
  bool operator!=(const dia_matrix_iterator&) const = default;

private:
  index_type idx_;
  index_type m_, n_;
  typename std::map<index_type,
                    std::pair<std::vector<T>, std::vector<bool>>>::iterator
      diagonal_;
  typename std::map<
      index_type, std::pair<std::vector<T>, std::vector<bool>>>::iterator end_;
};
