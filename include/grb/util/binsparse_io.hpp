#pragma once

#include "hdf5_tools.hpp"
#include <grb/grb.hpp>
#include <nlohmann/json.hpp>

namespace grb {
namespace experimental {

template <grb::MatrixRange M>
void binsparse_mwrite(std::string fname, M&& matrix) {

  using index_type = grb::matrix_index_t<M>;
  using value_type = grb::matrix_scalar_t<M>;

  std::vector<std::ranges::range_value_t<M>> tuples(matrix.begin(),
                                                    matrix.end());

  auto sort_fn = [](const auto& a, const auto& b) {
    auto&& [a_index, a_value] = a;
    auto&& [b_index, b_value] = b;
    auto&& [a_i, a_j] = a_index;
    auto&& [b_i, b_j] = b_index;
    if (a_i < b_i) {
      return true;
    } else if (a_i == b_i) {
      if (a_j < b_j) {
        return true;
      }
    }
    return false;
  };

  std::sort(tuples.begin(), tuples.end(), sort_fn);

  std::vector<grb::matrix_index_t<M>> rows;
  std::vector<grb::matrix_index_t<M>> cols;
  std::vector<grb::matrix_scalar_t<M>> vals;

  rows.reserve(matrix.size());
  cols.reserve(matrix.size());
  vals.reserve(matrix.size());

  for (auto&& [index, value] : tuples) {
    auto&& [row, col] = index;
    rows.push_back(row);
    cols.push_back(col);
    vals.push_back(value);
  }

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  hdf5_tools::write_dataset(f, "values", vals);
  hdf5_tools::write_dataset(f, "indices_0", rows);
  hdf5_tools::write_dataset(f, "indices_1", cols);

  std::string json_string = "{\n"
                            "  \"format\": \"COO\",\n"
                            "  \"shape\": [";
  json_string += std::to_string(matrix.shape()[0]) + ", " +
                 std::to_string(matrix.shape()[1]) + "],\n" +
                 "  \"nnz\": " + std::to_string(matrix.size()) + "\n" + "}\n";

  hdf5_tools::write_dataset(f, "metadata", json_string);

  f.close();
}

template <typename T, typename I = std::size_t, typename Hint = grb::sparse>
grb::matrix<T, I, Hint> binsparse_mread(std::string fname) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = hdf5_tools::read_dataset<char>(f, "metadata");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  if (data["format"] == "COO") {
    auto nrows = data["shape"][0];
    auto ncols = data["shape"][1];
    auto nnz = data["nnz"];

    auto values = hdf5_tools::read_dataset<T>(f, "values");
    auto rows = hdf5_tools::read_dataset<T>(f, "indices_0");
    auto cols = hdf5_tools::read_dataset<T>(f, "indices_1");

    grb::matrix<T, I, Hint> m({nrows, ncols});

    std::vector<std::tuple<std::tuple<I, I>, T>> tuples;

    for (size_t i = 0; i < nnz; i++) {
      tuples.push_back({{rows[i], cols[i]}, values[i]});
    }

    m.insert(tuples.begin(), tuples.end());

    return m;
  } else {
    assert(false);
  }
  f.close();
}

} // namespace experimental

} // namespace grb
