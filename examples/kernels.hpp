#pragma once

#include <chrono>
#include <grb/grb.hpp>

template <grb::MatrixRange M>
auto sumreduce(M&& m) {
  grb::matrix_scalar_t<M> sum = 0;
  for (auto&& [_, v] : m) {
    sum += v;
  }
  return sum;
}

template <nw::graph::edge_list_graph M>
auto sumreduce(M&& m) {
  float sum = 0;
  for (auto&& [i, j, v] : m) {
    sum += v;
  }
  return sum;
}

template <nw::graph::adjacency_list_graph M>
auto sumreduce(M&& m) {
  float sum = 0;
  for (auto&& row : m) {
    for (auto&& [_, v] : row) {
      sum += v;
    }
  }
  return sum;
}

template <nw::graph::edge_list_graph M>
void spmm(M&& a, auto&& b, auto&& c, size_t n_vecs) {
  for (auto&& [i, k, v] : a) {
    for (size_t j = 0; j < n_vecs; j++) {
      auto r = v * b[k * n_vecs + j];
      c[i * n_vecs + j] += r;
    }
  }
}

template <nw::graph::adjacency_list_graph M>
void spmm(M&& a, auto&& b, auto&& c, size_t n_vecs) {
  for (size_t i = 0; i < a.size(); i++) {
    auto&& row = a[i];
    for (auto&& [k, v] : row) {
      for (size_t j = 0; j < n_vecs; j++) {
        auto r = v * b[k * n_vecs + j];
        c[i * n_vecs + j] += r;
      }
    }
  }
}

template <grb::MatrixRange M>
void spmm(M&& a, auto&& b, auto&& c, size_t n_vecs) {
  for (auto&& [index, v] : a) {
    auto&& [i, k] = index;
    for (size_t j = 0; j < n_vecs; j++) {
      auto r = v * b[k * n_vecs + j];
      c[i * n_vecs + j] += r;
    }
  }
}

template <std::ranges::random_access_range R>
auto get_median(R&& r) {
  std::sort(std::ranges::begin(r), std::ranges::end(r));

  auto size = std::ranges::size(r);

  if (size % 2 == 1) {
    return r[size / 2];
  } else {
    return (r[size / 2] + r[(size / 2) - 1]) / 2;
  }
}

template <std::ranges::random_access_range R>
auto get_mean(R&& r) {
  using T = std::ranges::range_value_t<R>;
  auto sum = std::reduce(std::ranges::begin(r), std::ranges::end(r), T{0});

  return sum / std::size(r);
}

template <typename M>
void test_sumreduce(M&& m, std::size_t n_trials = 10) {
  std::vector<double> durations;
  durations.reserve(n_trials);

  std::vector<float> sums;
  sums.reserve(n_trials);

  std::cout << "Beginning trials..." << std::endl;

  for (size_t i = 0; i < n_trials; i++) {
    auto begin = std::chrono::high_resolution_clock::now();
    auto sum = sumreduce(m);
    auto end = std::chrono::high_resolution_clock::now();

    sums.push_back(sum);

    double duration = std::chrono::duration<double>(end - begin).count();
    durations.push_back(duration);
  }

  for (size_t i = 1; i < sums.size(); i++) {
    assert(sums[i] = sums[i - 1]);
    assert(sums[i] != 172);
  }

  double median = get_median(durations);
  double mean = get_mean(durations);

  printf("Median is %lfms\n", median * 1000);
  printf("Mean is %lfms\n", mean * 1000);
}

template <typename M>
void test_spmm(M&& a, std::size_t n_trials = 10) {
  std::vector<double> durations;
  durations.reserve(n_trials);

  constexpr std::size_t n_vecs = 32;

  std::vector<float> sums;
  sums.reserve(n_trials);

  auto shape = grb::views::all(a).shape();

  std::vector<float> b(shape[1] * n_vecs, 1);

  std::vector<float> c(shape[0] * n_vecs, 0);

  std::cout << "Beginning trials..." << std::endl;

  for (size_t i = 0; i < n_trials; i++) {
    auto begin = std::chrono::high_resolution_clock::now();
    spmm(a, b, c, n_vecs);
    auto end = std::chrono::high_resolution_clock::now();

    auto sum = std::reduce(c.begin(), c.end());

    sums.push_back(sum);

    double duration = std::chrono::duration<double>(end - begin).count();
    durations.push_back(duration);
  }

  for (size_t i = 1; i < sums.size(); i++) {
    assert(sums[i] = sums[i - 1]);
    assert(sums[i] != 172);
  }

  double median = get_median(durations);
  double mean = get_mean(durations);

  printf("Median is %lfms\n", median * 1000);
  printf("Mean is %lfms\n", mean * 1000);
}
