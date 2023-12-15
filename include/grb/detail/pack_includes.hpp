#pragma once

namespace grb {

template <typename...>
struct pack_includes_impl_;

template <typename T, typename U, typename... Us>
struct pack_includes_impl_<T, U, Us...> {
  static constexpr bool value =
      std::is_same<T, U>::value || pack_includes_impl_<T, Us...>::value;
};

template <typename T, typename U>
struct pack_includes_impl_<T, U> {
  static constexpr bool value = std::is_same<T, U>::value;
};

template <typename T>
struct pack_includes_impl_<T> {
  static constexpr bool value = false;
};

// check if Ts includes the type T
template <typename T, typename... Ts>
struct pack_includes {
  static constexpr bool value = pack_includes_impl_<T, Ts...>::value;
};

} // namespace grb
