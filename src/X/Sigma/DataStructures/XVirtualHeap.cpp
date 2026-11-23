#include "XVirtualHeap.h"

#include "../Math/MathGeneral.h"
#include "XLocalHeap.h"

#include <new>
#include <stdlib.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Memory/SigmaMemory.h"


































#include "../../../NintendoSDK/Include/nn/nn_Result.h"
namespace nn {









namespace os {
Result AllocateAddressRegion(uintptr_t* pOutAddress, size_t size);
Result AllocateMemoryPages(uintptr_t address, size_t size);
Result FreeMemoryPages(uintptr_t address, size_t size);
Result FreeAddressRegion(uintptr_t address);
}  // namespace os
}  // namespace nn

// The names of these constants are not in the DWARF.
static const size_t XVIRTUAL_PAGE_SIZE = 0x10000;
static const uint32 XVIRTUAL_POOL_COUNT = 12;
static const uint32 XVIRTUAL_TABLE_CHILD_COUNT = 64;
static const uint32 XVIRTUAL_MAX_NODE_SIZE = 0x3FFFFFFF;


















static void* sAllocateFromHeap(XLocalHeap* ptHeap, size_t size, const char* szFile, uint32 dwLine) {
    if (ptHeap != NULL) {
        return ptHeap->MemAllocTracked(size, 16, FALSE, szFile, dwLine);
    }
    return malloc(size);
}










static void sFreeFromHeap(XLocalHeap* ptHeap, void* pMem) {
    if (ptHeap != NULL) {
        ptHeap->MemFree(pMem);
    } else {
        free(pMem);
    }
}











XVirtualHeap::XVirtualHeap(XLocalHeap* ptNodesHeap, size_t dwHeapBufferSize, XVirtualHeapType eType)
    : m_dwNodeCount(0), m_dwNodeTableCount(0), m_dwPoolNodeCount(0), m_dwPoolNodeItemSize(0),
      m_dwHeapBufferSize(dwHeapBufferSize), m_dwHeapUsedSize(0), m_dwHeapPaddedUsedSize(0),
      m_pfnRetry(NULL), m_pCookie(NULL),
      m_ptNodesHeap(ptNodesHeap), m_eType(eType),
      m_tEmptyMemory(0, 0, XVIRTUAL_NODE_TYPE_HEADER) {












    m_ptTableBase = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualNodeTable) + XVIRTUAL_TABLE_CHILD_COUNT * sizeof(XVirtualNodeTable*), "", 0))
        XVirtualNodeTable();
    m_ptTableBase->dwRefCount = 0;
    m_ptTableBase->dwTableLevel = 0;
    m_ptTableBase->dwTableInfo = 0;
    SigmaMemoryClear(m_ptTableBase + 1, XVIRTUAL_TABLE_CHILD_COUNT * sizeof(XVirtualNodeTable*));


    m_ptFirstPool = (XVirtualPoolNode**)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(XVirtualPoolNode*), "", 0);
    m_ptLastPool = (XVirtualPoolNode**)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(XVirtualPoolNode*), "", 0);
    m_ptFirstAvailablePool = (XVirtualPoolNode**)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(XVirtualPoolNode*), "", 0);
    m_ptLastAvailablePool = (XVirtualPoolNode**)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(XVirtualPoolNode*), "", 0);

    m_ptFirstUnsizedPool = (XVirtualUnsizedPoolNode**)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(XVirtualUnsizedPoolNode*), "", 0);
    m_ptLastUnsizedPool = (XVirtualUnsizedPoolNode**)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(XVirtualUnsizedPoolNode*), "", 0);

    m_pdwPoolIDs = (uint32*)sAllocateFromHeap(m_ptNodesHeap, XVIRTUAL_POOL_COUNT * sizeof(uint32), "", 0);

    for (int32 i = 0; i < (int32)XVIRTUAL_POOL_COUNT; i++) {
        m_ptFirstPool[i] = NULL;
        m_ptLastPool[i] = NULL;
        m_ptFirstAvailablePool[i] = NULL;
        m_ptLastAvailablePool[i] = NULL;
        m_ptFirstUnsizedPool[i] = NULL;
        m_ptLastUnsizedPool[i] = NULL;
        m_pdwPoolIDs[i] = 1;
    }
}





















































XVirtualHeap::~XVirtualHeap() {





















    sFreeFromHeap(m_ptNodesHeap, m_pdwPoolIDs);

    sFreeFromHeap(m_ptNodesHeap, m_ptLastUnsizedPool);
    sFreeFromHeap(m_ptNodesHeap, m_ptFirstUnsizedPool);

    sFreeFromHeap(m_ptNodesHeap, m_ptLastAvailablePool);
    sFreeFromHeap(m_ptNodesHeap, m_ptFirstAvailablePool);
    sFreeFromHeap(m_ptNodesHeap, m_ptLastPool);
    sFreeFromHeap(m_ptNodesHeap, m_ptFirstPool);


    sFreeFromHeap(m_ptNodesHeap, m_ptTableBase);
}




