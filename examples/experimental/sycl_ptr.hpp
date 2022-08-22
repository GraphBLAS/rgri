#include <CL/sycl.hpp>
#include <type_traits>

template <typename T, std::size_t Alignment = 0>
requires(std::is_trivially_copyable_v<T>)
class device_allocator {
public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  device_allocator(const cl::sycl::queue &q) noexcept : device_(q.get_device()), context_(q.get_context()) {}
  template <typename U>
  device_allocator(const device_allocator<U, Alignment>& other) noexcept : device_(device_), context_(context_) {}
  device_allocator(const device_allocator&) = default;
  device_allocator& operator=(const device_allocator&) = default;
  ~device_allocator() = default;

  using is_always_equal = std::false_type;

  T* allocate(std::size_t size) {
    if constexpr(Alignment == 0) {
      return cl::sycl::malloc_device<T>(size, device_, context_);
    } else {
      return cl::sycl::aligned_alloc_device<T>(Alignment, size, device_, context_);
    }
  }

  void deallocate(pointer ptr, std::size_t n) {
    cl::sycl::free(ptr, context_);
  }

  bool operator==(const device_allocator&) const = default;
  bool operator!=(const device_allocator&) const = default;

  template <typename U>
  struct rebind {
    using other = device_allocator<U, Alignment>;
  };

private:
  cl::sycl::device device_;
  cl::sycl::context context_;
};
