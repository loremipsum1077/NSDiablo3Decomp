// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdConnection/bdUnicastConnection.h

#pragma once

#include "bdConnection.h"
#include "bdConnectionStatistics.h"
#include "bdMessage/bdReceivedMessage.h"
#include "bdPacket/bdChunk.h"
#include "bdWindow/bdUnreliableReceiveWindow.h"
#include "bdWindow/bdUnreliableSendWindow.h"
#include "../bdCore/bdContainers/bdQueue.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdEndpoint.h"
#include "../bdCore/bdTiming/bdStopwatch.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAddressMap;
class bdReliableReceiveWindow;
class bdReliableSendWindow;

class bdInitChunk;
class bdInitAckChunk;
typedef bdReference<bdInitChunk> bdInitChunkRef;

typedef bdReference<bdInitAckChunk> bdInitAckChunkRef;

typedef bdReference<bdMessage> bdMessageRef;

class bdUnicastConnection : public bdConnection {  // size 0x180
public:
    // Inferred friendship: bdConnectionStore::create/receiveAll invoke protected constructors, receive, and stack destructor; friends are absent from DWARF.
    friend class bdConnectionStore;
    BD_CLASS_ALLOCATOR(bdUnicastConnection);
    virtual bdBool connect();  // vtable slot 7
    virtual void disconnect();  // vtable slot 8
    virtual bdBool send(const bdMessageRef message, const bdBool reliable);  // vtable slot 3
    virtual bdConnection::Status getStatus() const;  // vtable slot 6
    virtual void close();  // vtable slot 9
    bdConnectionStatistics & getStats();
protected:
    bdUnicastConnection(bdCommonAddrRef, bdAddressMap *, bdBool);
    bdUnicastConnection(bdAddressMap *, bdBool);
    virtual ~bdUnicastConnection();  // vtable slot 0
    virtual bdBool receive(const bdUByte8 * buffer, const bdUInt32 bufferSize);  // vtable slot 2
    virtual bdUInt getDataToSend(bdUByte8 *const buffer, const bdUInt bufferSize);  // vtable slot 10
    virtual bdBool getMessageToDispatch(bdMessageRef & message);  // vtable slot 4
    virtual bdBool getOutOfOrderMessageToDispatch(bdMessageRef & message);  // vtable slot 5
    void setAcceptOutOfOrder(bdBool);
    bdBool handleData(bdChunkRef & chunk);
    bdBool handleSAck(bdChunkRef & chunk);
    bdBool handleInit(bdChunkRef & chunk);
    bdBool handleInitAck(bdChunkRef & chunk, const bdUInt32 vtag);
    bdBool handleCookieEcho(bdChunkRef & chunk, const bdUInt32);
    bdBool handleCookieAck(bdChunkRef &, const bdUInt32 vtag);
    bdBool handleHeartbeat(bdChunkRef & chunk);
    bdBool handleHeartbeatAck(bdChunkRef &);
    bdBool handleShutdown(bdChunkRef &);
    bdBool handleShutdownAck(bdChunkRef &);
    bdBool handleShutdownComplete(bdChunkRef &);
    bdBool sendInit();
    bdBool sendInitAck(bdInitChunkRef chunk);
    bdBool sendCookieEcho(bdInitAckChunkRef chunk);
    bdBool sendCookieAck();
    bdBool sendHeartbeat(bdInitChunkRef);
    bdBool sendHeartbeatAck(bdInitChunkRef);
    bdBool sendShutdown();
    bdBool sendShutdownAck();
    bdBool sendShutdownComplete();
    void reset();
    void callListenersConnect(const bdBool success);
    void callListenersDisconnect();
    void callListenersReconnect();
    bdBool windowsEmpty() const;
    enum bdUnicastConnectionStatus {
        BD_UC_CLOSED = 0,
        BD_UC_COOKIE_WAIT = 1,
        BD_UC_COOKIE_ECHOED = 2,
        BD_UC_ESTABLISHED = 3,
        BD_UC_SHUTDOWN_PENDING = 4,
        BD_UC_SHUTDOWN_SENT = 5,
        BD_UC_SHUTDOWN_RECEIVED = 6,
        BD_UC_SHUTDOWN_ACK_SENT = 7,
    };
    class bdControlChunkStore {  // size 0x10
    public:
        bdControlChunkStore(bdChunkRef chunk, bdBool lone) : m_chunk(chunk), m_lone(lone) {}
        bdChunkRef m_chunk;  // 0x00
        bdBool m_lone;  // 0x08
    };
    bdConnectionStatistics m_stats;  // 0x38
    bdReliableSendWindow * m_reliableSendWindow;  // 0x78
    bdReliableReceiveWindow * m_reliableRecvWindow;  // 0x80
    bdUnreliableSendWindow m_unreliableSendWindow;  // 0x88
    bdUnreliableReceiveWindow m_unreliableReceiveWindow;  // 0xb0
    bdQueue<bdUnicastConnection::bdControlChunkStore> m_outQueue;  // 0xf0
    bdStopwatch m_sendTimer;  // 0x108
    bdStopwatch m_receiveTimer;  // 0x110
    bdUnicastConnection::bdUnicastConnectionStatus m_state;  // 0x118
    bdUInt32 m_localTag;  // 0x11c
    bdUInt32 m_peerTag;  // 0x120
    bdStopwatch m_initTimer;  // 0x128
    bdUByte8 m_initResends;  // 0x130
    bdStopwatch m_cookieTimer;  // 0x138
    bdUByte8 m_cookieResends;  // 0x140
    bdStopwatch m_shutdownTimer;  // 0x148
    bdUByte8 m_shutdownResends;  // 0x150
    bdStopwatch m_shutdownGuard;  // 0x158
    bdInitAckChunkRef m_initAckChunk;  // 0x160
    bdFloat32 m_smoothedRTT;  // 0x168
    bdFloat32 m_RTTVariation;  // 0x16c
    bdAddressMap * m_addrMap;  // 0x170
    bdBool m_acceptOutOfOrder;  // 0x178
};
static_assert(sizeof(bdUnicastConnection) == 0x180, "bdUnicastConnection");


// Functions defined in this file (bodies are not in the DWARF):
//   line    40  void bdUnicastConnection::operator delete(void * p)  [inlined into 1 functions]
//   line    40  void * bdUnicastConnection::operator new(size_t nbytes)  [inlined into 2 functions]
//   line   163  bdUnicastConnection::bdControlChunkStore::bdControlChunkStore(const bdUnicastConnection::bdControlChunkStore &)  [inlined into 9 functions]
//   line   163  bdUnicastConnection::bdControlChunkStore::~bdControlChunkStore()  [inlined into 11 functions]
//   line   166  bdUnicastConnection::bdControlChunkStore::bdControlChunkStore(bdChunkRef chunk, bdBool lone)  [inlined into 9 functions]
