// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdExtendedAuthInfo.h

#pragma once

#include "../bdLobbyCommon/bdSerializable.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdExtendedAuthInfo : public bdSerializable {  // size 0x1020
public:
    BD_CLASS_ALLOCATOR(bdExtendedAuthInfo);
    bdExtendedAuthInfo();
    virtual ~bdExtendedAuthInfo();  // vtable slot 0
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 4
    const bdNChar8 * getData() const;
protected:
    friend class bdAuthSwitch;  // friends are not in the DWARF; bdAuthSwitch::processPlatformData calls setData
    bdBool setData(const bdNChar8 * data);
    bdNChar8 m_data[4097];  // 0x08
    bdBool m_initialized;  // 0x1009
};
static_assert(sizeof(bdExtendedAuthInfo) == 0x1020, "bdExtendedAuthInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    24  void bdExtendedAuthInfo::operator delete(void * p)  [inlined into 1 functions]
