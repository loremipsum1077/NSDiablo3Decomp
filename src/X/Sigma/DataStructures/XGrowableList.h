#pragma once

#include "XBaseList.h"
#include "XBaseListNode.h"
#include "XChainedFixedBlockAllocator.h"
#include "XDataStructuresShared.h"
#include "XTemplate.h"

class XMemoryAllocatorInterface;



















template <typename TYPE, typename ARG_TYPE>
class XGrowableList_Sub : public XBaseList<TYPE, ARG_TYPE, XChainedFixedBlockAllocator<XBaseListNode<TYPE> > > {
public:
    inline XGrowableList_Sub();
    ~XGrowableList_Sub();  // no `inline` keyword: with the hint, ~XGrowableList<ModalNotificationManagerBase::NotificationInfo> is not emitted (ModalNotificationManager.cpp)

    inline void Initialize(XMemoryAllocatorInterface* ptMemoryInterface);
    inline void Dispose();

    inline void SetBlockCount_Tracked(const int32 nBlockCount);
    inline void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam);
    inline void SetShrinkable(BOOL bShrinkable);

private:
    XChainedFixedBlockAllocator<XBaseListNode<TYPE> > m_NodeAlloc;
};


























template <typename TYPE, typename ARG_TYPE>
XGrowableList_Sub<TYPE, ARG_TYPE>::XGrowableList_Sub() {
    this->m_ptNodeAllocator = &m_NodeAlloc;
}



template <typename TYPE, typename ARG_TYPE>
XGrowableList_Sub<TYPE, ARG_TYPE>::~XGrowableList_Sub() {
    this->RemoveAll();
}



template <typename TYPE, typename ARG_TYPE>
void XGrowableList_Sub<TYPE, ARG_TYPE>::Initialize(XMemoryAllocatorInterface* ptMemoryInterface) {

    m_NodeAlloc.Initialize();
    m_NodeAlloc.SetMemoryInterface(ptMemoryInterface);

    XBaseList<TYPE, ARG_TYPE, XChainedFixedBlockAllocator<XBaseListNode<TYPE> > >::Initialize();
    this->m_ptNodeAllocator = &m_NodeAlloc;
}



template <typename TYPE, typename ARG_TYPE>
void XGrowableList_Sub<TYPE, ARG_TYPE>::Dispose() {

    XBaseList<TYPE, ARG_TYPE, XChainedFixedBlockAllocator<XBaseListNode<TYPE> > >::Dispose();
    this->m_ptNodeAllocator = NULL;  // line 101
    m_NodeAlloc.Dispose();
}



template <typename TYPE, typename ARG_TYPE>
void XGrowableList_Sub<TYPE, ARG_TYPE>::SetBlockCount_Tracked(const int32 nBlockCount) {










    this->m_ptNodeAllocator->SetBlockCount_Tracked(nBlockCount);
}




template <typename TYPE, typename ARG_TYPE>
void XGrowableList_Sub<TYPE, ARG_TYPE>::SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) {
    this->m_ptNodeAllocator->SetGrow(eGrow, nParam);  // pointer reload in SNOFilesAsyncGlobals::SNOFilesAsyncGlobals
}





template <typename TYPE, typename ARG_TYPE>
void XGrowableList_Sub<TYPE, ARG_TYPE>::SetShrinkable(BOOL bShrinkable) {
    this->m_ptNodeAllocator->SetShrinkable(bShrinkable);
}
















template <typename TYPE, typename ARG_TYPE>
class XGrowableList {
    XGrowableList_Sub<TYPE, ARG_TYPE> m_list;
    BOOL m_ConstructorCalled;

    XGrowableList(const XGrowableList<TYPE, ARG_TYPE>&);
    XGrowableList<TYPE, ARG_TYPE>& operator=(const XGrowableList<TYPE, ARG_TYPE>&);

public:





    inline XGrowableList() { m_ConstructorCalled = TRUE; }
    inline ~XGrowableList() {}

    inline void Initialize(XMemoryAllocatorInterface* ptMemoryInterface) {



        m_list.Initialize(ptMemoryInterface);
    }
    inline void Dispose() { m_list.Dispose(); }

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
    inline POS AddTail_NoAssign_Tracked() { return m_list.AddTail_NoAssign_Tracked(); }
    inline POS InsertBefore_Tracked(POS pos, ARG_TYPE t) { return m_list.InsertBefore_Tracked(pos, t); }
    inline POS InsertAfter_Tracked(POS pos, ARG_TYPE t) { return m_list.InsertAfter_Tracked(pos, t); }

    inline void SetBlockCount_Tracked(const int32 nBlockCount) { m_list.SetBlockCount_Tracked(nBlockCount); }


    inline void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) { m_list.SetGrow(eGrow, nParam); }
    inline void SetShrinkable(BOOL bShrinkable) { m_list.SetShrinkable(bShrinkable); }

    inline POS RemoveAt(POS pos) { return m_list.RemoveAt(pos); }
    inline void RemoveAll() { m_list.RemoveAll(); }
    inline void RemoveHead() { m_list.RemoveHead(); }
    inline void RemoveTail() { m_list.RemoveTail(); }
};
