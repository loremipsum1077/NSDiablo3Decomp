// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdConnection.h

#pragma once

#include "bdMessage/bdReceivedMessage.h"
#include "bdMessage/bdMessage.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "../bdCore/bdContainers/bdFastArray.h"
#include "../bdCore/bdReference/bdReferencable.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdEndpoint.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdSocket/bdAddrHandle.h"
#include <stddef.h>
#include <stdint.h>

class bdConnectionListener;

typedef bdReference<bdMessage> bdMessageRef;

class bdConnection : public bdReferencable {  // size 0x38
public:
    // Inferred friendship: bdConnectionStore::flush calls protected getDataToSend (virtual slot10); friends are absent from DWARF.
    friend class bdConnectionStore;
    BD_CLASS_ALLOCATOR(bdConnection);
    enum Status {
        BD_NOT_CONNECTED = 0,
        BD_CONNECTING = 1,
        BD_CONNECTED = 2,
        BD_DISCONNECTING = 3,
        BD_DISCONNECTED = 4,
    };
    virtual ~bdConnection();  // vtable slot 0
    virtual bdBool receive(const bdUByte8 *, const bdUInt) = 0;  // vtable slot 2
    virtual bdBool send(const bdMessageRef, const bdBool) = 0;  // vtable slot 3
    virtual bdBool getMessageToDispatch(bdMessageRef &) = 0;  // vtable slot 4
    virtual bdBool getOutOfOrderMessageToDispatch(bdMessageRef &) = 0;  // vtable slot 5
    bdCommonAddrRef getAddress() const;
    const bdAddrHandleRef & getAddressHandle() const;
    void setAddressHandle(const bdAddrHandleRef & addr);
    virtual bdConnection::Status getStatus() const;  // vtable slot 6
    void setTransmissionRate(const bdUInt);
    bdUInt getTransmissionRate() const;
    void registerListener(bdConnectionListener *const listener);
    void unregisterListener(bdConnectionListener *const listener);
    virtual bdBool connect();  // vtable slot 7
    virtual void disconnect();  // vtable slot 8
    virtual void close();  // vtable slot 9
protected:
    bdConnection();
    bdConnection(bdCommonAddrRef);
    virtual bdUInt getDataToSend(bdUByte8 *const, const bdUInt) = 0;  // vtable slot 10
    bdCommonAddrRef m_addr;  // 0x10
    bdAddrHandleRef m_addrHandle;  // 0x18
    bdFastArray<bdConnectionListener *> m_listeners;  // 0x20
    bdUInt m_maxTransmissionRate;  // 0x30
    bdConnection::Status m_status;  // 0x34
};
static_assert(sizeof(bdConnection) == 0x38, "bdConnection");

