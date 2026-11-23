#pragma once

#include "XBaseArray.h"
#include "XDataStructuresShared.h"

class XMemoryAllocatorInterface;























template <typename TYPE, typename ARG_TYPE>
class XGrowableArray_Sub : public XBaseArray<TYPE, ARG_TYPE, XDynamicAllocator> {
public:
    inline XGrowableArray_Sub();
    inline ~XGrowableArray_Sub();

    inline void Initialize();
    inline void SetUseSystemAllocator(BOOL bValue);
    inline void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface);
    inline void InitializeFromSerializeBlock(void* pData, int32 nSize, int32 nElementSize,
                                      XMemoryAllocatorInterface* ptMemoryInterface);

    inline void SetAtGrow_Tracked(const int32 nIndex, ARG_TYPE tData);
    inline void Insert_Tracked(const int32 nIndex, ARG_TYPE tData);
    inline TYPE& GetAtGrow_Tracked(const int32 nIndex);
    inline void Reserve_Tracked(const int32 nSize);
    inline void Copy_Tracked(const XGrowableArray_Sub<TYPE, ARG_TYPE>& tSource);
    inline const XGrowableArray_Sub<TYPE, ARG_TYPE>& operator=(const XGrowableArray_Sub<TYPE, ARG_TYPE>& tSource);
};

































































template <typename TYPE, typename ARG_TYPE>
XGrowableArray_Sub<TYPE, ARG_TYPE>::XGrowableArray_Sub() {
}



template <typename TYPE, typename ARG_TYPE>
XGrowableArray_Sub<TYPE, ARG_TYPE>::~XGrowableArray_Sub() {




    this->SetArraySize_Tracked(0);
}




template <typename TYPE, typename ARG_TYPE>
void XGrowableArray_Sub<TYPE, ARG_TYPE>::Initialize() {

    XBaseArray<TYPE, ARG_TYPE, XDynamicAllocator>::Initialize();
}











template <typename TYPE, typename ARG_TYPE>
void XGrowableArray_Sub<TYPE, ARG_TYPE>::SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) {
    this->m_MemAllocator.SetMemoryInterface(ptMemoryInterface);
}




template <typename TYPE, typename ARG_TYPE>
void XGrowableArray_Sub<TYPE, ARG_TYPE>::InitializeFromSerializeBlock(void* pData, int32 nSize, int32 nElementSize,
                                                                    XMemoryAllocatorInterface* ptMemoryInterface) {
    this->m_MemAllocator.InitializeFromSerializeBlock(pData, nSize, ptMemoryInterface);
    this->m_nArraySize = nSize / nElementSize;
    this->m_pData = (TYPE*)pData;
}






template <typename TYPE, typename ARG_TYPE>
void XGrowableArray_Sub<TYPE, ARG_TYPE>::Reserve_Tracked(const int32 nSize) {
    this->m_pData = (TYPE*)this->m_MemAllocator.Reserve_Tracked(nSize * sizeof(TYPE));
}














template <typename TYPE, typename ARG_TYPE>
void XGrowableArray_Sub<TYPE, ARG_TYPE>::SetAtGrow_Tracked(const int32 nIndex, ARG_TYPE tData) {
    if (nIndex >= this->m_nArraySize) {
        this->_ResizeArray_Tracked(nIndex + 1);
    }

    this->m_pData[nIndex] = tData;
}




































































template <typename TYPE, typename ARG_TYPE>
TYPE& XGrowableArray_Sub<TYPE, ARG_TYPE>::GetAtGrow_Tracked(const int32 nIndex) {
    if (nIndex >= this->m_nArraySize) {
        this->_ResizeArray_Tracked(nIndex + 1);
    }

    return this->m_pData[nIndex];
}


















































































































template <typename TYPE, typename ARG_TYPE>
class XGrowableArray {
    XGrowableArray(const XGrowableArray<TYPE, ARG_TYPE>&);
    XGrowableArray<TYPE, ARG_TYPE>& operator=(const XGrowableArray<TYPE, ARG_TYPE>&);

public:





    inline XGrowableArray() { m_ConstructorCalled = TRUE; }

    inline ~XGrowableArray() {}

    inline void Initialize() { m_array.Initialize(); }
    inline void Dispose() { m_array.Dispose(); }

    inline void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) { m_array.SetMemoryInterface(ptMemoryInterface); }

    inline void InitializeFromSerializeBlock(void* pData, int32 nSize, int32 nElementSize, XMemoryAllocatorInterface* ptMemoryInterface) { m_array.InitializeFromSerializeBlock(pData, nSize, nElementSize, ptMemoryInterface); }
    inline void SafeDispose() { m_array.SafeDispose(); }

    inline const TYPE& operator[](const size_t nIndex) const { return m_array[nIndex]; }
    inline TYPE& operator[](const size_t nIndex) { return m_array[nIndex]; }

    inline void EmptyArray() { m_array.EmptyArray(); }
    inline int32 GetArraySize() const { return m_array.GetArraySize(); }









    inline void SetAtGrow_Tracked(const int32 nIndex, ARG_TYPE tData) { m_array.SetAtGrow_Tracked(nIndex, tData); }
    inline TYPE& GetAtGrow_Tracked(const int32 nIndex) { return m_array.GetAtGrow_Tracked(nIndex); }
    inline void SetArraySize_Tracked(const int32 nSize) { m_array.SetArraySize_Tracked(nSize); }
    inline void Reserve_Tracked(const int32 nSize) { m_array.Reserve_Tracked(nSize); }

    inline void Copy_Tracked(const XGrowableArray<TYPE, ARG_TYPE>& tSource);

























    inline void Add_Tracked(ARG_TYPE tData) { m_array.Add_Tracked(tData); }



    inline void RemoveAt(const int32 nIndex) { m_array.RemoveAt(nIndex); }



    inline int32 Find(ARG_TYPE tValue) const { return m_array.Find(tValue); }
















































































    inline const TYPE* GetPointer() const { return m_array.GetPointer(); }
    inline TYPE* GetPointer() { return m_array.GetPointer(); }

    XGrowableArray_Sub<TYPE, ARG_TYPE> m_array;
    BOOL m_ConstructorCalled;
};
