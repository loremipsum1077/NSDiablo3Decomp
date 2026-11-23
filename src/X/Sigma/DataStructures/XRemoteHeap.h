#pragma once

#include "DataStructuresExternal.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "XMemoryAllocatorInterface.h"

class XLocalHeap;

struct XRemoteHeapNode {
    union {
        XRemoteHeapNode* ptNextFreeNode;
        const char* szFile;
    };
    union {
        XRemoteHeapNode* ptPrevFreeNode;
        uint32 dwLine;
    };
    XRemoteHeapNode* ptNextNode;
    size_t dwUsed : 1;
    size_t dwAllocated : 1;
    size_t dwSize : 62;
    void* pData;
};

struct XRemoteHeapNodeContainer {
    XRemoteHeapNode* ptNodes;
    int32 nNodeCount;
    int32 nMinUnallocatedNodeIndex;
    XRemoteHeapNodeContainer* ptNext;
};

class XRemoteHeap : public XBaseHeap {
public:
    XRemoteHeap(XLocalHeap* ptNodesHeap, void* pHeapBuffer, size_t dwHeapBufferSize, size_t dwMinAllocSize,
                size_t dwMaxAllocSize);
    virtual ~XRemoteHeap();

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

    void FullValidate();
    void FastValidate(XRemoteHeapNode* ptNode);
    XRemoteHeapNode* GetNodeFromMem(void* pMem);

    virtual size_t GetHeapSize() const;
    void* GetHeapBuffer() const;
    virtual size_t GetUsedSize() const;
    size_t GetBiggestContiguousFreeSize();

private:
    XLocalHeap* m_ptNodesHeap;
    XRemoteHeapNodeContainer* m_ptNodeContainers;
    void* m_pHeapBuffer;
    size_t m_dwHeapBufferSize;
    size_t m_dwHeapUsedSize;
    size_t m_dwBucketSize;
    size_t m_dwMinAllocSize;
    size_t m_dwLinearStep;
    uint32 m_dwMaxLog2Index;
    uint32 m_dwBaseLog2Index;
    size_t m_dwMaxExponentialAllocSize;
    size_t m_dwBiggestContiguousFreeSize;
    BOOL m_fBiggestDirty;
    XRemoteHeapNode* m_ptFreeNodes[32];
    XRemoteHeapNode* m_ptNodeBuckets[512];
    XRemoteHeapNode* m_ptLastNodeBuckets[512];
    Blizzard::Lock::RMutex m_tLock;

    static size_t AlignUp(size_t dwValue, size_t dwAlign);
    static size_t AlignDown(size_t dwValue, size_t dwAlign);

    void Lock();
    void Unlock();

    int32 NodeRemainingSpaceFor(XRemoteHeapNode* ptNode, size_t dwSize, size_t dwAlign) const;
    XRemoteHeapNode* AlignNodeData(XRemoteHeapNode* ptNode, size_t dwAlign);
    void AddToFreeList(XRemoteHeapNode* ptFreeNode, XRemoteHeapNode* ptPrevNode);
    void RemoveFromFreeList(XRemoteHeapNode* ptFreeNode);
    XRemoteHeapNode* FindNextNode(XRemoteHeapNode* ptNode);
    XRemoteHeapNode* FindPrevNode(XRemoteHeapNode* ptNode);
    void Truncate(XRemoteHeapNode* ptNode, size_t dwSize);
    XRemoteHeapNode* AddNewNode(void* pData);
    void RemoveNode(XRemoteHeapNode* ptNode);
    uint32 GetFreeSlotIndex(size_t dwSize) const;
    uint32 GetNodeBucket(void* pData) const;
    XRemoteHeapNode* GetHeapNode(void* pData, uint32 dwBucketIndex, XRemoteHeapNode*& ptPrevNode) const;
};





































inline size_t XRemoteHeap::GetHeapSize() const {
    return m_dwHeapBufferSize;
}



inline void* XRemoteHeap::GetHeapBuffer() const {
    return m_pHeapBuffer;
}



inline size_t XRemoteHeap::GetUsedSize() const {
    return m_dwHeapUsedSize;
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(XRemoteHeapNode) == 0x28, "XRemoteHeapNode");
static_assert(offsetof(XRemoteHeapNode, ptNextNode) == 0x10, "XRemoteHeapNode");
static_assert(offsetof(XRemoteHeapNode, pData) == 0x20, "XRemoteHeapNode");
static_assert(sizeof(XRemoteHeapNodeContainer) == 0x18, "XRemoteHeapNodeContainer");
static_assert(offsetof(XRemoteHeapNodeContainer, ptNodes) == 0x0, "XRemoteHeapNodeContainer");
static_assert(offsetof(XRemoteHeapNodeContainer, nNodeCount) == 0x8, "XRemoteHeapNodeContainer");
static_assert(offsetof(XRemoteHeapNodeContainer, nMinUnallocatedNodeIndex) == 0xc, "XRemoteHeapNodeContainer");
static_assert(offsetof(XRemoteHeapNodeContainer, ptNext) == 0x10, "XRemoteHeapNodeContainer");
static_assert(sizeof(XRemoteHeap) == 0x2188, "XRemoteHeap");
