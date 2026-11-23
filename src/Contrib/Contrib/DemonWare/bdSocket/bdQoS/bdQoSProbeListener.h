// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdQoSProbeListener.h

#pragma once

#include "../../bdCore/bdSocket/bdAddr.h"
#include "../../bdCore/bdSocket/bdCommonAddr.h"
#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdQoSProbeInfo {  // size 0xf8
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdQoSProbeInfo();
    ~bdQoSProbeInfo() {} // Original inline destructor at line 16.
    bdCommonAddrRef m_addr;  // 0x00
    bdAddr m_realAddr;  // 0x08
    bdFloat32 m_latency;  // 0x90
    bdUByte8 * m_data;  // 0x98
    bdUInt m_dataSize;  // 0xa0
    bdBool m_disabled;  // 0xa4
    bdUInt m_bandwidthDown;  // 0xa8
    bdUInt m_bandwidthUp;  // 0xac
    bdFloat32 m_minLatency;  // 0xb0
    bdUInt32 m_numReplies;  // 0xb4
    bdFloat32 m_replies[16];  // 0xb8
};
static_assert(sizeof(bdQoSProbeInfo) == 0xf8, "bdQoSProbeInfo");

class bdQoSProbeListener {  // size 0x8
    // vtable pointer
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    bdQoSProbeListener();
    virtual ~bdQoSProbeListener();  // vtable slot 0
    virtual void onQoSProbeSuccess(const bdQoSProbeInfo &) = 0;  // vtable slot 2
    virtual void onQoSProbeFail(bdCommonAddrRef) = 0;  // vtable slot 3
};
static_assert(sizeof(bdQoSProbeListener) == 0x8, "bdQoSProbeListener");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdQoSProbeInfo::~bdQoSProbeInfo()  [inlined into 1 functions]
