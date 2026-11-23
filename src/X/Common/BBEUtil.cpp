#include "BBEUtil.h"
#include "Attributes/FastAttribute.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include <strings.h>
#include "BBE.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include <string.h>
BOOL FastAttributeBBECompileAttrib(const TokenizedIdentifier & tTokenizeIdentifier, IdentifierResult * ptIdentifierResult);  // X/Common/Attributes/FastAttribute.cpp:2501
void PowerTagBBEIdentifierCompile(const TokenizedIdentifier & tTokenizeIdentifier, IdentifierResult * ptIdentifierResult);  // X/Common/TagMaps/PowerTags.cpp:1730



#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/GlobalDefinition.h"
#include "Game.h"
#include "GlobalSNO.h"
#include "Monsters.h"
#include "Player.h"
#include "../Sigma/Math/MathGeneral.h"
#include "ActorCommon.h"
#include "TagMaps/PowerTags.h"
float FastAttributeEvaluateBBEIdentifier(int32 nIdentifier, int32 nParam0, int32 nParam1, int32 nParam2, const BBECallbackData & tCallbackData);  // X/Common/Attributes/FastAttribute.cpp:4025
float PowerTagEvaluateBBEIdentifier(AttribGroupID idFastAttrib, int32 nSkillLevel, SNO snoPower, uint32 dwPowerTag);  // X/Common/TagMaps/PowerTags.cpp:1786
const GameBalanceDefinition * MonsterLevelTableLockR();  // X/Common/Definitions/GameBalanceDefinition.cpp:2281
void MonsterLevelTableUnlockR(const GameBalanceDefinition * lpDef);  // X/Common/Definitions/GameBalanceDefinition.cpp:2290








void InitBBECallbackData(
    BBECallbackData * pData)
{
    SigmaMemoryClear(pData, sizeof(BBECallbackData));
    pData->idFastAttrib = -1;
    pData->gbidItem = -1;
    pData->snoPower = -1;
    pData->uFlags = 0;
}



static float sGetMonsterDamageValue(
    int32 nLevel,
    GameBalanceAttribute eAttribute,
    AttribGroupID idFastAttrib)
{


    if (nLevel < 1)
        return 0.0f;



    float flMult = 1.0f;
    int32 nNumPlayers = PlayerGetNumPlayersForDifficulty();
    int32 nPlayerMult = nNumPlayers - 1;
    if (nNumPlayers > 1)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;

        const DifficultyTuningParams & tTuningParams = pdefGlobal->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(FALSE)];

        flMult = 1.0f + nPlayerMult * tTuningParams.flMonsterDamagePercentPerPlayer;
    }

    float flHandicapScalar = GameGetHandicapDamageScalar();
// Inference: the initial -1 of nTieredLootRunLevel (line 72) has no instruction of its own; the DWARF local is read after a FastAttribGroupIsValid test, and -1 makes the later >= 0 test fail, matching the compiled branch structure.
    int32 nTieredLootRunLevel = -1;
    if (FastAttribGroupIsValid(idFastAttrib))
    {
        nTieredLootRunLevel = FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL));
    }

    if (nTieredLootRunLevel >= 0)
    {
        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

        int32 nTieredLootRunLevelZeroBased;
        nTieredLootRunLevelZeroBased = Math::Min(nTieredLootRunLevel, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);
// Inference: only the upper bound is tested (cmp w8, w9 / b.lt at line 85); a lower-bound test would leave an instruction, so none is written.
        if (nTieredLootRunLevelZeroBased < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
        {
            const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nTieredLootRunLevelZeroBased];
            flHandicapScalar = tTieredLootRunLevel.tLootRunValues.flDamageScalar;
        }
    }

    if (FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_IN_SET_DUNGEON_WORLD)) != 0)
        flHandicapScalar = 1.0f;



    return flMult * flHandicapScalar * GetMonsterLevelAttribute(nLevel, -1, eAttribute);
}



