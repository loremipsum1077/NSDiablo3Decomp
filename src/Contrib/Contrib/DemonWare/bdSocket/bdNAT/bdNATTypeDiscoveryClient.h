// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdNATTypeDiscoveryClient.h

#pragma once

#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdNATTypeDiscoveryConfig.h"
#include "bdPacket/bdNATTypeDiscoveryPacket.h"
#include <stddef.h>
#include <stdint.h>

class bdNATTypeDiscoveryPacketReply;

class bdNATTypeDiscoveryClient {  // size 0x1c0
public:
    static void * operator new(size_t nbytes);
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdNATTypeDiscoveryClientState {
        BD_NTDCS_UNINITIALIZED = 0,
        BD_NTDCS_RUN_TEST_1 = 1,
        BD_NTDCS_RUN_TEST_2 = 2,
        BD_NTDCS_RUN_TEST_3 = 3,
        BD_NTDCS_FINI = 4,
        BD_NTDCS_ERROR = 5,
    };
    bdNATTypeDiscoveryClient();
    ~bdNATTypeDiscoveryClient();
    bdBool init(bdDefaultSocket * socket, const bdAddr & serverAddr, bdNATTypeDiscoveryConfig config);
    void pump(bdAddr fromAddr, bdUByte8 * data, const bdInt dataSize);
    bdBool quit();
    bdBool isRunning() const;
    bdNATType getNATType() const;
    bdNATTypeDiscoveryClient::bdNATTypeDiscoveryClientState getStatus() const;
    void getConfig(bdNATTypeDiscoveryConfig &) const;
protected:
    bdBool sendForTest1();
    bdBool sendForTest2();
    bdBool sendForTest3();
    void handleResponse(const bdAddr & addr, const bdNATTypeDiscoveryPacketReply & reply);
    bdBool sendNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacket::bdNATTypeDiscoveryPacketRequest packetType, const bdAddr & serverAddr);
    void receiveReplies(bdAddr fromAddr, bdUByte8 * data, const bdInt dataSize);
    void pumpActiveTest();
    bdDefaultSocket * m_socket;  // 0x00
    bdAddr m_serverAddr1;  // 0x08
    bdAddr m_serverAddr2;  // 0x90
    bdAddr m_mappedAddr;  // 0x118
    bdNATType m_NATType;  // 0x1a0
    bdNATTypeDiscoveryClient::bdNATTypeDiscoveryClientState m_state;  // 0x1a4
    bdUByte8 m_resends;  // 0x1a8
    bdStopwatch m_timer;  // 0x1b0
    bdNATTypeDiscoveryConfig m_config;  // 0x1b8
};
static_assert(sizeof(bdNATTypeDiscoveryClient) == 0x1c0, "bdNATTypeDiscoveryClient");


// Functions defined in this file (bodies are not in the DWARF):
//   line    29  void bdNATTypeDiscoveryClient::operator delete(void * p)  [inlined into 3 functions]
//   line    29  void * bdNATTypeDiscoveryClient::operator new(size_t nbytes)  [inlined into 1 functions]
