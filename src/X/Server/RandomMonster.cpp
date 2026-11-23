// Original file: X/Server/RandomMonster.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/ActorCommonData.h"
#include "../Common/GameBalance.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/MonsterDefinition.h"
#include "../Common/MonsterStruct.h"
#include "../Common/Monsters.h"
#include "../Common/BBE.h"
#include "../Common/BBEUtil.h"
#include "../Common/Target.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Attributes/FastAttribute.h"
#include "Actors/Actors.h"
#include "AI/ActorAI.h"
#include "Powers/Powers.h"
#include "ActorScript.h"
#include "SGame.h"
#include "../Common/Powers/PowersCommon.h"

// Declared here from the DWARF (X/Common/Definitions/GameBalanceDefinition.h:1523 and :1533); src's header lacks them.
static const int32 MAX_MONSTER_AFFIX_BONUSES = 10;
enum MonsterAffixSpawnPowerType {
    AFFIX_ON_SPAWN_MINION = 0,
    AFFIX_ON_SPAWN_CHAMPION = 1,
    AFFIX_ON_SPAWN_RARE = 2,
    NUM_AFFIX_ON_SPAWN_MINION = 3,
};
enum { GB_INVALID = -1, GB_MONSTER_AFFIXES = 18, GB_MONSTER_NAMES = 19 };  // X/Common/GameBalance.h (GameBalanceType; src keeps it local)
enum { AT_PREFIX = 0, AT_SUFFIX = 1, AT_IMMUNITY = 7, AT_RANDOM = 9 };  // X/Common/Definitions/GameBalanceDefinition.h:294 (AffixType)
#include "LuaScript/LuaScriptUtil.h"
// Declared here from their definitions elsewhere (no src header declares them yet).
TAffixSelectionPool & ActorAIGetAffixSelectionPool();  // X/Server/AI/ActorAI.cpp
void ActorAISetTempAffixList(AffixSelectionList * ptList, int32 * pnAffixListWeightTotal);  // X/Server/AI/ActorAI.cpp
lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:3353
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScript.cpp:1084













































static GBID sChooseRandomName(
	AffixType eAffixType,
	GBHandleList * ptlistAffixes)
{

	GBHandleList listNames(GBGetHandlePool());
	GBEnumerate(GB_MONSTER_NAMES, listNames);


	POS pos = listNames.GetTailPosition();
	while (pos)
	{
		POS posPrev = pos;
		const GBHandle & hGBName = listNames.GetPrev(pos);
		ScopedRareMonsterNameEntryLockR pName(hGBName.gbid);
		// the NULL test has no row of its own (its cbz carries the lock constructor's row)
		if (pName == NULL)
			continue;


		if (pName->eAffixType != eAffixType)
		{
			listNames.RemoveAt(posPrev);
		} else if (strlen(pName->szAffixEffect) > 0 &&

			Blizzard::String::CompareI(pName->szAffixEffect, "Generic") != 0)
		{
			BOOL bFound = FALSE;
			GBID gbidAffix = StringToGBID(pName->szAffixEffect);
			POS posAffix = ptlistAffixes->GetHeadPosition();
			while (posAffix)
			{
				GBHandle & hGBAffix = ptlistAffixes->GetNext(posAffix);
				if (hGBAffix.gbid == gbidAffix)
				{
					bFound = TRUE;
					break;
				}
			}

			if (!bFound)
			{
				listNames.RemoveAt(posPrev);
			}
		}

	}


	int32 nNameCount = listNames.Count();
	if (nNameCount == 0)
		return GBID_NONE;



	int32 nCount = RandRange_Internal(SGameGlobalsGet().rng, 0, nNameCount - 1) + 1;

	GBID gbidRetval = GBID_NONE;

	pos = listNames.GetHeadPosition();
	for (int32 i = 0; i < nCount; i++)
	{
		gbidRetval = listNames.GetNext(pos).gbid;
	}

	ScopedRareMonsterNameEntryLockR pName(gbidRetval);

	if (pName && strlen(pName->szAffixEffect) > 0)
	{
		GBID gbidAffix = StringToGBID(pName->szAffixEffect);
		// GetHeadPosition followed by GetPrev (call lines 156 and 160): only the head is ever visited
		POS posAffix = ptlistAffixes->GetHeadPosition();
		while (posAffix)
		{
			POS oldPos = posAffix;
			const GBHandle & hGBAffix = ptlistAffixes->GetPrev(posAffix);
			if (hGBAffix.gbid == gbidAffix)
			{
				ptlistAffixes->RemoveAt(oldPos);
			}
		}
	}

	return gbidRetval;
}





