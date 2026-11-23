// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEventJSON.h

#pragma once

#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "bdRewardEvent.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRewardEventJSON : public bdRewardEvent {  // size 0x828
public:
    BD_CLASS_ALLOCATOR(bdRewardEventJSON)  // line 22
    bdRewardEventJSON();
    bdRewardEventJSON(const bdNChar8 *);
    virtual ~bdRewardEventJSON();  // vtable slot 0
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    bdBool set(const bdNChar8 * jsonData);
    void reset();
    bdNChar8 m_jsonData[2048];  // 0x14
};
static_assert(sizeof(bdRewardEventJSON) == 0x828, "bdRewardEventJSON");
