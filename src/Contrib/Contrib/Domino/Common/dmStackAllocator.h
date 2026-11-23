// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Common/dmStackAllocator.h

#pragma once

#include "../Include/dmSerialize.h"

class dmStackAllocator {  // size 0x30
public:
    dmStackAllocator();
    ~dmStackAllocator();
    void Create(dm_int32 capacity);
    void CreateFixed(dm_int32, void *);
    void Destroy();
    void * Allocate(dm_int32 size);
    void Free(void * p);
#line 64
    dm_int32 GetAllocation() const
    {
        return m_allocation;
    }
#line 18
    dm_int32 GetMaxAllocation() const;
    dm_int32 GetBytes() const;
private:
    struct Entry {  // size 0x10
        char * pData;  // 0x00
        dm_int32 size;  // 0x08
        bool usedMalloc;  // 0x0c
    };
    bool m_fixedSize;  // 0x00
    char * m_pData;  // 0x08
    dm_int32 m_freeByte;  // 0x10
    dm_int32 m_allocation;  // 0x14
    dm_int32 m_maxAllocation;  // 0x18
    dm_int32 m_capacity;  // 0x1c
    dmStackAllocator::Entry * m_aEntries;  // 0x20
    dm_int32 m_entryCount;  // 0x28
    dm_int32 m_entryCapacity;  // 0x2c
};
static_assert(sizeof(dmStackAllocator) == 0x30, "dmStackAllocator");


// Functions defined in this file (bodies are not in the DWARF):
//   line    64  dm_int32 dmStackAllocator::GetAllocation() const  [inlined into 1 functions]
//   line    69  dm_int32 dmStackAllocator::GetMaxAllocation() const  [inline]
