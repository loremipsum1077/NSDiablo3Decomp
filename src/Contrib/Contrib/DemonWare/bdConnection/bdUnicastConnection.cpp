// Original file: Contrib/Contrib/DemonWare/bdConnection/bdUnicastConnection.cpp
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "bdMessage/bdMessage.h"
class bdAddrHandle;
#include "../bdCore/bdContainers/bdSingleton.h"
#include "../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdSocket/bdAddressMap.h"
#include "bdConnectionListener.h"
#include "bdPacket/bdCookieAckChunk.h"
#include "bdPacket/bdCookieEchoChunk.h"
#include "bdPacket/bdDataChunk.h"
#include "bdPacket/bdHeartbeatAckChunk.h"
#include "bdPacket/bdHeartbeatChunk.h"
#include "bdPacket/bdInitAckChunk.h"
#include "bdPacket/bdInitChunk.h"
#include "bdPacket/bdPacket.h"
#include "bdPacket/bdSAckChunk.h"
#include "bdPacket/bdShutdownAckChunk.h"
#include "bdPacket/bdShutdownChunk.h"
#include "bdPacket/bdShutdownCompleteChunk.h"
#include "bdUnicastConnection.h"
#include "bdWindow/bdReliableReceiveWindow.h"
#include "bdWindow/bdReliableSendWindow.h"





































































































































bdBool bdUnicastConnection::connect()
{
    bdBool result = false;



    if (m_state == BD_UC_CLOSED && bdConnection::connect()) {


        if (sendInit()) {
            m_state = BD_UC_COOKIE_WAIT;
            result = true;
        }
    }
    if (!result)








        bdUseVAArgs();
    return result;
}



void bdUnicastConnection::disconnect()
{
    if (m_state == BD_UC_ESTABLISHED) {
        m_state = BD_UC_SHUTDOWN_PENDING;
        m_shutdownGuard.start();
    } else if (m_state == BD_UC_COOKIE_WAIT || m_state == BD_UC_COOKIE_ECHOED) {
        close();
    }
}








bdBool bdUnicastConnection::send(const bdMessageRef message, const bdBool reliable)
{


    bdBool sent = false;
    if (m_state == BD_UC_ESTABLISHED) {


        bdUInt payLoadSize = 0;

        if (message->hasPayload())

            payLoadSize += message->getPayload()->getDataSize();


        if (message->hasUnencryptedPayload())

            payLoadSize += message->getUnencryptedPayload()->getDataSize();
        if (payLoadSize <= 1236) {
            bdDataChunk::bdDataFlags flags
                = reliable ? bdDataChunk::BD_DC_NO_FLAGS : bdDataChunk::BD_DC_UNRELIABLE;








            bdDataChunkRef chunk = new bdDataChunk(message, flags);
            if (reliable) {
                if (!m_reliableSendWindow)



                    m_reliableSendWindow = new bdReliableSendWindow();

                sent = m_reliableSendWindow->add(chunk);
                if (!sent)

                    bdUseVAArgs();
            } else {



                m_unreliableSendWindow.add(chunk);
                sent = true;
            }
        } else


            bdUseVAArgs(payLoadSize, 1236);
    } else



        bdUseVAArgs();
    return sent;
}




bdConnection::Status bdUnicastConnection::getStatus() const
{
    bdConnection::Status status = BD_DISCONNECTED;
    switch (m_state) {
    case BD_UC_CLOSED:
        status = BD_DISCONNECTED;
        break;
    case BD_UC_COOKIE_WAIT:
    case BD_UC_COOKIE_ECHOED:
        status = BD_CONNECTING;
        break;
    case BD_UC_ESTABLISHED:
        status = BD_CONNECTED;
        break;
    case BD_UC_SHUTDOWN_PENDING:
    case BD_UC_SHUTDOWN_SENT:
    case BD_UC_SHUTDOWN_RECEIVED:
    case BD_UC_SHUTDOWN_ACK_SENT:
        status = BD_DISCONNECTING;
        break;
    default:


        bdUseVAArgs();
        break;
    }
    return status;
}

void bdUnicastConnection::close()
{
    if (m_state != BD_UC_CLOSED) {
        if (m_state < BD_UC_ESTABLISHED)





            callListenersConnect(false);
        else


            callListenersDisconnect();
    }
    m_state = BD_UC_CLOSED;
}















