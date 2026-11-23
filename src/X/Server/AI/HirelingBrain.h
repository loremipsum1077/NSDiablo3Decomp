// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/AI/HirelingBrain.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "BrainComponents.h"
#include "../../Sigma/Math/MathGeneral.h"

struct TChooseAttack;

enum {
    MAX_TARGET_ACDS = 20,
    MAX_HIRELING_SKILLS = 4,
};

enum HirelingBrainState {
    HBS_NONE = 0,
    HBS_CATCHUP = 1,
    HBS_KEEPUP = 2,
    HBS_VENDOR = 3,
};

enum HirelingTarget {
    HT_MELEE = 0,
    HT_RANGED = 1,
    HT_LEADER = 2,
    HT_ALLY = 3,
    NUM_HIRELING_TARGETS = 4,
};

typedef BOOL (*HirelingChooseAttackFunc)(TChooseAttack &);

typedef void (*HirelingClassUpdateFunc)(ActorID);

struct HirelingBrainData {  // size 0x58
    ActorID idLeader;  // 0x00
    HirelingChooseAttackFunc pfnChooseAttack;  // 0x08
    HirelingClassUpdateFunc pfnUpdateFunction;  // 0x10
    HirelingBrainState eBrainState;  // 0x18
    uint32 dwLastUseSkill[4];  // 0x1c
    BOOL fAttacking;  // 0x2c
    uint32 dwCurrentAttackStartTime;  // 0x30
    SNO snoCurrentAttack;  // 0x34
    ACDID idACDCurrentTarget;  // 0x38
    ACDID idACDLastTarget;  // 0x3c
    ACDID idACDLastAttacker;  // 0x40
    ACDID idACDLastPlayerTarget;  // 0x44
    float flDefaultSpeed;  // 0x48
    float flCurrentSpeed;  // 0x4c
    BOOL fChangedLastTime;  // 0x50
    uint32 dwDeathPowerNextTime;  // 0x54
};
static_assert(sizeof(HirelingBrainData) == 0x58, "HirelingBrainData");

struct TChooseAttack {  // size 0x60
    ACDID idACDHireling;  // 0x00
    ACDID idACDPlayer;  // 0x04
    BrainAttackSettings tAttack;  // 0x08
    HirelingBrainData * ptData;  // 0x58
};
static_assert(sizeof(TChooseAttack) == 0x60, "TChooseAttack");

struct HTarget {  // size 0x14c
    ACDID idACDNormal[20];  // 0x00
    WorldDistance wdNormalDistance[20];  // 0x50
    int32 nNormalCount;  // 0xa0
    ACDID idACDStunned[20];  // 0xa4
    WorldDistance wdStunnedDistance[20];  // 0xf4
    int32 nStunnedCount;  // 0x144
    int32 nTotal;  // 0x148
};
static_assert(sizeof(HTarget) == 0x14c, "HTarget");

struct HirelingTargets {  // size 0x53c
    HTarget tClass[4];  // 0x00
    ACDID idACDLastTarget;  // 0x530
    ACDID idACDLastAttacker;  // 0x534
    ACDID idACDLastPlayerTarget;  // 0x538
};
static_assert(sizeof(HirelingTargets) == 0x53c, "HirelingTargets");
