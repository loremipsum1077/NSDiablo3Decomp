#pragma once

#include "DataStructuresExternal.h"
#include "XAllocator.h"
#include "XTemplate.h"

#include <new>

struct FixedBlockAllocator {
    void InitializeFixed(int32 nBlockSize, int32 nBlockCount, void* pMem, uint32 nMemSize,
                         XMemoryAllocatorInterface* ptMemoryInterface);
    void InitializeDynamic(int32 nBlockSize, int32 nBlockCount, BOOL bUseSysAlloc,
                           XMemoryAllocatorInterface* ptMemoryInterface);
    void Dispose();
    BOOL IsDisposed();

    void* Alloc_Tracked();
    void Free(void* pMem);
    BOOL ContainsMem(void* pMem);

    int32 GetFreeBlockCount();
    int32 GetUsedBlockCount();

    POS GetFirstBlockPOS();
    void* GetNextBlock(POS& pos);

    void PrintUsage();
    void PrintUsedBlockAddresses();

    void Reset();
    void Validate();

    uint8* _GetBlock(int32 nBlock);
    BOOL _IsOnBlockBoundary(void* pMem);
    BOOL _ValidateBlock(void* pMem);
    BOOL _IsBlockUsed(void* pMem);
    int32 _GetPaddedBlockSize();

    struct XFreeNode {
        XFreeNode* m_pNext;
    };

    uint8* m_pMemBlocks;
    XFreeNode* m_ptFreeList;
    int32 m_nBlockCount;
    int32 m_nBlockSize;
    int32 m_nMaxBlockAlloced;
    uint32 m_dwFlags;
    uint32 m_dwUserData;
    int32 m_nFreeBlockCount;
    XMemoryAllocatorInterface* m_ptMemoryInterface;
    CCookie m_Cookie;
};












































































































































template <typename TYPE, int COUNT>
class XFixedBlockAllocator {
public:
    inline XFixedBlockAllocator();
    inline ~XFixedBlockAllocator();

    inline void Initialize();
    inline void Dispose();

    inline TYPE* Alloc_Tracked();
    inline void Free(TYPE* pValue);
    inline void SetBlockCount_Tracked(int32 nBlockCount) {}
    inline int32 GetFreeBlockCount();
    inline int32 GetUsedBlockCount();

    inline POS GetFirstBlockPOS();
    inline TYPE* GetNextBlock(POS& pos);

protected:
    FixedBlockAllocator m_tAlloc;
    uint8 m_Data[sizeof(TYPE) * COUNT];
};
























































inline void FixedBlockAllocator::InitializeFixed(int32 nBlockSize, int32 nBlockCount, void* pMem, uint32 nMemSize,
                                                 XMemoryAllocatorInterface* ptMemoryInterface) {











    m_nFreeBlockCount = nBlockCount;
    m_nBlockCount = nBlockCount;
    m_nBlockSize = nBlockSize;
    m_nMaxBlockAlloced = 0;
    m_pMemBlocks = (uint8*)pMem;
    m_ptFreeList = NULL;
    m_dwFlags = 0;
    m_ptMemoryInterface = ptMemoryInterface;




















    m_Cookie.Set();
}


inline void FixedBlockAllocator::InitializeDynamic(int32 nBlockSize, int32 nBlockCount, BOOL bUseSysAlloc,
                                                   XMemoryAllocatorInterface* ptMemoryInterface) {
    uint32 nAdditionalBuffer = 0;









    m_nFreeBlockCount = nBlockCount;
    m_nBlockCount = nBlockCount;
    m_nBlockSize = nBlockSize;
    m_nMaxBlockAlloced = 0;
    m_ptFreeList = NULL;
    m_dwFlags = (1 << XALLOC_FLAG_MANAGE_MEM_BLOCK);
    m_ptMemoryInterface = ptMemoryInterface;













    if (bUseSysAlloc) {
        m_dwFlags |= (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY);
        m_pMemBlocks = (uint8*)malloc(nBlockCount * nBlockSize + nAdditionalBuffer);
    } else {


        m_pMemBlocks = (uint8*)SigmaMemoryNew(nBlockCount * nBlockSize + nAdditionalBuffer, 16, ptMemoryInterface,
                                              FALSE);
    }










    m_Cookie.Set();
}


