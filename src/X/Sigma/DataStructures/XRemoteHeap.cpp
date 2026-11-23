#include "XRemoteHeap.h"

#include "../Math/MathGeneral.h"
#include "XLocalHeap.h"

#include <stdio.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Memory/SigmaMemory.h"
















inline size_t XRemoteHeap::AlignUp(size_t dwValue, size_t dwAlign) {
    return (dwValue + dwAlign - 1) & ~(dwAlign - 1);
}




inline size_t XRemoteHeap::AlignDown(size_t dwValue, size_t dwAlign) {
    return dwValue & ~(dwAlign - 1);
}




XRemoteHeap::XRemoteHeap(XLocalHeap* ptNodesHeap, void* pHeapBuffer, size_t dwHeapBufferSize, size_t dwMinAllocSize,
                         size_t dwMaxAllocSize)
    : m_ptNodesHeap(ptNodesHeap),
      m_pHeapBuffer(pHeapBuffer),
      m_dwHeapBufferSize(dwHeapBufferSize),
      m_dwHeapUsedSize(0),
      m_dwMinAllocSize(dwMinAllocSize) {
    size_t dwBase;
    size_t dwCurAllocSize;
    XRemoteHeapNode* ptFirstNode;
    size_t dwAlignOffset;

    m_dwBucketSize = dwHeapBufferSize / 512;
    if (m_dwBucketSize * 512 < dwHeapBufferSize) {
        m_dwBucketSize++;
    }

    for (uint32 i = 0; i < 512; i++) {
        m_ptNodeBuckets[i] = NULL;
        m_ptLastNodeBuckets[i] = NULL;
    }

    if (m_ptNodesHeap != NULL) {
        m_ptNodeContainers = (XRemoteHeapNodeContainer*)m_ptNodesHeap->MemAlloc(
            sizeof(XRemoteHeapNodeContainer) + 4096 * sizeof(XRemoteHeapNode), 16, FALSE);
    } else {
        m_ptNodeContainers = (XRemoteHeapNodeContainer*)SigmaMemoryNew(
            sizeof(XRemoteHeapNodeContainer) + 4096 * sizeof(XRemoteHeapNode), 4, NULL, FALSE);
    }
    m_ptNodeContainers->nMinUnallocatedNodeIndex = 0;
    m_ptNodeContainers->nNodeCount = 4096;
    m_ptNodeContainers->ptNext = NULL;
    m_ptNodeContainers->ptNodes = (XRemoteHeapNode*)(m_ptNodeContainers + 1);
    for (uint32 i = 0; i < 4096; i++) {
        m_ptNodeContainers->ptNodes[i].dwAllocated = 0;
    }

    dwBase = (size_t)pHeapBuffer;







    dwAlignOffset = AlignUp(dwBase, 32) - dwBase;
    m_pHeapBuffer = (void*)(dwBase + dwAlignOffset);
    m_dwHeapBufferSize = AlignDown(dwHeapBufferSize - dwAlignOffset, 32);

    m_dwMaxExponentialAllocSize = m_dwHeapBufferSize;
    for (uint32 i = 0; i < 32; i++) {
        m_ptFreeNodes[i] = NULL;
    }

    dwCurAllocSize = dwMinAllocSize;
    for (uint32 i = 0; i < 32; i++) {
        size_t dwRemaining = m_dwHeapBufferSize - dwCurAllocSize;
        if (dwCurAllocSize > dwRemaining / (32 - i)) {
            m_dwMaxExponentialAllocSize = dwCurAllocSize;
            break;
        }
        dwCurAllocSize *= 2;
    }
















    m_dwBaseLog2Index = (uint32)Math::FloorLog2(dwMinAllocSize);
    m_dwMaxLog2Index = (uint32)Math::FloorLog2(m_dwMaxExponentialAllocSize) - m_dwBaseLog2Index;
    m_dwLinearStep = (m_dwHeapBufferSize - m_dwMaxExponentialAllocSize) / (32 - m_dwMaxLog2Index);

    ptFirstNode = AddNewNode(m_pHeapBuffer);
    ptFirstNode->dwSize = m_dwHeapBufferSize;
    ptFirstNode->dwUsed = 0;
    m_dwBiggestContiguousFreeSize = ptFirstNode->dwSize;
    m_fBiggestDirty = FALSE;
    AddToFreeList(ptFirstNode, NULL);
}






