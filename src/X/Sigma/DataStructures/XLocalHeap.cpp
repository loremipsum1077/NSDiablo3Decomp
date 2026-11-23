#include "XLocalHeap.h"
#include <new>
#include "../Math/MathGeneral.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Memory/SigmaMemory.h"



















inline size_t XLocalHeap::AlignUp(size_t dwValue, size_t dwAlign) {
    return (dwValue + dwAlign - 1) & ~(dwAlign - 1);
}




inline size_t XLocalHeap::AlignDown(size_t dwValue, size_t dwAlign) {
    return dwValue & ~(dwAlign - 1);
}




XLocalHeap::XLocalHeap(void* pHeapBuffer, size_t dwHeapBufferSize, size_t dwMinAllocSize, size_t dwMaxAllocSize)
    : m_dwHeapUsedSize(0),
      m_dwNodeCount(0),
      m_dwMinAllocSize(dwMinAllocSize),
      m_ptSmallSubHeap(NULL),
      m_dwMaxSmallAllocSize(0) {
    size_t dwCurAllocSize;
    size_t dwBase = (size_t)pHeapBuffer;
    XLocalHeapNode* ptFirstNode;
    size_t dwAlignOffset;

    // Invented: nothing in the binary uses dwMaxAllocSize, but CreateSmallSubHeap still loads it for this call,
    // so the original had some use of it that only died after dead-argument elimination had run.
    dwMaxAllocSize = AlignUp(dwMaxAllocSize, sizeof(XLocalHeapNode));
    dwAlignOffset = AlignUp(dwBase, sizeof(XLocalHeapNode)) - dwBase;
    m_pHeapBuffer = (void*)(dwBase + dwAlignOffset);
    dwHeapBufferSize = AlignDown(dwHeapBufferSize - dwAlignOffset, sizeof(XLocalHeapNode));
    m_dwHeapBufferSize = dwHeapBufferSize;
    m_dwMaxExponentialAllocSize = dwHeapBufferSize;

    for (uint32 i = 0; i < 32; i++)
        m_ptFreeNodes[i] = NULL;

    dwCurAllocSize = dwMinAllocSize;
    for (uint32 i = 0; i < 32; i++) {
        size_t dwRemaining = dwHeapBufferSize - dwCurAllocSize;
        if (dwCurAllocSize > dwRemaining / (32 - i)) {
            m_dwMaxExponentialAllocSize = dwCurAllocSize;
            break;
        }
        dwCurAllocSize <<= 1;
    }
















    m_dwBaseLog2Index = (uint32)Math::FloorLog2(dwMinAllocSize);
    m_dwMaxLog2Index = (uint32)Math::FloorLog2(m_dwMaxExponentialAllocSize) - m_dwBaseLog2Index;
    m_dwLinearStep = (dwHeapBufferSize - m_dwMaxExponentialAllocSize) / (32 - m_dwMaxLog2Index);

    ptFirstNode = (XLocalHeapNode*)m_pHeapBuffer;
    m_ptLastHeapNode = (XLocalHeapNode*)((uint8*)ptFirstNode + dwHeapBufferSize);

    ptFirstNode->ptPrevNode = NULL;
    ptFirstNode->dwUsed = 0;
    ptFirstNode->dwSize = dwHeapBufferSize - sizeof(XLocalHeapNode);
    m_dwNodeCount++;
    m_dwBiggestContiguousFreeSize = ptFirstNode->dwSize;
    m_fBiggestDirty = FALSE;



    XASSERT(ptFirstNode->GetNextNode() == m_ptLastHeapNode);

    AddToFreeList(ptFirstNode);
}


XLocalHeap::~XLocalHeap() {
}




