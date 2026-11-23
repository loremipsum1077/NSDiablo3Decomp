#pragma once

#include "config.h"
#include "type_traits.h"

namespace blz {

// line 17. The DWARF names the temporary `c` (swap<int>, swap<Score>), and
// for a type without a move constructor the binary shows a copy and two
// copy assignments at lines 19-21.
template <typename T>
inline void swap(T& a, T& b) {
    T c(blz::move(a));
    a = blz::move(b);
    b = blz::move(c);
}

template <typename T, size_t N>
inline void swap(T (&a)[N], T (&b)[N]) {
    for (size_t i = 0; i < N; ++i)
        swap(a[i], b[i]);
}

}  // namespace blz
