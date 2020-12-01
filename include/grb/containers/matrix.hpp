
#pragma once

#include <cstdint>
#include <memory>

namespace grb {

template <typename T,
          typename I = std::uint64_t,
          typename Allocator = std::allocator<T>>
class matrix {
public:
  using value_type = T;
  using index_type = I;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  matrix() = default;

  std::array<size_type, 2> shape() const noexcept {
    return {m_, n_};
  }

private:
  size_t m_ = 0;
  size_t n_ = 0;
};

} // end grb
