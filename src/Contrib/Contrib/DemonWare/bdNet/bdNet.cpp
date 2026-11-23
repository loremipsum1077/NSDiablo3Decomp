// Original file: Contrib/Contrib/DemonWare/bdNet/bdNet.cpp
#include "bdNet.h"
#include "bdGetHostByName/bdGetHostByName.h"
#include "../bdCore/bdSocket/bdSocket.h"
#include "../bdCore/bdTiming/bdStopwatch.h"
#include "../bdCore/bdUtilities/bdRandom.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include "../bdPlatform/bdPlatformTiming/bdPlatformTiming.h"
#include "../bdSocket/bdNAT/bdIPDiscoveryClient.h"
#include "../bdSocket/bdNAT/bdNATTypeDiscoveryClient.h"
#include "../bdSocket/bdSocketRouterAdHoc-switch.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Api.h"
#include "../../../../NintendoSDK/Include/nn/account/account_ApiNetworkServiceAccount.h"
#include "../../../../NintendoSDK/Include/nn/result/result_ErrorResult.h"
// Original SDK call at bdNet.cpp:50; declaration omitted from the adopted SDK subset.
namespace nn {
namespace ldn {
Result Initialize();
}
}
// Original class allocator bodies (bdGetHostByName.h:31, bdIPDiscoveryClient.h:19,
// bdNATTypeDiscoveryClient.h:29); no inline hint added.
inline void * bdGetHostByName::operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
inline void * bdIPDiscoveryClient::operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
inline void * bdNATTypeDiscoveryClient::operator new(size_t nbytes) { return bdMemory::allocate(nbytes); }  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
// Existing instantiation owned by X/Client/Console/Online.cpp (0x4dad0). Original pump
// calls it out of line at 0xb495ac/0xb49650; bdSingleton.h:42 declares this exact signature.
extern template bdNetImpl * bdSingleton<bdNetImpl>::getInstance();
template <> bdNetImpl * bdSingleton<bdNetImpl>::m_instance = NULL;  // line 31

