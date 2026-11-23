// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSocketRouter.cpp
#include "bdSocketRouter.h"
#include "../bdCore/bdSocket/bdSocket.h"
#include "bdPacketBuffer.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"
#include "../bdCore/bdSocket/bdCommonAddrInfo.h"
#include "../bdCore/bdSocket/bdSecurityInfo.h"
#include "../bdCore/bdContainers/bdQueue.h"
#include "bdDTLS/bdDTLSPackets.h"
#include "bdDTLS/bdPacket/bdDTLSInit.h"
#include "bdDTLS/bdPacket/bdDTLSCookieEcho.h"
#include "bdDTLS/bdPacket/bdDTLSCookieAck.h"
#include "bdPacketInterceptor.h"








bdSocketRouter::bdSocketRouter()
    : bdNATTravListener(), bdSecurityKeyMapListener(), m_socket(NULL), m_interceptors(), m_status(BD_SOCKET_ROUTER_UNINITIALIZED),
      m_config(), m_localCommonAddr(), m_addrMap(), m_keyStore(), m_natTrav(), m_ECCKey(), m_qosBandwidth(),
      m_dtls(4u, 0.75f), m_sendtoBuffer(NULL), m_qosProber(), m_endpointToAddrMap(4u, 0.75f)
{
}

bdSocketRouter::~bdSocketRouter()
{
    m_keyStore.unregisterListener();

    for (bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator it = m_dtls.getIterator(); it; m_dtls.next(it))
    {
        delete m_dtls.getValue(it);
    }
}

