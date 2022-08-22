#include <grb/grb.hpp>
#include <CL/sycl.hpp>
#include <span>
#include <fmt/core.h>
#include "sycl_help.hpp"

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;

  sycl::device d;

  try {
    d = sycl::device(sycl::accelerator_selector());
    std::cout << "Running on accelerator device \"" << d.get_info<sycl::info::device::name>() << "\"" << std::endl;
  } catch (sycl::exception const &e) {
    std::cout << "Cannot select an accelerator.  Message: \"" << e.what() << "\"" << std::endl;
    d = sycl::device(sycl::cpu_selector());
    std::cout << "Using CPU device \"" << d.get_info<sycl::info::device::name>() << "\"" << std::endl;
  }

  sycl::queue q(d);

  device_allocator<int> a(q);

  std::vector<int, device_allocator<int>> v(a);

  v.resize(100);

  return 0;
}