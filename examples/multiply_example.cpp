#include <grb/grb.hpp>

int main(int argc, char** argv) {
  auto x = grb::generate_random<int>({10, 10}, 0.1);
  auto y = grb::generate_random<int>({10, 10}, 0.1);
  auto z = grb::multiply(x, y);
  grb::print(z, "result");

  grb::matrix<int> c({10, 10});

  c[{7, 7}] = 12;

  grb::multiply(c, x, y);

  grb::print(c, "c");

  auto x1 = grb::generate_random<int>(10, 0.8);
  auto y1 = grb::generate_random<int>(10, 0.8);
  grb::print(x1, "x");
  grb::print(y1, "y");
  auto z1 = grb::multiply(x1, y1);

  grb::vector<int> c_v(10);

  c_v[1] = 12;

  grb::multiply(c_v, x1, x);

  auto c_ref = grb::multiply(x1, x);

  grb::print(c_v, "c_v");
  grb::print(c_v, "c_ref");

  return 0;
}
