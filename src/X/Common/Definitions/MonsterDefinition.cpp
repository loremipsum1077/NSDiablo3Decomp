// Original file: X/Common/Definitions/MonsterDefinition.cpp

#include "MonsterDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_GBID;

// Defined in X/Common/SNO/SymbolicValue.cpp:2941, :2962, :2982, :3148 and :4043; no adopted
// header declares them yet.
extern const SymbolicEntry gMonsterTypes[15];
extern const SymbolicEntry gMonsterFamily[14];
extern const SymbolicEntry gEncounterCategory[7];
extern const SymbolicEntry gDamageTypes[9];
extern const SymbolicEntry gResourceTypes[10];

enum {
    MONSTER_DEFINITION_VERSION = 30,
    MONSTER_AWARENESS_VERSION = 2,
    MONSTER_COMMON_SNO_VERSION = 1486,
    MONSTER_PC_SNO_VERSION = 1487,
    MONSTER_CONSOLE_SNO_VERSION = 1493,
    MONSTER_AXE_SNO_VERSION = 1492,
};

// GameBalanceType 2 (GB_ITEMS); the full GB_ enum is not adopted, see GameBalanceDefinition.h.
#define MONSTER_GB_ITEMS 2  // invented name

// The field names are "" in the binary.
static const TypeDescriptorField HealthDropInfo_Fields[4] = {  // line 49
    TypeDescriptorField("", DT_FLOAT, offsetof(HealthDropInfo, flDropChance), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(HealthDropInfo, gbidHealthItem), "", MONSTER_GB_ITEMS, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(HealthDropInfo, eHealthDropStyle), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HealthDropInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HealthDropInfo("HealthDropInfo", 0, HealthDropInfo_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 53
const XTypeDescriptor * DT_HEALTHDROPINFO = &TypeDescriptor_HealthDropInfo;

static const TypeDescriptorField SkillDeclaration_Fields[3] = {  // line 55
    FilterTypeDescriptorField("", DT_SNO, offsetof(SkillDeclaration, snoPower), "", POWER_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SkillDeclaration, nLevelMod), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SkillDeclaration), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SkillDeclaration("SkillDeclaration", 0, SkillDeclaration_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 58
const XTypeDescriptor * DT_SKILLDECLARATION = &TypeDescriptor_SkillDeclaration;

static const TypeDescriptorField MonsterSkillDeclaration_Fields[5] = {  // line 60
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterSkillDeclaration, flUseRangeMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterSkillDeclaration, flUseRangeMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterSkillDeclaration, dwWeight), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterSkillDeclaration, flTimer), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterSkillDeclaration), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterSkillDeclaration("MonsterSkillDeclaration", 0, MonsterSkillDeclaration_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 65
const XTypeDescriptor * DT_MONSTER_SKILLDECLARATION = &TypeDescriptor_MonsterSkillDeclaration;

static const TypeDescriptorField MonsterAwarenessInfo_Fields[6] = {  // line 67
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterAwarenessInfo, flIdleRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterAwarenessInfo, flCombatRadius), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterAwarenessInfo, flTargetAbandonTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterAwarenessInfo, flWarnOthersRadius), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(MonsterAwarenessInfo, bRequireLOSForAllTargets), NULL, (const void *)FALSE, (const void *)TRUE),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterAwarenessInfo), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterAwarenessInfo("Monster", MONSTER_AWARENESS_VERSION, MonsterAwarenessInfo_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 73
const XTypeDescriptor * DT_MONSTER_AWARENESS_INFO = &TypeDescriptor_MonsterAwarenessInfo;

static const TypeDescriptorField MonsterMinionSpawnItem_Fields[6] = {  // line 75
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterMinionSpawnItem, snoSpawn), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterMinionSpawnItem, nSpawnCountMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterMinionSpawnItem, nSpawnCountMax), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterMinionSpawnItem, nSpawnSpreadMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterMinionSpawnItem, nSpawnSpreadMax), (const void *)25, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterMinionSpawnItem), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterMinionSpawnItem("MonsterMinionSpawnItem", 0, MonsterMinionSpawnItem_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 81
const XTypeDescriptor * DT_MONSTER_MINION_SPAWN_ITEM = &TypeDescriptor_MonsterMinionSpawnItem;

static const TypeDescriptorField MonsterMinionSpawnGroup_Fields[5] = {  // line 83
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterMinionSpawnGroup, nWeight), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterMinionSpawnGroup, serSpawnItems), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MONSTER_MINION_SPAWN_ITEM, offsetof(MonsterMinionSpawnGroup, arSpawnItems), 0x21,
                                     (int32)offsetof(MonsterMinionSpawnGroup, serSpawnItems) - (int32)offsetof(MonsterMinionSpawnGroup, arSpawnItems)),
    VariableArrayCountTypeDescriptorField("", offsetof(MonsterMinionSpawnGroup, nSpawnItemCount), 0x10000, DT_MONSTER_MINION_SPAWN_ITEM,
                                          (int32)offsetof(MonsterMinionSpawnGroup, serSpawnItems) - (int32)offsetof(MonsterMinionSpawnGroup, nSpawnItemCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterMinionSpawnGroup), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterMinionSpawnGroup("MonsterMinionSpawnGroup", 0, MonsterMinionSpawnGroup_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 87
const XTypeDescriptor * DT_MONSTER_MINION_SPAWN_GROUP = &TypeDescriptor_MonsterMinionSpawnGroup;

static const TypeDescriptorField MonsterChampionSpawnItem_Fields[3] = {  // line 89
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterChampionSpawnItem, snoActor), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterChampionSpawnItem, nSpawnCount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterChampionSpawnItem), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterChampionSpawnItem("MonsterChampionSpawnItem", 0, MonsterChampionSpawnItem_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 92
const XTypeDescriptor * DT_MONSTER_CHAMPION_SPAWN_ITEM = &TypeDescriptor_MonsterChampionSpawnItem;

static const TypeDescriptorField MonsterChampionSpawnGroup_Fields[5] = {  // line 94
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterChampionSpawnGroup, flWeight), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterChampionSpawnGroup, serSpawnItems), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MONSTER_CHAMPION_SPAWN_ITEM, offsetof(MonsterChampionSpawnGroup, arSpawnItems), 0x21,
                                     (int32)offsetof(MonsterChampionSpawnGroup, serSpawnItems) - (int32)offsetof(MonsterChampionSpawnGroup, arSpawnItems)),
    VariableArrayCountTypeDescriptorField("", offsetof(MonsterChampionSpawnGroup, nSpawnItemCount), 0x10000, DT_MONSTER_CHAMPION_SPAWN_ITEM,
                                          (int32)offsetof(MonsterChampionSpawnGroup, serSpawnItems) - (int32)offsetof(MonsterChampionSpawnGroup, nSpawnItemCount)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterChampionSpawnGroup), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterChampionSpawnGroup("MonsterChampionSpawnGroup", 0, MonsterChampionSpawnGroup_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 98
const XTypeDescriptor * DT_MONSTER_CHAMPION_SPAWN_GROUP = &TypeDescriptor_MonsterChampionSpawnGroup;

static const TypeDescriptorField MonsterDefinition_Fields[67] = {  // line 100
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, dwFlags), NULL, 0x80001),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoActor), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nLookIndex), NULL, 1),
    EnumTypeDescriptorField("", offsetof(MonsterDefinition, eMonsterType), NULL, gMonsterTypes),
    EnumTypeDescriptorField("", offsetof(MonsterDefinition, eMonsterFamily), NULL, gMonsterFamily),
    EnumTypeDescriptorField("", offsetof(MonsterDefinition, eEncounterCategory), NULL, gEncounterCategory),
    EnumTypeDescriptorField("", offsetof(MonsterDefinition, eBaseElement), NULL, gDamageTypes),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nDefaultCountMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nDefaultCountDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flHPChampion), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flHPDeltaChampion), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flHPRare), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flHPDeltaRare), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flHPMinion), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flHPDeltaMinion), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoInventory), "", TREASURECLASS_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoSecondaryInventory), "", TREASURECLASS_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoLore), "", LORE_SNO, 0, -1),
    TypeDescriptorField("", DT_HEALTHDROPINFO, offsetof(MonsterDefinition, tHealthDropNormal), NULL, 1),
    TypeDescriptorField("", DT_HEALTHDROPINFO, offsetof(MonsterDefinition, tHealthDropChampion), NULL, 1),
    TypeDescriptorField("", DT_HEALTHDROPINFO, offsetof(MonsterDefinition, tHealthDropRare), NULL, 1),
    TypeDescriptorField("", DT_HEALTHDROPINFO, offsetof(MonsterDefinition, tHealthDropMinion), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nGoldGranted), NULL, 1),
    EnumTypeDescriptorField("", offsetof(MonsterDefinition, eResourceType), NULL, gResourceTypes),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, pctAttributeModifiers), NULL, 1, MAX_MONSTER_TABLE_ATTRIBUTES, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoSkillKit), "", SKILLKIT_SNO, 0, -1),
    FixedArrayTypeDescriptorField("", DT_SKILLDECLARATION, offsetof(MonsterDefinition, tSkills), NULL, 1, MAX_MONSTER_SKILLS, 0),
    FixedArrayTypeDescriptorField("", DT_MONSTER_SKILLDECLARATION, offsetof(MonsterDefinition, tMonsterSkills), NULL, 1, MAX_MONSTER_SKILLS, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoTreasureClassFirstKill), "", TREASURECLASS_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoTreasureClass), "", TREASURECLASS_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoTreasureClassRare), "", TREASURECLASS_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoTreasureClassChampion), "", TREASURECLASS_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoTreasureClassChampionLight), "", TREASURECLASS_SNO, 0, -1),
    TypeDescriptorField("", DT_MONSTER_AWARENESS_INFO, offsetof(MonsterDefinition, tAwareness), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flNoDropScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flFleeChance), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flFleeCooldownMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flFleeCooldownDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nSummonCountPer), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MonsterDefinition, flSummonLifetime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nSummonMaxConcurrent), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nSummonMaxTotal), NULL, 1),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, arAIBehaviors), NULL, 0, 6, AIBEHAVIOR_SNO, 0),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(MonsterDefinition, gbidMovementStyles), NULL, 1, 8, 0),
    FilteredFixedArrayTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoSummonActor), NULL, 0, MAX_SUMMON_ACTORS, ACTOR_SNO, 0),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nRandomAffixes), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(MonsterDefinition, gbidAffixes), NULL, 1, MAX_MONSTER_AFFIXES, 0),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(MonsterDefinition, gbidDisallowedAffixes), NULL, 1, MAX_DISALLOWED_MONSTER_AFFIXES, 0),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, eAITargetStyleNormal), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, eAITargetStyleChampion), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, eAITargetStyleRare), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(MonsterDefinition, szInternalName), "", 1, MONSTER_DEFINITION_INTERNAL_NAMELEN, -1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, bDoesNotDropNecroCorpse), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterDefinition, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(MonsterDefinition, hTagMap), 0x21,
                              (int32)offsetof(MonsterDefinition, serTagMap) - (int32)offsetof(MonsterDefinition, hTagMap), 0x120000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterDefinition, serMinionSpawnGroups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MONSTER_MINION_SPAWN_GROUP, offsetof(MonsterDefinition, arMinionSpawnGroups), 0x21,
                                     (int32)offsetof(MonsterDefinition, serMinionSpawnGroups) - (int32)offsetof(MonsterDefinition, arMinionSpawnGroups)),
    VariableArrayCountTypeDescriptorField("", offsetof(MonsterDefinition, nMinionSpawnGroupCount), 0x10000, DT_MONSTER_MINION_SPAWN_GROUP,
                                          (int32)offsetof(MonsterDefinition, serMinionSpawnGroups) - (int32)offsetof(MonsterDefinition, nMinionSpawnGroupCount)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MonsterDefinition, serChampionSpawnGroups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MONSTER_CHAMPION_SPAWN_GROUP, offsetof(MonsterDefinition, arChampionSpawnGroups), 0x21,
                                     (int32)offsetof(MonsterDefinition, serChampionSpawnGroups) - (int32)offsetof(MonsterDefinition, arChampionSpawnGroups)),
    VariableArrayCountTypeDescriptorField("", offsetof(MonsterDefinition, nChampionSpawnGroupCount), 0x10000, DT_MONSTER_CHAMPION_SPAWN_GROUP,
                                          (int32)offsetof(MonsterDefinition, serChampionSpawnGroups) - (int32)offsetof(MonsterDefinition, nChampionSpawnGroupCount)),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MonsterDefinition, snoAIStateAttackerCapReached), "", AISTATE_SNO, 0x700000, -1),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, bDontThrottleDropsOnConsole), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, bShowHealthBarWithOwnerOnConsole), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, bCanBeAvenger), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(MonsterDefinition, nPad), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MonsterDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MonsterDefinition("Monster", MONSTER_DEFINITION_VERSION, MonsterDefinition_Fields, 67, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 188
const XTypeDescriptor * DT_MONSTER_DEFINITION = &TypeDescriptor_MonsterDefinition;

MonsterGroupDef * gptGroupDefMonster;  // line 193

// line 198. The second and fifth versions have no named enumerator in the DWARF.
MonsterGroupDef::MonsterGroupDef()
    : GroupDefinition("Monster", MONSTER_SNO,
                      SNOVersions(MONSTER_PC_SNO_VERSION, 1494, MONSTER_CONSOLE_SNO_VERSION, MONSTER_CONSOLE_SNO_VERSION, 1494),
                      SNODATATYPE_TITLE, sizeof(MonsterDefinition), DT_MONSTER_DEFINITION, 0x8641, 0x500e) {
}

// line 224
uint32 MonsterGroupDef::GetMaxCount() {
    return 1024;
}

// line 235
void MonsterGroupDef::GetDefaultData(void * ptDefaultDef) const {
    MonsterDefinition * pdef = (MonsterDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(MonsterDefinition));

    pdef->dwFlags = 0;
    pdef->snoActor = SNO_NONE;

    pdef->eMonsterType = -1;
    pdef->eMonsterFamily = 0;
    pdef->eEncounterCategory = EC_NONE;
    pdef->eBaseElement = -1;

    pdef->tHealthDropNormal.gbidHealthItem = GBID_NONE;
    pdef->tHealthDropNormal.eHealthDropStyle = HEALTH_DROP_STYLE_ON_DEATH_ONLY;
    pdef->tHealthDropChampion.gbidHealthItem = GBID_NONE;
    pdef->tHealthDropChampion.eHealthDropStyle = HEALTH_DROP_STYLE_EVERY_50_PERCENT;
    pdef->tHealthDropRare.gbidHealthItem = GBID_NONE;
    pdef->tHealthDropRare.eHealthDropStyle = HEALTH_DROP_STYLE_EVERY_25_PERCENT;

    pdef->snoSkillKit = SNO_NONE;

    for (int i = 0; i < MAX_MONSTER_SKILLS; i++) {
        pdef->tSkills[i].snoPower = SNO_NONE;
        pdef->tSkills[i].nLevelMod = 0;
    }

    pdef->snoTreasureClassFirstKill = SNO_NONE;
    pdef->snoTreasureClass = SNO_NONE;
    pdef->snoTreasureClassRare = SNO_NONE;
    pdef->snoTreasureClassChampion = SNO_NONE;
    pdef->snoTreasureClassChampionLight = SNO_NONE;

    pdef->snoInventory = SNO_NONE;
    pdef->snoSecondaryInventory = SNO_NONE;

    for (int i = 0; i < 6; i++)
        pdef->arAIBehaviors[i] = SNO_NONE;
    for (int i = 0; i < 8; i++)
        pdef->gbidMovementStyles[i] = GBID_NONE;
    for (int i = 0; i < MAX_SUMMON_ACTORS; i++)
        pdef->snoSummonActor[i] = SNO_NONE;

    for (int i = 0; i < MAX_MONSTER_AFFIXES; i++)
        pdef->gbidAffixes[i] = GBID_NONE;
    for (int i = 0; i < MAX_DISALLOWED_MONSTER_AFFIXES; i++)
        pdef->gbidDisallowedAffixes[i] = GBID_NONE;

    pdef->eResourceType = -1;

    pdef->nSummonCountPer = 1;
    pdef->flSummonLifetime = 0.0f;
    pdef->nSummonMaxConcurrent = 1;
    pdef->nSummonMaxTotal = 1;

    pdef->bDoesNotDropNecroCorpse = FALSE;
}
