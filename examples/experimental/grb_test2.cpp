#include <grb/grb.hpp>
#include <CL/sycl.hpp>
#include <span>
#include <fmt/core.h>
#include "sycl_help.hpp"
#include "grb_help.hpp"

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;

  sycl::queue q(select_device(sycl::gpu_selector()));

  shared_allocator<int> allocator(q);

  shared_matrix<int> a("../data/chesapeake.mtx", allocator);

  shared_vector<int> x(a.shape()[1], allocator);
  shared_vector<int> b(a.shape()[0], allocator);

  grb::initialize_random(x);

  spanner a_view(a);
  spanner x_view(x);
  spanner b_view(b);

  grb::matrix_view a_view(a);

  print_range(x_view, "a");

  for_each(a_view, [=](auto&& entry) {
                     auto&& [index, value] = entry;
                     auto&& [i, j] = index;
                     b_view[i] += value * x_view[j];
                    }, q);

  print_range(b_view, "b");

  return 0;
}



