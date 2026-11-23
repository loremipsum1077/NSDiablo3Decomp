// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdServiceBandwidthArbitrator.h

#pragma once

#include "../bdCore/bdMemory/bdMemory-std.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdServiceBandwidthArbitrator {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdServiceBandwidthArbitrator);
    bdServiceBandwidthArbitrator();
    void reset();
    void addSliceQuota(bdUInt bytes);
    bdBool allowedSend(bdUInt) const;
    void send(bdUInt bytes);
protected:
    bdInt m_allowedSend;  // 0x00
    bdBool m_sentThisSlice;  // 0x04
};
static_assert(sizeof(bdServiceBandwidthArbitrator) == 0x8, "bdServiceBandwidthArbitrator");

