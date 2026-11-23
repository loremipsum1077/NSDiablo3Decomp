// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdNATTravClient.h

#pragma once

#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdContainers/bdHashMap.h"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdCore/bdThread/bdMutex.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdPacketInterceptor.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTravClientData;
class bdNATTravListener;
class bdNATTraversalPacket;
class bdServiceBandwidthArbitrator;

class bdNATTravClient : public bdPacketInterceptor {  // size 0x100
public:
    static void * operator new(size_t);
    static void operator delete(void * p);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdNATTravClientStatus {
        BD_NAT_TRAV_UNINITIALIZED = 0,
        BD_NAT_TRAV_INITIALIZED = 1,
        BD_NAT_TRAV_RUNNING = 2,
    };
    bdNATTravClient();
    virtual ~bdNATTravClient();  // vtable slot 0
    bdBool init(bdDefaultSocket * socket, bdServiceBandwidthArbitrator * bandArb, bdCommonAddrRef localCommonAddr);
    bdBool connect(bdCommonAddrRef remote, bdNATTravListener * listener, bdBool throttle);
    void pump();
    bdBool quit();
    virtual bdBool acceptPacket(bdDefaultSocket *, bdAddr addr, void * data, const bdUInt size, const bdUByte8 type);  // vtable slot 2
    void cancelConnect(bdCommonAddrRef remote);
    bdNATTravClient::bdNATTravClientStatus getStatus();
    bdBool setupIntroducers(const bdArray<bdAddr> & introducers);
    bdBool updateLocalCommonAddr(const bdCommonAddrRef newLocalCommonAddr);
    bdBool isConnectInProgress(const bdCommonAddrRef remote) const;
    bdBool getUsageStats(bdUInt64 *, bdUInt64 *, bdUInt64 *, bdUInt64 *);
    void resetUsageStats();
    void setNATTravTimeout(bdFloat32);
    void setNATTravInterleaveMode(bdBool);
protected:
    void receiveFrom(const bdAddr & addr, bdNATTraversalPacket & packet);
    bdBool sendKeepAlive();
    bdBool sendStage1(bdNATTravClientData & data);
    bdBool sendStage2(bdNATTravClientData & data);
    bdBool sendStage3(bdNATTravClientData &);
    bdBool doHMac(const bdUInt32 identifier, const bdAddr & src, const bdAddr & dest, bdUByte8 * data) const;
    bdBool connectionAllowed(bdCommonAddrRef, bdCommonAddrRef);
    bdServiceBandwidthArbitrator * m_bandArb;  // 0x08
    bdArray<bdAddr> m_introducers;  // 0x10
    bdDefaultSocket * m_socket;  // 0x20
    bdHashMap<unsigned int, bdNATTravClientData, bdHashingClass> m_callbacks;  // 0x28
    bdStopwatch m_keepAliveTimer;  // 0x48
    bdUByte8 m_secretKey[28];  // 0x50
    bdNATTravClient::bdNATTravClientStatus m_status;  // 0x6c
    bdCommonAddrRef m_localCommonAddr;  // 0x70
    struct CachedTraversal {  // size 0x8c
        bdUInt m_remoteAddressHash;  // 0x00
        bdAddr m_realAddress;  // 0x04
    };
    typedef bdNATTravClient::CachedTraversal CachedTraversal;
    static const bdAddr * getFromCache(bdUInt remoteAddressHash);
    static void saveInCache(bdUInt remoteAddressHash, const bdAddr & realAddress);
    static bdNATTravClient::CachedTraversal m_cachedTraversals[];
    static bdUInt m_nextCachedTraversal;
    static bdBool m_cacheFull;
    static bdMutex m_cacheLock;
    bdUInt64 m_traversalCount[4];  // 0x78
    bdUInt64 m_traversalDurationAggr[4];  // 0x98
    bdUInt64 m_traversalDurationAggrSq[4];  // 0xb8
    bdUInt64 m_traversalFailureCount[4];  // 0xd8
    bdFloat32 m_natTravTimeout;  // 0xf8
    bdBool m_interleaveStages;  // 0xfc
};
static_assert(sizeof(bdNATTravClient) == 0x100, "bdNATTravClient");


// Functions defined in this file (bodies are not in the DWARF):
//   line    57  void bdNATTravClient::operator delete(void * p)  [inlined into 1 functions]
//   line   265  bdNATTravClient::CachedTraversal::CachedTraversal()  [inline]
//   line   265  bdNATTravClient::CachedTraversal::~CachedTraversal()  [inlined into 1 functions]

// Original allocator owner bdNATTravClient.h:57.
inline void bdNATTravClient::operator delete(void *p) { bdMemory::deallocate(p); }
