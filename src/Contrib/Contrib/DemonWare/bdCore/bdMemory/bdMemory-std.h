// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMemory-std.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdMemory {  // size 0x1
    typedef void *(*bdAllocateFunc)(const bdUWord);
    typedef void (*bdDeallocateFunc)(void *);
    typedef void *(*bdReallocateFunc)(void *, const bdUWord);
    typedef void *(*bdAlignedAllocateFunc)(const bdUWord, const bdUWord);
    typedef void (*bdAlignedDeallocateFunc)(void *);
    typedef void *(*bdAlignedReallocateFunc)(void *, const bdUWord, const bdUWord);
public:
    static void setAllocateFunc(const bdMemory::bdAllocateFunc allocator);
    static void setDeallocateFunc(const bdMemory::bdDeallocateFunc deallocator);
    static void setReallocateFunc(const bdMemory::bdReallocateFunc reallocator);
    static void setAlignedAllocateFunc(const bdMemory::bdAlignedAllocateFunc allocator);
    static void setAlignedDeallocateFunc(const bdMemory::bdAlignedDeallocateFunc deallocator);
    static void setAlignedReallocateFunc(const bdMemory::bdAlignedReallocateFunc reallocator);
    static bdMemory::bdAllocateFunc getAllocateFunc();
    static bdMemory::bdDeallocateFunc getDeallocateFunc();
    static bdMemory::bdReallocateFunc getReallocateFunc();
    static bdMemory::bdAlignedAllocateFunc getAlignedAllocateFunc();
    static bdMemory::bdAlignedDeallocateFunc getAlignedDeallocateFunc();
    static bdMemory::bdAlignedReallocateFunc getAlignedReallocateFunc();
    static void * allocate(const bdUWord size);
    static void deallocate(void * p);
    static void * reallocate(void * p, const bdUWord size);
    static void * alignedAllocate(const bdUWord, const bdUWord);
    static void alignedDeallocate(void * p);
    static void * alignedReallocate(void * p, const bdUWord size, const bdUWord align);
protected:
    static bdMemory::bdAllocateFunc m_allocateFunc;
    static bdMemory::bdDeallocateFunc m_deallocateFunc;
    static bdMemory::bdReallocateFunc m_reallocateFunc;
    static bdMemory::bdAlignedAllocateFunc m_alignedAllocateFunc;
    static bdMemory::bdAlignedDeallocateFunc m_alignedDeallocateFunc;
    static bdMemory::bdAlignedReallocateFunc m_alignedReallocateFunc;
private:
    bdMemory();
};
static_assert(sizeof(bdMemory) == 0x1, "bdMemory");


// Class-scope allocation operators. Every bd class records the same eight static operators on
// one line of its header (bdReferencable.h:34, bdReference.h:28, ...), so they come from a
// macro; only `operator delete(void* p)` has an inlined copy (it calls bdMemory::deallocate).
// The macro name and the other seven bodies are not in the DWARF (invented).
#include <stddef.h>
#define BD_CLASS_ALLOCATOR(className) \
    static void* operator new(size_t nbytes) { return bdMemory::allocate(nbytes); } \
    static void operator delete(void* p) { bdMemory::deallocate(p); } \
    static void* operator new[](size_t nbytes) { return bdMemory::allocate(nbytes); } \
    static void operator delete[](void* p) { bdMemory::deallocate(p); } \
    static void* operator new(size_t, void* p) { return p; } \
    static void operator delete(void*, void*) {} \
    static void* operator new[](size_t, void* p) { return p; } \
    static void operator delete[](void*, void*) {}
