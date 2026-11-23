#pragma once

#include "XBaseList.h"
#include "XDataStructuresShared.h"
#include "XFixedBlockAllocator.h"
#include "XTemplate.h"
























template <typename TYPE, typename ARG_TYPE, int SIZE>
class XList_Sub : public XBaseList<TYPE, ARG_TYPE, XFixedBlockAllocator<XBaseListNode<TYPE>, SIZE> > {


public:
    inline XList_Sub();
    inline ~XList_Sub();

    inline void Initialize();
    inline void Dispose();

private:
    XFixedBlockAllocator<XBaseListNode<TYPE>, SIZE> m_NodeAlloc;
};














template <typename TYPE, typename ARG_TYPE, int SIZE>
XList_Sub<TYPE, ARG_TYPE, SIZE>::XList_Sub() {

    this->m_ptNodeAllocator = &m_NodeAlloc;
}


template <typename TYPE, typename ARG_TYPE, int SIZE>
XList_Sub<TYPE, ARG_TYPE, SIZE>::~XList_Sub() {

    this->RemoveAll();
}

// Initialize and Dispose are declared in the DWARF but never instantiated in
// the binary, so they have no bodies here.

































template <typename TYPE, typename ARG_TYPE, int SIZE>
class XList {
    XList_Sub<TYPE, ARG_TYPE, SIZE> m_list;

public:





    inline POS GetHeadPosition() const { return m_list.GetHeadPosition(); }
    inline POS GetTailPosition() const { return m_list.GetTailPosition(); }
    inline const TYPE& GetNext(POS& pos) const { return m_list.GetNext(pos); }
    inline TYPE& GetNext(POS& pos) { return m_list.GetNext(pos); }
    inline const TYPE& GetPrev(POS& pos) const { return m_list.GetPrev(pos); }
    inline TYPE& GetPrev(POS& pos) { return m_list.GetPrev(pos); }

    inline const TYPE& GetHead() const { return m_list.GetHead(); }
    inline TYPE& GetHead() { return m_list.GetHead(); }
    inline const TYPE& GetTail() const { return m_list.GetTail(); }
    inline TYPE& GetTail() { return m_list.GetTail(); }

    inline TYPE& GetAt(const POS& pos) { return m_list.GetAt(pos); }

    inline POS Find(ARG_TYPE tData) const { return m_list.Find(tData); }



    inline int32 Count() const { return m_list.Count(); }
    inline BOOL IsEmpty() const { return m_list.IsEmpty(); }







    inline POS AddHead_Tracked(ARG_TYPE t) { return m_list.AddHead_Tracked(t); }
    inline POS AddTail_Tracked(ARG_TYPE t) { return m_list.AddTail_Tracked(t); }

    inline void MoveToTail(const POS& pos) { m_list.MoveToTail(pos); }


    inline void RemoveAt(POS pos) { m_list.RemoveAt(pos); }
    inline void RemoveAll() { m_list.RemoveAll(); }
    inline void RemoveHead() { m_list.RemoveHead(); }
    inline void RemoveTail() { m_list.RemoveTail(); }
};
