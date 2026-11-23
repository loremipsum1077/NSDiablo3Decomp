// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSocketRouterAdHoc-switch.cpp
// Reconstructed from original DWARF, source-line and instruction evidence.

#include "bdSocketRouterAdHoc-switch.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Api.h"

bdSocketRouterAdHoc::bdSocketRouterAdHoc()
    : bdSocketRouter(), m_nodeID(8), m_secID(), m_isHost(false)
{
    bdMemset(&m_networkInfo, 0, sizeof(m_networkInfo));
}

bdSocketRouterAdHoc::~bdSocketRouterAdHoc()
{
}

bdBool bdSocketRouterAdHoc::init(bdDefaultSocket * socket, const bdCommonAddrRef localCommonAddr, const bdSocketRouterConfig & config)
{
    bdBool success = false;
    if (nn::ldn::GetState() == nn::ldn::State_Initialized)
    {
        success = bdSocketRouter::init(socket, localCommonAddr, config);
    }
    else
    {
        bdLogError("socket router", "LDN library is not initialized.");
    }
    return success;
}

void bdSocketRouterAdHoc::pump()
{
    bdSocketRouter::pump();
    processLdnState();
}

bdBool bdSocketRouterAdHoc::quit()
{
    bdBool success = false;
    if (m_status == BD_SOCKET_ROUTER_UNINITIALIZED)
    {
        bdLogWarn("socket router", "Already uninitialized.");
        return success;
    }
    success = leaveNetwork();
    if (success)
    {
        success = bdSocketRouter::quit();
    }
    if (nn::ldn::Disconnect().IsFailure())
    {
        success = false;
    }
    nn::ldn::Finalize();
    m_secID = bdSecurityID();
    return success;
}

bdBool bdSocketRouterAdHoc::leaveNetwork()
{
    bdBool success = false;
    const nn::ldn::State ldnState = nn::ldn::GetState();
    if (m_isHost)
    {
        if (ldnState == nn::ldn::State_AccessPoint || ldnState == nn::ldn::State_AccessPointCreated)
        {
            if (nn::ldn::DestroyNetwork().IsSuccess())
            {
                success = nn::ldn::CloseAccessPoint().IsSuccess();
            }
        }
    }
    else
    {
        if (ldnState == nn::ldn::State_Station || ldnState == nn::ldn::State_StationConnected)
        {
            success = nn::ldn::CloseStation().IsSuccess();
        }
    }
    bdMemset(&m_networkInfo, 0, sizeof(m_networkInfo));
    m_isHost = false;
    m_nodeID = 8;
    return success;
}

void bdSocketRouterAdHoc::processLdnState()
{
    const nn::ldn::State ldnState = nn::ldn::GetState();
    switch (ldnState)
    {
    case nn::ldn::State_AccessPoint:
    case nn::ldn::State_AccessPointCreated:
    case nn::ldn::State_Station:
    case nn::ldn::State_StationConnected:
        processNetworkNodeUpdates();
        break;
    case nn::ldn::State_Error:
        leaveNetwork();
        updateLocalCommonAddr(0);
        m_status = BD_SOCKET_ROUTER_ERROR;
        bdLogError("socket router", "LDN entered the error state.");
        break;
    default:
        break;
    }
}

#include "../bdCore/bdSocket/bdCommonAddrInfo.h"
#include "../bdCore/bdSocket/bdSecurityInfo.h"
#include "bdDTLS/bdDTLSAssociation.h"
#include "../../../../NintendoSDK/Include/nn/socket/socket_Api.h"

// SDK declarations: exact imported parameter signatures; layouts from this CU's DWARF.
namespace nn {
namespace err {
void ShowError(Result result);
}
namespace ldn {
enum ConnectOption { ConnectOption_None = 0 };
enum NodeStateChange {
    NodeStateChange_None = 0,
    NodeStateChange_Connect = 1,
    NodeStateChange_Disconnect = 2,
    NodeStateChange_DisconnectAndConnect = 3
};
struct SubnetMask {
    Bit32 raw;
};
struct NodeLatestUpdate {
    int8_t stateChange;
    Bit8 _reserved[7];
};
Result OpenStation();
Result OpenAccessPoint();
Result Connect(const NetworkInfo &, const SecurityConfig &, const UserConfig &, int, ConnectOption);
Result CreateNetwork(const NetworkConfig &, const SecurityConfig &, const UserConfig &);
Result GetIpv4Address(Ipv4Address *, SubnetMask *);
Result GetNetworkInfo(NetworkInfo *, NodeLatestUpdate *, int);
Result SetAdvertiseData(const void *, size_t);
Result Scan(NetworkInfo *, int *, int, const ScanFilter &, int);
bool operator==(Ipv4Address, Ipv4Address);
}
}

