
#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <tuple>

#include <grb/containers/backend/coo_matrix.hpp>

namespace grb {

// Read in a matrix market file `fname`, returning a sorted vector
// of nonzero tuples.
template <typename T, typename I = std::size_t>
inline
coo_matrix<T, I> mmread(std::string fname, bool one_indexed = true) {
	using index_type = I;
	using size_type = std::size_t;

  std::ifstream f;

  f.open(fname.c_str());

  if (!f.is_open()) {
  	// TODO better choice of exception.
    throw std::runtime_error("mmread: cannot open " + fname);
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
    throw std::runtime_error(fname + " could not be parsed as a Matrix Market file.");
  }
  ss >> item;
  if (item != "matrix") {
    throw std::runtime_error(fname + " could not be parsed as a Matrix Market file.");
  }
  ss >> item;
  if (item != "coordinate") {
    throw std::runtime_error(fname + " could not be parsed as a Matrix Market file.");
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
    std::getline(f, buf);

    if (buf[0] != '%') {
    	outOfComments = true;
    }
  }

  size_type m, n, nnz;
  // std::istringstream ss(buf);
  ss.clear();
  ss.str(buf);
  ss >> m >> n >> nnz;

  // NOTE for symmetric matrices: `nnz` holds the number of stored values in
  // the matrix market file, while `matrix.nnz_` will hold the total number of
  // stored values (including "mirrored" symmetric values).
  coo_matrix<T, I> matrix({m, n});
  if (symmetric) {
    matrix.reserve(2*nnz);
  } else {
    matrix.reserve(nnz);
  }
  /*
    TODO: reserve? (for general and for symmetric)
  */

  size_type c = 0;
  while (std::getline(f, buf)) {
    size_type i, j;
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
    	throw std::runtime_error("read_MatrixMarket: file has nonzero out of bounds.");
    }

    matrix.insert({{i, j}, v});

    if (symmetric && i != j) {
      matrix.insert({{j, i}, v});
    }

    c++;
    if (c > nnz) {
    	throw std::runtime_error("read_MatrixMarket: error reading Matrix Market file, file has more nonzeros than reported.");
    }
  }

  // std::sort(matrix.matrix_.begin(), matrix.matrix_.end(), sort_fn);

  f.close();

  return matrix;
}

} // end grb