// Original file: X/Server/System/LevelArea.cpp
#include "./LevelArea.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SWorld.h"
#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Common/World.h"
#include "../../Common/ActorCommonData.h"
#include <strings.h>

void SGameInitializeDungeonFinderPortalLevelAreaList();  // X/Server/SGame.cpp:8391
int32 SWorldGetRandSeedFromHash(SWorldID idSWorld, const int32 nHash);  // X/Server/SWorld.cpp:2069
float SWorldGetDungeonFinderEliteSpawnGroupDensityMultiplier(SNO snoWorld);  // X/Server/SWorld.cpp:2587
LPCSTR SWorldGetDungeonFinderPopulation(SNO snoWorld);  // X/Server/SWorld.cpp:2622
BOOL SQuestsActorIsRequiredByBounty(SNO snoActor);  // X/Server/Quests/SQuests.cpp:5410
BOOL SQuestsLevelAreaIsRequiredByKillBounty(SNO snoLevelArea);  // X/Server/Quests/SQuests.cpp:5544
struct LooteeInfo; struct Player;
BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo);  // ConditionDefinition.cpp:640

struct LevelAreaGlobals {  // size 0x8
    TDataArray<LevelArea, int> * ptLevelAreas;  // 0x00
};





















//@ 52
static const int32 MAX_LEVEL_AREAS = 320;




//@ 58
void LevelAreasInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptLevelAreaGlobals = (LevelAreaGlobals *)SigmaMemoryNew(sizeof(LevelAreaGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    tSGame.ptLevelAreaGlobals->ptLevelAreas = TDataArrayCreateTracked<LevelArea, LevelAreaID>("Level Areas", tSGame.ptMemoryInterface, MAX_LEVEL_AREAS);

    if (GameIsOpenWorld())
    {
        SGameInitializeDungeonFinderPortalLevelAreaList();
    }
}


//@ 73
void LevelAreasDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();

    TDataArrayIterator<LevelArea> tIt;
    while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
    {
        LevelArea * ptLevelArea = tIt.ptElement;
        tSGame.ptLevelAreaGlobals->ptLevelAreas->ElementDeleteByID(ptLevelArea->id);
    }

    TDataArrayDispose(tSGame.ptLevelAreaGlobals->ptLevelAreas);

    SigmaMemoryFree(tSGame.ptLevelAreaGlobals, tSGame.ptMemoryInterface); tSGame.ptLevelAreaGlobals = NULL;
}



//@ 92
void LevelAreaFlagAllForDeletion(uint32 dwDeleteTime)
{
    SGame & tSGame = SGameManager::GameGet();



    TDataArrayIterator<LevelArea> tIt;
    while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
    {
        LevelArea * ptLevelArea = tIt.ptElement;
        ptLevelArea->dwDeleteTime = dwDeleteTime;
    }
}























static BOOL sEvaluateLevelAreaCondition(
    SNO snoCondition,
    SpawnGroupType eType,
    BOOL fInverseGameModeCondition)
{



    return ConditionEvaluate(snoCondition, NULL, ACDID_INVALID, NULL);
}