XRemoteHeap::~XRemoteHeap() {
    XRemoteHeapNodeContainer* ptNodeContainer = m_ptNodeContainers;
    while (ptNodeContainer != NULL) {
        XRemoteHeapNodeContainer* ptNodeContainerToDel = ptNodeContainer;
        ptNodeContainer = ptNodeContainer->ptNext;
        if (m_ptNodesHeap != NULL) {
            m_ptNodesHeap->MemFree(ptNodeContainerToDel);
        } else {
            SigmaMemoryFree(ptNodeContainerToDel, NULL);
        }
    }
}







void* XRemoteHeap::MemTryAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) {
    uint32 dwFreeSlot;
    XRemoteHeapNode* ptBestNode;

    if (size == 0) {
        return (void*)((size_t)m_pHeapBuffer + m_dwHeapBufferSize);
    }

    if (align <= 32) {
        align = 32;
    }







    size = AlignUp(size, 32);

    dwFreeSlot = GetFreeSlotIndex(size);

    Lock();

    ptBestNode = NULL;
    do {
        int32 nMinRemaining = 0x7fffffff;
        XRemoteHeapNode* ptNode = m_ptFreeNodes[dwFreeSlot++];
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
    ptBestNode->dwUsed = 1;
    ptBestNode->szFile = szFile;
    ptBestNode->dwLine = dwLine;

    Truncate(ptBestNode, size);

    m_dwHeapUsedSize += ptBestNode->dwSize;

    Unlock();

    if (bClear) {


        ZeroMemory(ptBestNode->pData, size);
    }

    return ptBestNode->pData;
}









void* XRemoteHeap::MemAllocTracked(size_t size, size_t align, BOOL bClear, const char* szFile, uint32 dwLine) {
    void* pMem = MemTryAllocTracked(size, align, bClear, szFile, dwLine);
    if (pMem == NULL) {
        Dump();









        XASSERT(pMem != NULL);
    }
    return pMem;
}


void* XRemoteHeap::MemAlloc(size_t size, size_t align, BOOL bClear) {
    return MemAllocTracked(size, align, bClear, "", 0);
}





void XRemoteHeap::MemFree(void* pMem) {
    uint32 dwIndex;
    XRemoteHeapNode* ptNode;
    XRemoteHeapNode* ptPrev;

    if (pMem == NULL || pMem == (void*)((size_t)m_pHeapBuffer + m_dwHeapBufferSize)) {
        return;
    }






    dwIndex = GetNodeBucket(pMem);

    Lock();


    ptNode = GetHeapNode(pMem, dwIndex, ptPrev);

    ptNode->dwUsed = 0;
    m_dwHeapUsedSize -= ptNode->dwSize;

    AddToFreeList(ptNode, ptPrev);








    Unlock();
}





size_t XRemoteHeap::MemSize(void* pMem) {
    XRemoteHeapNode* ptNode;
    size_t dwSize;
    uint32 dwIndex;
    XRemoteHeapNode* ptPrev;

    if (pMem == NULL || pMem == (void*)((size_t)m_pHeapBuffer + m_dwHeapBufferSize)) {
        return 0;
    }





    dwIndex = GetNodeBucket(pMem);

    Lock();


    ptNode = GetHeapNode(pMem, dwIndex, ptPrev);
    dwSize = ptNode->dwSize;


    Unlock();
    return dwSize;
}


void* XRemoteHeap::MemResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
    pMem = MemTryResizeTracked(pMem, size, align, szFile, dwLine);
    if (pMem == NULL) {
        Dump();









        XASSERT(pMem != NULL);
    }
    return pMem;
}


