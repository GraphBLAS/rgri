#include <grb/grb.hpp>

template <grb::MatrixRange M>
void matrix(M&&) {}

int main(int argc, char** argv) {
  grb::matrix<float, int> a("data/chesapeake.mtx");

  grb::matrix<float, std::size_t, grb::dense> b("data/chesapeake.mtx");

  std::vector<float> data(b.shape()[0] * b.shape()[1], 0);

  std::experimental::mdspan b_span(data.data(), b.shape()[0], b.shape()[1]);

  auto view = grb::views::all(b_span);

  for (auto&& [index, value] : a) {
    view[index] = value;
  }

  matrix(view);

  auto v = grb::multiply(a, grb::views::all(b_span));

  grb::print(v);

  auto values = a.backend_.values_.data();
  auto rowptr = a.backend_.rowptr_.data();
  auto colind = a.backend_.colind_.data();
  auto shape = a.shape();
  int nnz = a.size();

  grb::csr_matrix_view csr_view(values, rowptr, colind, shape, nnz);

  auto c = grb::multiply(csr_view, b);

  grb::print(c);

  /*
    auto row = csr_view.row(4);
    for (auto&& [index, v] : row) {
      auto&& [i, j] = index;
      std::cout << i << ", " << j << ": " << v << std::endl;
    }

    for (auto&& row : csr_view.rows()) {
      for (auto&& [index, v] : row) {
        auto&& [i, j] = index;
        std::cout << i << ", " << j << ": " << v << std::endl;
      }
    }
    */

  return 0;
}