// Release log text is absent; retain only the surviving bdUseVAArgs calls.
bdBool bdNetImpl::start(const bdNetStartParams & params)
{
    bdBool success = true;

    if (m_status != BD_NET_STOPPED)
    {
        bdUseVAArgs();
        success = false;
    }
    else
    {
        m_params = params;



        if (bdPlatformSocket::getAdHoc())
        {
            nn::Result result = nn::ldn::Initialize();
            // DWARF: ErrorRange<203,16,32>::Includes inlined at line 52.
            if (nn::result::detail::ErrorRange<203, 16, 32>::Includes(result))
            {
                nn::err::ShowError(result);
            }
        }


        bdAddr addr;
        const bdBool getBindAddrResult = getBindAddr(addr);

        if (!getBindAddrResult)
        {
            bdUseVAArgs();
            m_status = BD_NET_PARAMS_CONFIG_ERROR;
        }
        else
        {

            success = bindRandPort(m_params, addr);
            if (success)
            {










                if (bdPlatformSocket::getAdHoc())
                {
                    m_socketRouter = new bdSocketRouterAdHoc();
                }
                else
                {
                    m_socketRouter = new bdSocketRouter();
                }





                if (!bdPlatformSocket::getAdHoc() && m_params.m_onlineGame)
                {
                    m_status = BD_NET_PENDING;
                    m_currentNatTravAddrIndex = 0;
                    m_currentNatTravHostIndex = 0;

                    m_getHostByName = new bdGetHostByName(params.m_threadStackSize);
                    const bdBool getHostByNameStartResult = m_getHostByName->start(m_params.m_natTravHosts[m_currentNatTravHostIndex].getBuffer(),
                                                                                   m_params.m_hostNameLookupConfig);

                    if (!getHostByNameStartResult)
                    {
                        bdUseVAArgs();
                        m_status = BD_NET_INIT_FAILED;
                        success = false;
                    }
                    else
                    {

                        const bdBool uPnPInitResult = m_UPnP.init(m_params.m_gamePort, &m_params.m_localAddresses,
                                                                  m_params.m_UPnPConfig);
                        if (!uPnPInitResult)
                        {
                            bdUseVAArgs();
                        }
                    }
                }
                else
                {
                    m_status = BD_NET_DONE;


                    if (m_params.m_localAddresses.isEmpty())
                    {
                        bdUseVAArgs();
                        return false;
                    }

                    bdArray<bdAddr> localAddresses;
                    for (bdUInt i = 0; i < m_params.m_localAddresses.getSize(); ++i)
                    {
                        const bdAddr local(m_params.m_localAddresses[i], m_params.m_gamePort);
                        localAddresses.pushBack(local);
                    }


                    bdAddr remAddr;


                    bdCommonAddrRef localCommonAddr = new bdCommonAddr(localAddresses, remAddr, BD_NAT_OPEN);


                    m_socketRouter->init(m_params.m_socket, localCommonAddr, m_params.m_socketRouterConfig);


                    m_connectionStore.init(m_socketRouter, m_params.m_connectionStoreConfig);
                }
            }
            else
            {
                bdUseVAArgs();
                m_status = BD_NET_BIND_FAILED;
            }
        }
    }
    return success;
}
// NON_MATCHING: 3656 vs 3652. The findFirst block (264-272) now has the original's instructions (log duplicated into both branches, pushBack only when not found); what remains is register/stack layout: the original spills numNATHosts (180) to [sp, #28] and its frame is 0x10 larger (0x8b0 vs 0x8a0).
void bdNetImpl::pump()
{










    if (!bdPlatformSocket::getAdHoc() && m_params.m_onlineGame && m_status == BD_NET_PENDING)
    {
        bdBool finishedIPLookup = false;
        bdBool finishedNATLookup = false;
        const bdUInt numNATHosts = m_params.m_natTravHosts.getSize();
        bdBool finishedNATHostLookup = m_currentNatTravHostIndex == numNATHosts;

        if (m_UPnP.isRunning())
        {
            m_UPnP.pump();
        }
        else if (m_UPnP.getState() == bdUPnP::BD_UPNP_FINISHED &&
                 m_UPnP.getPortStatus() == bdUPnPDevice::BD_UPNP_PORT_COLLISION_FOUND)
        {



            ++m_upnpCollisionRetryCount;
            if (m_upnpCollisionRetryCount > 20)
            {

                m_UPnP.startShutdown(bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE);
                bdUseVAArgs(m_upnpCollisionRetryCount);

                bdNetStartParams params(m_params);

                params.m_socket = NULL;




                params.m_gamePort = 3074;
                params.m_UPnPConfig.m_runMode = bdUPnPConfig::BD_UPNP_EXTERNAL_IP_ONLY;

                bdSingleton<bdNetImpl>::getInstance()->stop();

                start(params);
            }
            else
            {
                bdNetStartParams params(m_params);




                params.m_socket = NULL;





                ++params.m_gamePort;


                bdUseVAArgs(params.m_gamePort);

                bdSingleton<bdNetImpl>::getInstance()->stop();

                start(params);
            }
        }





        if (m_getHostByName)
        {
            bdGetHostByName::bdStatus status = m_getHostByName->getStatus();
            if (status == bdGetHostByName::BD_LOOKUP_PENDING)
            {
                m_getHostByName->pump();
            }
            else if (status == bdGetHostByName::BD_LOOKUP_SUCCEEDED)
            {
                bdUInt numAddrs = m_getHostByName->getNumAddresses();

                for (bdUInt j = 0; j < numAddrs; ++j)
                {
                    const bdInetAddr inAddr = m_getHostByName->getAddressAt(j).inUn.m_iaddr;
                    const bdAddr addr(inAddr, m_params.m_natTravPort);




                    bdUInt index = 0;
                    bdNChar8 addrBuffer[22];
                    addr.toString(addrBuffer, sizeof(addrBuffer));
                    if (!m_natTravAddrs.findFirst(addr, index))
                    {
                        bdUseVAArgs(addrBuffer, m_params.m_natTravHosts[m_currentNatTravHostIndex].getBuffer());
                        m_natTravAddrs.pushBack(addr);
                    }
                    else
                    {
                        bdUseVAArgs(addrBuffer, m_params.m_natTravHosts[m_currentNatTravHostIndex].getBuffer());
                    }
                }
                ++m_currentNatTravHostIndex;
            }
            else
            {

                bdUseVAArgs(m_params.m_natTravHosts[m_currentNatTravHostIndex].getBuffer());
                ++m_currentNatTravHostIndex;
            }


            if (status != bdGetHostByName::BD_LOOKUP_PENDING && m_currentNatTravHostIndex < numNATHosts)
            {

                m_getHostByName->quit();
                const bdBool getHostByNameStartResult = m_getHostByName->start(m_params.m_natTravHosts[m_currentNatTravHostIndex].getBuffer(), m_params.m_hostNameLookupConfig);
                if (!getHostByNameStartResult)
                {
                    bdUseVAArgs();
                    m_status = BD_NET_INIT_FAILED;
                }
            }
            if (m_currentNatTravHostIndex == numNATHosts)
            {

                m_getHostByName->quit();
                delete m_getHostByName;
                m_getHostByName = NULL;
            }
        }



        bdBool discIP;
        if (m_getHostByName == NULL &&
            !m_UPnP.isRunning() &&
            m_UPnP.getPortStatus() != bdUPnPDevice::BD_UPNP_PORT_COLLISION_FOUND &&
            m_ipDiscClient == NULL &&
            m_natTypeDiscClient == NULL)
        {
            discIP = m_params.m_overridePublicAddr == bdAddr();
            if (discIP)
            {
                if (!m_natTravAddrs.isEmpty())
                {
                    bdAddr addr(m_natTravAddrs[m_currentNatTravAddrIndex]);

                    m_ipDiscClient = new bdIPDiscoveryClient();
                    const bdBool startIPDiscoveryResult = m_ipDiscClient->init(m_params.m_socket, addr,
                                                                               m_params.m_IPDiscoveryConfig);

                    m_natTypeDiscClient = new bdNATTypeDiscoveryClient();
                    const bdBool startNATDiscoveryResult = m_natTypeDiscClient->init(m_params.m_socket, addr, bdNATTypeDiscoveryConfig());


                    if (!startIPDiscoveryResult)
                    {
                        bdUseVAArgs();
                        m_status = BD_NET_ONLINE_FAILED;
                    }
                    if (!startNATDiscoveryResult)
                    {
                        bdUseVAArgs();
                        m_status = BD_NET_ONLINE_FAILED;
                    }
                }
                else
                {
                    for (bdUInt i = 0; i < m_params.m_natTravHosts.getSize(); ++i)
                    {
                        bdUseVAArgs(m_params.m_natTravHosts[i].getBuffer());
                    }
                    m_status = BD_NET_ONLINE_FAILED;
                }
            }
            else
            {




                bdArray<bdAddr> localAddresses;
                for (bdUInt i = 0; i < m_params.m_localAddresses.getSize(); ++i)
                {
                    const bdAddr local(m_params.m_localAddresses[i], m_params.m_gamePort);
                    localAddresses.pushBack(local);
                }


                bdAddr publicAddr(m_params.m_overridePublicAddr);
                bdNChar8 publicAddrStr[22];
                publicAddr.toString(publicAddrStr, sizeof(publicAddrStr));

                bdUseVAArgs(publicAddrStr);

                bdAddr UPnPExternal;
                if (m_UPnP.getExternalAddr(UPnPExternal))
                {
                    if (UPnPExternal == publicAddr)
                    {
                        bdUseVAArgs();
                    }
                    else
                    {
                        bdUseVAArgs();
                    }
                }


                bdCommonAddrRef localCommonAddr = new bdCommonAddr(localAddresses,
                                                                   publicAddr,
                                                                   m_params.m_overrideNATType);


                m_socketRouter->init(m_params.m_socket, localCommonAddr, m_params.m_socketRouterConfig);


                m_connectionStore.init(m_socketRouter, m_params.m_connectionStoreConfig);

                m_status = BD_NET_DONE;
            }
        }


        if (m_status == BD_NET_PENDING) {
            const bdUInt dataSize = 1288;
            bdUByte8 data[dataSize];
            bdAddr fromAddr;
            bdInt received = m_params.m_socket->receiveFrom(fromAddr, data, dataSize);

            if (m_ipDiscClient)
            {
                m_ipDiscClient->pump(fromAddr, data, received);
                finishedIPLookup = m_ipDiscClient->getStatus() != bdIPDiscoveryClient::BD_IP_DISC_RUNNING;
            }




            if (m_natTypeDiscClient)
            {
                m_natTypeDiscClient->pump(fromAddr, data, received);
                finishedNATLookup = !m_natTypeDiscClient->isRunning();
            }




            if (finishedNATHostLookup && finishedIPLookup && finishedNATLookup)
            {



                if (m_ipDiscClient->getStatus() == bdIPDiscoveryClient::BD_IP_DISC_SUCCESS &&
                    m_natTypeDiscClient->getStatus() == bdNATTypeDiscoveryClient::BD_NTDCS_FINI)
                {

                    bdArray<bdAddr> localAddresses;
                    for (bdUInt i = 0; i < m_params.m_localAddresses.getSize(); ++i)
                    {
                        const bdAddr local(m_params.m_localAddresses[i], m_params.m_gamePort);
                        localAddresses.pushBack(local);
                    }


                    bdAddr publicAddr(m_ipDiscClient->getPublicAddress());

                    bdNATType clientNAT = m_natTypeDiscClient->getNATType();
                    bdAddr UPnPExternal;
                    if (m_UPnP.getExternalAddr(UPnPExternal))
                    {
                        if (UPnPExternal == publicAddr)
                        {
                            bdUseVAArgs();
                        }
                        else
                        {
                            bdUseVAArgs();
                        }
                    }


                    bdCommonAddrRef localCommonAddr = new bdCommonAddr(localAddresses, publicAddr, clientNAT);




                    if (m_ipDiscClient)
                    {
                        m_ipDiscClient->quit();
                        delete m_ipDiscClient;
                        m_ipDiscClient = NULL;
                    }

                    if (m_natTypeDiscClient)
                    {
                        m_natTypeDiscClient->quit();
                        delete m_natTypeDiscClient;
                        m_natTypeDiscClient = NULL;
                    }

                    bdArray<bdAddr> introducers(m_natTravAddrs);




                    if (m_currentNatTravAddrIndex != 0)
                    {
                        bdAddr tmpAddr(m_natTravAddrs[0]);
                        introducers[0] = m_natTravAddrs[m_currentNatTravAddrIndex];
                        introducers[m_currentNatTravAddrIndex] = tmpAddr;
                    }


                    m_socketRouter->init(m_params.m_socket, localCommonAddr, m_params.m_socketRouterConfig);

                    m_socketRouter->setupIntroducers(introducers);


                    m_connectionStore.init(m_socketRouter, m_params.m_connectionStoreConfig);

                    m_status = BD_NET_DONE;
                }
                else
                {
                    if (m_currentNatTravAddrIndex + 1 < m_natTravAddrs.getSize())
                    {
                        bdUseVAArgs();

                        ++m_currentNatTravAddrIndex;
                        bdAddr addr(m_natTravAddrs[m_currentNatTravAddrIndex]);

                        if (m_ipDiscClient)
                        {
                            m_ipDiscClient->quit();
                            delete m_ipDiscClient;
                            m_ipDiscClient = NULL;
                        }
                        m_ipDiscClient = new bdIPDiscoveryClient();
                        const bdBool startIPDiscoveryResult = m_ipDiscClient->init(m_params.m_socket, addr, bdIPDiscoveryConfig());

                        if (m_natTypeDiscClient)
                        {
                            m_natTypeDiscClient->quit();
                            delete m_natTypeDiscClient;
                            m_natTypeDiscClient = NULL;
                        }

                        m_natTypeDiscClient = new bdNATTypeDiscoveryClient();
                        const bdBool startNATDiscoveryResult = m_natTypeDiscClient->init(m_params.m_socket, addr, bdNATTypeDiscoveryConfig());

                        if (!startIPDiscoveryResult)
                        {
                            bdUseVAArgs();
                            m_status = BD_NET_ONLINE_FAILED;
                        }
                        if (!startNATDiscoveryResult)
                        {
                            bdUseVAArgs();
                            m_status = BD_NET_ONLINE_FAILED;
                        }
                    }
                    else
                    {
                        for (bdUInt i = 0; i < m_params.m_natTravHosts.getSize(); ++i)
                        {
                            bdUseVAArgs(m_params.m_natTravHosts[i].getBuffer());
                        }
                        m_status = BD_NET_ONLINE_FAILED;
                    }
                }
            }
        }
    }
}

