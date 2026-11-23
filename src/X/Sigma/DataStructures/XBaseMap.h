#pragma once

#include "../Math/MathGeneral.h"
#include "DataStructuresExternal.h"
#include "XChainedFixedBlockAllocator.h"
#include "XGrowableArray.h"
#include "XMapAssoc.h"
#include "XTemplate.h"

















template <typename ARG_KEY>
inline uint32 XHashKey(ARG_KEY key) {
    // The spelling of this body is inferred: the binary hashes the low four bytes of the key with
    // straight-line statements on lines 35-42; only the local `hash` is in the DWARF.
    uint32 dwKey = (uint32)(size_t)key;
    uint32 hash = 0x811C9DC5;



    hash ^= dwKey & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 8) & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 16) & 0xFF;
    hash *= 0x01000193;
    hash ^= (dwKey >> 24) & 0xFF;
    hash *= 0x01000193;

    return hash;
}












































template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR,
          typename HASH_ARRAY, int HASH_SIZE>
class XBaseMap {
    typedef XMapAssoc<Key, VALUE> XMapAssocSpecific;

public:
    inline XBaseMap();
    inline ~XBaseMap();

    inline void Initialize(int32 nBlockCount, BOOL bUseSystemMemory, XMemoryAllocatorInterface* ptMemoryInterface);
    inline void Dispose();

    inline void SetUseSystemAllocator(BOOL bValue);
    inline void SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface);

    inline BOOL Lookup(ARG_KEY key, VALUE& rValue) const;
    inline VALUE* LookupPtr(ARG_KEY key) const;

    inline void SetAt_Tracked(ARG_KEY key, ARG_VALUE value);

    inline void RemoveAt(ARG_KEY key);
    inline void RemoveAll();

    inline POS GetStartAssoc() const;
    inline void GetNextAssoc(POS& pos, Key& key, VALUE& value) const;
    inline VALUE* GetNextAssocPtr(POS& pos, Key& key) const;
    inline void GetNextAssocKey(POS& pos, Key& key) const;

    inline int32 Count() const;
    inline BOOL IsEmpty() const;

    inline void SetHashTableSize_Tracked(const int32 nHashTableSize);

    inline void _SetHashMask();
    inline void _InitHashTable_Tracked(const int32 nSize);
    inline uint32 _GetHash(ARG_KEY key) const;
    inline int32 _GetHashTableSize() const;
    inline POS _GetNextBucket(uint32 dwHash) const;
    inline void _SetAssocRefAt(ARG_KEY key, XMapAssocSpecific* pNewAssoc);
    inline XMapAssocSpecific* _GetAssocAt(ARG_KEY key, uint32& dwHash) const;
    inline XMapAssocSpecific* _AddAssoc_Tracked(ARG_KEY key, ARG_VALUE value);
    inline void _FreeAssoc(XMapAssocSpecific* pAssoc);

    int32 m_nHashMask;
    int32 m_nCount;
    ASSOC_ALLOCATOR* m_arAssoc;
    HASH_ARRAY m_arHashTable;
};

#define XBASEMAP_TEMPLATE                                                                                       \
    template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE, typename ASSOC_ALLOCATOR,     \
              typename HASH_ARRAY, int HASH_SIZE>
#define XBASEMAP XBaseMap<Key, ARG_KEY, VALUE, ARG_VALUE, ASSOC_ALLOCATOR, HASH_ARRAY, HASH_SIZE>






































































































XBASEMAP_TEMPLATE
XBASEMAP::XBaseMap() {
    m_nHashMask = 0;
    m_nCount = 0;
    m_arAssoc = NULL;
}







XBASEMAP_TEMPLATE
XBASEMAP::~XBaseMap() {
}






XBASEMAP_TEMPLATE
uint32 XBASEMAP::_GetHash(ARG_KEY key) const {
    uint32 dwRetval;



    dwRetval = XHashKey<ARG_KEY>(key) & ((HASH_SIZE != 0) ? (HASH_SIZE - 1) : m_nHashMask);

    return dwRetval;
}


XBASEMAP_TEMPLATE
int32 XBASEMAP::_GetHashTableSize() const {
    return m_arHashTable.GetArraySize();
}




XBASEMAP_TEMPLATE
void XBASEMAP::_SetHashMask() {
    int32 nHashTableSize = _GetHashTableSize();
    m_nHashMask = nHashTableSize - 1;
}






