#include "NX64NVNHeap.h"
#include "../../../Sigma/Memory/SigmaMemory.h"

#include "../../../../NintendoSDK/Include/nn/nn_Result.h"
namespace nn {









namespace os {
Result AllocateAddressRegion(uintptr_t* pOutAddress, size_t size);
Result AllocateMemoryPages(uintptr_t address, size_t size);
Result FreeMemoryPages(uintptr_t address, size_t size);
Result FreeAddressRegion(uintptr_t address);
}  // namespace os
}  // namespace nn

NX64NVNHeap::NX64NVNHeap(size_t uMaxSize, nvn::Device * pDevice, nvn::MemoryPoolFlags tFlags)
    : m_pDevice(pDevice)
    , m_tFlags(tFlags)
    , m_pHeaps(NULL)
    , m_tMutex()
    , m_tStats()
    , m_uMaxSize(uMaxSize)
{
    Blizzard::Lock::MutexCreate(m_tMutex);
}

NX64NVNHeap::~NX64NVNHeap()
{
    Blizzard::Lock::MutexDestroy(m_tMutex);
}

void * NX64NVNHeap::MemAlloc(size_t uSize, size_t uAlign, BOOL bClear)
{
    uintptr_t uAddress;
    void * pMemory;

    Blizzard::Lock::ScopedMutex lock(m_tMutex);
    uAddress = Alloc(uSize, uAlign, TRUE);
    pMemory = (void *)uAddress;

    if (bClear && pMemory)
    {
        SigmaMemoryClear(pMemory, uSize);
    }

    return pMemory;
}

void * NX64NVNHeap::MemAllocTracked(size_t size, size_t align, BOOL bClear, const char * szFile, uint32 dwLine)
{
    return MemAlloc(size, align, bClear);
}

// NON_MATCHING: in the inlined GetBlockFlags the operands of one `and` are commuted
// (x14, x17 for x17, x14); FreePage, which inlines the same FreeFromPage, matches.
// Rewriting GetBlockFlags (operand order, single-expression local, and-then-return) changes nothing.
#line 58
void NX64NVNHeap::MemFree(
    void * pMem)
{
    Blizzard::Lock::ScopedMutex lock(m_tMutex);
    uintptr_t uAddress = (uintptr_t)pMem;
    Free(uAddress, TRUE);
}
#line 70

void * NX64NVNHeap::MemResize(void * pMem, size_t size, size_t align)
{
    return NULL;
}

void * NX64NVNHeap::MemResizeTracked(void * pMem, size_t size, size_t align, const char * szFile, uint32 dwLine)
{
    return MemResize(pMem, size, align);
}

BOOL NX64NVNHeap::ContainsMem(void * pMem)
{
    Blizzard::Lock::ScopedMutex lock(m_tMutex);
    uintptr_t uAddress = (uintptr_t)pMem;
    Page * pPage = GetPageForAddress(uAddress);
    return pPage != NULL;
}

void NX64NVNHeap::PrintUsage()
{
}

void NX64NVNHeap::Dump()
{
}

size_t NX64NVNHeap::GetHeapSize() const
{
    return m_tStats.uTotalSize;
}

size_t NX64NVNHeap::GetUsedSize() const
{
    return m_tStats.uUsedSize;
}

size_t NX64NVNHeap::MemSize(void * pMem)
{
    return 0;
}

void * NX64NVNHeap::MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char * szFile, uint32 dwLine)
{
    return MemAlloc(size, align, bClear);
}

void NX64NVNHeap::AddHeap(const size_t uPageSize, const size_t uBlockSize, const size_t uAllocMax)
{
    Heap * pHeap = (Heap *)SigmaMemoryNew(sizeof(Heap), 4, NULL, 0);
    SigmaMemoryClear(pHeap, sizeof(Heap));
    pHeap->uPageSize = uPageSize;
    pHeap->uBlockSize = uBlockSize;
    pHeap->uAllocMax = uAllocMax;

    Heap ** pInsert;
    for (pInsert = &m_pHeaps; *pInsert; pInsert = &(*pInsert)->pNext)
    {
        if ((*pInsert)->uAllocMax > uAllocMax)
        {
            break;
        }
    }

    pHeap->pNext = *pInsert;
    *pInsert = pHeap;
}

