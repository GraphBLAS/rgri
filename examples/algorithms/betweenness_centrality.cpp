#include <grb/grb.hpp>

#include <fmt/core.h>

template <typename M>
size_t pick_random_vertex(M&& matrix) {
  size_t index_id = lrand48() % matrix.size();
  auto iter = matrix.begin();
  std::advance(iter, index_id);
  auto&& [idx, _] = *iter;
  auto&& [i, j] = idx;
  return j;
}

template <grb::MatrixRange A>
auto betweenness_centrality(A&& a, std::size_t source_vertex) {
  grb::vector<float> delta(a.shape()[0]);

  std::vector<grb::vector<int>> sigma;

  grb::vector<int> q(a.shape()[0]);

  q[source_vertex] = 1;

  grb::vector<int> p = q;

  q = grb::multiply(grb::transpose(a), q, grb::plus{}, grb::times{},
                    grb::complement_view(p));

  int d = 0;
  int sum = 0;

  do {
    sigma.push_back(q);

    p = grb::ewise_union(p, q, grb::plus{});

    q = grb::multiply(grb::transpose(a), q, grb::plus{}, grb::times{},
                      grb::complement_view(p));

    ++d;
  } while (!q.empty());

  grb::vector<float> t1(a.shape()[0]);
  grb::vector<float> t2(a.shape()[0]);
  grb::vector<float> t3(a.shape()[0]);
  grb::vector<float> t4(a.shape()[0]);

  for (int i = d - 1; i > 0; i--) {
    grb::assign(t1, 1.0f);

    t1 = grb::ewise_union(t1, delta, grb::plus{});

    t2 = sigma[i];

    t2 = grb::ewise_intersection(t1, t2, grb::divides{});
    t3 = grb::multiply(a, t2, grb::plus{}, grb::times{});
    t4 = sigma[i - 1];

    t4 = grb::ewise_intersection(t4, t3, grb::times{});

    delta = grb::ewise_union(delta, t4, grb::plus{});
  }

  return delta;
}

int main(int argc, char** argv) {
  grb::matrix<int> a("../data/gilbert.mtx");

  grb::print(a, "my graph");

  // int source_vertex = pick_random_vertex(a);
  int source_vertex = 0;

  auto d = betweenness_centrality(a, source_vertex);

  fmt::print("========================================\n");

  grb::print(d, "d");
  grb::vector<float> total_sum(a.shape()[0]);

  for (std::size_t source_vertex = 0; source_vertex < a.shape()[0];
       source_vertex++) {
    auto d = betweenness_centrality(a, source_vertex);
    grb::print(d, "d");
    total_sum = grb::ewise_union(total_sum, d, grb::plus{});
  }

  grb::print(total_sum, "sum");

  return 0;
}
