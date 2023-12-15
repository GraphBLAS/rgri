#pragma once

#include <CL/sycl.hpp>
#include <grb/grb.hpp>
#include <vector>

#include "allocators.hpp"

namespace shp {

template <typename T>
using shared_allocator =
    cl::sycl::usm_allocator<T, cl::sycl::usm::alloc::shared>;

template <typename T>
using shared_vector = std::vector<T, shared_allocator<T>>;

// template <typename T, std::integral I = std::size_t, typename Hint =
// grb::sparse> using shared_vector = grb::vector<T, I, Hint,
// shared_allocator<T>>;

template <typename T, std::integral I = std::size_t,
          typename Hint = grb::sparse>
using shared_matrix = grb::matrix<T, I, Hint, shared_allocator<T>>;

template <typename T, std::integral I = std::size_t,
          typename Hint = grb::sparse>
using device_matrix = grb::matrix<T, I, Hint, device_allocator<T>>;

} // namespace shp
