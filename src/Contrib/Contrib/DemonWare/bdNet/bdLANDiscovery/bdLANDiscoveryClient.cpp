// Original file: Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryClient.cpp
#include "bdLANDiscoveryClient.h"
#include "bdGameInfoFactory.h"
#include "bdLANDiscoveryListener.h"
#include "../bdNet.h"
#include "../../bdSocket/bdSocketRouterAdHoc-switch.h"
#include "../../bdConnection/bdMessage/bdMessageTypes.h"
#include "../../bdCore/bdContainers/bdBitBuffer.h"
#include "../../bdCore/bdContainers/bdBitBuffer.inl"
#include "../../bdCore/bdContainers/bdFastArray.inl"
#include "../../bdCore/bdContainers/bdSingleton.h"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"


bdLANDiscoveryClient::bdLANDiscoveryClient()
    : m_timeout(0.0f), m_timer(), m_socket(), m_listeners(), m_status(BD_IDLE)
{
}


bdLANDiscoveryClient::~bdLANDiscoveryClient()
{
    m_socket.close();
}

bdBool bdLANDiscoveryClient::discover(const bdUInt titleID,
                                      const bdFloat32 timeout,
                                      const bdInetAddr & addr,
                                      const bdPort discoveryPort)
{
    bdBool ok = false;
    if (m_status != BD_PENDING)
    {
        const bdBool blocking = false;
        ok = m_socket.create(blocking, true, true);











        if (ok)
        {
            bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(m_nonce, sizeof(m_nonce));
            m_timeout = timeout;
            bdBitBufferRef buffer(new bdBitBuffer(0, false));
            buffer->writeUByte8(27);
            buffer->writeFull(m_nonce);
            buffer->writeUInt32(titleID);




            const bdAddr dest(addr, discoveryPort);

            const bdInt sent = m_socket.sendTo(dest, buffer->getData(), buffer->getDataSize());
            if (sent < 0)
            {
                m_status = BD_ERROR;
                bdLogWarn("lan discovery", "Failed to send discovery request"); ok = false;
            }
            else
            {
                m_timer.start();
                m_status = BD_PENDING;

                bdLogInfo("lan discovery", "Discovery started, timeout %f", timeout);
            }
        }
        else
        {
            bdLogWarn("lan discovery", "Failed to create socket");
        }
    }
    else
    {
        bdLogWarn("lan discovery", "Discovery already in progress");
    }
    return ok;
}

void bdLANDiscoveryClient::update()
{
    if (m_status == BD_PENDING)
    {
        bdAddr addr;
        bdUByte8 data[1500];

        const bdInt received = m_socket.receiveFrom(addr, data, sizeof(data));

        if (received >= 12)
        {
            bdBitBuffer buffer(data, received * 8, true);

            bdMessageType type = 0;
            bdBool ok = buffer.readUByte8(type);


            if (ok && type == 28)
            {
                bdNChar8 addrStr[22];

                addr.toString(addrStr, sizeof(addrStr));
                bdLogInfo("lan discovery", "Received discovery reply from %s", addrStr);

                bdUByte8 nonce[8];
                ok = buffer.readFull(nonce);

                if (ok && bdMemcmp(m_nonce, nonce, sizeof(m_nonce)) == 0)
                {
                    bdGameInfoRef gameInfo(bdSingleton<bdGameInfoFactoryImpl>::getInstance()->create());
                    ok = gameInfo->deserialize(bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(), buffer);

                    bdLogInfo("lan discovery", "Deserialized game info");
                    fireOnDiscoveryListeners(gameInfo);
                }
                else
                {
                    bdLogWarn("lan discovery", "Nonce mismatch");
                }
            }
            else
            {
                bdLogWarn("lan discovery", "Unexpected message");
            }




        }

        if (m_timer.getElapsedTimeInSeconds() > m_timeout)
        {
            for (bdUInt i = 0; i < m_listeners.getSize(); ++i)
            {
                m_listeners[i]->onDiscoveryFinished();
            }
            m_status = BD_IDLE;
            m_socket.close();
        }
    }
    else
    {
        bdLogWarn("lan discovery", "update called while not discovering");
    }
}

bdLANDiscoveryClient::bdStatus bdLANDiscoveryClient::getStatus() const
{
    return m_status;
}

void bdLANDiscoveryClient::registerListener(bdLANDiscoveryListener * listener)
{
    m_listeners.pushBack(listener);
}

void bdLANDiscoveryClient::unregisterListener(bdLANDiscoveryListener * listener)
{
    m_listeners.removeAll(listener);
}

inline void bdLANDiscoveryClient::fireOnDiscoveryListeners(bdGameInfoRef gameInfo) const
{
    for (bdUInt i = 0; i < m_listeners.getSize(); ++i)
    {
        m_listeners[i]->onDiscovery(gameInfo);
    }
}