void* XLocalHeap::MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) {
    uint32 dwFreeSlot;
    XLocalHeapNode* ptBestNode;

    if (size == 0)
        return m_ptLastHeapNode + 1;

    if (align <= sizeof(XLocalHeapNode))
        align = sizeof(XLocalHeapNode);









    size = AlignUp(size, sizeof(XLocalHeapNode));

    if (size < m_dwMaxSmallAllocSize) {
        void* pMem = m_ptSmallSubHeap->MemTryAllocTracked(size, align, bClear, szFile, dwLine);
        if (pMem != NULL)
            return pMem;
    }










    dwFreeSlot = GetFreeSlotIndex(size);

    Lock();

    ptBestNode = NULL;
    do {
        int32 nMinRemaining = 0x7FFFFFFF;
        XLocalHeapNode* ptNode = m_ptFreeNodes[dwFreeSlot++];
        while (ptNode != NULL) {







            int32 nRemaining = NodeRemainingSpaceFor(ptNode, size, align);
            if (nRemaining >= 0) {
                if (nRemaining <= (int32)m_dwMinAllocSize) {
                    ptBestNode = ptNode;
                    break;
                }
                if (nMinRemaining >= nRemaining) {
                    ptBestNode = ptNode;
                    nMinRemaining = nRemaining;
                }
            }
            ptNode = ptNode->ptNextFreeNode;
        }
        if (dwFreeSlot == 32) {
            if (ptBestNode == NULL) {



















                Unlock();
                return NULL;
            }
            break;
        }
    } while (ptBestNode == NULL);

    RemoveFromFreeList(ptBestNode);










    ptBestNode = AlignNodeData(ptBestNode, align);

    ptBestNode->szFile = szFile;
    ptBestNode->dwLine = dwLine;
    ptBestNode->dwUsed = 1;
    Truncate(ptBestNode, size);

    m_dwHeapUsedSize += ptBestNode->dwSize;

    Unlock();

    void* pMem = ptBestNode + 1;
    if (bClear)

        ZeroMemory(pMem, size);

    return pMem;
}










void* XLocalHeap::MemAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) {
    void* pMem = MemTryAllocTracked(size, align, bClear, szFile, dwLine);
    if (pMem == NULL) {
        Dump();

        char str[64];
        Blizzard::String::Format(str, 64, "Error: allocation of %zu bytes failed!", size);









        XASSERT(FALSE);
    }
    return pMem;
}


void* XLocalHeap::MemAlloc(size_t size, size_t align, BOOL bClear) {
    return MemAllocTracked(size, align, bClear, "", 0);
}





void XLocalHeap::MemFree(void* pMem) {
    if (pMem == NULL)
        return;

    if (m_ptSmallSubHeap != NULL && m_ptSmallSubHeap->ContainsMem(pMem)) {
        m_ptSmallSubHeap->MemFree(pMem);
        return;
    }

    if (!ContainsMem(pMem))
        return;

    XLocalHeapNode* ptNode = (XLocalHeapNode*)pMem - 1;
    if (ptNode == m_ptLastHeapNode)
        return;


















    Lock();
    if (ptNode->dwUsed) {
        ptNode->dwUsed = 0;
        m_dwHeapUsedSize -= ptNode->dwSize;
        AddToFreeList(ptNode);
    }
    Unlock();
}

















size_t XLocalHeap::MemSize(void* pMem) {
    if (pMem == NULL)
        return 0;

    if (m_ptSmallSubHeap != NULL && m_ptSmallSubHeap->ContainsMem(pMem))
        return m_ptSmallSubHeap->MemSize(pMem);

    XLocalHeapNode* ptNode = (XLocalHeapNode*)pMem - 1;
    if (ptNode == m_ptLastHeapNode)
        return 0;

    return ptNode->dwSize;
}












void* XLocalHeap::MemResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
    pMem = MemTryResizeTracked(pMem, size, align, szFile, dwLine);
    if (pMem == NULL) {
        Dump();

        char str[64];
        Blizzard::String::Format(str, 64, "Error: allocation of %zu bytes failed!", size);









        XASSERT(FALSE);
    }
    return pMem;
}


