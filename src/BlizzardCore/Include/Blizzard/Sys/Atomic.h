// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Sys/Atomic.h

#pragma once

#include "../Types.h"

// The bodies are not in the DWARF. Callers in X/Common/Definitions/Definitions.cpp show a
// sequentially consistent ldaxr/stlxr loop for each, which is what the __sync builtins give.
// Increment and Decrement compare the old value against an adjusted constant in their callers,
// so the new value is formed with a plain (signed) add after the fetch.
namespace Blizzard {
namespace Lock {
namespace Atomic {

inline bool CompareAndSwap(int32 expectedValue, int32 newValue, volatile int32 * pointer) {  // line 115
    return __sync_bool_compare_and_swap(pointer, expectedValue, newValue);
}

inline int32 Increment(volatile int32 * value) {  // line 121
    const int32 result = __sync_fetch_and_add(value, 1) + 1;
    return result;
}

inline int32 Decrement(volatile int32 * value) {  // line 127
    const int32 result = __sync_fetch_and_sub(value, 1) - 1;
    return result;
}

inline int32 And(int32 mask, volatile int32 * pointer) {  // line 133
    return __sync_fetch_and_and(pointer, mask);
}

inline int32 Or(int32 bits, volatile int32 * pointer) {  // line 139
    return __sync_fetch_and_or(pointer, bits);
}

inline void ReadWriteBarrier() {  // line 145
    __sync_synchronize();
}

}  // namespace Atomic
}  // namespace Lock
}  // namespace Blizzard


// Functions defined in this file (bodies are not in the DWARF):
//   line   115  bool Blizzard::Lock::Atomic::CompareAndSwap(int32 expectedValue, int32 newValue, volatile int32 * pointer)  [inlined into 23 functions]
//   line   121  int32 Blizzard::Lock::Atomic::Increment(volatile int32 * value)  [inlined into 250 functions]
//   line   127  int32 Blizzard::Lock::Atomic::Decrement(volatile int32 * value)  [inlined into 542 functions]
//   line   133  int32 Blizzard::Lock::Atomic::And(int32 mask, volatile int32 * pointer)  [inlined into 12 functions]
//   line   139  int32 Blizzard::Lock::Atomic::Or(int32 bits, volatile int32 * pointer)  [inlined into 6 functions]
//   line   145  void Blizzard::Lock::Atomic::ReadWriteBarrier()  [inlined into 6 functions]
