// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMallocMemory-std.h

#pragma once

#include "../bdThread/bdMutex.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdMallocMemory {  // size 0x1
public:
    static void * allocate(const bdUWord size);
    static void deallocate(void * p);
    static void * reallocate(void * p, const bdUWord size);
    static void * alignedAllocate(const bdUWord size, const bdUWord align);
    static void alignedDeallocate(void * p);
    static void * alignedReallocate(void * p, const bdUWord size, const bdUWord align);
    static bdBool leakCheck();
    static void releaseAllMemory();
    struct bdMemoryChainElement {  // size 0x28
        bdUInt16 m_magic;  // 0x00
        bdUWord m_size;  // 0x08
        bdBool m_aligned;  // 0x10
        bdMallocMemory::bdMemoryChainElement * m_prev;  // 0x18
        bdMallocMemory::bdMemoryChainElement * m_next;  // 0x20
    };
private:
    static void * recordMemory(bdMallocMemory::bdMemoryChainElement * link, const bdUWord size, const bdBool aligned);
    static void eraseMemory(bdMallocMemory::bdMemoryChainElement * link);
    bdMallocMemory();
    static bdMallocMemory::bdMemoryChainElement * m_memoryChain;
public:
    static bdUWord m_allocatedBytes;
    static bdUInt m_numAllocations;
    static bdMutex m_mutex;
};
static_assert(sizeof(bdMallocMemory) == 0x1, "bdMallocMemory");