// Release logging uses the existing bdUseVAArgs helper; discarded macro format/channel text is unavailable.
// NON_MATCHING: 800 vs 784 bytes; lines 76/77 split per line table (76's bdCommonAddrRef temporary dies before 77:20 &&); result stays in w20/w22 where the original materializes 0/1 per return path, and the success block is placed after the ECC check; tried combined short circuit, true initializer, accumulated config result.
bdBool bdSocketRouter::init(bdDefaultSocket * socket,
    const bdCommonAddrRef localCommonAddr,
    const bdSocketRouterConfig & config)
{
    bdBool result = true;

    if (m_status != BD_SOCKET_ROUTER_UNINITIALIZED)
    {
        bdUseVAArgs();
        result = false; }

    else if (!socket)
    {
        bdUseVAArgs();
        result = false; }

    else if (localCommonAddr.isNull())
    {
        bdUseVAArgs();
        result = false; }

    else
    {
        m_socket = socket;
        m_localCommonAddr = localCommonAddr;


        if (m_ECCKey.getStatus() == bdECCKey::BD_ECC_KEY_UNINITIALIZED)
        {
            result = m_ECCKey.init();
        }

        if (!bdPlatformSocket::getAdHoc())
        {
            result = result && m_natTrav.init(socket, &m_qosBandwidth, m_localCommonAddr);
            result = result && m_qosProber.init(socket, &m_natTrav, &m_qosBandwidth);
        }

        if (result)
        {
            m_status = BD_SOCKET_ROUTER_INITIALIZED;

            if (!bdPlatformSocket::getAdHoc())
            {
                registerInterceptor(&m_natTrav);
                registerInterceptor(&m_qosProber);
            }
            m_keyStore.registerListener(this);








            m_config = config;


            result = m_config.sanityCheckConfig();
        }

        if (!result)
        {
            bdUseVAArgs();
            m_status = BD_SOCKET_ROUTER_ERROR;
        }
    }

    return result;
}
// Release logging preserves surviving calls/arguments; discarded channel/format text is unavailable.
// NON_MATCHING: 1004 vs 992 bytes; iterator/queue register allocation and spill differ; tried direct DWARF iterator/queue operations.
void bdSocketRouter::pump()
{
    if (!bdPlatformSocket::getAdHoc())
    {
        m_natTrav.pump();
        m_qosProber.pump();
    }

    bdQueue<bdEndpoint> toRemove;
    bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator it = m_dtls.getIterator();
    while (it)
    {
        const bdEndpoint & endpoint = m_dtls.getKey(it);
        bdDTLSAssociation * conn = m_dtls.getValue(it);
        conn->pump();

        switch (conn->getStatus())
        {
        case BD_SOCKET_IDLE:
            bdUseVAArgs();
            break;



        case BD_SOCKET_LOST:
        {
            bdNChar8 addrString[22];
            bdAddrHandleRef addrHandle = conn->getAddrHandle();

            addrHandle->getRealAddr().toString(addrString, sizeof(addrString));

            bdNChar8 secIDString[18];
            bdSecurityInfo::toString(endpoint.getSecID(), secIDString, sizeof(secIDString));
            bdUseVAArgs(addrString, secIDString);

            addrHandle->setStatus(bdAddrHandle::BD_ADDR_UNRESOLVED);
            toRemove.enqueue(endpoint);
            m_addrMap.unregisterAddr(addrHandle);
            if (m_sendtoBuffer)
            {
                m_sendtoBuffer->clearBuffer(endpoint.getHash());
            }
        }
            break;
        default: break;
        }
        m_dtls.next(it);
    }


    while (!toRemove.isEmpty())
    {
        const bdEndpoint & endpoint = toRemove.peek();
        it = m_dtls.getIterator(endpoint);

        if (!it)
        {
            bdUseVAArgs();
            // Inference: missing queued endpoint exits cleanup, as original +0x35c falls into queue destruction.
            break;
        }
        bdDTLSAssociation * dtls = m_dtls.getValue(it);

        delete dtls;
        m_dtls.remove(it);

        toRemove.dequeue();
    }

}
// Release logging uses the surviving bdUseVAArgs call; discarded format/channel text is unavailable.
bdBool bdSocketRouter::quit()
{
    bdBool result = false;

    if (m_status == BD_SOCKET_ROUTER_UNINITIALIZED)
    {

        bdUseVAArgs();
    }

    else
    {
        m_keyStore.unregisterListener();


        bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator it = m_dtls.getIterator();
        while (it)
        {
            delete m_dtls.getValue(it);
            m_dtls.next(it);
        }


        m_dtls.clear();


        if (m_sendtoBuffer)
        {
            m_sendtoBuffer->clearAll();
        }


        m_socket = NULL;

        if (!bdPlatformSocket::getAdHoc())
        {

            result = m_natTrav.quit();
            result &= m_qosProber.quit();


            unregisterInterceptor(&m_natTrav);
            unregisterInterceptor(&m_qosProber);
        }


        m_interceptors.clear();


        m_endpointToAddrMap.clear();

        m_keyStore.clear();

        m_config.reset();

        m_status = BD_SOCKET_ROUTER_UNINITIALIZED;
    }

    return result;
}
// Release logging retains only surviving bdUseVAArgs arguments; discarded format/channel text is unavailable.
bdBool bdSocketRouter::connect(bdAddrHandleRef & addrHandle)
{
    bdBool success = false;
    if (m_status != BD_SOCKET_ROUTER_INITIALIZED)
    {
        bdUseVAArgs();
    }

    else if (addrHandle.isNull())
    {
        bdUseVAArgs();
    }

    else
    {
        bdCommonAddrRef remote = addrHandle->m_endpoint.getCommonAddr();

        bdUseVAArgs();

        bdNChar8 commonAddrInfo[1024];
        bdCommonAddrInfo::getInfo(remote, commonAddrInfo, sizeof(commonAddrInfo));
        bdUseVAArgs();

        bdNChar8 secIDString[18];
        bdSecurityInfo::toString(addrHandle->m_endpoint.getSecID(), secIDString, sizeof(secIDString));
        bdUseVAArgs(secIDString);


        if (!m_keyStore.contains(addrHandle->m_endpoint.getSecID()))
        {
            bdUseVAArgs();
        }

        const bdAddrHandle::bdAddrHandleStatus status = addrHandle->getStatus();
        switch (status)
        {
        case bdAddrHandle::BD_ADDR_NOT_CONSTRUCTED:
        case bdAddrHandle::BD_ADDR_NOT_RESOLVED:
        case bdAddrHandle::BD_ADDR_ERROR:
        case bdAddrHandle::BD_ADDR_UNRESOLVED:



        {
            bdUseVAArgs();











            bdEndpoint endpoint(remote, addrHandle->m_endpoint.getSecID());
            bdBool added = m_endpointToAddrMap.put(endpoint, addrHandle);

            if (!added)
            {

                bdUseVAArgs();

                bdUseVAArgs();
                m_natTrav.cancelConnect(remote);


                bdUseVAArgs();
                m_endpointToAddrMap.remove(endpoint);
                added = m_endpointToAddrMap.put(endpoint, addrHandle);
            }

            if (added)
            {
                success = m_natTrav.connect(remote, this, false);
            }
            else
            {

                bdUseVAArgs();
            }
            break;
        }

        case bdAddrHandle::BD_ADDR_RESOLVED:

            bdUseVAArgs();
            break;
        default: break;
        }
    }

    return success;
}
// NON_MATCHING: 512 vs 520 bytes (544 before bdHashMap::remove(Iterator&) took its DWARF form, h17); n->m_next is now loaded before the match test as in the original (hoisted from both inlined next() calls), but the loop iterator is still split across two registers (extra mov at the loop head) and the slow paths are placed differently.
bdBool bdSocketRouter::disconnect(bdAddrHandleRef & addrHandle)
{
    bdBool success = false;
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED &&



        !addrHandle.isNull())



    {

        bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator it = m_dtls.getIterator();
        bdUInt numFound = 0;
        while (it)
        {
            bdDTLSAssociation * conn = m_dtls.getValue(it);
            if (*conn->getAddrHandle() == *addrHandle)
            {
                m_dtls.remove(it);
                delete conn;
                ++numFound;
            }
            else
            {
                m_dtls.next(it);
            }
        }

        if (numFound > 1)
        {
            bdLogWarn("bdSocket/router", "Found multiple DTLS associations for one address."); // Inference: the original bdUseVAArgs call at 375 (+0x1ac) sets no argument register, so numFound is not passed; format text is not recoverable.
        }
        if (m_sendtoBuffer)
        {
            m_sendtoBuffer->clearBuffer(addrHandle->m_endpoint.getHash());
        }
        success = m_addrMap.unregisterAddr(addrHandle);
    }
    else { bdLogWarn("bdSocket/router", "Cannot disconnect."); }
    return success;
}

