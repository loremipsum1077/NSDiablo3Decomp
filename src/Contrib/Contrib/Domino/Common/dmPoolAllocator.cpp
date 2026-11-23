// Original file: Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp
#include "dmPoolAllocator.h"
#include "../Include/dmSettings.h"
#include <string.h>

dm_int32 dmPoolAllocator::s_objectSizes[e_objectSizes] = {
    16, 32, 64, 96, 128, 160, 192, 224, 256, 320, 384, 448, 512, 768, 1280,
};
dm_uint8 dmPoolAllocator::s_objectSizeLookup[e_maxObjectSize + 1];
bool dmPoolAllocator::s_objectSizeLookupInitialized;

#line 41 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
dm_int32 s_aFreeCounts[15];

#line 60 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
void dmPoolAllocator::Create()
{
    m_chunkCount = 0;
    m_chunkCapacity = e_chunkArrayIncrement;

#line 66 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
    m_aChunks = (Chunk *)dmAlloc(m_chunkCapacity * sizeof(Chunk));

    memset(m_aChunks, 0, m_chunkCapacity * sizeof(Chunk));
    memset(m_aFreeLists, 0, sizeof(m_aFreeLists));
    memset(s_aFreeCounts, 0, sizeof(s_aFreeCounts));

    if (s_objectSizeLookupInitialized == false)
    {
        dm_int32 j = 0;
        for (dm_int32 i = 1; i <= e_maxObjectSize; ++i)
        {
#line 78 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
            dmAssert(j < e_objectSizes);
            if (i <= s_objectSizes[j])
            {
                s_objectSizeLookup[i] = (dm_uint8)j;
            }
            else
            {
                ++j;
                s_objectSizeLookup[i] = (dm_uint8)j;
            }
        }

        s_objectSizeLookupInitialized = true;
    }
}

#line 95 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
void dmPoolAllocator::Destroy()
{
    for (dm_int32 i = 0; i < m_chunkCount; ++i)
    {
        dmFree(m_aChunks[i].aObjects);
    }

    dmFree(m_aChunks);
    m_aChunks = NULL;
}

#line 107 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
void * dmPoolAllocator::Allocate(dm_int32 size, bool allowHeap)
{
    if (size == 0)
    {
        return NULL;
    }

    if (size > e_maxObjectSize && allowHeap)
    {
#line 116 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
        return dmAlloc(size);
    }

    dmAssert(0 < size && size <= e_maxObjectSize);

    dm_int32 index = s_objectSizeLookup[size];
    dmAssert(0 <= index && index < e_objectSizes);

    if (m_aFreeLists[index])
    {
        Object * pObject = m_aFreeLists[index];
        m_aFreeLists[index] = pObject->pNext;
        s_aFreeCounts[index] -= 1;
        return pObject;
    }
    else
    {
        if (m_chunkCount == m_chunkCapacity)
        {
            Chunk * pOldChunks = m_aChunks;
            m_chunkCapacity += e_chunkArrayIncrement;
#line 139 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
            m_aChunks = (Chunk *)dmAlloc(m_chunkCapacity * sizeof(Chunk));
            memcpy(m_aChunks, pOldChunks, m_chunkCount * sizeof(Chunk));
            memset(m_aChunks + m_chunkCount, 0, e_chunkArrayIncrement * sizeof(Chunk));
            dmFree(pOldChunks);
        }

        dm_int32 objectSize = s_objectSizes[index];
        dm_int32 objectCount = e_chunkSize / objectSize;

        Chunk * pChunk = m_aChunks + m_chunkCount;
        ++m_chunkCount;
        pChunk->objectSize = objectSize;
        pChunk->objectCount = objectCount;
#line 156 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
        pChunk->aObjects = (Object *)dmAlloc(objectCount * objectSize);
Object * pObject = pChunk->aObjects;
        for (dm_int32 i = 0; i < objectCount - 1; ++i)
        {
            Object * pNext = (Object *)((dm_int8 *)pObject + objectSize);
            pObject->pNext = pNext;
            pObject = pNext;
        }
        pObject->pNext = NULL;

        m_aFreeLists[index] = pChunk->aObjects->pNext;
        s_aFreeCounts[index] = objectCount - 1;

        return pChunk->aObjects;
    }
}

#line 185 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
void dmPoolAllocator::Free(void * pMemory, dm_int32 size, bool allowHeap)
{
    if (size == 0)
    {
        return;
    }

    if (size > e_maxObjectSize && allowHeap)
    {
        dmFree(pMemory);
        return;
    }

#line 198 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
    dmAssert(0 < size && size <= e_maxObjectSize);

    dm_int32 index = s_objectSizeLookup[size];
#line 203 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmPoolAllocator.cpp"
    dmAssert(0 <= index && index < e_objectSizes);

    Object * pObject = (Object *)pMemory;
    pObject->pNext = m_aFreeLists[index];
    m_aFreeLists[index] = pObject;
    s_aFreeCounts[index] += 1;
}
