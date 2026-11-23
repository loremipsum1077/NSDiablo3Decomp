// Original file: Contrib/Contrib/DemonWare/bdConnection/bdConnectionStore.h
// Completed from the DWARF (tools/gen_headers.py reference).

#pragma once

#include "bdConnectionListener.h"
#include "bdConnectionStoreConfig.h"
#include "bdDispatch/bdDispatcher.h"
#include "../bdCore/bdContainers/bdHashMap.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "../bdCore/bdTiming/bdStopwatch.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdSocket/bdAddrHandleRefWrapper.h"
#include <stddef.h>
#include <stdint.h>

class bdAddressMap;
class bdConnection;
class bdDispatchInterceptor;
class bdSecurityID;
class bdSocketRouter;

typedef bdReference<bdConnection> bdConnectionRef;

class bdConnectionStore {  // size 0x60
public:
    BD_CLASS_ALLOCATOR(bdConnectionStore);
    enum bdConnectionStoreStatus {
        BD_CONNECTION_STORE_UNINITIALIZED = 0,
        BD_CONNECTION_STORE_INITIALIZED = 1,
        BD_CONNECTION_STORE_SHUTTING_DOWN = 2,
        BD_CONNECTION_STORE_ERROR = 3,
    };
    enum bdConnectionStoreShutdownType {
        BD_CONNECTION_STORE_SHUTDOWN_ELEGANT = 0,
        BD_CONNECTION_STORE_SHUTDOWN_IMMEDIATE = 1,
    };
    bdConnectionStore();
    ~bdConnectionStore();
    bdBool init(bdSocketRouter * socket, const bdConnectionStoreConfig & config);
    bdBool flushAll();
    bdBool receiveAll();
    void dispatchAll();
    bdBool startShutdown(const bdConnectionStore::bdConnectionStoreShutdownType shutdownType);
    bdConnectionRef create(bdCommonAddrRef addr, const bdSecurityID & secID);
    bdConnectionStore::bdConnectionStoreStatus getStatus() const;
    void registerDispatchInterceptor(bdDispatchInterceptor *const interceptor);
    void unregisterDispatchInterceptor(bdDispatchInterceptor *const interceptor);
    void setConfig(bdConnectionStoreConfig &);
    void getConfig(bdConnectionStoreConfig &) const;
protected:
    bdUInt flush(bdConnection & connection);
    bdSocketRouter * m_socket;  // 0x00
    bdHashMap<bdAddrHandleRefWrapper, bdReference<bdConnection>, bdAddrHandleRefWrapper> m_connectionMap;  // 0x08
    bdConnectionStore::bdConnectionStoreStatus m_status;  // 0x28
    bdDispatcher m_dispatcher;  // 0x30
    bdStopwatch m_shutdownTimer;  // 0x40
    bdAddressMap * m_addrMap;  // 0x48
    bdConnectionStoreConfig m_config;  // 0x50
};
static_assert(sizeof(bdConnectionStore) == 0x60, "bdConnectionStore");
