#include <grb/grb.hpp>
#include <unistd.h>

template <typename T = float, typename I = std::size_t>
auto generate_random_matrix(I m, I n, std::size_t nnz) {
  grb::matrix<T, I> x({m, n});

  for (size_t i = 0; i < nnz; i++) {
    I i_ = lrand48() % m;
    I j_ = lrand48() % n;
    x.insert({{i_, j_}, T(drand48())});
  }
  return x;
}

int main(int argc, char** argv) {
  auto x = generate_random_matrix(8, 8, 10);
  auto y = generate_random_matrix(8, 8, 20);

  auto z = grb::ewise_intersection(x, y, [](auto a, auto b) { return a + b; });

  grb::print(x, "x");
  grb::print(y, "y");

  grb::print(z, "ewise_mult");

  auto z2 = grb::ewise_union(x, y, grb::plus());

  grb::print(z2, "ewise_add");

  return 0;
}