size_t XVirtualHeap::MemSize(void* pMem) {
    size_t dwSize = 0;

    if (pMem == NULL || pMem == &m_tEmptyMemory) {
        return 0;
    }


    Lock();

    XVirtualNodeHeader* ptHeader = FindVirtualNodeHeader(pMem);
    if (ptHeader != NULL) {
        if (ptHeader->eType == XVIRTUAL_NODE_TYPE_HEADER) {
            dwSize = ptHeader->dwSize;
        } else {
            dwSize = 16u << ((XVirtualPoolNode*)ptHeader)->dwPoolIndex;
        }
    }







    Unlock();

    return dwSize;
}




BOOL XVirtualHeap::ContainsMem(void* pMem) {
    XVirtualNodeHeader* ptHeader;
    BOOL fContainsMem;

    if (pMem == NULL) {
        return FALSE;
    }
    if (pMem == &m_tEmptyMemory) {
        return TRUE;
    }



    Lock();

    ptHeader = FindVirtualNodeHeader(pMem);
    fContainsMem = ptHeader != NULL;

    Unlock();

    return fContainsMem;
}




void XVirtualHeap::PrintUsage() {


    Lock();









    Unlock();
}





















































































void XVirtualHeap::Dump() {
    PrintUsage();
}








































































































































































inline void XVirtualHeap::Lock() {
    m_tLock.Enter();
}






inline void XVirtualHeap::Unlock() {
    m_tLock.Leave();
}






static BOOL sCanBePooled(size_t size, size_t align) {
    return size <= 0x8000;
}













void* XVirtualHeap::MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) {
    void* pMem = NULL;

    if (size == 0) {
        pMem = &m_tEmptyMemory;
    } else {






        size = Math::Max(size, align);


        if (sCanBePooled(size, align) == TRUE) {

            Lock();
            pMem = AllocPoolItem((uint32)size, szFile, dwLine);
            Unlock();
        } else {
            Lock();





            size_t dwPaddedSize = AlignUp(size, XVIRTUAL_PAGE_SIZE);

            if (m_dwHeapPaddedUsedSize + dwPaddedSize <= m_dwHeapBufferSize) {
                XVirtualNodeHeader* ptHeader = PlatformVirtualAlloc(XVIRTUAL_NODE_TYPE_HEADER, size, align);
                if (ptHeader != NULL) {
                    pMem = (void*)ptHeader->dwData;

                    RegisterVirtualNodeHeader(ptHeader);

                    m_dwNodeCount++;






                    m_dwHeapUsedSize += Math::Min(size, (size_t)XVIRTUAL_MAX_NODE_SIZE);
                    m_dwHeapPaddedUsedSize += Math::Min(dwPaddedSize, (size_t)XVIRTUAL_MAX_NODE_SIZE);
                }
            }

            Unlock();
        }



        if (bClear && pMem != NULL) {
            ZeroMemory(pMem, size);
        }
    }

    return pMem;
}






void* XVirtualHeap::MemAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) {
    void* pMem = MemTryAllocTracked(size, align, bClear, szFile, dwLine);

    if (pMem == NULL && m_pfnRetry != NULL) {
        while (pMem == NULL && m_pfnRetry(m_pCookie)) {
            pMem = MemTryAllocTracked(size, align, bClear, szFile, dwLine);
        }
    }

    if (pMem == NULL) {
        char str[64];

        Dump();
        Blizzard::String::Format(str, sizeof(str), "Error: allocation of %d bytes failed!", (uint32)size);
















        XASSERT(FALSE);
    }

    return pMem;
}




void* XVirtualHeap::MemAlloc(size_t size, size_t align, BOOL bClear) {
    return MemAllocTracked(size, align, bClear, "", 0);
}








void* XVirtualHeap::MemTryResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
    void* pNewMem = MemTryAllocTracked(size, align, FALSE, szFile, dwLine);

    if (pNewMem == NULL) {
        return NULL;
    }

    if (pMem != NULL) {
        size_t oldSize = MemSize(pMem);
        if (oldSize != 0) {








            SigmaMemoryCopy(pNewMem, pMem, Math::Min(size, oldSize));
        }
        MemFree(pMem);
    }

    return pNewMem;
}





void* XVirtualHeap::MemResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
    void* pNewMem = MemTryResizeTracked(pMem, size, align, szFile, dwLine);

    if (pNewMem == NULL) {
        char str[64];

        Dump();
        Blizzard::String::Format(str, sizeof(str), "Error: allocation of %d bytes failed!", (uint32)size);









        XASSERT(FALSE);
    }

    return pNewMem;
}




void* XVirtualHeap::MemResize(void* pMem, size_t size, size_t align) {
    return MemResizeTracked(pMem, size, align, "", 0);
}








