#pragma once

#include "XBaseListNode.h"
#include "XChainedFixedBlockAllocator.h"
#include "XDataStructuresShared.h"
#include "XFixedBlockAllocator.h"
#include "XTemplate.h"







































template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
class XBaseList {
    typedef XBaseListNode<TYPE> XBaseListNodeType;
    inline XBaseListNodeType* _FindBestNode(int32 (*pfnCompare)(const void*, const void*));
public:
    inline XBaseList();

    inline void Initialize();
    inline void Dispose();

    inline POS GetHeadPosition() const;
    inline POS GetTailPosition() const;

    inline const TYPE& GetNext(POS& pos) const;
    inline TYPE& GetNext(POS& pos);
    inline const TYPE& GetPrev(POS& pos) const;
    inline TYPE& GetPrev(POS& pos);
    inline void MoveToTail(const POS& pos);
    inline void MoveToHead(const POS& pos);  // 2.7.7 (inlined into 0x71005285b0)
    inline static TYPE& sGetNext(POS& pos);

    inline const TYPE& GetHead() const;
    inline TYPE& GetHead();
    inline const TYPE& GetTail() const;
    inline TYPE& GetTail();

    inline TYPE& GetAt(const POS& pos);

    inline POS Find(ARG_TYPE tData) const;

    inline int32 Count() const;
    inline BOOL IsEmpty() const;

    inline POS AddHead_Tracked(ARG_TYPE tData);
    inline POS AddTail_Tracked(ARG_TYPE tData);
    inline POS AddTail_NoAssign_Tracked();
    inline POS InsertBefore_Tracked(POS pos, ARG_TYPE tData);
    inline POS InsertAfter_Tracked(POS pos, ARG_TYPE tData);

    inline POS RemoveAt(POS pos);
    inline void RemoveAll();
    inline void RemoveHead();
    inline void RemoveTail();
    inline void Sort(int32 (*pfnCompare)(const void*, const void*));
    // Moves every node of tOther onto the tail of this list (body reconstructed from Server.cpp sGameConnectionDelete).
    inline void Splice(XBaseList & tOther)
    {
        m_nCount += tOther.m_nCount;
        if (m_pTail == NULL)
        {
            m_pHead = tOther.m_pHead;
            m_pTail = tOther.m_pTail;
        }
        else
        {
            m_pTail->m_pNext = tOther.m_pHead;
            if (m_pTail->m_pNext != NULL)
                m_pTail->m_pNext->m_pPrev = m_pTail;
            if (tOther.m_pTail != NULL)
                m_pTail = tOther.m_pTail;
        }
        tOther.m_nCount = 0;
        tOther.m_pHead = NULL;
        tOther.m_pTail = NULL;
    }
private:
    inline XBaseListNodeType* _AllocNode_Tracked(ARG_TYPE tData);
    inline XBaseListNodeType* _AllocNode_NoAssign_Tracked();
    inline void _FreeNode(XBaseListNodeType* pNode);
    inline void _RemoveFromList(XBaseListNodeType* pNode);

    XBaseListNodeType* m_pHead;
    XBaseListNodeType* m_pTail;
    int32 m_nCount;

protected:
    NODE_ALLOCATOR* m_ptNodeAllocator;
};







































































































template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::XBaseList() {
    m_pHead = NULL;
    m_pTail = NULL;
    m_nCount = 0;
}












template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Initialize() {
    m_pHead = NULL;
    m_pTail = NULL;
    m_nCount = 0;
}





template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Dispose() {
    RemoveAll();
    m_ptNodeAllocator = NULL;
}





template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetHeadPosition() const {
    return (POS)m_pHead;
}





template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetTailPosition() const {
    return (POS)m_pTail;
}





template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
const TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetHead() const {
    return m_pHead->m_tData;
}




template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetHead() {
    return m_pHead->m_tData;
}






template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
const TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetTail() const {
    return m_pTail->m_tData;
}






template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetTail() {
    return m_pTail->m_tData;
}

















template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetAt(const POS& pos) {
    return ((XBaseListNodeType*)pos)->m_tData;
}







template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::RemoveAll() {


    while (!IsEmpty()) {
        RemoveHead();
    }
}




template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
int32 XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Count() const {
    return m_nCount;
}





template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
BOOL XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::IsEmpty() const {


    return Count() == 0;
}