// NON_MATCHING: 532 vs 524; one return (the original epilogue and the default IDLE phi copy sit on 459:2), but the 414:5 PENDING test after the getCommonAddr temporary cleanup is tail-duplicated and placed last in ours.
bdDTLSAssociationStatus bdSocketRouter::getStatus(const bdAddrHandleRef addrHandle)
{
    bdDTLSAssociationStatus status = BD_SOCKET_LOST;

    if (addrHandle.isNull())
    {

        bdLogWarn("bdSocket/router", "Null address handle.");
    }
    else
    {
        bdNChar8 addrStr[22];
        bdAddressMap::addrToString(addrHandle, addrStr, sizeof(addrStr));

        switch (addrHandle->getStatus())
        {





        case bdAddrHandle::BD_ADDR_NOT_RESOLVED:

            if (!bdPlatformSocket::getAdHoc() && m_natTrav.isConnectInProgress(addrHandle->m_endpoint.getCommonAddr()))
            {

                status = BD_SOCKET_PENDING;
            }
            break;

        case bdAddrHandle::BD_ADDR_ERROR:
        case bdAddrHandle::BD_ADDR_UNRESOLVED:
            bdLogInfo("bdSocket/router", "Address %s is not resolved.", addrStr);
            break;





        case bdAddrHandle::BD_ADDR_RESOLVED:
        {
            bdEndpoint endpoint(addrHandle->m_endpoint.getCommonAddr(), addrHandle->m_endpoint.getSecID());

            bdDTLSAssociation * dtlsSocket = NULL;
            const bdBool found = m_dtls.get(endpoint, dtlsSocket);

            if (found && dtlsSocket)
            {
                status = dtlsSocket->getStatus();
            }
            else
            {
                bdLogWarn("bdSocket/router", "No DTLS association for %s.", addrStr);
                status = BD_SOCKET_LOST;
            }
            break;




        }

        default:
            status = BD_SOCKET_IDLE;
        }
    }





    return status;
}

bdSocketRouter::bdSocketRouterStatus bdSocketRouter::getStatus()
{
    return m_status;
}
// Release logging calls bdUseVAArgs directly; discarded channel/format strings cannot be recovered.
bdInt bdSocketRouter::sendTo(const bdAddrHandleRef addrHandle,
    void * data,
    const bdUInt length)
{
    bdInt val = -1;
    if (m_status != BD_SOCKET_ROUTER_INITIALIZED)
    {
        bdUseVAArgs();
    }
    else if (addrHandle.isNull())
    {
        bdUseVAArgs();
    }
    else
    {
        const bdAddr & realAddr = addrHandle->getRealAddr();

        bdDTLSAssociation * dtlsSocket = NULL;
        bdEndpoint endpoint(addrHandle->m_endpoint.getCommonAddr(), addrHandle->m_endpoint.getSecID());

        m_dtls.get(endpoint, dtlsSocket);




        if (cacheSendTo(addrHandle, data, length)) {
            // Inference: cached sends report length, retained in w19 through the original success path.
            val = length;
        }
        else if (dtlsSocket)
        {
            val = dtlsSocket->sendTo(realAddr, data, length);
        }
        else
        {
            bdUseVAArgs();
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(addrHandle->m_endpoint.getCommonAddr(), commonAddrInfo, sizeof(commonAddrInfo));
            bdUseVAArgs();
        }
    }

    return val;
}

