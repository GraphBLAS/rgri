#pragma once

namespace shp {

template <std::ranges::random_access_range R, typename Fn, typename Queue>
auto for_each_async(R&& r, Fn&& fn, Queue&& queue) {
  return queue.parallel_for(
      cl::sycl::range<1>(std::ranges::size(std::forward<R>(r))),
      [=](cl::sycl::id<1> id) { fn(*(std::ranges::begin(r) + id)); });
}

template <std::ranges::random_access_range R, typename Fn, typename Queue>
void for_each(R&& r, Fn&& fn, Queue&& queue) {
  for_each_async(std::forward<R>(r), std::forward<Fn>(fn),
                 std::forward<Queue>(queue))
      .wait();
}

} // namespace shp
