// Original file: X/Sigma/Sort.h. Recovered from UIConsoleInventoryFilters.cpp DWARF and code.
#pragma once
#include <stddef.h>
#include "Memory/SigmaMemory.h"
#include "SortImpl.h"
namespace UIInventory { namespace Console { struct ACDIDInfo; } }

namespace Sort {





















template <typename T, typename Compare>
void StableIndexSort(
    T *pArray,
    size_t nCount,
    T *pBuffer,
    size_t nBufferCount,
    Compare pCompare)
{
    // The original optimized wrapper does not inspect nBufferCount.
    StableIndexSortImpl(pArray, nCount, pBuffer, pCompare);
}







template <typename T>
class StableIndexSorter
{
public:

    StableIndexSorter() : m_pBuffer(NULL), m_nLen(0) {}



    explicit StableIndexSorter(size_t);








    template <typename Compare>
    void Sort(
        T *pArray,
        size_t nCount,
        Compare pCompare)
    {
        if (nCount == 0)
        {
            // Inference: the original zero-count branch skips all allocation and sorting.
            return;
        }
        size_t nBufferLen = nCount / 2 + 1;

        if (!m_pBuffer)
        { // Inference: original alignment4 for ACDIDInfo is alignof(T).
            m_pBuffer = static_cast<T *>(SigmaMemoryNew(sizeof(T) * nBufferLen, alignof(T), NULL, FALSE));
            m_nLen = nBufferLen;
        }
        else if (m_nLen < nCount)
        { // Inference: original alignment4 for ACDIDInfo is alignof(T).
            m_pBuffer = static_cast<T *>(SigmaMemoryResize(m_pBuffer, NULL, sizeof(T) * nBufferLen, alignof(T)));
            m_nLen = nBufferLen;
        }


        if (!m_pBuffer)
        {
            m_nLen = 0; // Inference: allocation failure returns without sorting.
            return;
        }

        StableIndexSort(pArray, nCount, m_pBuffer, m_nLen, pCompare);
    }

private:
    T *m_pBuffer;
    size_t m_nLen;
};
} // namespace Sort
static_assert(sizeof(Sort::StableIndexSorter<UIInventory::Console::ACDIDInfo>) == 0x10, "StableIndexSorter");
