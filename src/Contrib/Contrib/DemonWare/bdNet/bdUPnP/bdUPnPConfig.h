// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdUPnP/bdUPnPConfig.h

#pragma once

#include "../../bdCore/bdSocket/bdInetAddr.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdUPnPConfig {  // size 0xac
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdUPnPRunMode {
        BD_UPNP_DO_PORT_MAPPING = 0,
        BD_UPNP_EXTERNAL_IP_ONLY = 1,
    };
    enum bdUPnPPortSelectionMode {
        BD_UPNP_PORT_SELECTION_SEQUENTIAL = 0,
        BD_UPNP_PORT_SELECTION_EXPANDING_RANDOM = 1,
        BD_UPNP_PORT_SELECTION_RANDOM = 2,
    };
    bdUPnPConfig();
    void reset();
    void sanityCheckConfig();
    bdFloat32 m_discoveryTimeout;  // 0x00
    bdFloat32 m_responseTimeout;  // 0x04
    bdFloat32 m_connectTimeout;  // 0x08
    bdUPnPConfig::bdUPnPRunMode m_runMode;  // 0x0c
    bdUInt m_discoveryRetries;  // 0x10
    bdUInt m_mappingRetries;  // 0x14
    bdUPnPConfig::bdUPnPPortSelectionMode m_portSelectionMode;  // 0x18
    bdUInt32 m_portPoolSize;  // 0x1c
    bdBool m_disabled;  // 0x20
    bdInetAddr m_gatewayAddr;  // 0x24
    bdBool m_onlyUseGateway;  // 0xa8
};
static_assert(sizeof(bdUPnPConfig) == 0xac, "bdUPnPConfig");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdUPnPConfig::bdUPnPConfig(const bdUPnPConfig &)  [inlined into 3 functions]
//   line    16  bdUPnPConfig::~bdUPnPConfig()  [inlined into 9 functions]
