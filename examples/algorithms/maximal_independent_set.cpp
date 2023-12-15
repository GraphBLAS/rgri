#include <grb/grb.hpp>
#include <iostream>
#include <iterator>

int main(int argc, char** argv) {
  // srand48(0);
  // Import graph
  grb::matrix<int> a("../chesapeake/chesapeake.mtx");

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution;
  generator.seed(0);

  grb::vector<int> independent_set(a.shape()[0]);

  // Compute the degree of each vertex
  auto a_structure = grb::views::transform(a, [](auto&&) -> int { return 1; });
  auto degrees = grb::reduce(a_structure);

  grb::vector<bool> candidates = degrees;

  grb::assign(independent_set, grb::complement_view(degrees));

  grb::print(independent_set, "independent set");

  grb::print(candidates, "candidates");

  size_t iteration = 0;

  while (!candidates.empty()) {
    printf("Iteration %lu\n", iteration++);
    auto prob = grb::ewise_intersection(
        candidates, degrees, [&](auto&&, int degree) -> float {
          return 0.0001 + distribution(generator) / 1. + 2. * degree;
        });

    grb::print(prob, "probabilities");

    auto neighbor_max = grb::multiply(a, prob, grb::max(), grb::take_right());

    grb::print(neighbor_max, "neighbor max");

    auto new_members =
        grb::ewise_union(prob, neighbor_max, std::greater<float>());
    grb::print(new_members, "new members");

    grb::vector<bool> new_members_sparse(new_members.shape());

    for (auto&& [idx, value] : new_members) {
      if (bool(value)) {
        new_members_sparse.insert({idx, value});
      }
    }

    grb::print(new_members_sparse, "sparsified new members");

    // Remove new_members from candidates
    candidates = grb::ewise_intersection(
        candidates, grb::complement_view(new_members_sparse),
        grb::take_left<int>());

    grb::print(candidates, "candidates with new members removed");

    // Add new_members to independent_set
    independent_set =
        grb::ewise_union(independent_set, new_members_sparse, grb::plus());

    grb::print(independent_set, "independent set with new candidates");

    auto new_neighbors = grb::multiply(a, new_members_sparse, grb::logical_or(),
                                       grb::logical_and());

    grb::print(new_neighbors, "new neighbors");

    candidates = grb::ewise_intersection(
        candidates, grb::complement_view(new_neighbors), grb::take_left());

    grb::print(candidates, "candidates with new neighbors removed");
  }

  printf("RESULT:\n");
  grb::print(independent_set, "Maximal Independent Set");

  return 0;
}