void XVirtualHeap::MemFree(void* pMem) {
    XVirtualNodeHeader* ptHeader;

    if (pMem == NULL || pMem == &m_tEmptyMemory) {
        return;
    }


    Lock();

    ptHeader = FindVirtualNodeHeader(pMem);
    if (ptHeader != NULL) {
        if (ptHeader->eType == XVIRTUAL_NODE_TYPE_POOL) {
            FreePoolItem((XVirtualPoolNode*)ptHeader, pMem);
        } else {




            size_t dwPaddedSize = AlignUp(ptHeader->dwSize, XVIRTUAL_PAGE_SIZE);

            m_dwHeapPaddedUsedSize -= dwPaddedSize;
            m_dwHeapUsedSize -= ptHeader->dwSize;
            m_dwNodeCount--;

            UnregisterVirtualNodeHeader(ptHeader);
            PlatformVirtualFree(ptHeader);
        }
    }

    Unlock();
}








inline XVirtualNodeTable* XVirtualHeap::FindNextNode(XVirtualNodeTable* ptCurrentParent, XVirtualNodeTable* ptStartNode, uint32 dwWantedTableInfo, XVirtualNodeTable*& ptFoundPrev, uint32& dwFoundTableInfo) {
    uint32 dwNodeInfo = 0;
    XVirtualNodeTable* ptPrev = NULL;
    XVirtualNodeTable* ptNode = ptStartNode;

    while (ptNode != NULL) {
        if (ptNode->eType == XVIRTUAL_NODE_TYPE_TABLE) {
            dwNodeInfo = ptNode->dwTableInfo;
            if (dwNodeInfo >= dwWantedTableInfo) {
                break;
            }
        } else {

            dwNodeInfo = GetTableInfo(ptCurrentParent->dwTableLevel, ((XVirtualNodeHeader*)ptNode)->dwData);
            if (dwNodeInfo >= dwWantedTableInfo) {
                break;
            }
        }

        ptPrev = ptNode;
        ptNode = ptNode->ptNext;
    }

    ptFoundPrev = ptPrev;
    dwFoundTableInfo = dwNodeInfo;
    return ptNode;
}













XVirtualNodeHeader* XVirtualHeap::FindVirtualNodeHeader(void* pData) {
    XVirtualNodeTable* ptCurrentParent = m_ptTableBase;
    uintptr dwData = (uintptr)pData & ~(uintptr)(XVIRTUAL_PAGE_SIZE - 1);
    XVirtualNodeHeader* ptCurrentHeader;

    for (int32 i = 0; i < 5; i++) {
        XVirtualNodeTable* ptNode;
        uint32 dwNodeInfo;

        uint16 dwTLBInfo = GetTableInfo(i, dwData);
        uint32 dwTLBIndex = dwTLBInfo / 16;
        XVirtualNodeTable* ptPrev;
        ptNode = ptCurrentParent->Children()[dwTLBIndex];

        if (ptNode == NULL) {
            return NULL;
        }

        if (ptNode->eType != XVIRTUAL_NODE_TYPE_TABLE) {
            XVirtualNodeHeader* ptHeader = (XVirtualNodeHeader*)ptNode;
            uint16 dwHeaderInfo = GetTableInfo(i, ptHeader->dwData);
            if (dwHeaderInfo == dwTLBInfo) {
                if (ptHeader->dwData == dwData) {
                    return ptHeader;
                }
                return NULL;
            }
        }





        ptNode = FindNextNode(ptCurrentParent, ptNode, dwTLBInfo, ptPrev, dwNodeInfo);
        if (ptNode == NULL || dwNodeInfo != dwTLBInfo) {
            return NULL;
        }

        if (ptNode->eType != XVIRTUAL_NODE_TYPE_TABLE) {
            XVirtualNodeHeader* ptHeader = (XVirtualNodeHeader*)ptNode;
            if (ptHeader->dwData == dwData) {
                return ptHeader;
            }
            return NULL;
        }

        ptCurrentParent = ptNode;
    }




    uint16 dwTLBInfo = GetTableInfo(5, dwData);
    uint32 dwTLBIndex = dwTLBInfo / 16;
    ptCurrentHeader = ptCurrentParent->Headers()[dwTLBIndex];
    XVirtualNodeHeader* ptPrev = NULL;

    while (ptCurrentHeader != NULL) {
        if (ptCurrentHeader->dwData >= dwData) {
            break;
        }
        ptPrev = ptCurrentHeader;
        ptCurrentHeader = ptCurrentHeader->ptNext;
    }

    if (ptCurrentHeader == NULL || ptCurrentHeader->dwData != dwData) {
        return NULL;
    }
    return ptCurrentHeader;
}













inline void XVirtualHeap::Attach(XVirtualNodeTable* ptNode, uint32 dwTableIndex, XVirtualNodeTable* ptParent, XVirtualNodeTable* ptPrev, XVirtualNodeTable* ptNext) {
    ptNode->ptParent = ptParent;
    ptNode->ptNext = ptNext;
    if (ptNext != NULL) {
        ptNext->ptPrev = ptNode;
    }
    ptNode->ptPrev = ptPrev;
    if (ptPrev != NULL) {
        ptPrev->ptNext = ptNode;
    } else {
        ((XVirtualNodeTable**)(ptParent + 1))[dwTableIndex] = ptNode;
    }
}

