LevelAreaID LevelAreaCreate(
    SNO snoLevelArea,
    SWorldID idSWorld,
    RNG & tRng)
{
    SGame & tSGame = SGameManager::GameGet();

    if (tSGame.ptLevelAreaGlobals->ptLevelAreas->AvailableCount() <= 0)
    {


        int32 nCount = 0;
        TDataArrayIterator<LevelArea> tIt;
        while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
        {
            nCount++;  // body not recoverable (the loop has no effect in the binary); nCount is the DWARF local
        }



        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    LevelAreaID idLevelArea = tSGame.ptLevelAreaGlobals->ptLevelAreas->ElementNew();

    LevelArea * ptLevelArea = tSGame.ptLevelAreaGlobals->ptLevelAreas->ElementGet(idLevelArea);
    ptLevelArea->snoLevelArea = snoLevelArea;
    ptLevelArea->idSWorld = idSWorld;
    ptLevelArea->idSSceneWaypoint = SSCENEID_INVALID;
    ptLevelArea->dwFlags = 0;
    ptLevelArea->nPopulationIndex = -1;
    ptLevelArea->dwDeleteTime = 0;  // merged with the dwFlags store (line 171); its own line is a guess

    ptLevelArea->listSpawnGroupIndexes.SetPool(MemoryPoolsGetDATAIDListPool());

    for (int32 i = 0; i < NUM_ENCOUNTER_RANDS; i++)
    {
        ptLevelArea->nEncounterRand[i] = RandRange_Internal(tRng, 0, 0x7fffffff);
    }

    if (snoLevelArea != SNO_NONE)
    {
        SNO snoLevelAreaForSpawns = LevelAreaGetLevelAreaForSpawns(snoLevelArea);

        const LevelAreaDefinition * ptLevelAreaDef = (const LevelAreaDefinition *)gptGroupDefLevelArea->LockR(snoLevelAreaForSpawns, 0);

        SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);












        if (WorldIsDungeonFinderPlaceHolder(snoWorld))
        {
            const char * szPopulation = SWorldGetDungeonFinderPopulation(snoWorld);

            if (szPopulation != NULL)
            {
                for (int32 i = 0; i < ptLevelAreaDef->ptServerData->nSpawnPopulationCount; i++)
                {
                    if (strcasecmp(ptLevelAreaDef->ptServerData->arSpawnPopulations[i].szName, szPopulation) == 0)
                    {
                        ptLevelArea->nPopulationIndex = i;
                    }
                }
            }
        }

        if (ptLevelArea->nPopulationIndex == -1)
        {
            int32 nWeight = 0;
            for (int32 i = 0; i < ptLevelAreaDef->ptServerData->nSpawnPopulationCount; i++)
            {
                nWeight += ptLevelAreaDef->ptServerData->arSpawnPopulations[i].nWeight;
            }

            if (nWeight > 0)
            {
                int32 nPickWeight = RandRange_Internal(tRng, 0, nWeight - 1);

                nWeight = 0;

                for (int32 i = 0; i < ptLevelAreaDef->ptServerData->nSpawnPopulationCount; i++)
                {
                    if (ptLevelAreaDef->ptServerData->arSpawnPopulations[i].nWeight > 0)
                    {
                        nWeight += ptLevelAreaDef->ptServerData->arSpawnPopulations[i].nWeight;

                        if (nWeight > 0 && nWeight >= nPickWeight)
                        {
                            ptLevelArea->nPopulationIndex = i;
                            break;
                        }
                    }
                }
            }
        }



        if (ptLevelAreaDef->ptServerData->arSpawnPopulations != NULL && ptLevelArea->nPopulationIndex >= 0)
        {
            LevelAreaSpawnPopulation * ptSpawnPopulation = &ptLevelAreaDef->ptServerData->arSpawnPopulations[ptLevelArea->nPopulationIndex];







            BOOL fInverseGameModeCondition = FALSE;


            for (int32 nSpawnGroup = 0; nSpawnGroup < SPAWNGROUP_TYPE_COUNT; nSpawnGroup++)
            {
                int32 nPickCount = ptSpawnPopulation->arMinSpawnGroupPickCounts[nSpawnGroup];

                if (GameIsOpenWorld() && nSpawnGroup == SPAWNGROUP_TYPE_UNIQUE)
                {
                    nPickCount +=
                        SQuestsLevelAreaIsRequiredByKillBounty(ptLevelArea->snoLevelArea) ? 1 : 0;
                }





                if (nPickCount < 0)
                {

                    for (int32 i = 0; i < ptSpawnPopulation->nSpawnGroupCount; i++)
                    {
                        LevelAreaSpawnGroup & tSpawnGroup = ptSpawnPopulation->arSpawnGroups[i];
                        if (tSpawnGroup.eType != nSpawnGroup)
                            continue;

                        if (tSpawnGroup.snoCondition != SNO_NONE &&

                            !sEvaluateLevelAreaCondition(tSpawnGroup.snoCondition, tSpawnGroup.eType, fInverseGameModeCondition))
                        {
                            continue;
                        }


                        ptLevelArea->listSpawnGroupIndexes.AddTail_Tracked(i);
                    }
                }
                else
                {

                    int32 nPickCountMax = ptSpawnPopulation->arMaxSpawnGroupPickCounts[nSpawnGroup];

                    if (GameIsOpenWorld() && nSpawnGroup == SPAWNGROUP_TYPE_UNIQUE)
                    {
                        nPickCountMax +=
                            SQuestsLevelAreaIsRequiredByKillBounty(ptLevelArea->snoLevelArea) ? 1 : 0;
                    }





                    if (WorldIsDungeonFinderPlaceHolder(snoWorld) && nSpawnGroup != SPAWNGROUP_TYPE_NORMAL)
                    {
                        float flMultiplier = 1.0f;

                        if (nSpawnGroup == SPAWNGROUP_TYPE_CHAMPION || nSpawnGroup == SPAWNGROUP_TYPE_RARE)
                        {

                            flMultiplier = SWorldGetDungeonFinderEliteSpawnGroupDensityMultiplier(snoWorld);
                        }

                        float flPickCountMin = nPickCount * flMultiplier;
                        float flPickCountMax = nPickCountMax * flMultiplier;

                        nPickCount = Math::FloatToInt(flPickCountMin);
                        nPickCountMax = Math::FloatToInt(flPickCountMax);

                        float flFractionMin = flPickCountMin - nPickCount;
                        float flRandMin = RandFloat_Internal(SGameGlobalsGet().rng);
                        if (flRandMin < flFractionMin)
                        {
                            nPickCount++;
                        }

                        float flFractionMax = flPickCountMax - nPickCountMax;
                        float flRandMax = RandFloat_Internal(SGameGlobalsGet().rng);
                        if (flRandMax < flFractionMax)
                        {
                            nPickCountMax++;
                        }

                        nPickCountMax = Math::Max(nPickCount, nPickCountMax);
                    }

                    if (nPickCountMax > nPickCount)
                    {
                        nPickCount = RandRange_Internal(tRng, nPickCount, nPickCountMax - nPickCount);
                    }

                    if (nPickCount > 0)
                    {

                        int32 nTotalCount = 0;
                        XPooledDataIDList listGroupIdx(MemoryPoolsGetDATAIDListPool());
                        XPooledDataIDList listRequiredGroupIdx(MemoryPoolsGetDATAIDListPool());

                        for (int32 i = 0; i < ptSpawnPopulation->nSpawnGroupCount; i++)
                        {
                            LevelAreaSpawnGroup & tSpawnGroup = ptSpawnPopulation->arSpawnGroups[i];
                            if (tSpawnGroup.eType != nSpawnGroup)
                                continue;

                            if (tSpawnGroup.snoCondition != SNO_NONE &&

                                !sEvaluateLevelAreaCondition(tSpawnGroup.snoCondition, tSpawnGroup.eType, fInverseGameModeCondition))
                            {
                                continue;
                            }


                            BOOL bBountyRequirement = FALSE;

                            if (GameIsOpenWorld())
                            {
                                if (tSpawnGroup.eType == SPAWNGROUP_TYPE_UNIQUE)
                                {
                                    for (int32 j = 0; j < tSpawnGroup.nSpawnItemCount; j++)
                                    {
                                        if (tSpawnGroup.arSpawnItems[j].eMonsterRarity == MR_UNIQUE &&
                                            SQuestsActorIsRequiredByBounty(tSpawnGroup.arSpawnItems[j].snonameToSpawn.snoHandle))
                                        {
                                            bBountyRequirement = TRUE;
                                            break;
                                        }
                                    }
                                }
                            }

                            if (!bBountyRequirement)
                            {
                                listGroupIdx.AddTail_Tracked(i);
                            }
                            else
                            {
                                listRequiredGroupIdx.AddTail_Tracked(i);
                            }

                            nTotalCount++;
                        }



                        int32 nNumToRemove = nTotalCount - nPickCount;

                        if (WorldIsDungeonFinderPlaceHolder(snoWorld))
                        {
                            // The binary calls this at line 404 and ignores the result; the body
                            // (lines 405-416) compiled to nothing.
                        }








                        while (nNumToRemove > 0)
                        {
                            int32 nRemoveIdx = RandRange_Internal(tRng, 0, nNumToRemove);
                            int32 nIndex = 0;
                            POS pos = listGroupIdx.GetHeadPosition();
                            while (pos != NULL)
                            {
                                if (nRemoveIdx <= nIndex)
                                {
                                    listGroupIdx.RemoveAt(pos);
                                    break;
                                }

                                listGroupIdx.GetNext(pos);
                                nIndex++;
                            }

                            nNumToRemove--;
                        }


                        POS pos = listGroupIdx.GetHeadPosition();
                        while (pos != NULL)
                        {
                            int32 nIndex = listGroupIdx.GetNext(pos);
                            ptLevelArea->listSpawnGroupIndexes.AddTail_Tracked(nIndex);
                        }

                        pos = listRequiredGroupIdx.GetHeadPosition();
                        while (pos != NULL)
                        {
                            int32 nIndex = listRequiredGroupIdx.GetNext(pos);
                            ptLevelArea->listSpawnGroupIndexes.AddTail_Tracked(nIndex);
                        }
                    }
                }
            }
        }

        gptGroupDefLevelArea->UnlockR((void *&)ptLevelAreaDef);
    }

    return idLevelArea;
}