static GBID sChooseChampionName(
	AffixType eAffixType,
	GBHandleList * ptlistAffixes)
{
	POS pos = ptlistAffixes->GetHeadPosition();
	while (pos)
	{
		const GBHandle & hGB = ptlistAffixes->GetNext(pos);
		const GameBalanceDefinition * pdefGB;
		const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
		if (!ptAffix)
			continue;




		AffixType eAffixTypeInTable = ptAffix->eAffixType;
		GBUnlockR(pdefGB);

		if (eAffixTypeInTable == eAffixType)
		{
			return hGB.gbid;
		}
	}

	return GBID_NONE;
}




static void sMonsterNamesSet(
	ActorID idMonster,
	MonsterRarity eMonsterRarity,
	const GBHandleList * ptlistAffixes)
{


	ACDID idACD = ActorGet(idMonster)->idACD;
	ActorCommonData & tACD = ACDGet(idACD);

	GBHandleList tListNameAffixes(GBGetHandlePool());
	POS pos = ptlistAffixes->GetHeadPosition();


    while (pos)
	{
		GBHandle hGB = ptlistAffixes->GetNext(pos);
		tListNameAffixes.AddHead_Tracked(hGB);
	}

    switch (eMonsterRarity)
	{
	case MR_RARE:
        tACD.pRareNameGBIDs[0] = sChooseRandomName(AT_PREFIX, &tListNameAffixes);
        tACD.pRareNameGBIDs[1] = sChooseRandomName(AT_SUFFIX, &tListNameAffixes);
		break;
	// case order assumed: rare's code is laid out first, and its lines 229-230 precede 234-235
	case MR_CHAMPION:
        tACD.pRareNameGBIDs[0] = sChooseChampionName(AT_PREFIX, &tListNameAffixes);
        tACD.pRareNameGBIDs[1] = sChooseChampionName(AT_SUFFIX, &tListNameAffixes);
		break;
	}



}




enum AffixTypeToApply
{
	APPLY_PREFIX,
	APPLY_SUFFIX,
	APPLY_RANDOM,
	APPLY_IMMUNITY,
};

struct MonsterAffixParams
{
	int32 nLevel;
	BOOL bShooter;
	MonsterRarity eRarity;
	AffixTypeToApply nAffixType;
};

static BOOL sMonsterAffixIsValid(
	const ActorCommonData & tACD,
	const MonsterDefinition * ptMonster,
	const GBHandle & hGB,
	const MonsterAffixesEntry * ptAffix,
	const MonsterAffixParams & tAffixParams,
	const AffixSelectionList & listAffixes)
{
	if (ptAffix->nFrequencyWeight <= 0)
		return FALSE;





	for (int32 i = 0; i < MAX_DISALLOWED_MONSTER_AFFIXES; i++)
	{
        if (hGB.gbid == ptMonster->gbidDisallowedAffixes[i])
			return FALSE;
	}



	DamageType eACDDamageType = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_BASE_ELEMENT));
    if (eACDDamageType != DAMAGE_TYPE_NONE)
	{

        if (ptAffix->eBaseElement != DAMAGE_TYPE_NONE && ptAffix->eBaseElement != eACDDamageType)
			return FALSE;
	}




	switch (tAffixParams.nAffixType)
	{
	case APPLY_PREFIX:
		if (ptAffix->eAffixType != AT_PREFIX)
		{
			return FALSE;
		}
		break;

	case APPLY_SUFFIX:
		if (ptAffix->eAffixType != AT_SUFFIX)
		{
			return FALSE;
		}
		break;

	case APPLY_RANDOM:
        if (ptAffix->eAffixType != AT_RANDOM)
		{
			return FALSE;
		}
		break;

	case APPLY_IMMUNITY:
		if (ptAffix->eAffixType != AT_IMMUNITY)
		{
			return FALSE;
		}
		break;

	default:
		DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
	}



    if (ptAffix->nAffixLevelMin > -1 &&
        ptAffix->nAffixLevelMax > -1)
	{
        if (tAffixParams.nLevel < ptAffix->nAffixLevelMin || tAffixParams.nLevel > ptAffix->nAffixLevelMax)
			return FALSE;
	}
	else
	{
		// the clamp has no rows of its own (hoisted above the loop); the inline tree shows no Math:: helper


		int32 nLevel = tAffixParams.nLevel < MONSTER_LEVEL_MAX ? tAffixParams.nLevel : MONSTER_LEVEL_MAX;
		int32 nLevelIdx = (nLevel > 1 ? nLevel : 1) - 1;


        if (ptAffix->arAllowedByLevel[nLevelIdx] <= 0)
			return FALSE;
	}



	if (ptAffix->eValidFor == VALID_FOR_RARES && tAffixParams.eRarity != MR_RARE)
		return FALSE;




    if (ptAffix->eValidFor == VALID_FOR_CHAMPION && tAffixParams.eRarity != MR_CHAMPION)
		return FALSE;




    if (ptAffix->eValidFor == VALID_FOR_SHOOTERS && !tAffixParams.bShooter)
		return FALSE;




    if (!ptAffix->bAvailableForUniques &&
		(tAffixParams.eRarity == MR_UNIQUE || tAffixParams.eRarity == MR_BOSS))
		return FALSE;




	POS pos = listAffixes.GetHeadPosition();
    while (pos)
	{
		const AffixSelectionEntry & tEntry = listAffixes.GetNext(pos);
        if (tEntry.gbidAffix == hGB.gbid)
			return FALSE;
	}






	return TRUE;
}

