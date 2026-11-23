#pragma once

// Recovers the enclosing object from a pointer to one of its members (used by
// the intrusive containers). In the binary it is a single subtraction of the
// member offset (line 17), with no null test.

#include "config.h"

namespace blz {

// line 15
template <typename T, typename M>
inline T* object_from_member(M* member, const M T::* ptr_to_member) {
    return reinterpret_cast<T*>(reinterpret_cast<char*>(member) -
                                reinterpret_cast<size_t>(&(static_cast<T*>(0)->*ptr_to_member)));
}

template <typename T, typename M>
inline const T* object_from_member(const M* member, const M T::* ptr_to_member) {
    return reinterpret_cast<const T*>(reinterpret_cast<const char*>(member) -
                                      reinterpret_cast<size_t>(&(static_cast<const T*>(0)->*ptr_to_member)));
}

}  // namespace blz