bdBool bdSocketRouterAdHoc::scan(nn::ldn::NetworkInfo * networkInfoBuffer, const bdUInt networkInfoBufferSize, bdInt & numNetworkInfoFound, const nn::ldn::ScanFilter filter, const bdUInt channel)
{
    bdBool success = false;
    numNetworkInfoFound = 0;
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED)
    {
        nn::Result scanResult = nn::ldn::Scan(networkInfoBuffer, &numNetworkInfoFound, networkInfoBufferSize, filter, channel);
        if (scanResult.IsSuccess())
        {
            bdUseVAArgs(numNetworkInfoFound);
            success = true;
            for (bdUInt i = 0; i < networkInfoBufferSize && i < numNetworkInfoFound; ++i)
            {
                bdUseVAArgs(i);
                bdUseVAArgs(networkInfoBuffer[i].common.ssid.raw);
                bdUseVAArgs(networkInfoBuffer[i].common.channel);
                bdUseVAArgs(networkInfoBuffer[i].common.linkLevel);
                bdUseVAArgs(networkInfoBuffer[i].ldn.nodeCount);
                bdUseVAArgs(networkInfoBuffer[i].ldn.nodeCountMax);
                bdUseVAArgs(networkInfoBuffer[i].ldn.advertiseData);
            }
        }
        else
        {
            nn::err::ShowError(scanResult);
            bdUseVAArgs(scanResult.GetDescription(), scanResult.GetInnerValueForDebug());
        }
    }
    return success;
}

bdBool bdSocketRouterAdHoc::updateAdvertiseData(const bdNChar8 *const customData, size_t customDataSize, bdSecurityID secId, bdSecurityKey secKey)
{
    bdAdvertiseData advertiseData = {};
    advertiseData.m_secID = secId;
    advertiseData.m_secKey = secKey;
    bdMemcpy(advertiseData.m_customData, customData, customDataSize);
    nn::Result temporaryResult = nn::ldn::SetAdvertiseData(&advertiseData, sizeof(advertiseData));
    if (temporaryResult.IsFailure())
    {
        nn::err::ShowError(temporaryResult);
        return false;
    }
    return true;
}

bdBool bdSocketRouterAdHoc::updateLocalCommonAddr(const bdUInt ipAddress)
{
    bdPort localPort = m_localCommonAddr->getLocalAddrByIndex(0).getPort();
    bdAddr addr(bdInetAddr(ipAddress), localPort);
    bdArray<bdAddr> LocalAddresses;
    LocalAddresses.pushBack(addr);
    bdCommonAddrRef newLocalCommonAddr = new bdCommonAddr(LocalAddresses, addr, BD_NAT_OPEN);
    return bdSocketRouter::updateLocalCommonAddr(newLocalCommonAddr);
}

bdBool bdSocketRouterAdHoc::createNetwork(const nn::ldn::NetworkConfig networkConfig, const nn::ldn::SecurityConfig securityConfig, const nn::ldn::UserConfig userConfig, const bdNChar8 *const customData, size_t customDataSize, bdSecurityID secId, bdSecurityKey secKey)
{
    bdBool success = false;
    if (secId == bdSecurityID())
    {
        secKey = bdSecurityKey();
        bdSecurityKeyMap::initKey(m_secID, secKey);
        if (!m_keyStore.contains(m_secID))
        {
            m_keyStore.registerKey(m_secID, secKey);
        }
    }
    m_secID = secId;
    bdAdvertiseData advertiseData = {};
    advertiseData.m_secID = secId;
    advertiseData.m_secKey = secKey;
    bdMemcpy(advertiseData.m_customData, customData, customDataSize);
    nn::Result temporaryResult = nn::ldn::OpenAccessPoint();
    if (temporaryResult.IsSuccess())
    {
        temporaryResult = nn::ldn::SetAdvertiseData(&advertiseData, sizeof(advertiseData));
        if (temporaryResult.IsSuccess())
        {
            temporaryResult = nn::ldn::CreateNetwork(networkConfig, securityConfig, userConfig);
        }
    }
    if (temporaryResult.IsFailure())
    {
        nn::err::ShowError(temporaryResult);
        m_status = BD_SOCKET_ROUTER_ERROR;
    }
    else
    {
        nn::ldn::Ipv4Address ipAddr;
        nn::ldn::SubnetMask netMask;
        if (nn::ldn::GetIpv4Address(&ipAddr, &netMask).IsSuccess())
        {
            bdAddr addr(bdInetAddr(nn::socket::InetHtonl(ipAddr.raw)), m_localCommonAddr->getLocalAddrByIndex(0).getPort());
            bdArray<bdAddr> LocalAddresses;
            LocalAddresses.pushBack(addr);
            bdCommonAddrRef newLocalAddr = new bdCommonAddr(LocalAddresses, addr, BD_NAT_OPEN);
            m_localCommonAddr = newLocalAddr;
            const bdBool blocking = false;
            const bdBool broadcast = true;
            success = m_socket->close() && m_socket->create(blocking, broadcast, true) && m_socket->bind(addr) == 1;
            m_isHost = success;
        }
    }
    return success;
}

