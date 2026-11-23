#include "bdConnectionStore.h"
#include "bdConnection.h"
#include "../bdSocket/bdSocketRouter.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"



















bdConnectionStore::bdConnectionStore() : m_socket(NULL), m_connectionMap(4, 0.75f), m_status(BD_CONNECTION_STORE_UNINITIALIZED), m_addrMap(NULL)
{
}




bdConnectionStore::~bdConnectionStore()
{
    m_connectionMap.clear();
    m_dispatcher.reset();
}

bdBool bdConnectionStore::init(bdSocketRouter * socket,
    const bdConnectionStoreConfig & config)
{ // Inferred default for result: the original invalid-state and null-socket paths return false.
    bdBool result = false;

    if (m_status != BD_CONNECTION_STORE_UNINITIALIZED)
    {
        bdUseVAArgs();
    }

    else if (!socket)
    {
        bdUseVAArgs();
    }
    else
    { // Inferred success assignment: the original configured path returns true.
        result = true;
        m_socket = socket;
        m_addrMap = m_socket->getAddressMap();
        m_config = config;
        m_status = BD_CONNECTION_STORE_INITIALIZED;
    }
    return result;
}




























































































































































































































































































































void bdConnectionStore::dispatchAll()
{
    bdArray<bdReference<bdConnection> > connections(m_connectionMap.getSize());


    bdHashMap<bdAddrHandleRefWrapper, bdReference<bdConnection>, bdAddrHandleRefWrapper>::Iterator it = m_connectionMap.getIterator();
    while (it)
    {
        connections.pushBack(m_connectionMap.getValue(it));
        m_connectionMap.next(it);
    }

    // Inferred loop spelling: DWARF i starts at zero; original loops through the copied connection count.
    for (bdUInt i = 0; i < connections.getSize(); ++i)
    {
        m_dispatcher.process(connections[i]);
    }
}
// Inferred != test: line 400 col 43 is m_status; the switch (line 405, two tabs deep) is inside the if block; the rejection log is the shared tail.
bdBool bdConnectionStore::startShutdown(const bdConnectionStore::bdConnectionStoreShutdownType shutdownType)
{ // Inferred initialization from DWARF result location: false until shutdown succeeds.
    bdBool result = false;

    if (BD_CONNECTION_STORE_UNINITIALIZED != m_status)
    {



        switch (shutdownType)
        {
        case BD_CONNECTION_STORE_SHUTDOWN_ELEGANT:
            // Inferred source branch: original accepts INITIALIZED or ERROR, logs SHUTTING_DOWN.
            if (BD_CONNECTION_STORE_INITIALIZED == m_status || BD_CONNECTION_STORE_ERROR == m_status)
            {

                bdHashMap<bdAddrHandleRefWrapper, bdReference<bdConnection>, bdAddrHandleRefWrapper>::Iterator it = m_connectionMap.getIterator();
                while (it)
                {
                    bdConnectionRef conn = m_connectionMap.getValue(it);
                    conn->disconnect();
                    m_connectionMap.next(it);
                }
                m_status = BD_CONNECTION_STORE_SHUTTING_DOWN;
                m_shutdownTimer.start();
                // Inferred success assignment from the original shared true-return tail.
                result = true;
            }
            else if (BD_CONNECTION_STORE_SHUTTING_DOWN == m_status)
            {
                bdUseVAArgs();
            }
            break;

        case BD_CONNECTION_STORE_SHUTDOWN_IMMEDIATE:
            {
                bdHashMap<bdAddrHandleRefWrapper, bdReference<bdConnection>, bdAddrHandleRefWrapper>::Iterator it = m_connectionMap.getIterator();
                while (it)
                {
                    bdConnectionRef conn = m_connectionMap.getValue(it);
                    conn->disconnect();
                    conn->close();
                    m_connectionMap.next(it);
                }
                m_connectionMap.clear();
                m_dispatcher.reset();
                m_socket = NULL;
                m_status = BD_CONNECTION_STORE_UNINITIALIZED;
                m_shutdownTimer.reset();
                m_addrMap = NULL;
                m_config.reset();
                // Inferred success assignment from the original shared true-return tail.
                result = true;
            }
            break;
        }
    }
    else
        bdUseVAArgs();
    return result;
}























































































