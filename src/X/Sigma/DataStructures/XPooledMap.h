#pragma once

#include "XArray.h"
#include "XBaseMap.h"
#include "XChainedFixedBlockAllocator.h"
#include "XMapAssoc.h"
#include "XTemplate.h"














template <typename Key, typename VALUE>
class XMapMemoryPool : public XChainedFixedBlockAllocator<XMapAssoc<Key, VALUE> > {
};





















template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR,
          int HASH_SIZE>
class XPooledMap_Sub
    : public XBaseMap<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR,
                      XArrayPregrown<XMapAssoc<Key, VALUE>*, XMapAssoc<Key, VALUE>*, HASH_SIZE>, HASH_SIZE> {
public:
    inline XPooledMap_Sub();
    inline ~XPooledMap_Sub();

    inline BOOL IsDisposed();

    inline void Initialize(ASSOC_ALLOCATOR* pPool);
    inline void Dispose();
    inline void SetPool(ASSOC_ALLOCATOR* pPool);
};
















template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR, int HASH_SIZE>
XPooledMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR, HASH_SIZE>::XPooledMap_Sub() {





    this->_InitHashTable_Tracked(HASH_SIZE);
}



template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR, int HASH_SIZE>
XPooledMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR, HASH_SIZE>::~XPooledMap_Sub() {






    this->RemoveAll();
}










template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR, int HASH_SIZE>
void XPooledMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR, HASH_SIZE>::Initialize(ASSOC_ALLOCATOR* pPool) {
    this->m_arHashTable.Initialize();

    this->m_arAssoc = pPool;





    this->_InitHashTable_Tracked(HASH_SIZE);
}



template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR, int HASH_SIZE>
void XPooledMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR, HASH_SIZE>::Dispose() {






    this->RemoveAll();

    this->m_arAssoc = NULL;
    this->m_nHashMask = 0;
    this->m_nCount = 0;
    this->m_arHashTable.Dispose();
}

template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR, int HASH_SIZE>
void XPooledMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR, HASH_SIZE>::SetPool(ASSOC_ALLOCATOR* pPool) {


    this->m_arAssoc = pPool;
}





















template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, int HASH_SIZE>
class XPooledMap {
public:
    XPooledMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, XMapMemoryPool<Key, VALUE>, HASH_SIZE> m_map;




    inline XPooledMap() { m_ConstructorCalled = TRUE; }
    inline XPooledMap(XMapMemoryPool<Key, VALUE>* pPool) { m_ConstructorCalled = TRUE; m_map.Initialize(pPool); }
    inline ~XPooledMap() {}
    inline void Initialize(XMapMemoryPool<Key, VALUE>* pPool) { m_map.Initialize(pPool); }
    inline void SetPool(XMapMemoryPool<Key, VALUE>* pPool) { m_map.SetPool(pPool); }
    inline void Dispose() { m_map.Dispose(); }



    inline BOOL Lookup(ARG_KEY key, VALUE& value) const { return m_map.Lookup(key, value); }




    inline void SetAt_Tracked(ARG_KEY key, ARG_VALUE value) { m_map.SetAt_Tracked(key, value); }


    inline void RemoveAt(ARG_KEY key) { m_map.RemoveAt(key); }
    inline void RemoveAll() { m_map.RemoveAll(); }

    inline POS GetStartAssoc() const { return m_map.GetStartAssoc(); }

    inline void GetNextAssoc(POS& pos, Key& key, VALUE& value) const { m_map.GetNextAssoc(pos, key, value); }
    inline int32 Count() const { return m_map.Count(); }
    inline BOOL IsEmpty() const { return m_map.IsEmpty(); }

private:
    BOOL m_ConstructorCalled;
};
