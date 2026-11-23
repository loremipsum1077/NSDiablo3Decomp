// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformMemory/bdPlatformMemory.inl


// Functions defined in this file (bodies are not in the DWARF):
//   line    13  void * bdMalloc(bdUWord size)  [inlined into 2 functions]
//   line    20  void bdFree(void * p)  [inlined into 2 functions]
//   line    42  void * bdMemcpy(void *const dest, const void *const src, const bdUWord len)  [inlined into 88 functions]
//   line    50  bdInt bdMemcmp(const void *const s1, const void *const s2, const bdUWord len)  [inlined into 11 functions]
//   line    58  void * bdMemmove(void *const dest, const void *const src, const bdUWord len)  [inlined into 7 functions]
//   line    66  void * bdMemset(void *const s, const bdInt c, const bdUWord len)  [0x7100b22bf0, inlined into 76 functions]

#pragma once

#include <string.h>
#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

// line 42 (body invented: the minimal wrapper over memcpy).
inline void* bdMemcpy(void* const dest, const void* const src, const bdUWord len)
{
    return memcpy(dest, src, len);
}

// line 66 (body is the minimal wrapper over memset).
inline void* bdMemset(void* const s, const bdInt c, const bdUWord len)
{
    return memset(s, c, len);
}

// line 58 (body invented: the minimal wrapper over memmove).
inline void* bdMemmove(void* const dest, const void* const src, const bdUWord len)
{
    return memmove(dest, src, len);
}

// line 50; minimal wrapper, recovered from bdCookie::deserialize.
inline bdInt bdMemcmp(const void* const s1, const void* const s2, const bdUWord len)
{
    return memcmp(s1, s2, len);
}