inline void XVirtualHeap::Detach(XVirtualNodeTable* ptNode, uint32 dwTableIndex) {
    if (ptNode->ptPrev != NULL) {
        ptNode->ptPrev->ptNext = ptNode->ptNext;
    } else {







        ptNode->ptParent->Children()[dwTableIndex] = ptNode->ptNext;
    }
    if (ptNode->ptNext != NULL) {
        ptNode->ptNext->ptPrev = ptNode->ptPrev;
    }
    ptNode->ptPrev = NULL;
    ptNode->ptParent = NULL;
    ptNode->ptNext = NULL;
}








void XVirtualHeap::RegisterVirtualNodeHeader(XVirtualNodeHeader* ptHeader) {
    XVirtualNodeTable* ptCurrentParent = m_ptTableBase;
    uintptr dwData = ptHeader->dwData;
    XVirtualNodeHeader* ptCurrentHeader;

    ptCurrentParent->dwRefCount++;

    for (int32 i = 0; i < 5; i++) {
        XVirtualNodeTable* ptNode;
        uint32 dwNodeInfo;
        uint16 dwTLBInfo = GetTableInfo(i, dwData);
        uint32 dwTLBIndex = dwTLBInfo / 16;
        ptNode = ptCurrentParent->Children()[dwTLBIndex];
        XVirtualNodeTable* ptTLB;
        XVirtualNodeTable* ptPrev;

        if (ptNode == NULL) {

            Attach((XVirtualNodeTable*)ptHeader, dwTLBIndex, ptCurrentParent, NULL, NULL);
            return;
        }

        if (ptNode->eType != XVIRTUAL_NODE_TYPE_TABLE) {
            XVirtualNodeHeader* ptOtherHeader = (XVirtualNodeHeader*)ptNode;

            uint16 dwHeaderInfo = GetTableInfo(i, ptOtherHeader->dwData);
            if (dwHeaderInfo == dwTLBInfo) {
                m_dwNodeTableCount++;


                ptTLB = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualNodeTable) + XVIRTUAL_TABLE_CHILD_COUNT * sizeof(XVirtualNodeTable*), "", 0))
                    XVirtualNodeTable();
                ptTLB->dwRefCount = 1;
                ptTLB->dwTableLevel = i + 1;
                ptTLB->dwTableInfo = dwHeaderInfo;
                // Only the 16 slots a table uses; the other two sites clear all 64 that are allocated.
                SigmaMemoryClear(ptTLB + 1, 16 * sizeof(XVirtualNodeTable*));

                XVirtualNodeTable* ptNext = (XVirtualNodeTable*)ptOtherHeader->ptNext;
                XVirtualNodeTable* ptPrev = (XVirtualNodeTable*)ptOtherHeader->ptPrev;
                Detach((XVirtualNodeTable*)ptOtherHeader, dwTLBIndex);

                Attach(ptTLB, dwTLBIndex, ptCurrentParent, ptPrev, ptNext);

                uint32 dwHeaderIndex = GetTableInfo(i + 1, ptOtherHeader->dwData) / 16;


                Attach((XVirtualNodeTable*)ptOtherHeader, dwHeaderIndex, ptTLB, NULL, NULL);

                ptNode = ptTLB;
            }
        }



        ptNode = FindNextNode(ptCurrentParent, ptNode, dwTLBInfo, ptPrev, dwNodeInfo);

        if (ptNode == NULL || dwNodeInfo != dwTLBInfo) {


            Attach((XVirtualNodeTable*)ptHeader, dwTLBIndex, ptCurrentParent, ptPrev, ptNode);
            return;
        }

        if (ptNode->eType != XVIRTUAL_NODE_TYPE_TABLE) {
            XVirtualNodeHeader* ptOtherHeader = (XVirtualNodeHeader*)ptNode;
            uint32 dwHeaderInfo = dwNodeInfo;
            m_dwNodeTableCount++;
            ptTLB = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualNodeTable) + XVIRTUAL_TABLE_CHILD_COUNT * sizeof(XVirtualNodeTable*), "", 0))
                XVirtualNodeTable();
            ptTLB->dwRefCount = 1;
            ptTLB->dwTableLevel = i + 1;
            ptTLB->dwTableInfo = dwHeaderInfo;
            SigmaMemoryClear(ptTLB + 1, XVIRTUAL_TABLE_CHILD_COUNT * sizeof(XVirtualNodeTable*));

            Attach(ptTLB, dwTLBIndex, ptCurrentParent, (XVirtualNodeTable*)ptOtherHeader->ptPrev, (XVirtualNodeTable*)ptOtherHeader->ptNext);


            uint32 dwHeaderIndex = GetTableInfo(i + 1, ptOtherHeader->dwData) / 16;


            Attach((XVirtualNodeTable*)ptOtherHeader, dwHeaderIndex, ptTLB, NULL, NULL);

            ptNode = ptTLB;
        }

        ptNode->dwRefCount++;
        ptCurrentParent = ptNode;
    }




    uint16 dwTLBInfo = GetTableInfo(5, dwData);
    uint32 dwTLBIndex = dwTLBInfo / 16;
    ptCurrentHeader = ptCurrentParent->Headers()[dwTLBIndex];
    XVirtualNodeHeader* ptPrev = NULL;
    while (ptCurrentHeader != NULL) {
        if (ptCurrentHeader->dwData >= dwData) {
            break;
        }
        ptPrev = ptCurrentHeader;
        ptCurrentHeader = ptCurrentHeader->ptNext;
    }
    Attach((XVirtualNodeTable*)ptHeader, dwTLBIndex, ptCurrentParent, (XVirtualNodeTable*)ptPrev, (XVirtualNodeTable*)ptCurrentHeader);
}