bdUnicastConnection::bdUnicastConnection(
    bdCommonAddrRef dest, bdAddressMap* addrMap, bdBool acceptOutOfOrder)

    : bdConnection(dest)
    , m_reliableSendWindow(0)
    , m_reliableRecvWindow(0)
    , m_state(BD_UC_CLOSED)
    , m_addrMap(addrMap)
    , m_acceptOutOfOrder(acceptOutOfOrder)
{
    if (!addrMap)

        bdUseVAArgs();

    reset();
}

bdUnicastConnection::bdUnicastConnection(bdAddressMap* addrMap, bdBool acceptOutOfOrder)
    : m_reliableSendWindow(0)
    , m_reliableRecvWindow(0)
    , m_state(BD_UC_CLOSED)
    , m_addrMap(addrMap)
    , m_acceptOutOfOrder(acceptOutOfOrder)
{
    if (!addrMap)


        bdUseVAArgs();
    reset();
}


bdUnicastConnection::~bdUnicastConnection()
{
    delete m_reliableRecvWindow;
    delete m_reliableSendWindow;
}

// NON_MATCHING: 1040 vs 1040, every row on its original line; only block placement differs: the original puts ~bdPacket
// and the return right after the 492 log (later exits branch back to it), ours at the end. `else { log; return false; }` is worse.
bdBool bdUnicastConnection::receive(const bdUByte8* buffer,
                                     const bdUInt32 bufferSize)
{


    if (m_shutdownGuard.getElapsedTimeInSeconds() > 5.0f)
    {
        close();
        return false;
    }

    if (!bufferSize)
    {
        bdUseVAArgs();
        return false;
    }

    bdBool dataHandled = false;
    bdBool result = false;

    bdPacket packet;




    bdBool valid = packet.deserialize(buffer, bufferSize);

    m_stats.addBytesRecv(bufferSize + 28);


    if (valid)
    {
        m_stats.addPacketsRecv(1);
        m_stats.addPacketSizeRecv(bufferSize);
        {







            bdChunkRef chunk;
            packet.getNextChunk(chunk);

            if (chunk && chunk->isControl()) {

                const bdChunkTypes type = chunk->getType();

                switch (type)
                {
                case BD_CT_INIT:
                    result = handleInit(chunk);
                    break;
                case BD_CT_INIT_ACK:
                    result = handleInitAck(chunk, packet.getVerificationTag());
                    break;
                case BD_CT_COOKIE_ECHO:
                    result = handleCookieEcho(chunk, packet.getVerificationTag());
                    break;
                case BD_CT_COOKIE_ACK:
                    result = handleCookieAck(chunk, packet.getVerificationTag());
                    break;
                default:
                    break;
                }

                if (result)
                {
                    chunk = 0;
                }
            }




            if (m_localTag == packet.getVerificationTag()) {
                do {


                    if (chunk)
                    {
                        const bdChunkTypes type = chunk->getType();

                        switch (type)
                        {
                        case BD_CT_DATA:
                            result = handleData(chunk);
                            dataHandled |= result;
                            break;
                        case BD_CT_SACK:
                            result = handleSAck(chunk);
                            break;
                        case BD_CT_HEARTBEAT:
                            result = handleHeartbeat(chunk);
                            break;
                        case BD_CT_HEARTBEAT_ACK:
                            result = handleHeartbeatAck(chunk);
                            break;
                        case BD_CT_SHUTDOWN:
                            result = handleShutdown(chunk);
                            break;
                        case BD_CT_SHUTDOWN_ACK:
                            result = handleShutdownAck(chunk);
                            break;
                        case BD_CT_SHUTDOWN_COMPLETE:
                            result = handleShutdownComplete(chunk);
                            break;
                        default:
                            break;
                        }
                    }

                }
                while (packet.getNextChunk(chunk));
            }

            if (result)
            {
                m_receiveTimer.start();
            }
        }
    }
    else
        bdUseVAArgs();






    if (dataHandled && m_state == BD_UC_SHUTDOWN_SENT)
    {
        if (!sendShutdown())
        {
            close();
        }
    }








    return result;
}