void* XRemoteHeap::MemTryResizeTracked(void* pMem, size_t size, size_t align, const char* szFile, uint32 dwLine) {
    size_t dwBase;
    uint32 dwIndex;
    XRemoteHeapNode* ptNode;
    size_t dwMissingSize;
    XRemoteHeapNode* ptNext;
    void* pDest;
    XRemoteHeapNode* ptPrev;

    if (pMem == NULL || pMem == (void*)((size_t)m_pHeapBuffer + m_dwHeapBufferSize)) {
        return MemTryAllocTracked(size, align, FALSE, szFile, dwLine);
    }

    dwBase = (size_t)pMem;
    if (align <= 32) {
        align = 32;
    }






    size = AlignUp(size, 32);

    dwIndex = GetNodeBucket(pMem);

    Lock();


    ptNode = GetHeapNode(pMem, dwIndex, ptPrev);

    if (size <= ptNode->dwSize) {
        m_dwHeapUsedSize -= ptNode->dwSize;









        Truncate(ptNode, size);
        m_dwHeapUsedSize += ptNode->dwSize;


        Unlock();
        return pMem;
    }

    dwMissingSize = size - ptNode->dwSize;
    ptNext = ptNode->ptNextNode;









    if (AlignUp(dwBase, align) == dwBase && ptNext != NULL && !ptNext->dwUsed &&
        ptNode->dwSize + ptNext->dwSize >= size) {
        size_t dwBackupSize;

        m_dwHeapUsedSize -= ptNode->dwSize;

        RemoveFromFreeList(ptNext);
        ptNext->dwUsed = 1;




        Truncate(ptNext, dwMissingSize);

        dwBackupSize = ptNext->dwSize;
        RemoveNode(ptNext);
        ptNode->dwSize += dwBackupSize;

        m_dwHeapUsedSize += ptNode->dwSize;
    } else {















        Unlock();

        pDest = MemTryAllocTracked(size, align, FALSE, ptNode->szFile, ptNode->dwLine);
        if (pDest != NULL) {
            SigmaMemoryCopy(pDest, pMem, ptNode->dwSize);
            MemFree(pMem);
        }
        return pDest;
    }

    Unlock();
    return pMem;
}








void* XRemoteHeap::MemResize(void* pMem, size_t size, size_t align) {
    return MemResizeTracked(pMem, size, align, "", 0);
}





BOOL XRemoteHeap::ContainsMem(void* pMem) {
    return pMem >= m_pHeapBuffer && pMem <= (void*)(m_dwHeapBufferSize + (size_t)m_pHeapBuffer);
}



void XRemoteHeap::PrintUsage() {
    size_t dwUsedMemory = 0;
    size_t dwMaxContiguousUsed = 0;
    size_t dwMaxContiguousFree = 0;
    uint32 dwAllocatedNodes = 0;
    uint32 dwUsedNodes = 0;
    uint32 nMaxNodeCount = 0;
    XRemoteHeapNodeContainer* ptNodeContainer;
    size_t dwCurrentBase = (size_t)m_pHeapBuffer;
    Lock();

    for (uint32 dwBucket = 0; dwBucket < 512; dwBucket++) {
        XRemoteHeapNode* ptNode = m_ptNodeBuckets[dwBucket];
        XRemoteHeapNode* ptFirstNode = ptNode;
        while (ptNode != NULL) {
            if (ptNode->dwUsed) {
                dwUsedMemory += ptNode->dwSize;
                dwUsedNodes++;
                if (ptNode->dwSize > dwMaxContiguousUsed) {
                    dwMaxContiguousUsed = ptNode->dwSize;
                }
            } else {
                if (ptNode->dwSize > dwMaxContiguousFree) {
                    dwMaxContiguousFree = ptNode->dwSize;
                }
            }
            dwCurrentBase += ptNode->dwSize;
            ptNode = ptNode->ptNextNode;
        }
    }

    ptNodeContainer = m_ptNodeContainers;
    while (ptNodeContainer != NULL) {
        for (int32 i = 0; i < ptNodeContainer->nNodeCount; i++) {
            if (ptNodeContainer->ptNodes[i].dwAllocated) {
                dwAllocatedNodes++;
            }
        }
        nMaxNodeCount += ptNodeContainer->nNodeCount;
        ptNodeContainer = ptNodeContainer->ptNext;
    }


















    Unlock();
}




