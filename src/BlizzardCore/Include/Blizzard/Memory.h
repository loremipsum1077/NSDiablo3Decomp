// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Memory.h

#pragma once

#include <stddef.h>

namespace Blizzard {
namespace Memory {
enum {
    ZEROFILL = 1,
    NOTRACK = 2,
    DEBUGFILL = 4,
    INPLACEONLY = 8,
    PRIVATEBLK = 64,
    FREESTORE = 128,
};

class voidp_return {  // size 0x8
    void * ptr;  // 0x00
public:
    voidp_return(void *p) : ptr(p) {}
    // Not in the DWARF (a template); BitField.cpp assigns the result to a uint8 *.
    template <typename T> operator T *() const { return (T *)ptr; }
};
static_assert(sizeof(voidp_return) == 0x8, "voidp_return");

voidp_return RawAlloc(size_t size);  // _ZN8Blizzard6Memory8RawAllocEm
void RawFree(void * ptr);  // _ZN8Blizzard6Memory7RawFreeEPv
voidp_return Allocate(unsigned int size);  // _ZN8Blizzard6Memory8AllocateEj
voidp_return Allocate(unsigned int size, unsigned int flags);  // _ZN8Blizzard6Memory8AllocateEjj
void Free(void * ptr);  // _ZN8Blizzard6Memory4FreeEPv
// The parameter names of the two five-argument forms are invented (prototypes from the mangled names).
voidp_return Allocate(unsigned int size, unsigned int flags, const char * file, unsigned int line, const char * tag);  // _ZN8Blizzard6Memory8AllocateEjjPKcjS2_
void Free(void * ptr, unsigned int flags, const char * file, unsigned int line, const char * tag);  // _ZN8Blizzard6Memory4FreeEPvjPKcjS3_

}  // namespace Memory
}  // namespace Blizzard