void* XLocalHeap::MemTryResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
    void* pDest;
    size_t dwMissingSize;
    XLocalHeapNode* ptNext;
    XLocalHeapNode* ptNode;
    size_t dwBase;

    if (pMem == NULL)
        return MemTryAllocTracked(size, align, FALSE, szFile, dwLine);

    ptNode = (XLocalHeapNode*)pMem - 1;

    if (m_ptSmallSubHeap != NULL && m_ptSmallSubHeap->ContainsMem(pMem)) {
        if (size < m_dwMaxSmallAllocSize) {
            void* lpResized = m_ptSmallSubHeap->MemTryResizeTracked(pMem, size, align, szFile, dwLine);
            if (lpResized != NULL)
                return lpResized;
        }

        void* pDest = MemTryAllocTracked(size, align, FALSE, szFile, dwLine);
        if (pDest == NULL)
            return NULL;















        SigmaMemoryCopy(pDest, pMem, Math::Min<size_t>(ptNode->dwSize, size));
        MemFree(pMem);
        return pDest;
    }

    if (ptNode == m_ptLastHeapNode)
        return MemTryAllocTracked(size, align, FALSE, szFile, dwLine);

    if (align <= sizeof(XLocalHeapNode))
        align = sizeof(XLocalHeapNode);












    size = AlignUp(size, sizeof(XLocalHeapNode));

    Lock();

    XASSERT(ptNode->dwUsed);

    dwBase = ptNode->dwSize;
    if (size <= dwBase) {
        m_dwHeapUsedSize -= dwBase;
        Truncate(ptNode, size);
        m_dwHeapUsedSize += ptNode->dwSize;
        Unlock();
        return pMem;
    }



















    ptNext = ptNode->GetNextNode();
    if (AlignUp((size_t)pMem, align) == (size_t)pMem && ptNext != NULL && ptNext != m_ptLastHeapNode &&
        !ptNext->dwUsed && dwBase + ptNext->dwSize + sizeof(XLocalHeapNode) >= size) {
        m_dwHeapUsedSize -= dwBase;

        RemoveFromFreeList(ptNext);
        ptNext->dwUsed = 1;
        dwMissingSize = size - sizeof(XLocalHeapNode) - dwBase;
        Truncate(ptNext, dwMissingSize);

        ptNode->dwSize += ptNext->dwSize + sizeof(XLocalHeapNode);
        m_dwNodeCount--;







        ptNext = ptNode->GetNextNode();
        if (ptNext != m_ptLastHeapNode)
            ptNext->ptPrevNode = ptNode;

        m_dwHeapUsedSize += ptNode->dwSize;



        Unlock();
        return pMem;
    }










    Unlock();

    pDest = MemTryAllocTracked(size, align, FALSE, ptNode->szFile, ptNode->dwLine);
    if (pDest == NULL)
        return NULL;

    SigmaMemoryCopy(pDest, pMem, ptNode->dwSize);
    MemFree(pMem);
    return pDest;
}











void* XLocalHeap::MemResize(void* pMem, size_t size, size_t align) {
    return MemResizeTracked(pMem, size, align, "", 0);
}





BOOL XLocalHeap::ContainsMem(void* pMem) {
    if (m_ptSmallSubHeap != NULL && m_ptSmallSubHeap->ContainsMem(pMem))
        return TRUE;

    return pMem >= m_pHeapBuffer && pMem <= (uint8*)m_pHeapBuffer + m_dwHeapBufferSize + sizeof(XLocalHeapNode);
}





void XLocalHeap::PrintUsage() {
    size_t dwUsedMemory = 0;
    size_t dwMaxContiguousUsed = 0;
    size_t dwMaxContiguousFree = 0;
    uint32 dwUsedNodes = 0;
    XLocalHeapNode* ptNode;
    XLocalHeapNode* ptFirstNode;

    Lock();

    ptFirstNode = (XLocalHeapNode*)m_pHeapBuffer;
    XLocalHeapNode* ptPrev = NULL;
    for (ptNode = ptFirstNode; ptNode != m_ptLastHeapNode; ptNode = ptNode->GetNextNode()) {
        if (ptNode->dwUsed) {
            dwUsedMemory += ptNode->dwSize;
            dwUsedNodes++;
            dwMaxContiguousUsed = Math::Max<size_t>(dwMaxContiguousUsed, ptNode->dwSize);
        } else {
            dwMaxContiguousFree = Math::Max<size_t>(dwMaxContiguousFree, ptNode->dwSize);
        }
        ptPrev = ptNode;
    }

















    Unlock();

    size_t dwWastedByNodesHeader = m_dwNodeCount * sizeof(XLocalHeapNode);
    (void)dwWastedByNodesHeader;

    if (m_ptSmallSubHeap != NULL)
        m_ptSmallSubHeap->PrintUsage();
}