static int32 sMonsterGetValidAffixes(
	ActorCommonData & tACD,
	const GBHandleList & listAffixesPreexisting,
	const GBHandleList & listAffixesAll,
	AffixSelectionList & listAffixesValid,
	const MonsterAffixParams & tAffixParams)
{
	ScopedActorDefinitionLockR pdefActor(tACD.tSNOName.snoHandle);
	ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);

	int32 nTotalWeight = 0;
	POS pos = listAffixesAll.GetHeadPosition();

	while (pos)
	{
		const GBHandle & hGB = listAffixesAll.GetNext(pos);

		BOOL bFoundExisting = FALSE;
		POS posExisting = listAffixesPreexisting.GetHeadPosition();
        while (posExisting)
		{
			const GBHandle & hGBExisting = listAffixesPreexisting.GetNext(posExisting);
			if (hGBExisting == hGB)
			{
				bFoundExisting = TRUE;
				break;
			}
		}

		if (bFoundExisting)
			continue;



		const GameBalanceDefinition * pdefGB;
		const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
		if (!ptAffix)
			continue;



		if (sMonsterAffixIsValid(tACD, pdefMonster, hGB, ptAffix, tAffixParams, listAffixesValid))
		{
			AffixSelectionEntry tEntry;
			tEntry.gbidAffix = hGB.gbid;
			tEntry.nFrequencyWeight = ptAffix->nFrequencyWeight;
			listAffixesValid.AddTail_Tracked(tEntry);
			nTotalWeight += tEntry.nFrequencyWeight;
		}

		GBUnlockR(pdefGB);
	}

	return nTotalWeight;
}




static void sMonsterRemoveAllAffixesWithoutElement(
	AffixSelectionList & listAffixesValid,
	DamageType eElement,
	int32 & nTotalWeight)
{
	POS pos = listAffixesValid.GetHeadPosition();
    while (pos)
	{
		POS posCur = pos;
		const AffixSelectionEntry & tEntry = listAffixesValid.GetNext(pos);
		GBHandle hGB;
		hGB.Set(GB_MONSTER_AFFIXES, tEntry.gbidAffix);
		const GameBalanceDefinition * pdefGB;
		const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
		if (!ptAffix)
			continue;



		if (ptAffix->eBaseElement != DAMAGE_TYPE_NONE && ptAffix->eBaseElement != eElement)
		{

			nTotalWeight -= tEntry.nFrequencyWeight;
			listAffixesValid.RemoveAt(posCur);
		}

		GBUnlockR(pdefGB);
	}
}




static void sMonsterRemoveAllDefensiveAffixes(
	AffixSelectionList & listAffixesValid,
	int32 & nTotalWeight,
	BOOL bDefensive,
	BOOL bCC)
{
	POS pos = listAffixesValid.GetHeadPosition();
    while (pos)
	{
		POS posCur = pos;
		const AffixSelectionEntry & tEntry = listAffixesValid.GetNext(pos);
		GBHandle hGB;
		hGB.Set(GB_MONSTER_AFFIXES, tEntry.gbidAffix);
		const GameBalanceDefinition * pdefGB;
		const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
		if (!ptAffix)
			continue;



		if ((bDefensive && ptAffix->bDefensive) ||
			(bCC && ptAffix->bCrowdControl))
		{
			nTotalWeight -= tEntry.nFrequencyWeight;
			listAffixesValid.RemoveAt(posCur);
		}

		GBUnlockR(pdefGB);
	}
}