bdBool bdSocketRouterAdHoc::connectNetwork(nn::ldn::NetworkInfo & networkInfo, const nn::ldn::SecurityConfig securityConfig, const nn::ldn::UserConfig userConfig, bdAddrHandleRef & addrHandle, bdUInt communicationVersion, bool registerSecurityKey, bool hideErrors)
{
    bdBool success = false;
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED)
    {
        if (hideErrors)
        {
            if (nn::ldn::OpenStation().IsFailure())
            {
                return false;
            }
            if (nn::ldn::Connect(networkInfo, securityConfig, userConfig, communicationVersion, nn::ldn::ConnectOption_None).IsFailure())
            {
                return false;
            }
        }
        else
        {
            {
                nn::Result temporaryResult = nn::ldn::OpenStation();
                if (temporaryResult.IsFailure())
                {
                    nn::err::ShowError(temporaryResult);
                    return false;
                }
            }
            {
                nn::Result temporaryResult = nn::ldn::Connect(networkInfo, securityConfig, userConfig, communicationVersion, nn::ldn::ConnectOption_None);
                if (temporaryResult.IsFailure())
                {
                    nn::err::ShowError(temporaryResult);
                    return false;
                }
            }
        }
        bdAdvertiseData * advertisedata = reinterpret_cast<bdAdvertiseData *>(networkInfo.ldn.advertiseData);
        if (registerSecurityKey && !m_keyStore.contains(advertisedata->m_secID))
        {
            m_keyStore.registerKey(advertisedata->m_secID, advertisedata->m_secKey);
            m_secID = advertisedata->m_secID;
        }
        const nn::ldn::NodeInfo & accessPointHost = networkInfo.ldn.nodes[0];
        bdAddr accessPointHostAddr(bdInetAddr(nn::socket::InetHtonl(accessPointHost.ipv4Address.raw)), m_localCommonAddr->getLocalAddrByIndex(0).getPort());
        bdCommonAddrRef accessPointHostCommonAddr = new bdCommonAddr(accessPointHostAddr);
        if (m_addrMap.getAddrHandle(accessPointHostCommonAddr, advertisedata->m_secID, addrHandle))
        {
            addrHandle->setRealAddr(accessPointHostAddr);
            bdUseVAArgs(accessPointHost.userName);
        }
        nn::ldn::Ipv4Address ipAddr;
        nn::ldn::SubnetMask netMask;
        if (nn::ldn::GetIpv4Address(&ipAddr, &netMask).IsSuccess())
        {
            bdAddr addr(bdInetAddr(nn::socket::InetHtonl(ipAddr.raw)), m_localCommonAddr->getLocalAddrByIndex(0).getPort());
            bdArray<bdAddr> LocalAddresses;
            LocalAddresses.pushBack(addr);
            bdCommonAddrRef newLocalAddr = new bdCommonAddr(LocalAddresses, addr, BD_NAT_OPEN);
            m_localCommonAddr = newLocalAddr;
            const bdBool blocking = false;
            const bdBool broadcast = true;
            success = m_socket->close() && m_socket->create(blocking, broadcast, true) && m_socket->bind(addr) == 1;
        }
    }
    else
    {
        bdUseVAArgs();
    }
    return success;
}

