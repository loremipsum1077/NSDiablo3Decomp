// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdReceivedMessage.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include <stddef.h>
#include <stdint.h>

class bdMessage;
class bdConnection;

typedef bdReference<bdMessage> bdMessageRef;

typedef bdReference<bdConnection> bdConnectionRef;

class bdReceivedMessage {  // size 0x10
public:
    BD_CLASS_ALLOCATOR(bdReceivedMessage);
    bdReceivedMessage(const bdMessageRef, const bdConnectionRef);
    ~bdReceivedMessage();
    bdMessageRef getMessage() const;
    bdConnectionRef getConnection() const;
protected:
    bdMessageRef m_message;  // 0x00
    bdConnectionRef m_connection;  // 0x08
};
static_assert(sizeof(bdReceivedMessage) == 0x10, "bdReceivedMessage");