void XVirtualHeap::UnregisterVirtualNodeHeader(XVirtualNodeHeader* ptHeader) {
    XVirtualNodeTable* ptTLB = ptHeader->ptParent;
    uintptr dwData = ptHeader->dwData;



    uint16 dwTLBInfo = GetTableInfo(ptTLB->dwTableLevel, dwData);
    uint32 dwTLBIndex = dwTLBInfo / 16;

    Detach((XVirtualNodeTable*)ptHeader, dwTLBIndex);

    while (ptTLB != NULL) {
        XVirtualNodeTable* ptToRelease = ptTLB;

        ptTLB = ptTLB->ptParent;
        ptToRelease->dwRefCount--;

        if (ptTLB == NULL) {
            break;
        }

        if (ptToRelease->dwRefCount == 1) {
            XVirtualNodeHeader* ptTableHeader = NULL;
            XVirtualNodeTable* ptNext;
            XVirtualNodeTable* ptPrev;
            XVirtualNodeTable* ptParent;
            uint32 dwTableTLBIndex;

            {
                int32 i;
                for (i = 0; i < 16; i++) {
                    if (((XVirtualNodeHeader**)(ptToRelease + 1))[i] != NULL) {
                        break;
                    }
                }
                if (i < 16) {
                    ptTableHeader = ((XVirtualNodeHeader**)(ptToRelease + 1))[i];
                }
            }

            ptNext = ptToRelease->ptNext;
            dwTableTLBIndex = ptToRelease->dwTableInfo / 16;
            ptPrev = ptToRelease->ptPrev;
            ptParent = ptTLB;
            Detach(ptToRelease, dwTableTLBIndex);

            Attach((XVirtualNodeTable*)ptTableHeader, dwTableTLBIndex, ptParent, ptPrev, ptNext);

            m_dwNodeTableCount--;

            sFreeFromHeap(m_ptNodesHeap, ptToRelease);
        } else if (ptToRelease->dwRefCount == 0) {
            uint32 dwTableTLBIndex = ptToRelease->dwTableInfo / 16;



            Detach(ptToRelease, dwTableTLBIndex);

            m_dwNodeTableCount--;

            sFreeFromHeap(m_ptNodesHeap, ptToRelease);
        }
    }
}






inline uint32 XVirtualHeap::GetPoolIndex(uint32 dwSize) {
    uint32 dwPoolIndex;

    dwPoolIndex = Math::Max((int)Math::FloorLog2(dwSize) - 4, 0);
    if ((16u << dwPoolIndex) < dwSize) {
        dwPoolIndex++;
    }


    return Math::Min(dwPoolIndex, XVIRTUAL_POOL_COUNT - 1);
}

XVirtualPoolNode* XVirtualHeap::CreatePoolNode(uint32 dwPoolIndex, const char* szFile, uint32 dwLine, XVirtualUnsizedPoolNode* ptUnsizedPool) {
    uint32 dwItemSize = 1 << (dwPoolIndex + 4);
    XVirtualPoolNode* ptPool;

    if (ptUnsizedPool != NULL) {
        uint32 dwItemCount = XVIRTUAL_PAGE_SIZE / dwItemSize;
        size_t dwPaddedSize;
        uint16 dwBlockedItemCount;
        XVirtualPoolNodeItem* ptItems;




        ptPool = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualPoolNode) + dwItemCount * sizeof(XVirtualPoolNodeItem), "", 0))
            XVirtualPoolNode(ptUnsizedPool->dwData, ptUnsizedPool->dwSize, dwItemCount);


        dwPaddedSize = AlignUp(ptUnsizedPool->dwSize, XVIRTUAL_PAGE_SIZE);
        dwBlockedItemCount = AlignUp(XVIRTUAL_PAGE_SIZE - (dwPaddedSize - ptUnsizedPool->dwSize), dwItemSize) / dwItemSize;
        ptItems = (XVirtualPoolNodeItem*)(ptPool + 1);

        for (uint32 i = 0; i < dwBlockedItemCount; i++) {
            ptItems[i].dwNextFreeIndex = 0xFFFD;
        }

        ptPool->dwFirstFreeIndex = dwBlockedItemCount;
        ptPool->dwFreeCount -= dwBlockedItemCount;
        ptPool->dwBlockedCount = dwBlockedItemCount;









        FreeUnsizedPool(ptUnsizedPool);
    } else {
        if (m_dwHeapPaddedUsedSize + XVIRTUAL_PAGE_SIZE > m_dwHeapBufferSize) {
            return NULL;
        }

        ptPool = (XVirtualPoolNode*)PlatformVirtualAlloc(XVIRTUAL_NODE_TYPE_POOL, dwItemSize, 16);
        if (ptPool == NULL) {
            return NULL;
        }
    }

    ptPool->dwPoolIndex = dwPoolIndex;
    ptPool->dwPoolID = m_pdwPoolIDs[dwPoolIndex]++;

    RegisterVirtualNodeHeader(ptPool);

    m_dwPoolNodeCount++;
    m_dwPoolNodeItemSize += (XVIRTUAL_PAGE_SIZE * sizeof(XVirtualPoolNodeItem)) / dwItemSize;

    if (ptUnsizedPool == NULL) {
        m_dwHeapPaddedUsedSize += XVIRTUAL_PAGE_SIZE;
    }

    ptPool->ptPrevPool = m_ptLastPool[dwPoolIndex];
    ptPool->ptPrevAvailablePool = m_ptLastAvailablePool[dwPoolIndex];
    m_ptLastPool[dwPoolIndex] = ptPool;
    m_ptLastAvailablePool[dwPoolIndex] = ptPool;

    if (ptPool->ptPrevPool != NULL) {
        ptPool->ptPrevPool->ptNextPool = ptPool;
    } else {
        m_ptFirstPool[dwPoolIndex] = ptPool;
    }

    if (ptPool->ptPrevAvailablePool != NULL) {
        ptPool->ptPrevAvailablePool->ptNextAvailablePool = ptPool;
    } else {
        m_ptFirstAvailablePool[dwPoolIndex] = ptPool;
    }

    return ptPool;
}



