void XRemoteHeap::Dump() {
    char szDumpLocalBuffer[1024];
    size_t dwUsedMemory = 0;
    size_t dwMaxContiguousUsed = 0;
    size_t dwMaxContiguousFree = 0;
    uint32 dwAllocatedNodes = 0;
    uint32 dwUsedNodes = 0;
    uint32 dwDumpLocalBufferLen = 0;
    uint32 nMaxNodeCount = 0;
    XRemoteHeapNodeContainer* ptNodeContainer;
    size_t dwCurrentBase = (size_t)m_pHeapBuffer;
    Lock();

    for (uint32 dwBucket = 0; dwBucket < 512; dwBucket++) {
        XRemoteHeapNode* ptNode = m_ptNodeBuckets[dwBucket];
        XRemoteHeapNode* ptFirstNode = ptNode;
        while (ptNode != NULL) {
            if (ptNode->dwUsed) {
                dwUsedMemory += ptNode->dwSize;
                dwUsedNodes++;
                if (ptNode->dwSize > dwMaxContiguousUsed) {
                    dwMaxContiguousUsed = ptNode->dwSize;
                }
                if (ptNode->dwLine != 0) {
                    dwDumpLocalBufferLen += sprintf(szDumpLocalBuffer + dwDumpLocalBufferLen, "%s(%u)\t%p\t%zu\n",
                                                    ptNode->szFile, ptNode->dwLine, ptNode->pData,
                                                    (size_t)ptNode->dwSize);
                } else {
                    dwDumpLocalBufferLen += sprintf(szDumpLocalBuffer + dwDumpLocalBufferLen, "Unknown(%p)\t%p\t%zu\n",
                                                    ptNode->szFile, ptNode->pData, (size_t)ptNode->dwSize);
                }
            } else {
                if (ptNode->dwSize > dwMaxContiguousFree) {
                    dwMaxContiguousFree = ptNode->dwSize;
                }
                dwDumpLocalBufferLen += sprintf(szDumpLocalBuffer + dwDumpLocalBufferLen, "** FREE BLOCK **\t%p\t%zu\n",
                                                ptNode->pData, (size_t)ptNode->dwSize);
            }
            dwCurrentBase += ptNode->dwSize;
            ptNode = ptNode->ptNextNode;

            if (dwDumpLocalBufferLen > sizeof(szDumpLocalBuffer) - 300) {
                szDumpLocalBuffer[dwDumpLocalBufferLen] = '\0';
                dwDumpLocalBufferLen = 0;
            }
        }
    }

    if (dwDumpLocalBufferLen != 0) {
        szDumpLocalBuffer[dwDumpLocalBufferLen] = '\0';
    }

    ptNodeContainer = m_ptNodeContainers;
    while (ptNodeContainer != NULL) {
        for (int32 i = 0; i < ptNodeContainer->nNodeCount; i++) {
            if (ptNodeContainer->ptNodes[i].dwAllocated) {
                dwAllocatedNodes++;
            }
        }
        nMaxNodeCount += ptNodeContainer->nNodeCount;
        ptNodeContainer = ptNodeContainer->ptNext;
    }




























    Unlock();
}






















































































































































inline void XRemoteHeap::Lock() {


    m_tLock.Enter();
}

inline void XRemoteHeap::Unlock() {


    m_tLock.Leave();
}

inline int32 XRemoteHeap::NodeRemainingSpaceFor(XRemoteHeapNode* ptNode, size_t dwSize, size_t dwAlign) const {
    size_t dwBase = (size_t)ptNode->pData;
    int64 nRemaining = (int64)(ptNode->dwSize - (((dwBase + (dwAlign - 1)) & ~(dwAlign - 1)) - dwBase) - dwSize);
    return (int32)(nRemaining < 0x7fffffff ? nRemaining : 0x7fffffff);
}