inline void FixedBlockAllocator::Dispose() {




    Validate();

    if (m_dwFlags & (1 << XALLOC_FLAG_MANAGE_MEM_BLOCK)) {

        if (m_dwFlags & (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY)) {

            free(m_pMemBlocks);
        } else {


            XFREE(m_pMemBlocks, m_ptMemoryInterface);
        }
    }

    m_nFreeBlockCount = 0;
    m_nBlockCount = 0;
    m_nBlockSize = 0;
    m_nMaxBlockAlloced = 0;
    m_pMemBlocks = NULL;
    m_ptFreeList = NULL;
    m_dwFlags = 0;




    m_Cookie.Clear();
}

















































































inline void* FixedBlockAllocator::Alloc_Tracked() {
    void* ptr;

    if (m_ptFreeList != NULL) {
        ptr = m_ptFreeList;
        m_ptFreeList = m_ptFreeList->m_pNext;
    } else {
        if (m_nMaxBlockAlloced >= m_nBlockCount) {
            return NULL;
        }


































        ptr = _GetBlock(m_nMaxBlockAlloced);
        m_nMaxBlockAlloced++;
    }

    m_nFreeBlockCount--;
    return ptr;
}





















inline void FixedBlockAllocator::Free(void* pMem) {
    XFreeNode* ptFreeNode = (XFreeNode*)pMem;






























    ptFreeNode->m_pNext = m_ptFreeList;

    m_ptFreeList = ptFreeNode;





    m_nFreeBlockCount++;
}


inline BOOL FixedBlockAllocator::ContainsMem(void* pMem) {
    return pMem >= m_pMemBlocks && pMem < _GetBlock(m_nBlockCount);
}






inline int32 FixedBlockAllocator::GetFreeBlockCount() {




    return m_nFreeBlockCount;
}


inline int32 FixedBlockAllocator::GetUsedBlockCount() {




    return m_nBlockCount - m_nFreeBlockCount;
}


inline POS FixedBlockAllocator::GetFirstBlockPOS() {
    POS pos = NULL;



    uint8* pBlock = _GetBlock(0);
    uint8* pLastBlock = _GetBlock(m_nMaxBlockAlloced);


    while (pBlock < pLastBlock) {

        if (_IsBlockUsed(pBlock)) {
            pos = (POS)pBlock;
            break;
        }


        pBlock += _GetPaddedBlockSize();
    }

    return pos;
}
// NON_MATCHING (only exists inlined): in SubObjectGfxOcclusionHighlightingClearQueries the original keeps the
// free-list null test of _IsBlockUsed inside this loop; here it is hoisted in front of it. Same instructions otherwise.
inline void* FixedBlockAllocator::GetNextBlock(POS& pos) {
    uint8* pBlock = (uint8*)pos;
    uint8* pRet = pBlock;

    pos = NULL;


    uint8* pLastBlock = _GetBlock(m_nMaxBlockAlloced);




    pBlock += _GetPaddedBlockSize();

    while (pBlock < pLastBlock) {

        if (_IsBlockUsed(pBlock)) {
            pos = (POS)pBlock;
            break;
        }

        pBlock += _GetPaddedBlockSize();
    }

    return pRet;
}





























inline void FixedBlockAllocator::Reset() {















    m_nFreeBlockCount = m_nBlockCount;
    m_ptFreeList = NULL;
    m_nMaxBlockAlloced = 0;
}






inline void FixedBlockAllocator::Validate() {
    BOOL bAllBlocksValid = TRUE;

    if (m_ptFreeList != NULL) {
        for (XFreeNode* ptCur = m_ptFreeList; ptCur != NULL; ptCur = ptCur->m_pNext) {
        }
    }

    for (int32 i = 0; i < m_nBlockCount; i++) {











        bAllBlocksValid &= _ValidateBlock(_GetBlock(i));
    }
}










