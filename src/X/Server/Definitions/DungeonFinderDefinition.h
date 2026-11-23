// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Definitions/DungeonFinderDefinition.h
// Holds the definition structs (DungeonFinderDefinition at DWARF line 76, DungeonFinderLevelTable
// at 44), the override block SGame.h's ChallengeRiftData embeds, and the group definition class
// and its global, which X/Common/Game.cpp registers (GroupDefinition::Create<DungeonFinderGroupDef>).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

struct WeightedTransition {  // size 0x10
    SNO snoNextWorld;  // 0x00
    float flWeight;  // 0x04
    BOOL bExcludeFromTieredRift;  // 0x08
    int32 nTableIndex;  // 0x0c
};
static_assert(sizeof(WeightedTransition) == 0x10, "WeightedTransition");

struct WeightedPopulation {  // size 0x50
    SNO snoLevelArea;  // 0x00
    CHAR szPopulationName[64];  // 0x04
    float flWeight;  // 0x44
    BOOL bPlaystationOnly;  // 0x48
    BOOL bExcludeFromTieredRift;  // 0x4c
};
static_assert(sizeof(WeightedPopulation) == 0x50, "WeightedPopulation");

struct DungeonFinderLevelTable {  // size 0x48, line 44
    SNO snoWorldTileset;  // 0x00
    float flWeight;  // 0x04
    XPtr64<WeightedTransition> arTransitions;  // 0x08
    SerializeData serTransitions;  // 0x10
    XPtr64<WeightedPopulation> arPopulations;  // 0x18
    SerializeData serPopulations;  // 0x20
    float flSpawnGroupDensityMultiplier;  // 0x28
    float flEliteSpawnGroupDensityMultiplier;  // 0x2c
    float flGizmoDensityMultiplier;  // 0x30
    SNO snoBossEncounter;  // 0x34
    SNO snoElitePopulation;  // 0x38
    BOOL bExcludeFromTieredRift;  // 0x3c
    float flTotalTransitionWeights;  // 0x40
    float flTotalPopulationWeights;  // 0x44
};
static_assert(sizeof(DungeonFinderLevelTable) == 0x48, "DungeonFinderLevelTable");

struct DungeonFinderDefinition {  // size 0x28, line 76
    RequiredDefinitionHeader tHeader;  // 0x00
    XPtr64<DungeonFinderLevelTable> arDungeonFinderLevelTables;  // 0x10
    SerializeData serDungeonFinderLevelTables;  // 0x18
    float flTotalDungeonFinderLevelWeights;  // 0x20
};
static_assert(sizeof(DungeonFinderDefinition) == 0x28, "DungeonFinderDefinition");

struct DungeonFinderPylonInstance {  // size 0x10
    SNO snoActorPylon;  // 0x00
    Vector2D wpPos;  // 0x04
    int32 nRiftFloor;  // 0x0c
};
static_assert(sizeof(DungeonFinderPylonInstance) == 0x10, "DungeonFinderPylonInstance");

struct DungeonFinderLevelInstance {  // size 0x60
    SNO snoTilesetWorld;  // 0x00
    float flSpawnGroupDensityMultiplier;  // 0x04
    float flEliteSpawnGroupDensityMultiplier;  // 0x08
    float flGizmoDensityMultiplier;  // 0x0c
    char szPopulationName[64];  // 0x10
    SNO snoWeather;  // 0x50
    uint32 dwWorldSeed;  // 0x54
    SNO snoBossEncounter;  // 0x58
    SNO snoElitePopulation;  // 0x5c
};
static_assert(sizeof(DungeonFinderLevelInstance) == 0x60, "DungeonFinderLevelInstance");

struct DungeonFinderOverrides {  // size 0xcc
    SNO snoDungeonFinderEngine;  // 0x00
    int32 nSeed;  // 0x04
    SNO snoMasterTileset;  // 0x08
    SNO arSnoTilesets[10];  // 0x0c
    uint32 dwMasterPopulationHash;  // 0x34
    uint32 arPopulationHashes[10];  // 0x38
    SNO snoDungeonFinderBossOverride;  // 0x60
    int32 nMaxFloorSize;  // 0x64
    int32 nWeeklyChallengeRiftLevel;  // 0x68
    int32 nWeeklyChallengeCompletionMilliseconds;  // 0x6c
    int32 nWeeklyChallengeTargetMilliseconds;  // 0x70
    float flOrbValueMultiplier;  // 0x74
    BOOL bUsePylonOverrides;  // 0x78
    DungeonFinderPylonInstance arPylonOverrides[5];  // 0x7c
};
static_assert(sizeof(DungeonFinderOverrides) == 0xcc, "DungeonFinderOverrides");

class DungeonFinderGroupDef : public GroupDefinition {  // size 0xa0
public:
    DungeonFinderGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(DungeonFinderGroupDef) == 0xa0, "DungeonFinderGroupDef");

extern DungeonFinderGroupDef * gptGroupDefDungeonFinder;  // DungeonFinderDefinition.cpp

typedef ScopedDefinitionLockR<DungeonFinderDefinition> ScopedDungeonFinderDefinitionLockR;  // line 116 of the reference header

