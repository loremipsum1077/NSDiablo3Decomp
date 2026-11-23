#pragma once

#include "DataStructuresExternal.h"
#include "XAllocator.h"
#include "XFixedBlockAllocator.h"
#include "XTemplate.h"

struct ChainedFixedBlockAllocator {
    void Initialize(int32 nBlockSize, int32 nBlockCount, XMemoryAllocatorInterface* ptMemoryInterface);
    void Dispose();
    BOOL IsDisposed();

    void SetUseSystemAllocator(BOOL bValue);
    void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface);
    XMemoryAllocatorInterface* GetMemoryInterface();
    void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam);
    void SetShrinkable(BOOL bShrinkable);

    void* Alloc_Tracked();
    void Free(void* pMem);
    BOOL ContainsMem(void* pMem);

    void SetBlockCount_Tracked(int32 nBlockCount);
    int32 GetFreeBlockCount();
    int32 GetUsedBlockCount();

    POS GetFirstBlockPOS();
    void* GetNextBlock(POS& pos);

    void PrintUsage();
    void PrintUsedBlockAddresses();

    void Reset();
    void Validate();

    struct XFBANode {
        XFBANode* pNext;
        FixedBlockAllocator tAlloc;
        char pPadding[16];
    };

    XFBANode* _AllocFBANode();
    void _FreeFBANode(XFBANode* ptNode);

    int32 m_nBlockSize;
    int32 m_nBlockCount;
    int32 m_nNumAllocators;
    XFBANode* m_pHeadAlloc;
    uint8 m_ucFlags;
    CHAR m_eGrow;
    uint16 m_nGrowParam;
    XMemoryAllocatorInterface* m_ptMemoryInterface;
    CCookie m_Cookie;
};































































































































template <typename TYPE>
class XChainedFixedBlockAllocator {
public:
    inline XChainedFixedBlockAllocator();
    inline ~XChainedFixedBlockAllocator();

    inline void Initialize();
    inline void Initialize(int32 nBlockCount, XMemoryAllocatorInterface* ptMemoryInterface);
    inline void Dispose();

    inline void SetUseSystemAllocator(BOOL bValue);
    inline void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface);
    inline void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam);
    inline void SetShrinkable(BOOL bShrinkable);

    inline TYPE* Alloc_Tracked();
    inline void Free(TYPE* pValue);

    inline void SetBlockCount_Tracked(int32 nBlockCount);
    inline int32 GetUsedBlockCount();

    inline POS GetFirstBlockPOS();
    inline TYPE* GetNextBlock(POS& pos);

    inline void PrintUsage();
    inline void Reset();
    inline void Validate();

protected:
    ChainedFixedBlockAllocator m_tAlloc;
};
































































inline void ChainedFixedBlockAllocator::Initialize(int32 nBlockSize, int32 nBlockCount,
                                                   XMemoryAllocatorInterface* ptMemoryInterface) {
    m_nBlockCount = nBlockCount;
    m_nBlockSize = nBlockSize;
    m_nNumAllocators = 0;
    m_pHeadAlloc = NULL;
    m_ucFlags = 0;
    m_eGrow = XALLOC_GROW_EXPONENTIAL;
    m_nGrowParam = 2;
    m_ptMemoryInterface = ptMemoryInterface;









    m_Cookie.Set();
}


inline void ChainedFixedBlockAllocator::Dispose() {
    XFBANode* ptCur = m_pHeadAlloc;

    while (ptCur != NULL) {
        XFBANode* ptNext = ptCur->pNext;






        ptCur->tAlloc.Dispose();

        _FreeFBANode(ptCur);

        ptCur = ptNext;
    }

    m_nBlockSize = 0;
    m_nBlockCount = 0;
    m_nNumAllocators = 0;
    m_pHeadAlloc = NULL;
    m_ucFlags = 0;




    m_Cookie.Clear();
}









inline void ChainedFixedBlockAllocator::SetUseSystemAllocator(BOOL bValue) {
    if (bValue) {
        m_ucFlags |= (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY);
    } else {
        m_ucFlags &= ~(1 << XALLOC_FLAG_USE_SYSTEM_MEMORY);
    }
}





inline void ChainedFixedBlockAllocator::SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) {
    m_ptMemoryInterface = ptMemoryInterface;
}


















inline void ChainedFixedBlockAllocator::SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) {
    m_eGrow = eGrow;
    m_nGrowParam = nParam;
}






inline void ChainedFixedBlockAllocator::SetShrinkable(BOOL bShrinkable) {
    if (bShrinkable) {
        m_ucFlags |= (1 << XALLOC_FLAG_SHRINKABLE);
    } else {
        m_ucFlags &= ~(1 << XALLOC_FLAG_SHRINKABLE);
    }
}





