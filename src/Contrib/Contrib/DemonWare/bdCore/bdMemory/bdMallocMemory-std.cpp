// Original file: Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMallocMemory-std.cpp
#include "bdMallocMemory-std.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

// Defined in bdAlignedOffsetMemory.cpp; no header for them is in the DWARF.
void * bdAlignedOffsetMalloc(const bdUWord size, const bdUWord align, const bdUWord offset);
void bdAlignedOffsetFree(void * p);
void * bdAlignedOffsetRealloc(void * p, const bdUWord origSize, const bdUWord size, const bdUWord align, const bdUWord offset);
















bdMallocMemory::bdMemoryChainElement * bdMallocMemory::m_memoryChain = NULL;
bdUWord bdMallocMemory::m_allocatedBytes = 0;
bdUInt bdMallocMemory::m_numAllocations = 0;

bdMutex bdMallocMemory::m_mutex;

void * bdMallocMemory::allocate(const bdUWord size)
{
    const bdUWord totalSize = size + sizeof(bdMemoryChainElement);


    bdMemoryChainElement * link = static_cast<bdMemoryChainElement *>(bdAlignedOffsetMalloc(totalSize, 8, sizeof(bdMemoryChainElement)));
    return recordMemory(link, size, false);
}


void bdMallocMemory::deallocate(void * p)
{
    if (p)
    {
        bdMemoryChainElement * link = static_cast<bdMemoryChainElement *>(p) - 1;
        eraseMemory(link);





        bdAlignedOffsetFree(link);
    }
}

void * bdMallocMemory::reallocate(void * p,
                                  const bdUWord size)
{
    if (p)
    {
        bdMemoryChainElement * link = static_cast<bdMemoryChainElement *>(p) - 1;
        const bdUWord origSize = link->m_size + sizeof(bdMemoryChainElement);
        eraseMemory(link);

        const bdUWord totalSize = size + sizeof(bdMemoryChainElement);


        link = static_cast<bdMemoryChainElement *>(bdAlignedOffsetRealloc(link, origSize, totalSize, 8, sizeof(bdMemoryChainElement)));
        return recordMemory(link, size, false);
    }
    else
    {
        return allocate(size);
    }

}

void * bdMallocMemory::alignedAllocate(const bdUWord size,
                                       const bdUWord align)
{
    const bdUWord totalSize = size + sizeof(bdMemoryChainElement);



    bdMemoryChainElement * link = static_cast<bdMemoryChainElement *>(bdAlignedOffsetMalloc(totalSize, align, sizeof(bdMemoryChainElement)));
    return recordMemory(link, size, true);
}


void bdMallocMemory::alignedDeallocate(void * p)
{
    if (p)
    {
        bdMemoryChainElement * link = static_cast<bdMemoryChainElement *>(p) - 1;
        eraseMemory(link);





        bdAlignedOffsetFree(link);
    }
}


void * bdMallocMemory::alignedReallocate(void * p,
                                         const bdUWord size,
                                         const bdUWord align)
{
    if (p)
    {
        bdMemoryChainElement * link = static_cast<bdMemoryChainElement *>(p) - 1;
        const bdUWord origSize = link->m_size + sizeof(bdMemoryChainElement);
        eraseMemory(link);

        const bdUWord totalSize = size + sizeof(bdMemoryChainElement);

        link = static_cast<bdMemoryChainElement *>(bdAlignedOffsetRealloc(link, origSize, totalSize, align, sizeof(bdMemoryChainElement)));
        return recordMemory(link, size, true);
    }
    else
    {
        return alignedAllocate(size, align);
    }

}

bdBool bdMallocMemory::leakCheck()
{

    if (m_numAllocations != 0 && m_allocatedBytes != 0)
    {
        m_mutex.lock();
        bdNChar8 buffer[100];
        bdSnprintf(buffer, sizeof(buffer), "%u Bytes leaked in %u allocation(s)\n", m_allocatedBytes, m_numAllocations);
        m_mutex.unlock();
        bdFileHandle stream = bdGetStderr();
        bdFprintf(stream, "********************************************\n");
        bdFprintf(stream, "*      BITDEMON MEMORY LEAKS DETECTED      *\n");
        bdFprintf(stream, "********************************************\n");
        bdFprintf(stream, buffer);
        bdFprintf(stream, "********************************************\n");
        return false;
    }









    return true;
}
















inline void * bdMallocMemory::recordMemory(bdMallocMemory::bdMemoryChainElement * link, const bdUWord size, const bdBool aligned)
{
    void * ret = NULL;

    if (link)
    {
        m_mutex.lock();
        link->m_magic = 0xBDBD;
        link->m_size = size;
        link->m_aligned = aligned;
        link->m_next = m_memoryChain;
        link->m_prev = NULL;
        if (m_memoryChain)
        {
            m_memoryChain->m_prev = link;
        }
        m_memoryChain = link;

        m_allocatedBytes += size;
        ++m_numAllocations;

        m_mutex.unlock();
        ret = link + 1;
    }
    return ret;
}


inline void bdMallocMemory::eraseMemory(bdMallocMemory::bdMemoryChainElement * link)
{
    m_mutex.lock();
    if (link->m_magic != 0xBDBD)
    {

        m_mutex.unlock();
        bdLogError("mallocmemory", "Memory header corrupt or memory not allocated by bdMallocMemory");
        m_mutex.lock();
    }

    if (link->m_prev)
    {
        link->m_prev->m_next = link->m_next;
    }
    else
    {
        m_memoryChain = link->m_next;
    }

    if (link->m_next)
    {
        link->m_next->m_prev = link->m_prev;
    }

    m_allocatedBytes -= link->m_size;
    --m_numAllocations;
    m_mutex.unlock();
}