inline void XVirtualHeap::FreePoolNode(XVirtualPoolNode* ptPool) {
    uint32 dwPoolIndex = ptPool->dwPoolIndex;
    uint32 dwItemSize = 16 << dwPoolIndex;
    uint32 dwItemCount = XVIRTUAL_PAGE_SIZE / dwItemSize;

    if (ptPool->ptPrevPool != NULL) {
        ptPool->ptPrevPool->ptNextPool = ptPool->ptNextPool;
    } else {
        m_ptFirstPool[dwPoolIndex] = ptPool->ptNextPool;
    }

    if (ptPool->ptNextPool != NULL) {
        ptPool->ptNextPool->ptPrevPool = ptPool->ptPrevPool;
    } else {
        m_ptLastPool[dwPoolIndex] = ptPool->ptPrevPool;
    }

    if (ptPool->ptPrevAvailablePool != NULL) {
        ptPool->ptPrevAvailablePool->ptNextAvailablePool = ptPool->ptNextAvailablePool;
    } else {
        m_ptFirstAvailablePool[dwPoolIndex] = ptPool->ptNextAvailablePool;
    }

    if (ptPool->ptNextAvailablePool != NULL) {
        ptPool->ptNextAvailablePool->ptPrevAvailablePool = ptPool->ptPrevAvailablePool;
    } else {
        m_ptLastAvailablePool[dwPoolIndex] = ptPool->ptPrevAvailablePool;
    }

    m_dwHeapPaddedUsedSize -= XVIRTUAL_PAGE_SIZE;
    m_dwPoolNodeItemSize -= dwItemCount * sizeof(XVirtualPoolNodeItem);
    m_dwPoolNodeCount--;

    UnregisterVirtualNodeHeader(ptPool);
    PlatformVirtualFree(ptPool);
}



















inline XVirtualPoolNode* XVirtualHeap::GetAvailablePool(uint32 dwPoolIndex, const char* szFile, uint32 dwLine) {
    XVirtualPoolNode* ptPool = m_ptFirstAvailablePool[dwPoolIndex];

    if (ptPool == NULL) {
        for (int32 i = dwPoolIndex; i < (int32)XVIRTUAL_POOL_COUNT; i++) {
            if (m_ptFirstUnsizedPool[i] != NULL) {
                return CreatePoolNode(dwPoolIndex, szFile, dwLine, m_ptFirstUnsizedPool[i]);
            }
        }

        ptPool = CreatePoolNode(dwPoolIndex, szFile, dwLine, NULL);
    }

    return ptPool;
}











void* XVirtualHeap::AllocPoolItem(uint32 dwSize, const char* szFile, uint32 dwLine) {
    uint32 dwPoolIndex;
    XVirtualPoolNode* ptPool;


    dwPoolIndex = GetPoolIndex(dwSize);
    ptPool = GetAvailablePool(dwPoolIndex, szFile, dwLine);
    if (ptPool == NULL) {
        return NULL;
    }


    return AllocPoolItem(ptPool, szFile, dwLine);
}




