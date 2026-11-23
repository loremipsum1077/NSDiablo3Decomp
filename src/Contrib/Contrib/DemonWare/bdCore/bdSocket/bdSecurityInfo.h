// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdSocket/bdSecurityInfo.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdSecurityID;
class bdSecurityKey;

class bdSecurityInfo {  // size 0x1
public:
    static bdUInt toString(const bdSecurityID & id, bdNChar8 * buffer, bdUInt len);
    static bdUInt toString(const bdSecurityKey & key, bdNChar8 * buffer, bdUInt len);
protected:
    static bdUInt bufferToString(const bdUByte8 * buffer, const bdUInt bufferLen, bdNChar8 * outBuffer, const bdUInt outBufferLen);
    bdSecurityInfo();
};
static_assert(sizeof(bdSecurityInfo) == 0x1, "bdSecurityInfo");

