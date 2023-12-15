
#pragma once

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <grb/containers/backend/coo_matrix.hpp>

namespace grb {

template <typename T, typename I>
class mmread_matrix_iterator {
public:
  using index_type = I;
  using value_type = grb::matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = mmread_matrix_iterator<T, index_type>;

  using const_iterator = iterator;

  using reference = value_type;
  using const_reference = reference;

  using pointer = iterator;
  using const_pointer = const_iterator;

  mmread_matrix_iterator(index_type idx, std::ifstream&& f, bool is_symmetric)
      : idx_(idx), f_(std::move(f)), is_symmetric_(is_symmetric) {}

  mmread_matrix_iterator& operator++() {
    std::string buf;
    if (std::getline(f_, buf)) {
      std::istringstream ss(buf);
      if (!pattern_) {
        ss >> i_ >> j_ >> v_;
      } else {
        ss >> i_ >> j_;
        v_ = T(1);
      }
      if (true) {
        i_--;
        j_--;
      }
    }
    idx_++;
    return *this;
  }

  value_type operator*() const noexcept {
    return {{i_, j_}, v_};
  }

  bool operator==(const mmread_matrix_iterator& other) const noexcept {
    return idx_ == other.idx_;
  }

  bool operator!=(const mmread_matrix_iterator& other) const noexcept {
    return !(*this == other);
  }

private:
  index_type idx_ = 0;
  std::ifstream f_;
  bool is_symmetric_ = false;
  bool pattern_ = false;
  I i_, j_;
  T v_;
};

template <typename T, typename I = std::size_t>
class mmread_matrix {
public:
  using index_type = I;
  using value_type = grb::matrix_entry<T, I>;

  using key_type = grb::index<I>;
  using map_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = mmread_matrix_iterator<T, index_type>;

  using const_iterator = iterator;

  using reference = value_type;
  using const_reference = reference;

  using pointer = iterator;
  using const_pointer = const_iterator;

  mmread_matrix(std::string fname) : fname_(fname) {
    f_.open(fname.c_str());

    if (!f_.is_open()) {
      // TODO better choice of exception.
      throw std::runtime_error("mmread: cannot open " + fname);
    }

    std::string buf;

    // Make sure the file is matrix market matrix, coordinate, and check whether
    // it is symmetric. If the matrix is symmetric, non-diagonal elements will
    // be inserted in both (i, j) and (j, i).  Error out if skew-symmetric or
    // Hermitian.
    std::getline(f_, buf);
    std::istringstream ss(buf);
    std::string item;
    ss >> item;
    if (item != "%%MatrixMarket") {
      throw std::runtime_error(fname +
                               " could not be parsed as a Matrix Market file.");
    }
    ss >> item;
    if (item != "matrix") {
      throw std::runtime_error(fname +
                               " could not be parsed as a Matrix Market file.");
    }
    ss >> item;
    if (item != "coordinate") {
      throw std::runtime_error(fname +
                               " could not be parsed as a Matrix Market file.");
    }
    bool pattern;
    ss >> item;
    if (item == "pattern") {
      pattern = true;
    } else {
      pattern = false;
    }
    // TODO: do something with real vs. integer vs. pattern?
    ss >> item;
    bool symmetric;
    if (item == "general") {
      symmetric = false;
    } else if (item == "symmetric") {
      symmetric = true;
    } else {
      throw std::runtime_error(fname + " has an unsupported matrix type");
    }

    bool outOfComments = false;
    while (!outOfComments) {
      std::getline(f_, buf);

      if (buf[0] != '%') {
        outOfComments = true;
      }
    }

    // std::istringstream ss(buf);
    ss.clear();
    ss.str(buf);
    ss >> m_ >> n_ >> nnz_;

    is_symmetric_ = symmetric;
    // assert(!is_symmetric_);
  }

  iterator begin() noexcept {
    std::ifstream f;

    f.open(fname_.c_str());

    if (!f.is_open()) {
      // TODO better choice of exception.
      throw std::runtime_error("mmread: cannot open " + fname_);
    }

    f.seekg(f_.tellg());

    return iterator(0, std::move(f), is_symmetric_);
  }