inline void* XVirtualHeap::AllocPoolItem(XVirtualPoolNode* ptPool, const char* szFile, uint32 dwLine) {
    uint32 dwIndex;
    uint32 dwPoolIndex = ptPool->dwPoolIndex;
    uint32 dwItemSize = 16 << dwPoolIndex;
    XVirtualPoolNodeItem* ptItems = (XVirtualPoolNodeItem*)(ptPool + 1);

    if (ptPool->dwFreeCount == 0) {
        return NULL;
    }

    dwIndex = ptPool->dwFirstFreeIndex;
    ptPool->dwFirstFreeIndex = ptItems[dwIndex].dwNextFreeIndex;
    ptPool->dwFreeCount--;
    ptItems[dwIndex].dwNextFreeIndex = 0xFFFE;

    m_dwHeapUsedSize += dwItemSize;

    if (ptPool->dwFreeCount == 0) {
        if (ptPool->ptPrevAvailablePool != NULL) {
            ptPool->ptPrevAvailablePool->ptNextAvailablePool = ptPool->ptNextAvailablePool;
        } else {
            m_ptFirstAvailablePool[dwPoolIndex] = ptPool->ptNextAvailablePool;
        }

        if (ptPool->ptNextAvailablePool != NULL) {
            ptPool->ptNextAvailablePool->ptPrevAvailablePool = ptPool->ptPrevAvailablePool;
        } else {
            m_ptLastAvailablePool[dwPoolIndex] = ptPool->ptPrevAvailablePool;
        }
    }

    return (void*)(ptPool->dwData + dwItemSize * dwIndex);
}




















void XVirtualHeap::FreePoolItem(XVirtualPoolNode* ptPool, void* ptPoolItem) {
    uint32 dwItemSize = 16 << ptPool->dwPoolIndex;
    uint32 dwPoolID = ptPool->dwPoolID;
    uintptr dwItem = (uintptr)ptPoolItem;
    uint32 dwItemCount = XVIRTUAL_PAGE_SIZE / dwItemSize;
    uint32 dwPoolIndex = ptPool->dwPoolIndex;
    uint32 dwIndex = (dwItem - ptPool->dwData) / dwItemSize;
    XVirtualPoolNodeItem* ptItems = (XVirtualPoolNodeItem*)(ptPool + 1);

    ptItems[dwIndex].dwNextFreeIndex = ptPool->dwFirstFreeIndex;
    ptPool->dwFirstFreeIndex = dwIndex;
    ptPool->dwFreeCount++;

    m_dwHeapUsedSize -= dwItemSize;

    if (ptPool->dwFreeCount == 1) {
        XVirtualPoolNode* ptNextAvailable = m_ptFirstAvailablePool[dwPoolIndex];
        XVirtualPoolNode* ptPrevAvailable = NULL;

        while (ptNextAvailable != NULL) {
            if (ptNextAvailable->dwPoolID >= dwPoolID) {
                break;
            }
            ptPrevAvailable = ptNextAvailable;
            ptNextAvailable = ptNextAvailable->ptNextAvailablePool;
        }

        ptPool->ptPrevAvailablePool = ptPrevAvailable;
        if (ptPrevAvailable != NULL) {
            ptPrevAvailable->ptNextAvailablePool = ptPool;
        } else {
            m_ptFirstAvailablePool[dwPoolIndex] = ptPool;
        }

        ptPool->ptNextAvailablePool = ptNextAvailable;
        if (ptNextAvailable != NULL) {
            ptNextAvailable->ptPrevAvailablePool = ptPool;
        } else {
            m_ptLastAvailablePool[dwPoolIndex] = ptPool;
        }
    }

    if (ptPool->dwFreeCount == dwItemCount) {









        FreePoolNode(ptPool);
    }
}




XVirtualUnsizedPoolNode* XVirtualHeap::CreateUnsizedPool(void* pMem, size_t dwPaddedSize, size_t dwRealSize) {
    uint32 dwFreeSize = dwPaddedSize - dwRealSize;
    uint32 dwPoolIndex;
    XVirtualUnsizedPoolNode* ptPool;


    dwPoolIndex = Math::Min(Math::Max((int)Math::FloorLog2(dwFreeSize) - 4, 0), (int)XVIRTUAL_POOL_COUNT - 1);

    ptPool = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualUnsizedPoolNode), "", 0))
        XVirtualUnsizedPoolNode((uintptr)pMem + dwPaddedSize - XVIRTUAL_PAGE_SIZE, dwRealSize);
    ptPool->dwPoolIndex = dwPoolIndex;

    RegisterVirtualNodeHeader(ptPool);

    ptPool->ptPrevPool = m_ptLastUnsizedPool[dwPoolIndex];
    m_ptLastUnsizedPool[dwPoolIndex] = ptPool;

    if (ptPool->ptPrevPool != NULL) {
        ptPool->ptPrevPool->ptNextPool = ptPool;
    } else {
        m_ptFirstUnsizedPool[dwPoolIndex] = ptPool;
    }

    return ptPool;
}












inline void XVirtualHeap::FreeUnsizedPool(XVirtualUnsizedPoolNode* ptPool) {
    uint32 dwPoolIndex = ptPool->dwPoolIndex;

    if (ptPool->ptPrevPool != NULL) {
        ptPool->ptPrevPool->ptNextPool = ptPool->ptNextPool;
    } else {
        m_ptFirstUnsizedPool[dwPoolIndex] = ptPool->ptNextPool;
    }

    if (ptPool->ptNextPool != NULL) {
        ptPool->ptNextPool->ptPrevPool = ptPool->ptPrevPool;
    } else {
        m_ptLastUnsizedPool[dwPoolIndex] = ptPool->ptPrevPool;
    }

    UnregisterVirtualNodeHeader(ptPool);









    sFreeFromHeap(m_ptNodesHeap, ptPool);
}










































































































































































































