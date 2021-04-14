#include <vector>
#include <string>

#include <grb/grb.hpp>

template <typename MatrixType>
void check_size(const MatrixType& matrix, size_t m, size_t n, size_t nnz) {
    SECTION( "check shape of matrix" ) {
      grb::index_t shape = matrix.shape();
      REQUIRE( shape[0] == m );
      REQUIRE( shape[1] == n );
      REQUIRE( matrix.size() == nnz );
    }
}

TEMPLATE_PRODUCT_TEST_CASE( "matrix can be read in, access dimensions, etc.", "[matrix][template]",
	(grb::matrix), ((float, int), (float, size_t)) ) {

	std::vector<std::string> fnames = {"../examples/data/chesapeake.mtx"};
	std::vector<std::tuple<size_t, size_t, size_t>> details = {{39, 39, 170}};
	for (size_t i = 0; i < fnames.size(); i++) {
		const auto& fname = fnames[i];
		const auto& detail = details[i];
	  GIVEN( "Matrix read from \"" + fname + "\"") {
	    TestType matrix(fname);

	    check_size(matrix, std::get<0>(detail), std::get<1>(detail), std::get<2>(detail));
	  }
  }
}