bdConnectionStore::bdConnectionStoreStatus bdConnectionStore::getStatus() const
{
    return m_status;
}

void bdConnectionStore::registerDispatchInterceptor(bdDispatchInterceptor *const interceptor)
{
    m_dispatcher.registerInterceptor(interceptor);
}

void bdConnectionStore::unregisterDispatchInterceptor(bdDispatchInterceptor *const interceptor)
{
    m_dispatcher.unregisterInterceptor(interceptor);
}
































































#include "bdLoopbackConnection.h"
#include "bdUnicastConnection.h"
#include "../bdSocket/bdAddressMap.h"
#include "../bdPlatform/bdPlatformSocket/bdPlatformSocket.h"

// Original line 559. Stripped log messages retain only their surviving argument calls.
// Inferred separate case bodies: the original jump table (index status+7) has distinct targets for -3..-1 merged after lowering.
bdUInt bdConnectionStore::flush(bdConnection & connection)
{
    bdUByte8 data[1256];
    const bdUInt size = connection.getDataToSend(data, sizeof(data));
    if (size)
    {
        bdAddrHandleRef addrHandle = connection.getAddressHandle();
        const bdInt status = m_socket->sendTo(addrHandle, data, size);
        if (status < 0)
        {
            const bdSocketStatusCode error = static_cast<bdSocketStatusCode>(status);
            switch (error)
            {
            case BD_NET_ADDRESS_INVALID:
                bdUseVAArgs();
                connection.close();
                break;
            case BD_NET_ERROR:
                bdUseVAArgs();
                break;
            case BD_NET_WOULD_BLOCK:
                bdUseVAArgs();
                break;
            case BD_NET_SUBSYTEM_ERROR:
                bdUseVAArgs();
                break;
            }
        }
    }
    return size;
}

