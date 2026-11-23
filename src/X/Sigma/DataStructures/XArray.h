#pragma once

#include "XBaseArray.h"
#include "XDataStructuresShared.h"
#include "XFixedAllocator.h"























template <typename TYPE, typename ARG_TYPE, int CAPACITY>
class XArray_Sub : public XBaseArray<TYPE, ARG_TYPE, XFixedAllocator<sizeof(TYPE), CAPACITY> > {
public:
    inline ~XArray_Sub();
};









template <typename TYPE, typename ARG_TYPE, int CAPACITY>
XArray_Sub<TYPE, ARG_TYPE, CAPACITY>::~XArray_Sub() {





    this->SetArraySize_Tracked(0);

}















template <typename TYPE, typename ARG_TYPE, int CAPACITY>
class XArray {
    XArray_Sub<TYPE, ARG_TYPE, CAPACITY> m_array;

public:


    inline void Initialize() { m_array.Initialize(); }
    inline void Dispose() { m_array.Dispose(); }


    inline const TYPE& operator[](const int32 nIndex) const { return m_array[nIndex]; }
    inline TYPE& operator[](const int32 nIndex) { return m_array[nIndex]; }
    inline int32 GetArraySize() const { return m_array.GetArraySize(); }
    inline void EmptyArray() { m_array.EmptyArray(); }
    inline const TYPE* GetPointer() const { return m_array.GetPointer(); }
    inline TYPE* GetPointer() { return m_array.GetPointer(); }






    inline void SetArraySize_Tracked(const int32 nCount) { m_array.SetArraySize_Tracked(nCount); }
    inline void Add_Tracked(ARG_TYPE tData) { m_array.Add_Tracked(tData); }
    inline TYPE& AppendElement_Tracked() { return m_array.AppendElement_Tracked(); }


    inline void RemoveAt(const int32 nIndex) { m_array.RemoveAt(nIndex); }
    inline void RemoveAtReorder(const int32 nIndex) { m_array.RemoveAtReorder(nIndex); }

    inline int32 Find(ARG_TYPE tValue) const { return m_array.Find(tValue); }
};







template <typename TYPE, typename ARG_TYPE, int CAPACITY>
class XArrayPregrown : public XArray<TYPE, ARG_TYPE, CAPACITY> {
public:

    inline XArrayPregrown() {




        this->SetArraySize_Tracked(CAPACITY);
    }
};
