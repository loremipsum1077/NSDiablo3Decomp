// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdLoopbackConnection.h

#pragma once

#include "bdConnection.h"
#include "bdMessage/bdReceivedMessage.h"
#include "../bdCore/bdContainers/bdQueue.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdEndpoint.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

typedef bdReference<bdMessage> bdMessageRef;

class bdLoopbackConnection : public bdConnection {  // size 0x50
public:
    // Inferred friendship: bdConnectionStore::create invokes protected constructor; flushAll invokes updateStatus; friends are absent from DWARF.
    friend class bdConnectionStore;
    BD_CLASS_ALLOCATOR(bdLoopbackConnection);
    virtual ~bdLoopbackConnection();  // vtable slot 0
    virtual bdBool receive(const bdUByte8 *, const bdUInt32);  // vtable slot 2
    virtual bdBool send(const bdMessageRef message, const bdBool);  // vtable slot 3
    virtual bdBool getMessageToDispatch(bdMessageRef & message);  // vtable slot 4
    virtual bdBool getOutOfOrderMessageToDispatch(bdMessageRef &);  // vtable slot 5
protected:
    bdLoopbackConnection(bdCommonAddrRef);
    virtual bdUInt getDataToSend(bdUByte8 *const, const bdUInt32);  // vtable slot 10
    void updateStatus();
    bdQueue<bdReference<bdMessage> > m_messages;  // 0x38
};
static_assert(sizeof(bdLoopbackConnection) == 0x50, "bdLoopbackConnection");


// Functions defined in this file (bodies are not in the DWARF):
//   line    32  void bdLoopbackConnection::operator delete(void * p)  [inlined into 1 functions]
//   line    32  void * bdLoopbackConnection::operator new(size_t nbytes)  [inlined into 1 functions]