static float sGetMonsterHealthValue(
    int32 nLevel,
    AttribGroupID idFastAttrib)
{

    float flHP;


    const GameBalanceDefinition * ptMonsterLevelGBDef = MonsterLevelTableLockR();
    flHP = ptMonsterLevelGBDef->tMonsterLevelTable.ptMonsterLevels[nLevel - 1].flAttributesHitPoints;

    MonsterLevelTableUnlockR(ptMonsterLevelGBDef);
// Inference: same initial -1 for nTieredLootRunLevel (line 115) as in sGetMonsterDamageValue; no instruction of its own.
    int32 nTieredLootRunLevel = -1;
    if (FastAttribGroupIsValid(idFastAttrib))
    {
        nTieredLootRunLevel = FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL));
    }

    float flHandicapScalar = ActorGetHandicapHealthScalar();

    if (nTieredLootRunLevel >= 0)
    {
        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

        int32 nTieredLootRunLevelZeroBased;
        nTieredLootRunLevelZeroBased = Math::Min(nTieredLootRunLevel, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);
// Inference: single upper-bound test as in sGetMonsterDamageValue (line 85); the lower bound leaves no instruction.
        if (nTieredLootRunLevelZeroBased < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
        {
            const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nTieredLootRunLevelZeroBased];
            flHandicapScalar = tTieredLootRunLevel.tLootRunValues.flHealthScalar;
        }
    }

    if (FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_IN_SET_DUNGEON_WORLD)) != 0)
        flHandicapScalar = 1.0f;



    return flHP * flHandicapScalar;
}







