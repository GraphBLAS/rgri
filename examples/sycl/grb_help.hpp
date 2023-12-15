#pragma once

namespace grb {

template <typename M>
auto matrix_view(M&& matrix) {
  return grb::transpose(grb::transpose(std::forward<M>(matrix)));
}

template <typename Vector>
void initialize_random(Vector&& vector) {
  for (size_t i = 0; i < vector.size(); i++) {
    if (drand48() < 0.5) {
      vector[i] = lrand48() % 10;
    }
  }
}

} // namespace grb
