// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdReward/bdRewardEvent.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;

class bdRewardEvent : public bdTaskResult, public bdSerializable {  // size 0x28
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdRewardEventTypes {
        BD_REWARD_EVENT_DEFAULT = 0,
        BD_REWARD_EVENT_JSON = 1,
    };
    virtual bdBool serialize(bdByteBuffer & buffer) const = 0;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer) = 0;  // vtable slot 2
    virtual bdUInt sizeOf() = 0;  // vtable slot 3
    bdRewardEvent::bdRewardEventTypes getType() const;
protected:
    bdRewardEvent(bdRewardEvent::bdRewardEventTypes);
    virtual ~bdRewardEvent();  // vtable slot 0
    bdRewardEvent(const bdRewardEvent &);
    bdRewardEvent & operator=(const bdRewardEvent &);
    bdRewardEvent::bdRewardEventTypes m_type;  // 0x10
};
static_assert(sizeof(bdRewardEvent) == 0x28, "bdRewardEvent");

