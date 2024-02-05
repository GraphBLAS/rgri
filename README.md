# Reference GraphBLAS Implementation (RGRI)
This is a reference implementation of the [draft C++ specification of GraphBLAS](https://github.com/GraphBLAS/graphblas-api-cpp).
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

  grb::matrix<float, int> b({a.shape()[0], a.shape()[1]});

  for (auto&& [index, value] : a) {
  	b[index] = value*12;
  }

  auto c = grb::multiply(a, b);

  grb::print(c);

  return 0;
}

```

More documentation is [available here](https://rgri.readthedocs.io/en/main/?).

## Compiling with RGRI
RGRI is a header-only library. To compile with RGRI, add the `include` directory to your path (something like `-I$HOME/src/rgri/include`) and include `grb/grb.hpp` in your source files.  RGRI requires C++20, which will likely require the compile flag `-std=gnu++20`, `-std=c++20` or higher.  Check the directory `examples` for examples.

## Citation
To cite the RGRI reference GraphBLAS implementation:

Benjamin Brock. RGRI: A reference implementation of the C++ GraphBLAS specification. https://github.com/GraphBLAS/rgri, 2022.

```bibtex
@misc{rgri_repo,
  title = {{RGRI}: A Reference Implementation of the {C++} {GraphBLAS} Specification},
  howpublished = {\url{https://github.com/GraphBLAS/rgri}},
  author={Benjamin Brock},
  year={2022}
}
```

To cite the [GraphBLAS C++ Specification](https://github.com/GraphBLAS/graphblas-api-cpp):

Benjamin Brock, Scott McMillan, Aydın Buluc ̧, Timothy Mattson, and Jose ́ Moreira. GraphBLAS C++ specification. https://github.com/ GraphBLAS/graphblas-api-cpp, 2022.

```bibtex
@misc{graphblascppspec,
  title={Graph{BLAS} {C}++ Specification},
  author={Brock, Benjamin and McMillan, Scott and Bulu\c{c}, Ayd{\i}n and Mattson, Timothy and Moreira, Jos{\'e}},
  howpublished={\url{https://github.com/GraphBLAS/graphblas-api-cpp}},
  volume = {Version 1.0},
  year={2023}
}
```
