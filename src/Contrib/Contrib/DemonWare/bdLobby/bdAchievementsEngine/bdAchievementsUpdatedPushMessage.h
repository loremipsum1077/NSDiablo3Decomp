// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementsUpdatedPushMessage.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdContainers/bdArray.h"
#include "../bdLobbyCommon/bdStructBufferSerializable.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdAchievementState;
class bdString;
class bdStructBufferDeserializer;
class bdStructBufferSerializer;

class bdAchievementsUpdatedPushMessage : public bdStructBufferSerializable {  // size 0x28
public:
    BD_CLASS_ALLOCATOR(bdAchievementsUpdatedPushMessage);
    bdAchievementsUpdatedPushMessage();
    bdAchievementsUpdatedPushMessage(const bdAchievementState &);
    bool operator==(const bdAchievementsUpdatedPushMessage & rhs) const;
    bool operator!=(const bdAchievementsUpdatedPushMessage &) const;
    bdUInt getNumStates() const;
    const bdAchievementState & getState(bdUInt) const;
    virtual bdBool serialize(bdStructBufferSerializer &) const;  // vtable slot 2
    virtual bdBool deserialize(bdStructBufferDeserializer & deserializer);  // vtable slot 3
    bdString toString() const;
protected:
    bdArray<bdAchievementState> m_states;  // 0x08
};
static_assert(sizeof(bdAchievementsUpdatedPushMessage) == 0x28, "bdAchievementsUpdatedPushMessage");


// Functions defined in this file (bodies are not in the DWARF):
//   line    16  bdAchievementsUpdatedPushMessage::~bdAchievementsUpdatedPushMessage()  [0x7100b45920 (+1 more), inlined into 3 functions]
//   line    19  void bdAchievementsUpdatedPushMessage::operator delete(void * p)  [inlined into 1 functions]
