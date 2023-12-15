#include <algorithm>
#include <grb/grb.hpp>

int main(int argc, char** argv) {
  // Import graph
  grb::matrix<bool> a("../chesapeake/chesapeake.mtx");

  auto l = grb::views::filter(a, grb::lower_triangle());

  auto c = grb::multiply(l, l, grb::plus(), grb::times(), l);

  auto values = grb::views::values(c);

  auto num_triangles = std::reduce(values.begin(), values.end());

  std::cout << "Counted " << num_triangles << " triangles.\n";

  return 0;
}
