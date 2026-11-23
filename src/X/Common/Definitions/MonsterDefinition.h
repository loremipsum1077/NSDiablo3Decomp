// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/MonsterDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions.h"
#include "../GameBalance.h"
#include "../MonsterStruct.h"
#include "../Monsters.h"
#include "../SNO/SNOFiles.h"
#include "GameBalanceDefinition.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"

class CTagMap;  // X/Common/TagMaps/Tags.h; only held through XPtr64 here

typedef int32 MonsterType;  // X/Common/ActorCommonDataSwig.h
typedef int32 MonsterFamily;  // X/Common/ActorCommonDataSwig.h
typedef int32 ResourceType;  // X/Common/ActorCommonDataSwig.h
#include "../AI/ActorAIStatesSwig.h"  // AITargetStyle

enum {
    MONSTER_DEFINITION_INTERNAL_NAMELEN = 128,
};

struct MonsterMinionSpawnItem {  // size 0x14
    SNO snoSpawn;  // 0x00
    int32 nSpawnCountMin;  // 0x04
    int32 nSpawnCountMax;  // 0x08
    int32 nSpawnSpreadMin;  // 0x0c
    int32 nSpawnSpreadMax;  // 0x10
};
static_assert(sizeof(MonsterMinionSpawnItem) == 0x14, "MonsterMinionSpawnItem");

struct MonsterMinionSpawnGroup {  // size 0x18
    float nWeight;  // 0x00
    int32 nSpawnItemCount;  // 0x04
    XPtr64<MonsterMinionSpawnItem> arSpawnItems;  // 0x08
    SerializeData serSpawnItems;  // 0x10
};
static_assert(sizeof(MonsterMinionSpawnGroup) == 0x18, "MonsterMinionSpawnGroup");

struct MonsterChampionSpawnItem {  // size 0x8
    SNO snoActor;  // 0x00
    int32 nSpawnCount;  // 0x04
};
static_assert(sizeof(MonsterChampionSpawnItem) == 0x8, "MonsterChampionSpawnItem");

struct MonsterChampionSpawnGroup {  // size 0x18
    int32 nSpawnItemCount;  // 0x00
    float flWeight;  // 0x04
    XPtr64<MonsterChampionSpawnItem> arSpawnItems;  // 0x08
    SerializeData serSpawnItems;  // 0x10
};
static_assert(sizeof(MonsterChampionSpawnGroup) == 0x18, "MonsterChampionSpawnGroup");

enum {
    MONSTERDEF_IS_UNIQUE_BIT = 0,
    MONSTERDEF_IS_LARGE_MONSTER_BIT = 1,
    MONSTERDEF_IS_ELITE_BIT = 2,
    MONSTERDEF_HIRELING_OR_PET_BIT = 3,
    MONSTERDEF_IS_FEMALE_BIT = 4,
    MONSTERDEF_IS_BOSS_BIT = 5,
    MONSTERDEF_ALWAYS_USES_SELF_FOR_AWARENESS_RADIUS_BIT = 6,
    MONSTERDEF_FORCES_FLAVOR_TEXT_BIT = 7,
};