//@ 465
void LevelAreasDeleteFlagged()
{
    SGame & tSGame = SGameManager::GameGet();


    uint32 nTime = GameGetServerTime();

    TDataArrayIterator<LevelArea> tIt;
    while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
    {
        if (tIt->dwDeleteTime != 0 && tIt->dwDeleteTime <= nTime)
        {
            LevelArea * ptLevelArea = tIt.ptElement;
            tSGame.ptLevelAreaGlobals->ptLevelAreas->ElementDeleteByID(ptLevelArea->id);
        }
    }
}



//@ 486
LevelArea & LevelAreaGet(LevelAreaID idLevelArea)
{
    SGame & tSGame = SGameManager::GameGet();



    LevelArea * pLevelArea = tSGame.ptLevelAreaGlobals->ptLevelAreas->ElementGet(idLevelArea);

    return *pLevelArea;
}


//@ 499
LevelArea * LevelAreaTryToGet(LevelAreaID idLevelArea)
{
    SGame & tSGame = SGameManager::GameGet();


    if (idLevelArea == LEVELAREAID_INVALID)
        return NULL;
    if (tSGame.ptLevelAreaGlobals == NULL ||
        tSGame.ptLevelAreaGlobals->ptLevelAreas == NULL)
        return NULL;

    LevelArea * pLevelArea = tSGame.ptLevelAreaGlobals->ptLevelAreas->ElementTryToGet(idLevelArea);

    return pLevelArea;
}