// Inference (BBEUtil.cpp:631): every case ends in break and one return TRUE follows the switch. The shared return block has no line row of its own; with a return TRUE in each case the per-case `mov w1, #-1` of the script-formula calls was hoisted into the shared call tail (2812 bytes), with break the function matches the original (2984).
BOOL BBEIdentifierEvaluate(
    int32 nIdentifier,
    int32 nParam0,
    int32 nParam1,
    int32 nParam2,
    void * pData,
    float & flResult)
{


    flResult = 0.0f;
// Inference (BBEUtil.cpp:164): DWARF declares tCallbackData as a reference to BBECallbackData while pData is void *; the cast is the only way to form it. No instruction of its own.
    const BBECallbackData & tCallbackData = *(const BBECallbackData *)pData;  // pData is the BBECallbackData the callers pass (BBEEvaluate callbackData)
// Case 5 (BBE_IDENTIFIER_MONSTER_DAMAGE_TYPE) has no jump-table target of its own, so it falls to default.
    switch (nIdentifier)
    {
    case BBE_IDENTIFIER_FAST_ATTRIB:

        if (tCallbackData.idFastAttrib == -1)
            return FALSE;


























        flResult = FastAttributeEvaluateBBEIdentifier(nIdentifier, nParam0, nParam1, nParam2, tCallbackData);
        break;

    case BBE_IDENTIFIER_SKILL_LEVEL:
    {
        AttribGroupID idFastAttrib = tCallbackData.idFastAttrib;
        int32 nSkillLevel = tCallbackData.nSkillLevel;
        if (nSkillLevel == -1)
            nSkillLevel = FastAttribGetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, tCallbackData.snoPower));
        flResult = (float)nSkillLevel;
        break;
    }















    case BBE_IDENTIFIER_TRAIT_ACTIVE:
    {
        AttribGroupID idFastAttrib = tCallbackData.idFastAttrib;
        int32 nTrait = tCallbackData.bTraitActive;
        if (nTrait == 0)
            nTrait = FastAttribGetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_TRAIT, tCallbackData.snoPower));
        flResult = (float)nTrait;
        break;
    }















    case BBE_IDENTIFIER_SKILL_LEVEL_PREVIOUS:
    {
        AttribGroupID idFastAttrib = tCallbackData.idFastAttrib;
        int32 nLevel = tCallbackData.nSkillLevel;
        if (nLevel == -1)

            nLevel = FastAttribGetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, tCallbackData.snoPower));







        flResult = (nLevel - 1 < 0) ? 0.0f : (float)(nLevel - 1);
        break;
    }

    case BBE_IDENTIFIER_GIZMO_LEVEL:
    {
        AttribGroupID idFastAttrib = tCallbackData.idFastAttrib;
        flResult = (float)FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_LEVEL));
        break;
    }


    case BBE_IDENTIFIER_AREA_DAMAGE_MIN_PHYSICAL:
        flResult = sGetMonsterDamageValue(GameGetMonsterLevel(), GBA_DAMAGE, tCallbackData.idFastAttrib);
        break;


    case BBE_IDENTIFIER_AREA_DAMAGE_DELTA_PHYSICAL:
        flResult = sGetMonsterDamageValue(GameGetMonsterLevel(), GBA_DAMAGERANGE, tCallbackData.idFastAttrib);
        break;



    case BBE_IDENTIFIER_ITEM_LEVEL:
        if (tCallbackData.nLegendaryItemLevelOverride == 0)
        {
            ScopedItemDefinitionLockR ptItem(tCallbackData.gbidItem);
            if (ptItem == NULL)
                return FALSE;




            flResult = (float)ptItem->nItemLevel;
        }
        else
        {
            flResult = (float)tCallbackData.nLegendaryItemLevelOverride;
        }
        break;


    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_PHYSICAL:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_PHYSICAL:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_FIRE:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_FIRE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_FIRE:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_FIRE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_LIGHTNING:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_LIGHTNING, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_LIGHTNING:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_LIGHTNING, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_COLD:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_COLD, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_COLD:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_COLD, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_POISON:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_POISON, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_POISON:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_POISON, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_ARCANE:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_ARCANE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_ARCANE:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_ARCANE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_HOLY:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_HOLY, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_HOLY:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_HOLY, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_SIEGE:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGE_SIEGE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_SIEGE:
        flResult = sGetMonsterDamageValue(tCallbackData.nLevel, GBA_DAMAGERANGE_SIEGE, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_0:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_0);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_1:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_1);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_2:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_2);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_3:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_3);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_4:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_4);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_5:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_5);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_6:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_6);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_7:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_7);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_8:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_8);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_9:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_9);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_10:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_10);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_11:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_11);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_12:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_12);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_13:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_13);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_14:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_14);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_15:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_15);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_16:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_16);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_17:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_17);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_18:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_18);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_19:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_19);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_20:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_20);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_21:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_21);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_22:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_22);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_23:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_23);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_24:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_24);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_25:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_25);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_26:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_26);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_27:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_27);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_28:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_28);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_29:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_29);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_30:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_30);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_31:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_31);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_32:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_32);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_33:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_33);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_34:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_34);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_35:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_35);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_36:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_36);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_37:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_37);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_38:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_38);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_39:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_39);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_40:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_40);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_41:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_41);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_42:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_42);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_43:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_43);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_44:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_44);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_45:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_45);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_46:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_46);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_47:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_47);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_48:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_48);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_49:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_49);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_50:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_50);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_51:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_51);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_52:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_52);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_53:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_53);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_54:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_54);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_55:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_55);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_56:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_56);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_57:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_57);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_58:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_58);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_59:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_59);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_60:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_60);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_61:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_61);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_62:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_62);
        break;
    case BBE_IDENTIFIER_SCRIPT_FORMULA_63:
        flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, TAG_POWER_SCRIPT_FORMULA_63);
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_1:
        flResult = 0.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_2:
        flResult = 1.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_3:
        flResult = 2.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_4:
        flResult = 3.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_5:
        flResult = 4.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_6:
        flResult = 5.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_7:
        flResult = 6.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_HEALING:
        flResult = 7.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_WDCOST:
        flResult = 8.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_RUNE_DAMAGE_BONUS:
        flResult = 9.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_PRIMARY_STAT:
        flResult = 10.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_VITALITY:
        flResult = 11.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_ARMOR:
        flResult = 12.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_WEAPON_DPS:
        flResult = 13.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_LEGENDARY_PROC_DMG:
        flResult = 14.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_MIN_MIN:
        flResult = 15.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_MIN_MAX:
        flResult = 16.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_DELTA_MIN:
        flResult = 17.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_DELTA_MAX:
        flResult = 18.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_MIN_MIN:
        flResult = 19.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_MIN_MAX:
        flResult = 20.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_DELTA_MIN:
        flResult = 21.0f;
        break;
    case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_DELTA_MAX:
        flResult = 22.0f;
        break;
    case BBE_IDENTIFIER_MONSTER_HEALTH_MIN:
        flResult = sGetMonsterHealthValue(tCallbackData.nLevel, tCallbackData.idFastAttrib);
        break;
    case BBE_IDENTIFIER_POWER_TAG:
        if (nParam0 == -1)
        {
            flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, tCallbackData.snoPower, nParam1);
        }
        else
        {
            flResult = PowerTagEvaluateBBEIdentifier(tCallbackData.idFastAttrib, -1, nParam0, nParam1);
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;


}