nvn::MemoryPool * NX64NVNHeap::GetMemoryPool(void * pMemory, ptrdiff_t * pOffset)
{
    uintptr_t uAddress = (uintptr_t)pMemory;
    Page * pPage = GetPageForAddress(uAddress);
    if (pPage == NULL)
    {
        return NULL;
    }

    if (pOffset)
    {
        *pOffset = pPage->nOffset + (uAddress - pPage->uAddress);
    }

    return pPage->pMemoryPool;
}

void NX64NVNHeap::Preallocate(size_t uSize)
{
    Heap * pLast = m_pHeaps;
    while (pLast->pNext)
    {
        pLast = pLast->pNext;
    }

    while (pLast->uPageSize <= uSize)
    {
        AllocPage(pLast);
        uSize -= pLast->uPageSize;
    }
}

inline uintptr_t NX64NVNHeap::Alloc(size_t uSize, size_t uAlign, BOOL bTrack)
{
    for (Heap * pHeap = m_pHeaps; pHeap; pHeap = pHeap->pNext)
    {
        if (uSize < pHeap->uAllocMax && uAlign <= pHeap->uBlockSize)
        {
            return AllocFromHeap(pHeap, uSize, uAlign, bTrack);
        }
    }

    return 0;
}

inline void NX64NVNHeap::Free(uintptr_t uAddress, BOOL bTrack)
{
    Page * pPage = GetPageForAddress(uAddress);
    if (pPage)
    {
        FreeFromPage(pPage, uAddress, bTrack);
    }
}

inline uintptr_t NX64NVNHeap::AllocFromHeap(Heap * pHeap, size_t uSize, size_t uAlign, BOOL bTrack)
{
    for (Page * pPage = pHeap->pPages; pPage; pPage = pPage->pNext)
    {
        uintptr_t uAddress = AllocFromPage(pPage, uSize, uAlign, bTrack);
        if (uAddress)
        {
            return uAddress;
        }
    }

    Page * pPage = AllocPage(pHeap);
    if (pPage == NULL)
    {
        return 0;
    }

    return AllocFromPage(pPage, uSize, uAlign, bTrack);
}

uintptr_t NX64NVNHeap::AllocFromPage(Page * pPage, size_t uSize, size_t uAlign, BOOL bTrack)
{
    Heap * pHeap = pPage->pHeap;
    const size_t uAllocBlockCount = GetAllocBlockCount(pHeap, uSize);
    if (pPage->uFreeBlockCount < uAllocBlockCount)
    {
        return 0;
    }

    size_t uBlockIndex = 0;
    size_t uBlockCount = 0;
    const size_t uMaxBlocks = GetBlocksCount(pHeap);
    while (uAllocBlockCount != uBlockCount && uBlockIndex < uMaxBlocks)
    {
        const Block tBlock = GetBlock(uBlockIndex);
        const BlockFlags uBlockFlags = GetBlockFlags(pPage->uBlocks, tBlock);
        if (uBlockFlags == BLOCK_FLAGS_FREE)
        {
            uBlockCount++;
        }
        else
        {
            uBlockCount = 0;
        }
        uBlockIndex++;
    }

    if (uAllocBlockCount == uBlockCount)
    {
        BlockFlags uFlags = BLOCK_FLAGS_USED;
        uBlockIndex -= uAllocBlockCount;
        for (size_t i = 0; i != uAllocBlockCount; i++)
        {
            const Block tBlock = GetBlock(uBlockIndex + i);
            SetBlockFlags(pPage->uBlocks, tBlock, uFlags);
            uFlags = BLOCK_FLAGS_CONT;
        }

        pPage->uFreeBlockCount -= uAllocBlockCount;
        if (bTrack)
        {
            m_tStats.uUsedSize += pHeap->uBlockSize * uAllocBlockCount;
        }

        const uintptr_t uAddress = pPage->uAddress + uBlockIndex * pPage->pHeap->uBlockSize;
        return uAddress;
    }

    return 0;
}

