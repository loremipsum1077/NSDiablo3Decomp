// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementState.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "bdAchievementStatus.h"
#include "../bdLobbyCommon/bdStructBufferSerializable.h"
#include "../bdLobbyCommon/bdStructFixedSizeString.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdString;
class bdStructBufferDeserializer;
class bdStructBufferSerializer;

class bdAchievementState : public bdStructBufferSerializable {  // size 0xb0
public:
    BD_CLASS_ALLOCATOR(bdAchievementState);
    bdAchievementState();
    bdAchievementState(const bdNChar8 *, bdUInt16, bdUInt32, bdUInt32, bdAchievementStatus::Enum, bdUInt64, bdUInt64, bdUInt64, bdUInt32);
    bool operator==(const bdAchievementState & rhs) const;
    bool operator!=(const bdAchievementState &) const;
    const bdNChar8 * getName() const;
    bdUInt16 getKind() const;
    bdUInt32 getProgress() const;
    bdUInt32 getProgressTarget() const;
    bdAchievementStatus::Enum getStatus() const;
    bdUInt64 getCompletionTimestamp() const;
    bdUInt64 getActivationTimestamp() const;
    bdUInt64 getExpirationTimestamp() const;
    bdUInt32 getCompletionCount() const;
    virtual bdBool serialize(bdStructBufferSerializer &) const;  // vtable slot 2
    virtual bdBool deserialize(bdStructBufferDeserializer & deserializer);  // vtable slot 3
    bdString toString() const;
protected:
    bdStructFixedSizeString<100> m_name;  // 0x08
    bdUInt16 m_kind;  // 0x6e
    bdUInt32 m_progress;  // 0x70
    bdUInt32 m_progressTarget;  // 0x74
    bdAchievementStatus::Enum m_status;  // 0x78
    bdUInt64 m_completionTimestamp;  // 0x80
    bdUInt64 m_activationTimestamp;  // 0x88
    bdUInt64 m_expirationTimestamp;  // 0x90
    bdUInt32 m_completionCount;  // 0x98
};
static_assert(sizeof(bdAchievementState) == 0xb0, "bdAchievementState");


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  bdAchievementState::bdAchievementState(const bdAchievementState &)  [inlined into 1 functions]
//   line    17  bdAchievementState::~bdAchievementState()  [0x7100b47100 (+1 more), inlined into 3 functions]
//   line    20  void bdAchievementState::operator delete(void * p)  [inlined into 1 functions]
