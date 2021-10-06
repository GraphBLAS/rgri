#include <vector>
#include <string>

#include <grb/grb.hpp>
#include <catch.hpp>

TEMPLATE_PRODUCT_TEST_CASE( "can create insert into matrix", "[matrix][template]",
  (grb::matrix), ((float, int, grb::sparse), (float, size_t, grb::sparse),
                  (float, int, grb::dense),  (float, size_t, grb::dense))) {

  using value_type = typename TestType::value_type;

  // Each of the test matrices should have `1` as the value for each nonzero.
	std::vector<std::string> fnames = {"../examples/data/chesapeake.mtx"};
	std::vector<std::tuple<size_t, size_t, size_t>> details = {{39, 39, 340}};
	for (size_t i = 0; i < fnames.size(); i++) {
		const auto& fname = fnames[i];
		const auto& detail = details[i];
	  GIVEN( "Matrix read from \"" + fname + "\"") {
	    TestType matrix(fname);

      TestType insert_matrix(grb::index_t{matrix.shape()[0], matrix.shape()[1]});

      check_matrix(insert_matrix, std::get<0>(detail), std::get<1>(detail), 0, 3);

      size_t count = 0;

      for (auto val_ref : matrix) {
        count++;
        grb::index_t idx = val_ref;
        value_type value = val_ref;
        insert_matrix[idx] = 3;
        check_matrix(insert_matrix, std::get<0>(detail), std::get<1>(detail), count, 3);
      }
	  }
  }
}
