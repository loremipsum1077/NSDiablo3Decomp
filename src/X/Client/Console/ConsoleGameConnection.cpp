#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdHashMap.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "ConsoleGameConnection.h"
#include "ConsoleParty.h"
#include "../../../Contrib/Contrib/DemonWare/bdSocket/bdAddressMap.h"
#include "../../../Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdMessage.h"
namespace Console {
namespace Online {

extern PartyRef sptParty;  // ConsoleParty.cpp

enum {
    CONSOLE_MIN_GUARANTEED_RECEIVE_QUEUE_SIZE = 32768,
    CONSOLE_MAX_COMBINED_RECEIVE_QUEUE_SIZE = 1048576,
};

typedef bdHashMap<bdReference<bdConnection>, bdReference<GameConnection>, bdHashingClass> ServerGameConnectionMap;

GameNetwork::ConnectionRef GameGetClientGameConnection();






ServerGameConnectionMap * sptServerGameConnectionMap;
GameConnectionRef sptClientGameConnection;


Blizzard::Lock::RMutex stServerConnectionsMutex;
bdQueue<bdReference<GameConnection> > stServerIncomingConnectionQueue;






GameNetwork::ConnectionRef GameGetClientGameConnectionCheckID(uint32 idConnection)
{
    if (sptParty && sptParty->IsInGame() && sptClientGameConnection && sptClientGameConnection->GetID() == idConnection)
    {
        return GameGetClientGameConnection();
    }
    return NULL;
}
















inline GameNetwork::ConnectionRef GameGetClientGameConnection()
{
    return sptClientGameConnection;
}






uint32 GameCreateClientGameConnection(bdConnectionRef ptHostConnection)
{
    sptClientGameConnection = ::new GameConnection(ptHostConnection, GameConnection::CONNECTION_CLIENT_TO_SERVER);
    return sptClientGameConnection->GetID();
}







uint32 GameGetClientGameConnectionID()
{
    return sptClientGameConnection->GetID();
}







void GameClearClientGameConnection()
{
    sptClientGameConnection = NULL;
}





void GameClientConnectionMigrated()
{
    if (sptClientGameConnection)
    {
        sptClientGameConnection->SetMigrated();
        sptClientGameConnection = NULL;
    }
}







GameNetwork::ConnectionRef GameGetServerGameConnection(bdConnectionRef ptConnection)
{
    Blizzard::Lock::ScopedRMutex tLock(stServerConnectionsMutex);
    GameConnectionRef ptGameConnection;
    sptServerGameConnectionMap->get(ptConnection, ptGameConnection);
    return ptGameConnection;
}





void GameCreateServerGameConnection(
    bdConnectionRef ptConnection)
{
    Blizzard::Lock::ScopedRMutex tConnectionQueueLock(stServerConnectionsMutex);

    GameConnectionRef ptGameConnection;
    if (!sptServerGameConnectionMap->get(ptConnection, ptGameConnection))
    {
        ptGameConnection = ::new GameConnection(
            ptConnection, GameConnection::CONNECTION_SERVER_TO_CLIENT);


        stServerIncomingConnectionQueue.enqueue(ptGameConnection);
        sptServerGameConnectionMap->put(ptConnection, ptGameConnection);
    }
}




GameNetwork::ConnectionRef GameGetServerPendingConnection()
{
    Blizzard::Lock::ScopedRMutex tLock(stServerConnectionsMutex);
    while (!stServerIncomingConnectionQueue.isEmpty())
    {
        GameConnectionRef ptGameConnection = stServerIncomingConnectionQueue.peek();
        stServerIncomingConnectionQueue.dequeue();
        if (ptGameConnection->IsConnected())
        {
            return ptGameConnection;
        }
        sptServerGameConnectionMap->remove(ptGameConnection->GetConnection());
    }
    return NULL;
}









void GameClearServerPendingConnections()
{
    stServerConnectionsMutex.Enter();
    while (!stServerIncomingConnectionQueue.isEmpty())
    {
        stServerIncomingConnectionQueue.dequeue();
    }
    stServerConnectionsMutex.Leave();
}



void GameClearServerConnections()
{
    stServerConnectionsMutex.Enter();
    sptServerGameConnectionMap->clear();
    stServerConnectionsMutex.Leave();
}



void GameRemoveServerGameConnection(bdConnectionRef ptConnection)
{
    stServerConnectionsMutex.Enter();
    sptServerGameConnectionMap->remove(ptConnection);
    stServerConnectionsMutex.Leave();
}



void GameMarkServerGameConnectionsDontClose()
{
    Blizzard::Lock::ScopedRMutex tLock(stServerConnectionsMutex);
    for (ServerGameConnectionMap::Iterator iter = sptServerGameConnectionMap->getIterator(); iter; sptServerGameConnectionMap->next(iter))
    {
        GameConnectionRef ptGameConnection = sptServerGameConnectionMap->getValue(iter); ptGameConnection->SetDontCloseConnection();
    }
}











GameNetwork::ConnectionRef GameGetGameConnection(bdConnectionRef ptConnection)
{
    Blizzard::Lock::ScopedRMutex tLock(stServerConnectionsMutex);
    GameConnectionRef ptGameConnection;
    if (sptClientGameConnection && ptConnection == sptClientGameConnection->GetConnection())
    {
        ptGameConnection = sptClientGameConnection;
    }
    else
    {
        sptServerGameConnectionMap->get(ptConnection, ptGameConnection);
    }
    return ptGameConnection;
}







void GameCleanupGameConnections()

{
    Blizzard::Lock::ScopedRMutex tServerConnectionsLock(stServerConnectionsMutex);

    GameConnectionRef ptGameConnection;
    if (sptServerGameConnectionMap->get(NULL, ptGameConnection))  // INFERENCE: key is a null connection (original hashes 8 zero bytes: bucket 0, key compared with 0).
    {
        ptGameConnection->Close();
    }
    sptServerGameConnectionMap->clear();

    GameClearServerPendingConnections();

    if (sptClientGameConnection)
    {
        sptClientGameConnection->Close();
        sptClientGameConnection = NULL;
    }
}






void GameReceivePacketFromClient(bdConnectionRef ptConnection, bdBitBufferRef ptPayload)
{
    GameConnectionRef ptGameConnection;
    if (sptServerGameConnectionMap->get(ptConnection, ptGameConnection))
    {
        ptGameConnection->ReadPayload(ptPayload);
    }
}




BOOL GameReceivePacketFromServer(bdBitBufferRef ptPayload)
{
    if (!sptClientGameConnection)
    {
        return FALSE;
    }
    sptClientGameConnection->ReadPayload(ptPayload);
    return TRUE;
}







void GameConnection::InitializeForApp()
{
    sptServerGameConnectionMap = new ServerGameConnectionMap(4, 1.0f);
}





void GameConnection::DisposeForApp()
{
    delete sptServerGameConnectionMap;
    sptServerGameConnectionMap = NULL;
    sptClientGameConnection = NULL;
}



BOOL GameConnection::ReadPayload(bdBitBufferRef ptPayload)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tReceiveMutex);
    // INFERENCE: 0xFFFFFFFF marks a failed receive buffer (cmp with -1 here, in Receive and in ReceiveLoopbackData).
    BOOL bOk = m_dwReceiveBufferSize != 0xFFFFFFFF;