static void sTokenizeIdentifier(
    LPCSTR szIdentifier,
    TokenizedIdentifier & tTokenizeIdentifier)
{
    CHAR szValue[256];

    if (szIdentifier[0] == '@' || szIdentifier[0] == '#')
    {
        Blizzard::String::Copy(szValue, szIdentifier + 1, sizeof(szValue));
        tTokenizeIdentifier.cPrefix = szIdentifier[0];
    }
    else
    {
        Blizzard::String::Copy(szValue, szIdentifier, sizeof(szValue));
        tTokenizeIdentifier.cPrefix = '\0';
    }

    tTokenizeIdentifier.nStringCount = 0;
    const char * csDelimeter = ".";


    LPSTR szToken = strtok(szValue, csDelimeter);
    while (szToken != NULL && tTokenizeIdentifier.nStringCount < MAX_TOKENS)
    {
        tTokenizeIdentifier.sString[tTokenizeIdentifier.nStringCount] = szToken;
        tTokenizeIdentifier.nStringCount++;
        szToken = strtok(NULL, csDelimeter);
    }
}

struct TBBETranslateTable
{
    CHAR szIdentifier[64];
    int32 nIdentifier;
};

static TBBETranslateTable sgtTranslateTable[112] = {
    { "sLevel", BBE_IDENTIFIER_SKILL_LEVEL },
    { "sLevelp", BBE_IDENTIFIER_SKILL_LEVEL_PREVIOUS },
    { "gLevel", BBE_IDENTIFIER_GIZMO_LEVEL },
    { "mLevel", BBE_IDENTIFIER_GIZMO_LEVEL },
    { "mAreaDamageMin", BBE_IDENTIFIER_AREA_DAMAGE_MIN_PHYSICAL },
    { "mAreaDamageDelta", BBE_IDENTIFIER_AREA_DAMAGE_DELTA_PHYSICAL },
    { "iLevel", BBE_IDENTIFIER_ITEM_LEVEL },
    { "mDamageMin#Physical", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_PHYSICAL },
    { "mDamageDelta#Physical", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_PHYSICAL },
    { "mDamageMin#Fire", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_FIRE },
    { "mDamageDelta#Fire", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_FIRE },
    { "mDamageMin#Lightning", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_LIGHTNING },
    { "mDamageDelta#Lightning", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_LIGHTNING },
    { "mDamageMin#Cold", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_COLD },
    { "mDamageDelta#Cold", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_COLD },
    { "mDamageMin#Poison", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_POISON },
    { "mDamageDelta#Poison", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_POISON },
    { "mDamageMin#Arcane", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_ARCANE },
    { "mDamageDelta#Arcane", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_ARCANE },
    { "mDamageMin#Holy", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_HOLY },
    { "mDamageDelta#Holy", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_HOLY },
    { "mDamageMin#Siege", BBE_IDENTIFIER_MONSTER_DAMAGE_MIN_SIEGE },
    { "mDamageDelta#Siege", BBE_IDENTIFIER_MONSTER_DAMAGE_DELTA_SIEGE },
    { "SF_0", BBE_IDENTIFIER_SCRIPT_FORMULA_0 },
    { "SF_1", BBE_IDENTIFIER_SCRIPT_FORMULA_1 },
    { "SF_2", BBE_IDENTIFIER_SCRIPT_FORMULA_2 },
    { "SF_3", BBE_IDENTIFIER_SCRIPT_FORMULA_3 },
    { "SF_4", BBE_IDENTIFIER_SCRIPT_FORMULA_4 },
    { "SF_5", BBE_IDENTIFIER_SCRIPT_FORMULA_5 },
    { "SF_6", BBE_IDENTIFIER_SCRIPT_FORMULA_6 },
    { "SF_7", BBE_IDENTIFIER_SCRIPT_FORMULA_7 },
    { "SF_8", BBE_IDENTIFIER_SCRIPT_FORMULA_8 },
    { "SF_9", BBE_IDENTIFIER_SCRIPT_FORMULA_9 },
    { "SF_10", BBE_IDENTIFIER_SCRIPT_FORMULA_10 },
    { "SF_11", BBE_IDENTIFIER_SCRIPT_FORMULA_11 },
    { "SF_12", BBE_IDENTIFIER_SCRIPT_FORMULA_12 },
    { "SF_13", BBE_IDENTIFIER_SCRIPT_FORMULA_13 },
    { "SF_14", BBE_IDENTIFIER_SCRIPT_FORMULA_14 },
    { "SF_15", BBE_IDENTIFIER_SCRIPT_FORMULA_15 },
    { "SF_16", BBE_IDENTIFIER_SCRIPT_FORMULA_16 },
    { "SF_17", BBE_IDENTIFIER_SCRIPT_FORMULA_17 },
    { "SF_18", BBE_IDENTIFIER_SCRIPT_FORMULA_18 },
    { "SF_19", BBE_IDENTIFIER_SCRIPT_FORMULA_19 },
    { "SF_20", BBE_IDENTIFIER_SCRIPT_FORMULA_20 },
    { "SF_21", BBE_IDENTIFIER_SCRIPT_FORMULA_21 },
    { "SF_22", BBE_IDENTIFIER_SCRIPT_FORMULA_22 },
    { "SF_23", BBE_IDENTIFIER_SCRIPT_FORMULA_23 },
    { "SF_24", BBE_IDENTIFIER_SCRIPT_FORMULA_24 },
    { "SF_25", BBE_IDENTIFIER_SCRIPT_FORMULA_25 },
    { "SF_26", BBE_IDENTIFIER_SCRIPT_FORMULA_26 },
    { "SF_27", BBE_IDENTIFIER_SCRIPT_FORMULA_27 },
    { "SF_28", BBE_IDENTIFIER_SCRIPT_FORMULA_28 },
    { "SF_29", BBE_IDENTIFIER_SCRIPT_FORMULA_29 },
    { "SF_30", BBE_IDENTIFIER_SCRIPT_FORMULA_30 },
    { "SF_31", BBE_IDENTIFIER_SCRIPT_FORMULA_31 },
    { "SF_32", BBE_IDENTIFIER_SCRIPT_FORMULA_32 },
    { "SF_33", BBE_IDENTIFIER_SCRIPT_FORMULA_33 },
    { "SF_34", BBE_IDENTIFIER_SCRIPT_FORMULA_34 },
    { "SF_35", BBE_IDENTIFIER_SCRIPT_FORMULA_35 },
    { "SF_36", BBE_IDENTIFIER_SCRIPT_FORMULA_36 },
    { "SF_37", BBE_IDENTIFIER_SCRIPT_FORMULA_37 },
    { "SF_38", BBE_IDENTIFIER_SCRIPT_FORMULA_38 },
    { "SF_39", BBE_IDENTIFIER_SCRIPT_FORMULA_39 },
    { "SF_40", BBE_IDENTIFIER_SCRIPT_FORMULA_40 },
    { "SF_41", BBE_IDENTIFIER_SCRIPT_FORMULA_41 },
    { "SF_42", BBE_IDENTIFIER_SCRIPT_FORMULA_42 },
    { "SF_43", BBE_IDENTIFIER_SCRIPT_FORMULA_43 },
    { "SF_44", BBE_IDENTIFIER_SCRIPT_FORMULA_44 },
    { "SF_45", BBE_IDENTIFIER_SCRIPT_FORMULA_45 },
    { "SF_46", BBE_IDENTIFIER_SCRIPT_FORMULA_46 },
    { "SF_47", BBE_IDENTIFIER_SCRIPT_FORMULA_47 },
    { "SF_48", BBE_IDENTIFIER_SCRIPT_FORMULA_48 },
    { "SF_49", BBE_IDENTIFIER_SCRIPT_FORMULA_49 },
    { "SF_50", BBE_IDENTIFIER_SCRIPT_FORMULA_50 },
    { "SF_51", BBE_IDENTIFIER_SCRIPT_FORMULA_51 },
    { "SF_52", BBE_IDENTIFIER_SCRIPT_FORMULA_52 },
    { "SF_53", BBE_IDENTIFIER_SCRIPT_FORMULA_53 },
    { "SF_54", BBE_IDENTIFIER_SCRIPT_FORMULA_54 },
    { "SF_55", BBE_IDENTIFIER_SCRIPT_FORMULA_55 },
    { "SF_56", BBE_IDENTIFIER_SCRIPT_FORMULA_56 },
    { "SF_57", BBE_IDENTIFIER_SCRIPT_FORMULA_57 },
    { "SF_58", BBE_IDENTIFIER_SCRIPT_FORMULA_58 },
    { "SF_59", BBE_IDENTIFIER_SCRIPT_FORMULA_59 },
    { "SF_60", BBE_IDENTIFIER_SCRIPT_FORMULA_60 },
    { "SF_61", BBE_IDENTIFIER_SCRIPT_FORMULA_61 },
    { "SF_62", BBE_IDENTIFIER_SCRIPT_FORMULA_62 },
    { "SF_63", BBE_IDENTIFIER_SCRIPT_FORMULA_63 },
    { "DmgTier1", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_1 },
    { "DmgTier2", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_2 },
    { "DmgTier3", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_3 },
    { "DmgTier4", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_4 },
    { "DmgTier5", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_5 },
    { "DmgTier6", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_6 },
    { "DmgTier7", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_7 },
    { "Healing", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_HEALING },
    { "WDCost", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_WDCOST },
    { "RuneDamageBonus", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_RUNE_DAMAGE_BONUS },
    { "mHealthMin", BBE_IDENTIFIER_MONSTER_HEALTH_MIN },
    { "PvPAvgPrimaryStat", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_PRIMARY_STAT },
    { "PvPAvgVitality", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_VITALITY },
    { "PvPAvgArmor", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_ARMOR },
    { "PvPAvgWeaponDPS", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_WEAPON_DPS },
    { "LegendaryProcDmg", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_LEGENDARY_PROC_DMG },
    { "OffhandDmgMinMin", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_MIN_MIN },
    { "OffhandDmgMinMax", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_MIN_MAX },
    { "OffhandDmgDeltaMin", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_DELTA_MIN },
    { "OffhandDmgDeltaMax", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_DELTA_MAX },
    { "ShieldBlockMinMin", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_MIN_MIN },
    { "ShieldBlockMinMax", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_MIN_MAX },
    { "ShieldBlockDeltaMin", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_DELTA_MIN },
    { "ShieldBlockDeltaMax", BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_DELTA_MAX },
    { "bTraitActive", BBE_IDENTIFIER_TRAIT_ACTIVE },
};



