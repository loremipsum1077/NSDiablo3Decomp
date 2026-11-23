#pragma once

#include "../SigmaTypes.h"

class XMemoryAllocatorInterface {
public:










    virtual ~XMemoryAllocatorInterface() {}

    virtual void* MemAlloc(size_t size, size_t align, BOOL bClear) = 0;
    virtual void* MemAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine);
    virtual void MemFree(void* pMem) = 0;
    virtual void* MemResize(void* pMem, size_t size, size_t align) = 0;


























    virtual void* MemResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
        return MemResize(pMem, size, align);
    }
    virtual BOOL ContainsMem(void* pMem) = 0;
    virtual void PrintUsage() = 0;
};

class XBaseHeap : public XMemoryAllocatorInterface {
public:
    virtual ~XBaseHeap() {}

    virtual size_t MemSize(void* pMem) = 0;
    virtual void* MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) = 0;
    virtual void Dump();
    virtual size_t GetHeapSize() const;
    virtual size_t GetUsedSize() const;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XMemoryAllocatorInterface) == 0x8, "XMemoryAllocatorInterface");
static_assert(sizeof(XBaseHeap) == 0x8, "XBaseHeap");
