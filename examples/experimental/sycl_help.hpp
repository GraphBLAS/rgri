#pragma once

#include <CL/sycl.hpp>
#include "sycl_ptr.hpp"

template <typename T>
using shared_allocator = cl::sycl::usm_allocator<T, cl::sycl::usm::alloc::shared>;

template <typename T>
using shared_vector = std::vector<T, shared_allocator<T>>;

// template <typename T, std::integral I = std::size_t, typename Hint = grb::sparse>
// using shared_vector = grb::vector<T, I, Hint, shared_allocator<T>>;

template <typename T, std::integral I = std::size_t, typename Hint = grb::sparse>
using shared_matrix = grb::matrix<T, I, Hint, shared_allocator<T>>;

template <std::ranges::random_access_range R, typename Fn, typename Queue>
auto for_each_async(R&& r, Fn&& fn, Queue&& queue) {
  return queue.parallel_for(cl::sycl::range<1>(std::ranges::size(std::forward<R>(r))),
    [=](cl::sycl::id<1> id) {
      fn(*(std::ranges::begin(r)+id));
    });
}

template <std::ranges::random_access_range R, typename Fn, typename Queue>
void for_each(R&& r, Fn&& fn, Queue&& queue) {
  for_each_async(std::forward<R>(r), std::forward<Fn>(fn), std::forward<Queue>(queue)).wait();
}

template <typename Selector>
cl::sycl::device select_device(Selector&& selector) {
  cl::sycl::device d;

  try {
    d = cl::sycl::device(std::forward<Selector>(selector));
    std::cout << "Running on device \"" << d.get_info<cl::sycl::info::device::name>() << "\"" << std::endl;
  } catch (cl::sycl::exception const &e) {
    std::cout << "Cannot select an accelerator\n" << e.what() << "\n";
    std::cout << "Using a CPU device\n";
    d = cl::sycl::device(cl::sycl::cpu_selector());
  }
  return d;
}

template <typename Range>
void print_range(Range&& r, std::string label = "") {
  size_t indent = 1;

  if (label != "") {
    fmt::print("\"{}\": ", label);
    indent += label.size() + 4;
  }

  std::string indent_whitespace(indent, ' ');

  fmt::print("[");
  size_t columns = 10;
  size_t count = 1;
  for (auto iter = r.begin(); iter != r.end(); ++iter) {
    fmt::print("{}", static_cast<std::ranges::range_value_t<Range>>(*iter));

    auto next = iter;
    ++next;
    if (next != r.end()) {
      fmt::print(", ");
      if (count % columns == 0) {
        fmt::print("\n{}", indent_whitespace);
      }
    }
    ++count;
  }
  fmt::print("]\n");
}