bdBool bdSocketRouter::cacheSendTo(const bdAddrHandleRef addrHandle,
    void * data,
    bdUInt length)
{
    bdBool buffered = false;

    bdDTLSAssociation * dtlsSocket = NULL;
    m_dtls.get(addrHandle->m_endpoint, dtlsSocket);
    bdInt bufferKey = addrHandle->m_endpoint.getHash();

    if (dtlsSocket)
    {
        if (m_sendtoBuffer)
        {
            bdDTLSAssociationStatus dtlsStatus = dtlsSocket->getStatus();
            if (dtlsStatus == BD_SOCKET_CONNECTED)
            {
                const bdAddr & realAddr = addrHandle->getRealAddr();

                m_sendtoBuffer->flushBuffer(bufferKey, dtlsSocket, realAddr);

            }
            else if (dtlsStatus == BD_SOCKET_PENDING)
            {
                buffered = m_sendtoBuffer->bufferPacket(bufferKey, data, length);
            }
            else
            {
                m_sendtoBuffer->clearBuffer(bufferKey);
            }
        }
    }
    else if (m_natTrav.isConnectInProgress(addrHandle->m_endpoint.getCommonAddr()) && m_sendtoBuffer)
    {
        buffered = m_sendtoBuffer->bufferPacket(bufferKey, data, length);
    }
    return buffered;
}

bdInt bdSocketRouter::receiveFrom(bdAddrHandleRef & addrHandle,
    void * data,
    const bdUInt size)
{

    bdInt val = 0;

    if (m_status != BD_SOCKET_ROUTER_INITIALIZED)
    {
        bdLogWarn("bdSocket/router", "Not initialized.");
        val = BD_NET_WOULD_BLOCK;
    } else
    {
        bdBool receiving = true;
        bdAddr realAddr;
        bdUInt resetCount = 0;

        while (receiving)
        {
            val = m_socket->receiveFrom(realAddr, data, size);

            if (val > 0)
            {
                val = processPacket(addrHandle, realAddr, data, size, val, receiving);
            }
            else if (val < 0)
            {
                processError(realAddr, val, resetCount, receiving);
            }
            else
            {
                bdLogInfo("bdSocket/router", "Connection closed.");
                val = 0;
                receiving = false;
            }
        }
    }
    return val;
}
// NON_MATCHING: 2644 vs 2632 bytes; register/block scheduling differs; tried direct packet switch and original stack/heap DTLS construction.
bdInt bdSocketRouter::processPacket(bdAddrHandleRef & addrHandle,
    bdAddr & realAddr,
    void * data,
    const bdUInt size,
    const bdUInt bytesTransferred,
    bdBool & receiving)
{
    bdInt val = bytesTransferred;
    const bdUByte8 type = *static_cast<bdUByte8 *>(data);
    bdBool createNewDTLS = false;
    switch (type)
    {
    case BD_DTLS_INIT:
    {
        bdDTLSInit init;
        bdUInt offset = 0;
        bdBool ok = init.deserialize(data, val, offset, offset, m_config.m_cypherSuite);
        if (ok)
        {
            bdSecurityID initSecID;
            init.getSecID(initSecID);

            if (!tryToFindConnection(data, size, initSecID, realAddr, addrHandle, val, true))
            {
                bdDTLSAssociation serverDTLS(m_socket,
                    &m_keyStore,
                    &m_ECCKey,
                    realAddr,
                    addrHandle,
                    m_localCommonAddr,
                    &m_addrMap,
                    m_config.m_DTLSAssociationReceiveTimeout,
                    m_config.m_cypherSuite);

                val = serverDTLS.receiveFrom(realAddr, data, val, addrHandle, data, size);
                createNewDTLS = serverDTLS.getStatus() == BD_SOCKET_CONNECTED;
            }
        }
        break;
    }

    case BD_DTLS_COOKIE_ECHO:
    {
        bdDTLSCookieEcho cookieEcho;
        bdUInt offset = 0;
        bdBool ok = cookieEcho.deserialize(data, val, offset, offset, m_config.m_cypherSuite);
        if (ok)
        {
            bdSecurityID cookieEchoSecID;
            bdMemcpy(cookieEchoSecID.ab, cookieEcho.getSecID(), sizeof(cookieEchoSecID.ab));

            if (!tryToFindConnection(data, size, cookieEchoSecID, realAddr, addrHandle, val, true))
            {
                bdDTLSAssociation serverDTLS(m_socket,
                    &m_keyStore,
                    &m_ECCKey,
                    realAddr,
                    addrHandle,
                    m_localCommonAddr,
                    &m_addrMap,
                    m_config.m_DTLSAssociationReceiveTimeout,
                    m_config.m_cypherSuite);

                val = serverDTLS.receiveFrom(realAddr, data, val, addrHandle, data, size);
                createNewDTLS = serverDTLS.getStatus() == BD_SOCKET_CONNECTED;
            }
        }
        break;
    }

    case BD_DTLS_ERROR:
    {
        bdDTLSError error;
        bdUInt offset = 0;
        bdBool ok = error.deserialize(data, val, offset, offset);

        if (ok)
        {
            bdSecurityID errorSecID;
            error.getSecID(errorSecID);

            if (!tryToFindConnection(data, size, errorSecID, realAddr, addrHandle, val, true))
            {
                bdDTLSAssociation serverDTLS(m_socket,
                    &m_keyStore,
                    &m_ECCKey,
                    realAddr,
                    addrHandle,
                    m_localCommonAddr,
                    &m_addrMap,
                    m_config.m_DTLSAssociationReceiveTimeout,
                    m_config.m_cypherSuite);

                val = serverDTLS.receiveFrom(realAddr, data, val, addrHandle, data, size);
                createNewDTLS = serverDTLS.getStatus() == BD_SOCKET_CONNECTED;
            }
        }
        break;
    }

    case BD_DTLS_INIT_ACK:
    {
        bdDTLSInitAck initAck;
        bdUInt offset = 0;
        bdBool ok = initAck.deserialize(data, val, offset, offset, m_config.m_cypherSuite);
        if (ok)
        {
            bdSecurityID initAckSecID;
            initAck.getSecID(initAckSecID);


            tryDecryptPacket(data, size, initAckSecID, realAddr, addrHandle, val, true);
        }
        break;
    }


    case BD_DTLS_COOKIE_ACK:
    {
        bdDTLSCookieAck cookieAck;
        bdUInt offset = 0;
        bdBool ok = cookieAck.deserialize(data, val, offset, offset);
        if (ok)
        {
            bdSecurityID ackSecID;
            cookieAck.getSecID(ackSecID);


            tryDecryptPacket(data, size, ackSecID, realAddr, addrHandle, val, true);
        }
        break;
    }


    case BD_DTLS_DATA:
    {
        bdBool found = false;
        bdSecurityID fake_sec_id;
        found = tryDecryptPacket(data, size, fake_sec_id, realAddr, addrHandle, val, false);
        receiving = !found;
        break;
    }


    default:
    {
        bdBool accepted = false;
        for (bdUInt i = 0; i < m_interceptors.getSize() && !accepted; ++i)
        {
            accepted = m_interceptors[i]->acceptPacket(m_socket, realAddr, data, val, type);
        }
        break;
    }
    }

    if (createNewDTLS)
    {
        bdDTLSAssociation * dtlsSocket = new bdDTLSAssociation(m_socket,
            &m_keyStore,
            &m_ECCKey,
            realAddr,
            addrHandle,
            m_localCommonAddr,
            &m_addrMap,
            m_config.m_DTLSAssociationReceiveTimeout,
            m_config.m_cypherSuite);

        val = dtlsSocket->receiveFrom(realAddr, data, val, addrHandle, data, size);

        bdNChar8 addrString[22];
        realAddr.toString(addrString, sizeof(addrString));
        // Release logging retains the original addrString argument; discarded format/channel text is unavailable.
        bdUseVAArgs(addrString);
        bdEndpoint endpoint(dtlsSocket->getAddrHandle()->m_endpoint.getCommonAddr(),
            dtlsSocket->getAddrHandle()->m_endpoint.getSecID());

        const bdBool dtlsAdded = m_dtls.put(endpoint, dtlsSocket);
        if (!dtlsAdded)
        {
            bdNChar8 commonAddrString[1024];
            bdCommonAddrInfo::getInfo(endpoint.getCommonAddr(), commonAddrString, sizeof(commonAddrString));
            bdNChar8 secIDString[18];
            bdSecurityInfo::toString(endpoint.getSecID(), secIDString, sizeof(secIDString));
            bdUseVAArgs(secIDString);
            bdUseVAArgs(commonAddrString);

            delete dtlsSocket;
        }

        if (val >= 0)
        {

            receiving = false;
        }
    }
    return val;
}

