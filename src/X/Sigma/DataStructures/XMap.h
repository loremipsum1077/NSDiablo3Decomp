#pragma once

#include "XArray.h"
#include "XBaseMap.h"
#include "XFixedBlockAllocator.h"
#include "XMapAssoc.h"
#include "XTemplate.h"



























template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, int SIZE>
class XMap_Sub : public XBaseMap<Key, ARG_KEY, VALUE, ARG_VALUE, XFixedBlockAllocator<XMapAssoc<Key, VALUE>, SIZE>,
                                 XArrayPregrown<XMapAssoc<Key, VALUE>*, XMapAssoc<Key, VALUE>*, SIZE>, SIZE> {
public:
    inline XMap_Sub();
    inline ~XMap_Sub();

    // Declared in the DWARF but never instantiated in the binary; no bodies.
    inline void Initialize();
    inline void Dispose();

private:
    XFixedBlockAllocator<XMapAssoc<Key, VALUE>, SIZE> m_AssocAllocator;
};













template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, int SIZE>
XMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, SIZE>::XMap_Sub() {


    this->m_arAssoc = &m_AssocAllocator;




    this->_InitHashTable_Tracked(SIZE);
}





template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, int SIZE>
XMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, SIZE>::~XMap_Sub() {






    this->RemoveAll();
}






































template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, int SIZE>
class XMap {
    XMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE, SIZE> m_map;

public:






    inline BOOL Lookup(ARG_KEY key, VALUE& value) const { return m_map.Lookup(key, value); }




    inline void SetAt_Tracked(ARG_KEY key, ARG_VALUE value) { m_map.SetAt_Tracked(key, value); }

    inline void RemoveAt(ARG_KEY key) { m_map.RemoveAt(key); }
    inline void RemoveAll() { m_map.RemoveAll(); }

    inline POS GetStartAssoc() const { return m_map.GetStartAssoc(); }

    inline void GetNextAssoc(POS& pos, Key& key, VALUE& value) const { m_map.GetNextAssoc(pos, key, value); }
    inline int32 Count() const { return m_map.Count(); }
    inline BOOL IsEmpty() const { return m_map.IsEmpty(); }
};
