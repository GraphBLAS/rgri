#pragma once

#include <CL/sycl.hpp>
#include <type_traits>

template <typename T, std::size_t Alignment = 0>
requires(std::is_trivially_copyable_v<T>)
class device_allocator;

template <typename T>
requires(std::is_trivially_copyable_v<T> || std::is_void_v<T>)
class device_ref {
public:
  device_ref() = delete;
  ~device_ref() = default;
  device_ref(const device_ref&) = default;
  device_ref& operator=(const device_ref&) = default;

  device_ref(T* pointer) : pointer_(pointer) {}

  operator T() const {
    #ifdef __SYCL_DEVICE_ONLY__
      return *pointer_;
    #else
      cl::sycl::queue q;
      char buffer[sizeof(T)] __attribute__((aligned(sizeof(T))));
      q.memcpy(reinterpret_cast<T*>(buffer), pointer_, sizeof(T)).wait();
      return *reinterpret_cast<T*>(buffer);
    #endif
  }

  device_ref operator=(const T& value) {
    #ifdef __SYCL_DEVICE_ONLY__
      *pointer_ = value;
    #else
      cl::sycl::queue q;
      q.memcpy(pointer_, &value, sizeof(T)).wait();
    #endif
    return *this;
  }

private:
  T* pointer_;
};

template <typename T>
requires(std::is_trivially_copyable_v<T> || std::is_void_v<T>)
class device_ptr {
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = device_ptr<T>;
  using const_pointer = device_ptr<std::add_const_t<T>>;
  using nonconst_pointer = device_ptr<std::add_const_t<T>>;
  using iterator_category = std::random_access_iterator_tag;
  using reference = device_ref<T>;

  device_ptr(T* pointer) noexcept : pointer_(pointer) {}
  device_ptr() noexcept = default;
  ~device_ptr() noexcept = default;
  device_ptr(const device_ptr&) noexcept = default;
  device_ptr& operator=(const device_ptr&) noexcept = default;

  device_ptr(std::nullptr_t) noexcept : pointer_(nullptr) {}

  device_ptr& operator=(std::nullptr_t) noexcept {
    pointer_ = nullptr;
    return *this;
  }

  operator device_ptr<void>() const noexcept
  requires(!std::is_void_v<T>)
  {
    return device_ptr<void>(reinterpret_cast<void*>(pointer_));
  }

  operator device_ptr<const void>() const noexcept
  requires(!std::is_void_v<T>)
  {
    return device_ptr<const void>(reinterpret_cast<const void*>(pointer_));
  }

  operator const_pointer() const noexcept
  requires(!std::is_const_v<T>)
  {
    return const_pointer(pointer_);
  }

  bool operator==(std::nullptr_t) const noexcept { return pointer_ == nullptr; }
  bool operator!=(std::nullptr_t) const noexcept { return pointer_ != nullptr; }

  bool operator==(const device_ptr&) const noexcept = default;
  bool operator!=(const device_ptr&) const noexcept = default;

  pointer operator+(difference_type offset) const noexcept { return pointer(pointer_ + offset); }
  pointer operator-(difference_type offset) const noexcept { return pointer(pointer_ - offset); }

  difference_type operator-(const_pointer other) const noexcept
  requires(!std::is_const_v<T>)
  { return pointer_ - other.pointer_; }
  difference_type operator-(pointer other) const noexcept { return pointer_ - other.pointer_; }

  bool operator<(const_pointer other) const noexcept { return pointer_ < other.pointer_; }
  bool operator>(const_pointer other) const noexcept { return pointer_ > other.pointer_; }
  bool operator<=(const_pointer other) const noexcept { return pointer_ <= other.pointer_; }
  bool operator>=(const_pointer other) const noexcept { return pointer_ >= other.pointer_; }

  pointer& operator++() noexcept {
    ++pointer_;
    return *this;
  }

  pointer operator++(int) noexcept {
    pointer other = *this;
    ++(*this);
    return other;
  }

  pointer& operator--() noexcept {
    --pointer_;
    return *this;
  }

  pointer& operator--(int) noexcept {
    pointer other = *this;
    --(*this);
    return other;
  }

  pointer& operator+=(difference_type offset) noexcept {
    pointer_ += offset;
    return *this;
  }

  pointer& operator-=(difference_type offset) noexcept {
    pointer_ -= offset;
    return *this;
  }

  reference operator*() const noexcept {
    return reference(pointer_);
  }

  reference operator[](size_type offset) const noexcept {
    return reference(*(*this + offset));
  }

  friend pointer operator+(difference_type n, pointer iter) {
    return iter + n;
  }

  template <typename U, std::size_t Alignment>
  requires(std::is_trivially_copyable_v<U>)
  friend class device_allocator;

private:
  T* pointer_;
};

template <typename T, std::size_t Alignment>
requires(std::is_trivially_copyable_v<T>)
class device_allocator {
public:
  using value_type = T;
  using pointer = device_ptr<T>;
  using const_pointer = device_ptr<T>;
  using reference = device_ref<T>;
  using const_reference = device_ref<const T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  device_allocator(const cl::sycl::queue &q) noexcept : device_(q.get_device()), context_(q.get_context()) {}
  template <typename U>
  device_allocator(const device_allocator<U, Alignment>& other) noexcept : device_(device_), context_(context_) {}
  device_allocator(const device_allocator&) = default;
  device_allocator& operator=(const device_allocator&) = default;
  ~device_allocator() = default;

  using is_always_equal = std::false_type;

  pointer allocate(std::size_t size) {
    if constexpr(Alignment == 0) {
      return pointer(cl::sycl::malloc_device<T>(size, device_, context_));
    } else {
      return pointer(cl::sycl::aligned_alloc_device<T>(Alignment, size, device_, context_));
    }
  }

  void deallocate(pointer ptr, std::size_t n) {
    cl::sycl::free(ptr.pointer_, context_);
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