void bdNetImpl::stop()
{

    if (m_UPnP.getState() != bdUPnP::BD_UPNP_UNINITIALISED)
    {
        m_UPnP.startShutdown(bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_ELEGANT);
    }


    m_connectionStore.startShutdown(bdConnectionStore::BD_CONNECTION_STORE_SHUTDOWN_ELEGANT);


    bdStopwatch timer;
    timer.start();

    bdBool waiting = true;

    while (waiting)
    {
        waiting = false;
        if (m_connectionStore.getStatus() != bdConnectionStore::BD_CONNECTION_STORE_UNINITIALIZED)
        {
            receiveAll();
            dispatchAll();
            sendAll();
            waiting = true;
        }

        if (m_UPnP.getState() != bdUPnP::BD_UPNP_UNINITIALISED)
        {


            m_UPnP.pump();
            waiting = true;
        }
        if (timer.getElapsedTimeInSeconds() > 1.0f)
        {


            m_connectionStore.startShutdown(bdConnectionStore::BD_CONNECTION_STORE_SHUTDOWN_IMMEDIATE);
            m_UPnP.startShutdown(bdUPnPDevice::BD_UPNP_DEVICE_SHUTDOWN_IMMEDIATE);
            waiting = false;
        }
        else if (waiting)
            bdPlatformTiming::sleep(20);
    }


    if (m_socketRouter)
    {
        m_socketRouter->quit();
        delete m_socketRouter;
        m_socketRouter = NULL;
    }

    if (m_params.m_socket)
    {
        const bdBool socketCloseResult = m_params.m_socket->close();
        if (!socketCloseResult)
        {
            bdLogWarn("bdNet/net", "Failed to close socket.");
        }
        delete m_params.m_socket;
        m_params.m_socket = NULL;
    }

    if (m_ipDiscClient)
    {
        m_ipDiscClient->quit();
        delete m_ipDiscClient;
        m_ipDiscClient = NULL;
    }

    if (m_natTypeDiscClient)
    {
        m_natTypeDiscClient->quit();
        delete m_natTypeDiscClient;
        m_natTypeDiscClient = NULL;
    }

    if (m_getHostByName)
    {


        m_getHostByName->quit();
        delete m_getHostByName;
        m_getHostByName = NULL;
    }

    m_status = BD_NET_STOPPED;
    m_currentNatTravAddrIndex = 0;
    m_currentNatTravHostIndex = 0;
    m_params.m_localAddresses.clear();
    m_params.m_natTravHosts.clear();
    m_natTravAddrs.clear();
}

