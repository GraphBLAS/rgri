#include <grb/grb.hpp>
#include <iterator>
#include <ranges>

int main(int argc, char** argv) {
  auto x = grb::generate_random<float>({10, 10}, 0.5);
  grb::matrix<float> y({10, 10});

  y[{1, 2}] = 12;

  grb::print(x, "x before");

  grb::print(y, "y before");

  grb::assign(x, y);

  grb::print(x, "x after");

  grb::print(y, "y after");

  auto x_v = grb::generate_random<float>(10, 0.5);

  grb::print(x_v, "x_v");

  auto y_v = grb::generate_random<float>(10, 0.5);

  grb::print(y_v, "y_v");

  grb::assign(x_v, y_v);

  grb::print(x_v, "x_v after");

  return 0;
}
