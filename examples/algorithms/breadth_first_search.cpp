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
  return j;
}

int main(int argc, char** argv) {
  // srand48(0);
  // Import graph
  grb::matrix<int> a("../data/chesapeake.mtx");

  // Create vector
  grb::vector<int> x(a.shape()[1]);

  // Pick a random vertex at which to start
  int vertex = pick_random_vertex(a);

  std::cout << "Starting at vertex " << vertex << std::endl;

  x[vertex] = 1;

  grb::vector<bool> mask(x.shape());
  mask[vertex] = true;

  bool v = mask[vertex];

  size_t iteration = 0;

  while (x.size() > 0) {
    std::cout << "Iteration " << iteration << ":" << std::endl;
    grb::print(x, "X");

    grb::print(mask, "Mask");

    auto b = grb::multiply(grb::transpose(a), x, grb::plus{}, grb::times{}, grb::complement_view(mask));

    auto new_mask = grb::ewise_union(b, mask, [](auto&&, auto&&) -> bool { return true; });
    std::swap(new_mask, mask);

    std::swap(x, b);
    iteration++;
  }

  return 0;
}
