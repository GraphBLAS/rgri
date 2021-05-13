#include <grb/grb.hpp>
#include <functional>

int main(int argc, char** argv) {
	grb::matrix<float, int, grb::sparse> a({1024, 1024});

  a[{12, 13}] = 12;
  a[{34, 12}] = 12;
  a[{0, 2}] = 0.1;
  a[{123, 123}] = 14;
  a[{141, 191}] = 12;

  grb::vector<float, int, grb::sparse> b(1024);

  b[12] = 12;

  grb::vector<float, int, grb::sparse> c(1024);

  grb::vector<float, int, grb::sparse> mask(1024);

  auto plus = [](auto a, auto b) { return a + b; };
  auto times = [](auto a, auto b) { return a * b; };

  auto times_plus = grb::make_semiring(times, plus);

  grb::mxv(a, b, c, c, times_plus, std::plus<float>{});

  std::cout << times_plus.combine(12, 13) << std::endl;
  std::cout << times_plus.reduce(12, 13) << std::endl;

  return 0;
}

