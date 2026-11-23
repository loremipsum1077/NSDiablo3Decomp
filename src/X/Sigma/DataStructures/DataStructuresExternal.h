#pragma once

// Not an original file. Gathers the headers outside X/Sigma/DataStructures that
// this directory needs, plus the helpers that have no DWARF name (XASSERT,
// XFREE) and the NX SDK <math.h> isfinite() workaround.

#include "../Cookie.h"
#include "../ErrorManager/ErrorManager.h"
#include "../Memory/SigmaMemory.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"

#include <stdlib.h>

class XMemoryAllocatorInterface;

// The assert that survives in the shipping build. Its name is not in the
// DWARF; the file name it passes is an empty string in the binary.
#ifndef XASSERT
#define XASSERT(expr)                                                        \
    do {                                                                     \
        if (!(expr)) {                                                       \
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);  \
            ExitProgram(ERROR_INTERNAL_FATAL);                               \
        }                                                                    \
    } while (0)
#endif

// Frees through SigmaMemoryFree and nulls the pointer; the binary always shows
// the two together. Name not in the DWARF.
#ifndef XFREE
#define XFREE(p, ptMemoryInterface)                  \
    do {                                             \
        SigmaMemoryFree((p), (ptMemoryInterface));   \
        (p) = NULL;                                  \
    } while (0)
#endif

// NX SDK <math.h>
#include <cmath>
#ifdef MATCHING_HACK_NX_CLANG
// The NX SDK's libc++ implements isfinite() with the bit test from its (musl)
// math.h. The toolchain image's libc++ uses the compiler builtin, which
// compiles differently, so the SDK's version is reproduced here until the SDK
// libc headers are available.
// Shared guard permits the original SDK owning header to supply this helper.
#ifndef D3_NX_FLOAT_BITS_DEFINED
#define D3_NX_FLOAT_BITS_DEFINED
static inline unsigned __FLOAT_BITS(float __f) {
    union {
        float __f;
        unsigned __i;
    } __u;
    __u.__f = __f;
    return __u.__i;
}
#endif
#undef isfinite
#define isfinite(x) ((__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000)
#else
using std::isfinite;
#endif
