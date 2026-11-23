// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Common/dmPoolAllocator.h

#pragma once

#include "../Include/dmSerialize.h"

class dmPoolAllocator {  // size 0x88
public:
    dmPoolAllocator();
    ~dmPoolAllocator();
    void Create();
    void Destroy();
    void * Allocate(dm_int32 size, bool allowHeap);
    void Free(void * pMemory, dm_int32 size, bool allowHeap);
    void Clear();
    dm_int32 GetBytes() const;
    dm_int32 GetFreeBytes() const;
private:
    enum {
        e_chunkSize = 32512,
        e_maxObjectSize = 1280,
        e_objectSizes = 15,
        e_chunkArrayIncrement = 128,
    };
    struct Object {  // size 0x8
        dmPoolAllocator::Object * pNext;  // 0x00
    };
    struct Chunk {  // size 0x10
        dmPoolAllocator::Object * aObjects;  // 0x00
        dm_int32 objectSize;  // 0x08
        dm_int32 objectCount;  // 0x0c
    };
    dmPoolAllocator::Chunk * m_aChunks;  // 0x00
    dm_int32 m_chunkCount;  // 0x08
    dm_int32 m_chunkCapacity;  // 0x0c
    dmPoolAllocator::Object * m_aFreeLists[15];  // 0x10
    static dm_int32 s_objectSizes[15];
    static dm_uint8 s_objectSizeLookup[1281];
    static bool s_objectSizeLookupInitialized;
};
static_assert(sizeof(dmPoolAllocator) == 0x88, "dmPoolAllocator");

