#include <concepts>
#include <grb/containers/matrix.hpp>
#include <grb/containers/vector.hpp>
#include <grb/exceptions/exception.hpp>
#include <grb/util/matrix_hints.hpp>
#include <random>
#include <unordered_map>

namespace grb {

namespace {

template <typename T>
struct uniform_distribution {
  using type = std::uniform_int_distribution<T>;
};

template <std::floating_point T>
struct uniform_distribution<T> {
  using type = std::uniform_real_distribution<T>;
};

template <typename T>
using uniform_distribution_t = typename uniform_distribution<T>::type;

} // namespace

template <typename T = float, std::integral I = std::size_t,
          typename Hint = grb::sparse>
grb::matrix<T, I, Hint> generate_random(grb::index<I> shape,
                                        double density = 0.01,
                                        unsigned int seed = 0) {

  if (density > 1.0 || density < 0) {
    throw grb::invalid_argument("generate_random: invalid density argument.");
  }

  std::map<std::pair<I, I>, T> tuples;

  size_t nnz = density * shape[0] * shape[1];

  std::mt19937 gen(seed);
  std::uniform_int_distribution<I> row(0, shape[0] - 1);
  std::uniform_int_distribution<I> column(0, shape[1] - 1);

  uniform_distribution_t<T> values(0, 1);

  while (tuples.size() < nnz) {
    auto i = row(gen);
    auto j = column(gen);
    if (tuples.find({i, j}) == tuples.end()) {
      T value = values(gen);
      tuples.insert({{i, j}, value});
    }
  }

  grb::matrix<T, I, Hint> matrix(shape);
  matrix.insert(tuples.begin(), tuples.end());
  return matrix;
}

template <typename T = float, std::integral I = std::size_t,
          typename Hint = grb::sparse>
grb::vector<T, I, Hint> generate_random(I shape, double density = 0.01) {

  if (density > 1.0 || density < 0) {
    throw grb::invalid_argument("generate_random: invalid density argument.");
  }

  std::map<I, T> tuples;

  size_t nnz = density * shape;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<I> index(0, shape - 1);

  uniform_distribution_t<T> values(0, 1);

  while (tuples.size() < nnz) {
    auto i = index(gen);
    if (tuples.find(i) == tuples.end()) {
      T value = values(gen);
      tuples.insert({i, value});
    }
  }

  grb::vector<T, I, Hint> vector(shape);
  vector.insert(tuples.begin(), tuples.end());
  return vector;
}

} // namespace grb