// Original line 64.
bdBool bdConnectionStore::flushAll()
{
    bdBool result = true;
    // Inferred positive condition: original line 67 tests the two accepted states (sub 1; cmp 1; b.hi); block 71-189, rejection log at 192.
    if ((BD_CONNECTION_STORE_INITIALIZED == m_status) || (BD_CONNECTION_STORE_SHUTTING_DOWN == m_status))
    {
        bdQueue<bdConnectionRef> toDisconnect;
        m_socket->pump();
        bdHashMap<bdAddrHandleRefWrapper, bdConnectionRef, bdAddrHandleRefWrapper>::Iterator it = m_connectionMap.getIterator();
        while (it)
        {
            bdConnectionRef connection = m_connectionMap.getValue(it);
            // Inferred negated test: the non-loopback block (lines 84-129) precedes the loopback update (line 135).
            if (!connection->getAddress()->isLoopback())
            {
                const bdDTLSAssociationStatus & status = m_socket->getStatus(connection->getAddressHandle());
                bdUInt flushIters = 0;
                switch (status)
                {
                case BD_SOCKET_CONNECTED:
                    while (flushIters < m_config.m_maxFlushIterations && flush(*connection))
                        ++flushIters;
                    if (connection->getStatus() == bdConnection::BD_DISCONNECTED)
                    {
                        const bdAddrHandleRef addrHandle = connection->getAddressHandle();
                        bdNChar8 addrStr[22];
                        bdAddressMap::addrToString(addrHandle, addrStr, sizeof(addrStr));
                        bdUseVAArgs(addrStr);
                        toDisconnect.enqueue(connection);
                    }
                    break;
                case BD_SOCKET_LOST:
                    {
                        const bdAddrHandleRef addrHandle = connection->getAddressHandle();
                        bdNChar8 addrStr[22];
                        bdAddressMap::addrToString(addrHandle, addrStr, sizeof(addrStr));
                        bdUseVAArgs(addrStr);
                        toDisconnect.enqueue(connection);
                    }
                    break;
                case BD_SOCKET_IDLE:
                    {
                        const bdAddrHandleRef addrHandle = connection->getAddressHandle();
                        bdNChar8 addrStr[22];
                        bdAddressMap::addrToString(addrHandle, addrStr, sizeof(addrStr));
                        bdUseVAArgs(addrStr);
                        toDisconnect.enqueue(connection);
                    }
                    break;
                }
            }
            else
            {
                reference_cast<bdLoopbackConnection>(connection)->updateStatus();
                if (connection->getStatus() == bdConnection::BD_DISCONNECTED)
                    toDisconnect.enqueue(connection);
            }
            m_connectionMap.next(it);
        }
        while (!toDisconnect.isEmpty())
        {
            bdConnectionRef connection = toDisconnect.peek();
            bdAddrHandleRef addr = connection->getAddressHandle();
            connection->close();
            const bdBool mapRemoveResult = m_connectionMap.remove(bdAddrHandleRefWrapper(addr));
            if (!mapRemoveResult)
            {
                bdNChar8 addrStr[22];
                bdAddressMap::addrToString(addr, addrStr, sizeof(addrStr));
                bdUseVAArgs(addrStr);
                result = false;
            }
            const bdBool socketDisconnectResult = m_socket->disconnect(addr);
            if (!socketDisconnectResult)
            {
                bdNChar8 addrStr[22];
                bdAddressMap::addrToString(addr, addrStr, sizeof(addrStr));
                bdUseVAArgs(addrStr);
                result = false;
            }
            toDisconnect.dequeue();
        }
        if (m_status == BD_CONNECTION_STORE_SHUTTING_DOWN && (m_connectionMap.getSize() == 0 || m_shutdownTimer.getElapsedTimeInSeconds() > m_config.m_maxShutdownDuration))
            result = startShutdown(BD_CONNECTION_STORE_SHUTDOWN_IMMEDIATE);
    }
    else
    {
        bdUseVAArgs();
        // Inferred: rejected path returns false (line 0 after the line-192 log).
        result = false;
    }
    return result;
}

// Original line 459.
bdConnectionRef bdConnectionStore::create(bdCommonAddrRef addr, const bdSecurityID & secID)
{
    bdConnectionRef connection;
    // Inferred positive condition: original line 462 tests m_status, line 464 inlines isNull; both failures share the final log tail call.
    if (BD_CONNECTION_STORE_INITIALIZED == m_status
        && !addr.isNull())
    {
        bdAddrHandleRef addrHandle;
        if (m_addrMap)
        {
            const bdBool getHandleResult = m_addrMap->getAddrHandle(addr, secID, addrHandle);
            if (getHandleResult)
            {
                if (m_connectionMap.get(bdAddrHandleRefWrapper(addrHandle), connection))
                {
                    bdUseVAArgs();
                    if (addrHandle->getStatus() == bdAddrHandle::BD_ADDR_NOT_RESOLVED && m_socket->isRelayEnabled())
                        m_socket->connect(addrHandle);
                }
                else if (addr->isLoopback())
                {
                    bdUseVAArgs();
                    connection = new bdLoopbackConnection(addr);
                    connection->setAddressHandle(addrHandle);
                    if (!m_connectionMap.put(bdAddrHandleRefWrapper(addrHandle), connection))
                        connection = NULL;
                }
                else
                {
                    bdUseVAArgs();
                    connection = new bdUnicastConnection(addr, m_addrMap, m_config.m_acceptOutOfOrder);
                    connection->setAddressHandle(addrHandle);
                    bdBool ok = m_socket->connect(addrHandle);
                    // Inferred from line 511 columns: the put test follows ok with &&.
                    ok = ok && m_connectionMap.put(bdAddrHandleRefWrapper(addrHandle), connection);
                    if (!ok)
                    {
                        connection->disconnect();
                        connection->close();
                        connection = NULL;
                    }
                }
            }
            else
            {
                bdUseVAArgs();
                connection = NULL;
            }
        }
        else
        {
            bdUseVAArgs();
        }
    }
    else
    {
        bdUseVAArgs();
    }
    return connection;
}