//@ 518
SNO LevelAreaGetSNO(LevelAreaID idLevelArea)
{
    LevelArea * ptLevelArea = LevelAreaTryToGet(idLevelArea);
    if (ptLevelArea == NULL)
        return SNO_NONE;
    return ptLevelArea->snoLevelArea;
}






//@ 532
LevelAreaID LevelAreaFindOrCreateBySWorld(SWorldID idSWorld, SNO snoLevelArea)
{

    LevelAreaID idLevelArea = LevelAreaFind(snoLevelArea, idSWorld);
    if (idLevelArea != LEVELAREAID_INVALID && LevelAreaGet(idLevelArea).dwDeleteTime == 0)
        return idLevelArea;

    SWorld * ptSWorld = SWorldGet(idSWorld);
    (void)ptSWorld;
    int32 nSeed = SWorldGetRandSeedFromHash(idSWorld, snoLevelArea);
    RNG tRng;
    RandSetSeed_Internal(tRng, nSeed);







    idLevelArea = LevelAreaCreate(snoLevelArea, idSWorld, tRng);


    return idLevelArea;
}



//@ 560
int32 LevelAreaGetPopulationIndex(LevelAreaID idLevelArea)
{
    LevelArea & tLevelArea = LevelAreaGet(idLevelArea);
    return tLevelArea.nPopulationIndex;
}















//@ 581
LevelAreaID LevelAreaFind(const SNO snoLevelArea, const SWorldID idSWorld)


{
    SGame & tSGame = SGameManager::GameGet();



    TDataArrayIterator<LevelArea> tIt;
    while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
    {
        LevelArea * ptLevelArea = tIt.ptElement;
        if (ptLevelArea->snoLevelArea == snoLevelArea &&
            ptLevelArea->idSWorld == idSWorld &&
            ptLevelArea->dwDeleteTime == 0)
        {
            return ptLevelArea->id;
        }
    }

    return LEVELAREAID_INVALID;
}

//@ 605
void LevelAreaGetName(const SNO snoLevelArea, CRefString & sName)


{
    sName = "";
    if (snoLevelArea != SNO_NONE)
    {
        sName = LevelAreaGetNameLabel(snoLevelArea);
    }
}


//@ 618
void LevelAreaActivate(const LevelAreaID idLevelArea)
{
    LevelArea & tLevelArea = LevelAreaGet(idLevelArea);

    if (!(tLevelArea.dwFlags & (1 << LEVELAREA_FLAG_ACTIVATED)))
    {

        tLevelArea.dwFlags |= (1 << LEVELAREA_FLAG_ACTIVATED);
    }
}






//@ 635
SSceneID LevelAreaGetWaypointSSceneID(const LevelAreaID idLevelArea)
{
    LevelArea & tLevelArea = LevelAreaGet(idLevelArea);
    return tLevelArea.idSSceneWaypoint;
}




//@ 645
int32 LevelAreaGetCount()
{
    SGame & tSGame = SGameManager::GameGet();

    return tSGame.ptLevelAreaGlobals->ptLevelAreas->dwTotalCount;
}


//@ 654
void LevelAreaWorldFlagForDeletion(SWorldID idSWorld, uint32 dwDeleteTime)

{
    SGame & tSGame = SGameManager::GameGet();



    TDataArrayIterator<LevelArea> tIt;
    while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
    {
        LevelArea * ptLevelArea = tIt.ptElement;
        if (ptLevelArea->idSWorld == idSWorld)
        {
            ptLevelArea->dwDeleteTime = dwDeleteTime;
        }
    }
}


//@ 674
SNO LevelAreaFirstInWorld(SWorldID idSWorld)
{
    SGame & tSGame = SGameManager::GameGet();



    TDataArrayIterator<LevelArea> tIt;
    while (tSGame.ptLevelAreaGlobals->ptLevelAreas->Iterate(tIt))
    {
        LevelArea * ptLevelArea = tIt.ptElement;
        if (ptLevelArea->idSWorld == idSWorld)
        {
            return ptLevelArea->snoLevelArea;
        }
    }

    return SNO_NONE;
}