// Laid on the original's line rows (getDataToSend 0x7100b15500).
bdUInt bdUnicastConnection::getDataToSend(bdUByte8* const buffer,
                                          const bdUInt bufferSize)
{
    bdPacket packet(m_peerTag, bufferSize);



    if (m_shutdownGuard.getElapsedTimeInSeconds() > 5.0f)
    {
        close();
        return 0;
    }




    if (m_outQueue.isEmpty())
    {

        if (m_initTimer.getElapsedTimeInSeconds() > 0.5f)
        {
            if (!sendInit())
            {
                close();
            }
        }


        else if (m_cookieTimer.getElapsedTimeInSeconds() > 0.5f)
        {
            if (!sendCookieEcho(m_initAckChunk))
            {
                close();
            }
        }


        else if (m_shutdownTimer.getElapsedTimeInSeconds() > 0.5f)
        {
            switch (m_state)
            {
            case BD_UC_SHUTDOWN_SENT:
                if (!sendShutdown())
                {
                    close();
                }
                break;

            case BD_UC_SHUTDOWN_RECEIVED:
            case BD_UC_SHUTDOWN_ACK_SENT:

                if (!sendShutdownAck())
                {
                    close();
                }
                break;
            }
        }
    }







    while (!m_outQueue.isEmpty())
    {
        bdControlChunkStore& chunkStore = m_outQueue.peek();

        if (chunkStore.m_lone)
        {


            bdChunkRef chunk;
            while (packet.getNextChunk(chunk))
            {
                m_outQueue.enqueue(bdControlChunkStore(chunk, false));
            }

            bdUInt32 tagToUse = m_peerTag;

            if (chunkStore.m_chunk->getType() == BD_CT_INIT_ACK)
            {
                tagToUse = bdInitAckChunkRef(chunkStore.m_chunk)->getPeerTag();
            }


            bdPacket lpacket(tagToUse, bufferSize);
            lpacket.addChunk(chunkStore.m_chunk);
            m_outQueue.dequeue();

            return lpacket.serialize(buffer, bufferSize);
        }


        if (!packet.addChunk(chunkStore.m_chunk))
            break;
        m_outQueue.dequeue();
    }











    const bdFloat32 lastReceived = m_receiveTimer.getElapsedTimeInSeconds();
    switch (m_state)
    {
    case BD_UC_ESTABLISHED:
    case BD_UC_SHUTDOWN_PENDING:
    case BD_UC_SHUTDOWN_SENT:
    case BD_UC_SHUTDOWN_RECEIVED:



        if (lastReceived > 9.0f)
        {
            bdUseVAArgs();
            close();
        }
        else
        {



            if (m_reliableRecvWindow)
            {
                m_reliableRecvWindow->getDataToSend(packet);
            }
            if (m_reliableSendWindow)
            {
                m_reliableSendWindow->getDataToSend(packet);
            }


            m_unreliableSendWindow.getDataToSend(packet);
        }
        break;
    }








    switch (m_state)
    {
    case BD_UC_SHUTDOWN_PENDING:
        if (packet.isEmpty() && windowsEmpty())
        {

            if (sendShutdown())
            {
                m_state = BD_UC_SHUTDOWN_SENT;
            }
            else
            {
                close();
            }
        }
        break;
    case BD_UC_SHUTDOWN_RECEIVED:
        if (packet.isEmpty() && windowsEmpty())
        {
            if (sendShutdownAck())
            {
                m_state = BD_UC_SHUTDOWN_ACK_SENT;
            }
            else
            {
                close();
            }
        }
        break;
    }






    switch (m_state)
    {
    case BD_UC_ESTABLISHED:
        {
            if (packet.isEmpty())
            {
                const bdFloat32 lastSent = m_sendTimer.getElapsedTimeInSeconds();



                if (lastSent > 1.8f)
                {
                    bdUseVAArgs(lastSent);

                    bdHeartbeatChunkRef chunk = new bdHeartbeatChunk();

                    packet.addChunk(chunk);
                }
            }
        }
        break;
    }



    bdUInt dataToSend = 0;
    if (!packet.isEmpty())
    {
        dataToSend = packet.serialize(buffer, bufferSize);

        if (dataToSend > bufferSize)
        {
            bdUseVAArgs();
        }

        m_sendTimer.start();


        if (dataToSend)
        {
            m_stats.addBytesSent(dataToSend + 28);
            m_stats.addPacketsSent(1);
            m_stats.addPacketSizeSent(dataToSend);
        }
    }
    return dataToSend;

}






