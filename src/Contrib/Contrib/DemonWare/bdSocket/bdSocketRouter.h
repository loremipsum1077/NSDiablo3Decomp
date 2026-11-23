// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSocketRouter.h
// Completed from the DWARF (tools/gen_headers.py reference).

#pragma once

#include "../bdCore/bdContainers/bdArray.h"
#include "../bdCore/bdContainers/bdFastArray.h"
#include "../bdCore/bdContainers/bdHashMap.h"
#include "../bdCore/bdCrypto/bdECCKey.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "../bdCore/bdSocket/bdDefaultSocket.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdAddrHandle.h"
#include "bdAddressMap.h"
#include "bdDTLS/bdDTLSAssociation.h"
#include "bdNAT/bdNATTravClient.h"
#include "bdNAT/bdNATTravListener.h"
#include "bdQoS/bdQoSProbe.h"
#include "bdSecurityKeyMap.h"
#include "bdSecurityKeyMapListener.h"
#include "bdServiceBandwidthArbitrator.h"
#include "bdSocketRouterConfig.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;
class bdPacketBuffer;
class bdPacketInterceptor;
class bdSecurityID;

class bdSocketRouter : public bdNATTravListener, public bdSecurityKeyMapListener {  // size 0x318
public:
    BD_CLASS_ALLOCATOR(bdSocketRouter);
    enum bdSocketRouterStatus {
        BD_SOCKET_ROUTER_UNINITIALIZED = 0,
        BD_SOCKET_ROUTER_INITIALIZED = 1,
        BD_SOCKET_ROUTER_ERROR = 2,
    };
    bdSocketRouter();
    virtual ~bdSocketRouter();  // vtable slot 0
    virtual bdBool init(bdDefaultSocket * socket, const bdCommonAddrRef localCommonAddr, const bdSocketRouterConfig & config);  // vtable slot 4
    virtual void pump();  // vtable slot 5
    virtual bdBool quit();  // vtable slot 6
    virtual bdBool connect(bdAddrHandleRef & addrHandle);  // vtable slot 7
    virtual bdBool disconnect(bdAddrHandleRef & addrHandle);  // vtable slot 8
    virtual bdDTLSAssociationStatus getStatus(const bdAddrHandleRef addrHandle);  // vtable slot 9
    virtual bdSocketRouter::bdSocketRouterStatus getStatus();  // vtable slot 10
    virtual bdInt sendTo(const bdAddrHandleRef addrHandle, void * data, const bdUInt length);  // vtable slot 11
    virtual bdInt receiveFrom(bdAddrHandleRef & addrHandle, void * data, const bdUInt size);  // vtable slot 12
    void registerInterceptor(bdPacketInterceptor *const interceptor);
    void unregisterInterceptor(bdPacketInterceptor *const interceptor);
    virtual void onNATAddrDiscovery(bdCommonAddrRef remote, const bdAddr & realAddr);  // vtable slot 2
    virtual void onNATAddrDiscoveryFailed(bdCommonAddrRef remote);  // vtable slot 3
    virtual void onSecurityKeyRemove(const bdSecurityID & id);  // vtable slot 13
    bdBool setupIntroducers(const bdArray<bdAddr> & introducers);
    void setSendtoBuffer(bdPacketBuffer *);
    bdPacketBuffer * getSendtoBuffer();
    bdBool cacheSendTo(const bdAddrHandleRef addrHandle, void * data, bdUInt length);
    bdNATTravClient * getNATTravClient();
    bdQoSProbe * getQoSProber();
    void getConfig(bdSocketRouterConfig &) const;
    bdAddressMap * getAddressMap();
    bdSecurityKeyMap * getKeyMap();
    bdBool updateLocalCommonAddr(const bdCommonAddrRef newLocalCommonAddr);
    bdCommonAddrRef getLocalCommonAddr() const;
    virtual bdBool isRelayEnabled() const;  // vtable slot 14
protected:
    bdInt processPacket(bdAddrHandleRef & addrHandle, bdAddr & realAddr, void * data, const bdUInt size, const bdUInt bytesTransferred, bdBool & receiving);
    void processError(bdAddr & realAddr, bdInt val, bdUInt & resetCount, bdBool & receiving);
    bdDefaultSocket * m_socket;  // 0x10
    bdFastArray<bdPacketInterceptor *> m_interceptors;  // 0x18
    bdSocketRouter::bdSocketRouterStatus m_status;  // 0x28
    bdSocketRouterConfig m_config;  // 0x2c
    bdCommonAddrRef m_localCommonAddr;  // 0x40
    bdAddressMap m_addrMap;  // 0x48
    bdSecurityKeyMap m_keyStore;  // 0x58
    bdNATTravClient m_natTrav;  // 0x80
    bdECCKey m_ECCKey;  // 0x180
    bdServiceBandwidthArbitrator m_qosBandwidth;  // 0x1b0
    bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass> m_dtls;  // 0x1b8
    bdPacketBuffer * m_sendtoBuffer;  // 0x1d8
    bdQoSProbe m_qosProber;  // 0x1e0
    bdHashMap<bdEndpoint, bdReference<bdAddrHandle>, bdEndpointHashingClass> m_endpointToAddrMap;  // 0x2f8
private:
    bdBool tryDecryptPacket(void * data, const bdUInt & size, const bdSecurityID & secID, bdAddr & realAddr, bdAddrHandleRef & addrHandle, bdInt & val, bdBool checkSecID);
    bdBool tryToFindConnection(void * data, const bdUInt & size, const bdSecurityID & secID, bdAddr & realAddr, bdAddrHandleRef & addrHandle, bdInt & val, bdBool checkSecID);
};
static_assert(sizeof(bdSocketRouter) == 0x318, "bdSocketRouter");


// Functions defined in this file (bodies are not in the DWARF):
//   line    78  void bdSocketRouter::operator delete(void * p)  [inlined into 1 functions]
//   line    78  void * bdSocketRouter::operator new(size_t nbytes)  [inlined into 1 functions]