inline void NX64NVNHeap::FreeFromPage(Page * pPage, uintptr_t uAddress, BOOL bTrack)
{
    Heap * pHeap = pPage->pHeap;
    BlockFlags uFlags = BLOCK_FLAGS_USED;
    size_t uBlockCount = 0;
    size_t uBlockIndex = (uAddress - pPage->uAddress) / pHeap->uBlockSize;
    const size_t uMaxBlocks = GetBlocksCount(pHeap);
    while (uBlockIndex < uMaxBlocks)
    {
        const Block tBlock = GetBlock(uBlockIndex);
        const BlockFlags uBlockFlags = GetBlockFlags(pPage->uBlocks, tBlock);
        if (uBlockFlags != uFlags)
        {
            break;
        }

        SetBlockFlags(pPage->uBlocks, tBlock, BLOCK_FLAGS_FREE);
        uFlags = BLOCK_FLAGS_CONT;
        uBlockIndex++;
        uBlockCount++;
    }

    pPage->uFreeBlockCount += uBlockCount;
    if (bTrack)
    {
        m_tStats.uUsedSize -= pHeap->uBlockSize * uBlockCount;
    }

    if (pHeap->pNext && pPage->uFreeBlockCount == uMaxBlocks)
    {
        FreePage(pPage);
    }
}

NX64NVNHeap::Page * NX64NVNHeap::AllocPage(Heap * pHeap)
{
    if (pHeap->pNext == NULL && m_uMaxSize < m_tStats.uTotalSize + pHeap->uPageSize)
    {
        return NULL;
    }

    const size_t uAllocSize = sizeof(Page) + GetBlockFlagsSize(pHeap);
    Page * pPage = (Page *)SigmaMemoryNew(uAllocSize, 8, NULL, 0);
    if (pPage == NULL)
    {
        return NULL;
    }

    SigmaMemoryClear(pPage, uAllocSize);
    pPage->uBlocks = (BlockFlags *)(pPage + 1);

    if (pHeap->pNext)
    {
        pPage->uAddress = Alloc(pHeap->uPageSize, pHeap->uBlockSize, FALSE);
        if (pPage->uAddress == 0)
        {
            SigmaMemoryFree(pPage, NULL);
            return NULL;
        }

        pPage->pMemoryPool = GetMemoryPool((void *)pPage->uAddress, &pPage->nOffset);
    }
    else
    {
        nvn::MemoryPoolBuilder builder;
        nn::Result tResult = nn::os::AllocateAddressRegion(&pPage->uAddress, pHeap->uPageSize);
        if (tResult.IsFailure())
        {
            SigmaMemoryFree(pPage, NULL);
            return NULL;
        }

        tResult = nn::os::AllocateMemoryPages(pPage->uAddress, pHeap->uPageSize);
        if (tResult.IsFailure())
        {
            nn::os::FreeAddressRegion(pPage->uAddress);
            SigmaMemoryFree(pPage, NULL);
            return NULL;
        }

        builder.SetDefaults();
        builder.SetDevice(m_pDevice);
        builder.SetFlags(m_tFlags);
        builder.SetStorage((void *)pPage->uAddress, pHeap->uPageSize);

        pPage->pMemoryPool = new nvn::MemoryPool;
        if (!pPage->pMemoryPool->Initialize(&builder))
        {
            delete pPage->pMemoryPool;
            nn::os::FreeMemoryPages(pPage->uAddress, pHeap->uPageSize);
            nn::os::FreeAddressRegion(pPage->uAddress);
            SigmaMemoryFree(pPage, NULL);
        }

        m_tStats.uTotalSize += pHeap->uPageSize;
    }

    pPage->pHeap = pHeap;
    pPage->uFreeBlockCount = GetBlocksCount(pHeap);

    Page ** pLast = &pHeap->pPages;
    while (*pLast)
    {
        pLast = &(*pLast)->pNext;
    }
    *pLast = pPage;
    pHeap->uPageCount++;

    return pPage;
}