// Line 217. Written from the copy inlined into GameTesting::AddDungeonFinder (GameTesting.cpp:1303),
// which calls LockRFast and has nothing from an unlock, so the unlock is taken to be empty, as for
// TimedEventDefinition.h.
template <> inline const DungeonFinderDefinition * DefinitionLockR<DungeonFinderDefinition>(SNO sno) {
    return (const DungeonFinderDefinition *)gptGroupDefDungeonFinder->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<DungeonFinderDefinition>(const DungeonFinderDefinition *) {}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(WeightedTransition) == 0x10, "WeightedTransition");
static_assert(offsetof(WeightedTransition, snoNextWorld) == 0x0, "WeightedTransition");
static_assert(offsetof(WeightedTransition, flWeight) == 0x4, "WeightedTransition");
static_assert(offsetof(WeightedTransition, bExcludeFromTieredRift) == 0x8, "WeightedTransition");
static_assert(offsetof(WeightedTransition, nTableIndex) == 0xc, "WeightedTransition");
static_assert(sizeof(WeightedPopulation) == 0x50, "WeightedPopulation");
static_assert(offsetof(WeightedPopulation, snoLevelArea) == 0x0, "WeightedPopulation");
static_assert(offsetof(WeightedPopulation, szPopulationName) == 0x4, "WeightedPopulation");
static_assert(offsetof(WeightedPopulation, flWeight) == 0x44, "WeightedPopulation");
static_assert(offsetof(WeightedPopulation, bPlaystationOnly) == 0x48, "WeightedPopulation");
static_assert(offsetof(WeightedPopulation, bExcludeFromTieredRift) == 0x4c, "WeightedPopulation");
static_assert(sizeof(DungeonFinderLevelTable) == 0x48, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, snoWorldTileset) == 0x0, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, flWeight) == 0x4, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, arTransitions) == 0x8, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, serTransitions) == 0x10, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, arPopulations) == 0x18, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, serPopulations) == 0x20, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, flSpawnGroupDensityMultiplier) == 0x28, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, flEliteSpawnGroupDensityMultiplier) == 0x2c, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, flGizmoDensityMultiplier) == 0x30, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, snoBossEncounter) == 0x34, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, snoElitePopulation) == 0x38, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, bExcludeFromTieredRift) == 0x3c, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, flTotalTransitionWeights) == 0x40, "DungeonFinderLevelTable");
static_assert(offsetof(DungeonFinderLevelTable, flTotalPopulationWeights) == 0x44, "DungeonFinderLevelTable");
static_assert(sizeof(DungeonFinderDefinition) == 0x28, "DungeonFinderDefinition");
static_assert(offsetof(DungeonFinderDefinition, tHeader) == 0x0, "DungeonFinderDefinition");
static_assert(offsetof(DungeonFinderDefinition, arDungeonFinderLevelTables) == 0x10, "DungeonFinderDefinition");
static_assert(offsetof(DungeonFinderDefinition, serDungeonFinderLevelTables) == 0x18, "DungeonFinderDefinition");
static_assert(offsetof(DungeonFinderDefinition, flTotalDungeonFinderLevelWeights) == 0x20, "DungeonFinderDefinition");
static_assert(sizeof(DungeonFinderPylonInstance) == 0x10, "DungeonFinderPylonInstance");
static_assert(offsetof(DungeonFinderPylonInstance, snoActorPylon) == 0x0, "DungeonFinderPylonInstance");
static_assert(offsetof(DungeonFinderPylonInstance, wpPos) == 0x4, "DungeonFinderPylonInstance");
static_assert(offsetof(DungeonFinderPylonInstance, nRiftFloor) == 0xc, "DungeonFinderPylonInstance");
static_assert(sizeof(DungeonFinderOverrides) == 0xcc, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, snoDungeonFinderEngine) == 0x0, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, nSeed) == 0x4, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, snoMasterTileset) == 0x8, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, arSnoTilesets) == 0xc, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, dwMasterPopulationHash) == 0x34, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, arPopulationHashes) == 0x38, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, snoDungeonFinderBossOverride) == 0x60, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, nMaxFloorSize) == 0x64, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, nWeeklyChallengeRiftLevel) == 0x68, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, nWeeklyChallengeCompletionMilliseconds) == 0x6c, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, nWeeklyChallengeTargetMilliseconds) == 0x70, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, flOrbValueMultiplier) == 0x74, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, bUsePylonOverrides) == 0x78, "DungeonFinderOverrides");
static_assert(offsetof(DungeonFinderOverrides, arPylonOverrides) == 0x7c, "DungeonFinderOverrides");
static_assert(sizeof(DungeonFinderLevelInstance) == 0x60, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, snoTilesetWorld) == 0x0, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, flSpawnGroupDensityMultiplier) == 0x4, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, flEliteSpawnGroupDensityMultiplier) == 0x8, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, flGizmoDensityMultiplier) == 0xc, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, szPopulationName) == 0x10, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, snoWeather) == 0x50, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, dwWorldSeed) == 0x54, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, snoBossEncounter) == 0x58, "DungeonFinderLevelInstance");
static_assert(offsetof(DungeonFinderLevelInstance, snoElitePopulation) == 0x5c, "DungeonFinderLevelInstance");
static_assert(sizeof(DungeonFinderGroupDef) == 0xa0, "DungeonFinderGroupDef");
