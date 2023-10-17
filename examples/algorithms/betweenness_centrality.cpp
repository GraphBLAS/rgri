#include <grb/grb.hpp>

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
  grb::matrix<int> a("../data/chesapeake.mtx");

  int source_vertex = pick_random_vertex(a);

  grb::vector<float> delta(a.shape()[0]);

  std::vector<grb::vector<int>> sigma;

  grb::vector<int> q(a.shape()[0]);

  q[source_vertex] = 1;

  grb::vector<int> p = q;

  q = grb::multiply(grb::transpose(a), q, grb::plus{}, grb::times{}, grb::complement_view(q));

  int d = 0;
  int sum = 0;

  do {
    sigma.push_back(p);

    p = grb::ewise_union(p, q, grb::plus{});

    q = grb::multiply(grb::transpose(a), q, grb::plus{}, grb::times{}, grb::complement_view(p));

    ++d;
  } while (!q.empty());

  grb::vector<float> t1(a.shape()[0]);
  grb::vector<float> t2(a.shape()[0]);
  grb::vector<float> t3(a.shape()[0]);
  grb::vector<float> t4(a.shape()[0]);

  for (int i = d-1; i > 0; i--) {
    grb::assign(t1, 1.0f);

    t1 = grb::ewise_union(t1, delta, grb::plus{});

    auto&& t2 = sigma[i];

    t2 = grb::ewise_intersection(t1, t2, grb::divides{});

    t3 = grb::multiply(a, t2, grb::plus{}, grb::times{});

    auto&& t4 = sigma[i-1];

    t4 = grb::ewise_intersection(t4, t3, grb::times{});

    delta = grb::ewise_union(delta, t4, grb::plus{});
  }

  grb::print(delta);

  return 0;
}