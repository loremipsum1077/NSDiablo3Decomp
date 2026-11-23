// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMemory.inl


// Functions defined in this file (bodies are not in the DWARF):
//   line    10  bdAddr * bdAllocate<bdAddr>(const bdUWord n)  [inlined into 81 functions]
//   line    16  void bdDeallocate<unsigned char>(unsigned char * p)  [inlined into 109 functions]
//   line    22  bdDispatchInterceptor ** bdReallocate<bdDispatchInterceptor *>(bdDispatchInterceptor ** p, const bdUWord n)  [inlined into 4 functions]

#pragma once

#include "bdMemory-std.h"

// line 10
template<typename T>
inline T* bdAllocate(const bdUWord n)
{
    return static_cast<T*>(bdMemory::allocate(n * sizeof(T)));
}

// line 16
template<typename T>
inline void bdDeallocate(T* p)
{
    bdMemory::deallocate(p);
}

// line 22, observed by FastArray decreaseCapacity.
template<typename T> inline T * bdReallocate(T * p, const bdUWord n) { return static_cast<T *>(bdMemory::reallocate(p, n * sizeof(T))); }
