#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "XGrowableArray.h"
#include "XGrowableMap.h"












template <typename _KEY, typename _VALUE>
class XGrowableMapTS {

public:
    typedef XGrowableArray<_VALUE, const _VALUE&> MapContentsArray;


    inline XGrowableMapTS() : m_tMap(16, NULL) {}
    inline ~XGrowableMapTS() {}

    inline int32 Count();
    inline void Copy(MapContentsArray& tMapContents);
    inline void RemoveAll();
    inline void RemoveAt(const _KEY& tKey);
    inline BOOL RemoveAt(const _KEY& tKey, _VALUE& tValue);
    inline BOOL Lookup(const _KEY& tKey, _VALUE& tValue);
    inline void SetAt_Tracked(const _KEY& tKey, const _VALUE& tValue);

protected:
    Blizzard::Lock::RMutex m_tLock;
    XGrowableMap<_KEY, const _KEY&, _VALUE, const _VALUE&> m_tMap;
};
































template <typename _KEY, typename _VALUE>
int32 XGrowableMapTS<_KEY, _VALUE>::Count() {


    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    return m_tMap.Count();
}



template <typename _KEY, typename _VALUE>
void XGrowableMapTS<_KEY, _VALUE>::Copy(MapContentsArray& tMapContents) {
    int32 nCounter = 0;
    POS pos;

    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);









    tMapContents.Reserve_Tracked(m_tMap.Count());


    pos = m_tMap.GetStartAssoc();
    while (pos != NULL) {
        _KEY tKey;




        m_tMap.GetNextAssoc(pos, tKey, tMapContents.GetAtGrow_Tracked(nCounter++));
    }
}





template <typename _KEY, typename _VALUE>
void XGrowableMapTS<_KEY, _VALUE>::RemoveAll() {

    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    m_tMap.RemoveAll();
}



template <typename _KEY, typename _VALUE>
void XGrowableMapTS<_KEY, _VALUE>::RemoveAt(const _KEY& tKey) {


    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    m_tMap.RemoveAt(tKey);
}



template <typename _KEY, typename _VALUE>
BOOL XGrowableMapTS<_KEY, _VALUE>::RemoveAt(const _KEY& tKey, _VALUE& tValue) {



    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    BOOL bFound = m_tMap.Lookup(tKey, tValue);
    if (bFound) {

        m_tMap.RemoveAt(tKey);
    }

    return bFound;
}


template <typename _KEY, typename _VALUE>
BOOL XGrowableMapTS<_KEY, _VALUE>::Lookup(const _KEY& tKey, _VALUE& tValue) {



    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    return m_tMap.Lookup(tKey, tValue);
}



template <typename _KEY, typename _VALUE>
void XGrowableMapTS<_KEY, _VALUE>::SetAt_Tracked(const _KEY& tKey, const _VALUE& tValue) {









    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);



    m_tMap.SetAt_Tracked(tKey, tValue);
}
