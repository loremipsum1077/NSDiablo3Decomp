// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdNetStartParams.h

#pragma once

#include "../bdConnection/bdConnectionStoreConfig.h"
#include "../bdCore/bdContainers/bdArray.h"
#include "../bdCore/bdSocket/bdAddr.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "../bdCore/bdSocket/bdDefaultSocket.h"
#include "../bdCore/bdSocket/bdPort.h"
#include "../bdCore/bdString/bdString.h"
#include "bdGetHostByName/bdGetHostByNameConfig.h"
#include "bdUPnP/bdUPnPConfig.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdSocket/bdNAT/bdIPDiscoveryConfig.h"
#include "../bdSocket/bdSocketRouterConfig.h"
#include <stddef.h>
#include <stdint.h>

class bdNetStartParams {  // size 0x1a8
public:
    static void operator delete(void *);
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    bdNetStartParams();
    bdBool m_onlineGame;  // 0x00
    bdPort m_gamePort;  // 0x02
    bdDefaultSocket * m_socket;  // 0x08
    bdArray<bdString> m_natTravHosts;  // 0x10
    bdPort m_natTravPort;  // 0x20
    bdArray<bdInetAddr> m_localAddresses;  // 0x28
    bdAddr m_overridePublicAddr;  // 0x38
    bdNATType m_overrideNATType;  // 0xc0
    bdGetHostByNameConfig m_hostNameLookupConfig;  // 0xc4
    bdUPnPConfig m_UPnPConfig;  // 0xc8
    bdBool m_useAnyIP;  // 0x174
    bdUInt m_threadStackSize;  // 0x178
    bdSocketRouterConfig m_socketRouterConfig;  // 0x17c
    bdConnectionStoreConfig m_connectionStoreConfig;  // 0x18c
    bdIPDiscoveryConfig m_IPDiscoveryConfig;  // 0x19c
};
static_assert(sizeof(bdNetStartParams) == 0x1a8, "bdNetStartParams");


// Functions defined in this file (bodies are not in the DWARF):
//   line    42  bdNetStartParams::bdNetStartParams(const bdNetStartParams &)  [0x7100b4a310]
//   line    42  bdNetStartParams & bdNetStartParams::operator=(const bdNetStartParams &)  [inlined into 1 functions]
//   line    42  bdNetStartParams::~bdNetStartParams()  [inlined into 3 functions]