bdBool bdUnicastConnection::getMessageToDispatch(bdMessageRef& message)
{
    bdBool result = false;

    bdMessageRef toDispatch;
    if (m_reliableRecvWindow) {


        bdDataChunkRef chunk = m_reliableRecvWindow->getNextToRead();
        if (chunk) {

            toDispatch = chunk->getMessage();
            result = true;
        }
    }
    if (!result) {


        bdDataChunkRef chunk = m_unreliableReceiveWindow.getNextToRead();
        if (chunk) {

            toDispatch = chunk->getMessage();
            result = true;
        }
    }
    if (result)
        message = toDispatch;
    return result;
}





bdBool bdUnicastConnection::getOutOfOrderMessageToDispatch(bdMessageRef& message)
{


    bdDataChunkRef chunk = m_unreliableReceiveWindow.getNextOutOfOrderToRead();
    bdBool result = false;
    if (chunk) {
        message = chunk->getMessage();
        result = true;
    }
    return result;
}







bdBool bdUnicastConnection::handleData(bdChunkRef& chunk)
{
    bdBool result;

    bdDataChunkRef dchunk = reference_cast<bdDataChunk>(chunk);

    if (dchunk->getFlags() & bdDataChunk::BD_DC_UNRELIABLE) {

        result = m_unreliableReceiveWindow.add(dchunk, m_acceptOutOfOrder);
    } else {
        if (!m_reliableRecvWindow)



            m_reliableRecvWindow = new bdReliableReceiveWindow();

        result = m_reliableRecvWindow->add(dchunk);
        if (!result)


            bdUseVAArgs();
    }
    return result;
}



bdBool bdUnicastConnection::handleSAck(bdChunkRef& chunk)
{
    bdBool result = false;


    bdFloat32 rtt;
    bdSAckChunkRef schunk = reference_cast<bdSAckChunk>(chunk);
    if (m_reliableSendWindow) {

        result = m_reliableSendWindow->handleAck(schunk, rtt);
        if (result) {
            if (rtt > 0.0f) {
                if (m_smoothedRTT == 0.0f && m_RTTVariation == 0.0f) {
                    m_smoothedRTT = rtt;
                    m_RTTVariation = rtt * 0.5f;
                } else {
                    bdFloat32 absRTT
                        = m_smoothedRTT > rtt ? m_smoothedRTT - rtt : rtt - m_smoothedRTT;
                    m_RTTVariation = 0.75f * m_RTTVariation + 0.25f * absRTT;
                    m_smoothedRTT = 0.875f * m_smoothedRTT + 0.125f * rtt;
                }




























                m_stats.setLastRTT(m_smoothedRTT);
                if (m_RTTVariation == 0.0f)
                    m_RTTVariation = 100.0f;
                bdFloat32 rto = m_smoothedRTT + m_RTTVariation * 2.0f;
                if (rto < 0.02f)
                    rto = 0.02f;
                else if (rto > 2.0f)
                    rto = 2.0f;




















                m_reliableSendWindow->setTimeoutPeriod(rto);
            }
        } else {







            bdUseVAArgs();
            disconnect();
        }
    } else


        bdUseVAArgs();
    return result;
}



bdBool bdUnicastConnection::handleInit(bdChunkRef& chunk)
{
    bdBool result = false;

    bdInitChunkRef ichunk = reference_cast<bdInitChunk>(chunk);

    bdSecurityID id;
    if (m_addrMap) {





        m_addrMap->addrToCommonAddr(m_addrHandle, m_addr, id);
        if (ichunk->getInitTag()) {

            bdUseVAArgs(m_localTag);
            result = sendInitAck(chunk);
        } else


            bdUseVAArgs(ichunk->getInitTag());
    } else
        bdUseVAArgs();
    return result;
}



