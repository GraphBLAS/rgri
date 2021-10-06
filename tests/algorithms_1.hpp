#include <vector>
#include <string>

#include <grb/grb.hpp>
#include <catch.hpp>

TEMPLATE_PRODUCT_TEST_CASE( "can perform ewise", "[matrix][template]",
  (grb::matrix), ((float, int, grb::sparse), (float, size_t, grb::sparse),
                  (float, int, grb::dense),  (float, size_t, grb::dense))) {

  using value_type = typename TestType::value_type;

  // These test matrices can have any kind of values.
	std::vector<std::string> fnames = {"../examples/data/chesapeake.mtx"};
	std::vector<std::tuple<size_t, size_t, size_t>> details = {{39, 39, 340}};
	for (size_t i = 0; i < fnames.size(); i++) {
		const auto& fname = fnames[i];
		const auto& detail = details[i];
	  GIVEN( "Matrix read from \"" + fname + "\"") {
	    TestType a_matrix(fname);
	    TestType b_matrix(fname);

      auto c_matrix = grb::ewise(a_matrix, b_matrix, grb::plus{});

      auto c_sum = grb::sum(c_matrix);

      auto a_sum = grb::sum(a_matrix);
      auto b_sum = grb::sum(b_matrix);

      REQUIRE( a_sum + b_sum == c_sum );
	  }
  }
}
