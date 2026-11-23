// Original file: Contrib/Contrib/DemonWare/bdNet/bdNet.h
// Completed from the DWARF (tools/gen_headers.py reference).

#pragma once

#include "../bdConnection/bdConnectionStore.h"
#include "../bdCore/bdContainers/bdArray.h"
#include "../bdCore/bdContainers/bdSingleton.h"
#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "bdNetStartParams.h"
#include "bdUPnP/bdUPnP.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdSocket/bdSocketRouter.h"
#include <stddef.h>
#include <stdint.h>

class bdAddr;
class bdGetHostByName;
class bdIPDiscoveryClient;
class bdNATTypeDiscoveryClient;
class bdSocketRouter;
class bdSocketRouterAdHoc;
class bdString;

class bdNetImpl {  // size 0x4dc0
public:
    BD_CLASS_ALLOCATOR(bdNetImpl);
    enum bdNetStatus {
        BD_NET_PARAMS_CONFIG_ERROR = -4,
        BD_NET_BIND_FAILED = -3,
        BD_NET_ONLINE_FAILED = -2,
        BD_NET_INIT_FAILED = -1,
        BD_NET_STOPPED = 0,
        BD_NET_PENDING = 1,
        BD_NET_DONE = 2,
    };
    bdBool start(const bdNetStartParams & params);
    void pump();
    void stop();
    bdNetImpl::bdNetStatus getStatus() const;
    bdBool sendAll();
    bdBool receiveAll();
    void dispatchAll();
    bdConnectionStore * getConnectionStore();
    bdCommonAddrRef getLocalCommonAddr() const;
    const bdNetStartParams & getParams();
    bdSocketRouter * getSocketRouter();
    bdString getStatusAsString() const;
    bdSocketRouterAdHoc * getSocketRouterAdHoc();
protected:
    friend class bdSingleton<bdNetImpl>;
    bdNetImpl();
    virtual ~bdNetImpl();  // vtable slot 0
    static bdBool bindRandPort(bdNetStartParams & params, bdAddr & addr);
    bdBool getBindAddr(bdAddr & addr);
    bdNetStartParams m_params;  // 0x08
    bdConnectionStore m_connectionStore;  // 0x1b0
    bdSocketRouter * m_socketRouter;  // 0x210
    bdNetImpl::bdNetStatus m_status;  // 0x218
    bdIPDiscoveryClient * m_ipDiscClient;  // 0x220
    bdNATTypeDiscoveryClient * m_natTypeDiscClient;  // 0x228
    bdGetHostByName * m_getHostByName;  // 0x230
    bdUPnP m_UPnP;  // 0x238
    bdArray<bdAddr> m_natTravAddrs;  // 0x4da0
    bdUInt m_currentNatTravAddrIndex;  // 0x4db0
    bdUInt m_currentNatTravHostIndex;  // 0x4db4
    bdUInt m_upnpCollisionRetryCount;  // 0x4db8
};
static_assert(sizeof(bdNetImpl) == 0x4dc0, "bdNetImpl");

// bdSingleton<bdNetImpl> is an ordinary instantiation of bdSingleton.inl in every CU that uses it;
// only its m_instance is defined once, in bdNet.cpp (DWARF cu_07b54af1).
