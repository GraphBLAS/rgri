#include <grb/grb.hpp>
#include "generate.hpp"

int main(int argc, char** argv) {
  auto x = generate_random<int>({10, 10}, 0.1);
  auto y = generate_random<int>({10, 10}, 0.1);
  auto z = grb::multiply(x, y);
  grb::print(z, "result");

  auto x1 = generate_random<int>(10, 0.8);
  auto y1 = generate_random<int>(10, 0.8);
  grb::print(x1, "x");
  grb::print(y1, "y");
  auto z1 = grb::multiply(x1, y1);
  std::cout << z1.value_or(0) << std::endl;

  return 0;
}