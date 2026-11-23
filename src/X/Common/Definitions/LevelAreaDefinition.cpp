// Original file: X/Common/Definitions/LevelAreaDefinition.cpp
// Blank lines keep the original line numbers.

#include "LevelAreaDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../GameCommonData.h"
#include "../Game.h"
#include "../GlobalSNO.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "StringListDefinition.h"
#include "../../Client/CGame.h"
#include "../../Server/SGame.h"
#include "../../Server/SGameManager.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/GizmoLocs.cpp:80; no adopted header declares it yet.
extern const XTypeDescriptor * DT_GIZMOLOC_SET;

// Symbolic tables defined elsewhere (sizes from the original data).
extern const SymbolicEntry gMonsterRarity[8];
extern const SymbolicEntry gSpawnDistributions[4];

// Defined in X/Common/World.cpp:987 and X/Common/Quests/Quests.cpp:1003; no adopted header
// declares them yet.
CRefString WorldGetDungeonFinderName(const int32 nSeed, const int32 nLevel);
BOOL QuestDungeonFinderIsComplete();

enum {
    LEVELAREA_XML_VERSION = 3,
    LEVELAREA_COMMON_SNO_VERSION = 81,
    LEVELAREA_SPAWN_GROUP_XML_VERSION = 6,
    LEVELAREA_SPAWN_ITEM_XML_VERSION = 3,
    LEVELAREA_SPAWN_POPULATION_XML_VERSION = 4,
    LEVELAREA_SERVER_DATA_XML_VERSION = 1,
    LEVELAREA_PC_SNO_VERSION = 85,
    LEVELAREA_CONSOLE_SNO_VERSION = 84,
    LEVELAREA_AXE_SNO_VERSION = 90,
};

// The field names are "" in the binary.


























static const TypeDescriptorField LevelAreaSpawnItem_Fields[8] = { TypeDescriptorField("", DT_SNO_NAME, offsetof(LevelAreaSpawnItem, snonameToSpawn), NULL, 1),
    EnumTypeDescriptorField("", offsetof(LevelAreaSpawnItem, eMonsterRarity), NULL, gMonsterRarity),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnItem, nSpawnCountMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnItem, nSpawnCountMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnItem, nSpawnSpreadMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnItem, nSpawnSpreadMax), (const void *)4, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LevelAreaSpawnItem, flFarmingUnitMult), (const void *)0x3f800000, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LevelAreaSpawnItem), NULL, 0),};
ComplexTypeDescriptor TypeDescriptor_LevelAreaSpawnItem("LevelAreaSpawnItem", LEVELAREA_SPAWN_ITEM_XML_VERSION, LevelAreaSpawnItem_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEVELAREA_SPAWN_ITEM = &TypeDescriptor_LevelAreaSpawnItem;

static const TypeDescriptorField LevelAreaSpawnGroup_Fields[11] = { EnumTypeDescriptorField("", offsetof(LevelAreaSpawnGroup, eSpawnDistribution), NULL, gSpawnDistributions),
    TypeDescriptorField("", DT_FLOAT, offsetof(LevelAreaSpawnGroup, flSpawnDensity), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LevelAreaSpawnGroup, flSpawnChance), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnGroup, nSpawnExactCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LevelAreaSpawnGroup, serSpawnItems), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LEVELAREA_SPAWN_ITEM, offsetof(LevelAreaSpawnGroup, arSpawnItems), 0x21, (int32)offsetof(LevelAreaSpawnGroup, serSpawnItems) - (int32)offsetof(LevelAreaSpawnGroup, arSpawnItems)),
    VariableArrayCountTypeDescriptorField("", offsetof(LevelAreaSpawnGroup, nSpawnItemCount), 0x10000, DT_LEVELAREA_SPAWN_ITEM, (int32)offsetof(LevelAreaSpawnGroup, serSpawnItems) - (int32)offsetof(LevelAreaSpawnGroup, nSpawnItemCount)),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnGroup, eTeam), (const void *)10, 1),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnGroup, eType), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LevelAreaSpawnGroup, snoCondition), "", 0x33, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LevelAreaSpawnGroup), NULL, 0),};

