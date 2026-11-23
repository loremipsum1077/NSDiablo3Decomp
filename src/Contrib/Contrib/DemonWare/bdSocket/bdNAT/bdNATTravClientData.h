// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdNAT/bdNATTravClientData.h

#pragma once

#include "../../bdCore/bdSocket/bdEndpoint.h"
#include "../../bdCore/bdTiming/bdStopwatch.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdAddr;
class bdNATTravListener;

class bdNATTravClientData {  // size 0x48
public:
    enum bdNATTravClientDataState {
        BD_NTCDS_INITIAL = 0,
        BD_NTCDS_STAGE_1 = 1,
        BD_NTCDS_STAGE_2 = 2,
        BD_NTCDS_STAGE_3 = 3,
    };
public:
    bdNATTravClientData();
    bdNATTravClientData(bdCommonAddrRef, bdCommonAddrRef, bdNATTravListener *);
    void callOnNATAddrDiscovery(bdCommonAddrRef remote, const bdAddr & realAddr);
    void callOnNATAddrDiscoveryFailed(bdCommonAddrRef remote);
    bdNATTravClientData::bdNATTravClientDataState m_state;  // 0x00
    bdCommonAddrRef m_local;  // 0x08
    bdCommonAddrRef m_remote;  // 0x10
    bdNATTravListener * m_listener;  // 0x18
    bdNATTravListener * m_secondaryListener;  // 0x20
    bdUInt m_tries;  // 0x28
    bdStopwatch m_lastSent;  // 0x30
    bdBool m_throttled;  // 0x38
    bdStopwatch m_age;  // 0x40
};
static_assert(sizeof(bdNATTravClientData) == 0x48, "bdNATTravClientData");


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  bdNATTravClientData::bdNATTravClientData(const bdNATTravClientData &)  [inlined into 4 functions]
//   line    17  bdNATTravClientData & bdNATTravClientData::operator=(const bdNATTravClientData &)  [inlined into 1 functions]
//   line    17  bdNATTravClientData::~bdNATTravClientData()  [inlined into 9 functions]
