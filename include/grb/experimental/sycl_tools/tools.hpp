#pragma once

#include <CL/sycl.hpp>
#include <iostream>

namespace shp {

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
    std::cout << "\"" << label << "\": ";
    indent += label.size() + 4;
  }

  std::string indent_whitespace(indent, ' ');

  std::cout << "[";
  size_t columns = 10;
  size_t count = 1;
  for (auto iter = r.begin(); iter != r.end(); ++iter) {
    std::cout << static_cast<std::ranges::range_value_t<Range>>(*iter);

    auto next = iter;
    ++next;
    if (next != r.end()) {
      std::cout << ", ";
      if (count % columns == 0) {
        std::cout << "\n" << indent_whitespace;
      }
    }
    ++count;
  }
  std::cout << std::endl;
}

} // end shp