bdBool bdUnicastConnection::handleInitAck(bdChunkRef& chunk, const bdUInt32 vtag)
{
    bdBool result = false;
    if (m_state == BD_UC_COOKIE_WAIT) {
        if (m_localTag == vtag) {







            bdInitAckChunkRef iachunk = reference_cast<bdInitAckChunk>(chunk);

            m_peerTag = iachunk->getInitTag();
            if (m_peerTag) {


                bdUseVAArgs(m_localTag, m_peerTag);

                if (sendCookieEcho(iachunk)) {

                    m_initTimer.reset();
                    m_state = BD_UC_COOKIE_ECHOED;
                    result = true;
                }
            } else



                bdUseVAArgs();
        } else



            bdUseVAArgs();
    }
    return result;
}

// NON_MATCHING: 1212 vs 1212, on the original rows (tie case first at 1080, 1144/1172 blocks, closing brace 1195); block
// placement only: after the peerTieTag test ours lays out the 1156 chain, the original the 1133 m_localTag re-test and body.
bdBool bdUnicastConnection::handleCookieEcho(bdChunkRef& chunk, const bdUInt32)
{
    bdBool result = false;

    bdCookieEchoChunkRef cchunk = reference_cast<bdCookieEchoChunk>(chunk);

    bdCookieRef cookie;

    if (cchunk->getCookie(cookie)) {


        const bdUInt32 localTag = cookie->getLocalTag();
        const bdUInt32 peerTag = cookie->getPeerTag();
        const bdUInt32 localTieTag = cookie->getLocalTieTag();
        const bdUInt32 peerTieTag = cookie->getPeerTieTag();

        switch (m_state)
        {
        case BD_UC_CLOSED:
        {

            bdSecurityID id;
            if (m_addrMap) {





                m_addrMap->addrToCommonAddr(m_addrHandle, m_addr, id);

                m_localTag = localTag;
                m_peerTag = peerTag;
                bdUseVAArgs(localTag, peerTag);
                sendCookieAck();

                m_state = BD_UC_ESTABLISHED;
                callListenersConnect(true);
                result = true;
            } else
                bdUseVAArgs();
        }
        break;
        default:

























            // Rows: the tie-tag restart is tested first (1080); the original orders its compares L, PT, LT, P.
            if (m_localTag != localTag && m_peerTag != peerTag && m_localTag == localTieTag && m_peerTag == peerTieTag) {

























                reset();

                sendCookieAck();

                bdUseVAArgs(m_localTag, m_peerTag);
                bdSecurityID id;
                if (m_addrMap) {





                    m_addrMap->addrToCommonAddr(m_addrHandle, m_addr, id);

                    m_localTag = localTag;
                    m_peerTag = peerTag;

                    m_state = BD_UC_ESTABLISHED;

                    callListenersReconnect();
                    result = true;
                } else
                    bdUseVAArgs();



            } else if ((m_localTag == localTag && m_peerTag != peerTag)
                || (peerTieTag == 0 && m_localTag == localTag)) {










                if (m_state != BD_UC_ESTABLISHED) {

                    m_state = BD_UC_ESTABLISHED;
                    callListenersConnect(true);
                }
                m_peerTag = peerTag;
                m_initTimer.reset();
                m_cookieTimer.reset();
                sendCookieAck();
                result = true;


            } else if (m_localTag != localTag && m_peerTag == peerTag) {
                result = false;







            } else if (m_localTag == localTag && m_peerTag == peerTag) {






                if (m_state != BD_UC_ESTABLISHED) {


                    m_state = BD_UC_ESTABLISHED;
                    callListenersConnect(true);
                }
                m_initTimer.reset();
                m_cookieTimer.reset();
                sendCookieAck();
                result = true;
            } else
                result = true;
            break;
        }
    }







    return result;
}

bdBool bdUnicastConnection::handleCookieAck(bdChunkRef&, const bdUInt32 vtag)
{
    bdBool result = false;
    if (m_localTag == vtag) {
        if (m_state == BD_UC_COOKIE_ECHOED) {





            bdUseVAArgs(vtag, m_peerTag);
            m_state = BD_UC_ESTABLISHED;

            callListenersConnect(true);

            m_cookieTimer.reset();
            result = true;
        } else
            bdUseVAArgs(m_state);
    } else
        bdUseVAArgs(vtag);
    return result;
}









bdBool bdUnicastConnection::handleHeartbeat(bdChunkRef& chunk)
{
    m_receiveTimer.start();

    return sendHeartbeatAck(chunk);
}

bdBool bdUnicastConnection::handleHeartbeatAck(bdChunkRef&)
{
    m_receiveTimer.start();

    return true;
}

