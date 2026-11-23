// Original file: Contrib/Contrib/Domino/Common/dmStackAllocator.cpp
#include "dmStackAllocator.h"
#include "../Include/dmMath.h"
#include "../Include/dmSettings.h"
#include <stdint.h>
#include <string.h>

#line 50 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
void dmStackAllocator::Create(dm_int32 capacity)
{
    m_pData = (char *)dmAlloc(capacity);

    m_entryCapacity = 64;
    m_aEntries = (Entry *)dmAlloc(m_entryCapacity * sizeof(Entry));
    m_freeByte = 0;
    m_allocation = 0;
    m_maxAllocation = 0;
    m_capacity = capacity;
    m_entryCount = 0;
    m_fixedSize = false;
}

#line 79 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
void dmStackAllocator::Destroy()
{
    dmAssert(m_freeByte == 0);
    dmAssert(m_entryCount == 0);

    if (m_fixedSize == false)
    {
        dmFree(m_aEntries);
        dmFree(m_pData);
    }

    m_aEntries = NULL;
    m_pData = NULL;
}

#line 93 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
void * dmStackAllocator::Allocate(dm_int32 size)
{
    if (m_fixedSize)
    {
        dm_int32 sizeEntry16 = (sizeof(Entry) + 15) & ~15;
        dm_int32 size16 = (size + 15) & ~15;

        dmAssert(m_freeByte + size16 + sizeEntry16 <= m_capacity);

        Entry * entry = (Entry *)(m_pData + m_freeByte);
        entry->size = size16;
        entry->pData = m_pData + m_freeByte + sizeEntry16;
        entry->usedMalloc = false;
        m_freeByte += size16 + sizeEntry16;

        m_allocation += size16 + sizeEntry16;
        m_maxAllocation = dmMax(m_maxAllocation, m_allocation);
        ++m_entryCount;

        dmAssert(((uintptr_t)entry->pData & 0xF) == 0x0);
        return entry->pData;
    }

    if (m_entryCount == m_entryCapacity)
    {
#line 120 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
        m_entryCapacity *= 2;
        Entry * pNewEntries = (Entry *)dmAlloc(m_entryCapacity * sizeof(Entry));
        memcpy(pNewEntries, m_aEntries, m_entryCount * sizeof(Entry));
        dmFree(m_aEntries);
        m_aEntries = pNewEntries;
    }

    dm_int32 size16 = (size + 15) & ~15;
    Entry * entry = m_aEntries + m_entryCount;
    entry->size = size16;
    if (m_freeByte + size16 > m_capacity)
    {
        static bool once = false;
        dmWarn("The Domino stack capacity of %d bytes exceeded. Contact ecatto@blizzard.com", m_capacity);
        if (once == false)
        {
            dmLog("The Domino stack capacity of %d bytes exceeded. Contact ecatto@blizzard.com\n", m_capacity);
            once = true;
        }

#line 142 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
        entry->pData = (char *)dmAlloc(size);
        entry->usedMalloc = true;
    }
    else
    {
        entry->pData = m_pData + m_freeByte;
        entry->usedMalloc = false;
        m_freeByte += size16;
    }

    m_allocation += size16;
    m_maxAllocation = dmMax(m_maxAllocation, m_allocation);
    ++m_entryCount;

#line 156 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
    dmAssert(((uintptr_t)entry->pData & 0xF) == 0x0);
    return entry->pData;
}

#line 163 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
void dmStackAllocator::Free(void * p)
{
    dmAssert(m_entryCount > 0);

    if (m_fixedSize)
    {
        Entry * entry = (Entry *)((char *)p - 16);
        dmAssert(p == entry->pData);
        m_freeByte -= entry->size + 16;
        m_allocation -= entry->size + 16;
        --m_entryCount;
        return;
    }
#line 176 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Common/dmStackAllocator.cpp"
    Entry * entry = m_aEntries + m_entryCount - 1;
    dmAssert(p == entry->pData);
    if (entry->usedMalloc)
    {
        dmFree(p);
    }
    else
    {
        m_freeByte -= entry->size;
    }
    m_allocation -= entry->size;
    --m_entryCount;
}
