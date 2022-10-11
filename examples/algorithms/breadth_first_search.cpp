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

  // Create vector to represent our frontier
  grb::vector<bool> x(a.shape()[1]);

  // Pick a random starting vertex.
  int vertex = pick_random_vertex(a);
  x[vertex] = true;

  std::cout << "Starting at vertex " << vertex << std::endl;

  grb::vector<bool> mask(x.shape());
  mask[vertex] = true;

  size_t iteration = 0;

  while (x.size() > 0) {
    std::cout << "Iteration " << iteration << ":" << std::endl;
    grb::print(x, "Nodes Visited This Iteration");

    grb::print(mask, "All Nodes Previously Visited");

    auto b = grb::multiply(grb::transpose(a), x,
                           grb::plus{}, grb::times<int, bool, bool>{},
                           grb::complement_view(mask));

    auto new_mask = grb::ewise_union(b, mask, [](auto&&, auto&&) -> bool { return true; });
    std::swap(new_mask, mask);


    std::swap(x, b);
    iteration++;
  }

  return 0;
}
