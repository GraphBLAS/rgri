#include <grb/containers/backend/csr_matrix.hpp>
#include <grb/containers/backend/coo_matrix.hpp>
#include <grb/util/matrix_io.hpp>
#include <ranges>
#include <fmt/core.h>
#include <iterator>

template <std::ranges::random_access_range R>
void foo(R&& r) {
  size_t count = 0;
  for (auto&& v : r) {
    count++;
  }
  fmt::print("{} elements in range\n", count);
}

int main(int argc, char** argv) {
  grb::coo_matrix<float, int> x({1024, 1024});

  std::tuple<std::tuple<int, int>, float> elem{{12, 12}, 42.0f};

  // x.insert(&elem, &elem + 1);
  x.insert({{12, 12}, 42.0f});
  x.insert({{13, 14}, 42.0f});
  x.insert({{52, 33}, 42.0f});
  x.insert_or_assign({52, 33}, 13.0f);
  x.insert_or_assign({52, 33}, 44.0f);

  fmt::print("{}, {}: {}\n", x.shape()[0], x.shape()[1], x.size());

  const auto& x_r = x;
  std::ranges::for_each(x, [](auto&& entry) {
                             auto&& [index, value] = entry;
                             // entry = "1234";
                             value = 12;
                             // entry.value() = 12;
                             auto&& [i, j] = index;
                             fmt::print("{}, {}: {}\n", index[0], index[1],
                                        value);
  });

  auto iter = x.find({52, 33});
  if (iter != x.end()) {
    fmt::print("Found!\n");
  }

  size_t count = 0;
  for (auto&& [index, value] : x) {
    auto&& [i, j] = index;
    count++;
  }

  fmt::print("Counted {} values\n", count);

/*
  size_t count = 0;
  for (auto iter = x.begin(); iter != x.end(); ++iter) {
    // grb::matrix_element<const float> elem_ = *iter;
    // auto&& [i, j, v] = *iter;
    auto&& [i, j, v] = elem;
    grb::matrix_element<const float> elem = *iter;
    auto&& [i, j, v] = elem;
    fmt::print("{}, {}, {}\n", i, j, v);
    count++;
  }

  std::vector<int> v(x.size());

  std::transform(x.begin(), x.end(), v.begin(), [](auto x) {
    auto&& [i, j, v] = x;
    fmt::print("{}, {}, {}\n", i, j, v);
    return i;
  });

  fmt::print("{}\n", v[0]);
  */

  return 0;
}
