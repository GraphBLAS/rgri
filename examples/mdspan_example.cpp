#include <grb/grb.hpp>

template <grb::MatrixRange M>
void matrix(M&&) {}

int main(int argc, char** argv) {
  grb::matrix<float, int> a("data/chesapeake.mtx");

  grb::matrix<float, std::size_t, grb::dense> b("data/chesapeake.mtx");

  std::vector<float> data(b.shape()[0]*b.shape()[1], 0);

  std::experimental::mdspan b_span(data.data(), b.shape()[0], b.shape()[1]);

  auto view = grb::views::all(b_span);

  for (auto&& [index, value] : a) {
    view[index] = value;
  }

  matrix(view);

  auto v = grb::multiply(a, grb::views::all(b_span));

  grb::print(v);

  return 0;
}