    if (!bOk)
    {
        return bOk;
    }



    uint32 dwReadSize = (ptPayload->getWritePosition() - ptPayload->getReadPosition()) / 8;



    uint32 dwAvailableSize = CONSOLE_RECEIVE_BUFFER_SIZE - m_dwReceiveBufferSize;
    uint32 dwActualReadSize = Math::Min(dwReadSize, dwAvailableSize);



    if (dwActualReadSize > 0)
    {
        bOk = bOk && ptPayload->readBits(m_pbReceiveBuffer + m_dwReceiveBufferSize, dwActualReadSize * 8);
        m_dwReceiveBufferSize += dwActualReadSize;
    }


    if (bOk && dwReadSize > dwAvailableSize)
    {


        if ((m_dwMessageQueueUsed + dwReadSize) < CONSOLE_MIN_GUARANTEED_RECEIVE_QUEUE_SIZE ||
            (ms_dwTotalMessageQueueUsed + dwReadSize) < CONSOLE_MAX_COMBINED_RECEIVE_QUEUE_SIZE)
        {
            uint32 dwDataSize = ptPayload->getDataSize();
            m_dwMessageQueueUsed += dwDataSize;
            ms_dwTotalMessageQueueUsed += dwDataSize;
            m_tMessageQueue.enqueue(ptPayload);
        }
        else
        {
            bOk = FALSE;
        }
    }