static const int32 sgnNumTranslateTableEntries = 112;
static const char * sgszPowerTagToken = "PowerTag";
static const char * sgszShortPowerTagToken = "P";

BOOL BBEIdentifierTranslate(
    const BBETranslationData & bbeTranslate,
    const char * szIdentifier,
    BOOL * pfConstant,
    BOOL * pfLong,
    uint8 * pbResult)
{


    IdentifierResult * pIdentiferResult = (IdentifierResult *)pbResult;  // pbResult is the byte buffer of an IdentifierResult (callee PowerTagBBEIdentifierCompile takes IdentifierResult *)
    SigmaMemoryClear(pIdentiferResult, sizeof(IdentifierResult));
    *pfConstant = FALSE;
    *pfLong = TRUE;

    for (int32 i = 0; i < sgnNumTranslateTableEntries; ++i)
    {
        if (strcasecmp(szIdentifier, sgtTranslateTable[i].szIdentifier) == 0)
        {
            pIdentiferResult->nIdentifier = sgtTranslateTable[i].nIdentifier;
            return TRUE;
        }
    }

    TokenizedIdentifier tTokenizedIdentifier;
    sTokenizeIdentifier(szIdentifier, tTokenizedIdentifier);

    if (tTokenizedIdentifier.nStringCount > 0 &&
        strcasecmp(tTokenizedIdentifier.sString[0].str(), sgszPowerTagToken) == 0)
    {
        PowerTagBBEIdentifierCompile(tTokenizedIdentifier, pIdentiferResult);
        return TRUE;
    }

    if (tTokenizedIdentifier.nStringCount > 0 &&
        strcasecmp(tTokenizedIdentifier.sString[0].str(), sgszShortPowerTagToken) == 0)
    {
        PowerTagBBEIdentifierCompile(tTokenizedIdentifier, pIdentiferResult);
        return TRUE;
    }



    return FastAttributeBBECompileAttrib(tTokenizedIdentifier, pIdentiferResult);
}