static GBHandle sMonsterPickAndRemoveRandomAffix(
	ActorCommonData & tACD,
	AffixSelectionList & listAffixesValid,
	int32 & nTotalWeight)
{
    SNO snoScript = GlobalSNOGet(GLOBALSNO_SCRIPT_MONSTERAFFIXCHOOSE);
    lua_State * ptLuaState = LuaScriptGetLuaStateWithSNO(snoScript);
    LPCSTR szFuncName = "OnAffixPicked";
    BOOL bHasFunction = LuaScriptGetFunction(ptLuaState, szFuncName);

    RNG & tRNG = SGameGlobalsGet().rng;
    int32 nChosenWeight = nTotalWeight > 1 ? RandRange_Internal(tRNG, 0, nTotalWeight - 1) : 1;
	int32 nCurWeight = 0;
	POS pos = listAffixesValid.GetHeadPosition();
	while (pos)
	{
		POS posCur = pos;
		const AffixSelectionEntry & tEntry = listAffixesValid.GetNext(pos);
		GBHandle hGB;
		hGB.Set(GB_MONSTER_AFFIXES, tEntry.gbidAffix);
		const GameBalanceDefinition * pdefGB;
		const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
		if (!ptAffix)
			continue;



		DamageType eChosenElementType = ptAffix->eBaseElement;
		BOOL bDefensive = ptAffix->bDefensive;
		BOOL bCC = ptAffix->bCrowdControl;

		GBUnlockR(pdefGB);

		nCurWeight += tEntry.nFrequencyWeight;

		if (nChosenWeight < nCurWeight)
		{
			nTotalWeight -= tEntry.nFrequencyWeight;
			listAffixesValid.RemoveAt(posCur);

            if (eChosenElementType != DAMAGE_TYPE_NONE)
			{
				sMonsterRemoveAllAffixesWithoutElement(listAffixesValid, eChosenElementType, nTotalWeight);

                if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_BASE_ELEMENT)) == DAMAGE_TYPE_NONE)
				{
                    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_BASE_ELEMENT), eChosenElementType);
				}
			}

            if (bDefensive || bCC)
			{
                sMonsterRemoveAllDefensiveAffixes(listAffixesValid, nTotalWeight, bDefensive, bCC);
			}

            if (bHasFunction)
			{
                ActorAISetTempAffixList(&listAffixesValid, &nTotalWeight);
                lua_pushinteger(ptLuaState, tACD.id);
                lua_pushinteger(ptLuaState, hGB.gbid);
                LuaSNOScriptCall(snoScript, ptLuaState, 2, 0, szFuncName);
                ActorAISetTempAffixList(NULL, NULL);
			}

			return hGB;
		}
	}

	DamageType eElement = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_BASE_ELEMENT));












	GBHandle hGB(GB_INVALID, GBID_NONE);
	return hGB;


}



static void sMonsterAffixesPick(
	ActorCommonData & tACD,
	const SNOName & tActorSNOName,
	int32 nNumToApply,
	int32 nLevel,
	MonsterRarity eRarity,
	AffixTypeToApply nAffixType,
	GBHandleList & listAffixes)
{
	if (nNumToApply == 0)
		return;



	EncounterCategory eCategory = MonsterGetMonsterCategory(tActorSNOName);

	MonsterAffixParams tAffixParams;
	tAffixParams.nLevel = nLevel;
	tAffixParams.bShooter = (eCategory == MC_RANGED);
	tAffixParams.eRarity = eRarity;
	tAffixParams.nAffixType = nAffixType;

	GBHandleList listAffixesAll(GBGetHandlePool());
	GBEnumerate(GB_MONSTER_AFFIXES, listAffixesAll);

	AffixSelectionList listAffixesValid(&ActorAIGetAffixSelectionPool());

	int32 nTotalWeight = sMonsterGetValidAffixes(tACD, listAffixes, listAffixesAll, listAffixesValid, tAffixParams);

	int32 nCount = listAffixesValid.Count();
	int32 nNumToUse = nNumToApply;

	if (nCount < nNumToUse)
		nNumToUse = nCount;




    if (nNumToUse <= 1)
	{
        sMonsterRemoveAllDefensiveAffixes(listAffixesValid, nTotalWeight, TRUE, FALSE);
	}

	for (int32 i = 0; i < nNumToUse; i++)
	{
		GBHandle hGB = sMonsterPickAndRemoveRandomAffix(tACD, listAffixesValid, nTotalWeight);
		if (hGB.gbid != GBID_NONE)
		{
			listAffixes.AddTail_Tracked(hGB);
		}
	}
}



