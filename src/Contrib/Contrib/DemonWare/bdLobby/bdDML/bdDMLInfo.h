// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdDML/bdDMLInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdDMLInfo : public bdTaskResult {  // size 0x138
public:
    BD_CLASS_ALLOCATOR(bdDMLInfo);
    bdDMLInfo();
    virtual ~bdDMLInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    void get2DCartesianLocation(bdFloat32 &, bdFloat32 &, bdFloat32);
    bdNChar8 m_countryCode[3];  // 0x08
    bdNChar8 m_country[65];  // 0x0b
    bdNChar8 m_region[65];  // 0x4c
    bdNChar8 m_city[129];  // 0x8d
    bdNChar8 m_timezone[9];  // 0x10e
    bdFloat32 m_latitude;  // 0x118
    bdFloat32 m_longitude;  // 0x11c
    bdUInt32 m_asn;  // 0x120
};
static_assert(sizeof(bdDMLInfo) == 0x138, "bdDMLInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    31  void bdDMLInfo::operator delete(void * p)  [inlined into 1 functions]
