#include <grb/grb.hpp>

#include <grb/containers/views/masked_view.hpp>

template <typename ContainerType>
  requires(grb::MatrixRange<ContainerType> || grb::VectorRange<ContainerType>)
class tmp : public std::ranges::view_interface<tmp<ContainerType>> {
public:
  tmp(ContainerType matrix) : matrix_(matrix) {}

private:
  std::ranges::views::all_t<ContainerType> matrix_;
};

template <grb::MatrixRange MatrixType>
tmp(MatrixType&&) -> tmp<MatrixType>;

template <grb::VectorRange VectorType>
tmp(VectorType&&) -> tmp<VectorType>;

int main(int argc, char** argv) {
  grb::matrix<float> matrix({10, 10});

  matrix[{2, 3}] = 12;
  matrix[{7, 1}] = 12;
  matrix[{5, 5}] = 12;
  matrix[{2, 7}] = 12;
  matrix[{9, 6}] = 12;

  grb::print(matrix, "original matrix");

  auto matrix_t = grb::transpose(matrix);

  grb::print(matrix_t, "matrix transposed");

  auto matrix_tr = grb::views::transform(matrix_t, [](auto&& entry) {
    auto&& [idx, value] = entry;
    auto&& [i, j] = idx;
    return i + j / 10.0f;
  });

  grb::print(matrix_tr, "matrix transformed");

  auto matrix_str = grb::views::structure(matrix_tr);

  grb::print(matrix_str, "matrix structure");

  auto lel = grb::get<1>(*matrix_str.begin());

  auto filter = grb::views::filter(matrix_tr, [](auto&& entry) {
    auto&& [index, value] = entry;
    auto&& [i, j] = index;
    return i < 5;
  });

  grb::print(filter);

  auto jklse = [](auto&&) { return "jklse"; };

  std::ranges::transform_view tview(matrix, jklse);

  std::cout << "Transform view: ";
  for (auto&& v : tview) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  std::cout << "Transform view adapt: ";
  for (auto&& v : std::ranges::views::transform(jklse)(matrix)) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  grb::matrix<bool> mask(matrix.shape());

  grb::masked_view view(matrix, mask);

  mask[{2, 3}] = true;
  mask[{5, 5}] = true;
  mask[{9, 6}] = false;

  grb::print(matrix, "matrix");

  grb::print(view, "masked view");

  grb::print(matrix | grb::views::mask(mask), "adapted matrix");

  grb::matrix<float> a(matrix.shape());

  grb::assign(a, matrix | grb::views::mask(mask));

  grb::print(a, "A");

  /*
    for (size_t i = 0; i < matrix.shape()[0]; i += 4) {
      for (size_t j = 0; j < matrix.shape()[1]; j += 4) {
        std::cout << "Submatrix [" << i << ":" << i+4 << "," << j << ":" << j+4
    << "]\n"; grb::submatrix_view mat_sub(matrix, {i, i+4}, {j, j+4});
        grb::print(mat_sub);
      }
    }
    */

  return 0;
}
