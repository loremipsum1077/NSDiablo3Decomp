// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryClient.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdContainers/bdFastArray.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdSocket/bdPort.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "bdGameInfo.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdInetAddr;
class bdLANDiscoveryListener;

typedef bdReference<bdGameInfo> bdGameInfoRef;

class bdLANDiscoveryClient {  // size 0x48
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdLANDiscoveryClient);
    enum bdStatus {
        BD_IDLE = 0,
        BD_PENDING = 1,
        BD_ERROR = 2,
    };
    bdLANDiscoveryClient();
    virtual ~bdLANDiscoveryClient();  // vtable slot 0
    bdBool discover(const bdUInt titleID, const bdFloat32 timeout, const bdInetAddr & addr, const bdPort discoveryPort);
    void update();
    bdLANDiscoveryClient::bdStatus getStatus() const;
    void registerListener(bdLANDiscoveryListener * listener);
    void unregisterListener(bdLANDiscoveryListener * listener);
    void fireOnDiscoveryListeners(bdGameInfoRef gameInfo) const;
protected:
    bdFloat32 m_timeout;  // 0x08
    bdStopwatch m_timer;  // 0x10
    bdDefaultSocket m_socket;  // 0x18
    bdUByte8 m_nonce[8];  // 0x28
    bdFastArray<bdLANDiscoveryListener *> m_listeners;  // 0x30
    bdLANDiscoveryClient::bdStatus m_status;  // 0x40
};
static_assert(sizeof(bdLANDiscoveryClient) == 0x48, "bdLANDiscoveryClient");


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  void bdLANDiscoveryClient::operator delete(void * p)  [inlined into 1 functions]
