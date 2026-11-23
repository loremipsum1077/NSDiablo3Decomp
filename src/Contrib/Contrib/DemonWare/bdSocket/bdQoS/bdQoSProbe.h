// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdQoSProbe.h

#pragma once

#include "../../bdCore/bdContainers/bdArray.h"
#include "../../bdCore/bdContainers/bdFastArray.h"
#include "../../bdCore/bdContainers/bdHashMap.h"
#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdCore/bdSocket/bdSecurityID.h"
#include "../../bdCore/bdSocket/bdSecurityKey.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdNAT/bdNATTravListener.h"
#include "../bdPacketInterceptor.h"
#include "bdPacket/bdQoSReplyPacket.h"
#include "bdQosHostStats.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTravClient;
struct bdQoSHostStats;
class bdQoSProbeListener;
class bdQoSRemoteAddr;
class bdQoSRequestPacket;
class bdServiceBandwidthArbitrator;

class bdQoSProbe : public bdNATTravListener, public bdPacketInterceptor {  // size 0x118
public:
    BD_CLASS_ALLOCATOR(bdQoSProbe);
    enum bdQoSProbeStatus {
        BD_QOS_PROBE_UNINITIALIZED = 0,
        BD_QOS_PROBE_INITIALIZED = 1,
        BD_QOS_PROBE_RUNNING = 2,
    };
    void setMaxBandwidth(const bdUInt);
    bdBool probe(bdQoSRemoteAddr &, bdQoSProbeListener *, bdUInt);
    bdBool probe(const bdArray<bdQoSRemoteAddr> &, bdQoSProbeListener *, bdUInt);
    void cancelProbes();
    void pump();
    bdBool listen(const bdSecurityID &, bdUByte8 *, bdUInt);
    bdBool listen(const bdFastArray<bdSecurityID> &, bdUByte8 *, bdUInt);
    void registerSecId(const bdSecurityID &);
    bdBool setData(bdUByte8 * data, bdUInt dataSize);
    void disableListener();
    void enableListener();
    void shutdownListener();
    bdBool isProbing() const;
    bdBool getListenStats(const bdSecurityID &, bdQoSHostStats &);
protected:
    bdUInt m_maxBandwidth;  // 0x10
    bdBool probe(bdCommonAddrRef, const bdSecurityID &, const bdSecurityKey &, bdQoSProbeListener *, bdUInt);
public:
    bdQoSProbe();
    bdQoSProbe(bdDefaultSocket *, bdNATTravClient *, bdServiceBandwidthArbitrator *);
    virtual ~bdQoSProbe();  // vtable slot 0
    bdBool init(bdDefaultSocket * socket, bdNATTravClient * natTrav, bdServiceBandwidthArbitrator * bandArb);
    bdBool quit();
    virtual void onNATAddrDiscovery(bdCommonAddrRef remote, const bdAddr & realAddr);  // vtable slot 2
    virtual void onNATAddrDiscoveryFailed(bdCommonAddrRef remote);  // vtable slot 3
    virtual bdBool acceptPacket(bdDefaultSocket *, bdAddr addr, void * data, const bdUInt size, const bdUByte8 type);  // vtable slot 4
    void setMaxRetries(bdUInt);
    void setProbeTimeout(bdFloat32);
    void setMaxConcurrentProbes(bdUInt);
    void useMultiplePacketsPerProbe(bdBool);
protected:
    bdBool handleRequest(bdQoSRequestPacket & packet, bdAddr & addr, bdInt32 packetSize);
    bdBool handleReply(bdQoSReplyPacket & packet, bdAddr & addr, bdUInt packetSize);
    bdUInt shrinkSecId(const bdSecurityID & id) const;
    bdDefaultSocket * m_socket;  // 0x18
    bdNATTravClient * m_natTrav;  // 0x20
    bdServiceBandwidthArbitrator * m_bandArb;  // 0x28
    bdUInt32 m_nextProbeId;  // 0x30
    bdStopwatch m_probingTimer;  // 0x38
    bdFastArray<unsigned int> m_secids;  // 0x40
    bdQoSReplyPacket m_replyData;  // 0x50
    enum bdQoSListenerState {
        BD_QOS_OFF = 0,
        BD_QOS_DISABLED = 1,
        BD_QOS_ENABLED = 2,
    };
    bdQoSProbe::bdQoSListenerState m_listenState;  // 0x88
    bdQoSProbe::bdQoSProbeStatus m_status;  // 0x8c
    bdUInt m_maxRetries;  // 0x90
    bdFloat32 m_probeTimeout;  // 0x94
    bdFloat32 m_hostSideTimeout;  // 0x98
    bdUInt m_maxConcurrentProbes;  // 0x9c
    bdBool m_useMultiplePacketsPerProbe;  // 0xa0
    struct bdQosProbeClient {  // size 0x20
        static void * operator new(size_t);
        static void operator delete(void *);
        static void * operator new[](size_t);
        static void operator delete[](void *);
        static void * operator new(size_t, void *);
        static void operator delete(void *, void *);
        static void * operator new[](size_t, void *);
        static void operator delete[](void *, void *);
        bdQosProbeClient() : m_numPacketsReceived(0), m_bandwidthUp(0), m_bandwidthDown(0), m_timeBetweenPackets(0) { m_latency = m_minLatency = 2147483647.0f; m_timer.reset(); }
        bdUInt m_numPacketsReceived;  // 0x00
        bdUInt m_bandwidthUp;  // 0x04
        bdUInt m_bandwidthDown;  // 0x08
        bdUInt m_timeBetweenPackets;  // 0x0c
        bdFloat32 m_latency;  // 0x10
        bdFloat32 m_minLatency;  // 0x14
        bdStopwatch m_timer;  // 0x18
        void reset() { m_numPacketsReceived = m_bandwidthUp = m_bandwidthDown = m_timeBetweenPackets = 0; m_latency = m_minLatency = 2147483647.0f; m_timer.reset(); }
    };
    struct bdQosProbeHost {  // size 0xc8
        static void * operator new(size_t);
        static void operator delete(void *);
        static void * operator new[](size_t);
        static void operator delete[](void *);
        static void * operator new(size_t, void *);
        static void operator delete(void *, void *);
        static void * operator new[](size_t, void *);
        static void operator delete[](void *, void *);
        bdQosProbeHost() : m_numPacketsReceived(0), m_completed(false), m_bandwidth(0), m_id(0), m_probeTimestamp(0), m_needToSendData(false), m_secid(0) { m_timer.reset(); m_holdTimePacket1.reset(); m_holdTimePacket2.reset(); }
        bdUInt32 m_numPacketsReceived;  // 0x00
        bdAddr m_realAddr;  // 0x04
        bdStopwatch m_timer;  // 0x90
        bdStopwatch m_holdTimePacket1;  // 0x98
        bdStopwatch m_holdTimePacket2;  // 0xa0
        bdBool m_completed;  // 0xa8
        bdUInt32 m_bandwidth;  // 0xac
        bdUInt32 m_id;  // 0xb0
        bdUInt64 m_probeTimestamp;  // 0xb8
        bdBool m_needToSendData;  // 0xc0
        bdUInt32 m_secid;  // 0xc4
    };
    struct bdQoSProbeEntryWrapper {  // size 0x138
        static void * operator new(size_t);
        static void operator delete(void *);
        static void * operator new[](size_t);
        static void operator delete[](void *);
        static void * operator new(size_t, void *);
        static void operator delete(void *, void *);
        static void * operator new[](size_t, void *);
        static void operator delete[](void *, void *);
        bdQoSProbeEntryWrapper() : m_listener(0), m_retries(0), m_numRepliesReceived(0), m_numRepliesExpected(0), m_latency(0), m_bandwidthUp(0), m_bandwidthDown(0) { m_lastTry.reset(); m_minLatency = 2147483647.0f; }
        bdCommonAddrRef m_addr;  // 0x00
        bdSecurityID m_id;  // 0x08
        bdSecurityKey m_key;  // 0x10
        bdQoSProbeListener * m_listener;  // 0x20
        bdAddr m_realAddr;  // 0x28
        bdUInt m_retries;  // 0xb0
        bdStopwatch m_lastTry;  // 0xb8
        bdUInt m_numRepliesReceived;  // 0xc0
        bdUInt m_numRepliesExpected;  // 0xc4
        bdFloat32 m_latency;  // 0xc8
        bdFloat32 m_minLatency;  // 0xcc
        bdFloat32 m_replies[16];  // 0xd0
        bdUInt m_bandwidthUp;  // 0x110
        bdUInt m_bandwidthDown;  // 0x114
        bdQoSProbe::bdQosProbeClient m_currentProbe;  // 0x118
    };
public:
    bdHashMap<bdReference<bdCommonAddr>, bdArray<bdQoSProbe::bdQoSProbeEntryWrapper>, bdCommonAddrHash> m_probesResolving;  // 0xa8
    bdHashMap<unsigned int, bdQoSProbe::bdQoSProbeEntryWrapper, bdHashingClass> m_probesProbing;  // 0xc8
    bdHashMap<unsigned int, bdQoSProbe::bdQosProbeHost, bdHashingClass> m_probesProcessing;  // 0xe8
    bdArray<bdQoSHostStats> m_statsPerSession;  // 0x108
private:
    void singleProbeComplete(bdQoSReplyPacket & packet, bdQoSProbe::bdQoSProbeEntryWrapper & entry, bdUInt packetSize);
    bdUInt32 calculateBandwidth(bdFloat32 timeDiffBetweenPackets, bdUInt32 packetSize);
    bdBool sendReply(const bdAddr & addr, const bdQoSProbe::bdQosProbeHost & probe);
    void getPacketSendSize(bdUInt serializedSize, bdUInt & numPackets, bdUInt & totalSize);
    void checkHostProbes();
    bdBool processRequestPackets(const bdQoSRequestPacket & packet, const bdAddr & addr, bdInt32 packetSize);
    void sendRequest(bdBool & stopProcessing, bdQoSProbe::bdQoSProbeEntryWrapper & entry, bdUInt32 id);
    void logProbeRequests(bdUInt32 secID, bdBool requestingData);
    void logProbeReplys(bdUInt32 secID, bdBool sendingData);
    void logProbeDiscards(bdUInt32 secID);
    bdBool findHostStats(const bdUInt32 & secID, bdUInt32 & index);
};
static_assert(sizeof(bdQoSProbe) == 0x118, "bdQoSProbe");