bdNetImpl::bdNetStatus bdNetImpl::getStatus() const
{
    return m_status;
}

bdBool bdNetImpl::sendAll()
{
    const bdBool result = m_connectionStore.flushAll();
    if (!result)
    {
        bdLogWarn("bdNet/net", "Failed to flush all connections.");
    }
    return result;
}

bdBool bdNetImpl::receiveAll()
{
    const bdBool result = m_connectionStore.receiveAll();
    if (!result)
    {
        bdLogWarn("bdNet/net", "Failed to receive on all connections.");
    }
    return result;
}

void bdNetImpl::dispatchAll()
{
    m_connectionStore.dispatchAll();
}

bdConnectionStore * bdNetImpl::getConnectionStore()
{
    return &m_connectionStore;
}

bdCommonAddrRef bdNetImpl::getLocalCommonAddr() const
{
    bdCommonAddrRef local;
    if (m_socketRouter)
    {
        local = m_socketRouter->getLocalCommonAddr();
    }
    return local;
}






bdNetImpl::bdNetImpl()
    : m_socketRouter(NULL),
      m_status(BD_NET_STOPPED),
      m_ipDiscClient(NULL),
      m_natTypeDiscClient(NULL),
      m_getHostByName(NULL),
      m_currentNatTravAddrIndex(0),
      m_currentNatTravHostIndex(0),
      m_upnpCollisionRetryCount(0)
{



}