ComplexTypeDescriptor TypeDescriptor_LevelAreaSpawnGroup("LevelAreaSpawnGroup", LEVELAREA_SPAWN_GROUP_XML_VERSION, LevelAreaSpawnGroup_Fields, 11, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEVELAREA_SPAWN_GROUP = &TypeDescriptor_LevelAreaSpawnGroup;

static const TypeDescriptorField LevelAreaSpawnPopulation_Fields[13] = { CharArrayTypeDescriptorField("", offsetof(LevelAreaSpawnPopulation, szName), "", 1, MAX_LEVEL_AREA_SPAWN_POPULATION_NAME, -1),
    TypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnPopulation, nWeight), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LevelAreaSpawnPopulation, flFarmingDensityMult), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LevelAreaSpawnPopulation, flFarmingUnitMult), (const void *)0x3f800000, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnPopulation, arMinSpawnGroupPickCounts), NULL, 1, 4, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(LevelAreaSpawnPopulation, arMaxSpawnGroupPickCounts), NULL, 1, 4, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LevelAreaSpawnPopulation, serDungeonFinderWeatherSNOs), NULL, 0),
    FilteredVariableArrayTypeDescriptorField("", DT_SNO, offsetof(LevelAreaSpawnPopulation, arDungeonFinderWeatherSNOs), 0x21, (int32)offsetof(LevelAreaSpawnPopulation, serDungeonFinderWeatherSNOs) - (int32)offsetof(LevelAreaSpawnPopulation, arDungeonFinderWeatherSNOs), 0x2f),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LevelAreaSpawnPopulation, serSpawnGroups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LEVELAREA_SPAWN_GROUP, offsetof(LevelAreaSpawnPopulation, arSpawnGroups), 0x21, (int32)offsetof(LevelAreaSpawnPopulation, serSpawnGroups) - (int32)offsetof(LevelAreaSpawnPopulation, arSpawnGroups)),
    VariableArrayCountTypeDescriptorField("", offsetof(LevelAreaSpawnPopulation, nSpawnGroupCount), 0x10000, DT_LEVELAREA_SPAWN_GROUP, (int32)offsetof(LevelAreaSpawnPopulation, serSpawnGroups) - (int32)offsetof(LevelAreaSpawnPopulation, nSpawnGroupCount)),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LevelAreaSpawnPopulation, snoDungeonFinderOverrideBossEncounter), "", 15, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LevelAreaSpawnPopulation), NULL, 0),};
ComplexTypeDescriptor TypeDescriptor_LevelAreaSpawnPopulation("LevelAreaSpawnPopulation", LEVELAREA_SPAWN_POPULATION_XML_VERSION, LevelAreaSpawnPopulation_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEVELAREA_SPAWN_POPULATION = &TypeDescriptor_LevelAreaSpawnPopulation;
static const TypeDescriptorField LevelAreaServerData_Fields[7] = { FilterTypeDescriptorField("", DT_SNO, offsetof(LevelAreaServerData, snoLevelAreaOverrideForGizmoLocs), "", 22, 0, -1),
    TypeDescriptorField("", DT_GIZMOLOC_SET, offsetof(LevelAreaServerData, tGizmoLocSet), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LevelAreaServerData, snoLevelAreaOverrideForSpawns), "", 22, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LevelAreaServerData, serSpawnPopulations), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LEVELAREA_SPAWN_POPULATION, offsetof(LevelAreaServerData, arSpawnPopulations), 0x21, (int32)offsetof(LevelAreaServerData, serSpawnPopulations) - (int32)offsetof(LevelAreaServerData, arSpawnPopulations)),
    VariableArrayCountTypeDescriptorField("", offsetof(LevelAreaServerData, nSpawnPopulationCount), 0x10000, DT_LEVELAREA_SPAWN_POPULATION, (int32)offsetof(LevelAreaServerData, serSpawnPopulations) - (int32)offsetof(LevelAreaServerData, nSpawnPopulationCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LevelAreaServerData), NULL, 0),};
