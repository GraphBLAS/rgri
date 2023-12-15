#include <grb/grb.hpp>
#include <iostream>

void ewise_matrix() {
  std::cout << "Performing element-wise operations on 10 x 10 matrix."
            << std::endl;
  grb::matrix<float> x({10, 10});

  size_t nnz = 10;

  for (size_t i = 0; i < nnz; i++) {
    x[{lrand48() % x.shape()[0], lrand48() % x.shape()[1]}] = drand48();
  }

  grb::matrix<float> y = x;

  x[{2, 2}] = 12;
  x[{4, 4}] = 12;

  grb::print(x, "x");
  grb::print(y, "y");

  grb::matrix<int> mask(x.shape());

  for (auto&& [idx, _] : x) {
    if (lrand48() % 2) {
      mask[idx] = true;
    }
  }

  mask[{2, 2}] = true;

  grb::print(mask, "mask");

  auto z_union = grb::ewise_union(
      x, y, [](auto a, auto b) { return a + b; }, mask);
  auto z_intersection = grb::ewise_intersection(
      x, y, [](auto a, auto b) { return a + b; }, mask);

  grb::print(z_union, "union");

  grb::print(z_intersection, "intersection");
}

void ewise_vector() {
  std::cout << "Performing element-wise operations on dim 10 vector."
            << std::endl;
  grb::vector<float> x(10);

  size_t nnz = 4;

  for (size_t i = 0; i < nnz; i++) {
    x[lrand48() % x.shape()] = drand48();
  }

  grb::vector<float> y = x;

  y[2] = 12;
  y[3] = 12;

  grb::print(x, "x");
  grb::print(y, "y");

  grb::vector<int> mask(x.shape());

  for (auto&& [idx, _] : x) {
    if (lrand48() % 2) {
      mask[idx] = true;
    }
  }

  grb::print(mask, "mask");

  auto z_intersection = grb::ewise_intersection(
      x, y, [](auto a, auto b) { return a + b; }, mask);
  auto z_union = grb::ewise_union(
      x, y, [](auto a, auto b) { return a + b; }, mask);

  grb::print(z_intersection, "intersection");
  grb::print(z_union, "union");
}

int main(int argc, char** argv) {
  ewise_matrix();
  ewise_vector();

  return 0;
}