inline uint8* FixedBlockAllocator::_GetBlock(int32 nBlock) {



    return m_pMemBlocks + nBlock * _GetPaddedBlockSize();
}






inline BOOL FixedBlockAllocator::_IsOnBlockBoundary(void* pMem) {
    return ((uint8*)pMem - m_pMemBlocks) % _GetPaddedBlockSize() == 0;
}




inline BOOL FixedBlockAllocator::_ValidateBlock(void* pMem) {
    BOOL bValid = TRUE;











    XASSERT(pMem != NULL);




    XASSERT(pMem >= m_pMemBlocks);
    XASSERT(pMem < _GetBlock(m_nBlockCount));












    XASSERT(_IsOnBlockBoundary(pMem));

    return bValid;
}

























inline BOOL FixedBlockAllocator::_IsBlockUsed(void* pMem) {
    BOOL bUsed = TRUE;









    for (XFreeNode* ptCur = m_ptFreeList; ptCur != NULL; ptCur = ptCur->m_pNext) {

        if (pMem == ptCur) {
            bUsed = FALSE;
            break;
        }
    }

    return bUsed;
}






inline int32 FixedBlockAllocator::_GetPaddedBlockSize() {
    return m_nBlockSize;
}








template <typename TYPE, int COUNT>
XFixedBlockAllocator<TYPE, COUNT>::XFixedBlockAllocator() {
    Initialize();
}



template <typename TYPE, int COUNT>
XFixedBlockAllocator<TYPE, COUNT>::~XFixedBlockAllocator() {
    Dispose();
}



template <typename TYPE, int COUNT>
void XFixedBlockAllocator<TYPE, COUNT>::Initialize() {

    m_tAlloc.InitializeFixed(sizeof(TYPE), COUNT, m_Data, sizeof(m_Data), NULL);
}



template <typename TYPE, int COUNT>
void XFixedBlockAllocator<TYPE, COUNT>::Dispose() {

    m_tAlloc.Dispose();
}











template <typename TYPE, int COUNT>
TYPE* XFixedBlockAllocator<TYPE, COUNT>::Alloc_Tracked() {
    TYPE* pType = NULL;








    void* pMem = m_tAlloc.Alloc_Tracked();


    if (pMem != NULL) {


        pType = new (pMem) TYPE;
    }

    return pType;
}



template <typename TYPE, int COUNT>
void XFixedBlockAllocator<TYPE, COUNT>::Free(TYPE* pValue) {
    pValue->~TYPE();


    m_tAlloc.Free(pValue);
}

























template <typename TYPE, int COUNT>
int32 XFixedBlockAllocator<TYPE, COUNT>::GetFreeBlockCount() {

    return m_tAlloc.GetFreeBlockCount();
}



template <typename TYPE, int COUNT>
int32 XFixedBlockAllocator<TYPE, COUNT>::GetUsedBlockCount() {

    return m_tAlloc.GetUsedBlockCount();
}




template <typename TYPE, int COUNT>
POS XFixedBlockAllocator<TYPE, COUNT>::GetFirstBlockPOS() {

    return m_tAlloc.GetFirstBlockPOS();
}



template <typename TYPE, int COUNT>
TYPE* XFixedBlockAllocator<TYPE, COUNT>::GetNextBlock(POS& pos) {

    return (TYPE*)m_tAlloc.GetNextBlock(pos);
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
// Not checked: XFixedBlockAllocator (template)
static_assert(sizeof(FixedBlockAllocator) == 0x38, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_pMemBlocks) == 0x0, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_ptFreeList) == 0x8, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_nBlockCount) == 0x10, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_nBlockSize) == 0x14, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_nMaxBlockAlloced) == 0x18, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_dwFlags) == 0x1c, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_dwUserData) == 0x20, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_nFreeBlockCount) == 0x24, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_ptMemoryInterface) == 0x28, "FixedBlockAllocator");
static_assert(offsetof(FixedBlockAllocator, m_Cookie) == 0x30, "FixedBlockAllocator");
