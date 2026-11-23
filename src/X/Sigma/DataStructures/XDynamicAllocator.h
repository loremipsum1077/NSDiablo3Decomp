#pragma once

#include "DataStructuresExternal.h"
#include "XAllocator.h"
#include "XPtr64.h"

class XMemoryAllocatorInterface;

class XDynamicAllocator {
public:
    XDynamicAllocator();
    ~XDynamicAllocator();

    void Initialize(XMemoryAllocatorInterface* ptMemoryInterface = NULL);
    void InitializeFromSerializeBlock(void* pData, int32 nSize, XMemoryAllocatorInterface* ptMemoryInterface);
    void Dispose();
    BOOL IsDisposed();

    void SetUseSystemAllocator(BOOL bValue);
    void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface);
    void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam);

    void* Reserve_Tracked(int32 nSize);
    void* SetSize_Tracked(int32 nSize);
    int32 GetSize();

    int32 _GetNewSize(int32 nSizeHint);
    void* _InternalResize(void* pMem, int32 nSize);

    XPtr64<unsigned char> m_pMem;
    int32 m_nSize;
    uint8 m_ucFlags;
    CHAR m_eGrowAlgorithm;
    uint16 m_nGrowParam;
    XPtr64<XMemoryAllocatorInterface> m_ptMemoryInterface;
    CCookie m_Cookie;
};


































































































inline XDynamicAllocator::XDynamicAllocator() {
    Initialize();
}


inline XDynamicAllocator::~XDynamicAllocator() {
    Dispose();
}




inline void XDynamicAllocator::Initialize(XMemoryAllocatorInterface* ptMemoryInterface) {
    m_pMem = NULL;
    m_nSize = 0;
    m_ucFlags = (1 << XALLOC_FLAG_MANAGE_MEM_BLOCK);
    m_eGrowAlgorithm = XALLOC_GROW_EXPONENTIAL;
    m_nGrowParam = 2;
    m_ptMemoryInterface = ptMemoryInterface;
    m_Cookie.Set();
}





inline void XDynamicAllocator::InitializeFromSerializeBlock(void* pData, int32 nSize,
                                                            XMemoryAllocatorInterface* ptMemoryInterface) {
    m_pMem = (unsigned char*)pData;
    m_nSize = nSize;
    m_ucFlags = (1 << XALLOC_FLAG_MANAGE_MEM_BLOCK);
    m_eGrowAlgorithm = XALLOC_GROW_EXPONENTIAL;
    m_nGrowParam = 2;
    m_ptMemoryInterface = ptMemoryInterface;
    m_Cookie.Set();
}






inline void XDynamicAllocator::Dispose() {
    if (m_ucFlags & (1 << XALLOC_FLAG_MANAGE_MEM_BLOCK)) {
        if (m_ucFlags & (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY)) {
            free(m_pMem);
        } else {








            XFREE(m_pMem, m_ptMemoryInterface);
        }
    }

    m_pMem = NULL;
    m_nSize = 0;
    m_ucFlags = 0;

    m_Cookie.Clear();
}



inline BOOL XDynamicAllocator::IsDisposed() {
    return m_Cookie.IsCleared();
}




















inline void XDynamicAllocator::SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) {
    m_ptMemoryInterface = ptMemoryInterface;
}


























inline void* XDynamicAllocator::Reserve_Tracked(int32 nSize) {
    if (m_nSize < nSize) {













        m_nSize = _GetNewSize(nSize);




        m_pMem = (unsigned char*)_InternalResize(m_pMem, m_nSize);
    }


    return m_pMem;
}



inline void* XDynamicAllocator::SetSize_Tracked(int32 nSize) {
    if (m_nSize != nSize) {














        m_pMem = (unsigned char*)_InternalResize(m_pMem, nSize);
        m_nSize = nSize;
    }



    return m_pMem;
}















inline int32 XDynamicAllocator::_GetNewSize(int32 nSizeHint) {
    int32 nNewSize;

    switch (m_eGrowAlgorithm) {
    case XALLOC_GROW_LINEAR:
        nNewSize = m_nSize + m_nGrowParam;
        break;
    case XALLOC_GROW_EXPONENTIAL:
        nNewSize = m_nSize * m_nGrowParam;
        break;
    default:
        nNewSize = 0;
        break;
    }

    if (nNewSize < nSizeHint) {
        nNewSize = nSizeHint;
    }

    return nNewSize;
}











inline void* XDynamicAllocator::_InternalResize(void* pMem, int32 nSize) {
    if (m_ucFlags & (1 << XALLOC_FLAG_USE_SYSTEM_MEMORY)) {
        return realloc(pMem, nSize);
    }

    if (nSize > 0) {
        if (pMem == NULL) {






















            return SigmaMemoryNew(nSize, 4, m_ptMemoryInterface, FALSE);
        }


        return SigmaMemoryResize(pMem, m_ptMemoryInterface, nSize, 4);
    }



    SigmaMemoryFree(pMem, m_ptMemoryInterface);
    return NULL;
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(XDynamicAllocator) == 0x20, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_pMem) == 0x0, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_nSize) == 0x8, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_ucFlags) == 0xc, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_eGrowAlgorithm) == 0xd, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_nGrowParam) == 0xe, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_ptMemoryInterface) == 0x10, "XDynamicAllocator");
static_assert(offsetof(XDynamicAllocator, m_Cookie) == 0x18, "XDynamicAllocator");