inline XRemoteHeapNode* XRemoteHeap::AlignNodeData(XRemoteHeapNode* ptNode, size_t dwAlign) {
    size_t dwBase = (size_t)ptNode->pData;
    size_t dwAlignOffset = ((dwBase + dwAlign - 1) & ~(dwAlign - 1)) - dwBase;
    if (dwAlignOffset != 0) {
        XRemoteHeapNode* ptAlignedNode = AddNewNode((void*)(dwBase + dwAlignOffset));
        ptAlignedNode->dwSize = ptNode->dwSize - dwAlignOffset;
        ptAlignedNode->dwUsed = 1;
        ptNode->dwUsed = 0;
        ptNode->dwSize = dwAlignOffset;
        AddToFreeList(ptNode, NULL);
        return ptAlignedNode;
    }
    return ptNode;
}












void XRemoteHeap::AddToFreeList(XRemoteHeapNode* ptFreeNode, XRemoteHeapNode* ptPrevNode) {
    XRemoteHeapNode* ptNextNode;
    uint32 dwFreeSlot;
    XRemoteHeapNode* ptNext;






    ptNextNode = FindNextNode(ptFreeNode);
    if (ptNextNode != NULL && !ptNextNode->dwUsed) {
        size_t dwSizeBackup;
        RemoveFromFreeList(ptNextNode);
        dwSizeBackup = ptNextNode->dwSize;
        RemoveNode(ptNextNode);
        ptFreeNode->dwSize += dwSizeBackup;
    }

    if (ptPrevNode == NULL) {







        uint32 dwBucketIndex = GetNodeBucket(ptFreeNode->pData);
        if (m_ptNodeBuckets[dwBucketIndex] == ptFreeNode) {

            ptPrevNode = FindPrevNode(ptFreeNode);
        }
    }

    if (ptPrevNode != NULL && !ptPrevNode->dwUsed) {
        size_t dwSizeBackup;
        RemoveFromFreeList(ptPrevNode);
        dwSizeBackup = ptFreeNode->dwSize;
        RemoveNode(ptFreeNode);
        ptPrevNode->dwSize += dwSizeBackup;
        ptFreeNode = ptPrevNode;
    }






    dwFreeSlot = GetFreeSlotIndex(ptFreeNode->dwSize);
    ptNext = m_ptFreeNodes[dwFreeSlot];
    ptFreeNode->ptNextFreeNode = ptNext;
    ptFreeNode->ptPrevFreeNode = NULL;
    if (ptNext != NULL) {
        ptNext->ptPrevFreeNode = ptFreeNode;
    }
    if (ptFreeNode->dwSize > m_dwBiggestContiguousFreeSize) {
        m_dwBiggestContiguousFreeSize = ptFreeNode->dwSize;
    }
    m_ptFreeNodes[dwFreeSlot] = ptFreeNode;
}






inline XRemoteHeapNode* XRemoteHeap::FindNextNode(XRemoteHeapNode* ptNode) {
    if (ptNode->ptNextNode != NULL) {
        return ptNode->ptNextNode;
    }

    uint32 dwBucketIndex = GetNodeBucket((uint8*)ptNode->pData + ptNode->dwSize);
    XRemoteHeapNode* ptNextNode = NULL;
    if (dwBucketIndex != 512) {
        ptNextNode = m_ptNodeBuckets[dwBucketIndex];
    }
    return ptNextNode;
}




inline XRemoteHeapNode* XRemoteHeap::FindPrevNode(XRemoteHeapNode* ptNode) {
    uint32 dwBucketIndex = GetNodeBucket(ptNode->pData);
    while (dwBucketIndex > 0) {
        dwBucketIndex--;
        XRemoteHeapNode* ptPrevNode = m_ptLastNodeBuckets[dwBucketIndex];
        if (ptPrevNode != NULL) {
            return ptPrevNode;
        }
    }
    return NULL;
}
















