// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/GizmoLocs.h
// Struct/member/method lines follow the DWARF decl_line of each entity.
#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"
#include "SNO/SNOName.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Files/XSerializeData.h"

class CRefString;
struct RNG;









enum {
    GLF_AFFECTED_BY_GIZMO_DENSITY_MULTIPLIER = 0,
};








struct GizmoLocSpawnChoice
{
    SNOName snonameToSpawn;
    BOOL bForceRandomFacing;
    float flWeight;
    int32 nMaxTimesPicked;

    void Initialize();
};


struct GizmoLocSpawnEntry
{
    int32 nSpawnMin;
    int32 nSpawnMax;
    SNO snoRequiredQuest;
    uint32 dwFlags;
    float flNoSpawnChance;
    SNO snoCondition;

    XPtr64<GizmoLocSpawnChoice> arChoice;
    SerializeData serChoice;

    void Initialize();


    void CopyFrom(const GizmoLocSpawnEntry & tOther);


    GizmoLocSpawnChoice * RollChoice(RNG & rng, int32 nEntryIdx) const;



    int32 GetNumValidChoices() const;


    SNOName GetDisplaySNOName() const;
};



struct GizmoLocSpawnType
{
    XPtr64<GizmoLocSpawnEntry> arEntry;
    SerializeData serEntry;


    static const int32 GIZMOLOC_SET_MAX_NAME_LENGTH = 80;
    static const int32 GIZMOLOC_SET_MAX_NOTE_LENGTH = 256;
};










struct GizmoLocSet
{
    GizmoLocSpawnType arSpawnArray[52];
};

// GizmoLocs.cpp:610.
CRefString GizmoLocSuffixFromIndex(const int32 nIndex);

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(GizmoLocSpawnChoice) == 0x14, "GizmoLocSpawnChoice");
static_assert(offsetof(GizmoLocSpawnChoice, snonameToSpawn) == 0x0, "GizmoLocSpawnChoice");
static_assert(offsetof(GizmoLocSpawnChoice, bForceRandomFacing) == 0x8, "GizmoLocSpawnChoice");
static_assert(offsetof(GizmoLocSpawnChoice, flWeight) == 0xc, "GizmoLocSpawnChoice");
static_assert(offsetof(GizmoLocSpawnChoice, nMaxTimesPicked) == 0x10, "GizmoLocSpawnChoice");
static_assert(sizeof(GizmoLocSpawnEntry) == 0x28, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, nSpawnMin) == 0x0, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, nSpawnMax) == 0x4, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, snoRequiredQuest) == 0x8, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, dwFlags) == 0xc, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, flNoSpawnChance) == 0x10, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, snoCondition) == 0x14, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, arChoice) == 0x18, "GizmoLocSpawnEntry");
static_assert(offsetof(GizmoLocSpawnEntry, serChoice) == 0x20, "GizmoLocSpawnEntry");
static_assert(sizeof(GizmoLocSpawnType) == 0x10, "GizmoLocSpawnType");
static_assert(offsetof(GizmoLocSpawnType, arEntry) == 0x0, "GizmoLocSpawnType");
static_assert(offsetof(GizmoLocSpawnType, serEntry) == 0x8, "GizmoLocSpawnType");
static_assert(sizeof(GizmoLocSet) == 0x340, "GizmoLocSet");
static_assert(offsetof(GizmoLocSet, arSpawnArray) == 0x0, "GizmoLocSet");