  iterator end() noexcept {
    std::ifstream f;
    return iterator(size(), std::move(f), is_symmetric_);
  }

  grb::index<I> shape() const noexcept {
    return {m_, n_};
  }

  size_type size() const noexcept {
    return nnz_;
  }

private:
  index_type m_, n_;
  size_type nnz_;
  std::string fname_;
  std::ifstream f_;
  bool is_symmetric_ = false;
};

/// Read in the Matrix Market file at location `file_path` and a return
/// a coo_matrix data structure with its contents.
template <typename T, typename I = std::size_t>
inline coo_matrix<T, I> mmread(std::string file_path, bool one_indexed = true) {
  using index_type = I;
  using size_type = std::size_t;

  std::ifstream f;

  f.open(file_path.c_str());

  if (!f.is_open()) {
    // TODO better choice of exception.
    throw std::runtime_error("mmread: cannot open " + file_path);
  }

  std::string buf;

  // Make sure the file is matrix market matrix, coordinate, and check whether
  // it is symmetric. If the matrix is symmetric, non-diagonal elements will
  // be inserted in both (i, j) and (j, i).  Error out if skew-symmetric or
  // Hermitian.
  std::getline(f, buf);
  std::istringstream ss(buf);
  std::string item;
  ss >> item;
  if (item != "%%MatrixMarket") {
    throw std::runtime_error(file_path +
                             " could not be parsed as a Matrix Market file.");
  }
  ss >> item;
  if (item != "matrix") {
    throw std::runtime_error(file_path +
                             " could not be parsed as a Matrix Market file.");
  }
  ss >> item;
  if (item != "coordinate") {
    throw std::runtime_error(file_path +
                             " could not be parsed as a Matrix Market file.");
  }
  bool pattern;
  ss >> item;
  if (item == "pattern") {
    pattern = true;
  } else {
    pattern = false;
  }
  // TODO: do something with real vs. integer vs. pattern?
  ss >> item;
  bool symmetric;
  if (item == "general") {
    symmetric = false;
  } else if (item == "symmetric") {
    symmetric = true;
  } else {
    throw std::runtime_error(file_path + " has an unsupported matrix type");
  }

  bool outOfComments = false;
  while (!outOfComments) {
    std::getline(f, buf);

    if (buf[0] != '%') {
      outOfComments = true;
    }
  }

  I m, n, nnz;
  // std::istringstream ss(buf);
  ss.clear();
  ss.str(buf);
  ss >> m >> n >> nnz;

  // NOTE for symmetric matrices: `nnz` holds the number of stored values in
  // the matrix market file, while `matrix.nnz_` will hold the total number of
  // stored values (including "mirrored" symmetric values).
  coo_matrix<T, I> matrix({m, n});
  if (symmetric) {
    matrix.reserve(2 * nnz);
  } else {
    matrix.reserve(nnz);
  }
  /*
    TODO: reserve? (for general and for symmetric)
  */

  size_type c = 0;
  while (std::getline(f, buf)) {
    I i, j;
    T v;
    std::istringstream ss(buf);
    if (!pattern) {
      ss >> i >> j >> v;
    } else {
      ss >> i >> j;
      v = T(1);
    }
    if (one_indexed) {
      i--;
      j--;
    }

    if (i >= m || j >= n) {
      throw std::runtime_error(
          "read_MatrixMarket: file has nonzero out of bounds.");
    }

    matrix.push_back({{i, j}, v});

    if (symmetric && i != j) {
      matrix.push_back({{j, i}, v});
    }

    c++;
    if (c > nnz) {
      throw std::runtime_error("read_MatrixMarket: error reading Matrix Market "
                               "file, file has more nonzeros than reported.");
    }
  }

  // std::sort(matrix.matrix_.begin(), matrix.matrix_.end(), sort_fn);

  f.close();

  return matrix;
}

} // namespace grb