template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::RemoveHead() {
    XBaseListNodeType* pNode = m_pHead;

    m_pHead = pNode->m_pNext;
    if (m_pHead == NULL) {
        m_pTail = NULL;
    } else {
        m_pHead->m_pPrev = NULL;
    }






    _FreeNode(pNode);
}



template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::RemoveTail() {
    XBaseListNodeType* pNode = m_pTail;

    m_pTail = pNode->m_pPrev;
    if (m_pTail == NULL) {
        m_pHead = NULL;
    } else {
        m_pTail->m_pNext = NULL;
    }

    _FreeNode(pNode);
}




template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::MoveToTail(const POS& pos) {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    if (pNode->m_pNext != NULL) {
        if (pNode->m_pPrev != NULL) {
            pNode->m_pPrev->m_pNext = pNode->m_pNext;
        } else {
            m_pHead = pNode->m_pNext;
        }
        pNode->m_pNext->m_pPrev = pNode->m_pPrev;
        m_pTail->m_pNext = pNode;
        pNode->m_pPrev = m_pTail;
        pNode->m_pNext = NULL;
        m_pTail = pNode;
    }
}

template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::MoveToHead(const POS& pos) {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    if (m_pHead != pNode) {
        if (pNode->m_pNext != NULL) {
            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
        } else {
            m_pTail = pNode->m_pPrev;
        }
        pNode->m_pPrev->m_pNext = pNode->m_pNext;
        m_pHead->m_pPrev = pNode;
        pNode->m_pNext = m_pHead;
        pNode->m_pPrev = NULL;
        m_pHead = pNode;
    }
}








































































































template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
typename XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::XBaseListNodeType*
XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::_FindBestNode(int32 (*pfnCompare)(const void*, const void*)) {
    // Bodies of _FindBestNode and Sort written from the copy inlined into
    // UIInventory::GetSetItemTooltip (UIInventory.cpp:414).
    XBaseListNodeType* pNode;
    XBaseListNodeType* pNodeBest;

    pNodeBest = m_pHead;
    pNode = m_pHead->m_pNext;

    while (pNode != NULL) {

        if (pfnCompare(&pNode->m_tData, &pNodeBest->m_tData) > 0) {
            pNodeBest = pNode;
        }
        pNode = pNode->m_pNext;
    }

    return pNodeBest;
}


template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Sort(int32 (*pfnCompare)(const void*, const void*)) {
    XBaseListNodeType* pNewListHead = NULL;
    XBaseListNodeType* pNewListTail = NULL;
    if (m_pHead == NULL ||
        m_pHead->m_pNext == NULL) {
        return;
    }




    while (m_pHead != NULL) {


        XBaseListNodeType* pNodeBest = _FindBestNode(pfnCompare);
        _RemoveFromList(pNodeBest);

        pNodeBest->m_pPrev = NULL;
        pNodeBest->m_pNext = NULL;
        if (pNewListHead == NULL) {
            pNewListHead = pNodeBest;

        } else {


            pNewListTail->m_pNext = pNodeBest;
            pNodeBest->m_pPrev = pNewListTail;
        }
        pNewListTail = pNodeBest;
    }


    m_pHead = pNewListHead;
    m_pTail = pNewListTail;
}


template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::AddHead_Tracked(ARG_TYPE tData) {
    XBaseListNodeType* pNode = _AllocNode_Tracked(tData);

    pNode->m_pPrev = NULL;
    pNode->m_pNext = NULL;

    if (m_pHead != NULL) {
        m_pHead->m_pPrev = pNode;
        pNode->m_pNext = m_pHead;
    } else {
        m_pTail = pNode;
    }
    m_pHead = pNode;

    return (POS)pNode;
}


















template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::AddTail_Tracked(ARG_TYPE tData) {











    XBaseListNodeType* pNode = _AllocNode_Tracked(tData);

    pNode->m_pPrev = NULL;
    pNode->m_pNext = NULL;

    if (m_pTail != NULL) {
        m_pTail->m_pNext = pNode;
        pNode->m_pPrev = m_pTail;
    } else {
        m_pHead = pNode;
    }
    m_pTail = pNode;

    return (POS)pNode;
}






template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::AddTail_NoAssign_Tracked() {










    XBaseListNodeType* pNode = _AllocNode_NoAssign_Tracked();

    pNode->m_pPrev = NULL;
    pNode->m_pNext = NULL;

    if (m_pTail != NULL) {
        m_pTail->m_pNext = pNode;
        pNode->m_pPrev = m_pTail;
    } else {
        m_pHead = pNode;
    }
    m_pTail = pNode;
    return (POS)pNode;
}








