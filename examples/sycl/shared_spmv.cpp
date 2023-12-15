#include "grb_help.hpp"
#include <CL/sycl.hpp>
#include <grb/grb.hpp>
#include <span>

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;
  using grb::detail::spanner;

  using namespace shp;

  sycl::queue q(select_device(sycl::gpu_selector()));

  shared_allocator<int> allocator(q);

  shared_matrix<int> a("../data/chesapeake.mtx", allocator);

  shared_vector<int> x(a.shape()[1], allocator);
  shared_vector<int> b(a.shape()[0], allocator);

  grb::initialize_random(x);

  spanner a_view(a);
  spanner x_view(x);
  spanner b_view(b);

  print_range(x_view, "a");

  using device_atomic_ref = sycl::atomic_ref<int, sycl::memory_order::relaxed,
                                             sycl::memory_scope::device>;

  for_each(
      a_view,
      [=](auto&& entry) {
        auto&& [index, value] = entry;
        auto&& [i, j] = index;
        device_atomic_ref b_value(b_view[i]);
        b_value += value * x_view[j];
      },
      q);

  print_range(b_view, "b");

  return 0;
}
