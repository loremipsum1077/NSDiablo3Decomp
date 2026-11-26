#pragma once

#include "XBaseList.h"
#include "XChainedFixedBlockAllocator.h"
#include "XDataStructuresShared.h"
#include "XTemplate.h"














template <typename T>
class XListMemoryPool : public XChainedFixedBlockAllocator<XBaseListNode<T> > {
};
































template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
class XPooledList_Sub : public XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR> {
public:
    inline XPooledList_Sub();
    inline ~XPooledList_Sub();

    inline void Initialize(NODE_ALLOCATOR* pPool);
    inline void SetPool(NODE_ALLOCATOR* pPool);
    inline void Dispose();
};














template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
XPooledList_Sub<TYPE, ARG_TYPE, NODE_ALLOCATOR>::XPooledList_Sub() {
    this->m_ptNodeAllocator = NULL;
}



template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
XPooledList_Sub<TYPE, ARG_TYPE, NODE_ALLOCATOR>::~XPooledList_Sub() {
    this->RemoveAll();
}











template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XPooledList_Sub<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Initialize(NODE_ALLOCATOR* pPool) {
    XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Initialize();
    this->m_ptNodeAllocator = pPool;
}



template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XPooledList_Sub<TYPE, ARG_TYPE, NODE_ALLOCATOR>::SetPool(NODE_ALLOCATOR* pPool) {
    this->m_ptNodeAllocator = pPool;
}



template <typename TYPE, typename ARG_TYPE, typename NODE_ALLOCATOR>
void XPooledList_Sub<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Dispose() {
    XBaseList<TYPE, ARG_TYPE, NODE_ALLOCATOR>::Dispose();
}















template <typename TYPE, typename ARG_TYPE>
class XPooledList {
    XPooledList_Sub<TYPE, ARG_TYPE, XListMemoryPool<TYPE> > m_list;

public:

    inline XPooledList() { m_ConstructorCalled = TRUE; }
    inline XPooledList(XListMemoryPool<TYPE>* pPool) { m_ConstructorCalled = TRUE; m_list.Initialize(pPool); }
    inline ~XPooledList() {}
    inline void Initialize(XListMemoryPool<TYPE>* pPool) { m_list.Initialize(pPool); }
    inline void SetPool(XListMemoryPool<TYPE>* pPool) { m_list.SetPool(pPool); }
    inline void Dispose() { m_list.Dispose(); }

    inline POS GetHeadPosition() const { return m_list.GetHeadPosition(); }
    inline POS GetTailPosition() const { return m_list.GetTailPosition(); }
    inline const TYPE& GetNext(POS& pos) const { return m_list.GetNext(pos); }
    inline TYPE& GetNext(POS& pos) { return m_list.GetNext(pos); }
    inline static TYPE& sGetNext(POS& pos) { return XBaseList<TYPE, ARG_TYPE, XListMemoryPool<TYPE> >::sGetNext(pos); }
    inline const TYPE& GetPrev(POS& pos) const { return m_list.GetPrev(pos); }
    inline TYPE& GetPrev(POS& pos) { return m_list.GetPrev(pos); }

    inline TYPE& GetAt(const POS& pos) { return m_list.GetAt(pos); }
    inline const TYPE& GetHead() const { return m_list.GetHead(); }
    inline TYPE& GetHead() { return m_list.GetHead(); }
    inline const TYPE& GetTail() const { return m_list.GetTail(); }
    inline TYPE& GetTail() { return m_list.GetTail(); }



    inline POS Find(ARG_TYPE tData) const { return m_list.Find(tData); }

    inline void Sort(int32 (*pfnCompare)(const void*, const void*)) { m_list.Sort(pfnCompare); }

    inline int32 Count() const { return m_list.Count(); }
    inline BOOL IsEmpty() const { return m_list.IsEmpty(); }







    inline POS AddHead_Tracked(ARG_TYPE t) { return m_list.AddHead_Tracked(t); }
    inline POS AddTail_Tracked(ARG_TYPE t) { return m_list.AddTail_Tracked(t); }
    inline POS InsertBefore_Tracked(POS pos, ARG_TYPE t) { return m_list.InsertBefore_Tracked(pos, t); }
    inline void MoveToTail(POS pos) { m_list.MoveToTail(pos); }  // line 189
    inline void MoveToHead(POS pos) { m_list.MoveToHead(pos); }  // 2.7.7 (inlined into 0x71005285b0)
    inline void Copy(const XPooledList<TYPE, ARG_TYPE> & source);  // line 192, defined below


    inline void RemoveAt(POS pos) { m_list.RemoveAt(pos); }
    inline void RemoveAll() { m_list.RemoveAll(); }
    inline void RemoveHead() { m_list.RemoveHead(); }
    inline void RemoveTail() { m_list.RemoveTail(); }
    inline void Splice(XPooledList<TYPE, ARG_TYPE> & tOther) { m_list.Splice(tOther.m_list); }

private:
    BOOL m_ConstructorCalled;
};
template <typename TYPE, typename ARG_TYPE> inline void XPooledList<TYPE, ARG_TYPE>::Copy(const XPooledList<TYPE, ARG_TYPE> & source)
{


    if (Count() != 0)
    {
        RemoveAll();
    }
    POS pos = source.GetHeadPosition();
    while (pos != NULL)
    {
        const TYPE & value = source.GetNext(pos);



        AddTail_Tracked(value);
    }
}
