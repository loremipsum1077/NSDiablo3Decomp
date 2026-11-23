// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/Networking/Connection.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.h"
#include "../../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../Networking/XSocket.h"

class XNetAddress;

namespace Console {
namespace GameNetwork {
class Connection : public bdReferencable {  // size 0x10
public:
    virtual ~Connection() {}  // vtable slot 0; line 28. Defined in the class: the original has no out-of-line copy (inferred)
    virtual BOOL IsConnected() const = 0;  // vtable slot 2
    virtual BOOL IsMigrated() const = 0;  // vtable slot 3
    virtual void GetAddress(XNetAddress &) const = 0;  // vtable slot 4
    virtual void Close() = 0;  // vtable slot 5
    virtual uint32 GetMaxSendBufferSize() const = 0;  // vtable slot 6
    virtual BOOL HasDataToSend() const = 0;  // vtable slot 7
    virtual BOOL Send(void *, uint32) = 0;  // vtable slot 8
    virtual BOOL Flush() = 0;  // vtable slot 9
    virtual BOOL Receive(intptr, SocketMessageHandlerProc) = 0;  // vtable slot 10
    virtual void onDisconnect() = 0;  // vtable slot 11
};
static_assert(sizeof(Connection) == 0x10, "Connection");

typedef bdReference<Console::GameNetwork::Connection> ConnectionRef;

}  // namespace GameNetwork
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    25  Console::GameNetwork::Connection::Connection()  [inlined into 2 functions]
