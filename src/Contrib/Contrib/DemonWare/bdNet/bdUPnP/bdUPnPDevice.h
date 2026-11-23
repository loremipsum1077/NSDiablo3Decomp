// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdNet/bdUPnP/bdUPnPDevice.h

#pragma once

#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdInetAddr.h"
#include "../../bdCore/bdSocket/bdPort.h"
#include "../../bdCore/bdSocket/bdStreamSocket.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "bdUPnPConfig.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdUPnPDevice {  // size 0x2340
public:
    static void operator delete(void *);
    static void operator delete(void *, void *);
    static void operator delete[](void *);
    static void operator delete[](void *, void *);
    static void * operator new(size_t);
    static void * operator new(size_t, void *);
    static void * operator new[](size_t);
    static void * operator new[](size_t, void *);
    enum bdUPnPDeviceState {
        BD_UPNP_DEVICE_UNINITIALISED = 0,
        BD_UPNP_DEVICE_INITIALISED = 1,
        BD_UPNP_DEVICE_SENDING_DESCRIPTION_REQ = 2,
        BD_UPNP_DEVICE_GETTING_DESCRIPTION = 3,
        BD_UPNP_DEVICE_GOT_DESCRIPTION = 4,
        BD_UPNP_DEVICE_SENDING_IP_REQ = 5,
        BD_UPNP_DEVICE_DISCOVERING_IP = 6,
        BD_UPNP_DEVICE_DISCOVERED_IP = 7,
        BD_UPNP_DEVICE_SENDING_MAPPING_REQ = 8,
        BD_UPNP_DEVICE_MAPPING_PORT = 9,
        BD_UPNP_DEVICE_MAPPED_PORT = 10,
        BD_UPNP_DEVICE_SENDING_UNMAPPING_REQ = 11,
        BD_UPNP_DEVICE_UNMAPPING_PORT = 12,
        BD_UPNP_DEVICE_UNMAPPED_PORT = 13,
        BD_UPNP_DEVICE_SHUTTING_DOWN = 14,
        BD_UPNP_DEVICE_FINISHED = 15,
        BD_UPNP_DEVICE_FAILED = 16,
    };
    enum bdUPnPDeviceShutdownType {
        BD_UPNP_DEVICE_SHUTDOWN_ELEGANT = 0,
        BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE = 1,
    };
    enum bdUPnPPortStatus {
        BD_UPNP_PORT_STATUS_UNKNOWN = 0,
        BD_UPNP_PORT_COLLISION_FOUND = 1,
        BD_UPNP_PORT_NOT_MAPPED = 2,
        BD_UPNP_PORT_MAPPED = 3,
    };
    bdUPnPDevice();
    ~bdUPnPDevice();
    bdBool init(const bdUPnPConfig config, bdArray<bdInetAddr> * localAddrs, const bdAddr deviceAddress, const bdNChar8 *const fetchLocation, const bdUInt fetchLocationLength, const bdPort port);
    void pump();
    void startShutdown(const bdUPnPDevice::bdUPnPDeviceShutdownType shutdownType);
    void cleanup();
    bdAddr getDeviceAddr() const;
    bdAddr getExternalAddr() const;
    bdUPnPDevice::bdUPnPPortStatus getPortStatus() const;
    bdUPnPDevice::bdUPnPDeviceState getDeviceState() const;
protected:
    bdBool connectToDevice();
    bdBool setLocalConnectedAddr();
    void setupReceive();
    bdBool pumpSendRequest(bdBool & isComplete);
    bdBool pumpReceive();
    bdBool confirmHttpSuccess();
    bdBool extractUPnPError(bdInt32 & errorCode);
    bdBool checkForFinishedSoapResponse();
    void genUPnPCommand(const bdNChar8 *const command, const bdNChar8 *const arguments);
    bdBool extractTag(const bdNChar8 * tag, bdNChar8 * buffer, bdNChar8 ** value, bdUInt & valueLen);
    void prepareGetDescriptionRequest();
    bdBool parseDescriptionResponse();
    bdBool extractURLBase(bdNChar8 ** baseLoc, bdUInt & baseLen);
    bdBool extractServiceType(bdBool & serviceType);
    bdBool extractControlURL(bdNChar8 ** controlLoc, bdUInt & controlLen);
    void prepareGetExternalIPRequest();
    bdBool parseGetExternalIPResponse();
    bdBool extractExternalAddress(bdInetAddr & externalAddress);
    void prepareGetMappingsRequest();
    bdBool parseGetMappingsResponse(bdBool &, bdBool &);
    bdBool extractMappingOwner(bdInetAddr &, bdNChar8 *, bdUInt);
    void prepareCreatePortMappingRequest();
    void preparePortUnMappingRequest();
    void selectNewExternalPort();
    void sendCreatePortMappingRequest();
    bdUPnPDevice::bdUPnPDeviceState m_state;  // 0x00
    bdBool m_shutdownRequested;  // 0x04
    bdUPnPDevice::bdUPnPDeviceShutdownType m_shutdownType;  // 0x08
    bdUPnPDevice::bdUPnPPortStatus m_portStatus;  // 0x0c
    bdAddr m_deviceAddr;  // 0x10
    bdArray<bdInetAddr> * m_localAddrs;  // 0x98
    bdInetAddr m_localConnectedAddr;  // 0xa0
    bdStreamSocket m_streamSocket;  // 0x124
    bdBool m_isIp;  // 0x128
    bdUPnPConfig m_config;  // 0x12c
    bdPort m_gamePort;  // 0x1d8
    bdPort m_externalPort;  // 0x1da
    bdInetAddr m_externalDeviceAddr;  // 0x1dc
    bdNChar8 m_requestBuffer[2048];  // 0x260
    bdUInt m_requestSize;  // 0xa60
    bdNChar8 m_readBuffer[6144];  // 0xa64
    bdUInt m_bytesReceived;  // 0x2264
    bdNChar8 m_fetchLocation[64];  // 0x2268
    bdNChar8 m_controlURL[128];  // 0x22a8
    bdStopwatch m_connectTimer;  // 0x2328
    bdStopwatch m_responseTimer;  // 0x2330
    bdUInt32 m_mappingAttempts;  // 0x2338
};
static_assert(sizeof(bdUPnPDevice) == 0x2340, "bdUPnPDevice");