void XLocalHeap::Dump() {
    PrintUsage();
}



































































































































































































































void XLocalHeap::Lock() {


    m_tLock.Enter();
}

void XLocalHeap::Unlock() {


    m_tLock.Leave();
}

int32 XLocalHeap::NodeRemainingSpaceFor(XLocalHeapNode* ptNode, size_t dwSize, size_t dwAlign) const {
    size_t dwBase = (size_t)(ptNode + 1);
    int64 nRemaining = (int64)(dwBase - ((dwBase + dwAlign - 1) & ~(dwAlign - 1)) + ptNode->dwSize - dwSize);
    return (int32)(nRemaining < 0x7FFFFFFF ? nRemaining : 0x7FFFFFFF);
}












XLocalHeapNode* XLocalHeap::AlignNodeData(XLocalHeapNode* ptNode, size_t dwAlign) {
    size_t dwBase = (size_t)(ptNode + 1);
    size_t dwAlignOffset;


    dwAlignOffset = AlignUp(dwBase, dwAlign) - dwBase;
    if (dwAlignOffset != 0) {






        XASSERT(dwAlignOffset >= sizeof(XLocalHeapNode));

        XLocalHeapNode tCopy = *ptNode;
        XLocalHeapNode* ptNext = (XLocalHeapNode*)(dwBase + dwAlignOffset - sizeof(XLocalHeapNode));

        m_dwNodeCount++;
        ptNode->dwUsed = 0;
        ptNode->dwSize = dwAlignOffset - sizeof(XLocalHeapNode);
        ptNext->dwUsed = 1;
        AddToFreeList(ptNode);

        ptNext->ptPrevNode = ptNode;
        ptNext->dwSize = tCopy.dwSize - dwAlignOffset;
        ptNode = ptNext;




        ptNext = ptNode->GetNextNode();
        if (ptNext != m_ptLastHeapNode)
            ptNext->ptPrevNode = ptNode;
    }
    return ptNode;
}



void XLocalHeap::AddToFreeList(XLocalHeapNode* ptFreeNode) {


    XASSERT(!ptFreeNode->dwUsed);


    XLocalHeapNode* ptNextNode = ptFreeNode->GetNextNode();
    if (ptNextNode != m_ptLastHeapNode && !ptNextNode->dwUsed) {
        RemoveFromFreeList(ptNextNode);

        ptFreeNode->dwSize += ptNextNode->dwSize + sizeof(XLocalHeapNode);


        XASSERT(ptNextNode->GetNextNode() == ptFreeNode->GetNextNode());
        m_dwNodeCount--;

        ptNextNode = ptFreeNode->GetNextNode();
        if (ptNextNode != m_ptLastHeapNode) {

            XASSERT(ptNextNode->dwUsed);
            ptNextNode->ptPrevNode = ptFreeNode;
        }
    }

    XLocalHeapNode* ptPrevNode = ptFreeNode->ptPrevNode;
    if (ptPrevNode != NULL && !ptPrevNode->dwUsed) {
        RemoveFromFreeList(ptPrevNode);

        ptPrevNode->dwSize += ptFreeNode->dwSize + sizeof(XLocalHeapNode);



        XASSERT(ptPrevNode->GetNextNode() == ptNextNode);
        m_dwNodeCount--;

        if (ptNextNode != m_ptLastHeapNode) {

            XASSERT(ptNextNode->dwUsed);
            ptNextNode->ptPrevNode = ptPrevNode;
        }

        ptFreeNode = ptPrevNode;
    }


    uint32 dwFreeSlot = GetFreeSlotIndex(ptFreeNode->dwSize);
    XLocalHeapNode* ptNext = m_ptFreeNodes[dwFreeSlot];
    ptFreeNode->ptNextFreeNode = ptNext;
    ptFreeNode->ptPrevFreeNode = NULL;
    if (ptNext != NULL)
        ptNext->ptPrevFreeNode = ptFreeNode;

    if (ptFreeNode->dwSize > m_dwBiggestContiguousFreeSize)
        m_dwBiggestContiguousFreeSize = ptFreeNode->dwSize;

    m_ptFreeNodes[dwFreeSlot] = ptFreeNode;
}






