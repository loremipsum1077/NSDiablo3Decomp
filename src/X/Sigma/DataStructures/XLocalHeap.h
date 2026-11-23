#pragma once

#include "DataStructuresExternal.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "XMemoryAllocatorInterface.h"





















struct XLocalHeapNode {
    union {
        XLocalHeapNode* ptNextFreeNode;
        const char* szFile;
    };
    union {
        XLocalHeapNode* ptPrevFreeNode;
        uint32 dwLine;
    };
    XLocalHeapNode* ptPrevNode;
    size_t dwUsed : 1;
    size_t dwSize : 63;

    XLocalHeapNode* GetNextNode() const;
};



inline XLocalHeapNode* XLocalHeapNode::GetNextNode() const {
    return (XLocalHeapNode*)((size_t)(this + 1) + dwSize);
}





class XLocalHeap : public XBaseHeap {
public:
    XLocalHeap(void* pHeapBuffer, size_t dwHeapBufferSize, size_t dwMinAllocSize, size_t dwMaxAllocSize);
    virtual ~XLocalHeap();

    virtual void* MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine);
    virtual void* MemAlloc(size_t size, size_t align, BOOL bClear);
    virtual void* MemAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine);
    virtual void MemFree(void* pMem);
    virtual size_t MemSize(void* pMem);
    virtual void* MemTryResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine);
    virtual void* MemResize(void* pMem, size_t size, size_t align);
    virtual void* MemResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine);
    virtual BOOL ContainsMem(void* pMem);
    virtual void PrintUsage();
    virtual void Dump();

    virtual size_t GetHeapSize() const;
    virtual size_t GetUsedSize() const;

    void CreateSmallSubHeap(size_t dwHeapSize, size_t dwMaxAllocSize);

private:
    void* m_pHeapBuffer;
    size_t m_dwHeapBufferSize;
    size_t m_dwHeapUsedSize;
    uint32 m_dwNodeCount;
    size_t m_dwMinAllocSize;
    size_t m_dwLinearStep;
    uint32 m_dwMaxLog2Index;
    uint32 m_dwBaseLog2Index;
    size_t m_dwMaxExponentialAllocSize;
    size_t m_dwBiggestContiguousFreeSize;
    BOOL m_fBiggestDirty;
    XLocalHeap* m_ptSmallSubHeap;
    size_t m_dwMaxSmallAllocSize;
    XLocalHeapNode* m_ptLastHeapNode;
    XLocalHeapNode* m_ptFreeNodes[32];
    Blizzard::Lock::RMutex m_tLock;

    static size_t AlignUp(size_t dwValue, size_t dwAlign);
    static size_t AlignDown(size_t dwValue, size_t dwAlign);

    inline void Lock();
    inline void Unlock();

    inline int32 NodeRemainingSpaceFor(XLocalHeapNode* ptNode, size_t dwSize, size_t dwAlign) const;
    inline XLocalHeapNode* AlignNodeData(XLocalHeapNode* ptNode, size_t dwAlign);
    void AddToFreeList(XLocalHeapNode* ptFreeNode);
    void RemoveFromFreeList(XLocalHeapNode* ptFreeNode);
    void Truncate(XLocalHeapNode* ptNode, size_t dwSize);
    inline uint32 GetFreeSlotIndex(size_t dwSize) const;
};



























inline size_t XLocalHeap::GetHeapSize() const {
    return m_dwHeapBufferSize;
}



inline size_t XLocalHeap::GetUsedSize() const {
    return m_dwHeapUsedSize + m_dwNodeCount * sizeof(XLocalHeapNode);
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(XLocalHeapNode) == 0x20, "XLocalHeapNode");
static_assert(offsetof(XLocalHeapNode, ptPrevNode) == 0x10, "XLocalHeapNode");
static_assert(sizeof(XLocalHeap) == 0x190, "XLocalHeap");
