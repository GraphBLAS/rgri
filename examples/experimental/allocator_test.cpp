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

  using vector_type = std::vector<int, device_allocator<int>>;

  device_ptr<int> ptr = a.allocate(100);

  for (size_t i = 0; i < 100; i++) {
    ptr[i] = i;
  }

  q.parallel_for(sycl::range<1>(100), [=](sycl::id<1> id) {
                                      ptr[id] = 2 + int(ptr[id]);
                                    }).wait();

  for (size_t i = 0; i < 100; i++) {
    int v = ptr[i];
    printf("%d\n", v);
  }

  return 0;
}