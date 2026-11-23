// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/LevelAreaDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<LevelAreaGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../Act.h"
#include "../GizmoLocs.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

class XMemoryAllocatorInterface;
class XTypeDescriptor;
class CRefString;

typedef int32 MonsterRarity;

typedef int32 Team;

struct LevelAreaSpawnItem {  // size 0x20
    SNOName snonameToSpawn;  // 0x00
    MonsterRarity eMonsterRarity;  // 0x08
    int32 nSpawnCountMin;  // 0x0c
    int32 nSpawnCountMax;  // 0x10
    int32 nSpawnSpreadMin;  // 0x14
    int32 nSpawnSpreadMax;  // 0x18
    float flFarmingUnitMult;  // 0x1c
};
static_assert(sizeof(LevelAreaSpawnItem) == 0x20, "LevelAreaSpawnItem");

enum SpawnGroupDistribution {
    DISTRIBUTION_DENSITY = 0,
    DISTRIBUTION_EXACT = 1,
    DISTRIBUTION_XP_DENSITY = 2,
};

enum {
    SPAWNGROUP_TYPE_NORMAL = 0,
    SPAWNGROUP_TYPE_CHAMPION = 1,
    SPAWNGROUP_TYPE_RARE = 2,
    SPAWNGROUP_TYPE_UNIQUE = 3,
    SPAWNGROUP_TYPE_COUNT = 4,
};

typedef int32 SpawnGroupType;

struct LevelAreaSpawnGroup {  // size 0x38
    SpawnGroupDistribution eSpawnDistribution;  // 0x00
    float flSpawnDensity;  // 0x04
    float flSpawnChance;  // 0x08
    int32 nSpawnExactCount;  // 0x0c
    int32 nSpawnItemCount;  // 0x10
    XPtr64<LevelAreaSpawnItem> arSpawnItems;  // 0x18
    SerializeData serSpawnItems;  // 0x20
    Team eTeam;  // 0x28
    SpawnGroupType eType;  // 0x2c
    SNO snoCondition;  // 0x30
    static const int32 SPAWN_GROUP_PICK_ALL = -1;
    static const int32 LEVELAREA_MAX_NAME_LENGTH = 128;
};
static_assert(sizeof(LevelAreaSpawnGroup) == 0x38, "LevelAreaSpawnGroup");

enum {
    MAX_LEVEL_AREA_SPAWN_POPULATION_NAME = 64,
};

struct LevelAreaSpawnPopulation {  // size 0x98
    CHAR szName[64];  // 0x00
    int32 nWeight;  // 0x40
    float flFarmingDensityMult;  // 0x44
    float flFarmingUnitMult;  // 0x48
    int32 arMinSpawnGroupPickCounts[4];  // 0x4c
    int32 arMaxSpawnGroupPickCounts[4];  // 0x5c
    int32 nSpawnGroupCount;  // 0x6c
    XPtr64<LevelAreaSpawnGroup> arSpawnGroups;  // 0x70
    SerializeData serSpawnGroups;  // 0x78
    XPtr64<int> arDungeonFinderWeatherSNOs;  // 0x80
    SerializeData serDungeonFinderWeatherSNOs;  // 0x88
    SNO snoDungeonFinderOverrideBossEncounter;  // 0x90
};
static_assert(sizeof(LevelAreaSpawnPopulation) == 0x98, "LevelAreaSpawnPopulation");

struct LevelAreaServerData {  // size 0x360
    SNO snoLevelAreaOverrideForGizmoLocs;  // 0x00
    GizmoLocSet tGizmoLocSet;  // 0x08
    SNO snoLevelAreaOverrideForSpawns;  // 0x348
    int32 nSpawnPopulationCount;  // 0x34c
    XPtr64<LevelAreaSpawnPopulation> arSpawnPopulations;  // 0x350
    SerializeData serSpawnPopulations;  // 0x358
};
static_assert(sizeof(LevelAreaServerData) == 0x360, "LevelAreaServerData");

