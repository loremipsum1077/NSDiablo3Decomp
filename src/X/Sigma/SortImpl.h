// Original file: X/Sigma/SortImpl.h. Recovered from UIConsoleInventoryFilters.cpp DWARF and code.
#pragma once
#include <stddef.h>
#include "Memory/SigmaMemory.h"
namespace {





















template <typename T, typename Compare>
void Merge(
    T *pBase,
    size_t nMiddle,
    size_t nCount,
    T *pBuffer,
    Compare pCompare)
{
    SigmaMemoryCopy(pBuffer, pBase, sizeof(T) * nMiddle);

    size_t nLeft = 0;
    size_t nRight = nMiddle;
    size_t nDst = 0;

    for (; nLeft < nMiddle && nRight < nCount; ++nDst)
    {
        T &a = pBuffer[nLeft];
        T &b = pBase[nRight];
        if (pCompare(a, b) > 0)
        {
            pBase[nDst] = b;
            ++nRight;
        }
        else
        {
            pBase[nDst] = a;
            ++nLeft;
        }
    }

    if (nLeft < nMiddle)
    {
        SigmaMemoryCopy(pBase + nDst, pBuffer + nLeft, sizeof(T) * (nMiddle - nLeft));
    }
}




template <typename T, typename Compare>
void StableIndexSortImpl(
    T *pArray,
    size_t nCount,
    T *pBuffer,
    Compare pCompare)
{
    if (nCount < 2)
    {
        // Inference: the base-case branch returns through the original epilogue.
        return;
    }
    size_t nMiddle = nCount / 2;

    StableIndexSortImpl(pArray, nMiddle, pBuffer, pCompare);
    StableIndexSortImpl(pArray + nMiddle, nCount - nMiddle, pBuffer, pCompare);
    Merge(pArray, nMiddle, nCount, pBuffer, pCompare);
}
} // anonymous namespace