void XLocalHeap::RemoveFromFreeList(XLocalHeapNode* ptFreeNode) {
    if (ptFreeNode->dwSize == m_dwBiggestContiguousFreeSize)
        m_fBiggestDirty = TRUE;

    XLocalHeapNode* ptNext = ptFreeNode->ptNextFreeNode;
    XLocalHeapNode* ptPrev = ptFreeNode->ptPrevFreeNode;

    if (ptPrev != NULL) {
        ptPrev->ptNextFreeNode = ptNext;
    } else {








        uint32 dwFreeSlot = GetFreeSlotIndex(ptFreeNode->dwSize);
        XASSERT(m_ptFreeNodes[dwFreeSlot] == ptFreeNode);
        m_ptFreeNodes[dwFreeSlot] = ptNext;
    }

    if (ptNext != NULL)
        ptNext->ptPrevFreeNode = ptPrev;
}



void XLocalHeap::Truncate(XLocalHeapNode* ptNode, size_t dwSize) {



    XASSERT(ptNode->dwUsed);

    size_t dwOriginalSize = ptNode->dwSize;
    if (dwOriginalSize - dwSize < m_dwMinAllocSize + sizeof(XLocalHeapNode))
        return;



    XLocalHeapNode* ptNextNode = ptNode->GetNextNode();
    ptNode->dwSize = dwSize;



    XLocalHeapNode* ptNewNode = ptNode->GetNextNode();
    ptNewNode->dwSize = dwOriginalSize - dwSize - sizeof(XLocalHeapNode);
    XASSERT(ptNewNode->dwSize >= sizeof(XLocalHeapNode));
    ptNewNode->ptPrevNode = ptNode;
    ptNewNode->dwUsed = 0;
    m_dwNodeCount++;

    XASSERT(ptNewNode->GetNextNode() == ptNextNode);
    if (ptNextNode != m_ptLastHeapNode)
        ptNextNode->ptPrevNode = ptNewNode;

    AddToFreeList(ptNewNode);
}





uint32 XLocalHeap::GetFreeSlotIndex(size_t dwSize) const {
    if (dwSize == 0)
        return 0;

    if (dwSize <= m_dwMinAllocSize)
        return 1;

    if (dwSize < m_dwMaxExponentialAllocSize) {











        return (uint32)Math::FloorLog2(dwSize) - m_dwBaseLog2Index + 1;
    }



    return Math::Min<uint32>((uint32)((dwSize - m_dwMaxExponentialAllocSize) / m_dwLinearStep + m_dwMaxLog2Index), 30) + 1;
}

void XLocalHeap::CreateSmallSubHeap(size_t dwHeapSize, size_t dwMaxAllocSize) {
    void* pSmallSubHeapMemory = MemAlloc(dwHeapSize, sizeof(XLocalHeapNode), FALSE);
    void* pSmallSubHeap = MemAlloc(sizeof(XLocalHeap), sizeof(XLocalHeapNode), FALSE);
    m_ptSmallSubHeap = new (pSmallSubHeap) XLocalHeap(pSmallSubHeapMemory, dwHeapSize, sizeof(XLocalHeapNode), dwMaxAllocSize);
    m_dwMaxSmallAllocSize = dwMaxAllocSize;
}
