// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdFeatureBan/bdFeatureBanInfo.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdFeatureBanInfo : public bdTaskResult {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdFeatureBanInfo);
    bdFeatureBanInfo();
    virtual ~bdFeatureBanInfo();  // vtable slot 0
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdUInt64 m_category;  // 0x08
    bdUInt32 m_secondsRemaining;  // 0x10
};
static_assert(sizeof(bdFeatureBanInfo) == 0x28, "bdFeatureBanInfo");