void XRemoteHeap::RemoveFromFreeList(XRemoteHeapNode* ptFreeNode) {
    XRemoteHeapNode* ptNext;
    XRemoteHeapNode* ptPrev;

    if (ptFreeNode->dwSize == m_dwBiggestContiguousFreeSize) {
        m_fBiggestDirty = TRUE;
    }

    ptNext = ptFreeNode->ptNextFreeNode;
    ptPrev = ptFreeNode->ptPrevFreeNode;
    if (ptPrev != NULL) {
        ptPrev->ptNextFreeNode = ptNext;
    } else {





        uint32 dwFreeSlot = GetFreeSlotIndex(ptFreeNode->dwSize);
        m_ptFreeNodes[dwFreeSlot] = ptNext;
    }
    if (ptNext != NULL) {
        ptNext->ptPrevFreeNode = ptPrev;
    }
}




inline void XRemoteHeap::Truncate(XRemoteHeapNode* ptNode, size_t dwSize) {
    if (ptNode->dwSize - dwSize >= m_dwMinAllocSize) {
        XRemoteHeapNode* ptNewNode = AddNewNode((uint8*)ptNode->pData + dwSize);
        ptNewNode->dwSize = ptNode->dwSize - dwSize;
        ptNewNode->dwUsed = 0;
        ptNode->dwSize = dwSize;
        AddToFreeList(ptNewNode, NULL);
    }
}

XRemoteHeapNode* XRemoteHeap::AddNewNode(void* pData) {
    int32 nIndex;
    XRemoteHeapNodeContainer* ptNodeContainer;
    XRemoteHeapNode* ptNode;
    size_t dwBase;
    XRemoteHeapNode* ptBucketNode;
    XRemoteHeapNode* ptPrevNode;
    uint32 dwBucketIndex;

    ptNodeContainer = m_ptNodeContainers;
    while (ptNodeContainer != NULL) {
        nIndex = ptNodeContainer->nMinUnallocatedNodeIndex;
        if (nIndex != ptNodeContainer->nNodeCount) {
            ptNode = &ptNodeContainer->ptNodes[nIndex];
            while (ptNode->dwAllocated && nIndex < ptNodeContainer->nNodeCount) {
                ptNode++;
                nIndex++;
            }
            ptNodeContainer->nMinUnallocatedNodeIndex = nIndex;
            if (nIndex < ptNodeContainer->nNodeCount) {
                break;
            }
        }
        ptNodeContainer = ptNodeContainer->ptNext;
    }

    if (ptNodeContainer == NULL) {
        if (m_ptNodesHeap != NULL) {
            ptNodeContainer = (XRemoteHeapNodeContainer*)m_ptNodesHeap->MemAlloc(
                sizeof(XRemoteHeapNodeContainer) + 4096 * sizeof(XRemoteHeapNode), 16, FALSE);
        } else {
            ptNodeContainer = (XRemoteHeapNodeContainer*)SigmaMemoryNew(
                sizeof(XRemoteHeapNodeContainer) + 4096 * sizeof(XRemoteHeapNode), 4, NULL, FALSE);
        }
        ptNodeContainer->nNodeCount = 4096;
        ptNodeContainer->nMinUnallocatedNodeIndex = 0;
        ptNodeContainer->ptNext = m_ptNodeContainers;
        ptNodeContainer->ptNodes = (XRemoteHeapNode*)(ptNodeContainer + 1);
        for (uint32 i = 0; i < 4096; i++) {
            ptNodeContainer->ptNodes[i].dwAllocated = 0;
        }
        m_ptNodeContainers = ptNodeContainer;
        nIndex = 0;
        ptNode = ptNodeContainer->ptNodes;
    }

    ptNodeContainer->nMinUnallocatedNodeIndex = nIndex + 1;
    ptNode->dwAllocated = 1;
    ptNode->pData = pData;

    dwBase = (size_t)pData;






    dwBucketIndex = GetNodeBucket(pData);
    ptBucketNode = m_ptNodeBuckets[dwBucketIndex];
    ptPrevNode = NULL;
    while (ptBucketNode != NULL) {
        if (dwBase < (size_t)ptBucketNode->pData) {
            ptNode->ptNextNode = ptBucketNode;
            if (ptPrevNode != NULL) {
                ptPrevNode->ptNextNode = ptNode;
            } else {
                m_ptNodeBuckets[dwBucketIndex] = ptNode;
            }
            return ptNode;
        }
        ptPrevNode = ptBucketNode;
        ptBucketNode = ptBucketNode->ptNextNode;
    }

    ptNode->ptNextNode = NULL;
    if (ptPrevNode != NULL) {
        ptPrevNode->ptNextNode = ptNode;
    } else {
        m_ptNodeBuckets[dwBucketIndex] = ptNode;
    }
    m_ptLastNodeBuckets[dwBucketIndex] = ptNode;
    return ptNode;
}




















