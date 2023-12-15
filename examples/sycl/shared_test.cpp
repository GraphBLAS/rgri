#include <CL/sycl.hpp>
#include <grb/grb.hpp>
#include <span>

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;

  using namespace shp;

  sycl::queue q(select_device(sycl::gpu_selector()));

  shared_allocator<int> allocator(q);
  std::vector<int, shared_allocator<int>> v(10, allocator);

  for (size_t i = 0; i < v.size(); i++) {
    v[i] = i;
  }

  print_range(v);

  std::span view(v);

  q.parallel_for(cl::sycl::range<1>(v.size()), [=](cl::sycl::id<1> i) {
     view[i] += 1;
   }).wait();

  print_range(v);

  return 0;
}