int32 MonsterGetNumberOfRandomAffixes(
    int32 nLevel,
    int32 nFixedAffixes)
{
    int32 nRandomAffixes = 1;
    const int32 NUM_AFFIX_WEIGHTS = 4;
    float flAffixWeights[NUM_AFFIX_WEIGHTS];
    flAffixWeights[0] = GetMonsterLevelAttribute(nLevel, SNO_NONE, GBA_MONSTER_1_AFFIX_WEIGHT);
    flAffixWeights[1] = GetMonsterLevelAttribute(nLevel, SNO_NONE, GBA_MONSTER_2_AFFIX_WEIGHT);
    flAffixWeights[2] = GetMonsterLevelAttribute(nLevel, SNO_NONE, GBA_MONSTER_3_AFFIX_WEIGHT);
    flAffixWeights[3] = GetMonsterLevelAttribute(nLevel, SNO_NONE, GBA_MONSTER_4_AFFIX_WEIGHT);
    // the shift has no line rows of its own (unrolled into line 676); one slot per fixed affix
    for (int32 x = 0; x < nFixedAffixes; x++)
    {
        flAffixWeights[0] = flAffixWeights[1];
        flAffixWeights[1] = flAffixWeights[2];
        flAffixWeights[2] = flAffixWeights[3];
        flAffixWeights[3] = 0.0f;
    }




    float flWeightTotal = 0.0f;
    for (int32 i = 0; i < NUM_AFFIX_WEIGHTS; i++)
    {
        flWeightTotal += flAffixWeights[i];
    }

    if (flWeightTotal > 0.0f)
    {
        float flRandomValue = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, flWeightTotal);  // every row of line 695 is at column 31: a macro in the original
        float flCurrentWeight = 0.0f;
        for (int32 i = 0; i < NUM_AFFIX_WEIGHTS; i++)
        {
            flCurrentWeight += flAffixWeights[i];
            if (flRandomValue < flCurrentWeight)
            {
                nRandomAffixes = i + 1;
                break;
            }
        }
    }

    return nRandomAffixes;
}



static void sMonsterAffixesGenerateRandom(
	ActorCommonData & tACD,
	const SNOName & tActorSNOName,
	MonsterRarity eMonsterRarity,
	int32 nLevel,
	GBHandleList & listAffixes,
	int32 nSpawnerAdditionalAffixes)
{
    if (eMonsterRarity != MR_CHAMPION && eMonsterRarity != MR_RARE)
    {


        ScopedActorDefinitionLockR pdefActor(tActorSNOName.snoHandle);

        ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
        if (pdefMonster->nRandomAffixes >= 1 || nSpawnerAdditionalAffixes > 0)
        {
            int32 nMaxRandomAffixes = Math::Max(8 - listAffixes.Count() - nSpawnerAdditionalAffixes, 0);  // 8: GBID pMonsterAffixGBIDs[8] of ActorAddStruct/ActorCommonData; original spelling unknown
            sMonsterAffixesPick(tACD, tActorSNOName, Math::Pin(pdefMonster->nRandomAffixes + nSpawnerAdditionalAffixes, 1, nMaxRandomAffixes), nLevel, eMonsterRarity, APPLY_RANDOM, listAffixes);
        }

        return;
    }


    int32 nFixedAffixes = 0;
    POS pos = listAffixes.GetHeadPosition();
    while (pos)
    {
        const GBHandle & hGB = listAffixes.GetNext(pos);
        const GameBalanceDefinition * pdefGB;
        const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry*)GBLockR(hGB, pdefGB, TRUE);
        if (!ptAffix)
            continue;



        if (ptAffix->bShown)
            nFixedAffixes++;



        GBUnlockR(pdefGB);
    }
    int32 nRarityAffixes = MonsterGetNumberOfRandomAffixes(nLevel, nFixedAffixes);
    // nRarityImmunities/nAffixesToApply/nImmunitiesToApply have no location. Inferred: an APPLY_IMMUNITY pick that folds away
    int32 nRarityImmunities = 0;
    // only after inlining (the original keeps sMonsterAffixesPick's nAffixType parameter, so IPSCCP saw a second value)
    int32 nAffixesToApply = nRarityAffixes - nRarityImmunities;
    int32 nImmunitiesToApply = Math::Max(nRarityImmunities, 0);

	sMonsterAffixesPick(tACD, tActorSNOName, nAffixesToApply, nLevel, eMonsterRarity, APPLY_RANDOM, listAffixes);
	if (nImmunitiesToApply > 0)
		sMonsterAffixesPick(tACD, tActorSNOName, nImmunitiesToApply, nLevel, eMonsterRarity, APPLY_IMMUNITY, listAffixes);
}



static void sMonsterCopyMinionAffixesFromRare(
	const GBHandleList & listRare,
	GBHandleList & listMinion)
{
	POS pos = listRare.GetHeadPosition();

	while (pos)
	{
		GBHandle hGB = listRare.GetNext(pos);

		const GameBalanceDefinition * pdefGB;
		const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
		if (!ptAffix)
			continue;




		if (ptAffix->ptMinionAttributes[0].eAttribute != ATTRIBUTE_NONE ||
			ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_MINION] != SNO_NONE)
		{
			listMinion.AddTail_Tracked(hGB);
		}

		GBUnlockR(pdefGB);
	}
}




