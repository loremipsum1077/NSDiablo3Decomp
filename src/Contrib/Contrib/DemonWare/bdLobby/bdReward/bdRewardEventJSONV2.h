// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEventJSONV2.h

#pragma once

#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdString/bdString.h"
#include "bdRewardEvent.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRewardEventJSONV2 : public bdRewardEvent {  // size 0x30
public:
    BD_CLASS_ALLOCATOR(bdRewardEventJSONV2)
    bdRewardEventJSONV2();
    explicit bdRewardEventJSONV2(const bdString &);
    explicit bdRewardEventJSONV2(const bdNChar8 *);
    virtual ~bdRewardEventJSONV2();  // vtable slot 0
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    virtual bdUInt sizeOf();  // vtable slot 3
    const bdNChar8 * getJsonData() const;
    bdBool setJsonData(const bdNChar8 *);
    void reset();
protected:
    bdString m_jsonData;  // 0x18
};
static_assert(sizeof(bdRewardEventJSONV2) == 0x30, "bdRewardEventJSONV2");
