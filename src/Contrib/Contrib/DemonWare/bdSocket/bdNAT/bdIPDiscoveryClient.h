// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdIPDiscoveryClient.h

#pragma once

#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdDefaultSocket.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdIPDiscoveryConfig.h"
#include <stddef.h>
#include <stdint.h>

class bdIPDiscoveryClient {  // size 0x138
public:
    static void * operator new(size_t nbytes);
    static void operator delete(void * p) { bdMemory::deallocate(p); }
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdIPDiscoveryClientStatus {
        BD_IP_DISC_UNINITIALIZED = 0,
        BD_IP_DISC_RUNNING = 1,
        BD_IP_DISC_SUCCESS = 2,
        BD_IP_DISC_FAIL = 3,
        BD_IP_DISC_ERROR = 4,
    };
    bdIPDiscoveryClient();
    ~bdIPDiscoveryClient();
    bdBool init(bdDefaultSocket * socket, const bdAddr & server, bdIPDiscoveryConfig config);
    void pump(bdAddr fromAddr, bdUByte8 * data, const bdInt dataSize);
    void quit();
    const bdAddr & getPublicAddress() const;
    bdIPDiscoveryClient::bdIPDiscoveryClientStatus getStatus() const;
protected:
    bdBool sendIPDiscoveryPacket();
    bdDefaultSocket * m_socket;  // 0x00
    bdAddr m_serverAddr;  // 0x08
    bdAddr m_publicAddr;  // 0x90
    bdStopwatch m_timer;  // 0x118
    bdIPDiscoveryClient::bdIPDiscoveryClientStatus m_status;  // 0x120
    bdIPDiscoveryConfig m_config;  // 0x124
    bdUInt m_retries;  // 0x130
};
static_assert(sizeof(bdIPDiscoveryClient) == 0x138, "bdIPDiscoveryClient");


// Functions defined in this file (bodies are not in the DWARF):
//   line    19  void bdIPDiscoveryClient::operator delete(void * p)  [inlined into 3 functions]
//   line    19  void * bdIPDiscoveryClient::operator new(size_t nbytes)  [inlined into 1 functions]