static void sMonsterGetAffixesFromMonsterSpreadsheet(
	const SNOName & tSNOName,
	GBHandleList & listAffixes)
{

	ScopedActorDefinitionLockR pdefActor(tSNOName.snoHandle);


	const MonsterDefinition * ptDefMonster = (const MonsterDefinition *)gptGroupDefMonster->LockR(pdefActor->snoMonster, FALSE);

	GBHandle hGBAffix;
	hGBAffix.eType = GB_MONSTER_AFFIXES;

	for (int32 i = 0; i < MAX_MONSTER_AFFIXES; i++)
	{
		hGBAffix.gbid = ptDefMonster->gbidAffixes[i];

		if (hGBAffix.gbid == GBID_NONE)
			break;



		if (GBIDExists(hGBAffix))
		{




			listAffixes.AddTail_Tracked(hGBAffix);
		}
	}
    gptGroupDefMonster->UnlockR((void *&)ptDefMonster);
}




static void sMonsterGetAffixesFromRarity(
	MonsterRarity eMonsterRarity,
	GBHandleList & listAffixes,
	int32,
	const BOOL)
{
	GBHandle hGB;
	hGB.eType = GB_MONSTER_AFFIXES;
	// one GlobalGBIDGet call through a table (0x7100e58ae0: 80,82,81,83,-,-,83); case grouping and order assumed
	switch (eMonsterRarity)
	{
	case MR_CHAMPION:
		hGB.gbid = GlobalGBIDGet(GLOBALGBID_MONSTER_AFFIX_CHAMPION_BASE);
		break;

	case MR_RARE:
		hGB.gbid = GlobalGBIDGet(GLOBALGBID_MONSTER_AFFIX_RARE);
		break;

	case MR_RARE_MINION:
		hGB.gbid = GlobalGBIDGet(GLOBALGBID_MONSTER_AFFIX_MINION);
		break;

	case MR_UNIQUE:
	case MR_BOSS:
		hGB.gbid = GlobalGBIDGet(GLOBALGBID_MONSTER_AFFIX_UNIQUE);
		break;

	default:
		return;
	}

	listAffixes.AddTail_Tracked(hGB);
}




static void sMonsterAddAffixName(
	ActorCommonData & tACD,
	GBID gbidMonsterAffix)
{
	// the search loop is unrolled into line 885; its exit test and the full-array return have no rows
	int32 nAffixCount = 0;
	for (; nAffixCount < 8; nAffixCount++)  // ActorCommonData::pMonsterAffixGBIDs is GBID[8]; no named constant in the DWARF
	{
		if (tACD.pMonsterAffixGBIDs[nAffixCount] == GBID_NONE)
			break;
	}
	if (nAffixCount >= 8)
		return;








	tACD.pMonsterAffixGBIDs[nAffixCount] = gbidMonsterAffix;
	tACD.ModifiedMark(ACDMODIFIED_RARE_MONSTER_NAME);
}


void sHandleAffixOnSpawnPower(
	ActorCommonData & tACD,
	MonsterRarity eMonsterRarity,
	const MonsterAffixesEntry * ptAffix)
{
    SNO snoPower = SNO_NONE;
    // the cases are one table lookup (0x7100e58aa0: offsets 0x22c,0x228,0x22c,0x224,0x22c,-,-,0x22c); case order assumed
    switch (eMonsterRarity)
    {
    case MR_NORMAL:
        snoPower = ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_RARE];
        break;
    case MR_CHAMPION:
        snoPower = ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_CHAMPION];
        break;
    case MR_RARE:
        snoPower = ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_RARE];
        break;
    case MR_RARE_MINION:
        snoPower = ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_MINION];
        break;
    case MR_UNIQUE:
        snoPower = ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_RARE];
        break;
    case MR_BOSS:
        snoPower = ptAffix->snoSpawnPower[AFFIX_ON_SPAWN_RARE];
        break;
    default:
        break;
    }

    if (snoPower != SNO_NONE)
    {
        Target tTarget;
        tTarget.InitializeFromPlace(tACD.GetPlace());
        PowerStart(tACD.id, snoPower, tTarget);
    }
}