// Original line 197.
bdBool bdConnectionStore::receiveAll()
{
    bdBool result = true;
    // Inferred positive condition: original line 200 tests the two accepted states (sub 1; cmp 1; b.hi); block 205-367.
    if ((BD_CONNECTION_STORE_INITIALIZED == m_status) || (BD_CONNECTION_STORE_SHUTTING_DOWN == m_status))
    {
        bdUByte8 data[1288];
        bdBool receivedData;
        bdUInt iterationCount = 0;
        do
        {
            bdAddrHandleRef address;
            const bdInt received = m_socket->receiveFrom(address, data, sizeof(data));
            receivedData = received > 0;
            if (received < 0)
            {
                // Inferred: the original error paths store constant receivedData values (line-0 mov w26 in every case and the default).
                receivedData = false;
                const bdSocketStatusCode error = static_cast<bdSocketStatusCode>(received);
                switch (error)
                {
                case BD_NET_CONNECTION_RESET:
                    receivedData = true;
                    result = true;
                    break;
                case BD_NET_WOULD_BLOCK:
                    result = true;
                    break;
                case BD_NET_ERROR:
                case BD_NET_SUBSYTEM_ERROR:
                case BD_NET_ADDRESS_IN_USE:
                case BD_NET_MSG_SIZE:
                case BD_NET_ADDRESS_INVALID:
                case BD_NET_BLOCKING_CALL_CANCELED:
                case BD_NET_NOT_BOUND:
                case BD_NET_INVALID_HANDLE:
                case BD_NET_NOT_CONNECTED:
                case BD_NET_CONNECTION_REFUSED:
                case BD_NET_HOST_UNREACH:
                    bdUseVAArgs(error);
                    result = false;
                    break;
                }
            }
            else
            {
                const bdUInt bytesReceived = received;
                bdConnectionRef receivedConnection;
                if (!address)
                    bdUseVAArgs();
                else if (m_connectionMap.get(bdAddrHandleRefWrapper(address), receivedConnection) && receivedConnection && receivedConnection->receive(data, bytesReceived))
                {
                    if (!receivedConnection->getAddressHandle()->getRealAddr().getAddress().isValid())
                        receivedConnection->setAddressHandle(address);
                }
                else if (m_status != BD_CONNECTION_STORE_SHUTTING_DOWN)
                {
                    bdUnicastConnection newConnection(m_addrMap, m_config.m_acceptOutOfOrder);
                    newConnection.setAddressHandle(address);
                    bdBool ok = newConnection.receive(data, bytesReceived);
                    if (ok)
                    {
                        if (newConnection.getStatus() == bdConnection::BD_CONNECTED)
                        {
                            const bdConnectionRef establishedConnection = new bdUnicastConnection(m_addrMap, m_config.m_acceptOutOfOrder);
                            establishedConnection->setAddressHandle(address);
                            ok = establishedConnection->receive(data, bytesReceived) && establishedConnection->getStatus() == bdConnection::BD_CONNECTED;
                            if (ok)
                            {
                                ok = m_connectionMap.put(bdAddrHandleRefWrapper(address), establishedConnection);
                                if (ok)
                                    bdUseVAArgs();
                            }
                        }
                        else
                            flush(newConnection);
                    }
                    else
                        result = false;
                }
            }
        } while (receivedData && iterationCount++ < m_config.m_maxReceiveIterations);
    }
    else
    {
        bdUseVAArgs();
        // Inferred: original rejected path materialises false into the return register (line 0 after the line-370 log).
        result = false;
    }
    return result;
}