void XRemoteHeap::RemoveNode(XRemoteHeapNode* ptNode) {
    XRemoteHeapNodeContainer* ptNodeContainer;
    int32 nIndex;
    uint32 dwBucketIndex;
    XRemoteHeapNode* ptPrev;

    ptNodeContainer = m_ptNodeContainers;
    while (ptNodeContainer != NULL) {
        if (ptNode >= ptNodeContainer->ptNodes && ptNode <= ptNodeContainer->ptNodes + ptNodeContainer->nNodeCount) {
            break;
        }
        ptNodeContainer = ptNodeContainer->ptNext;
    }

    nIndex = (int32)(ptNode - ptNodeContainer->ptNodes);
    if (nIndex < ptNodeContainer->nMinUnallocatedNodeIndex) {
        ptNodeContainer->nMinUnallocatedNodeIndex = nIndex;
    }
    ptNode->dwAllocated = 0;







    dwBucketIndex = GetNodeBucket(ptNode->pData);

    GetHeapNode(ptNode->pData, dwBucketIndex, ptPrev);

    if (m_ptNodeBuckets[dwBucketIndex] == ptNode) {
        m_ptNodeBuckets[dwBucketIndex] = ptNode->ptNextNode;
        if (ptNode->ptNextNode == NULL) {
            m_ptLastNodeBuckets[dwBucketIndex] = NULL;
        }
    } else {
        ptPrev->ptNextNode = ptNode->ptNextNode;
        if (ptNode->ptNextNode == NULL) {
            m_ptLastNodeBuckets[dwBucketIndex] = ptPrev;
        }
    }
}












inline uint32 XRemoteHeap::GetFreeSlotIndex(size_t dwSize) const {
    if (dwSize <= m_dwMinAllocSize) {
        return 0;
    }

    if (dwSize < m_dwMaxExponentialAllocSize) {
        return (uint32)Math::FloorLog2(dwSize) - m_dwBaseLog2Index;
    }

    return Math::Min<uint32>((uint32)((dwSize - m_dwMaxExponentialAllocSize) / m_dwLinearStep + m_dwMaxLog2Index), 31);
}











inline uint32 XRemoteHeap::GetNodeBucket(void* pData) const {
    size_t dwOffset = (size_t)pData - (size_t)m_pHeapBuffer;
    return (uint32)(dwOffset / m_dwBucketSize);
}




inline XRemoteHeapNode* XRemoteHeap::GetHeapNode(void* pData, uint32 dwBucketIndex,
                                                 XRemoteHeapNode*& ptPrevNode) const {
    size_t dwBase = (size_t)pData;
    XRemoteHeapNode* ptNode = m_ptNodeBuckets[dwBucketIndex];
    XRemoteHeapNode* ptPrev = NULL;
    while (ptNode != NULL) {
        size_t dwNodeBase = (size_t)ptNode->pData;
        if (dwBase >= dwNodeBase && dwBase < dwNodeBase + ptNode->dwSize) {
            uint32 dwPreviousBucket = dwBucketIndex - 1;
            while (ptPrev == NULL && dwPreviousBucket < 512) {
                ptPrev = m_ptLastNodeBuckets[dwPreviousBucket];
                dwPreviousBucket--;
            }
            ptPrevNode = ptPrev;
            return ptNode;
        }
        ptPrev = ptNode;
        ptNode = ptNode->ptNextNode;
    }
    ptPrevNode = NULL;
    return NULL;
}























