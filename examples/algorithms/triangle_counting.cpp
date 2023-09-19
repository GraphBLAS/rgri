#include <grb/grb.hpp>

template <grb::MatrixRange M>
auto lower_triangle(M&& m) {
  using T = grb::matrix_scalar_t<M>;
  using I = grb::matrix_index_t<M>;
  grb::matrix<T, I> l(m.shape());

  for (auto&& [idx, v] : m) {
    auto&& [i, j] = idx;
    if (i <= j) {
      l.insert({idx, v});
    }
  }
  return l;
}

int main(int argc, char** argv) {
  // Import graph
  grb::matrix<bool> a("../data/chesapeake.mtx");

  auto l = lower_triangle(a);

  auto c = grb::multiply(l, l, grb::plus(), grb::times(), l);

  auto values = grb::views::values(c);

  auto num_triangles = std::reduce(values.begin(), values.end());

  std::cout << "Counted " << num_triangles << " triangles.\n";

  return 0;
}