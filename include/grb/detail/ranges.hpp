#pragma once

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201911L &&                \
    defined(__cpp_lib_ranges_zip) && __cpp_lib_ranges_zip >= 202110L

#include <ranges>

namespace grb {

namespace __ranges = std::ranges;

}

#elif __has_include(<range/v3/all.hpp>)

#include <range/v3/all.hpp>

namespace grb {

namespace __ranges = ::ranges;

}

#else
static_assert(false, "rgri with binsparse requires support for std::ranges.  "
                     "Compile with C++23 or later.");

#endif
