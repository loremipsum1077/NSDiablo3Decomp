#pragma once

#include "DataStructuresExternal.h"
#include "XDataStructuresShared.h"
#include "XDynamicAllocator.h"
#include "XFixedAllocator.h"
#include "XPtr64.h"

#include <new>
#include <string.h>

































template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
class XBaseArray {
public:
    inline XBaseArray();

    inline void Initialize();
    inline void Dispose();
    inline void SafeDispose();

    inline const TYPE& operator[](const int32 nIndex) const;
    inline TYPE& operator[](const int32 nIndex);

    inline const TYPE* GetPointer() const;
    inline TYPE* GetPointer();

    inline int32 GetArraySize() const;
    inline void EmptyArray();

    inline void Add_Tracked(ARG_TYPE tData);
    inline TYPE& AppendElement_Tracked();
    inline void SetArraySize_Tracked(const int32 nSize);

    inline void RemoveAt(const int32 nIndex);
    inline void RemoveAtReorder(const int32 nIndex);

    inline int32 Find(ARG_TYPE tValue) const;

protected:
    inline void _ResizeArray_Tracked(const int32 nNewSize);
    inline void _PreResizeArray(const int32 nOldSize, const int32 nNewSize);
    inline void _PostResizeArray(const int32 nOldSize, const int32 nNewSize);

    XPtr64<TYPE> m_pData;
    int32 m_nArraySize;
    MEMORY_ALLOCATOR m_MemAllocator;
};














template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::Add_Tracked(ARG_TYPE tData) {

    AppendElement_Tracked() = tData;
}















template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
TYPE& XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::AppendElement_Tracked() {
    int32 nOldArraySize = GetArraySize();
    _ResizeArray_Tracked(nOldArraySize + 1);
    return (*this)[nOldArraySize];
}


template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::SetArraySize_Tracked(const int32 nSize) {
    int32 nOldSize = m_nArraySize;
    m_nArraySize = nSize;









    _PreResizeArray(nOldSize, nSize);




    m_pData = (TYPE*)m_MemAllocator.SetSize_Tracked(nSize * sizeof(TYPE));


    _PostResizeArray(nOldSize, nSize);
}

























































template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::XBaseArray() {

    m_pData = NULL;
    m_nArraySize = 0;
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::Initialize() {

    m_pData = NULL;
    m_nArraySize = 0;
    m_MemAllocator.Initialize();
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::Dispose() {

    m_pData = NULL;
    m_nArraySize = 0;
    m_MemAllocator.Dispose();
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::SafeDispose() {
    m_pData = NULL;
    m_nArraySize = 0;
    if (!m_MemAllocator.IsDisposed()) {
        m_MemAllocator.Dispose();
    }
}







template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
const TYPE& XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::operator[](const int32 nIndex) const {










    return m_pData[nIndex];
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
TYPE& XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::operator[](const int32 nIndex) {










    return m_pData[nIndex];
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
const TYPE* XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::GetPointer() const {
    return m_pData;
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
TYPE* XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::GetPointer() {
    return m_pData;
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
int32 XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::GetArraySize() const {
    return m_nArraySize;
}




template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::EmptyArray() {
    _PreResizeArray(m_nArraySize, 0);
    m_nArraySize = 0;
}





template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::RemoveAt(const int32 nIndex) {
    (&m_pData[nIndex])->~TYPE();

    // Lines 320-328 from the copy inlined into X/Server/STimedEvents.cpp (STimedEventReset).
    if (m_nArraySize - 1 - nIndex > 0) {
        void * pDataDest = m_pData.Get() + nIndex;
        void * pDataSrc = m_pData.Get() + nIndex + 1;
        size_t nSize = (m_nArraySize - 1 - nIndex) * sizeof(TYPE);
        SigmaMemoryMove(pDataDest, pDataSrc, nSize);
    }
    m_nArraySize--;
}







template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::RemoveAtReorder(const int32 nIndex) {
    // DWARF (SubObjectGfx.cpp inline copy): if 344:10 (`<`, 19/32 cols), assignment 347:25/27, decrement 351:17.








    (&m_pData[nIndex])->~TYPE();
    if ( nIndex < m_nArraySize - 1 ) {


        m_pData[nIndex] = m_pData[m_nArraySize - 1];
    }


    m_nArraySize--;
}











template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::_ResizeArray_Tracked(const int32 nNewSize) {
    int32 nOldSize = m_nArraySize;
    m_nArraySize = nNewSize;









    _PreResizeArray(nOldSize, nNewSize);






    m_pData = (TYPE*)m_MemAllocator.Reserve_Tracked(nNewSize * sizeof(TYPE));


    _PostResizeArray(nOldSize, nNewSize);
}



template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::_PreResizeArray(const int32 nOldSize, const int32 nNewSize) {
    for (int32 i = nNewSize; i < nOldSize; i++) {







        (&m_pData[i])->~TYPE();
    }
}




template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
void XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::_PostResizeArray(const int32 nOldSize, const int32 nNewSize) {
    for (int32 i = nOldSize; i < nNewSize; i++) {







        new (&m_pData[i]) TYPE;
    }
}












template <typename TYPE, typename ARG_TYPE, typename MEMORY_ALLOCATOR>
int32 XBaseArray<TYPE, ARG_TYPE, MEMORY_ALLOCATOR>::Find(ARG_TYPE tValue) const {
    for (int32 i = 0; i < GetArraySize(); i++) {
        if (m_pData[i] == tValue) {
            return i;
        }
    }

    return -1;
}
