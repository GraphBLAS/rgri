#include <grb/grb.hpp>
#include <CL/sycl.hpp>
#include <span>
#include <fmt/core.h>

template <typename T>
using shared_allocator = cl::sycl::usm_allocator<T, cl::sycl::usm::alloc::shared>;

template <typename T>
using shared_vector = std::vector<T, shared_allocator<T>>;

template <typename T, std::integral I = std::size_t, typename Hint = grb::sparse>
using shared_matrix = grb::matrix<T, I, Hint, shared_allocator<T>>;

template <std::ranges::random_access_range R, typename Fn, typename Queue>
auto for_each_async(R&& r, Fn&& fn, Queue&& queue) {
  return queue.parallel_for(cl::sycl::range<1>(std::ranges::size(std::forward<R>(r))),
    [=](cl::sycl::id<1> id) {
      fn(*(r.begin()+id));
    });
}

template <std::ranges::random_access_range R, typename Fn, typename Queue>
void for_each(R&& r, Fn&& fn, Queue&& queue) {
  for_each_async(std::forward<R>(r), std::forward<Fn>(fn), std::forward<Queue>(queue)).wait();
}

int main(int argc, char** argv) {
  namespace sycl = cl::sycl;

  sycl::device d;

  try {
    d = sycl::device(sycl::cpu_selector());
    std::cout << "Running on device \"" << d.get_info<sycl::info::device::name>() << "\"" << std::endl;
  } catch (sycl::exception const &e) {
    std::cout << "Cannot select an accelerator\n" << e.what() << "\n";
    std::cout << "Using a CPU device\n";
    d = sycl::device(sycl::cpu_selector());
  }

  sycl::queue q(d);
  shared_allocator<bool> allocator(q);

  shared_matrix<bool> m("../data/chesapeake.mtx", allocator);

  for (auto&& [index, value] : m) {
    auto&& [i, j] = index;
    if (i % 2 == 0) {
      value = true;
    } else {
      value = false;
    }
  }

  using grb::detail::spanner;

  spanner m_view(m);

  grb::print(m, "Original values");

  for_each(spanner(m), [=](auto&& entry) {
                     auto&& [index, value] = entry;
                     auto&& [i, j] = index;
                     if (i == 38 && j == 7) {
                       value = 0;
                     }
                    }, q);

  grb::print(m, "After assigning");

  for (auto&& [index, value] : m) {
    auto&& [i, j] = index;
    if (i % 2 == 0 && bool(value) != true) {
      fmt::print("Error: {}, {}: {} is wrong.\n", i, j, bool(value));
    } else if (i % 2 == 1 && bool(value) == true) {
      fmt::print("Error: {}, {}: {} is wrong.\n", i, j, bool(value));
    }
  }

  return 0;
}



