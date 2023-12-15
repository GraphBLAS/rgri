#include <string>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <grb/grb.hpp>

template <typename MatrixType>
void check_matrix(MatrixType& matrix, size_t m, size_t n, size_t nnz,
                  size_t expected_value) {
  SECTION("check that sum of matrix elements matches expected value") {
    auto shape = matrix.shape();
    REQUIRE(shape[0] == m);
    REQUIRE(shape[1] == n);
    using hint_type = typename MatrixType::hint_type;
    REQUIRE(matrix.size() == nnz);

    size_t sum = 0;
    size_t counted_nnz = 0;
    for (auto&& [index, v] : matrix) {
      auto&& [i, j] = index;
      REQUIRE(i < m);
      REQUIRE(j < n);
      REQUIRE(v == expected_value);
      sum += v;
      counted_nnz++;
    }
    REQUIRE(matrix.size() == counted_nnz);
    REQUIRE(sum == expected_value * nnz);
  }
}

TEMPLATE_PRODUCT_TEST_CASE("can iterate through matrix, modify values",
                           "[matrix][template]", (grb::matrix),
                           ((float, int, grb::sparse),
                            (float, size_t, grb::sparse),
                            (float, int, grb::dense),
                            (float, size_t, grb::dense))) {

  // Each of the test matrices should have `1` as the value for each nonzero.
  std::vector<std::string> fnames = {"chesapeake/chesapeake.mtx"};
  std::vector<std::tuple<size_t, size_t, size_t>> details = {{39, 39, 340}};
  for (size_t i = 0; i < fnames.size(); i++) {
    const auto& fname = fnames[i];
    const auto& detail = details[i];
    GIVEN("Matrix read from \"" + fname + "\"") {
      TestType matrix(fname);

      check_matrix(matrix, std::get<0>(detail), std::get<1>(detail),
                   std::get<2>(detail), 1);

      using T = typename decltype(matrix)::scalar_type;
      using I = typename decltype(matrix)::index_type;

      std::for_each(matrix.begin(), matrix.end(),
                    [](grb::matrix_entry<T, I> val_ref) {
                      auto&& [index, value] = val_ref;
                      value += 2;
                    });

      check_matrix(matrix, std::get<0>(detail), std::get<1>(detail),
                   std::get<2>(detail), 3);
    }
  }
}
