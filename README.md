# Reference GraphBLAS Implementation (RGRI)
This is a reference implementation of the draft C++ specification of GraphBLAS.
GraphBLAS provides facilities for implementing graph algorithms in the language
of linear algebra.

## Compiling with RGRI
RGRI is a header-only library. To compile with RGRI, add the `include` directory to your path (something like `-I$HOME/src/rgri/include`) and include `grb/grb.hpp` in your source files.  RGRI requires C++17, which will likely require the compile flag `-std=gnu++17`, `-std=c++17` or higher.  Check the directory `examples` for examples.

Documentation is available on [read the docs](https://rgri.readthedocs.io/en/latest/?).