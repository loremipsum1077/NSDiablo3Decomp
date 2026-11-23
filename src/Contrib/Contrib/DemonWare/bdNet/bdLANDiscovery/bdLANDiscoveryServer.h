// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryServer.h

#pragma once

#include "../../bdCore/bdContainers/bdFastArray.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdSocket/bdPort.h"
#include "bdGameInfo.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdInetAddr;
class bdLANDiscoveryListener;

class bdLANDiscoveryServer {  // size 0x30
public:
    enum bdStatus {
        BD_IDLE = 0,
        BD_PENDING = 1,
        BD_ERROR = 2,
    };
    bdLANDiscoveryServer();
    ~bdLANDiscoveryServer();
    bdBool start(const bdGameInfoRef gameInfo, const bdInetAddr & localAddr, const bdPort discoveryPort);
    void update();
    void stop();
    void registerListener(bdLANDiscoveryListener *);
    void unregisterListener(bdLANDiscoveryListener *);
    bdLANDiscoveryServer::bdStatus getStatus() const;
    bdGameInfoRef getGameInfo();
protected:
    bdGameInfoRef m_gameInfo;  // 0x00
    bdDefaultSocket m_socket;  // 0x08
    bdFastArray<bdLANDiscoveryListener *> m_listeners;  // 0x18
    bdLANDiscoveryServer::bdStatus m_status;  // 0x28
};
static_assert(sizeof(bdLANDiscoveryServer) == 0x30, "bdLANDiscoveryServer");
