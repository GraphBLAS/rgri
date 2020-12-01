
#pragma once

#include <vector>

namespace grb {

template <typename T, typename Allocator = std::allocator<T>>
using vector = std::vector<T, Allocator>;

} // end grb