XBASEMAP_TEMPLATE
void XBASEMAP::SetHashTableSize_Tracked(const int32 nHashTableSize) {












    int32 nAdjustedHashTableSize = Math::Max<int32>(Math::Ceiling2Power(nHashTableSize), 2);




    m_arHashTable.SetArraySize_Tracked(nAdjustedHashTableSize);

    for (int32 i = 0; i < nAdjustedHashTableSize; i++) {
        m_arHashTable[i] = NULL;
    }

    _SetHashMask();
}









































































XBASEMAP_TEMPLATE
void XBASEMAP::Initialize(int32 nBlockCount, BOOL bUseSystemMemory, XMemoryAllocatorInterface* ptMemoryInterface) {



    m_arAssoc->Initialize();
    m_arHashTable.Initialize();

    SetUseSystemAllocator(bUseSystemMemory);
    SetMemoryInterface(ptMemoryInterface);




    _InitHashTable_Tracked(nBlockCount);
}




XBASEMAP_TEMPLATE
void XBASEMAP::Dispose() {





    RemoveAll();
    m_arAssoc->Dispose();
    m_arHashTable.Dispose();

    m_nHashMask = 0;
    m_nCount = 0;
}


XBASEMAP_TEMPLATE
void XBASEMAP::SetUseSystemAllocator(BOOL bValue) {

    m_arAssoc->SetUseSystemAllocator(bValue);
}




XBASEMAP_TEMPLATE
void XBASEMAP::SetMemoryInterface(XMemoryAllocatorInterface* ptMemoryInterface) {

    m_arAssoc->SetMemoryInterface(ptMemoryInterface);
    m_arHashTable.SetMemoryInterface(ptMemoryInterface);
}



XBASEMAP_TEMPLATE
void XBASEMAP::_InitHashTable_Tracked(const int32 nSize) {
    int32 nGetHashTableSize = _GetHashTableSize();

    if (nGetHashTableSize == 0) {
        // size 0: SetBlockCount runs even for nSize 0 (0x7100972770: 0x972834 store reached via cbz 0x9727e8)
        // size==nSize: skipped (FastAttribute ElementNew 0x71005f1540: pregrown 256, NULL pool, no pool store)
        // so neither != nSize nor unconditional fits.










        m_arHashTable.SetArraySize_Tracked(nSize);
        m_arAssoc->SetBlockCount_Tracked(nSize);
    }





    for (int32 i = 0; i < nSize; i++) {

        m_arHashTable[i] = NULL;
    }

    m_nHashMask = nSize - 1;
    m_nCount = 0;
}



XBASEMAP_TEMPLATE
POS XBASEMAP::GetStartAssoc() const {
    if (m_nCount == 0) {
        return NULL;
    }





    return _GetNextBucket(0);
}



XBASEMAP_TEMPLATE
POS XBASEMAP::_GetNextBucket(uint32 dwHash) const {

    uint32 dwHashTableSize = _GetHashTableSize();

    for (; dwHash < dwHashTableSize; dwHash++) {
        if (m_arHashTable[dwHash] != NULL) {
            return (POS)m_arHashTable[dwHash];
        }
    }

    return NULL;
}
































XBASEMAP_TEMPLATE
void XBASEMAP::GetNextAssoc(POS& pos, Key& key, VALUE& value) const {
    XMapAssocSpecific* pAssoc = (XMapAssocSpecific*)pos;

    key = pAssoc->m_Key;
    value = pAssoc->m_Value;

    if (pAssoc->m_pNext != NULL) {
        pos = (POS)pAssoc->m_pNext;
    } else {







        uint32 dwHash = _GetHash(key);
        pos = _GetNextBucket(dwHash + 1);
    }
}


XBASEMAP_TEMPLATE
VALUE* XBASEMAP::GetNextAssocPtr(POS& pos, Key& key) const {
    XMapAssocSpecific* pAssoc = (XMapAssocSpecific*)pos;

    key = pAssoc->m_Key;

    if (pAssoc->m_pNext != NULL) {
        pos = (POS)pAssoc->m_pNext;
    } else {
        uint32 dwHash = _GetHash(key);
        pos = _GetNextBucket(dwHash + 1);
    }

    return &pAssoc->m_Value;
}




















XBASEMAP_TEMPLATE
void XBASEMAP::GetNextAssocKey(POS& pos, Key& key) const {
    XMapAssocSpecific* pAssoc = (XMapAssocSpecific*)pos;

    key = pAssoc->m_Key;

    if (pAssoc->m_pNext != NULL) {
        pos = (POS)pAssoc->m_pNext;
    } else {
        uint32 dwHash = _GetHash(key);
        pos = _GetNextBucket(dwHash + 1);
    }
}








