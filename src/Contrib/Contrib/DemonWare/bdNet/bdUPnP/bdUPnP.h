// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdUPnP/bdUPnP.h

#pragma once

#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdSocket/bdPort.h"
#include "../../bdCore/bdSocket/bdSocket.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "bdUPnPConfig.h"
#include "bdUPnPDevice.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;

class bdUPnP {  // size 0x4b68
public:
    static void operator delete(void *);
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    enum bdUPnPState {
        BD_UPNP_UNINITIALISED = 0,
        BD_UPNP_INITIALISED = 1,
        BD_UPNP_DISCOVERING_DEVICES = 2,
        BD_UPNP_DISCOVERED_DEVICE = 3,
        BD_UPNP_CONFIGURING_DEVICE = 4,
        BD_UPNP_SHUTTING_DOWN = 5,
        BD_UPNP_FINISHED = 6,
        BD_UPNP_FAILED = 7,
    };
    bdUPnP();
    ~bdUPnP();
    bdBool init(bdPort port, bdArray<bdInetAddr> * localAddrs, bdUPnPConfig config);
    void pump();
    bdBool isRunning();
    void startShutdown(const bdUPnPDevice::bdUPnPDeviceShutdownType shutdownType);
    void cleanup();
    bdUPnP::bdUPnPState getState() const;
    bdUPnPDevice::bdUPnPPortStatus getPortStatus() const;
    bdBool getExternalAddr(bdAddr & externalAddr) const;
protected:
    bdBool setupDiscoverySocket();
    bdBool startDiscovery();
    bdBool checkForDiscoveredDevices(bdUPnPDevice & discoveredDevice);
    bdBool handleDiscoveryResponse(bdUPnPDevice & discoveredDevice);
    bdBool extractDeviceInfo(bdAddr & deviceAddr, bdNChar8 ** fetchLoc, bdUInt & fetchLen);
    bdUPnP::bdUPnPState m_state;  // 0x00
    bdPort m_gamePort;  // 0x04
    bdArray<bdInetAddr> * m_localAddrs;  // 0x08
    bdUPnPConfig m_config;  // 0x10
    bdSocket m_discoverySocket;  // 0xc0
    bdNChar8 m_readBuffer[1025];  // 0xd0
    bdUInt m_bytesReceived;  // 0x4d4
    bdUPnPDevice m_device;  // 0x4d8
    bdUPnPDevice m_cachedDevice;  // 0x2818
    bdStopwatch m_discoveryTimer;  // 0x4b58
    bdUInt m_numDiscoveryAttempts;  // 0x4b60
    bdBool m_shutdownRequested;  // 0x4b64
};
static_assert(sizeof(bdUPnP) == 0x4b68, "bdUPnP");

