// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/MonsterStruct.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "GameBalance.h"
#include "../Sigma/SigmaTypes.h"

enum {
    EC_NONE = -1,
    TC_BARREL = 0,
    TC_CHEST = 1,
    TC_SHRINE = 2,
    MIN_TREASURE_CATERGORIES = 0,
    MAX_TREASURE_CATERGORIES = 2,
    MC_BIG = 3,
    MC_STANDARD = 4,
    MC_RANGED = 5,
    MC_SWARM = 6,
    MC_BOSS = 7,
    MAX_STANDARD_MONSTER_CATERGORIES = 6,
    MIN_MONSTER_CATERGORIES = 3,
    MAX_MONSTER_CATERGORIES = 7,
    NUM_ENCOUNTER_CATEGORIES = 8,
};

typedef int32 EncounterCategory;

enum {
    HEALTH_DROP_STYLE_NEVER = 0,
    HEALTH_DROP_STYLE_EVERY_20_PERCENT = 1,
    HEALTH_DROP_STYLE_EVERY_25_PERCENT = 2,
    HEALTH_DROP_STYLE_EVERY_33_PERCENT = 3,
    HEALTH_DROP_STYLE_EVERY_50_PERCENT = 4,
    HEALTH_DROP_STYLE_ON_DEATH_ONLY = 5,
    HEALTH_DROP_STYLE_ON_PACK_DEATH = 6,
    HEALTH_DROP_STYLE_COUNT = 7,
};

typedef int32 HealthDropStyle;

struct MonsterAwarenessInfo {  // size 0x14
    float flIdleRadius;  // 0x00
    float flCombatRadius;  // 0x04
    float flTargetAbandonTime;  // 0x08
    float flWarnOthersRadius;  // 0x0c
    BOOL bRequireLOSForAllTargets;  // 0x10
};
static_assert(sizeof(MonsterAwarenessInfo) == 0x14, "MonsterAwarenessInfo");

struct HealthDropInfo {  // size 0xc
    float flDropChance;  // 0x00
    GBID gbidHealthItem;  // 0x04
    HealthDropStyle eHealthDropStyle;  // 0x08
};
static_assert(sizeof(HealthDropInfo) == 0xc, "HealthDropInfo");


// Values from the array bounds that use them (MonsterDefinition::tSkills, gbidAffixes,
// gbidDisallowedAffixes, pctAttributeModifiers, snoSummonActor).
static const int32 MAX_MONSTER_SKILLS = 8;  // line 67
static const int32 MAX_MONSTER_AFFIXES = 4;  // line 68
static const int32 MAX_DISALLOWED_MONSTER_AFFIXES = 6;  // line 69
static const int32 MAX_MONSTER_TABLE_ATTRIBUTES = 146;  // line 70
static const int32 MAX_SUMMON_ACTORS = 6;  // line 71
