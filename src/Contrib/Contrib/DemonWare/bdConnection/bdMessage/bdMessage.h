// Adopted from DWARF; allocator inline follows recorded original ownership.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdMessage.h

#pragma once

#include "bdMessageTypes.h"
#include "../../bdCore/bdContainers/bdBitBuffer.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdMessage;
typedef bdReference<bdMessage> bdMessageRef;

class bdMessage : public bdReferencable {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdMessage);
    bdMessage(const bdMessageType, const bdBool);
    bdMessage(const bdMessageType, const bdUByte8 *const, const bdUInt32, const bdBool, const bdUByte8 *const, const bdUInt32);
    virtual ~bdMessage();  // vtable slot 0
    bdMessageType getType() const;
    bdBitBufferRef getPayload();
    bdBool hasPayload() const;
    void createUnencryptedPayload(const bdUInt);
    bdByteBufferRef getUnencryptedPayload() const;
    bdBool hasUnencryptedPayload() const;
protected:
    bdMessageType m_type;  // 0x0c
    bdBitBufferRef m_payload;  // 0x10
    bdBool m_payloadTypeChecked;  // 0x18
    bdByteBufferRef m_unencPayload;  // 0x20
};
static_assert(sizeof(bdMessage) == 0x28, "bdMessage");


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  void bdMessage::operator delete(void * p)  [inlined into 1 functions]
//   line    26  void * bdMessage::operator new(size_t nbytes)  [inlined into 1 functions]