bdNetImpl::~bdNetImpl()
{
    if (m_getHostByName)
    {


        m_getHostByName->quit();
        delete m_getHostByName;
        m_getHostByName = NULL;
    }
    if (m_natTypeDiscClient)
    {
        m_natTypeDiscClient->quit();
        delete m_natTypeDiscClient;
        m_natTypeDiscClient = NULL;
    }

    if (m_ipDiscClient)
    {
        m_ipDiscClient->quit();
        delete m_ipDiscClient;
        m_ipDiscClient = NULL;
    }

    if (m_socketRouter)
    {
        m_socketRouter->quit();
        delete m_socketRouter;
        m_socketRouter = NULL;
    }
}


bdBool bdNetImpl::bindRandPort(bdNetStartParams & params, bdAddr & addr)
{
    const bdPort basePort = addr.getPort();
    bdPort port = basePort;
    bdBool success = false;
    const bdBool blocking = false;

    if (!params.m_socket)
    {


        params.m_socket = new bdSocket();
    }


    for (bdUInt i = 0; i < 100; ++i)
    {
        const bdBool socketCreateResult = params.m_socket->create(blocking, true, true);
        if (!socketCreateResult)
        {
            bdLogError("bdNet/net", "Failed to create socket.");
            break;
        }


        bdAddr tryAddr(addr.getAddress(), port);
        const bdSocketStatusCode bindResult = params.m_socket->bind(tryAddr);
        if (bindResult == BD_NET_SUCCESS)
        {
            params.m_gamePort = tryAddr.getPort();
            bdLogInfo("bdNet/net", "Bound to port %u (base %u).", port, params.m_gamePort);
            success = true;
            break;
        }
        else if (bindResult == BD_NET_ADDRESS_IN_USE)
        {



            const bdBool socketCloseResult = params.m_socket->close();
            if (socketCloseResult)
            {
                bdLogInfo("bdNet/net", "Port in use, trying another.");
            }
        }
        else
        {
            const bdBool socketCloseResult = params.m_socket->close();
            if (!socketCloseResult)
            {
                bdLogWarn("bdNet/net", "Failed to close socket.");
            }
        }
        port = basePort + (bdRandom().nextUInt() % 100);
    }

    return success;
}

bdBool bdNetImpl::getBindAddr(bdAddr & addr)
{
    bdBool result = true;

    if (false == m_params.m_useAnyIP)
    {
        if (m_params.m_localAddresses.getSize() > 1)
        {
            bdLogWarn("bdNet/net", "More than one local address, using the first.");
        }

        if (m_params.m_localAddresses.getSize() != 0)
        {
            addr = bdAddr(m_params.m_localAddresses[0], m_params.m_gamePort);
        }
        else
        {

            bdLogError("bdNet/net", "No local address to bind to.");
            result = false;
        }
    }
    else
    {
        addr = bdAddr(bdInetAddr::Any(), m_params.m_gamePort);
    }

    return result;
}

bdSocketRouter * bdNetImpl::getSocketRouter()
{
    return m_socketRouter;
}











bdSocketRouterAdHoc * bdNetImpl::getSocketRouterAdHoc()
{
    if (bdPlatformSocket::getAdHoc())
    {
        return static_cast<bdSocketRouterAdHoc *>(m_socketRouter);
    }



    return NULL;
}
