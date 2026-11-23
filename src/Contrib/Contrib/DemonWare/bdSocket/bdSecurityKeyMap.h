// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSecurityKeyMap.h

#pragma once

#include "../bdCore/bdSocket/bdSecurityID.h"
#include "../bdCore/bdSocket/bdSecurityKey.h"
#include "../bdCore/bdContainers/bdHashMap.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdSecurityID;
class bdSecurityKey;
class bdSecurityKeyMapListener;

class bdSecurityKeyMap {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdSecurityKeyMap);
    bdSecurityKeyMap();
    static bdBool initKey(bdSecurityID & id, bdSecurityKey & key);
    bdBool registerKey(const bdSecurityID & id, const bdSecurityKey & key);
    bdBool unregisterKey(const bdSecurityID & id);
    bdBool get(const bdSecurityID & id, bdSecurityKey & key);
    bdBool contains(const bdSecurityID & id) const;
    void clear();
    void registerListener(bdSecurityKeyMapListener * listener);
    void unregisterListener();
protected:
    bdSecurityKeyMapListener * m_listener;  // 0x00
    bdHashMap<bdSecurityID, bdSecurityKey, bdHashingClass> m_map;  // 0x08
};
static_assert(sizeof(bdSecurityKeyMap) == 0x28, "bdSecurityKeyMap");


// Functions defined in this file (bodies are not in the DWARF):
//   line    28  bdSecurityKeyMap::~bdSecurityKeyMap()  [inlined into 1 functions]
