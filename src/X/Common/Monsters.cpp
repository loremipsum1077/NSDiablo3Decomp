#include "Monsters.h"

#include "ActorCommonData.h"
#include "GameBalance.h"
#include "GlobalSNO.h"
#include "MonsterStruct.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/MonsterDefinition.h"
#include "TagMaps/ActorTags.h"
#include "UserInterface/FontDefinition.h"
#include "../Sigma/LanguageRules.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"
#include "../Sigma/RefString.h"

typedef int32 FontSpecifier;  // X/Common/UserInterface/FontDefinition.h

int32 GameGetMonsterLevel();  // X/Common/Game.cpp
const GameBalanceDefinition * MonsterLevelTableLockR();  // X/Common/Definitions/GameBalanceDefinition.cpp
void MonsterLevelTableUnlockR(const GameBalanceDefinition * ptMonsterLevelGBDef);  // X/Common/Definitions/GameBalanceDefinition.cpp
BOOL StringListFetch(SNO snoStringList, LPCSTR szLabel, CRefString & sResult);  // X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
CRefString StringListEntryReplace(LPCSTR szString, LPCSTR szToken, LPCSTR szReplacement, BOOL bReplaceAll);  // X/Common/Definitions/StringListDefinition.cpp
void Set_Text_Color(CRefString & sText, FontSpecifier eColor);  // X/Common/UserInterface/UITextControl.cpp
LPCSTR MarkerNameGetString(uint32 tName);  // X/Common/Marker/MarkerName.cpp

BOOL MonsterGetSNONameFromActor(SNO snoActor, SNOName & tSNOName)
{
    ScopedActorDefinitionLockR pdefActor(snoActor);

    if (pdefActor && pdefActor->eType == ACTORTYPE_MONSTER)
    {
        if (pdefActor->snoMonster != SNO_NONE)
        {
            tSNOName.snoGroup = MONSTER_SNO;
            tSNOName.snoHandle = pdefActor->snoMonster;
            return TRUE;
        }
    }

    tSNOName.snoGroup = SNOGROUP_NONE;
    tSNOName.snoHandle = SNO_NONE;
    return FALSE;
}

BOOL MonsterExists(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (!bHasMonsterSNO)
        return FALSE;

    return MonsterExists(tMonsterSNOName.snoHandle);
}

BOOL MonsterExists(SNO snoMonster)
{
    return SNOFileExists(MONSTER_SNO, snoMonster);
}

inline CRefString MonsterGetSNOName(SNO snoMonster)
{
    return SNOToString(MONSTER_SNO, snoMonster, FALSE);
}

CRefString MonsterGetSNOName(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
        return MonsterGetSNOName(tMonsterSNOName.snoHandle);

    CRefString strBlank;
    return strBlank;
}

BOOL MonsterIsLargeMonster(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (!bHasMonsterSNO)
        return FALSE;

    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        return (pdefMonster->dwFlags >> MONSTERDEF_IS_LARGE_MONSTER_BIT) & 1;
    }
}

MonsterType MonsterGetMonsterType(const SNOName & tActorSNOName)
{
    if (tActorSNOName.snoGroup != ACTOR_SNO)
        return -1;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    return pdefMonster->eMonsterType;
}

MonsterFamily MonsterGetMonsterFamily(const SNOName & tActorSNOName)
{
    if (tActorSNOName.snoGroup != ACTOR_SNO)
        return -1;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    return pdefMonster->eMonsterFamily;
}

DamageType MonsterGetBaseElement(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    return pdefMonster->eBaseElement;
}

EncounterCategory MonsterGetMonsterCategory(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    return pdefMonster->eEncounterCategory;
}

void MonsterGetAwareness(const SNOName & tActorSNOName, MonsterAwarenessInfo & tAwareness)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    SigmaMemoryCopy(&tAwareness, &pdefMonster->tAwareness, sizeof(tAwareness));
}

float MonsterGetScanRadius(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    return pdefMonster->tAwareness.flCombatRadius;
}

int32 MonsterGetLevel(const SNOName & tActorSNOName)
{
    int32 nLevel = GameGetMonsterLevel();
    BOOL bMerchant = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_TRADE, tActorSNOName.snoHandle);
    if (bMerchant)
        nLevel = Math::Max(nLevel, (int32)MERCHANT_LEVEL_MIN);

    return nLevel;
}