struct MonsterDefinition {  // size 0x558
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    SNO snoActor;  // 0x10
    int32 nLookIndex;  // 0x14
    MonsterType eMonsterType;  // 0x18
    MonsterFamily eMonsterFamily;  // 0x1c
    EncounterCategory eEncounterCategory;  // 0x20
    MonsterAwarenessInfo tAwareness;  // 0x24
    DamageType eBaseElement;  // 0x38
    int32 nDefaultCountMin;  // 0x3c
    int32 nDefaultCountDelta;  // 0x40
    float pctAttributeModifiers[MAX_MONSTER_TABLE_ATTRIBUTES];  // 0x44
    float flHPChampion;  // 0x28c
    float flHPDeltaChampion;  // 0x290
    float flHPRare;  // 0x294
    float flHPDeltaRare;  // 0x298
    float flHPMinion;  // 0x29c
    float flHPDeltaMinion;  // 0x2a0
    int32 nGoldGranted;  // 0x2a4
    HealthDropInfo tHealthDropNormal;  // 0x2a8
    HealthDropInfo tHealthDropChampion;  // 0x2b4
    HealthDropInfo tHealthDropRare;  // 0x2c0
    HealthDropInfo tHealthDropMinion;  // 0x2cc
    SNO snoSkillKit;  // 0x2d8
    SkillDeclaration tSkills[MAX_MONSTER_SKILLS];  // 0x2dc
    MonsterSkillDeclaration tMonsterSkills[MAX_MONSTER_SKILLS];  // 0x31c
    SNO snoTreasureClassFirstKill;  // 0x39c
    SNO snoTreasureClass;  // 0x3a0
    SNO snoTreasureClassRare;  // 0x3a4
    SNO snoTreasureClassChampion;  // 0x3a8
    SNO snoTreasureClassChampionLight;  // 0x3ac
    float flNoDropScalar;  // 0x3b0
    float flFleeChance;  // 0x3b4
    float flFleeCooldownMin;  // 0x3b8
    float flFleeCooldownDelta;  // 0x3bc
    int32 nSummonCountPer;  // 0x3c0
    float flSummonLifetime;  // 0x3c4
    int32 nSummonMaxConcurrent;  // 0x3c8
    int32 nSummonMaxTotal;  // 0x3cc
    SNO snoInventory;  // 0x3d0
    SNO snoSecondaryInventory;  // 0x3d4
    SNO snoLore;  // 0x3d8
    SNO arAIBehaviors[6];  // 0x3dc
    GBID gbidMovementStyles[8];  // 0x3f4
    SNO snoSummonActor[MAX_SUMMON_ACTORS];  // 0x414
    int32 nRandomAffixes;  // 0x42c
    GBID gbidAffixes[MAX_MONSTER_AFFIXES];  // 0x430
    GBID gbidDisallowedAffixes[MAX_DISALLOWED_MONSTER_AFFIXES];  // 0x440
    AITargetStyle eAITargetStyleNormal;  // 0x458
    AITargetStyle eAITargetStyleChampion;  // 0x45c
    AITargetStyle eAITargetStyleRare;  // 0x460
    ResourceType eResourceType;  // 0x464
    AttributeSpecifier ptAttributes[1];  // 0x468
    SerializeData serTagMap;  // 0x480
    XPtr64<CTagMap> hTagMap;  // 0x488
    int32 nMinionSpawnGroupCount;  // 0x490
    XPtr64<MonsterMinionSpawnGroup> arMinionSpawnGroups;  // 0x498
    SerializeData serMinionSpawnGroups;  // 0x4a0
    int32 nChampionSpawnGroupCount;  // 0x4a8
    XPtr64<MonsterChampionSpawnGroup> arChampionSpawnGroups;  // 0x4b0
    SerializeData serChampionSpawnGroups;  // 0x4b8
    CHAR szInternalName[MONSTER_DEFINITION_INTERNAL_NAMELEN];  // 0x4c0
    BOOL bDoesNotDropNecroCorpse;  // 0x540
    int32 nPad;  // 0x544
    SNO snoAIStateAttackerCapReached;  // 0x548
    BOOL bDontThrottleDropsOnConsole;  // 0x54c
    BOOL bShowHealthBarWithOwnerOnConsole;  // 0x550
    BOOL bCanBeAvenger;  // 0x554
};
static_assert(sizeof(MonsterDefinition) == 0x558, "MonsterDefinition");

class MonsterGroupDef : public GroupDefinition {  // size 0xa0
public:
    MonsterGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 190
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 192
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(MonsterGroupDef) == 0xa0, "MonsterGroupDef");

extern const XTypeDescriptor * DT_HEALTHDROPINFO;  // MonsterDefinition.cpp:53
extern const XTypeDescriptor * DT_SKILLDECLARATION;  // MonsterDefinition.cpp:58
extern const XTypeDescriptor * DT_MONSTER_SKILLDECLARATION;  // MonsterDefinition.cpp:65
extern const XTypeDescriptor * DT_MONSTER_AWARENESS_INFO;  // MonsterDefinition.cpp:73
extern const XTypeDescriptor * DT_MONSTER_MINION_SPAWN_ITEM;  // MonsterDefinition.cpp:81
extern const XTypeDescriptor * DT_MONSTER_MINION_SPAWN_GROUP;  // MonsterDefinition.cpp:87
extern const XTypeDescriptor * DT_MONSTER_CHAMPION_SPAWN_ITEM;  // MonsterDefinition.cpp:92
extern const XTypeDescriptor * DT_MONSTER_CHAMPION_SPAWN_GROUP;  // MonsterDefinition.cpp:98
extern const XTypeDescriptor * DT_MONSTER_DEFINITION;  // MonsterDefinition.cpp:188
extern MonsterGroupDef * gptGroupDefMonster;  // MonsterDefinition.cpp:193

typedef ScopedDefinitionLockR<MonsterDefinition> ScopedMonsterDefinitionLockR;

template <> inline const MonsterDefinition * DefinitionLockR<MonsterDefinition>(SNO sno) {  // line 213
    return (const MonsterDefinition *)gptGroupDefMonster->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<MonsterDefinition>(const MonsterDefinition * pdef) {  // line 214
    void * lpDef = (void *)pdef;
    gptGroupDefMonster->UnlockR(lpDef);
}
