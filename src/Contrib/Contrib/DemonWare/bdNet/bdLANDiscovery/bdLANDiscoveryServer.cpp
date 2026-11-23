// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryServer.cpp

#include "bdLANDiscoveryServer.h"
#include "bdLANDiscoveryListener.h"
#include "../../bdConnection/bdMessage/bdMessageTypes.h"
#include "../../bdCore/bdContainers/bdBitBuffer.h"
#include "../../bdCore/bdContainers/bdBitBuffer.inl"
#include "../../bdCore/bdContainers/bdFastArray.inl"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdSocket/bdSocketRouterAdHoc-switch.h"





bdLANDiscoveryServer::bdLANDiscoveryServer()
    : m_gameInfo(), m_socket(), m_listeners(), m_status(BD_IDLE)
{
}

bdLANDiscoveryServer::~bdLANDiscoveryServer()
{
    stop();
}

bdBool bdLANDiscoveryServer::start(const bdGameInfoRef gameInfo, const bdInetAddr & localAddr, const bdPort discoveryPort)
{
    const bdBool blocking = false;
    bdBool ok = m_socket.create(blocking, true, true);
    if (ok)
    {
        const bdAddr bindAddr(localAddr, discoveryPort);
        if (m_socket.bind(bindAddr) == BD_NET_SUCCESS)
        {
            m_status = BD_PENDING;
            m_gameInfo = gameInfo;
            ok = true;
        }
        else
        {
            ok = false;
            m_status = BD_ERROR;
        }
    }
    else
    {
        m_status = BD_ERROR;
    }
    return ok;
}



void bdLANDiscoveryServer::update()
{
    if (m_status == BD_PENDING)
    {
        bdAddr from;
        bdUByte8 data[1288];

        const bdInt32 received = m_socket.receiveFrom(from, data, sizeof(data));

        if (received >= 12)
        {
            bdBitBuffer payload(data, received * 8, true);




            bdMessageType type;
            bdBool ok = payload.readUByte8(type);
            if (ok && type == 27)
            {
                bdUByte8 tempNonce[8];
                bdUInt32 titleID;
                ok = payload.readFull(tempNonce);


                ok = ok && payload.readUInt32(titleID);


                if (ok && titleID == m_gameInfo->getTitleID())
                {
                    bdBitBuffer discoveryReply(0, false);
                    discoveryReply.writeUByte8(28);
                    discoveryReply.writeFull(tempNonce);
                    m_gameInfo->serialize(discoveryReply);


















                    const bdInt sendResult = m_socket.sendTo(from, discoveryReply.getData(), discoveryReply.getDataSize());
                    if (sendResult > 0)
                    {

                        bdNChar8 buf[22];
                        from.toString(buf, sizeof(buf));
                        bdLogInfo("lan discovery", "Replied to discovery request from %s", buf);

                        for (bdUInt i = 0; i < m_listeners.getSize(); ++i)
                        {
                            m_listeners[i]->onRequest();
                        }
                    }
                }
            }
        }
    }
    else
    {
        bdLogWarn("lan discovery", "update called while not running");
    }
}
inline void bdLANDiscoveryServer::stop()
{

    m_socket.close();
    m_status = BD_IDLE;
}















bdGameInfoRef bdLANDiscoveryServer::getGameInfo()
{
    return m_gameInfo;
}