void MonsterAffixesApply(
	ActorCommonData & tACD,
	MonsterRarity eMonsterRarity,
	const GBHandle & hGB)
{
	const GameBalanceDefinition * pdefGB;
	const MonsterAffixesEntry * ptAffix = (const MonsterAffixesEntry *)GBLockR(hGB, pdefGB, TRUE);
	if (!ptAffix)
		return;



	AttribGroupID idFastAttribGroupLight = FastAttribLightCreate(0);

	for (int32 i = 0; i < MAX_MONSTER_AFFIX_BONUSES; i++)
	{
		const AttributeSpecifier * ptAttribSpec;

		if (eMonsterRarity == MR_RARE_MINION)
			ptAttribSpec = &ptAffix->ptMinionAttributes[i];
		else
			ptAttribSpec = &ptAffix->ptAttributes[i];





		if (ptAttribSpec->eAttribute == ATTRIBUTE_NONE)
			break;



		BBECallbackData tCallback;
		InitBBECallbackData(&tCallback);
		tCallback.nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
		tCallback.idFastAttrib = tACD.idFastAttrib;

		float flRetval;
		BOOL bSuccess = BBEEvaluate(
			ptAttribSpec->pbAttributeFormula,
			ptAttribSpec->serAttributeFormula.dwSize,
			BBEIdentifierEvaluate,
			BBEFunctions,
			NUM_BBEFUNCTIONS,
			NULL,
			&tCallback,
			flRetval);






		FastAttribKey keyAttribute = MakeAttribKey(ptAttribSpec->eAttribute, ptAttribSpec->nParam);
		AttributeDataType eAttribType = KeyGetDataType(keyAttribute);

		if (eAttribType == ATTRIBUTEDT_FLOAT)
		{
			FastAttribSetValueFloat(idFastAttribGroupLight, keyAttribute, flRetval);
		}
		else
		{
			FastAttribSetValueInt(idFastAttribGroupLight, keyAttribute, Math::FloatToInt(flRetval));
		}
	}

    if (ptAffix->eBaseElement != DAMAGE_TYPE_NONE)
    {
        FastAttribSetValueInt(idFastAttribGroupLight, FastAttribKey(ATTRIBUTE_BASE_ELEMENT), ptAffix->eBaseElement);
    }

	tACD.AttributesMergeBaseAttributeGroup(idFastAttribGroupLight);
    FastAttribGroupDelete(idFastAttribGroupLight);

	sMonsterAddAffixName(tACD, hGB.gbid);

	sHandleAffixOnSpawnPower(tACD, eMonsterRarity, ptAffix);

	GBUnlockR(pdefGB);
}




static void sMonsterAffixesApplyList(
	ActorCommonData & tACD,
	MonsterRarity eMonsterRarity,
	const GBHandleList & listAffixes)
{
	POS pos = listAffixes.GetHeadPosition();
	while (pos)
	{
		const GBHandle & hGB = listAffixes.GetNext(pos);
		MonsterAffixesApply(tACD, eMonsterRarity, hGB);
	}
}




static void sGBHandleListCopy(
	const GBHandleList & listSource,
	GBHandleList & listDest)
{
	listDest.RemoveAll();

	POS pos = listSource.GetHeadPosition();

	while (pos)
	{
		listDest.AddTail_Tracked(listSource.GetNext(pos));
	}
}




