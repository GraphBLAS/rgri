#include <grb/grb.hpp>
#include <iostream>
#include <iterator>

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
  grb::matrix<int> a("../chesapeake/chesapeake.mtx");

  // Create vector to represent our frontier
  grb::vector<int> x(a.shape()[1]);

  // Pick a random starting vertex.
  int vertex = pick_random_vertex(a);
  x[vertex] = true;

  std::cout << "Starting at vertex " << vertex << std::endl;

  grb::vector<int> mask(x.shape());

  size_t iteration = 1;

  mask[vertex] = iteration;

  while (x.size() > 0) {
    std::cout << "Iteration " << iteration << ":" << std::endl;
    grb::print(x, "Frontier");

    auto b =
        grb::multiply(grb::transpose(a), x, grb::plus{},
                      grb::times<int, bool, int>{}, grb::complement_view(mask));

    auto b_view = grb::views::transform(b, [=](auto&&) { return iteration; });

    auto new_mask = grb::ewise_union(
        b_view, mask, [=](auto&&, auto&&) -> int { return iteration; });

    std::swap(new_mask, mask);
    std::swap(x, b);

    iteration++;
    grb::print(mask, "Nodes visited");
  }

  grb::print(mask, "final mask");

  return 0;
}