void bdSocketRouter::processError(bdAddr & realAddr,
    bdInt val,
    bdUInt & resetCount,
    bdBool & receiving)
{

    receiving = false;

    const bdSocketStatusCode error = static_cast<bdSocketStatusCode>(val);
    switch (error)
    {
    case BD_NET_SUCCESS:
    case BD_NET_CONNECTION_CLOSED:
    case BD_NET_MSG_SIZE:
        bdLogWarn("bdSocket/router", "Unexpected status.");
        break;

    case BD_NET_CONNECTION_RESET:
        if (!m_config.m_ignoreConnectionReset)

        {

            m_addrMap.unregisterRealAddr(realAddr);
        }
        else
        {
            const bdUWord addrBufferSize = 100;
            bdNChar8 addrBuffer[100];
            realAddr.toString(addrBuffer, addrBufferSize);
            bdLogInfo("bdSocket/router", "Connection reset from %s ignored.", addrBuffer);
        }




        if (resetCount++ < m_config.m_maxConnectionResets)
        {
            receiving = true;
        }
        else
        {
            const bdUWord addrBufferSize = 100;
            bdNChar8 addrBuffer[100];
            realAddr.toString(addrBuffer, addrBufferSize);
            bdLogWarn("bdSocket/router", "%u connection resets.", resetCount);
            bdLogWarn("bdSocket/router", "Last reset from %s.", addrBuffer);
        }
        break;



    case BD_NET_ERROR:
    case BD_NET_SUBSYTEM_ERROR:
    case BD_NET_ADDRESS_IN_USE:
        bdLogWarn("bdSocket/router", "Socket error (%i).", val);
        break;
    case BD_NET_ADDRESS_INVALID:
    case BD_NET_BLOCKING_CALL_CANCELED:
    case BD_NET_NOT_BOUND:
    case BD_NET_INVALID_HANDLE:
    case BD_NET_NOT_CONNECTED:
    case BD_NET_CONNECTION_REFUSED:
    case BD_NET_HOST_UNREACH:










        bdLogWarn("bdSocket/router", "Socket error (%i).", val);
        break;

    default:
        break;



    }

}
// NON_MATCHING: 1576 vs 1536 bytes (1540 before bdHashMap::remove(Iterator&) took its DWARF form, h17); iterator-removal register/branch layout differs; tried original iterator overload and DTLS construction.
void bdSocketRouter::onNATAddrDiscovery(bdCommonAddrRef remote, const bdAddr & realAddr)
{
    bdAddrHandleRef addrHandle;
    // Release logging preserves surviving calls/arguments; discarded channel/format text is unavailable.
    if (m_status != BD_SOCKET_ROUTER_INITIALIZED)
    {
        bdUseVAArgs();
    }
    else
    {
        bdHashMap<bdEndpoint, bdReference<bdAddrHandle>, bdHashingClass>::Iterator iterator = m_endpointToAddrMap.getIterator();
        bdBool found = false;
        while (iterator)
        {
            bdEndpoint key = m_endpointToAddrMap.getKey(iterator);
            addrHandle = m_endpointToAddrMap.getValue(iterator);

            if (*key.getCommonAddr() == *remote) {
                // Inference: a matching endpoint records success, retained through the original loop in w28.
                found = true;
                m_endpointToAddrMap.remove(iterator);
                addrHandle->setRealAddr(realAddr);
                bdEndpoint endpoint(addrHandle->m_endpoint.getCommonAddr(), addrHandle->m_endpoint.getSecID());

                bdDTLSAssociation * conn = NULL;
                if (!m_dtls.get(endpoint, conn))
                {
                    bdNChar8 addrString[22];
                    realAddr.toString(addrString, sizeof(addrString));

                    bdUseVAArgs(addrString);

                    conn = new bdDTLSAssociation(m_socket,
                        &m_keyStore,
                        &m_ECCKey,
                        realAddr,
                        addrHandle,
                        m_localCommonAddr,
                        &m_addrMap,
                        m_config.m_DTLSAssociationReceiveTimeout,
                        m_config.m_cypherSuite);
                    conn->connect();

                    const bdBool putResult = m_dtls.put(key, conn);
                    if (!putResult)
                    {
                        bdNChar8 commonAddrString[1024];
                        bdCommonAddrInfo::getInfo(endpoint.getCommonAddr(), commonAddrString, sizeof(commonAddrString));
                        bdNChar8 secIDString[18];
                        bdSecurityInfo::toString(endpoint.getSecID(), secIDString, sizeof(secIDString));

                        bdUseVAArgs(secIDString);
                        bdUseVAArgs(commonAddrString);

                        delete conn;
                    }
                }
                else
                {
                    bdUseVAArgs();
                }
            }
            else
            {
                m_endpointToAddrMap.next(iterator);
            }
        }

        if (!found)
        {
            bdUseVAArgs();
            bdNChar8 commonAddrInfo[1024];
            bdCommonAddrInfo::getBriefInfo(remote, commonAddrInfo, sizeof(commonAddrInfo));
            bdUseVAArgs();
        }
    }

}
// NON_MATCHING: 968 vs 980 bytes; iterator-removal register/branch layout differs; tried original iterator overload.
void bdSocketRouter::onNATAddrDiscoveryFailed(bdCommonAddrRef remote)
{
    bdUseVAArgs();
    // Release logging preserves surviving calls/arguments; discarded channel/format text is unavailable.
    bdNChar8 commonAddrInfo[1024];
    bdUseVAArgs();
    bdCommonAddrInfo::getInfo(m_localCommonAddr, commonAddrInfo, sizeof(commonAddrInfo));
    bdUseVAArgs();
    bdUseVAArgs();
    bdCommonAddrInfo::getInfo(remote, commonAddrInfo, sizeof(commonAddrInfo));
    bdUseVAArgs();

    bdAddrHandleRef addrHandle;

    bdHashMap<bdEndpoint, bdReference<bdAddrHandle>, bdHashingClass>::Iterator iterator = m_endpointToAddrMap.getIterator();
    bdBool found = false;

    while (iterator)
    {
        bdEndpoint key = m_endpointToAddrMap.getKey(iterator);
        addrHandle = m_endpointToAddrMap.getValue(iterator);

        if (*key.getCommonAddr() == *remote) {
            // Inference: record the matched endpoint; original +0x2e8 sets found after status handling.
            found = true;
            if (m_sendtoBuffer)
            {
                m_sendtoBuffer->clearBuffer(key.getHash());
            }
            m_endpointToAddrMap.remove(iterator);
            if (addrHandle->getStatus() == bdAddrHandle::BD_ADDR_NOT_RESOLVED)
            {
                addrHandle->setStatus(bdAddrHandle::BD_ADDR_ERROR);
            }
        }
        else
        {
            m_endpointToAddrMap.next(iterator);
        }
    }

    if (!found)
    {
        bdUseVAArgs();
    }
}
// Recovered from live init inline instances; discarded out-of-line copy is DWARF DIE 0x07bd0d80 (low_pc 0).
void bdSocketRouter::registerInterceptor(bdPacketInterceptor *const interceptor)
{
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED)
    {








        m_interceptors.pushBack(interceptor);
    }
    else
    {
        bdUseVAArgs();
    }
}
// Recovered from live quit inline calls; discarded concrete DIE 0x07bd1e5d has low_pc 0.
void bdSocketRouter::unregisterInterceptor(bdPacketInterceptor *const interceptor)
{
    m_interceptors.removeAllKeepOrder(interceptor);
}












