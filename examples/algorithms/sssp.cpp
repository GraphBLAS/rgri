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

  // Create vector to hold distances to each vertex
  grb::vector<int> dist(a.shape()[1]);

  // Pick a random vertex from which to start
  int vertex = pick_random_vertex(a);
  dist[vertex] = 0;

  std::cout << "Starting at vertex " << vertex << std::endl;

  size_t iteration = 0;

  bool updated_vertex = true;

  while (true) {
    std::cout << "Iteration " << iteration << ":" << std::endl;
    grb::print(dist, "Current distances");

    updated_vertex = false;
    auto signaling_min = [&](auto&& a, auto&& b) {
      auto min = grb::min{}(a, b);
      if (min != a) {
        printf("Setting update!\n");
        updated_vertex = true;
      }
      return min;
    };

    auto update = grb::multiply(grb::transpose(a), dist, grb::min{},
                                grb::plus{}, grb::full_vector_mask{});

    grb::print(update, "update");

    auto dist2 = grb::ewise_union(dist, update, signaling_min);

    printf("%s\n", (updated_vertex || dist.size() != dist2.size())
                       ? "was an update"
                       : "was not an update");

    if (updated_vertex || dist.size() != dist2.size()) {
      std::swap(dist, dist2);
    } else {
      break;
    }
  }

  return 0;
}
