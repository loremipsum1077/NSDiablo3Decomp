// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Monsters.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SNO/SNOFiles.h"

enum MonsterLevels {
    MONSTER_LEVEL_MAX = 99,
    MERCHANT_LEVEL_MIN = 4,
};

struct SkillDeclaration {  // size 0x8
    SNO snoPower;  // 0x00
    int32 nLevelMod;  // 0x04
};
static_assert(sizeof(SkillDeclaration) == 0x8, "SkillDeclaration");

struct MonsterSkillDeclaration {  // size 0x10
    float flUseRangeMin;  // 0x00
    float flUseRangeMax;  // 0x04
    uint32 dwWeight;  // 0x08
    float flTimer;  // 0x0c
};
static_assert(sizeof(MonsterSkillDeclaration) == 0x10, "MonsterSkillDeclaration");

typedef int32 DamageType;


struct SNOName;
struct RNG;
struct HealthDropInfo;
struct MonsterAwarenessInfo;
struct MonsterDefinition;
struct GameBalanceDefinition;
struct ActorCommonData;
class CRefString;

typedef int32 GBID;  // X/Common/GameBalance.h
typedef int32 MonsterType;  // X/Common/ActorCommonDataSwig.h
typedef int32 MonsterFamily;  // X/Common/ActorCommonDataSwig.h
typedef int32 ResourceType;  // X/Common/ActorCommonDataSwig.h
typedef int32 MonsterRarity;  // X/Common/ActorCommonDataSwig.h
typedef int32 EncounterCategory;  // X/Common/MonsterStruct.h
typedef int32 AIStateType;  // X/Common/AI/ActorAIStatesSwig.h
typedef int32 AITargetStyle;  // X/Common/AI/ActorAIStatesSwig.h
typedef int32 GameBalanceAttribute;  // X/Common/Definitions/GameBalanceDefinition.h

BOOL MonsterGetSNONameFromActor(SNO snoActor, SNOName & tSNOName);
BOOL MonsterExists(const SNOName & tActorSNOName);
BOOL MonsterExists(SNO snoMonster);
CRefString MonsterGetSNOName(SNO snoMonster);
CRefString MonsterGetSNOName(const SNOName & tActorSNOName);
BOOL MonsterIsLargeMonster(const SNOName & tActorSNOName);
MonsterType MonsterGetMonsterType(const SNOName & tActorSNOName);
MonsterFamily MonsterGetMonsterFamily(const SNOName & tActorSNOName);
DamageType MonsterGetBaseElement(const SNOName & tActorSNOName);
EncounterCategory MonsterGetMonsterCategory(const SNOName & tActorSNOName);
void MonsterGetAwareness(const SNOName & tActorSNOName, MonsterAwarenessInfo & tAwareness);
float MonsterGetScanRadius(const SNOName & tActorSNOName);
int32 MonsterGetLevel(const SNOName & tActorSNOName);
int32 MonsterGetGoldGranted(const SNOName & tActorSNOName);
SkillDeclaration MonsterGetSkill(const SNOName & tActorSNOName, const int32 nSkillIndex);
MonsterSkillDeclaration MonsterGetMonsterSkill(const SNOName & tActorSNOName, const int32 nSkillIndex);
float MonsterGetNoDropScalar(const SNOName & tActorSNOName);
void MonsterGetFleeInfo(const SNOName & tActorSNOName, float & flChance, float & flCooldownMin, float & flCooldownDelta);
BOOL MonsterGetHealthDropInfo(const SNOName & tActorSNOName, const MonsterRarity eRarity, HealthDropInfo & tHealthDropInfo);
GBID MonsterGetMovementStyle(const SNOName & tActorSNOName, AIStateType eStateType);
SNO MonsterGetRandomSummonActor(const SNOName & tActorSNOName, RNG & tRNG);
SNO MonsterGetSummonActor(const SNOName & tActorSNOName, const int32 nSummonIndex);
int32 MonsterGetSummonCountPer(const SNOName & tActorSNOName);
float MonsterGetSummonLifetime(const SNOName & tActorSNOName);
int32 MonsterGetSummonMaxConcurrent(const SNOName & tActorSNOName);
int32 MonsterGetSummonMaxTotal(const SNOName & tActorSNOName);
float GetMonsterLevelAttribute(int32 nLevel, SNO snoMonster, const GameBalanceAttribute eMonsterAttribute);
float GetHirelingMonsterLevelAttribute(const int32 nLevel, const SNO snoMonster, const GameBalanceAttribute eMonsterAttribute, const GameBalanceAttribute eMonsterHirelingAttribute);
float GetMonsterLevelAttribute(int32 nLevel, const MonsterDefinition * pdefMonster, const GameBalanceAttribute eMonsterAttribute, const GameBalanceDefinition * ptMonsterLevelGBDef);
float GetHirelingMonsterLevelAttribute(const int32 nLevel, const MonsterDefinition * pdefMonster, const GameBalanceAttribute eMonsterAttribute, const GameBalanceAttribute eMonsterHirelingAttribute, const GameBalanceDefinition * ptMonsterLevelGBDef);
float GetMonsterAttribute(const MonsterDefinition * pdefMonster, const GameBalanceAttribute eMonsterAttribute);
ResourceType GetMonsterResourceType(const MonsterDefinition * pdefMonster);
BOOL MonsterGetBaseName(SNO snoMonster, CRefString & usReturnString);
BOOL MonsterGetFlavor(SNO snoMonster, CRefString & usReturnString);
BOOL MonsterGetBaseName(const SNOName & tActorSNOName, CRefString & usReturnString);
CRefString MonsterGetName(const SNOName & tActorSNOName, const GBID * pRareNameGBIDs, MonsterRarity eRarity, const BOOL bSetTextColor);
CRefString MonsterGetName(SNO snoMonster, const GBID * pRareNameGBIDs, MonsterRarity eRarity, const BOOL bSetTextColor);
CRefString MonsterGetName(const ActorCommonData * ptACD, const BOOL bSetTextColor);
AITargetStyle MonsterGetTargetStyle(const SNOName & tActorSNOName, MonsterRarity eMonsterRarity);