ComplexTypeDescriptor TypeDescriptor_LevelAreaServerData("LevelAreaServerData", LEVELAREA_SERVER_DATA_XML_VERSION, LevelAreaServerData_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEVELAREA_SERVER_DATA = &TypeDescriptor_LevelAreaServerData;
static const TypeDescriptorField LevelAreaDefinition_Fields[6] = { TypeDescriptorField("", DT_INT, offsetof(LevelAreaDefinition, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LevelAreaDefinition, snoParentLevelArea), "", 22, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LevelAreaDefinition, snoOpenWorldDisplayLevelArea), "", 22, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(LevelAreaDefinition, serServerData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_LEVELAREA_SERVER_DATA, offsetof(LevelAreaDefinition, ptServerData), 0x421, (int32)offsetof(LevelAreaDefinition, serServerData) - (int32)offsetof(LevelAreaDefinition, ptServerData)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LevelAreaDefinition), NULL, 0),};
ComplexTypeDescriptor TypeDescriptor_LevelAreaDefinition("LevelArea", LEVELAREA_XML_VERSION, LevelAreaDefinition_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_LEVELAREA = &TypeDescriptor_LevelAreaDefinition;





LevelAreaGroupDef * gptGroupDefLevelArea;  // line 145





LevelAreaGroupDef::LevelAreaGroupDef()
    : GroupDefinition("LevelArea", LEVELAREA_SNO, SNOVersions(LEVELAREA_PC_SNO_VERSION, 92, LEVELAREA_CONSOLE_SNO_VERSION, LEVELAREA_CONSOLE_SNO_VERSION, 85),
                      SNODATATYPE_TITLE, sizeof(LevelAreaDefinition), DT_LEVELAREA, 0x9441, 0x500e) {
}























uint32 LevelAreaGroupDef::GetMaxCount() {
    return 128;
}









void LevelAreaGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(LevelAreaDefinition));

    LevelAreaDefinition * pdef = (LevelAreaDefinition *)ptDefaultDef;
    pdef->ptServerData = (LevelAreaServerData *)SigmaMemoryNew(sizeof(LevelAreaServerData), 4, NULL, TRUE);
    pdef->serServerData.dwSize = sizeof(LevelAreaServerData);

    pdef->dwFlags = 0;
    pdef->ptServerData->nSpawnPopulationCount = 0;
    pdef->ptServerData->arSpawnPopulations = NULL;
    pdef->ptServerData->serSpawnPopulations.dwSize = 0;
    pdef->ptServerData->snoLevelAreaOverrideForGizmoLocs = SNO_NONE;
    pdef->ptServerData->snoLevelAreaOverrideForSpawns = SNO_NONE;
}

































































































void LevelAreaGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}























void LevelAreaGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer,
                             uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);
}







































































CRefString LevelAreaGetNameLabel(SNO snoLevelArea) {

    return SNOToString(LEVELAREA_SNO, snoLevelArea, FALSE);
}





CRefString LevelAreaGetLocalizedName(SNO snoLevelArea) {

    int32 nDungeonFinderLevel = LevelAreaGetDungeonFinderLevel(snoLevelArea);

    if (nDungeonFinderLevel == 0) {
        if (GameIsOpenWorld() && snoLevelArea != SNO_NONE) {

            ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
            if (pdefLevelArea && pdefLevelArea->snoOpenWorldDisplayLevelArea != SNO_NONE) {
                snoLevelArea = pdefLevelArea->snoOpenWorldDisplayLevelArea;
            }
        }

        return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS), LevelAreaGetNameLabel(snoLevelArea));
    } else {
        int32 nDungeonFinderSeed;
        if (GameActiveGameCommonDataIsClient()) {
            nDungeonFinderSeed = CGameGlobalsGet().nDungeonFinderSeed;
        } else {
            SGame & tSGame = SGameManager::GameGet();
            nDungeonFinderSeed = tSGame.ptSGameGlobals->tDungeonFinderData.tDungeonFinderRNG.dwSeed;
        }

        CRefString sName = WorldGetDungeonFinderName(nDungeonFinderSeed, nDungeonFinderLevel);
        if (QuestDungeonFinderIsComplete()) {
            return LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "LootRunPortalComplete").str(), sName.str(), NULL);
        }

        return sName;
    }
}





















BOOL LevelAreaIsTown(SNO snoLevelArea) {

    if (snoLevelArea == SNO_NONE) {
        return FALSE;
    }
    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    return (pdefLevelArea->dwFlags >> LEVEL_AREA_FLAG_TOWN) & 1;
}







BOOL LevelAreaDisallowsBanter(SNO snoLevelArea) {

    if (snoLevelArea == SNO_NONE) {
        return FALSE;
    }
    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    return (pdefLevelArea->dwFlags >> LEVEL_AREA_FLAG_NO_BANTER) & 1;
}






















