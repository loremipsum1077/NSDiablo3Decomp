// Original file: Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMemory.cpp

#include "bdMemory-std.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

#include <string.h>

bdMemory::bdAllocateFunc bdMemory::m_allocateFunc = 0;
bdMemory::bdDeallocateFunc bdMemory::m_deallocateFunc = 0;
bdMemory::bdReallocateFunc bdMemory::m_reallocateFunc = 0;
bdMemory::bdAlignedAllocateFunc bdMemory::m_alignedAllocateFunc = 0;
bdMemory::bdAlignedDeallocateFunc bdMemory::m_alignedDeallocateFunc = 0;
bdMemory::bdAlignedReallocateFunc bdMemory::m_alignedReallocateFunc = 0;

void bdMemory::setAllocateFunc(const bdAllocateFunc allocator)
{
    m_allocateFunc = allocator;
}

void bdMemory::setDeallocateFunc(const bdDeallocateFunc deallocator)
{
    m_deallocateFunc = deallocator;
}

void bdMemory::setReallocateFunc(const bdReallocateFunc reallocator)
{
    m_reallocateFunc = reallocator;
}

void bdMemory::setAlignedAllocateFunc(const bdAlignedAllocateFunc allocator)
{
    m_alignedAllocateFunc = allocator;
}

void bdMemory::setAlignedDeallocateFunc(const bdAlignedDeallocateFunc deallocator)
{
    m_alignedDeallocateFunc = deallocator;
}

void bdMemory::setAlignedReallocateFunc(const bdAlignedReallocateFunc reallocator)
{
    m_alignedReallocateFunc = reallocator;
}

bdMemory::bdAllocateFunc bdMemory::getAllocateFunc()
{
    return m_allocateFunc;
}

bdMemory::bdDeallocateFunc bdMemory::getDeallocateFunc()
{
    return m_deallocateFunc;
}

bdMemory::bdReallocateFunc bdMemory::getReallocateFunc()
{
    return m_reallocateFunc;
}

// The failed-allocation path is a trap that returns (trap word, then `ret`), so it is a
// breakpoint and not an abort. The original macro is not in the DWARF (a bdAssert of some kind).
void* bdMemory::allocate(const bdUWord size)
{
    if (m_allocateFunc == 0)
        return 0;
    void* block = m_allocateFunc(size);
    if (block == 0)
        __builtin_debugtrap();
    return block;
}

void bdMemory::deallocate(void* p)
{
    if (m_deallocateFunc != 0)
        m_deallocateFunc(p);
}

void* bdMemory::reallocate(void* p, const bdUWord size)
{
    if (m_reallocateFunc == 0)
        return 0;
    void* block = m_reallocateFunc(p, size);
    if (block == 0)
        __builtin_debugtrap();
    return block;
}

void bdMemory::alignedDeallocate(void* p)
{
    if (m_alignedDeallocateFunc != 0)
        m_alignedDeallocateFunc(p);
}

void* bdMemory::alignedReallocate(void* p, const bdUWord size, const bdUWord align)
{
    if (m_alignedReallocateFunc == 0)
        return 0;
    void* block = m_alignedReallocateFunc(p, size, align);
    if (block == 0)
        __builtin_debugtrap();
    return block;
}

extern "C" void* libTomCryptMalloc(size_t n)
{
    return bdMemory::allocate(n);
}

extern "C" void* libTomCryptRealloc(void* p, size_t n)
{
    return bdMemory::reallocate(p, n);
}

extern "C" void* libTomCryptCalloc(size_t n, size_t s)
{
    void* const p = bdMemory::allocate(s * n);
    if (p != 0)
        bdMemset(p, 0, s * n);
    return p;
}

extern "C" void libTomCryptFree(void* p)
{
    bdMemory::deallocate(p);
}
