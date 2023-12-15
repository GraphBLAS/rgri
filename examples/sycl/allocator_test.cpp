#include <CL/sycl.hpp>
#include <grb/grb.hpp>
#include <span>

template <std::ranges::forward_range R>
void test_range(R&&) {}

template <std::random_access_iterator Iter>
void test_iter(Iter&&) {}

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;
  using namespace shp;

  sycl::queue q(select_device(sycl::gpu_selector()));

  device_allocator<int> a(q);

  vector<int, device_allocator<int>> vec(100, 2, a);

  grb::detail::spanner vec_view(vec);

  test_range(vec);

  for_each(
      vec_view, [](auto&& entry) { entry = entry + 2; }, q);
  print_range(vec, "vec");

  device_ptr<int> ptr = a.allocate(100);

  for (size_t i = 0; i < 100; i++) {
    ptr[i] = i;
  }

  q.parallel_for(sycl::range<1>(100), [=](sycl::id<1> id) {
     ptr[id] = 2 + int(ptr[id]);
   }).wait();

  print_range(grb::detail::spanner(ptr, 100));
  // grb::detail::spanner<device_ptr<int>> s;
  test_iter(device_ptr<int>());

  a.deallocate(ptr, 0);

  return 0;
}