static void sMonsterAffixesGenerate(
	ActorID idActor,
	const ActorAddStruct & tActorAdd,
	EncounterAddStruct & tEncounterAdd,
	GBHandleList & listAffixes)
{
	const Actor *ptActor = ActorGet(idActor);
	ACDID idACD = ptActor->idACD;

	if (tActorAdd.eMonsterRarity == MR_CHAMPION &&
		tEncounterAdd.eGroupAffixType == MR_CHAMPION)
	{

		sGBHandleListCopy(*tEncounterAdd.plistGroupAffixes, listAffixes);
		return;
	}

	BOOL fHasManualAffixes = FALSE;
	GBHandle hGB;
	hGB.eType = GB_MONSTER_AFFIXES;

	// no row of its own: this rarity test is merged into line 1071's
	if (tActorAdd.eMonsterRarity == MR_RARE_MINION)
	{




		sMonsterCopyMinionAffixesFromRare(*tEncounterAdd.plistGroupAffixes, listAffixes);
	}
	else
	{
		int32 nTotalFixedAffixes = 0;
		for (int32 i = 0; i < 8; i++)  // ActorAddStruct::pMonsterSpawnerAffixGBIDs is GBID[8]
		{
			if (tActorAdd.pMonsterSpawnerAffixGBIDs[i] != GBID_NONE)
			{
				hGB.gbid = tActorAdd.pMonsterSpawnerAffixGBIDs[i];
				listAffixes.AddTail_Tracked(hGB);
				nTotalFixedAffixes++;
			}
		}


		for (int32 i = 0; i < 8; i++)  // ActorAddStruct::pMonsterAffixGBIDs is GBID[8]
		{
			if (tActorAdd.pMonsterAffixGBIDs[i] != GBID_NONE)
			{
				hGB.gbid = tActorAdd.pMonsterAffixGBIDs[i];
				listAffixes.AddTail_Tracked(hGB);
				fHasManualAffixes = TRUE;
			}
		}
	}










	sMonsterGetAffixesFromMonsterSpreadsheet(tActorAdd.tSNOName, listAffixes);

	ActorCommonData & tACD = ACDGet(idACD);
	int32 nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
	// the unnamed int32 / const BOOL parameters leave no code: nLevel and fHasManualAffixes inferred
	sMonsterGetAffixesFromRarity(tActorAdd.eMonsterRarity, listAffixes, nLevel, fHasManualAffixes);

	if (!fHasManualAffixes)
	{
		sMonsterAffixesGenerateRandom(
			tACD,
			tActorAdd.tSNOName,
			tActorAdd.eMonsterRarity,
			nLevel,
			listAffixes,
			tActorAdd.nSpawnerAdditionalAffixes);
	}

	if (tActorAdd.eMonsterRarity == MR_RARE)
	{






        tEncounterAdd.plistGroupAffixes->RemoveAll();
        ScopedActorDefinitionLockR pdefActor(tACD.tSNOName.snoHandle);
        ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
        POS pos = listAffixes.GetHeadPosition();
        while (pos)
        {

            GBHandle hGBAffix = listAffixes.GetNext(pos);
            BOOL bCopy = TRUE;
            for (int32 i = 0; i < MAX_MONSTER_AFFIXES; i++)
            {
                GBID gbidInherentAffix = pdefMonster->gbidAffixes[i];
                if (gbidInherentAffix != GBID_NONE && gbidInherentAffix == hGBAffix.gbid)
                {
                    bCopy = FALSE;
                    break;
                }
            }


            if (bCopy)

                tEncounterAdd.plistGroupAffixes->AddTail_Tracked(hGBAffix);
        }


		tEncounterAdd.eGroupAffixType = MR_RARE;
	}

	if (tActorAdd.eMonsterRarity == MR_CHAMPION)
	{






		sGBHandleListCopy(listAffixes, *tEncounterAdd.plistGroupAffixes);
		tEncounterAdd.eGroupAffixType = MR_CHAMPION;
	}




}
void MonsterAddAffixes(
	ActorID idActor,
	const ActorAddStruct & tActorAdd,
	EncounterAddStruct & tEncounterAdd)
{
	ACDID idACD = ACDFindByActor(idActor);

	GBHandleList listAffixes(GBGetHandlePool());

	if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_IS_FROM_INACTIVE))
	{
		ActorCommonData & tACD = ACDGet(idACD);


		SigmaMemoryCopy(tACD.pRareNameGBIDs, tActorAdd.pRareNameGBIDs, sizeof(tACD.pRareNameGBIDs));

		GBHandle hGBAffix;
		hGBAffix.eType = GB_MONSTER_AFFIXES;


		for (int32 i = 0; i < 8; i++)  // ActorAddStruct::pMonsterAffixGBIDs is GBID[8]
		{
			if (tActorAdd.pMonsterAffixGBIDs[i] == GBID_NONE)
				break;

			hGBAffix.gbid = tActorAdd.pMonsterAffixGBIDs[i];



			listAffixes.AddTail_Tracked(hGBAffix);
		}
	}
	else
	{
		sMonsterAffixesGenerate(idActor, tActorAdd, tEncounterAdd, listAffixes);





		sMonsterNamesSet(
			idActor,
			tActorAdd.eMonsterRarity,
			&listAffixes);


		if (tActorAdd.dwActorAddFlags & (1 << ACTORADD_OVERRIDE_RARE_NAME))
		{
			ActorCommonData & tACD = ACDGet(idACD);
			tACD.pRareNameGBIDs[0] = tActorAdd.pRareNameGBIDs[0];
			tACD.pRareNameGBIDs[1] = tActorAdd.pRareNameGBIDs[1];
		}
	}

	ActorCommonData & tACD = ACDGet(idACD);
	sMonsterAffixesApplyList(
		tACD,
		tActorAdd.eMonsterRarity,
		listAffixes);
}



void MonsterDoOnCreatePowers(
    ACDID idACD)
{
    const ActorCommonData & tACD = ACDGet(idACD);



    for (int32 i = 0; i < MAX_MONSTER_SKILLS; i++)
    {
        const SkillDeclaration tSkill = MonsterGetSkill(tACD.tSNOName, i);

        if (tSkill.snoPower == SNO_NONE)
            break;



        if (PowerGetTemplate(AutoPowerDefinitionR(tSkill.snoPower), idACD) == ONCREATE_POWER)
        {
            PowerStart(idACD, tSkill.snoPower, cgTargetNone);
        }
    }
}









