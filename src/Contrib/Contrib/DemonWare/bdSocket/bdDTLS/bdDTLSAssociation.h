// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdDTLSAssociation.h

#pragma once

#include "../../bdCore/bdContainers/bdNonce.h"
#include "../../bdCore/bdContainers/bdSequenceNumber.h"
#include "../../bdCore/bdContainers/bdSequenceNumberStore.h"
#include "../../bdCore/bdCrypto/bdCryptoDynamic.h"
#include "../../bdCore/bdCrypto/bdCypher.h"
#include "../../bdCore/bdCrypto/bdCypherSuites.h"
#include "../../bdCore/bdCrypto/bdHash.h"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdAddrHandle.h"
#include "bdDTLSRandom.h"
#include "bdPacket/bdDTLSError.h"
#include "bdPacket/bdDTLSInitAck.h"
#include <stddef.h>
#include <stdint.h>

class bdAddressMap;
class bdDTLSCookieEcho;
class bdDTLSData;
class bdDTLSInit;
class bdECCKey;
class bdSecurityKeyMap;

enum bdDTLSAssociationStatus {
    BD_SOCKET_IDLE = 0,
    BD_SOCKET_PENDING = 1,
    BD_SOCKET_CONNECTED = 2,
    BD_SOCKET_LOST = 3,
};

class bdDTLSAssociation {  // size 0x1040
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdDTLSAssociation);
    bdDTLSAssociation(bdDefaultSocket * socket, bdSecurityKeyMap * keyMap, bdECCKey * ECCkey, const bdAddr & addr, bdAddrHandleRef addrHandle, bdCommonAddrRef localCommonAddr, bdAddressMap * addrMap, bdFloat32 receiveTimeout, const bdCypherSuite cypherSuite);
    virtual ~bdDTLSAssociation();  // vtable slot 0
    void connect();
    bdDTLSAssociationStatus getStatus();
    virtual bdInt sendTo(const bdAddr & addr, const void * data, const bdUInt length);  // vtable slot 2
    virtual bdInt receiveFrom(bdAddr & addr, const void * data, const bdUInt size, bdAddrHandleRef & addrHandle, void * buffer, const bdUInt bufferSize);  // vtable slot 3
    void pump();
    bdAddrHandleRef getAddrHandle();
    const bdSecurityID * getLocalSecurityId();
    bdUInt prepareData(const void *, const bdUInt, void *, const bdUInt);
    enum bdDTLSStatus {
        BD_DTLS_CLOSED = 0,
        BD_DTLS_COOKIE_WAIT = 1,
        BD_DTLS_COOKIE_ECHOED = 2,
        BD_DTLS_ESTABLISHED = 3,
    };
protected:
    bdInt handleInit(const bdAddr & addr, const void * data, const bdUInt size);
    bdInt handleInitAck(const bdAddr & addr, const void * data, const bdUInt size);
    bdInt handleCookieEcho(const bdAddr & addr, const void * data, const bdUInt size);
    bdInt handleCookieAck(const void * data, const bdUInt size);
    bdInt handleError(const bdAddr & addr, const void * data, const bdUInt size);
    bdInt handleData(bdAddr & addr, const bdUByte8 * data, const bdUInt size, bdAddrHandleRef & addrHandle, const bdUInt16 vtag, bdUByte8 * buffer, const bdUInt bufferSize);
    void sendInit();
    void sendInitAck(const bdAddr & addr, const bdDTLSInit & init);
    void sendCookieEcho(const bdAddr & addr);
    void sendCookieAck(const bdAddr & addr, const bdDTLSCookieEcho &);
    void sendError(const bdAddr & addr, const bdSecurityID & secID, const bdDTLSError::bdDTLSErrorType & type);
    bdInt sendData(const bdAddr & addr, const void * data, const bdUInt length);
    void reset();
    bdBool calculateSharedKey(const bdUByte8 *const pubKey, const bdUInt keylen, const bdSecurityID & secID);
    bdBool verify(const bdDTLSData & dataPacket);
    bdBool checkCookieValidity(const bdAddr & peerAddr, const bdDTLSCookieEcho & cookiePacket) const;
    bdUInt16 makeTag() const;
    bdDefaultSocket * m_socket;  // 0x08
    bdSecurityKeyMap * m_keyMap;  // 0x10
    bdECCKey * m_ECCKey;  // 0x18
    bdCypherSuite m_cypherSuite;  // 0x20
    bdCypherAlgorithms m_cypherAlgorithm;  // 0x24
    bdDynamicCypher m_sendingCypher;  // 0x28
    bdDynamicCypher m_receivingCypher;  // 0x6e8
    bdHashAlgorithms m_hashAlgorithm;  // 0xda8
    bdDynamicHash m_hash;  // 0xdb0
    bdUByte8 m_sendingIVMaterial[32];  // 0xdd0
    bdUByte8 m_receivingIVMaterial[32];  // 0xdf0
    bdAddr m_addr;  // 0xe10
    bdAddrHandleRef m_addrHandle;  // 0xe98
    bdDTLSAssociation::bdDTLSStatus m_state;  // 0xea0
    bdStopwatch m_lastReceived;  // 0xea8
    bdStopwatch m_initTimer;  // 0xeb0
    bdUByte8 m_initResends;  // 0xeb8
    bdStopwatch m_cookieTimer;  // 0xec0
    bdUByte8 m_cookieResends;  // 0xec8
    bdUInt16 m_localTag;  // 0xeca
    bdUInt16 m_peerTag;  // 0xecc
    bdDTLSRandom m_localRandom;  // 0xece
    bdDTLSRandom m_peerRandom;  // 0xeef
    bdSequenceNumber m_seqNum;  // 0xf10
    bdSequenceNumberStore m_incomingSeqNums;  // 0xf14
    bdDTLSInitAck m_initAck;  // 0xf20
    static bdNonce<16> m_cookieKey;
    bdSecurityID m_localId;  // 0x1020
    bdCommonAddrRef m_localCommonAddr;  // 0x1028
    bdAddressMap * m_addrMap;  // 0x1030
    bdFloat32 m_receiveTimeout;  // 0x1038
    bdDTLSAssociation(const bdDTLSAssociation &);
    bdDTLSAssociation & operator=(const bdDTLSAssociation &);
};
static_assert(sizeof(bdDTLSAssociation) == 0x1040, "bdDTLSAssociation");