bdBool bdSocketRouterAdHoc::connect(bdAddrHandleRef & addrHandle)
{
    bdBool success = false;
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED && !addrHandle.isNull())
    {
        bdCommonAddrRef remote = addrHandle->m_endpoint.getCommonAddr();
        bdNChar8 commonAddrInfo[1024];
        bdCommonAddrInfo::getInfo(remote, commonAddrInfo, sizeof(commonAddrInfo));
        bdNChar8 secIDString[18];
        bdSecurityInfo::toString(addrHandle->m_endpoint.getSecID(), secIDString, sizeof(secIDString));
        bdUseVAArgs(commonAddrInfo);
        bdUseVAArgs(secIDString);
        if (!m_keyStore.contains(addrHandle->m_endpoint.getSecID()))
        {
            bdUseVAArgs();
        }
        bdEndpoint endpoint(addrHandle->m_endpoint.getCommonAddr(), addrHandle->m_endpoint.getSecID());
        bdAddr realAddr = endpoint.getCommonAddr()->getLocalAddrByIndex(0);
        addrHandle->setRealAddr(realAddr);
        bdNChar8 addrString[22];
        realAddr.toString(addrString, sizeof(addrString));
        bdDTLSAssociation * dtlsSocket;
        if (!m_dtls.get(endpoint, dtlsSocket))
        {
            bdUseVAArgs(addrString);
            dtlsSocket = new bdDTLSAssociation(m_socket, &m_keyStore, &m_ECCKey, realAddr, addrHandle, m_localCommonAddr, &m_addrMap, m_config.m_DTLSAssociationReceiveTimeout, m_config.m_cypherSuite);
            dtlsSocket->connect();
            success = m_dtls.put(endpoint, dtlsSocket);
            if (!success)
            {
                bdNChar8 addrInfo[1024];
                bdCommonAddrInfo::getInfo(endpoint.getCommonAddr(), addrInfo, sizeof(addrInfo));
                bdNChar8 secIDString[18];
                bdSecurityInfo::toString(endpoint.getSecID(), secIDString, sizeof(secIDString));
                bdUseVAArgs(secIDString);
                bdUseVAArgs(addrInfo);
                delete dtlsSocket;
            }
        }
        else
        {
            bdUseVAArgs(addrString);
            success = true;
        }
    }
    else
    {
        bdUseVAArgs();
    }
    return success;
}

void bdSocketRouterAdHoc::processNetworkNodeUpdates()
{
    nn::ldn::NodeLatestUpdate updates[8];
    if (nn::ldn::GetNetworkInfo(&m_networkInfo, updates, 8).IsSuccess())
    {
        if (m_nodeID == 8)
        {
            nn::ldn::Ipv4Address ipAddr = {};
            nn::ldn::SubnetMask netMask = {};
            if (nn::ldn::GetIpv4Address(&ipAddr, &netMask).IsFailure())
            {
                bdUseVAArgs();
            }
            for (bdInt node = 0; node < 8; ++node)
            {
                if (m_networkInfo.ldn.nodes[node].ipv4Address == ipAddr)
                {
                    m_nodeID = node;
                    break;
                }
            }
            updateLocalCommonAddr(nn::socket::InetHtonl(ipAddr.raw));
        }
        for (bdUInt node = 0; node < 8; ++node)
        {
            if (updates[node].stateChange & nn::ldn::NodeStateChange_Disconnect)
            {
                const nn::ldn::NodeInfo nodeInfo = m_nodesInfo[node];
                bdAddr nodeAddr(bdInetAddr(nn::socket::InetHtonl(nodeInfo.ipv4Address.raw)), m_localCommonAddr->getLocalAddrByIndex(0).getPort());
                bdAddrHandleRef addrHandle;
                bdCommonAddrRef remote = new bdCommonAddr(nodeAddr);
                if (!m_addrMap.getAddrHandle(remote, m_secID, addrHandle))
                {
                    bdUseVAArgs(node);
                }
                disconnect(addrHandle);
                bdNChar8 nodeAddrString[22];
                nodeAddr.toString(nodeAddrString, sizeof(nodeAddrString));
                bdUseVAArgs(node, nodeInfo.userName, nodeAddrString);
            }
            if (updates[node].stateChange & nn::ldn::NodeStateChange_Connect)
            {
                const nn::ldn::NodeInfo & nodeInfo = m_networkInfo.ldn.nodes[node];
                bdAddr nodeAddr(bdInetAddr(nn::socket::InetHtonl(nodeInfo.ipv4Address.raw)), m_localCommonAddr->getLocalAddrByIndex(0).getPort());
                bdCommonAddrRef remote = new bdCommonAddr(nodeAddr);
                bdNChar8 nodeAddrString[22];
                nodeAddr.toString(nodeAddrString, sizeof(nodeAddrString));
                m_nodesInfo[node] = nodeInfo;
                bdUseVAArgs(node, nodeInfo.userName, nodeAddrString);
            }
        }
    }
    else
    {
        leaveNetwork();
        updateLocalCommonAddr(0);
    }
}
