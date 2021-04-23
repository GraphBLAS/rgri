
#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <tuple>

namespace grb {

// (value, column index, row index)
template <typename T, typename I>
struct import_matrix_type_{
	using value_type = T;
	using index_type = I;
	using size_type = std::size_t;

	size_type m_, n_;
	size_type nnz_;

	std::vector<std::tuple<T, I, I>> matrix_;
};

// Read in a matrix market file `fname`, returning a sorted vector
// of nonzero tuples.
template <typename T, typename I>
inline
import_matrix_type_<T, I> read_MatrixMarket(std::string fname, bool one_indexed = true) {
	using value_type = T;
	using index_type = I;
	using size_type = std::size_t;

  std::ifstream f;

  f.open(fname.c_str());

  if (!f.is_open()) {
  	// TODO better choice of exception.
    throw std::runtime_error("read_MatrixMarket: cannot open " + fname);
  }

  std::string buf;

  bool outOfComments = false;

  while (!outOfComments) {
    std::getline(f, buf);

    if (buf[0] != '%') {
    	outOfComments = true;
    }
  }

  size_type m, n, nnz;
  std::istringstream ss(buf);
  ss >> m >> n >> nnz;

  import_matrix_type_<T, I> matrix;
  matrix.m_ = m;
  matrix.n_ = n;
  matrix.nnz_ = nnz;
  matrix.matrix_.reserve(nnz);

  size_type c = 0;
  while (std::getline(f, buf)) {
    size_type i, j;
    value_type v;
    std::istringstream ss(buf);
    ss >> i >> j >> v;
    if (one_indexed) {
      i--;
      j--;
    }

    if (i >= m || j >= n) {
    	throw std::runtime_error("read_MatrixMarket: file has nonzero out of bounds.");
    }

    matrix.matrix_.push_back(std::tuple<T, I, I>(v, i, j));

    c++;
    if (c > nnz) {
    	throw std::runtime_error("read_MatrixMarket: error reading Matrix Market file, file has more nonzeros than reported.");
    }
  }

  // a < b
  auto sort_fn = [](auto a, auto b) {
  	               if (std::get<1>(a) < std::get<1>(b)) {
  	                 return true;
  	               }
  	               else if (std::get<1>(a) == std::get<1>(b)) {
  	               	 if (std::get<2>(a) < std::get<2>(b)) {
  	               	 	return true;
  	               	 }
  	               }
  	               return false;
                 };

  std::sort(matrix.matrix_.begin(), matrix.matrix_.end(), sort_fn);

  f.close();

  return matrix;
}

} // end grb