BOOL BBEIdentifierToString(
    const int32 nIdentifier,
    int32 nParam0,
    int32 nParam1,
    int32 nParam2,
    CRefString & sIdentifier)
{
    if (nIdentifier == BBE_IDENTIFIER_FAST_ATTRIB)
    {
        IdentifierFastAttribResult tResult;
        FastAttribGetIdentifierAttributeFromParams(tResult, nIdentifier, nParam0, nParam1, nParam2);
        sIdentifier = AttribToString(tResult.eAttrib);
        return TRUE;
    }


    for (int32 i = 0; i < sgnNumTranslateTableEntries; ++i)
    {
        if (sgtTranslateTable[i].nIdentifier == nIdentifier)
        {
            sIdentifier = sgtTranslateTable[i].szIdentifier;
            return TRUE;
        }
    }
    return FALSE;
}





int32 BBEPowerFormulaTableIndexFromString(
    const char * szIdentifier)
{
    for (int32 i = 0; i < sgnNumTranslateTableEntries; ++i)
    {
        if (strcasecmp(szIdentifier, sgtTranslateTable[i].szIdentifier) == 0)
        {
            switch (sgtTranslateTable[i].nIdentifier)
            {
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_1:
                return 0;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_2:
                return 1;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_3:
                return 2;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_4:
                return 3;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_5:
                return 4;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_6:
                return 5;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_DAMAGE_TIER_7:
                return 6;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_HEALING:
                return 7;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_WDCOST:
                return 8;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_RUNE_DAMAGE_BONUS:
                return 9;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_PRIMARY_STAT:
                return 10;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_VITALITY:
                return 11;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_ARMOR:
                return 12;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_PVP_AVERAGE_WEAPON_DPS:
                return 13;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_LEGENDARY_PROC_DMG:
                return 14;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_MIN_MIN:
                return 15;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_MIN_MAX:
                return 16;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_DELTA_MIN:
                return 17;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_OFFHAND_DMG_DELTA_MAX:
                return 18;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_MIN_MIN:
                return 19;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_MIN_MAX:
                return 20;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_DELTA_MIN:
                return 21;
            case BBE_IDENTIFIER_TABLE_FORMULA_INDEX_SHIELD_BLOCK_DELTA_MAX:
                return 22;
            }
        }
    }
    return -1;
}