BOOL LevelAreaTownPortalAllowed(SNO snoLevelArea) {
    BOOL bPortalAllowed = TRUE;
    if (snoLevelArea != SNO_NONE) {
        LevelAreaDefinition * ptLevelAreaDef = (LevelAreaDefinition *)gptGroupDefLevelArea->LockR(snoLevelArea, FALSE);
        bPortalAllowed = !((ptLevelAreaDef->dwFlags >> LEVEL_AREA_FLAG_TOWN_PORTAL_NOT_ALLOWED) & 1);
        gptGroupDefLevelArea->UnlockR((void *&)ptLevelAreaDef);
    }
    return bPortalAllowed;
}









BOOL LevelAreaTeleportAllowed(SNO snoLevelArea) {
    BOOL bAllowed = TRUE;
    if (snoLevelArea != SNO_NONE) {
        LevelAreaDefinition * ptLevelAreaDef = (LevelAreaDefinition *)gptGroupDefLevelArea->LockR(snoLevelArea, FALSE);
        bAllowed = !((ptLevelAreaDef->dwFlags >> LEVEL_AREA_FLAG_TELEPORT_NOT_ALLOWED) & 1);
        gptGroupDefLevelArea->UnlockR((void *&)ptLevelAreaDef);
    }
    return bAllowed;
}








BOOL LevelAreaKillsEnemyMonsters(SNO snoLevelArea) {
    if (snoLevelArea == SNO_NONE) {
        return FALSE;
    }
    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    return (pdefLevelArea->dwFlags >> LEVEL_AREA_FLAG_KILLS_ENEMY_MONSTERS) & 1;
}








BOOL LevelAreaAllowsAvengers(SNO snoLevelArea) {

    if (snoLevelArea == SNO_NONE) {
        return FALSE;
    }
    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    return !((pdefLevelArea->dwFlags >> LEVEL_AREA_FLAG_AVENGERS_NOT_ALLOWED) & 1);
}






SNO LevelAreaGetLevelAreaForSpawns(SNO snoLevelArea) {
    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    if (pdefLevelArea->ptServerData->snoLevelAreaOverrideForSpawns != SNO_NONE) {
        return pdefLevelArea->ptServerData->snoLevelAreaOverrideForSpawns;
    }
    return snoLevelArea;
}








SNO LevelAreaGetParentLevelArea(SNO snoLevelArea) {

    if (snoLevelArea == SNO_NONE) {
        return SNO_NONE;
    }
    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    return pdefLevelArea->snoParentLevelArea;
}






Act LevelAreaGetAct(SNO snoLevelArea) {

    CRefString sLevelArea = SNOToString(LEVELAREA_SNO, snoLevelArea, FALSE);
    if (Blizzard::String::FindI(sLevelArea.str(), "A1") == sLevelArea.str()) {
        return ACT_1;
    }
    if (Blizzard::String::FindI(sLevelArea.str(), "A2") == sLevelArea.str()) {
        return ACT_2;
    }
    if (Blizzard::String::FindI(sLevelArea.str(), "A3") == sLevelArea.str()) {
        return ACT_3;
    }
    if (Blizzard::String::FindI(sLevelArea.str(), "A4") == sLevelArea.str()) {
        return ACT_4;
    }
    if (Blizzard::String::FindI(sLevelArea.str(), "A5") == sLevelArea.str()) {
        return ACT_5;
    }
    if (Blizzard::String::FindI(sLevelArea.str(), "X1_") == sLevelArea.str()) {
        return ACT_5;
    }
    return ACT_INVALID;
}















































































































































































































































































































int32 LevelAreaGetDungeonFinderLevel(const SNO snoLevelArea) {

    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_01) == snoLevelArea) return 1;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_02) == snoLevelArea) return 2;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_03) == snoLevelArea) return 3;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_04) == snoLevelArea) return 4;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_05) == snoLevelArea) return 5;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_06) == snoLevelArea) return 6;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_07) == snoLevelArea) return 7;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_08) == snoLevelArea) return 8;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_09) == snoLevelArea) return 9;
    if (GlobalSNOGet(GLOBALSNO_LEVELAREA_DUNGEON_FINDER_LEVEL_10) == snoLevelArea) return 10;
    return 0;
}