int32 MonsterGetGoldGranted(const SNOName & tActorSNOName)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    return pdefMonster->nGoldGranted;
}

SkillDeclaration MonsterGetSkill(const SNOName & tActorSNOName, const int32 nSkillIndex)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedActorDefinitionLockR pdefActor(tActorSNOName.snoHandle);
    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);

    return pdefMonster->tSkills[nSkillIndex];
}

MonsterSkillDeclaration MonsterGetMonsterSkill(const SNOName & tActorSNOName, const int32 nSkillIndex)
{
    MonsterSkillDeclaration tMonsterSkillDec;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    tMonsterSkillDec = pdefMonster->tMonsterSkills[nSkillIndex];

    return tMonsterSkillDec;
}

float MonsterGetNoDropScalar(const SNOName & tActorSNOName)
{
    float flScalar = 1.0f;

    if (tActorSNOName.snoGroup == ACTOR_SNO)
    {
        SNOName tMonsterSNOName;
        BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
        if (bHasMonsterSNO)
        {
            ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
            flScalar = pdefMonster->flNoDropScalar;
        }
    }

    return flScalar;
}

void MonsterGetFleeInfo(const SNOName & tActorSNOName, float & flChance, float & flCooldownMin, float & flCooldownDelta)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    flChance = 0.0f;
    flCooldownMin = 0.0f;
    flCooldownDelta = 0.0f;

    if (bHasMonsterSNO && MonsterExists(tActorSNOName))
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        flChance = pdefMonster->flFleeChance;
        flCooldownMin = pdefMonster->flFleeCooldownMin;
        flCooldownDelta = pdefMonster->flFleeCooldownDelta;
    }
}

BOOL MonsterGetHealthDropInfo(const SNOName & tActorSNOName, const MonsterRarity eRarity, HealthDropInfo & tHealthDropInfo)
{
    BOOL bResult = FALSE;

    if (tActorSNOName.snoGroup != ACTOR_SNO)
        return FALSE;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        switch (eRarity)
        {
            case MR_NORMAL:
            case MR_UNIQUE:
            case MR_BOSS:
                tHealthDropInfo = pdefMonster->tHealthDropNormal;
                bResult = TRUE;
                break;

            case MR_CHAMPION:
                tHealthDropInfo = pdefMonster->tHealthDropChampion;
                bResult = TRUE;
                break;

            case MR_RARE:
                tHealthDropInfo = pdefMonster->tHealthDropRare;
                bResult = TRUE;
                break;

            case MR_RARE_MINION:
                tHealthDropInfo = pdefMonster->tHealthDropMinion;
                bResult = TRUE;
                break;
        }
    }

    if (!bResult)
        SigmaMemoryClear(&tHealthDropInfo, sizeof(tHealthDropInfo));

    return bResult;
}

GBID MonsterGetMovementStyle(const SNOName & tActorSNOName, AIStateType eStateType)
{
    GBID gbidStyle = GBID_NONE;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        gbidStyle = pdefMonster->gbidMovementStyles[eStateType];
    }

    return gbidStyle;
}

SNO MonsterGetRandomSummonActor(const SNOName & tActorSNOName, RNG & tRNG)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        int32 nActorCount = 0;
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);

        for (int32 i = 0; i < MAX_SUMMON_ACTORS; i++)
        {
            SNO snoActor = pdefMonster->snoSummonActor[i];
            if (snoActor != SNO_NONE)
                nActorCount++;
        }

        if (nActorCount > 0)
        {
            int32 nRandomIndex = RandRange_Internal(tRNG, 0, nActorCount - 1);
            return pdefMonster->snoSummonActor[nRandomIndex];
        }
    }

    return SNO_NONE;
}

SNO MonsterGetSummonActor(const SNOName & tActorSNOName, const int32 nSummonIndex)
{
    SNO snoActor = SNO_NONE;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        snoActor = pdefMonster->snoSummonActor[nSummonIndex];
    }

    return snoActor;
}

int32 MonsterGetSummonCountPer(const SNOName & tActorSNOName)
{
    int32 nSummonCountPer = 1;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        nSummonCountPer = pdefMonster->nSummonCountPer;
    }

    return nSummonCountPer;
}

