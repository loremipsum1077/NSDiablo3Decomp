// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdAddressMap.h

#pragma once

#include "../bdCore/bdContainers/bdArray.h"
#include "../bdCore/bdSocket/bdEndpoint.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdAddrHandle.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;
class bdSecurityID;

class bdAddressMap {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdAddressMap);
    bdAddressMap();
    bdBool getAddrHandle(const bdCommonAddrRef ca, const bdSecurityID & id, bdAddrHandleRef & addrHandle);
    bdBool findAddrHandle(const bdCommonAddrRef ca, const bdSecurityID & id, bdAddrHandleRef & addrHandle) const;
    bdBool addrToCommonAddr(const bdAddrHandleRef & addrHandle, bdCommonAddrRef & ca, bdSecurityID & id) const;
    static bdUWord addrToString(const bdAddrHandleRef & addrHandle, bdNChar8 *const str, const bdUWord size);
    bdBool commonAddrToAddr(const bdCommonAddrRef, const bdSecurityID &, bdAddrHandleRef &);
    void clear();
protected:
    // bdSocketRouter::processError calls unregisterRealAddr (the DWARF does not record friends).
    friend class bdSocketRouter;
    bdBool unregisterAddr(bdAddrHandleRef & addrHandle);
    bdBool unregisterRealAddr(const bdAddr realAddr);
    bdArray<bdReference<bdAddrHandle> > m_addrHandles;  // 0x00
};
static_assert(sizeof(bdAddressMap) == 0x10, "bdAddressMap");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  bdAddressMap::~bdAddressMap()  [inlined into 1 functions]
