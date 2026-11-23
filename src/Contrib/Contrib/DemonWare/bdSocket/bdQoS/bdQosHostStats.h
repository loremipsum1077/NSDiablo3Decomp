// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdQosHostStats.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

struct bdQoSHostStats {  // size 0x38
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdQoSHostStats(); // Constructor is not observed by this campaign.
    ~bdQoSHostStats() {}
    bdUInt64 m_numDataRequestsReceived;  // 0x00
    bdUInt64 m_numProbesReceived;  // 0x08
    bdUInt64 m_numSlotsFullDiscards;  // 0x10
    bdUInt64 m_numDataRepliesSent;  // 0x18
    bdUInt64 m_numDataReplyBytesSent;  // 0x20
    bdUInt64 m_numProbeRepliesSent;  // 0x28
    bdUInt32 m_secID;  // 0x30
};
static_assert(sizeof(bdQoSHostStats) == 0x38, "bdQoSHostStats");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdQoSHostStats::bdQoSHostStats()  [inline]
