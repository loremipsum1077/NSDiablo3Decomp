// Original file: Contrib/Contrib/DemonWare/bdConnection/bdLoopbackConnection.cpp
#include "bdMessage/bdMessage.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
class bdAddrHandle;
#include "bdLoopbackConnection.h"
#include "bdConnectionListener.h"

// line 13
bdBool bdLoopbackConnection::receive(const bdUByte8 *, const bdUInt32) { return true; }

// line 19
bdBool bdLoopbackConnection::send(const bdMessageRef message, const bdBool)
{
    if (message->getPayload())
        message->getPayload()->resetReadPosition();
    m_messages.enqueue(message);
    return true;
}

// line 33
bdBool bdLoopbackConnection::getMessageToDispatch(bdMessageRef &message)
{
    bdBool result = false;
    if (!m_messages.isEmpty()) {
        message = m_messages.peek();
        m_messages.dequeue();
        result = true;
    }
    return result;
}

// line 45
bdBool bdLoopbackConnection::getOutOfOrderMessageToDispatch(bdMessageRef &) { return false; }

// line 50
bdUInt bdLoopbackConnection::getDataToSend(bdUByte8 *const, const bdUInt32) { return 0; }

// line 56
void bdLoopbackConnection::updateStatus()
{
    if (m_status == BD_CONNECTING) {
        for (bdUInt32 i = 0; i < m_listeners.getSize(); ++i)
            m_listeners[i]->onConnect(this);
        m_status = BD_CONNECTED;
    } else if (m_status == BD_DISCONNECTING) {
        for (bdUInt32 i = 0; i < m_listeners.getSize(); ++i)
            m_listeners[i]->onDisconnect(this);
        m_status = BD_DISCONNECTED;
    }
}

// line 92
bdLoopbackConnection::bdLoopbackConnection(bdCommonAddrRef addr) : bdConnection(addr) {}

// line 97
bdLoopbackConnection::~bdLoopbackConnection() {}
