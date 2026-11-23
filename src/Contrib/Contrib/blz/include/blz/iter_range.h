#pragma once

// Free begin/end. The container versions are recorded at lines 60-81 and
// return Cont::iterator / Cont::const_iterator.

#include "config.h"

namespace blz {

template <typename T, size_t N>
inline T* begin(T (&a)[N]) {
    return a;
}

template <typename T, size_t N>
inline T* end(T (&a)[N]) {
    return a + N;
}

// line 60
template <typename Cont>
inline typename Cont::iterator begin(Cont& c) {
    return c.begin();
}

// line 67
template <typename Cont>
inline typename Cont::const_iterator begin(const Cont& c) {
    return c.begin();
}

// line 74
template <typename Cont>
inline typename Cont::iterator end(Cont& c) {
    return c.end();
}

// line 81
template <typename Cont>
inline typename Cont::const_iterator end(const Cont& c) {
    return c.end();
}

}  // namespace blz