bdQoSProbe * bdSocketRouter::getQoSProber()
{
    return &m_qosProber;
}
// NON_MATCHING: 520 vs 496 bytes; post-inline InstCombine folds the else path's (Node*)it cast into a second Node* phi web while the remove path stays on the void* web, so the iterator lives in two registers (x26/x27). An IR probe that blocks that fold gives the original loop body and 496; the source cause is unknown (build/try/draft/bdSocketRouter.txt, d18).
void bdSocketRouter::onSecurityKeyRemove(const bdSecurityID & id)
{
    bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator it = m_dtls.getIterator();
    while (it)
    {
        bdDTLSAssociation * conn = m_dtls.getValue(it);
        if (*conn->getLocalSecurityId() == id)
        {
            bdAddrHandleRef addrHandle(conn->getAddrHandle());
            addrHandle->setStatus(bdAddrHandle::BD_ADDR_UNRESOLVED);

            delete conn;
            m_dtls.remove(it);

            m_addrMap.unregisterAddr(addrHandle);
            if (m_sendtoBuffer)
            {
                m_sendtoBuffer->clearBuffer(addrHandle->m_endpoint.getHash());
            }

        }
        else
        {
            m_dtls.next(it);
        }
    }

}

bdBool bdSocketRouter::setupIntroducers(const bdArray<bdAddr> & introducers)
{
    bdBool result = false;
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED)
    {
        result = m_natTrav.setupIntroducers(introducers);
    }
    return result;
}