inline void* ChainedFixedBlockAllocator::Alloc_Tracked() {
    XFBANode* ptCur = m_pHeadAlloc;
    void* pMem;

    while (ptCur != NULL) {















        pMem = ptCur->tAlloc.Alloc_Tracked();
        if (pMem != NULL) {
            return pMem;
        }

        ptCur = ptCur->pNext;
    }











    ptCur = _AllocFBANode();

    if (m_ucFlags & (1 << XALLOC_FLAG_SHRINKABLE)) {
        ptCur->pNext = NULL;

        XFBANode* ptPrev = m_pHeadAlloc;
        if (ptPrev == NULL) {
            m_pHeadAlloc = ptCur;
        } else {
            while (ptPrev->pNext != NULL) {
                ptPrev = ptPrev->pNext;
            }
            ptPrev->pNext = ptCur;
        }
    } else {
        ptCur->pNext = m_pHeadAlloc;
        m_pHeadAlloc = ptCur;
    }
















    return ptCur->tAlloc.Alloc_Tracked();
}






inline void ChainedFixedBlockAllocator::Free(void* pMem) {
    XFBANode* ptCur = m_pHeadAlloc;
    XFBANode** ptPrev = &m_pHeadAlloc;
    BOOL bFreed = FALSE;

    while (ptCur != NULL) {





        if (ptCur->tAlloc.ContainsMem(pMem)) {
            ptCur->tAlloc.Free(pMem);
            bFreed = TRUE;
            break;
        }

        ptPrev = &ptCur->pNext;
        ptCur = ptCur->pNext;
    }

    if (m_ucFlags & (1 << XALLOC_FLAG_SHRINKABLE)) {






        if (ptCur->tAlloc.GetUsedBlockCount() == 0 && ptCur != m_pHeadAlloc) {
            m_nBlockCount -= ptCur->tAlloc.m_nBlockCount;
            *ptPrev = ptCur->pNext;


            _FreeFBANode(ptCur);
        }
    }
}


























inline void ChainedFixedBlockAllocator::SetBlockCount_Tracked(int32 nBlockCount) {
    m_nBlockCount = nBlockCount;
}

































inline int32 ChainedFixedBlockAllocator::GetUsedBlockCount() {
    int32 nUsedBlockCount = 0;

    for (XFBANode* ptCur = m_pHeadAlloc; ptCur != NULL; ptCur = ptCur->pNext) {
        nUsedBlockCount += ptCur->tAlloc.GetUsedBlockCount();
    }

    return nUsedBlockCount;
}



inline void ChainedFixedBlockAllocator::Validate() {
    for (XFBANode* ptCur = m_pHeadAlloc; ptCur != NULL; ptCur = ptCur->pNext) {
        ptCur->tAlloc.Validate();
    }
}



inline POS ChainedFixedBlockAllocator::GetFirstBlockPOS() {
    // Body written from TagMapDisposeForGame (Tags.cpp), where it is inlined: only the head
    // allocator is asked.
    if (m_pHeadAlloc != NULL) {


        return m_pHeadAlloc->tAlloc.GetFirstBlockPOS();
    }
    return NULL;
}




inline BOOL ChainedFixedBlockAllocator::ContainsMem(void* pMem) {
    // Body written from MaterialFreeTexAnimState (TexAnimation.cpp), where it is inlined.
    for (XFBANode* ptCur = m_pHeadAlloc; ptCur != NULL; ptCur = ptCur->pNext) {
        if (ptCur->tAlloc.ContainsMem(pMem)) {
            return TRUE;
        }
    }
    return FALSE;
}

inline void* ChainedFixedBlockAllocator::GetNextBlock(POS& pos) {
    // Body written from TagMapDisposeForGame (Tags.cpp), where it is inlined.
    for (XFBANode* ptCur = m_pHeadAlloc; ptCur != NULL; ptCur = ptCur->pNext) {









        if (ptCur->tAlloc.ContainsMem(pos)) {

            void* pRet = ptCur->tAlloc.GetNextBlock(pos);
            if (pos == NULL) {
                ptCur = ptCur->pNext;
                if (ptCur != NULL) {




                    pos = ptCur->tAlloc.GetFirstBlockPOS();
                }
            }
            return pRet;
        }
    }
    return NULL;
}






inline void ChainedFixedBlockAllocator::PrintUsage() {
    // Original line 654: TagMapDisposeForGame 0x6a3a10-0x6a3a18 retains only this walk; release reports are compiled out.
    for (XFBANode* ptCur = m_pHeadAlloc; ptCur != NULL; ptCur = ptCur->pNext) {
    }
}











































































