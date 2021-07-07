# Reference GraphBLAS Implementation (RGRI)
This is a reference implementation of the draft C++ specification of GraphBLAS.
GraphBLAS provides facilities for implementing graph algorithms in the language
of linear algebra.

GraphBLAS provides data structures for storing dense and sparse matrix and vector objects, as well as algorithms, such as matrix multiplication, that can be applied to them in order to build graph algorithms.  GraphBLAS also supports operations using non-standard semirings instead of the classical linear algebra `+` and `*`, including a variety of built-in semirings and the ability to create custom semirings and monoids.

```C++
#include <grb/grb.hpp>

int main(int argc, char** argv) {
  // Create a new matrix, reading in from a file.
  grb::matrix<float, int> a("data/chesapeake.mtx");

  size_t m = a.shape()[0];
  size_t k = a.shape()[1];

  // Set element 12,9 (row 12, column 9) to 12.
  a[{12, 9}] = 12;

  grb::matrix<float, int> b(a.shape()[0], a.shape()[1]);

  for (auto ref : a) {
  	float value = ref;
  	grb::index_t idx = ref;
  	b[idx] = value*12;
  }

  auto c = grb::multiply(a, b);

  std::cout << grb::sum(c) << std::endl;

  return 0;
}

```

More documentation is [available here](https://rgri.readthedocs.io/en/main/?).

## Compiling with RGRI
RGRI is a header-only library. To compile with RGRI, add the `include` directory to your path (something like `-I$HOME/src/rgri/include`) and include `grb/grb.hpp` in your source files.  RGRI requires C++17, which will likely require the compile flag `-std=gnu++17`, `-std=c++17` or higher.  Check the directory `examples` for examples.

