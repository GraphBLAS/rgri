#include <algorithm>
#include <grb/grb.hpp>
#include <iomanip>
#include <iostream>

int main(int argc, char** argv) {
  grb::matrix<float, int> a("chesapeake/chesapeake.mtx");
  grb::matrix<float, int> b("chesapeake/chesapeake.mtx");

  auto c = grb::multiply(a, b);

  grb::print(c);

  return 0;
}
