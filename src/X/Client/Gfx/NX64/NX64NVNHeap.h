// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/NX64/NX64NVNHeap.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../NintendoSDK/Include/nvn/nvn_Cpp.h"
#include "../../../Sigma/DataStructures/XMemoryAllocatorInterface.h"
#include "../../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class NX64NVNHeap : public XBaseHeap {  // size 0x58
    typedef uint64_t BlockFlags;
    struct Heap;
    struct Page {  // size 0x38
        nvn::MemoryPool * pMemoryPool;  // 0x00
        ptrdiff_t nOffset;  // 0x08
        uintptr_t uAddress;  // 0x10
        NX64NVNHeap::Heap * pHeap;  // 0x18
        NX64NVNHeap::Page * pNext;  // 0x20
        size_t uFreeBlockCount;  // 0x28
        NX64NVNHeap::BlockFlags * uBlocks;  // 0x30
    };
    struct Heap {  // size 0x30
        NX64NVNHeap::Page * pPages;  // 0x00
        size_t uPageSize;  // 0x08
        size_t uBlockSize;  // 0x10
        size_t uAllocMax;  // 0x18
        NX64NVNHeap::Heap * pNext;  // 0x20
        size_t uPageCount;  // 0x28
    };
    struct Stats {  // size 0x10
        size_t uTotalSize;  // 0x00
        size_t uUsedSize;  // 0x08
    };
    struct Block {  // size 0x18
        NX64NVNHeap::BlockFlags uFlagsMask;  // 0x00
        NX64NVNHeap::BlockFlags uFlagsShift;  // 0x08
        size_t uFlagsOffset;  // 0x10
    };
    static const NX64NVNHeap::BlockFlags BLOCK_FLAGS_FREE = 0;
    static const NX64NVNHeap::BlockFlags BLOCK_FLAGS_USED = 1;
    static const NX64NVNHeap::BlockFlags BLOCK_FLAGS_CONT = 3;
    nvn::Device * m_pDevice;  // 0x08
    nvn::MemoryPoolFlags m_tFlags;  // 0x10
    NX64NVNHeap::Heap * m_pHeaps;  // 0x18
    Blizzard::Lock::Mutex m_tMutex;  // 0x20
    NX64NVNHeap::Stats m_tStats;  // 0x40
    size_t m_uMaxSize;  // 0x50
    uintptr_t Alloc(size_t uSize, size_t uAlign, BOOL bTrack);
    void Free(uintptr_t uAddress, BOOL bTrack);
    uintptr_t AllocFromHeap(NX64NVNHeap::Heap * pHeap, size_t uSize, size_t uAlign, BOOL bTrack);
    uintptr_t AllocFromPage(NX64NVNHeap::Page * pPage, size_t uSize, size_t uAlign, BOOL bTrack);
    void FreeFromPage(NX64NVNHeap::Page * pPage, uintptr_t uAddress, BOOL bTrack);
    NX64NVNHeap::Page * AllocPage(NX64NVNHeap::Heap * pHeap);
    void FreePage(NX64NVNHeap::Page * pPage);
    NX64NVNHeap::Page * GetPageForAddress(const uintptr_t uAddress);
    BOOL CheckFreeBlocks(const NX64NVNHeap::Page *);
    size_t GetBlocksCount(const NX64NVNHeap::Heap * pHeap);
    size_t GetBlockFlagsCount(const NX64NVNHeap::Heap * pHeap);
    size_t GetBlockFlagsSize(const NX64NVNHeap::Heap * pHeap);
    size_t GetAllocBlockCount(const NX64NVNHeap::Heap * pHeap, const size_t uSize);
    NX64NVNHeap::Block GetBlock(const size_t uBlockIndex);
    NX64NVNHeap::BlockFlags GetBlockFlags(NX64NVNHeap::BlockFlags * pBlockFlags, const NX64NVNHeap::Block & tBlock);
    void SetBlockFlags(NX64NVNHeap::BlockFlags * pBlockFlags, const NX64NVNHeap::Block & tBlock, const NX64NVNHeap::BlockFlags uBlockFlags);
public:
    NX64NVNHeap(size_t, nvn::Device *, nvn::MemoryPoolFlags);
    virtual ~NX64NVNHeap();  // vtable slot 0
    virtual void * MemAlloc(size_t uSize, size_t uAlign, BOOL bClear);  // vtable slot 2
    virtual void * MemAllocTracked(size_t size, size_t align, BOOL bClear, const char * szFile, uint32 dwLine);  // vtable slot 3
    virtual void MemFree(void * pMem);  // vtable slot 4
    virtual void * MemResize(void * pMem, size_t size, size_t align);  // vtable slot 5
    virtual void * MemResizeTracked(void * pMem, size_t size, size_t align, const char * szFile, uint32 dwLine);  // vtable slot 6
    virtual BOOL ContainsMem(void * pMem);  // vtable slot 7
    virtual void PrintUsage();  // vtable slot 8
    virtual void Dump();  // vtable slot 11
    virtual size_t GetHeapSize() const;  // vtable slot 12
    virtual size_t GetUsedSize() const;  // vtable slot 13
    virtual size_t MemSize(void * pMem);  // vtable slot 9
    virtual void * MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char * szFile, uint32 dwLine);  // vtable slot 10
    void AddHeap(const size_t uPageSize, const size_t uBlockSize, const size_t uAllocMax);
    nvn::MemoryPool * GetMemoryPool(void * pMemory, ptrdiff_t * pOffset);
    void Preallocate(size_t uSize);
    void Test();
};
static_assert(sizeof(NX64NVNHeap) == 0x58, "NX64NVNHeap");