float MonsterGetSummonLifetime(const SNOName & tActorSNOName)
{
    float flLifetime = 0.0f;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        flLifetime = pdefMonster->flSummonLifetime;
    }

    return flLifetime;
}

int32 MonsterGetSummonMaxConcurrent(const SNOName & tActorSNOName)
{
    int32 nMaxConcurrent = 1;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        nMaxConcurrent = pdefMonster->nSummonMaxConcurrent;
    }

    return nMaxConcurrent;
}

int32 MonsterGetSummonMaxTotal(const SNOName & tActorSNOName)
{
    int32 nMaxTotal = 1;

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        nMaxTotal = pdefMonster->nSummonMaxTotal;
    }

    return nMaxTotal;
}

// The GameBalanceAttribute enumerators are not adopted yet; the case labels are their values.
static float sGetMonsterLevelAttribute(const MonsterLevelDefinition * ptMonster, const GameBalanceAttribute eMonsterAttribute)
{
    switch (eMonsterAttribute)
    {
        case 120: return ptMonster->flAttributesStrength;
        case 121: return ptMonster->flAttributesDexterity;
        case 123: return ptMonster->flAttributesIntelligence;
        case 122: return ptMonster->flAttributesVitality;
        case 4: return ptMonster->flAttributesHitPoints;
        case 7: return ptMonster->flAttributesHPRange;
        case 8: return ptMonster->flAttributesHPRegen;
        case 9: return ptMonster->flAttributesPrimaryResourceBase;
        case 11: return ptMonster->flAttributesPrimaryResourceRegen;
        case 45: return ptMonster->flAttributesArmor;
        case 22: return ptMonster->flAttributesDamage;
        case 23: return ptMonster->flAttributesDamageRange;
        case 24: return ptMonster->flAttributesDamageFire;
        case 25: return ptMonster->flAttributesDamageRangeFire;
        case 26: return ptMonster->flAttributesDamageLightning;
        case 27: return ptMonster->flAttributesDamageRangeLightning;
        case 28: return ptMonster->flAttributesDamageCold;
        case 29: return ptMonster->flAttributesDamageRangeCold;
        case 30: return ptMonster->flAttributesDamagePoison;
        case 31: return ptMonster->flAttributesDamageRangePoison;
        case 32: return ptMonster->flAttributesDamageArcane;
        case 33: return ptMonster->flAttributesDamageRangeArcane;
        case 109: return ptMonster->flAttributesDamageHoly;
        case 110: return ptMonster->flAttributesDamageRangeHoly;
        case 34: return ptMonster->flAttributesDamageSiege;
        case 35: return ptMonster->flAttributesDamageRangeSiege;
        case 114: return ptMonster->flAttributesHirelingHitPoints;
        case 115: return ptMonster->flAttributesHirelingHPRange;
        case 116: return ptMonster->flAttributesHirelingHPRegen;
        case 117: return ptMonster->flAttributesHirelingDamage;
        case 118: return ptMonster->flAttributesHirelingDamageRange;
        case 125: return ptMonster->flAttributesHirelingRetrainCost;
        case 36: return ptMonster->flAttributesGetHitDamage;
        case 37: return ptMonster->flAttributesGetHitDamageScalar;
        case 38: return ptMonster->flAttributesGetHitMax;
        case 39: return ptMonster->flAttributesGetHitRecovery;
        case 54: return ptMonster->flAttributesWalkSpeed;
        case 55: return ptMonster->flAttributesRunSpeed;
        case 43: return ptMonster->flAttributesSprintSpeed;
        case 129: return ptMonster->flAttributesStrafeSpeed;
        case 44: return ptMonster->flAttributesAttackSpeed;
        case 57: return ptMonster->flAttributesProjSpeed;
        case 58: return ptMonster->flAttributesExperience;
        case 66: return ptMonster->flAttributesResistPhysical;
        case 67: return ptMonster->flAttributesResistFire;
        case 68: return ptMonster->flAttributesResistLightning;
        case 69: return ptMonster->flAttributesResistCold;
        case 70: return ptMonster->flAttributesResistPoison;
        case 71: return ptMonster->flAttributesResistArcane;
        case 72: return ptMonster->flAttributesResistSiege;
        case 73: return ptMonster->flAttributesResistChill;
        case 74: return ptMonster->flAttributesResistStun;
        case 140: return ptMonster->flAttributesConsoleHealthScalar;
        case 141: return ptMonster->flAttributesConsoleDamageScalar;
        case 142: return ptMonster->flAttributesMonster1AffixWeight;
        case 143: return ptMonster->flAttributesMonster2AffixWeight;
        case 144: return ptMonster->flAttributesMonster3AffixWeight;
        case 145: return ptMonster->flAttributesMonster4AffixWeight;
    }

    return 0.0f;
}

