#pragma once

#include "DataStructuresExternal.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "XMemoryAllocatorInterface.h"

#include <stddef.h>
#include <stdint.h>

class XLocalHeap;
struct XVirtualNodeTable;

typedef BOOL (*XVirtualHeapAllocationRetryFn)(void*);

enum XVirtualNodeType {
    XVIRTUAL_NODE_TYPE_TABLE = 0,
    XVIRTUAL_NODE_TYPE_HEADER = 1,
    XVIRTUAL_NODE_TYPE_POOL = 2,
    XVIRTUAL_NODE_TYPE_UNSIZED_POOL = 3,
};

enum XVirtualHeapType {
    XVIRTUAL_HEAP_TYPE_CPU_RW_NO_GPU = 0,
    XVIRTUAL_HEAP_TYPE_CPU_RW_GPU_R = 1,
    XVIRTUAL_HEAP_TYPE_CPU_W_GPU_RW = 2,
    XVIRTUAL_HEAP_TYPE_COUNT = 3,
};

struct XVirtualNodeHeader {
    uint32 eType : 2;
    uint32 dwSize : 30;
    uint16 dwLine;
    uint16 dwDummy;
    XVirtualNodeHeader* ptNext;
    XVirtualNodeHeader* ptPrev;
    XVirtualNodeTable* ptParent;
    uintptr dwData;


























    XVirtualNodeHeader(uintptr _dwData, uint32 _dwSize, XVirtualNodeType _eType) {
        eType = _eType;
        dwSize = _dwSize;
        dwLine = 0;
        dwDummy = 0;
        ptNext = NULL;
        ptPrev = NULL;
        ptParent = NULL;
        dwData = _dwData;
    }
};

struct XVirtualPoolNodeItem {
    uint16 dwNextFreeIndex;
};

struct XVirtualPoolNode : public XVirtualNodeHeader {
    XVirtualPoolNode* ptNextPool;
    XVirtualPoolNode* ptPrevPool;
    XVirtualPoolNode* ptNextAvailablePool;
    XVirtualPoolNode* ptPrevAvailablePool;
    uint16 dwFirstFreeIndex;
    uint16 dwFreeCount;
    uint16 dwBlockedCount;
    uint16 dwPoolIndex;
    uint32 dwPoolID;















































    XVirtualPoolNode(uintptr _dwData, uint32 _dwSize, uint32 _dwPoolItemCount)
        : XVirtualNodeHeader(_dwData, _dwSize, XVIRTUAL_NODE_TYPE_POOL) {
        ptNextPool = NULL;
        ptPrevPool = NULL;
        ptNextAvailablePool = NULL;
        ptPrevAvailablePool = NULL;
        dwFirstFreeIndex = 0;
        dwBlockedCount = 0;
        dwPoolIndex = 0;
        dwPoolID = 0;
        dwFreeCount = _dwPoolItemCount;

        XVirtualPoolNodeItem* ptItems = (XVirtualPoolNodeItem*)(this + 1);
        for (uint32 i = 0; i < _dwPoolItemCount; i++) {
            ptItems[i].dwNextFreeIndex = i + 1;
        }
        ptItems[_dwPoolItemCount - 1].dwNextFreeIndex = 0xFFFF;
    }
};

struct XVirtualUnsizedPoolNode : public XVirtualNodeHeader {
    XVirtualUnsizedPoolNode* ptNextPool;
    XVirtualUnsizedPoolNode* ptPrevPool;
    uint16 dwPoolIndex;













    XVirtualUnsizedPoolNode(uintptr _dwData, uint32 _dwSize)
        : XVirtualNodeHeader(_dwData, _dwSize, XVIRTUAL_NODE_TYPE_UNSIZED_POOL) {
        ptNextPool = NULL;
        ptPrevPool = NULL;
    }
};

struct XVirtualNodeTable {
    uint32 eType : 2;
    uint32 dwRefCount : 30;
    uint16 dwTableLevel;
    uint16 dwTableInfo;
    XVirtualNodeTable* ptNext;
    XVirtualNodeTable* ptPrev;
    XVirtualNodeTable* ptParent;








    XVirtualNodeTable() {
        eType = XVIRTUAL_NODE_TYPE_TABLE;
        dwRefCount = 0;
        dwTableLevel = 0;
        dwTableInfo = 0;
        ptNext = NULL;
        ptPrev = NULL;
        ptParent = NULL;
    }






    XVirtualNodeTable** Children() {
        return (XVirtualNodeTable**)(this + 1);
    }


    XVirtualNodeHeader** Headers() {
        return (XVirtualNodeHeader**)(this + 1);
    }
};

class XVirtualHeap : public XBaseHeap {
public:
    XVirtualHeap(XLocalHeap* ptNodesHeap, size_t dwHeapBufferSize, XVirtualHeapType eType);
    virtual ~XVirtualHeap();

    virtual size_t MemSize(void* pMem);
    virtual BOOL ContainsMem(void* pMem);
    virtual void PrintUsage();
    virtual void Dump();
    void FullValidate();
    void SetAllocationRetryFn(XVirtualHeapAllocationRetryFn pfnRetry, void* pCookie);
    virtual void* MemAlloc(size_t size, size_t align, BOOL bClear);
    virtual void* MemAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine);
    virtual void* MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine);
    virtual void* MemResize(void* pMem, size_t size, size_t align);
    virtual void* MemResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine);
    virtual void* MemTryResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine);
    virtual void MemFree(void* pMem);

























































    virtual size_t GetHeapSize() const {
        return m_dwHeapBufferSize;
    }



    virtual size_t GetUsedSize() const {
        return m_dwHeapPaddedUsedSize;
    }