    if (!bOk)
    {
        m_dwReceiveBufferSize = 0xFFFFFFFF;
    }

    return bOk;
}




BOOL GameConnection::Receive(
    intptr idUpcallHandle,
    SocketMessageHandlerProc pfnMsgHandler)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tReceiveMutex);

    if (m_dwReceiveBufferSize == 0xFFFFFFFF)
    {
        bdNChar8 szAddress[22] = "LOOPBACK";
        if (m_ptConnection)
        {
            bdAddressMap::addrToString(m_ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
        }
        // INFERENCE: szAddress only feeds a log call compiled out of this build.

        if (!m_tMessageQueue.isEmpty())
        {
            while (!m_tMessageQueue.isEmpty())
            {
                m_tMessageQueue.dequeue();
            }

            ms_dwTotalMessageQueueUsed -= m_dwMessageQueueUsed;
            m_dwMessageQueueUsed = 0;
        }
        return FALSE;
    }
    else if (m_dwReceiveBufferSize > 0)
    {

        SocketReceivedData tData;
        tData.lpMsg          = m_pbReceiveBuffer;
        tData.dwSize         = m_dwReceiveBufferSize;
        tData.dwConsumed     = 0xFFFFFFFF;

        m_tReceiveMutex.Leave();
        BOOL bResult = pfnMsgHandler(idUpcallHandle, SOCKETEVENT_RECEIVED_DATA, &tData);



        m_tReceiveMutex.Enter();

        if (!bResult)
        {
            bdNChar8 szAddress[22] = "LOOPBACK";
            if (m_ptConnection)
            {
                bdAddressMap::addrToString(m_ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
            }
            // INFERENCE: szAddress only feeds a log call compiled out of this build.
            return FALSE;
        }




        if (tData.dwConsumed > 0)
        {
            uint32 dwBytesRemaining = m_dwReceiveBufferSize - tData.dwConsumed;
            if (dwBytesRemaining > 0)
            {
                SigmaMemoryMove(m_pbReceiveBuffer, m_pbReceiveBuffer + tData.dwConsumed, dwBytesRemaining);
            }



            m_dwReceiveBufferSize = dwBytesRemaining;
        }
    }


    while (!m_tMessageQueue.isEmpty() &&
        m_dwReceiveBufferSize < CONSOLE_RECEIVE_BUFFER_SIZE)
    {
        bdBitBufferRef ptPayload = m_tMessageQueue.peek();

        uint32 dwAvailableSize = CONSOLE_RECEIVE_BUFFER_SIZE - m_dwReceiveBufferSize;


        uint32 dwReadSize = (ptPayload->getWritePosition() - ptPayload->getReadPosition()) / 8;
        uint32 dwActualReadSize = Math::Min(dwReadSize, dwAvailableSize);

        bdBool bOk = ptPayload->readBits(m_pbReceiveBuffer + m_dwReceiveBufferSize, dwActualReadSize * 8);
        m_dwReceiveBufferSize += dwActualReadSize;

        if (!bOk)
        {
            bdNChar8 szAddress[22] = "LOOPBACK";
            if (m_ptConnection)
            {
                bdAddressMap::addrToString(m_ptConnection->getAddressHandle(), szAddress, sizeof(szAddress));
            }
            // INFERENCE: szAddress only feeds a log call compiled out of this build.
            m_dwReceiveBufferSize = 0xFFFFFFFF;
        }


        if (dwReadSize <= dwAvailableSize)
        {
            uint32 dwDataSize = ptPayload->getDataSize();
            m_dwMessageQueueUsed -= dwDataSize;
            ms_dwTotalMessageQueueUsed -= dwDataSize;
            m_tMessageQueue.dequeue();
        }
    }

    return TRUE;
}




uint32 GameConnection::ReceiveLoopbackData(
    void * pbBuffer,
    uint32 dwSize)
{
    Blizzard::Lock::ScopedRMutex tLock(m_tReceiveMutex);

    BOOL bOk = m_dwReceiveBufferSize != 0xFFFFFFFF;

    if (!bOk)
    {
        return 0;
    }


    uint32 dwAvailableSize = CONSOLE_RECEIVE_BUFFER_SIZE - m_dwReceiveBufferSize;
    if (dwSize > dwAvailableSize)
    {
        dwSize = dwAvailableSize;
    }

    if (dwSize > 0)
    {
        SigmaMemoryCopy(m_pbReceiveBuffer + m_dwReceiveBufferSize, pbBuffer, dwSize);
        m_dwReceiveBufferSize += dwSize;
    }

    return dwSize;
}




BOOL GameConnection::Send(void * pbBuffer, uint32 dwSize)
{
    if (!IsConnected())
    {
        return FALSE;
    }

    // Callers check GetMaxSendBufferSize first.









    XASSERT(dwSize <= CONSOLE_SEND_BUFFER_SIZE - m_dwSendBufferSize);
    SigmaMemoryCopy(m_pbSendBuffer + m_dwSendBufferSize, pbBuffer, dwSize);
    m_dwSendBufferSize += dwSize;
    return TRUE;
}









BOOL GameConnection::Flush()

{
    uint8 * pbBuffer = m_pbSendBuffer;
    uint32 dwSize = m_dwSendBufferSize;

    if (dwSize == 0)
    {
        return TRUE;
    }

    if (m_eType == CONNECTION_CLIENT_TO_SERVER &&
        !GameGetClientGameConnection())
    {
        // INFERENCE: the client connection is gone; drop the pending data.
        m_dwSendBufferSize = 0;
        return TRUE;
    }

    if (!IsConnected())
    {
        return FALSE;
    }

    if (m_ptConnection)
    {
        ScopedNetworkSendLock tLock;

        while (dwSize > 0)
        {
            bdMessageType eMessageType = (m_eType == CONNECTION_SERVER_TO_CLIENT) ? D3_GAMESTREAM_SERVER : D3_GAMESTREAM_CLIENT;
            bdMessageRef ptMessage = ::new bdMessage(eMessageType, false);
            bdBitBufferRef ptPayload = ptMessage->getPayload();


            uint32 dwFragmentSize = Math::Min<uint32>(dwSize, MAX_FRAGMENT_SIZE);


            ptPayload->writeBits(pbBuffer, dwFragmentSize * 8);
            if (!m_ptConnection->send(ptMessage, true))
            {
                break;
            }

            dwSize -= dwFragmentSize;
            pbBuffer += dwFragmentSize;

        }
    }
    else
    {
        GameConnectionRef ptGameConnection;

        if (m_eType == CONNECTION_CLIENT_TO_SERVER)
        {

            ptGameConnection = reference_cast<GameConnection>(GameGetServerGameConnection(m_ptConnection));
        }
        else
        {

            ptGameConnection = reference_cast<GameConnection>(GameGetClientGameConnection());
        }

        // INFERENCE: null check; the original returns FALSE here but the check has no line row.
        if (!ptGameConnection)
        {
            return FALSE;
        }

        uint32 uSentSize = ptGameConnection->ReceiveLoopbackData(pbBuffer, dwSize);
        dwSize -= uSentSize;
        pbBuffer += uSentSize;
    }


    if (dwSize > 0 && m_pbSendBuffer != pbBuffer)
    {

        SigmaMemoryMove(m_pbSendBuffer, pbBuffer, dwSize);
    }
    m_dwSendBufferSize = dwSize;

    return TRUE;
}

}  // namespace Online
}  // namespace Console