void NX64NVNHeap::FreePage(Page * pPage)
{
    Page ** pPrev = &pPage->pHeap->pPages;
    Heap * pHeap = pPage->pHeap;
    while (*pPrev != pPage)
    {
        pPrev = &(*pPrev)->pNext;
    }
    *pPrev = pPage->pNext;

    if (pHeap->pNext)
    {
        Free(pPage->uAddress, FALSE);
    }
    else
    {
        pPage->pMemoryPool->Finalize();
        delete pPage->pMemoryPool;
        nn::Result tResult = nn::os::FreeMemoryPages(pPage->uAddress, pHeap->uPageSize);
        tResult = nn::os::FreeAddressRegion(pPage->uAddress);
        m_tStats.uTotalSize -= pHeap->uPageSize;
    }

    SigmaMemoryFree(pPage, NULL);
    pHeap->uPageCount--;
}

inline NX64NVNHeap::Page * NX64NVNHeap::GetPageForAddress(const uintptr_t uAddress)
{
    for (Heap * pHeap = m_pHeaps; pHeap; pHeap = pHeap->pNext)
    {
        for (Page * pPage = pHeap->pPages; pPage; pPage = pPage->pNext)
        {
            if (uAddress >= pPage->uAddress && uAddress < pPage->uAddress + pHeap->uPageSize)
            {
                return pPage;
            }
        }
    }

    return NULL;
}

inline size_t NX64NVNHeap::GetBlocksCount(const Heap * pHeap)
{
    size_t uBlocksCount = pHeap->uPageSize / pHeap->uBlockSize;
    return uBlocksCount;
}

inline size_t NX64NVNHeap::GetBlockFlagsCount(const Heap * pHeap)
{
    const size_t uBlocksCount = GetBlocksCount(pHeap);
    return (uBlocksCount + 31) / 32;
}

inline size_t NX64NVNHeap::GetBlockFlagsSize(const Heap * pHeap)
{
    size_t uBlockFlagsSize = GetBlockFlagsCount(pHeap) * sizeof(BlockFlags);
    return uBlockFlagsSize;
}

inline size_t NX64NVNHeap::GetAllocBlockCount(const Heap * pHeap, const size_t uSize)
{
    size_t uAllocBlockCount = (uSize + pHeap->uBlockSize - 1) / pHeap->uBlockSize;
    return uAllocBlockCount;
}

inline NX64NVNHeap::Block NX64NVNHeap::GetBlock(const size_t uBlockIndex)
{
    Block block;
    block.uFlagsShift = (uBlockIndex % 32) * 2;
    block.uFlagsMask = BLOCK_FLAGS_CONT << block.uFlagsShift;
    block.uFlagsOffset = uBlockIndex / 32;
    return block;
}

inline NX64NVNHeap::BlockFlags NX64NVNHeap::GetBlockFlags(BlockFlags * pBlockFlags, const Block & tBlock)
{
    const BlockFlags uBlockFlags = pBlockFlags[tBlock.uFlagsOffset];
    return (uBlockFlags & tBlock.uFlagsMask) >> tBlock.uFlagsShift;
}

inline void NX64NVNHeap::SetBlockFlags(BlockFlags * pBlockFlags, const Block & tBlock, const BlockFlags uBlockFlags)
{
    pBlockFlags[tBlock.uFlagsOffset] = (uBlockFlags << tBlock.uFlagsShift) | (pBlockFlags[tBlock.uFlagsOffset] & ~tBlock.uFlagsMask);
}