protected:
    uint32 m_dwNodeCount;
    uint32 m_dwNodeTableCount;
    uint32 m_dwPoolNodeCount;
    uint32 m_dwPoolNodeItemSize;
    uintptr m_dwHeapBufferSize;
    uintptr m_dwHeapUsedSize;
    uintptr m_dwHeapPaddedUsedSize;
    XVirtualNodeTable* m_ptTableBase;
    XVirtualPoolNode** m_ptFirstPool;
    XVirtualPoolNode** m_ptLastPool;
    XVirtualPoolNode** m_ptFirstAvailablePool;
    XVirtualPoolNode** m_ptLastAvailablePool;
    XVirtualUnsizedPoolNode** m_ptFirstUnsizedPool;
    XVirtualUnsizedPoolNode** m_ptLastUnsizedPool;
    XVirtualHeapAllocationRetryFn m_pfnRetry;
    void* m_pCookie;
    uint32* m_pdwPoolIDs;
    XLocalHeap* m_ptNodesHeap;
    XVirtualHeapType m_eType;
    XVirtualNodeHeader m_tEmptyMemory;
    Blizzard::Lock::RMutex m_tLock;
























    static uint16 GetTableInfo(uint32 dwLevel, uintptr dwData) {
        return (uint16)((dwData >> (56 - dwLevel * 8)) & 0xFF);
    }




    static size_t AlignUp(size_t dwValue, size_t dwAlign) {
        return (dwValue + dwAlign - 1) & ~(dwAlign - 1);
    }

    void Lock();
    void Unlock();
    XVirtualNodeTable* FindNextNode(XVirtualNodeTable* ptCurrentParent, XVirtualNodeTable* ptStartNode, uint32 dwWantedTableInfo, XVirtualNodeTable*& ptFoundPrev, uint32& dwFoundTableInfo);
    XVirtualNodeHeader* FindVirtualNodeHeader(void* pData);
    void Attach(XVirtualNodeTable* ptNode, uint32 dwTableIndex, XVirtualNodeTable* ptParent, XVirtualNodeTable* ptPrev, XVirtualNodeTable* ptNext);
    void Detach(XVirtualNodeTable* ptNode, uint32 dwTableIndex);
    void RegisterVirtualNodeHeader(XVirtualNodeHeader* ptHeader);
    void UnregisterVirtualNodeHeader(XVirtualNodeHeader* ptHeader);
    uint32 GetPoolIndex(uint32 dwSize);
    XVirtualPoolNode* CreatePoolNode(uint32 dwPoolIndex, const char* szFile, uint32 dwLine, XVirtualUnsizedPoolNode* ptUnsizedPool);
    void FreePoolNode(XVirtualPoolNode* ptPool);
    XVirtualPoolNode* GetAvailablePool(uint32 dwPoolIndex, const char* szFile, uint32 dwLine);
    void* AllocPoolItem(uint32 dwSize, const char* szFile, uint32 dwLine);
    void* AllocPoolItem(XVirtualPoolNode* ptPool, const char* szFile, uint32 dwLine);
    void FreePoolItem(XVirtualPoolNode* ptPool, void* ptPoolItem);
    XVirtualUnsizedPoolNode* CreateUnsizedPool(void* pMem, size_t dwPaddedSize, size_t dwRealSize);
    void FreeUnsizedPool(XVirtualUnsizedPoolNode* ptPool);
    XVirtualNodeHeader* PlatformVirtualAlloc(XVirtualNodeType eType, size_t size, size_t align);
    void PlatformVirtualFree(XVirtualNodeHeader* ptHeader);
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XVirtualNodeHeader) == 0x28, "XVirtualNodeHeader");
static_assert(offsetof(XVirtualNodeHeader, dwLine) == 0x4, "XVirtualNodeHeader");
static_assert(offsetof(XVirtualNodeHeader, dwDummy) == 0x6, "XVirtualNodeHeader");
static_assert(offsetof(XVirtualNodeHeader, ptNext) == 0x8, "XVirtualNodeHeader");
static_assert(offsetof(XVirtualNodeHeader, ptPrev) == 0x10, "XVirtualNodeHeader");
static_assert(offsetof(XVirtualNodeHeader, ptParent) == 0x18, "XVirtualNodeHeader");
static_assert(offsetof(XVirtualNodeHeader, dwData) == 0x20, "XVirtualNodeHeader");
static_assert(sizeof(XVirtualPoolNodeItem) == 0x2, "XVirtualPoolNodeItem");
static_assert(offsetof(XVirtualPoolNodeItem, dwNextFreeIndex) == 0x0, "XVirtualPoolNodeItem");
static_assert(sizeof(XVirtualPoolNode) == 0x58, "XVirtualPoolNode");
static_assert(sizeof(XVirtualUnsizedPoolNode) == 0x40, "XVirtualUnsizedPoolNode");
static_assert(sizeof(XVirtualNodeTable) == 0x20, "XVirtualNodeTable");
static_assert(offsetof(XVirtualNodeTable, dwTableLevel) == 0x4, "XVirtualNodeTable");
static_assert(offsetof(XVirtualNodeTable, dwTableInfo) == 0x6, "XVirtualNodeTable");
static_assert(offsetof(XVirtualNodeTable, ptNext) == 0x8, "XVirtualNodeTable");
static_assert(offsetof(XVirtualNodeTable, ptPrev) == 0x10, "XVirtualNodeTable");
static_assert(offsetof(XVirtualNodeTable, ptParent) == 0x18, "XVirtualNodeTable");
static_assert(sizeof(XVirtualHeap) == 0xd8, "XVirtualHeap");