float GetMonsterLevelAttribute(int32 nLevel, SNO snoMonster, const GameBalanceAttribute eMonsterAttribute)
{
    float flAdjustment = 1.0f;

    const GameBalanceDefinition * ptMonsterLevelGBDef = MonsterLevelTableLockR();

    if (snoMonster != SNO_NONE)
    {
        ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
        flAdjustment = pdefMonster->pctAttributeModifiers[eMonsterAttribute];
    }

    float flRetval = sGetMonsterLevelAttribute(&ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1], eMonsterAttribute) * flAdjustment;

    MonsterLevelTableUnlockR(ptMonsterLevelGBDef);

    return flRetval;
}

float GetHirelingMonsterLevelAttribute(const int32 nLevel, const SNO snoMonster, const GameBalanceAttribute eMonsterAttribute, const GameBalanceAttribute eMonsterHirelingAttribute)
{
    GameBalanceAttribute eAttribToUse = eMonsterAttribute;
    float flAdjustment = 1.0f;

    const GameBalanceDefinition * ptMonsterLevelGBDef = MonsterLevelTableLockR();

    if (snoMonster != SNO_NONE)
    {
        ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
        if (pdefMonster->dwFlags & (1 << MONSTERDEF_HIRELING_OR_PET_BIT))
            eAttribToUse = eMonsterHirelingAttribute;

        flAdjustment = pdefMonster->pctAttributeModifiers[eMonsterAttribute];
    }

    float flRetval = sGetMonsterLevelAttribute(&ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1], eAttribToUse) * flAdjustment;

    MonsterLevelTableUnlockR(ptMonsterLevelGBDef);

    return flRetval;
}

float GetMonsterLevelAttribute(int32 nLevel, const MonsterDefinition * pdefMonster, const GameBalanceAttribute eMonsterAttribute, const GameBalanceDefinition * ptMonsterLevelGBDef)
{
    float flAdjustment = pdefMonster->pctAttributeModifiers[eMonsterAttribute];

    float flRetval = sGetMonsterLevelAttribute(&ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1], eMonsterAttribute) * flAdjustment;

    return flRetval;
}

float GetHirelingMonsterLevelAttribute(const int32 nLevel, const MonsterDefinition * pdefMonster, const GameBalanceAttribute eMonsterAttribute, const GameBalanceAttribute eMonsterHirelingAttribute, const GameBalanceDefinition * ptMonsterLevelGBDef)
{
    GameBalanceAttribute eAttribToUse = eMonsterAttribute;
    if (pdefMonster->dwFlags & (1 << MONSTERDEF_HIRELING_OR_PET_BIT))
        eAttribToUse = eMonsterHirelingAttribute;

    float flAdjustment = pdefMonster->pctAttributeModifiers[eMonsterAttribute];

    float flRetval = sGetMonsterLevelAttribute(&ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1], eAttribToUse) * flAdjustment;

    return flRetval;
}

float GetMonsterAttribute(const MonsterDefinition * pdefMonster, const GameBalanceAttribute eMonsterAttribute)
{
    return pdefMonster->pctAttributeModifiers[eMonsterAttribute];
}

ResourceType GetMonsterResourceType(const MonsterDefinition * pdefMonster)
{
    return pdefMonster->eResourceType;
}

static CRefString sMonsterNameTableGetName(GBID gbidMonsterName, const BOOL fIsFemale)
{
    ScopedRareMonsterNameEntryLockR pName(gbidMonsterName);

    if (pName == NULL)
        return CRefString("!! MISSING RARE NAME");

    uint32 dwGrammaticalForm = 1 << (fIsFemale ? GF_FEMININE_BIT : GF_MASCULINE_BIT);

    CRefString usRetval;
    uint32 dwExtractionFlagsIgnored;
    if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTER_NAMES), pName->tHeader.szName, usRetval))
        LanguageRules::ExtractMatchingGrammaticalForm(usRetval, dwGrammaticalForm, dwExtractionFlagsIgnored);

    return usRetval;
}