bdBool bdSocketRouter::updateLocalCommonAddr(const bdCommonAddrRef newLocalCommonAddr)
{
    bdBool result = false;
    if (m_status == BD_SOCKET_ROUTER_INITIALIZED && !m_localCommonAddr.isNull())
    {
        m_localCommonAddr = newLocalCommonAddr;
        if (bdPlatformSocket::getAdHoc())
        {
            result = true;
        }
        else
        {
            result = m_natTrav.updateLocalCommonAddr(newLocalCommonAddr);
        }
    }
    else
    {
        bdLogWarn("bdSocket/router", "Cannot update local common address.");
    }
    return result;
}










bdAddressMap * bdSocketRouter::getAddressMap()
{
    return &m_addrMap;
}

bdSecurityKeyMap * bdSocketRouter::getKeyMap()
{
    return &m_keyStore;
}











bdCommonAddrRef bdSocketRouter::getLocalCommonAddr() const
{
    return m_localCommonAddr;
}


bdBool bdSocketRouter::tryDecryptPacket(void * data,
    const bdUInt & size,
    const bdSecurityID & secID,
    bdAddr & realAddr,
    bdAddrHandleRef & addrHandle,
    bdInt & val,
    bdBool checkSecID)
{

    bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator iterator = m_dtls.getIterator();
    bdBool found = false;


    while (iterator && !found)
    {

        bdEndpoint key = m_dtls.getKey(iterator);

        bdDTLSAssociation * dtls = m_dtls.getValue(iterator);

        bdAddrHandleRef dtlsAddrHandle;
        bdAddr dtlsRealAddr;

        if (dtls)
        {
            dtlsAddrHandle = dtls->getAddrHandle();

            // Inference: null-handle guard is folded into the reference assignment branch at +0xcc.
            if (dtlsAddrHandle.notNull())
            {
            dtlsRealAddr = dtlsAddrHandle->getRealAddr();




            if (
                dtlsRealAddr == realAddr &&
                (!checkSecID || key.getSecID() == secID))
            {


                bdInt returnVal = dtls->receiveFrom(realAddr, data, val, addrHandle, data, size);


                if (returnVal >= 0)
                {
                    found = true;

                    val = returnVal;
                }
            }
            }
        }
        if (!found)
        {

            m_dtls.next(iterator);
        }
    }
    return found;
}
// NON_MATCHING: 1564 vs 1580 bytes; the 1259 getAddrHandle temporary shares the 1223/1261 stack slot in ours but has its own slot in the original (frame 0x5b0 vs 0x5c0), and the iterator-advance block is placed last; tried found && guard on 1258, direct DWARF locals, endpoint migration.
bdBool bdSocketRouter::tryToFindConnection(void * data,
    const bdUInt & size,
    const bdSecurityID & secID,
    bdAddr & realAddr,
    bdAddrHandleRef & addrHandle,
    bdInt & val,
    bdBool checkSecID)
{
    // Release logging preserves surviving calls/arguments; discarded channel/format text is unavailable.
    bdHashMap<bdEndpoint, bdDTLSAssociation *, bdEndpointHashingClass>::Iterator iterator = m_dtls.getIterator();
    bdBool found = false;

    while (iterator && !found)
    {

        bdEndpoint key = m_dtls.getKey(iterator);

        bdDTLSAssociation * dtls = m_dtls.getValue(iterator);

        bdAddrHandleRef dtlsAddrHandle;
        bdAddr dtlsRealAddr;

        if (dtls)
        {
            dtlsAddrHandle = dtls->getAddrHandle();
            // Inference: null-handle guard folds into original reference assignment branch +0xe4.
            if (dtlsAddrHandle.notNull())
            {
                dtlsRealAddr = dtlsAddrHandle->getRealAddr();
            } }
        // Inference: no dtls test here: 1230:71 (bdAddr ==) and 1230:84 (&&) columns fit only this spelling; a null dtls leaves dtlsAddrHandle null.
        bdBool verifiedRealAddr = dtlsAddrHandle.notNull() && (dtlsRealAddr == realAddr) &&
            (!checkSecID || key.getSecID() == secID);



        if (verifiedRealAddr)
        {



            bdInt returnVal = dtls->receiveFrom(realAddr, data, val, addrHandle, data, size);


            if (returnVal >= 0 || returnVal == BD_NET_WOULD_BLOCK)
            {
                // Inference: successful receive sets found; original +0x574 materializes true after migration handling.
                found = true;
                val = returnVal;










                if (dtlsAddrHandle !=
                    dtls->getAddrHandle()) {

                    if (!dtls->getAddrHandle().isNull())
                    {
                        bdAddrHandleRef newDtlsHandle = dtls->getAddrHandle();
                        bdEndpoint newEndpoint(newDtlsHandle->m_endpoint.getCommonAddr(),
                            newDtlsHandle->m_endpoint.getSecID());
                        if (!(newEndpoint == key))
                        {
                            bdNChar8 oldPublicAddrBuf[64];
                            bdNChar8 newPublicAddrBuf[64];
                            key.getCommonAddr()->getPublicAddr().toString(oldPublicAddrBuf, sizeof(oldPublicAddrBuf));
                            newEndpoint.getCommonAddr()->getPublicAddr().toString(newPublicAddrBuf, sizeof(newPublicAddrBuf));
                            bdUseVAArgs(oldPublicAddrBuf, newPublicAddrBuf);

                            const bdBool removed = m_dtls.remove(key);



                            const bdBool added = m_dtls.put(newEndpoint, dtls);
                            if (!added)
                            {
                                bdNChar8 commonAddrString[1024];
                                bdCommonAddrInfo::getInfo(newEndpoint.getCommonAddr(), commonAddrString, sizeof(commonAddrString));


                                bdNChar8 secIDString[18];
                                bdSecurityInfo::toString(newEndpoint.getSecID(), secIDString, sizeof(secIDString));


                                bdUseVAArgs(secIDString);

                                bdUseVAArgs(commonAddrString);

                                if (removed)
                                {
                                    delete dtls;
                                }
                            }
                        }
                    }

                    else
                    {

                        bdUseVAArgs();
                    }
                }
            }
        }

        if (!found)
        {
            m_dtls.next(iterator);
        }
    }
    return found;
}

bdBool bdSocketRouter::isRelayEnabled() const
{
    return false;
}