bdBool bdUnicastConnection::handleShutdown(bdChunkRef&)
{
    bdBool result = true;

    switch (m_state)
    {
    case BD_UC_ESTABLISHED:
        bdUseVAArgs();
        m_state = BD_UC_SHUTDOWN_RECEIVED;
        break;

    case BD_UC_SHUTDOWN_SENT:
        bdUseVAArgs();
        m_state = BD_UC_SHUTDOWN_RECEIVED;
        result = sendShutdownAck();
        break;
    default:
        bdUseVAArgs();
        result = false;
        break;
    }
    if (result)
    {
        bdUseVAArgs();
        m_shutdownGuard.start();
    }
    // Rows (removed out-of-line copy, 0x70 bytes): 1257 col 13 is `result = sendShutdownAck()`; every inlined exit sets the result to 1.
    return true;
}
// Two identical branches: the original tests 5 then 7 separately (cmp/b.eq, cmp/b.ne) and all of the shared body carries line 1277, i.e. the bodies were merged late, after InstCombine.
bdBool bdUnicastConnection::handleShutdownAck(bdChunkRef&)
{
    bdBool result = false;
    if (m_state == BD_UC_SHUTDOWN_SENT) {

















        bdUseVAArgs(); sendShutdownComplete(); close(); m_shutdownTimer.reset(); result = true;
    } else if (m_state == BD_UC_SHUTDOWN_ACK_SENT) {
        bdUseVAArgs(); sendShutdownComplete(); close(); m_shutdownTimer.reset(); result = true;
    } else
        bdUseVAArgs(m_state);
    return result;
}
bdBool bdUnicastConnection::handleShutdownComplete(bdChunkRef&)
{
    bdBool result = false;

    switch (m_state)
    {
    case BD_UC_SHUTDOWN_ACK_SENT:
        bdUseVAArgs();
        m_shutdownTimer.reset();
        close();
        result = true;
        break;
    default:
        bdUseVAArgs();
        break;
    }

    return result;
}


bdBool bdUnicastConnection::sendInit()
{
    bdBool result = m_state == BD_UC_CLOSED || m_state == BD_UC_COOKIE_WAIT;
    if (m_initResends++ < 5) {




        m_initTimer.start();

        bdInitChunkRef chunk = new bdInitChunk(m_localTag, 15000);

        m_outQueue.enqueue(bdControlChunkStore(chunk, false));

        bdUseVAArgs(m_localTag);
    } else
        result = false;
    return result;
}




// Rows: the default label leaves no row; its log sits at 1420 after the ack is sent, so the
// switch only selects the tags and `result` decides whether the ack goes out (inferred shape).
bdBool bdUnicastConnection::sendInitAck(bdInitChunkRef chunk)
{
    bdBool result = true;

    bdUInt32 peerTag = chunk->getInitTag();

    bdUInt32 localTag;
    bdUInt32 peerTieTag;
    bdUInt32 localTieTag;

    switch (m_state)
    {
    case BD_UC_CLOSED:
    case BD_UC_SHUTDOWN_ACK_SENT:

        m_peerTag = peerTag;
        localTag = m_localTag;
        peerTieTag = localTieTag = 0;
        break;

    case BD_UC_COOKIE_ECHOED:
        peerTieTag = m_peerTag;
        localTieTag = m_localTag;
        localTag = m_localTag;
        break;









    case BD_UC_COOKIE_WAIT:
        localTag = m_localTag;
        peerTieTag = localTieTag = 0;
        break;










    case BD_UC_ESTABLISHED:
    case BD_UC_SHUTDOWN_PENDING:
    case BD_UC_SHUTDOWN_SENT:
    case BD_UC_SHUTDOWN_RECEIVED:
        localTag = bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt();
        peerTieTag = m_peerTag;
        localTieTag = m_localTag;
        break;
    default:
        result = false;
        break;
    }
    if (result)
    {

        bdCookieRef cookie = new bdCookie(localTag, peerTag, localTieTag, peerTieTag);

        bdInitAckChunkRef iachunk = new bdInitAckChunk(localTag, cookie, 15000, peerTag);

        m_outQueue.enqueue(bdControlChunkStore(iachunk, true));

        bdUseVAArgs(m_localTag, localTag, m_peerTag);
    }
    else
    {
        bdUseVAArgs();
    }

    return result;
}

