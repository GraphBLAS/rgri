#include <grb/grb.hpp>

TEMPLATE_PRODUCT_TEST_CASE( "matrix can be read in, access dimensions, etc.", "[matrix][template]", (grb::matrix), ((float, int), (float, size_t)) ) {
  GIVEN( "A matrix read from a file" ) {
    TestType matrix("../examples/data/chesapeake.mtx");

    SECTION( "check shape of matrix" ) {
      grb::index_t shape = matrix.shape();
      REQUIRE( shape[0] == 39 );
      REQUIRE( shape[1] == 39 );
      REQUIRE( matrix.size() == 170 );
    }
  }
}