XVirtualNodeHeader* XVirtualHeap::PlatformVirtualAlloc(XVirtualNodeType eType, size_t size, size_t align) {
    uintptr_t address = 0;
    BOOL fCreatePoolInPadding;



    size_t dwAllocSize = AlignUp(size, XVIRTUAL_PAGE_SIZE);
    XVirtualNodeHeader* ptHeader = NULL;
    void* pMem;

    fCreatePoolInPadding = (dwAllocSize - size) >= 16;

    nn::Result result = nn::os::AllocateAddressRegion(&address, dwAllocSize);
    if (result.IsFailure()) {
        return NULL;
    }

    result = nn::os::AllocateMemoryPages(address, dwAllocSize);
    if (result.IsFailure()) {
        nn::os::FreeAddressRegion(address);
        return NULL;
    }

    pMem = (void*)address;

    if (eType == XVIRTUAL_NODE_TYPE_POOL) {
        XVirtualPoolNode* ptPool;
        uint32 dwItemCount = XVIRTUAL_PAGE_SIZE / size;


















































































































        ptPool = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualPoolNode) + dwItemCount * sizeof(XVirtualPoolNodeItem), "", 0))
            XVirtualPoolNode((uintptr)pMem, XVIRTUAL_PAGE_SIZE, dwItemCount);
        ptHeader = ptPool;
    } else {

        ptHeader = new (sAllocateFromHeap(m_ptNodesHeap, sizeof(XVirtualNodeHeader), "", 0))
            XVirtualNodeHeader((uintptr)pMem, size, XVIRTUAL_NODE_TYPE_HEADER);

        if (size > XVIRTUAL_MAX_NODE_SIZE) {
            ptHeader->dwSize = XVIRTUAL_MAX_NODE_SIZE;
        }

        if (fCreatePoolInPadding) {
            XVirtualUnsizedPoolNode* ptPool = CreateUnsizedPool(pMem, dwAllocSize, size);
        }
    }

    return ptHeader;
}


























void XVirtualHeap::PlatformVirtualFree(XVirtualNodeHeader* ptHeader) {
    size_t size = ptHeader->dwSize;
    BOOL fKeepLastPage = FALSE;
    uintptr_t address = ptHeader->dwData;
    size_t dwAllocSize = AlignUp(size, XVIRTUAL_PAGE_SIZE);
    XVirtualNodeHeader* ptUnsizedPoolHeader = NULL;

    if (ptHeader->eType == XVIRTUAL_NODE_TYPE_HEADER && dwAllocSize - size >= 16) {
        ptUnsizedPoolHeader = FindVirtualNodeHeader((void*)(address + dwAllocSize - XVIRTUAL_PAGE_SIZE));

        if (ptUnsizedPoolHeader->eType == XVIRTUAL_NODE_TYPE_UNSIZED_POOL) {







            FreeUnsizedPool((XVirtualUnsizedPoolNode*)ptUnsizedPoolHeader);
        } else {
            fKeepLastPage = TRUE;
        }
    }

    if (ptHeader->dwSize > 0x8000 && ptHeader->eType == XVIRTUAL_NODE_TYPE_POOL) {
        address = ptHeader->dwData + XVIRTUAL_PAGE_SIZE - dwAllocSize;
    }

    if (fKeepLastPage) {
        nn::Result nn_result = nn::os::FreeMemoryPages(address, dwAllocSize - XVIRTUAL_PAGE_SIZE);
    } else {
        {
            nn::Result nn_result = nn::os::FreeMemoryPages(address, dwAllocSize);
        }
        {
            nn::Result nn_result = nn::os::FreeAddressRegion(address);
        }
    }






























































    sFreeFromHeap(m_ptNodesHeap, ptHeader);

    if (fKeepLastPage) {
        XVirtualPoolNode* ptPool = (XVirtualPoolNode*)ptUnsizedPoolHeader;
        size_t dwPaddedSize;
        uint32 dwItemSize;
        uint32 dwBlockedItemCount;

        m_dwHeapPaddedUsedSize += XVIRTUAL_PAGE_SIZE;
        ptPool->dwBlockedCount = 0;
        dwPaddedSize = AlignUp(ptPool->dwSize, XVIRTUAL_PAGE_SIZE);
        dwItemSize = 16 << ptPool->dwPoolIndex;
        dwBlockedItemCount = (uint16)(AlignUp(ptPool->dwSize + XVIRTUAL_PAGE_SIZE - dwPaddedSize, dwItemSize) / dwItemSize);

        m_dwHeapUsedSize += dwBlockedItemCount * dwItemSize;

        for (int32 i = dwBlockedItemCount - 1; i >= 0; i--) {
            FreePoolItem(ptPool, (void*)(ptPool->dwData + i * dwItemSize));
        }
    }
}