template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::InsertBefore_Tracked(POS pos, ARG_TYPE tData) {
    // Body reconstructed from the original line table (XBaseList.h:689-702).












    XBaseListNodeType* pNewNode = _AllocNode_Tracked(tData);


    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;

    XBaseListNodeType* pPrevNode = pNode->m_pPrev;

    pNode->m_pPrev = pNewNode;
    pNewNode->m_pPrev = pPrevNode;
    pNewNode->m_pNext = pNode;

    if (pPrevNode == NULL) {
        m_pHead = pNewNode;
    } else {
        pPrevNode->m_pNext = pNewNode;
    }

    return (POS)pNewNode;
}





template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::InsertAfter_Tracked(POS pos, ARG_TYPE tData) {
    // Body from the original DWARF and line table (XBaseList.h:714-748; abstract DIE in the
    // SNOFilesAsync CU: locals pNewNode 728, pRefNode 732, pNextNode 733; rows 733:43 load of
    // pRefNode->m_pNext, 735 store, 737:20 one stp of the new node's prev/next, 739:7 test,
    // 741:14 / 745:3 the two arms). Mirrors InsertBefore_Tracked above.
    XBaseListNodeType* pNewNode = _AllocNode_Tracked(tData);
    XBaseListNodeType* pRefNode = (XBaseListNodeType*)pos;
    XBaseListNodeType* pNextNode = pRefNode->m_pNext;

    pRefNode->m_pNext = pNewNode;
    pNewNode->m_pPrev = pRefNode;
    pNewNode->m_pNext = pNextNode;

    if (pNextNode != NULL) {
        pNextNode->m_pPrev = pNewNode;
    } else {
        m_pTail = pNewNode;
    }

    return (POS)pNewNode;
}


















template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
const TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetNext(POS& pos) const {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    pos = (POS)pNode->m_pNext;
    return pNode->m_tData;
}







template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetNext(POS& pos) {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    TYPE& tRetval = pNode->m_tData;
    pos = (POS)pNode->m_pNext;
    return tRetval;
}






template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
const TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetPrev(POS& pos) const {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    pos = (POS)pNode->m_pPrev;
    return pNode->m_tData;
}







template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::GetPrev(POS& pos) {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    pos = (POS)pNode->m_pPrev;
    return pNode->m_tData;
}







template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
TYPE& XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::sGetNext(POS& pos) {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    pos = (POS)pNode->m_pNext;
    return pNode->m_tData;
}


















template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Find(ARG_TYPE tData) const {
    for (XBaseListNodeType* pNode = m_pHead; pNode != NULL; pNode = pNode->m_pNext) {
        if (tData == pNode->m_tData) {
            return (POS)pNode;
        }
    }

    return NULL;
}





























template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::_RemoveFromList(XBaseListNodeType* pNode) {
    if (pNode->m_pNext != NULL) {
        if (pNode->m_pPrev != NULL) {
            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
            pNode->m_pPrev->m_pNext = pNode->m_pNext;
        } else {
            pNode->m_pNext->m_pPrev = NULL;
            m_pHead = pNode->m_pNext;
        }
    } else {
        if (pNode->m_pPrev != NULL) {
            pNode->m_pPrev->m_pNext = NULL;
            m_pTail = pNode->m_pPrev;
        } else {
            m_pHead = NULL;
            m_pTail = NULL;
        }
    }
}













template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
POS XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::RemoveAt(POS pos) {
    XBaseListNodeType* pNode = (XBaseListNodeType*)pos;
    POS next = (POS)pNode->m_pNext;


    _RemoveFromList(pNode);
    _FreeNode(pNode);

    return next;
}


template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
typename XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::XBaseListNodeType*
XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::_AllocNode_Tracked(ARG_TYPE tData) {
    XBaseListNodeType* pNode;








    pNode = m_ptNodeAllocator->Alloc_Tracked();
    pNode->m_tData = tData;
    m_nCount++;

    return pNode;
}



template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
typename XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::XBaseListNodeType*
XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::_AllocNode_NoAssign_Tracked() {
    XBaseListNodeType* pNode;

    pNode = m_ptNodeAllocator->Alloc_Tracked();
    m_nCount++;

    return pNode;
}








template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::_FreeNode(XBaseListNodeType* pNode) {
    m_nCount--;

    m_ptNodeAllocator->Free(pNode);
}
