// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Powers/PowersStruct.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Attributes/FastAttributeStructs.h"
#include "../CommonTypes.h"
#include "../Definitions/ActorDefinition.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

enum {
    POWER_PASSABILITY_DURING = 0,
    POWER_PASSABILITY_DESTINATION = 1,
    POWER_PASSABILITY_COUNT = 2,
};

typedef int32 PowerPassabilityType;

enum {
    POWERBIT_PASSABILITY_DURING_ENABLED = 0,
    POWERBIT_PASSABILITY_DEST_ENABLED = 1,
    POWERBIT_CONTACTFRAME_ENABLED_FIRST = 2,
    POWERBIT_CONTACTFRAME_ENABLED_LAST = 5,
    POWERBIT_REQUIRES_WALKING_PATH_TO_TARGET = 6,
    POWERBIT_CONSOLE_MOVES_FORWARD_NO_TARGET = 7,
    POWERBIT_CONSOLE_TARGET_AREA_GROWS = 8,
    POWERBIT_CONSOLE_ALLOW_WALK_BETWEEN_CASTS = 9,
    POWERBIT_CONSOLE_USE_RADIAL_TARGETING = 10,
    POWERBIT_CONSOLE_USE_CONE_TARGETING = 11,
    POWERBIT_CONSOLE_USE_SECONDARY_TARGETING = 12,
    POWERBIT_CONSOLE_PREFER_RADIAL_TARGETING = 13,
    POWERBIT_DEPRECATED_CONSOLE_PREFER_CONE_TARGETING = 14,
    POWERBIT_CONSOLE_ON_BUTTON_UP_ONLY = 15,
    POWERBIT_CONSOLE_TARGET_RANGE_GROWS = 16,
    POWERBIT_COUNT = 17,
};

struct BuffFilterDef {  // size 0x8
    SNO snoPower;  // 0x00
    uint32 dwRunes;  // 0x04
};
static_assert(sizeof(BuffFilterDef) == 0x8, "BuffFilterDef");

struct BuffDef {  // size 0x10
    SerializeData serBuffFilters;  // 0x00
    XPtr64<BuffFilterDef> arBuffFilters;  // 0x08
};
static_assert(sizeof(BuffDef) == 0x10, "BuffDef");

struct PowerDef {  // size 0x2d8
    SerializeData serTagMap;  // 0x00
    XPtr64<CTagMap> hTagMap;  // 0x08
    SerializeData serGlobalTagMap;  // 0x10
    XPtr64<CTagMap> hGeneralTagMap;  // 0x18
    SerializeData serPVPGlobalTagMap;  // 0x20
    XPtr64<CTagMap> hPVPGeneralTagMap;  // 0x28
    SerializeData serContactTagMap[4];  // 0x30
    XPtr64<CTagMap> hContactTagMap[4];  // 0x50
    SerializeData serPVPContactTagMap[4];  // 0x70
    XPtr64<CTagMap> hPVPContactTagMap[4];  // 0x90
    uint32 uFlags;  // 0xb0
    ActorCollisionFlags arCollFlags[2];  // 0xb4
    BuffDef arBuff[32];  // 0xd8
};
static_assert(sizeof(PowerDef) == 0x2d8, "PowerDef");

struct DamageAttribute {  // size 0x10
    FastAttribKey tKey;  // 0x00
    float flValue;  // 0x04
    int32 nValue;  // 0x08
    int32 nDuration;  // 0x0c
};
static_assert(sizeof(DamageAttribute) == 0x10, "DamageAttribute");

typedef XPooledList<DamageAttribute, const DamageAttribute &> DamageList;

struct PreplayedAnimData {  // size 0xc
    uint32 dwServerTimeAnimStarted;  // 0x00
    uint32 dwSyncedSeed;  // 0x04
    uint32 dwPrePlayedAnimFlags;  // 0x08
    PreplayedAnimData() {  // line 123
        dwServerTimeAnimStarted = 0;
        dwSyncedSeed = 0;
        dwPrePlayedAnimFlags = 0;
    }
};
static_assert(sizeof(PreplayedAnimData) == 0xc, "PreplayedAnimData");

enum {
    POWERID_INVALID = -1,
};

typedef int32 PowerID;

typedef void (*PowerTriggerCallback)(const PowerID, void *);

struct ACDSortedTarget {  // size 0x8
    ACDID idACD;  // 0x00
    float flDistance;  // 0x04
};
static_assert(sizeof(ACDSortedTarget) == 0x8, "ACDSortedTarget");


// Functions defined in this file (bodies are not in the DWARF):
//   line   123  PreplayedAnimData::PreplayedAnimData()  [inlined into 32 functions]

// Variables defined in this file:
//   line    24  static const uint32 MAX_NUM_CONTACT_FRAMES
//   line    25  static const uint32 MAX_BUFFS_PER_POWER

// DWARF PowersStruct.h:24 (const_value 4; referenced by AnimPlayer, FastAttribute, PowerDefinition, PowersCommon,
// PowerTrigger and SpellEffect). This header is not on DWARF lines; appended here so no existing line moves.
static const uint32 MAX_NUM_CONTACT_FRAMES = 4;