enum {
    LEVEL_AREA_FLAG_UNUSED = 0,
    LEVEL_AREA_FLAG_TOWN = 1,
    LEVEL_AREA_FLAG_HUB_TOWN = 2,
    LEVEL_AREA_MULTI_INSTANCE_AREA = 3,
    LEVEL_AREA_FLAG_TOWN_PORTAL_NOT_ALLOWED = 4,
    LEVEL_AREA_FLAG_TELEPORT_NOT_ALLOWED = 5,
    LEVEL_AREA_FLAG_INDOOR = 6,
    LEVEL_AREA_FLAG_NO_BANTER = 7,
    LEVEL_AREA_FLAG_KILLS_ENEMY_MONSTERS = 8,
    LEVEL_AREA_FLAG_UNKNOWN_DEPRECATED = 9,
    LEVEL_AREA_FLAG_DUNGEON_FINDER_AREA = 10,
    LEVEL_AREA_FLAG_AVENGERS_NOT_ALLOWED = 11,
};

struct LevelAreaDefinition {  // size 0x28
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    SNO snoParentLevelArea;  // 0x10
    SNO snoOpenWorldDisplayLevelArea;  // 0x14
    XPtr64<LevelAreaServerData> ptServerData;  // 0x18
    SerializeData serServerData;  // 0x20
};
static_assert(sizeof(LevelAreaDefinition) == 0x28, "LevelAreaDefinition");

class LevelAreaGroupDef : public GroupDefinition {  // size 0xa0
public:
    LevelAreaGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 171
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7, line 173
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
};
static_assert(sizeof(LevelAreaGroupDef) == 0xa0, "LevelAreaGroupDef");

extern LevelAreaGroupDef * gptGroupDefLevelArea;  // LevelAreaDefinition.cpp

extern const XTypeDescriptor * DT_LEVELAREA_SPAWN_ITEM;  // LevelAreaDefinition.cpp:95
extern const XTypeDescriptor * DT_LEVELAREA_SPAWN_GROUP;  // LevelAreaDefinition.cpp:109
extern const XTypeDescriptor * DT_LEVELAREA_SPAWN_POPULATION;  // LevelAreaDefinition.cpp:123
extern const XTypeDescriptor * DT_LEVELAREA_SERVER_DATA;  // LevelAreaDefinition.cpp:131
extern const XTypeDescriptor * DT_LEVELAREA;  // LevelAreaDefinition.cpp:139

typedef ScopedDefinitionLockR<LevelAreaDefinition> ScopedLevelAreaDefinitionLockR;

template <> inline const LevelAreaDefinition * DefinitionLockR<LevelAreaDefinition>(SNO sno) {  // line 223
    return (const LevelAreaDefinition *)gptGroupDefLevelArea->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<LevelAreaDefinition>(const LevelAreaDefinition * pdef) {  // line 224
    void * lpDef = (void *)pdef;
    gptGroupDefLevelArea->UnlockR(lpDef);
}

CRefString LevelAreaGetNameLabel(SNO snoLevelArea);  // LevelAreaDefinition.cpp:402
CRefString LevelAreaGetLocalizedName(SNO snoLevelArea);  // LevelAreaDefinition.cpp:411
BOOL LevelAreaIsTown(SNO snoLevelArea);  // LevelAreaDefinition.cpp:463
BOOL LevelAreaDisallowsBanter(SNO snoLevelArea);  // LevelAreaDefinition.cpp:478
BOOL LevelAreaTownPortalAllowed(SNO snoLevelArea);  // LevelAreaDefinition.cpp:508
BOOL LevelAreaTeleportAllowed(SNO snoLevelArea);  // LevelAreaDefinition.cpp:526
BOOL LevelAreaKillsEnemyMonsters(SNO snoLevelArea);  // LevelAreaDefinition.cpp:543
BOOL LevelAreaAllowsAvengers(SNO snoLevelArea);  // LevelAreaDefinition.cpp:558
SNO LevelAreaGetLevelAreaForSpawns(SNO snoLevelArea);  // LevelAreaDefinition.cpp:572
SNO LevelAreaGetParentLevelArea(SNO snoLevelArea);  // LevelAreaDefinition.cpp:587
Act LevelAreaGetAct(SNO snoLevelArea);  // LevelAreaDefinition.cpp:601
int32 LevelAreaGetDungeonFinderLevel(const SNO snoLevelArea);  // LevelAreaDefinition.cpp:927