inline ChainedFixedBlockAllocator::XFBANode* ChainedFixedBlockAllocator::_AllocFBANode() {
    int32 nNewAllocatorBlockCount;

    switch (m_eGrow) {
    case XALLOC_GROW_LINEAR:
        nNewAllocatorBlockCount = (m_nNumAllocators == 0) ? m_nBlockCount : m_nGrowParam;
        break;
    case XALLOC_GROW_EXPONENTIAL:
        nNewAllocatorBlockCount = (m_nNumAllocators == 0) ? m_nBlockCount : m_nBlockCount * m_nGrowParam;
        break;
    default:
        nNewAllocatorBlockCount = 0;
        break;
    }

    int32 nMemSize = sizeof(XFBANode) + nNewAllocatorBlockCount * m_nBlockSize;
    void* pMem;

    if (m_ucFlags & (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY)) {
        pMem = malloc(nMemSize);
    } else {
        pMem = SigmaMemoryNew(nMemSize, 16, m_ptMemoryInterface, FALSE);
    }

    XFBANode* ptNewNode = (XFBANode*)pMem;





















    ptNewNode->tAlloc.InitializeFixed(m_nBlockSize, nNewAllocatorBlockCount, ptNewNode + 1,
                                      nNewAllocatorBlockCount * m_nBlockSize, NULL);
    ptNewNode->pNext = NULL;

    if (m_nNumAllocators > 0) {
        m_nBlockCount += nNewAllocatorBlockCount;
    }
    m_nNumAllocators++;

    return ptNewNode;
}






inline void ChainedFixedBlockAllocator::_FreeFBANode(XFBANode* ptNode) {
    if (m_ucFlags & (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY)) {
        free(ptNode);
    } else {
        XFREE(ptNode, m_ptMemoryInterface);
    }

    m_nNumAllocators--;
}













template <typename TYPE>
XChainedFixedBlockAllocator<TYPE>::XChainedFixedBlockAllocator() {
    Initialize();
}



template <typename TYPE>
XChainedFixedBlockAllocator<TYPE>::~XChainedFixedBlockAllocator() {
    Dispose();
}



template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::Initialize() {

    m_tAlloc.Initialize(sizeof(TYPE), 10, NULL);
}



template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::Initialize(int32 nBlockCount, XMemoryAllocatorInterface* ptMemoryInterface) {
    m_tAlloc.Initialize(sizeof(TYPE), nBlockCount, ptMemoryInterface);
}





template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::Dispose() {

    m_tAlloc.Dispose();
}











template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::SetUseSystemAllocator(BOOL bValue) {
    m_tAlloc.SetUseSystemAllocator(bValue);
}




template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) {

    m_tAlloc.SetMemoryInterface(ptMemoryInterface);
}











template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) {


    m_tAlloc.SetGrow(eGrow, nParam);
}



template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::SetShrinkable(BOOL bShrinkable) {

    m_tAlloc.SetShrinkable(bShrinkable);
}



template <typename TYPE>
TYPE* XChainedFixedBlockAllocator<TYPE>::Alloc_Tracked() {
    void* pMem = m_tAlloc.Alloc_Tracked();
    TYPE* pType = NULL;

    if (pMem != NULL) {












        pType = new (pMem) TYPE;
    }

    return pType;
}





template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::Free(TYPE* pValue) {
    pValue->~TYPE();


    m_tAlloc.Free(pValue);
}











template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::SetBlockCount_Tracked(int32 nBlockCount) {










    m_tAlloc.SetBlockCount_Tracked(nBlockCount);
}


template <typename TYPE>
int32 XChainedFixedBlockAllocator<TYPE>::GetUsedBlockCount() {
    return m_tAlloc.GetUsedBlockCount();
}

template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::Validate() {
    m_tAlloc.Validate();
}

template <typename TYPE>
POS XChainedFixedBlockAllocator<TYPE>::GetFirstBlockPOS() {  // line 991
    return m_tAlloc.GetFirstBlockPOS();
}

template <typename TYPE>
TYPE* XChainedFixedBlockAllocator<TYPE>::GetNextBlock(POS& pos) {  // line 999
    return (TYPE*)m_tAlloc.GetNextBlock(pos);
}

template <typename TYPE>
void XChainedFixedBlockAllocator<TYPE>::PrintUsage() {  // line 1007
    m_tAlloc.PrintUsage();
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
// Not checked: XChainedFixedBlockAllocator (template)
static_assert(sizeof(ChainedFixedBlockAllocator) == 0x30, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_nBlockSize) == 0x0, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_nBlockCount) == 0x4, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_nNumAllocators) == 0x8, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_pHeadAlloc) == 0x10, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_ucFlags) == 0x18, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_eGrow) == 0x19, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_nGrowParam) == 0x1a, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_ptMemoryInterface) == 0x20, "ChainedFixedBlockAllocator");
static_assert(offsetof(ChainedFixedBlockAllocator, m_Cookie) == 0x28, "ChainedFixedBlockAllocator");

// Original line 692 (inlined into 2 functions; body written from X/Client/Sprite.cpp SpritesFlush).
inline void ChainedFixedBlockAllocator::Reset() {
    for (XFBANode* ptCur = m_pHeadAlloc; ptCur != NULL; ptCur = ptCur->pNext)
        ptCur->tAlloc.Reset();
}

// Original line 1015.
template <typename TYPE>
inline void XChainedFixedBlockAllocator<TYPE>::Reset() {
    m_tAlloc.Reset();
}
