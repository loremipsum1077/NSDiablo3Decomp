#pragma once

#include "XBaseMap.h"
#include "XChainedFixedBlockAllocator.h"
#include "XGrowableArray.h"
#include "XMapAssoc.h"
#include "XTemplate.h"

























template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
class XGrowableMap_Sub
    : public XBaseMap<Key, ARG_KEY, VALUE, ARG_VALUE, XChainedFixedBlockAllocator<XMapAssoc<Key, VALUE> >,
                      XGrowableArray<XMapAssoc<Key, VALUE>*, XMapAssoc<Key, VALUE>*>, 0> {
    typedef XBaseMap<Key, ARG_KEY, VALUE, ARG_VALUE, XChainedFixedBlockAllocator<XMapAssoc<Key, VALUE> >,
                     XGrowableArray<XMapAssoc<Key, VALUE>*, XMapAssoc<Key, VALUE>*>, 0>
        XBaseMapSpecific;

public:
    inline XGrowableMap_Sub(int32 nBlockCount, XMemoryAllocatorInterface* ptMemoryInterface);
    inline ~XGrowableMap_Sub();

    inline void Initialize(int32 nBlockCount, BOOL bUseSystemMemory, XMemoryAllocatorInterface* ptMemoryInterface);
    inline void Dispose();

    inline void SetBlockCount_Tracked(int32 nBlockCount);
    inline void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam);
    inline void SetShrinkable(BOOL bShrinkable);

private:
    XChainedFixedBlockAllocator<XMapAssoc<Key, VALUE> > m_AssocAllocator;
};






















template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::XGrowableMap_Sub(int32 nBlockCount,
                                                                 XMemoryAllocatorInterface* ptMemoryInterface) {
    this->m_arAssoc = &m_AssocAllocator;


    this->SetMemoryInterface(ptMemoryInterface);





    this->_InitHashTable_Tracked(nBlockCount);
    this->SetHashTableSize_Tracked(16);
}





template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::~XGrowableMap_Sub() {





    this->RemoveAll();
}



template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::Initialize(int32 nBlockCount, BOOL bUseSystemMemory,
                                                                XMemoryAllocatorInterface* ptMemoryInterface) {
    this->m_arAssoc = &m_AssocAllocator;



    XBaseMapSpecific::Initialize(nBlockCount, bUseSystemMemory, ptMemoryInterface);
}



template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::Dispose() {
    XBaseMapSpecific::Dispose();
}




template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) {  // Original line 133; pointer reloads at Tags.cpp 0x6a3690 and SNOFiles.cpp 0x69631c.
    this->m_arAssoc->SetGrow(eGrow, nParam);
}





template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::SetBlockCount_Tracked(int32 nBlockCount) {
    // Through m_arAssoc, not m_AssocAllocator: StringListGroupDef::Postprocess reloads the pointer
    // (the call is on line 150 of the original, twelve lines into the body).
    this->m_arAssoc->SetBlockCount_Tracked(nBlockCount);
}














template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
void XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE>::SetShrinkable(BOOL bShrinkable) {  // Original line 159; pointer reloads at Tags.cpp 0x6a36a4 and SNOFiles.cpp 0x696330.
    this->m_arAssoc->SetShrinkable(bShrinkable);
}




















template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE>
class XGrowableMap {
    XGrowableMap_Sub<Key, ARG_KEY, VALUE, ARG_VALUE> m_map;
    BOOL m_ConstructorCalled;

    XGrowableMap(const XGrowableMap<Key, ARG_KEY, VALUE, ARG_VALUE>&);
    XGrowableMap<Key, ARG_KEY, VALUE, ARG_VALUE>& operator=(const XGrowableMap<Key, ARG_KEY, VALUE, ARG_VALUE>&);

public:





    // The defaults are what SNOGlobals' implicit constructor passes (SNOFiles.cpp:923).
    inline XGrowableMap(int32 nBlockCount = 16, XMemoryAllocatorInterface* ptMemoryInterface = NULL)
        : m_map(nBlockCount, ptMemoryInterface) {
        m_ConstructorCalled = TRUE;
    }




    inline ~XGrowableMap() {}

    inline void Initialize(int32 nBlockCount, BOOL bUseSystemMemory, XMemoryAllocatorInterface* ptMemoryInterface) {
        m_map.Initialize(nBlockCount, bUseSystemMemory, ptMemoryInterface);
    }





    inline void Dispose() { m_map.Dispose(); }


    inline BOOL Lookup(ARG_KEY key, VALUE& value) const { return m_map.Lookup(key, value); }

    inline VALUE* LookupPtr(ARG_KEY key) const { return m_map.LookupPtr(key); }






    inline void SetAt_Tracked(ARG_KEY key, ARG_VALUE value) { m_map.SetAt_Tracked(key, value); }
    inline void SetHashTableSize_Tracked(const int32 nHashTableSize) { m_map.SetHashTableSize_Tracked(nHashTableSize); }
    inline void SetBlockCount_Tracked(int32 nBlockCount) { m_map.SetBlockCount_Tracked(nBlockCount); }


    inline void SetGrow(XAllocatorGrowAlgorithm eGrow, int32 nParam) { m_map.SetGrow(eGrow, nParam); }
    inline void SetShrinkable(BOOL bShrinkable) { m_map.SetShrinkable(bShrinkable); }

    inline void RemoveAt(ARG_KEY key) { m_map.RemoveAt(key); }
    inline void RemoveAll() { m_map.RemoveAll(); }

    inline POS GetStartAssoc() const { return m_map.GetStartAssoc(); }

    inline void GetNextAssoc(POS& pos, Key& key, VALUE& value) const { m_map.GetNextAssoc(pos, key, value); }

    inline VALUE* GetNextAssocPtr(POS& pos, Key& key) const { return m_map.GetNextAssocPtr(pos, key); }

    inline void GetNextAssocKey(POS& pos, Key& key) const { m_map.GetNextAssocKey(pos, key); }
    inline int32 Count() const { return m_map.Count(); }
    inline BOOL IsEmpty() const { return m_map.IsEmpty(); }

    inline void Copy(const XGrowableMap<Key, ARG_KEY, VALUE, ARG_VALUE>& copy)
    {


        if (Count())
        {
            RemoveAll();
        }
        POS pos = copy.GetStartAssoc();
        while (pos)
        {
            VALUE value;
            Key key;
            copy.GetNextAssoc(pos, key, value);



            SetAt_Tracked(key, value);
        }

    }
};