bdBool bdUnicastConnection::sendCookieEcho(bdInitAckChunkRef chunk)
{
    m_initAckChunk = chunk;
    bdBool result = false;
    if (m_cookieResends++ < 5) {



        bdByteBufferRef cookie;
        if (chunk->getCookie(cookie)) {

            m_cookieTimer.start();

            bdCookieEchoChunkRef cookieEcho = new bdCookieEchoChunk(cookie);

            m_outQueue.enqueue(bdControlChunkStore(cookieEcho, false));

            bdUseVAArgs(m_localTag, m_peerTag);
        }
        result = true;
    }
    return result;
}





bdBool bdUnicastConnection::sendCookieAck()
{


    bdCookieAckChunkRef chunk = new bdCookieAckChunk();

    m_outQueue.enqueue(bdControlChunkStore(chunk, false));

    bdUseVAArgs(m_localTag, m_peerTag);
    return true;
}











bdBool bdUnicastConnection::sendHeartbeatAck(bdInitChunkRef)
{


    if (m_outQueue.isEmpty() && windowsEmpty()) {

        bdHeartbeatAckChunkRef hchunk = new bdHeartbeatAckChunk();
        m_outQueue.enqueue(bdControlChunkStore(hchunk, false));
    }
    return true;
}


bdBool bdUnicastConnection::sendShutdown()
{
    bdBool result = false;
    if (m_shutdownResends++ < 5) {


        bdUseVAArgs(m_shutdownResends, 5);
        bdChunkRef chunk = new bdShutdownChunk();
        m_outQueue.enqueue(bdControlChunkStore(chunk, false));

        m_shutdownTimer.start();
        result = true;
    }
    return result;
}





bdBool bdUnicastConnection::sendShutdownAck()
{
    bdBool result = false;
    if (m_shutdownResends++ < 5) {


        bdUseVAArgs(m_shutdownResends, 5);
        bdChunkRef chunk = new bdShutdownAckChunk();
        m_outQueue.enqueue(bdControlChunkStore(chunk, false));

        m_shutdownTimer.start();
        result = true;
    }
    return result;
}





bdBool bdUnicastConnection::sendShutdownComplete()
{


    bdUseVAArgs();

    bdChunkRef chunk = new bdShutdownCompleteChunk();
    m_outQueue.enqueue(bdControlChunkStore(chunk, false));
    return true;
}




void bdUnicastConnection::reset()
{
    delete m_reliableRecvWindow;
    delete m_reliableSendWindow;
    m_reliableSendWindow = 0;
    m_reliableRecvWindow = 0;


    while (!m_outQueue.isEmpty())

        m_outQueue.dequeue();


    m_unreliableReceiveWindow.reset();
    m_unreliableSendWindow.reset();

    m_sendTimer.start();
    m_receiveTimer.start();
    m_initResends = 0;
    m_cookieResends = 0;
    m_shutdownResends = 0;


    m_localTag = bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUInt();
    m_peerTag = 0;
    m_smoothedRTT = m_RTTVariation = 0.0f;
}




void bdUnicastConnection::callListenersConnect(const bdBool success)
{
    for (bdUInt i = 0; i < m_listeners.getSize(); ++i)
    {
        if (success)
        {
            m_listeners[i]->onConnect(this);
        }
        else
        {
            m_listeners[i]->onConnectFailed(this);
        }
    }
}

void bdUnicastConnection::callListenersDisconnect()
{


    bdFastArray<bdConnectionListener*> copy(m_listeners);
    for (bdUInt i = 0; i < copy.getSize(); ++i) {

        bdUInt ignored;
        if (m_listeners.findFirst(copy[i], ignored))
            copy[i]->onDisconnect(this);
    }
}



void bdUnicastConnection::callListenersReconnect()
{
    for (bdUInt i = 0; i < m_listeners.getSize(); ++i)
    {
        m_listeners[i]->onReconnect(this);
    }
}

bdBool bdUnicastConnection::windowsEmpty() const
{
    bdBool empty = true;
    if (m_reliableSendWindow && !m_reliableSendWindow->isEmpty())
    {
        empty = false;
    }
    return empty;
}
