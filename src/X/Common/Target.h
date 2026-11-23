// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Target.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "SNO/SNOFiles.h"
#include "WorldPlace.h"
#include "../Sigma/SigmaTypes.h"

class ActorCommonData;
class CRefString;
struct NavPlace;

enum {
    TT_INVALID = -1,
    TT_NONE = 0,
    TT_ACTOR = 1,
    TT_POINT = 2,
    TT_ACTOR_WITH_FALLBACK_POINT = 3,
    TT_NUM_TYPES = 4,
};

typedef int32 TargetType;

class Target {  // size 0x1c
public:
    Target() {  // line 31
        Clear();
    }
    void Clear();
    BOOL IsNone() const;
    void InitializeFromPlace(const WorldPlace & _tPlace);
    void InitializeFromACD(const ACDID idACD, const BOOL bWantFallbackPoint, const BOOL bForcedTarget);
    BOOL IsInWorld() const;
    BOOL GetPlace(WorldPlace & _tPlace) const;
    BOOL GetAttackPosition(WorldPlace & placeAttack) const;
    ACDID GetACD() const;
    CRefString Trace() const;
    BOOL GetNavPlace(NavPlace & tNavPlace) const;
    BOOL IsInvalidForSource(const ActorCommonData * ptACDSource) const;
    BOOL IsInvalid() const;
    BOOL IsTargetingACD() const;
    void ValidateTargetACD();
    TargetType eType;  // 0x00
    ACDID idACDTarget;  // 0x04
    WorldPlace tPlace;  // 0x08
    BOOL bForcedActorTarget;  // 0x18
};
static_assert(sizeof(Target) == 0x1c, "Target");

extern const Target cgTargetNone;

struct TargetLock {  // size 0x2c
    void Set(const SNO _snoOriginalPower, const SNO _snoPower, const Target & _tTarget, const ACDID _idACDItemToUse, const BOOL _bIgnoreRange);
    void Clear();
    SNO snoOriginalPower;  // 0x00
    SNO snoPower;  // 0x04
    Target tTarget;  // 0x08
    ACDID idACDItemToUse;  // 0x24
    BOOL bIgnoreRange;  // 0x28
};
static_assert(sizeof(TargetLock) == 0x2c, "TargetLock");

// Target.h:88 and :102. Bodies written from callers (CPlayerInputCancelMode).
inline void TargetLock::Set(const SNO _snoOriginalPower, const SNO _snoPower, const Target & _tTarget, const ACDID _idACDItemToUse, const BOOL _bIgnoreRange) {
    snoOriginalPower = _snoOriginalPower;
    snoPower = _snoPower;
    tTarget = _tTarget;
    idACDItemToUse = _idACDItemToUse;
    bIgnoreRange = _bIgnoreRange;
}
inline void TargetLock::Clear() {
    snoOriginalPower = SNO_NONE;
    snoPower = SNO_NONE;
    tTarget.Clear();
    idACDItemToUse = ACDID_INVALID;
    bIgnoreRange = FALSE;
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    28  Target::Target(const Target &)  [inlined into 24 functions]
//   line    28  Target & Target::operator=(const Target &)  [inlined into 52 functions]
//   line    31  Target::Target()  [inlined into 249 functions]
//   line    86  TargetLock::TargetLock(const TargetLock &)  [inlined into 4 functions]
//   line    86  TargetLock & TargetLock::operator=(const TargetLock &)  [inlined into 2 functions]
//   line    88  void TargetLock::Set(const SNO _snoOriginalPower, const SNO _snoPower, const Target & _tTarget, const ACDID _idACDItemToUse, const BOOL _bIgnoreRange)  [inlined into 4 functions]
//   line   102  void TargetLock::Clear()  [inlined into 16 functions]
