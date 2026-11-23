// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/ConsoleGameConnection.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdConnection.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdConnectionListener.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdQueue.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdSocket/bdAddrHandle.h"
#include "../../Sigma/Networking/XNetAddress.h"
#include "../../Common/Console/Networking/Connection.h"
#include "../../Common/Networking/XSocket.h"
#include <stddef.h>
#include <stdint.h>

class XNetAddress;

namespace Console {
namespace Online {

enum {
    CONSOLE_RECEIVE_BUFFER_SIZE = 32768,
    CONSOLE_SEND_BUFFER_SIZE = 32768,
};

class GameConnection : public Console::GameNetwork::Connection {  // size 0x10070
public:
    // line 33
    BD_CLASS_ALLOCATOR(GameConnection);
    enum Type {
        CONNECTION_CLIENT_TO_SERVER = 0,
        CONNECTION_SERVER_TO_CLIENT = 1,
    };
    // line 44
    GameConnection(bdConnectionRef ptConnection, Type eType)
        : m_id(ms_idSequence++)
        , m_ptConnection(ptConnection)
        , m_bConnected(TRUE)
        , m_eType(eType)
        , m_bMigrated(FALSE)
        , m_bDontCloseConnection(FALSE)
        , m_dwReceiveBufferSize(0)
        , m_dwSendBufferSize(0)
        , m_dwMessageQueueUsed(0)
    {
        if (m_ptConnection)
        {
            m_bConnected = m_ptConnection->getStatus() == bdConnection::BD_CONNECTED;
        }
    }
    // line 60
    virtual ~GameConnection()
    {
        ms_dwTotalMessageQueueUsed -= m_dwMessageQueueUsed;
    }
    static void InitializeForApp();
    static void DisposeForApp();
    BOOL ReadPayload(bdBitBufferRef ptPayload);
    // line 71
    virtual BOOL IsConnected() const
    {
        return m_bConnected;
    }
    // line 77
    virtual BOOL IsMigrated() const
    {
        return m_bMigrated;
    }
    // line 83
    virtual void GetAddress(XNetAddress & tAddress) const
    {
        if (m_ptConnection)
        {
            bdAddrHandleRef ptAddrHandle = m_ptConnection->getAddressHandle();
            bdAddr tRealAddr = ptAddrHandle->getRealAddr();
            tAddress.SetNodeAndPort(tRealAddr.getAddress().toUInt32(), tRealAddr.getPort());
        }
        else
        {
            tAddress.SetLoopback(0);
        }
    }
    // line 97
    virtual void Close()
    {
        if (m_ptConnection && !m_bDontCloseConnection)
        {
            m_ptConnection->disconnect();
        }
        m_bConnected = FALSE;
        m_bMigrated = FALSE;
    }
    // line 110
    void SetDontCloseConnection()
    {
        m_bDontCloseConnection = TRUE;
    }
    // line 117
    virtual uint32 GetMaxSendBufferSize() const
    {
        if (!IsConnected())
        {
            return 0;
        }
        return CONSOLE_SEND_BUFFER_SIZE - m_dwSendBufferSize;
    }
    // line 128
    virtual BOOL HasDataToSend() const
    {
        return m_dwSendBufferSize != 0;
    }
    virtual BOOL Send(void * pbBuffer, uint32 dwSize);
    virtual BOOL Flush();
    virtual BOOL Receive(intptr idUpcallHandle, SocketMessageHandlerProc pfnMsgHandler);
    uint32 ReceiveLoopbackData(void * pbBuffer, uint32 dwSize);
    // line 149
    bdConnectionRef GetConnection() const
    {
        return m_ptConnection;
    }
    // line 155
    void SetMigrated()
    {
        m_bMigrated = TRUE;
    }
    // line 161
    virtual void onDisconnect()
    {
        m_bConnected = FALSE;
    }
    // line 167
    uint32 GetID() const
    {
        return m_id;
    }
private:
    static uint32 ms_idSequence;
    uint32 m_id;  // 0x0c
    bdConnectionRef m_ptConnection;  // 0x10
    BOOL m_bConnected;  // 0x18
    Type m_eType;  // 0x1c
    BOOL m_bMigrated;  // 0x20
    BOOL m_bDontCloseConnection;  // 0x24
    Blizzard::Lock::RMutex m_tReceiveMutex;  // 0x28
    uint8 m_pbReceiveBuffer[32768];  // 0x48
    uint32 m_dwReceiveBufferSize;  // 0x8048
    uint8 m_pbSendBuffer[32768];  // 0x804c
    uint32 m_dwSendBufferSize;  // 0x1004c
    bdQueue<bdReference<bdBitBuffer> > m_tMessageQueue;  // 0x10050
    uint32 m_dwMessageQueueUsed;  // 0x10068
    static uint32 ms_dwTotalMessageQueueUsed;
};
static_assert(sizeof(GameConnection) == 0x10070, "GameConnection");

typedef bdReference<Console::Online::GameConnection> GameConnectionRef;

}  // namespace Online
}  // namespace Console

namespace Console { namespace Online {
void GameClientConnectionMigrated();  // Defined in ConsoleGameConnection.cpp:109.
} }
