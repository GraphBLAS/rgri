#include <grb/grb.hpp>
#include <iterator>
#include <iostream>

template <typename M>
size_t pick_random_vertex(M&& matrix) {
  size_t index_id = lrand48() % matrix.size();
  auto iter = matrix.begin();
  std::advance(iter, index_id);
  auto&& [idx, _] = *iter;
  auto&& [i, j] = idx;
  return i;
}

int main(int argc, char** argv) {
  srand48(1);
  // Import graph
  grb::matrix<int> a("data/chesapeake.mtx");

  // Create vector
  grb::vector<int> x(a.shape()[1]);

  // Pick a random vertex at which to start
  int vertex = pick_random_vertex(a);

  x[vertex] = 1;

  grb::vector<int> mask = x;

  while (x.size() > 0) {
    std::cout << "Iteration 1:" << std::endl;;
    grb::print(x);
    auto b = grb::multiply_mv(a, x, grb::plus{}, grb::multiplies{}, mask);

    std::cout << "Output:" << std::endl;
    grb::print(b);

    auto new_mask = grb::ewise_union(b, mask, grb::plus{});

    std::swap(new_mask, mask);
    std::swap(x, b);
  }

  return 0;
}