XBASEMAP_TEMPLATE
int32 XBASEMAP::Count() const {
    return m_nCount;
}




XBASEMAP_TEMPLATE
BOOL XBASEMAP::IsEmpty() const {
    return m_nCount == 0;
}





XBASEMAP_TEMPLATE
BOOL XBASEMAP::Lookup(ARG_KEY key, VALUE& rValue) const {
    uint32 dwHash;
    XMapAssocSpecific* pAssoc;




    pAssoc = _GetAssocAt(key, dwHash);
    if (pAssoc == NULL) {
        return FALSE;
    }

    rValue = pAssoc->m_Value;
    return TRUE;
}





XBASEMAP_TEMPLATE
VALUE* XBASEMAP::LookupPtr(ARG_KEY key) const {
    uint32 dwHash;
    XMapAssocSpecific* pAssoc;

    pAssoc = _GetAssocAt(key, dwHash);
    if (pAssoc == NULL) {
        return NULL;
    }

    return &pAssoc->m_Value;
}





XBASEMAP_TEMPLATE
void XBASEMAP::SetAt_Tracked(ARG_KEY key, ARG_VALUE value) {
    uint32 dwHash;
    XMapAssocSpecific* pAssoc;









    pAssoc = _GetAssocAt(key, dwHash);
    if (pAssoc == NULL) {








        pAssoc = _AddAssoc_Tracked(key, value);


















        pAssoc->m_pNext = m_arHashTable[dwHash];
        m_arHashTable[dwHash] = pAssoc;
    } else {


        pAssoc->m_Value = value;
    }
}



XBASEMAP_TEMPLATE
void XBASEMAP::RemoveAt(ARG_KEY key) {
    uint32 dwHash;
    XMapAssocSpecific* pAssoc;

    pAssoc = _GetAssocAt(key, dwHash);

    _SetAssocRefAt(key, pAssoc->m_pNext);
    _FreeAssoc(pAssoc);
}


XBASEMAP_TEMPLATE
void XBASEMAP::RemoveAll() {
    POS pos;

    pos = GetStartAssoc();
    while (pos != NULL) {
        Key key;
        VALUE value;

        GetNextAssoc(pos, key, value);
        RemoveAt(key);
    }
}


XBASEMAP_TEMPLATE
void XBASEMAP::_FreeAssoc(XMapAssocSpecific* pAssoc) {

    m_arAssoc->Free(pAssoc);
    m_nCount--;
}



XBASEMAP_TEMPLATE
void XBASEMAP::_SetAssocRefAt(ARG_KEY key, XMapAssocSpecific* pNewAssoc) {
    uint32 dwHash = _GetHash(key);
    XMapAssocSpecific* pPrevAssoc = NULL;



    for (XMapAssocSpecific* pAssoc = m_arHashTable[dwHash]; pAssoc != NULL; pAssoc = pAssoc->m_pNext) {
        if (pAssoc->m_Key == key) {
            if (pPrevAssoc != NULL) {
                pPrevAssoc->m_pNext = pNewAssoc;
            } else {
                m_arHashTable[dwHash] = pNewAssoc;
            }
            return;
        }

        pPrevAssoc = pAssoc;
    }






    XASSERT(FALSE);
}





XBASEMAP_TEMPLATE
typename XBASEMAP::XMapAssocSpecific* XBASEMAP::_GetAssocAt(ARG_KEY key, uint32& dwHash) const {
    XMapAssocSpecific** pAssocBase;
    XMapAssocSpecific* pAssoc;

    dwHash = _GetHash(key);








    pAssocBase = (XMapAssocSpecific**)m_arHashTable.GetPointer();

    for (pAssoc = pAssocBase[dwHash]; pAssoc != NULL; pAssoc = pAssoc->m_pNext) {
        if (pAssoc->m_Key == key) {
            return pAssoc;
        }
    }

    return NULL;
}











XBASEMAP_TEMPLATE
typename XBASEMAP::XMapAssocSpecific* XBASEMAP::_AddAssoc_Tracked(ARG_KEY key, ARG_VALUE value) {
    XMapAssocSpecific* pAssoc;










    pAssoc = m_arAssoc->Alloc_Tracked();

    pAssoc->m_Key = key;
    pAssoc->m_Value = value;

    m_nCount++;

    return pAssoc;
}

#undef XBASEMAP_TEMPLATE
#undef XBASEMAP