// Functions defined in this file (bodies are not in the DWARF):
//   line    95  void bdQoSProbe::operator delete(void * p)  [inlined into 1 functions]
//   line   365  bdQoSProbe::bdQosProbeClient::bdQosProbeClient()  [inlined into 1 functions]
//   line   385  void bdQoSProbe::bdQosProbeClient::reset()  [inlined into 1 functions]
//   line   397  bdQoSProbe::bdQosProbeHost::bdQosProbeHost(const bdQoSProbe::bdQosProbeHost &)  [inlined into 2 functions]
//   line   397  bdQoSProbe::bdQosProbeHost::~bdQosProbeHost()  [inlined into 5 functions]
//   line   403  bdQoSProbe::bdQosProbeHost::bdQosProbeHost()  [inlined into 2 functions]
//   line   431  bdQoSProbe::bdQoSProbeEntryWrapper::bdQoSProbeEntryWrapper(const bdQoSProbe::bdQoSProbeEntryWrapper &)  [inlined into 4 functions]
//   line   431  bdQoSProbe::bdQoSProbeEntryWrapper & bdQoSProbe::bdQoSProbeEntryWrapper::operator=(const bdQoSProbe::bdQoSProbeEntryWrapper &)  [inlined into 2 functions]
//   line   431  bdQoSProbe::bdQoSProbeEntryWrapper::~bdQoSProbeEntryWrapper()  [inlined into 12 functions]
//   line   437  bdQoSProbe::bdQoSProbeEntryWrapper::bdQoSProbeEntryWrapper()  [inlined into 1 functions]
