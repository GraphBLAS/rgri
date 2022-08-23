#include <grb/grb.hpp>
#include <CL/sycl.hpp>
#include <span>
#include <fmt/core.h>
#include "sycl_help.hpp"

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;


  sycl::queue q(select_device(sycl::gpu_selector()));

  shared_allocator<int> allocator(q);
  std::vector<int, shared_allocator<int>> v(10, allocator);

  for (size_t i = 0; i < v.size(); i++) {
    v[i] = i;
  }

  print_range(v);

  std::span view(v);

  q.parallel_for(v.size(),
                 [=](auto&& i) {
                   view[i] += 1;
                 }).wait();

  print_range(v);

  return 0;
}