BOOL MonsterGetBaseName(SNO snoMonster, CRefString & usReturnString)
{
    if (!MonsterExists(snoMonster))
    {
        usReturnString = "[bad monster]";
        return FALSE;
    }

    CRefString sName = MonsterGetSNOName(snoMonster);

    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTERS), sName, usReturnString);
}

BOOL MonsterGetFlavor(SNO snoMonster, CRefString & usReturnString)
{
    if (!MonsterExists(snoMonster))
    {
        usReturnString = "[bad monster]";
        return FALSE;
    }

    CRefString sName = MonsterGetSNOName(snoMonster);

    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTER_FLAVORS), sName, usReturnString);
}

BOOL MonsterGetBaseName(const SNOName & tActorSNOName, CRefString & usReturnString)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (!bHasMonsterSNO)
        return FALSE;

    return MonsterGetBaseName(tMonsterSNOName.snoHandle, usReturnString);
}

static FontSpecifier sGetMonsterTextColor(const MonsterRarity eRarity)
{
    switch (eRarity)
    {
        case MR_CHAMPION: return FONTCOLOR_BLUE;
        case MR_RARE: return FONTCOLOR_YELLOW;
        case MR_UNIQUE: return FONTCOLOR_GOLD;
        case MR_BOSS: return FONTCOLOR_GOLD;
        default: return FONTCOLOR_WHITE;
    }
}

CRefString MonsterGetName(const SNOName & tActorSNOName, const GBID * pRareNameGBIDs, MonsterRarity eRarity, const BOOL bSetTextColor)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);

    return MonsterGetName(tMonsterSNOName.snoHandle, pRareNameGBIDs, eRarity, bSetTextColor);
}

CRefString MonsterGetName(SNO snoMonster, const GBID * pRareNameGBIDs, MonsterRarity eRarity, const BOOL bSetTextColor)
{
    if (snoMonster == SNO_NONE)
        return CRefString("");

    ScopedMonsterDefinitionLockR pdefMonster(snoMonster);

    BOOL fIsFemale = (pdefMonster->dwFlags >> MONSTERDEF_IS_FEMALE_BIT) & 1;

    CRefString usMonsterName;

    if (eRarity == MR_RARE)
    {
        CRefString usPrefix = sMonsterNameTableGetName(pRareNameGBIDs[0], fIsFemale);
        CRefString usSuffix = sMonsterNameTableGetName(pRareNameGBIDs[1], fIsFemale);
        usMonsterName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "MonsterRare");
        usMonsterName = StringListEntryReplace(usMonsterName, "{PREFIX}", usPrefix, 999);
        usMonsterName = StringListEntryReplace(usMonsterName, "{SUFFIX}", usSuffix, 999);
    }
    else
    {
        CRefString sInitialName;
        CRefString sName2Ignored;
        uint32 dwGrammaticalForm;
        MonsterGetBaseName(snoMonster, sInitialName);

        LanguageRules::ExtractGrammaticalForm(sInitialName, usMonsterName, sName2Ignored, dwGrammaticalForm);
    }

    if (bSetTextColor == TRUE)
        Set_Text_Color(usMonsterName, sGetMonsterTextColor(eRarity));

    return usMonsterName;
}

CRefString MonsterGetName(const ActorCommonData * ptACD, const BOOL bSetTextColor)
{
    if (ptACD == NULL)
        return CRefString("ACDID_INVALID");

    if (ptACD->IsMonster())
        return MonsterGetName(ptACD->tSNOName, ptACD->pRareNameGBIDs, ptACD->eMonsterRarity, bSetTextColor);

    return CRefString(MarkerNameGetString(ptACD->tName));
}

AITargetStyle MonsterGetTargetStyle(const SNOName & tActorSNOName, MonsterRarity eMonsterRarity)
{
    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tActorSNOName.snoHandle, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
        switch (eMonsterRarity)
        {
            case MR_RARE:
                return pdefMonster->eAITargetStyleRare;
            case MR_CHAMPION:
                return pdefMonster->eAITargetStyleChampion;
            default:
                return pdefMonster->eAITargetStyleNormal;
        }
    }

    return AITARGET_STYLE_CLOSEST;
}
