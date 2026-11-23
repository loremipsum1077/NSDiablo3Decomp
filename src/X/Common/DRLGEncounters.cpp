// Original file: X/Common/DRLGEncounters.cpp
// Written: every owner function sits on its DWARF decl line.

#include "DRLGEncounters.h"
#include "DRLGTypes.h"
#include "Game.h"
#include "GameCommonData.h"
#include "ThreadGlobals.h"
#include "World.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/LevelAreaDefinition.h"
#include "Definitions/MonsterDefinition.h"
#include "../Server/SWorld.h"
#include "../Server/SGameManager.h"
#include <float.h>
#include "SNO/SNOTOC.h"
// SceneCommon.cpp:1834 (not declared in SceneCommon.h in this tree)
DataID SceneCommonAdd(const SNO snoScene, const SceneSpecification & tSceneSpec, SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, const SNO snoMarkerSetSrc, int32 nMarkerSetSrcIndex, LabelListPooled & listAppliedLabels, BOOL bIsVista);
#include "ActorCommon.h"
#include "Hyperlink.h"
// DRLGEncounters.cpp:1309 (defined below)
static BOOL sPickLocationForGroup(SNO snoLevelArea, SpawnGroupLocation & tLocation, TPlacedSceneList & listPlacedScenes, RNG & tRng, BOOL bSpecialType);
#include "Definitions/SceneDefinition.h"
// X/Server/Quests/SQuests.cpp:5410
BOOL SQuestsActorIsRequiredByBounty(SNO snoActor);
// X/Server/SWorld.cpp:3812
void SWorldRegisterUniqueMonsterActor(SWorldID idSWorld, SNO snoActor, SNO snoLevelArea, int32 nDRLGIndex);
#include "Definitions/GlobalDefinition.h"
#include "../Server/System/LevelArea.h"
#include "../Server/Definitions/DungeonFinderDefinition.h"
#include "../Server/SGame.h"
// DRLGEncounters.cpp:1416 (defined below)
static void sGetLevelAreas(PooledSNONameList & listLevelAreaSNOs, const TPlacedSceneList & listPlacedScenes);
// DRLGEncounters.cpp:2067, 2099 (defined below)
void DRLGGetMonsterXPForLevelAreaSpawnItem(const LevelAreaSpawnItem & tSpawnItem, float & flXPMin, float & flXPMax);
void DRLGGetMonsterXPForLevelAreaSpawnGroup(const LevelAreaSpawnGroup & tSpawnGroup, float & flXPMin, float & flXPMax);
// X/Common/World.cpp:505, 904
BOOL WorldIsDRLG(const SNO snoWorld);
SNO WorldGetDungeonFinderLevelArea(const SNO snoWorld);
// X/Server/System/LevelArea.cpp:532, 560
LevelAreaID LevelAreaFindOrCreateBySWorld(SWorldID idSWorld, SNO snoLevelArea);
int32 LevelAreaGetPopulationIndex(LevelAreaID idLevelArea);
// X/Server/SWorld.cpp:2569, 3449
float SWorldGetDungeonFinderSpawnGroupDensityMultiplier(SNO snoWorld);
DungeonFinderLevelInstance * SWorldGlobalsGetDungeonFinderLevelData(int32 nFloor);

#include "GizmoLocs.h"
#include "GizmoSwig.h"
#include "Gizmo.h"
#include "SceneCommon.h"
#include "TagMaps/Tags.h"
#include "TagMaps/ActorTags.h"
// X/Common/DRLG.cpp:275
void MarkerStartMinimapMarkersFromAdventure(const SNO snoAdventure, const SSceneID idSScene, const PRTransform & transform);
#include "TagMaps/MarkerTags.h"
#include "Marker/SmartMarker.h"
#include "Marker/SmartGizmoLocationMarker.h"
#include "GameTemplate.h"
#include "Definitions/MarkerSetDefinition.h"
// X/Common/DRLG.cpp:904; X/Common/Nav/NavigationMesh.cpp:3954; X/Server/SWorld.cpp:302, 2604; ConditionDefinition.cpp:640
TDRLGGizmoPlacePool * DRLGGetGizmoPlacePool();
SNO NavMeshGetLevelAreaAtScenePos(SSceneID idSScene, const Vector2D & vPos2D);
TGizmoLocSpawnMap & SWorldGetGizmoLocSpawnMap();
float SWorldGetDungeonFinderGizmoDensityMultiplier(SNO snoWorld);
struct LooteeInfo;
BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo);





















































































static void sPlaceScene(
    DRLGPlacedScene * ptPlacedScene,
    DRLGPlacedScene * ptPlacedSceneParent,
    SWorldID idSWorld)
{
    DRLGPlacedScene & tPlacedScene = *ptPlacedScene;

    if (SNONAME_NONE == tPlacedScene.snoname)
        return;
    // Inference: the early return at 159/160 leaves no line of its own; the guarded body (165-187) runs only when the name is not SNONAME_NONE (decompile guards the SceneCommonAdd call with this test; line table has 159:6 right after the SNOName::operator== lines 62:33/62:43).



    SNO snoScene = tPlacedScene.snoname.snoHandle;
    SSceneID idSSceneParent = SSCENEID_INVALID;
    SNO snoSceneGroup = tPlacedScene.snoSceneGroup;
    SNO snoMarkerSetSrc = tPlacedScene.snoMarkerSetSrc;
    int32 nMarkerSetSrcIndex = tPlacedScene.nMarkerSetSrcIndex;

    if (ptPlacedSceneParent)
        idSSceneParent = ptPlacedSceneParent->idSScene;
    // Inference: 166 init = SSCENEID_INVALID (no instruction) and the parent lookup at 171 (instruction order: parent loads after the 167-169 loads, so the select is not at 166).


    DataID idScene = SceneCommonAdd(snoScene, tPlacedScene.tSceneSpec, idSWorld,


        PRTransform(cgqIdentity, tPlacedScene.wp),



        idSSceneParent, snoSceneGroup, snoMarkerSetSrc, nMarkerSetSrcIndex,
        tPlacedScene.listAppliedLabels,
        tPlacedScene.tSceneSpec.tDRLGCreationInfo.bIgnoreNavmesh);

    tPlacedScene.idSScene = idScene;
}





void DRLGPlaceTiles(
    SWorldID idSWorld,
    TPlacedSceneList & listPlacedScenes)
{

    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
    ScopedWorldDefinitionLockR pdefWorld(snoWorld);

    POS pos = listPlacedScenes.GetHeadPosition();

    while (pos)
    {
        DRLGPlacedScene * ptPlacedScene = &listPlacedScenes.GetNext(pos);
        DRLGPlacedScene * ptPlacedSceneParent = NULL;

        ptPlacedSceneParent = (DRLGPlacedScene *)ptPlacedScene->posParentScene;
    // Inference: ptPlacedSceneParent (decl 207) is posParentScene, the parent node itself: the decompile reads idSScene at +0x104 of that POS (list node == DRLGPlacedScene, m_tData first).
    // Inference: the snoHandle != SNO_NONE test at 214 (line table 214:36 load, 214:13 compare, ahead of the operator== lines 62:33/62:43 in the inlined sPlaceScene) is separate from the SNONAME_NONE test in sPlaceScene.


        if (ptPlacedScene->snoname.snoHandle != SNO_NONE)

            sPlaceScene(ptPlacedScene, ptPlacedSceneParent, idSWorld);
    }

}


void DRLGCopySpawnGroupExactInstancesToWorld(
    SWorldID idSWorld,
    const TPooledSpawnGroupExactInstanceList & listSpawnGroupExact)
{


    GameCommonData & tGCD = GameGetActiveGameCommonData();

    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
        return;



    SWorld * ptSWorld = SWorldGet(idSWorld);

    POS pos = listSpawnGroupExact.GetHeadPosition();
    while (pos)
    {
        const SpawnGroupExactInstance & tInst = listSpawnGroupExact.GetNext(pos);

        ptSWorld->listSpawnGroupExact.AddHead_Tracked(tInst);
    }
}

struct GroupSpawnInfo {  // size 0x28

    float flSpawnMult;  // 0x00
    SNO snoLevelAreaOrig;  // 0x04
    SNO snoLevelAreaSpawn;  // 0x08
    BOOL bGameIsOpenWorld;  // 0x0c
    BOOL bSpecialType;  // 0x10
    int32 nPopIndex;  // 0x14
    int32 nGroupIndex;  // 0x18
    LevelAreaSpawnPopulation * ptSpawnPop;  // 0x20

    GroupSpawnInfo()
    {
        flSpawnMult = 1.0f;
        snoLevelAreaOrig = SNO_NONE;
        snoLevelAreaSpawn = SNO_NONE;
        bGameIsOpenWorld = FALSE;
        bSpecialType = FALSE;
        nPopIndex = -1;
        nGroupIndex = -1;
        ptSpawnPop = NULL;
    }
};
// Inference (303): the spawn-chance roll returns when RandFloat >= chance (fcmp + b.ge); 311: bSpawnOne forces 1 (csinc).
// Inference (317/318): the multiplier check is split across two lines (line table 317 cbz, 318 fabs/fcmp); 325 holds increment + clamp.
// Inference (355/374): register calls pass tSpawnGroupExactInst.snoLevelAreaOrig (reloaded from the instance copy, not tInfo).
static BOOL sPlaceExactSpawnGroup(
    LevelAreaSpawnGroup & tSpawnGroup,
    SWorldID idSWorld,
    TPlacedSceneList & listPlacedScenes,
    TPooledSpawnGroupExactInstanceList & listSpawnGroupExact,
    RNG & tRng,
    GroupSpawnInfo & tInfo,
    BOOL bSpawnOne)
{
    BOOL bBountyRequirement = FALSE;

    if (tSpawnGroup.flSpawnChance < 0.999999f)
    {
        if (GameIsOpenWorld())
        {
            if (tSpawnGroup.eType == SPAWNGROUP_TYPE_UNIQUE)
            {
                for (int32 i = 0; i < tSpawnGroup.nSpawnItemCount; i++)
                {
                    if (tSpawnGroup.arSpawnItems[i].eMonsterRarity == MR_UNIQUE &&
                        SQuestsActorIsRequiredByBounty(tSpawnGroup.arSpawnItems[i].snonameToSpawn.snoHandle))
                    {
                        bBountyRequirement = TRUE;
                        break;
                    }
                }
            }
        }

        if (!bBountyRequirement)
        {
            if (RandFloat_Internal(tRng) >= tSpawnGroup.flSpawnChance)
                return TRUE;
        }
    }



    int32 nSpawnExactCount = tSpawnGroup.nSpawnExactCount;
    if (bSpawnOne) nSpawnExactCount = 1;





    if (nSpawnExactCount != 0 &&
        Math::Abs(tInfo.flSpawnMult - 1.0f) > 0.000001f)
    {
        float flSpawnExactCount = tInfo.flSpawnMult * nSpawnExactCount;
        nSpawnExactCount = Math::FloatToInt(flSpawnExactCount);

        float flFraction = flSpawnExactCount - nSpawnExactCount;
        float flRand = RandFloat_Internal(SGameGlobalsGet().rng);
        if (flRand < flFraction) nSpawnExactCount++; if (nSpawnExactCount < 1) nSpawnExactCount = 1;
    }






    for (int32 j = 0; j < nSpawnExactCount; j++)
    {
        SpawnGroupExactInstance tSpawnGroupExactInst;

        if (sPickLocationForGroup(tInfo.snoLevelAreaOrig, tSpawnGroupExactInst.tLocation, listPlacedScenes, tRng, tInfo.bSpecialType))
        {
            tSpawnGroupExactInst.snoLevelAreaOrig = tInfo.snoLevelAreaOrig;
            tSpawnGroupExactInst.snoLevelAreaSpawn = tInfo.snoLevelAreaSpawn;
            tSpawnGroupExactInst.nPopIndex = tInfo.nPopIndex;
            tSpawnGroupExactInst.nGroupIndex = tInfo.nGroupIndex;

            listSpawnGroupExact.AddTail_Tracked(tSpawnGroupExactInst);

            if (bBountyRequirement && tInfo.bGameIsOpenWorld)
            {
                if (tSpawnGroup.eType == SPAWNGROUP_TYPE_UNIQUE)
                {
                    for (int32 i = 0; i < tSpawnGroup.nSpawnItemCount; i++)
                    {
                        if (tSpawnGroup.arSpawnItems[i].eMonsterRarity == MR_UNIQUE)
                        {
                            SWorldRegisterUniqueMonsterActor(idSWorld,
                                tSpawnGroup.arSpawnItems[i].snonameToSpawn.snoHandle, tSpawnGroupExactInst.snoLevelAreaOrig, tSpawnGroupExactInst.tLocation.nDRLGIndex);
                        }
                    }
                }
            }



            if (tInfo.bGameIsOpenWorld)
            {
                if (tSpawnGroup.eType == SPAWNGROUP_TYPE_NORMAL)
                {
                    for (int32 i = 0; i < tSpawnGroup.nSpawnItemCount; i++)
                    {
                        if (tSpawnGroup.arSpawnItems[i].eMonsterRarity == MR_UNIQUE)
                        {
                            SNO snoActor = tSpawnGroup.arSpawnItems[i].snonameToSpawn.snoHandle;
                            if (ActorIsKeywarden(snoActor))
                            {
                                SWorldRegisterUniqueMonsterActor(idSWorld, snoActor, tSpawnGroupExactInst.snoLevelAreaOrig, tSpawnGroupExactInst.tLocation.nDRLGIndex);
                            }
                        }
                    }
                }
            }




            if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_SPECIAL_MARKER_CREATION))
            {
                CRefString sLevelArea = HyperlinkCreateFromSNO(SNOName(LEVELAREA_SNO, tInfo.snoLevelAreaSpawn), NULL);
                CRefString sScene = "UNKNOWN";
                POS pos = listPlacedScenes.GetHeadPosition();
                while (pos)
                {
                    const DRLGPlacedScene & tPlacedScene = listPlacedScenes.GetNext(pos);

                    if (tPlacedScene.tSceneSpec.nDRLGIndex == tSpawnGroupExactInst.tLocation.nDRLGIndex)
                    {
                        sScene = HyperlinkCreateFromSNO(SNOName(SCENE_SNO, tPlacedScene.snoname.snoHandle), NULL);
                        break;
                    }
                }



            }
        }
        else
        {
            CRefString sLevelArea = HyperlinkCreateFromSNO(SNOName(LEVELAREA_SNO, tInfo.snoLevelAreaSpawn), NULL);


            return FALSE;
        }
    }
    return TRUE;
}
// Inference (446/1325/1358): the index/SNOName guard uses bitwise | (original keeps the unthreaded mov #k / cmn #-1
// pattern a short-circuit || jump-threads away).

static int32 sSceneGetLevelAreaIndex(
    const SceneSpecification & tSceneSpec,
    SNO snoLevelArea)
{
    int32 nIndex = -1;
    for (int32 i = 0; i < 4; i++)
    {
        if (tSceneSpec.arSnoLevelAreas[i] == snoLevelArea)
        {
            nIndex = i;
            break;
        }
    }
    return nIndex;
}
// Inference (455/458): flNodes is converted first and multiplied by flXPDensityMultiplier under the scene lock (fmul after LockR).


static int32 sGetLevelAreaWalkableTiles(
    SNO snoLevelArea,
    const TPlacedSceneList & listPlacedScenes)
{
    float flNumTiles = 0.0f;
    POS pos = listPlacedScenes.GetHeadPosition();
    while (pos)
    {
        const DRLGPlacedScene & tPlacedScene = listPlacedScenes.GetNext(pos);
        int32 nLevelAreaIndex = sSceneGetLevelAreaIndex(tPlacedScene.tSceneSpec, snoLevelArea);

        if ((nLevelAreaIndex == -1) | (SNONAME_NONE == tPlacedScene.snoname))
            continue;




        const SceneCachedValues & tSceneCachedValues = tPlacedScene.tSceneSpec.tCachedValues;
        if (!(tSceneCachedValues.dwFlags & (1 << SCENE_DISABLE_DISTRIBUTION_SPAWNING_BIT)))
        {
            float flNodes = (float)tSceneCachedValues.arNumNodesForDistByLevelArea[nLevelAreaIndex];

            ScopedSceneDefinitionLockR pdefScene(tPlacedScene.snoname.snoHandle);
            flNodes *= pdefScene->flXPDensityMultiplier;
            flNumTiles += flNodes;
        }
    }

    return Math::FloatRoundToInt(flNumTiles);
}




BOOL DRLGPlaceSpawnGroupExactInstances(
    SWorldID idSWorld,
    TPlacedSceneList & listPlacedScenes,
    TPooledSpawnGroupExactInstanceList & listSpawnGroupExact,
    RNG & tRng)
{
// Inference (537/555/664/1348): Rand_Internal(rng, n) is inclusive, so callers pass count - 1 (sub #1 before RandGetDWord).
// Inference (565/677/1377): weight compares are >= (b.ge); 578/612: bSpecialType is the eType-1 < 3 range test.
// Inference (630): switch on eSpawnDistribution (cmp chain on one line); 639: if (n > 0) (bic asr).
// Inference (652): Math::Max(0.0f, delta) (DWARF const x = 0, fmax); 688: Math::Max(flXPMin, 0.01f) (fmaxnm).
// Inference (661): nWalkableTiles * (flXPDensity / 1000.0f): this operand order gives the original fdiv s1 / scvtf s0 allocation.
    if (GameGetActiveGameCommonData().GameModeGet() != GAMEMODE_PLAYING)
        return TRUE;




    BOOL bSuccess = TRUE;





    GroupSpawnInfo tInfo;
    tInfo.bGameIsOpenWorld = GameIsOpenWorld();

    DungeonFinderLevelInstance * ptDungeonFinderInstance = NULL;
    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
    if (WorldIsDungeonFinderPlaceHolder(snoWorld))
    {

        if (WorldIsDRLG(snoWorld))
        {
            SNO snoDungeonFinderLevelArea = WorldGetDungeonFinderLevelArea(snoWorld);
            LevelAreaFindOrCreateBySWorld(idSWorld, snoDungeonFinderLevelArea);
        }

        int32 nFloor = WorldGetDungeonFinderLevel(snoWorld);
        if (nFloor > 0)
        {
            ptDungeonFinderInstance = SWorldGlobalsGetDungeonFinderLevelData(nFloor - 1);
        }
    }

    PooledSNONameList listLevelAreaSNOs(GetSnoNameListPool());


    sGetLevelAreas(listLevelAreaSNOs, listPlacedScenes);

    POS pos = listLevelAreaSNOs.GetHeadPosition();
    while (pos)
    {
        tInfo.snoLevelAreaOrig = listLevelAreaSNOs.GetNext(pos).snoHandle;

        ScopedGlobalDefinitionLockR pdefGlobal;


        int32 nWalkableTiles = sGetLevelAreaWalkableTiles(tInfo.snoLevelAreaOrig, listPlacedScenes);


        if (ptDungeonFinderInstance && ptDungeonFinderInstance->snoElitePopulation != SNO_NONE)
        {
            ScopedLevelAreaDefinitionLockR ptLevelAreaDef(ptDungeonFinderInstance->snoElitePopulation);
            if (ptLevelAreaDef->ptServerData->nSpawnPopulationCount > 0)
            {
                int32 nNumElitesToSpawn = Math::FloatFloorToInt(pdefGlobal->ptServerData->flGlobalEliteDensity * ptDungeonFinderInstance->flEliteSpawnGroupDensityMultiplier / 1000.0f * nWalkableTiles);
                for (int32 i = 0; i < nNumElitesToSpawn; i++)
                {
                    tInfo.nPopIndex = Rand_Internal(tRng, ptLevelAreaDef->ptServerData->nSpawnPopulationCount - 1);

                    tInfo.snoLevelAreaSpawn = ptDungeonFinderInstance->snoElitePopulation;

                    tInfo.ptSpawnPop = &ptLevelAreaDef->ptServerData->arSpawnPopulations[tInfo.nPopIndex];

                    int32 nWeightTotal = 0;
                    for (int32 nSpawnGroupIdx = 0; nSpawnGroupIdx < tInfo.ptSpawnPop->nSpawnGroupCount; nSpawnGroupIdx++)
                    {
                        LevelAreaSpawnGroup & tSpawnGroup = tInfo.ptSpawnPop->arSpawnGroups[nSpawnGroupIdx];
                        if (tSpawnGroup.eSpawnDistribution == DISTRIBUTION_XP_DENSITY)
                        {
                            nWeightTotal += tSpawnGroup.nSpawnExactCount;
                        }
                    }

                    if (nWeightTotal > 0)
                    {
                        int32 nWeightTarget = Rand_Internal(tRng, nWeightTotal - 1);
                        int32 nWeight = 0;
                        tInfo.nGroupIndex = -1;

                        for (int32 nSpawnGroupIdx = 0; nSpawnGroupIdx < tInfo.ptSpawnPop->nSpawnGroupCount; nSpawnGroupIdx++)
                        {
                            LevelAreaSpawnGroup & tSpawnGroup = tInfo.ptSpawnPop->arSpawnGroups[nSpawnGroupIdx];
                            if (tSpawnGroup.eSpawnDistribution == DISTRIBUTION_XP_DENSITY)
                            {
                                nWeight += tSpawnGroup.nSpawnExactCount;
                                if (nWeight >= nWeightTarget)
                                {
                                    tInfo.nGroupIndex = nSpawnGroupIdx;
                                    break;
                                }
                            }
                        }

                        if (tInfo.nGroupIndex >= 0)
                        {

                            LevelAreaSpawnGroup & tSpawnGroup = tInfo.ptSpawnPop->arSpawnGroups[tInfo.nGroupIndex];

                            tInfo.bSpecialType = (tSpawnGroup.eType == SPAWNGROUP_TYPE_CHAMPION || tSpawnGroup.eType == SPAWNGROUP_TYPE_RARE || tSpawnGroup.eType == SPAWNGROUP_TYPE_UNIQUE);






                            tInfo.flSpawnMult = 1.0f;

                            if (!sPlaceExactSpawnGroup(tSpawnGroup, idSWorld, listPlacedScenes, listSpawnGroupExact, tRng, tInfo, TRUE)) bSuccess = FALSE;
                        }
                    }
                }
            }



        }

        tInfo.snoLevelAreaSpawn = LevelAreaGetLevelAreaForSpawns(tInfo.snoLevelAreaOrig);
        ScopedLevelAreaDefinitionLockR ptLevelAreaDef(tInfo.snoLevelAreaSpawn);
        LevelAreaID idLevelArea = LevelAreaFindOrCreateBySWorld(idSWorld, tInfo.snoLevelAreaOrig);
        tInfo.nPopIndex = LevelAreaGetPopulationIndex(idLevelArea);
        if (tInfo.nPopIndex >= 0 && tInfo.nPopIndex < ptLevelAreaDef->ptServerData->nSpawnPopulationCount)
        {
            int32 nTotalXPDensityWeight = 0;
            tInfo.ptSpawnPop = &ptLevelAreaDef->ptServerData->arSpawnPopulations[tInfo.nPopIndex];
            LevelArea & tLevelArea = LevelAreaGet(idLevelArea);
            POS posSpawnGroupIndexes = tLevelArea.listSpawnGroupIndexes.GetHeadPosition();
            while (posSpawnGroupIndexes)
            {
                tInfo.nGroupIndex = tLevelArea.listSpawnGroupIndexes.GetNext(posSpawnGroupIndexes);
                LevelAreaSpawnGroup & tSpawnGroup = tInfo.ptSpawnPop->arSpawnGroups[tInfo.nGroupIndex];

                tInfo.bSpecialType = (tSpawnGroup.eType == SPAWNGROUP_TYPE_CHAMPION || tSpawnGroup.eType == SPAWNGROUP_TYPE_RARE || tSpawnGroup.eType == SPAWNGROUP_TYPE_UNIQUE);






                tInfo.flSpawnMult = 1.0f;


                if (WorldIsDungeonFinderPlaceHolder(snoWorld))
                {
                    if (tSpawnGroup.eType == SPAWNGROUP_TYPE_NORMAL)
                    {
                        tInfo.flSpawnMult = SWorldGetDungeonFinderSpawnGroupDensityMultiplier(snoWorld);
                    }
                }

                switch (tSpawnGroup.eSpawnDistribution)
                {
                case DISTRIBUTION_EXACT: if (!sPlaceExactSpawnGroup(tSpawnGroup, idSWorld, listPlacedScenes, listSpawnGroupExact, tRng, tInfo, FALSE)) bSuccess = FALSE;
                    break;




                case DISTRIBUTION_XP_DENSITY:
                    if (tSpawnGroup.nSpawnExactCount > 0) nTotalXPDensityWeight += tSpawnGroup.nSpawnExactCount;
                    break;
                }
            }



            if (nTotalXPDensityWeight > 0)
            {
                tInfo.bSpecialType = FALSE;
                tInfo.flSpawnMult = 1.0f;

                float flXPDensityDelta = pdefGlobal->ptServerData->flGlobalMonsterXPDensityMax - pdefGlobal->ptServerData->flGlobalMonsterXPDensityMin;
                flXPDensityDelta = Math::Max(0.0f, flXPDensityDelta);
                float flXPDensity = RandRangeFloat_Internal(tRng, pdefGlobal->ptServerData->flGlobalMonsterXPDensityMin, flXPDensityDelta);


                if (ptDungeonFinderInstance)
                {
                    flXPDensity *= ptDungeonFinderInstance->flSpawnGroupDensityMultiplier;
                }

                float flXPRemaining = nWalkableTiles * (flXPDensity / 1000.0f);
                while (flXPRemaining > 0.000001f && bSuccess)
                {
                    int32 nWeightTarget = Rand_Internal(tRng, nTotalXPDensityWeight - 1);
                    int32 nWeight = 0;
                    posSpawnGroupIndexes = tLevelArea.listSpawnGroupIndexes.GetHeadPosition();
                    while (posSpawnGroupIndexes)
                    {
                        tInfo.nGroupIndex = tLevelArea.listSpawnGroupIndexes.GetNext(posSpawnGroupIndexes);

                        LevelAreaSpawnGroup & tSpawnGroup = tInfo.ptSpawnPop->arSpawnGroups[tInfo.nGroupIndex];
                        if (tSpawnGroup.eSpawnDistribution == DISTRIBUTION_XP_DENSITY)
                        {
                            if (tSpawnGroup.nSpawnExactCount > 0)
                            {
                                nWeight += tSpawnGroup.nSpawnExactCount;
                                if (nWeight >= nWeightTarget)
                                {
                                    if (sPlaceExactSpawnGroup(tSpawnGroup, idSWorld, listPlacedScenes, listSpawnGroupExact, tRng, tInfo, FALSE))
                                    {




                                        float flXPMin;
                                        float flXPMax;
                                        DRLGGetMonsterXPForLevelAreaSpawnGroup(tSpawnGroup, flXPMin, flXPMax);
                                        float flXPConsumed = Math::Max(flXPMin, 0.01f);
                                        flXPRemaining -= flXPConsumed;
                                    }
                                    else
                                        bSuccess = FALSE;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    return bSuccess;
}



























































































































































































































































































































































































































































































































































































































static BOOL sPickLocationForGroup(
    SNO snoLevelArea,
    SpawnGroupLocation & tLocation,
    TPlacedSceneList & listPlacedScenes,
    RNG & tRng,
    BOOL bSpecialType)
{
    BOOL bFoundLocation = FALSE;
    int32 nWeight = 0;

    POS pos = listPlacedScenes.GetHeadPosition();
    while (pos)
    {
        const DRLGPlacedScene & tPlacedScene = listPlacedScenes.GetNext(pos);
        int32 nLevelAreaIndex = sSceneGetLevelAreaIndex(tPlacedScene.tSceneSpec, snoLevelArea);

        if ((nLevelAreaIndex == -1) | (SNONAME_NONE == tPlacedScene.snoname))
            continue;




        if (bSpecialType && tPlacedScene.nSpecialTypesPlaced > 0)
            continue;



        const SceneCachedValues & tSceneCachedValues = tPlacedScene.tSceneSpec.tCachedValues;

        if (!(tSceneCachedValues.dwFlags & (1 << SCENE_DISABLE_DISTRIBUTION_SPAWNING_BIT)))
        {
            int32 nNodes = tSceneCachedValues.arNumNodesForDistByLevelArea[nLevelAreaIndex];

            nWeight += nNodes;
        }
    }

    if (nWeight > 0)
    {
        int32 nPickWeight = Rand_Internal(tRng, nWeight - 1);

        nWeight = 0;

        pos = listPlacedScenes.GetHeadPosition();
        while (pos)
        {
            DRLGPlacedScene & tPlacedScene = listPlacedScenes.GetNext(pos);
            int32 nLevelAreaIndex = sSceneGetLevelAreaIndex(tPlacedScene.tSceneSpec, snoLevelArea);

            if ((nLevelAreaIndex == -1) | (SNONAME_NONE == tPlacedScene.snoname))
                continue;




            if (bSpecialType && tPlacedScene.nSpecialTypesPlaced > 0)
                continue;



            const SceneCachedValues & tSceneCachedValues = tPlacedScene.tSceneSpec.tCachedValues;

            if (!(tSceneCachedValues.dwFlags & (1 << SCENE_DISABLE_DISTRIBUTION_SPAWNING_BIT)))
            {
                int32 nNodes = tSceneCachedValues.arNumNodesForDistByLevelArea[nLevelAreaIndex];

                nWeight += nNodes;

                if (nWeight >= nPickWeight)
                {

                    tLocation.nDRLGIndex = tPlacedScene.tSceneSpec.nDRLGIndex;
                    tLocation.nNodeCount = nNodes;

                    if (bSpecialType)
                        tPlacedScene.nSpecialTypesPlaced++;

                    bFoundLocation = TRUE;
                    break;
                }
            }
        }
    }
    if (!bFoundLocation && bSpecialType)
        return sPickLocationForGroup(snoLevelArea, tLocation, listPlacedScenes, tRng, FALSE);

    return bFoundLocation;
}
// Inference (1392/1393): the special-type retry is a tail call back into sPickLocationForGroup with bSpecialType FALSE.
// Inference (1414-1440): sGetLevelAreas stays out of line because DRLGPlaceGizmoLocMarkers (1923) is its second caller.

















static void sGetLevelAreas(
    PooledSNONameList & listLevelAreaSNOs,
    const TPlacedSceneList & listPlacedScenes)
{
    POS pos = listPlacedScenes.GetHeadPosition();

    while (pos)
    {
        const DRLGPlacedScene & tPlacedScene = listPlacedScenes.GetNext(pos);

        for (int32 i = 0; i < 4; i++)
        {
            SNO snoLevelArea = tPlacedScene.tSceneSpec.arSnoLevelAreas[i];

            SNOName snoName(LEVELAREA_SNO, snoLevelArea);

            if (snoLevelArea != SNO_NONE && !listLevelAreaSNOs.Find(snoName))
            {



                listLevelAreaSNOs.AddHead_Tracked(snoName);
            }
        }
    }
}
// Inference (1501): sSpawnGizmoLocEntry has external linkage (mangled _Z19, no L), despite the s prefix.
// Inference (1528): both snoGroup compares sit on 1528, so it is a switch; ACTOR_SNO case scope ends at 1553 (lock dtor).
// Inference (1537/1538, 1556/1557): transform copies the placement transform (copy ctor) then is reassigned to
// tSCD.transformWorld * transform (operator* + operator=).
// Inference (1548/1563): bPlacedLoot is set from eGizmoType == ACTORGIZMO_PLACED_LOOT; the compare sinks to the 1563 branch.






















































void sSpawnGizmoLocEntry(
    const GizmoLocSpawnEntry & tEntry,
    const DRLGGizmoPlace & tGizmoPlace,
    RNG & tRng,
    int32 nEntryIdx)
{

    SceneCommonData & tSCD = SceneCommonDataGet(tGizmoPlace.idSScene);
    GizmoLocPlacement tPlacement;
    GizmoLocSpawnChoice * ptChoice = tEntry.RollChoice(tRng, nEntryIdx);
    if (ptChoice)
    {
        tPlacement.snoname = ptChoice->snonameToSpawn;
        tPlacement.tPlace = tGizmoPlace;

        if (ptChoice->bForceRandomFacing)
        {
            Angle aYaw = RandAngle(tRng);
            tPlacement.tPlace.transform.q.FromYaw(aYaw);
        }

        BOOL bPlacedLoot = FALSE;

        if (GameActiveGameCommonDataIsServer())
        {


            switch (tPlacement.snoname.snoGroup)
            {
            case ACTOR_SNO: {
                    ScopedActorDefinitionLockR pdefActor(tPlacement.snoname.snoHandle);
                    float flNoSpawnRadius = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_NO_SPAWN_RADIUS);
                    if (flNoSpawnRadius > 0.000001f)
                    {
                        SWorld * ptSWorld = SWorldTryToGet(tSCD.idSWorld);

                        PRTransform transform = tPlacement.tPlace.transform;
                        transform = tSCD.transformWorld * transform;

                        Circle tCircle;
                        tCircle.Set(transform.wp.x, transform.wp.y, flNoSpawnRadius);
                        ptSWorld->tNoSpawnTree.AddCircle(tCircle);
                    }

                    if (pdefActor->eType == ACTORTYPE_GIZMO)
                    {
                        GizmoType eGizmoType = ActorGetGizmoType(tPlacement.snoname.snoHandle);
                        if (eGizmoType == ACTORGIZMO_PLACED_LOOT)
                        {
                            bPlacedLoot = TRUE;
                        }
                    }
                }
                break;
            case ADVENTURE_SNO: {
                    PRTransform transform = tPlacement.tPlace.transform;
                    transform = tSCD.transformWorld * transform;
                    MarkerStartMinimapMarkersFromAdventure(tPlacement.snoname.snoHandle, tSCD.idSScene, transform);
                }
                break;
            }
        }
        if (bPlacedLoot)
        {
            tSCD.listGizmoLocLoot.AddTail_Tracked(tPlacement);
        }
        else
        {
            tSCD.listGizmoLocPlacements.AddTail_Tracked(tPlacement);
        }
    }
}
// Inference (1578-1585): translation returns the override when set (csel on cmn #-1), lock scope closes at 1585.

static SNO sGetTranslatedLevelAreaForGizmoLoc(
    SNO snoLevelArea)
{
    if (snoLevelArea != SNO_NONE)
    {
        ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
        if (pdefLevelArea->ptServerData->snoLevelAreaOverrideForGizmoLocs != SNO_NONE)
        {
            snoLevelArea = pdefLevelArea->ptServerData->snoLevelAreaOverrideForGizmoLocs;
        }
    }

    return snoLevelArea;
}


static BOOL sSceneHasLevelAreaForGizmoPlacement(
    const SceneSpecification & tSceneSpec,
    SNO snoLevelArea)
{
    for (int32 i = 0; i < 4; i++)
    {
        SNO snoLevelAreaToCheck = tSceneSpec.arSnoLevelAreas[i];
        snoLevelAreaToCheck = sGetTranslatedLevelAreaForGizmoLoc(snoLevelAreaToCheck);
        if (snoLevelAreaToCheck == snoLevelArea)
            return TRUE;
    }

    return FALSE;
}
// Inference (1625): early return when the area has a gizmo-loc override; lists and lock die at 1897 (function end).
// Inference (1632/1634, 1746): 52 = GizmoLocSet::arSpawnArray extent; no named constant exists in the DWARF.
// Inference (1734): pGizmoMarker->GetGizmoIndex() is inlined; its load folds into the ldrsw attributed to 1735.
// Inference (1769/1796): Math::Max(nSpawnMin, max) (cmp/csel operand order).
static void sPlaceGizmoLocMarkersInLevelArea(
    SWorldID idSWorld,
    SNO snoLevelArea,
    TPlacedSceneList & listPlacedScenes,
    RNG & tRng)
{


    if (snoLevelArea == SNO_NONE)
        return;



    BOOL bWorldIsDungeonFinderWorld = WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld));

    ScopedLevelAreaDefinitionLockR pdefLevelArea(snoLevelArea);
    if (pdefLevelArea->ptServerData->snoLevelAreaOverrideForGizmoLocs != SNO_NONE)
        return;




    TDRLGGizmoPlaceList arlistGizmoPlace[52];
    for (int32 i = 0; i < 52; i++)
    {
        arlistGizmoPlace[i].SetPool(DRLGGetGizmoPlacePool());
    }

    Vector3D wpEntrance;

    POS pos = listPlacedScenes.GetHeadPosition();
    while (pos)
    {
        DRLGPlacedScene * ptPlacedScene = &listPlacedScenes.GetNext(pos);
        if (ptPlacedScene->snoname.snoGroup != SCENE_SNO)
            continue;









        if (!sSceneHasLevelAreaForGizmoPlacement(ptPlacedScene->tSceneSpec, snoLevelArea))
            continue;




        SNO snoScene = ptPlacedScene->snoname.snoHandle;
        ScopedSceneDefinitionLockR pdefScene(snoScene);
        for (int32 nMarkerSet = 0; nMarkerSet < pdefScene->arMarkerSets.GetArraySize(); nMarkerSet++)
        {
            SNO snoMarkerSet = pdefScene->arMarkerSets[nMarkerSet];
            ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
            GBID gbidLabel = pdefMarkerSet->nLabel;
            if (gbidLabel != GBID_NONE &&
                !ptPlacedScene->listAppliedLabels.Find(gbidLabel))
            {
                continue;
            }




            for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
            {

                int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];


                SmartMarker sMarker(pdefMarkerSet, nIndex);

                if (!sMarker.IsGizmoLocationMarker())
                    continue;









                if (bWorldIsDungeonFinderWorld)
                {
                    if (TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_DISABLED_IN_DUNGEON_FINDER))
                        continue;
                }
                else
                {
                    if (TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_ENABLED_IN_DUNGEON_FINDER))
                        continue;
                }


                if (GameIsOpenWorld() &&
                    TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_DISABLED_IN_OPEN_WORLD))
                {
                    continue;
                }

                if (GameIsStoryMode() &&
                    TagMapGetInt(sMarker.GetTagMap(), TAG_MARKER_DISABLED_IN_STORY_MODE))
                {
                    continue;
                }


                DRLGGizmoPlace tGizmoPlace;
                tGizmoPlace.idSScene = ptPlacedScene->idSScene;
                tGizmoPlace.transform = sMarker.GetTransform();
                tGizmoPlace.snoMarkerSet = snoMarkerSet;
                tGizmoPlace.nMarkerIndex = nIndex;
                tGizmoPlace.snoLevelArea = snoLevelArea;

                SNO snoLevelAreaAtLocation = NavMeshGetLevelAreaAtScenePos(tGizmoPlace.idSScene, tGizmoPlace.transform.wp.As2D());
                snoLevelAreaAtLocation = sGetTranslatedLevelAreaForGizmoLoc(snoLevelAreaAtLocation);
                if (snoLevelAreaAtLocation == snoLevelArea)
                {
                    GizmoLocationMarker * pGizmoMarker = (GizmoLocationMarker *)sMarker.InterfaceGet();


                    int nIndexLoc = pGizmoMarker->GetGizmoIndex();
                    arlistGizmoPlace[nIndexLoc].AddTail_Tracked(tGizmoPlace);
                }
            }


        }
    }




    for (int32 nType = 0; nType < 52; nType++)
    {
        const GizmoLocSpawnType & tType = pdefLevelArea->ptServerData->tGizmoLocSet.arSpawnArray[nType];
        int32 nArraySize = tType.serEntry.dwSize / sizeof(GizmoLocSpawnEntry);
        TDRLGGizmoPlaceList & tListTransform = arlistGizmoPlace[nType];
        for (int32 nEntry = 0; nEntry < nArraySize; nEntry++)
        {
            TGizmoLocSpawnMap & mapSpawns = SWorldGetGizmoLocSpawnMap();
            mapSpawns.RemoveAll();

            GizmoLocSpawnEntry & tEntry = tType.arEntry[nEntry];

            if (tEntry.snoCondition != SNO_NONE &&

                !ConditionEvaluate(tEntry.snoCondition, NULL, ACDID_INVALID, NULL))
            {
                continue;
            }


            if (tEntry.GetNumValidChoices() <= 0)
                continue;
            int32 nSpawnMin = Math::Max(tEntry.nSpawnMin, 0);
            int32 nSpawnMax = Math::Max(nSpawnMin, tEntry.nSpawnMax);



            SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
            if (WorldIsDungeonFinderPlaceHolder(snoWorld) &&

                (tEntry.dwFlags & (1 << GLF_AFFECTED_BY_GIZMO_DENSITY_MULTIPLIER)))
            {
                float flMultiplier = SWorldGetDungeonFinderGizmoDensityMultiplier(snoWorld);

                float flSpawnMin = flMultiplier * nSpawnMin;
                nSpawnMin = Math::FloatToInt(flSpawnMin);
                float flSpawnMinFraction = flSpawnMin - nSpawnMin;
                if (RandFloat_Internal(SGameGlobalsGet().rng) < flSpawnMinFraction) nSpawnMin++;




                float flSpawnMax = flMultiplier * nSpawnMax;
                nSpawnMax = Math::FloatToInt(flSpawnMax);
                float flSpawnMaxFraction = flSpawnMax - nSpawnMax;
                if (RandFloat_Internal(SGameGlobalsGet().rng) < flSpawnMaxFraction) nSpawnMax++;




                nSpawnMax = Math::Max(nSpawnMin, nSpawnMax);
            }







































            int32 nSpawnDelta = nSpawnMax - nSpawnMin;
            int32 nSpawnNum = RandRange_Internal(tRng, nSpawnMin, nSpawnDelta);
            while (nSpawnNum > 0 && tListTransform.Count() > 0)
            {
                int32 nTotalCount = tListTransform.Count();
                int32 nRandChoose = RandRange_Internal(tRng, 0, nTotalCount - 1);
                POS posLoc = tListTransform.GetHeadPosition();
                for (int32 i = 0; i <= nRandChoose; i++)
                {
                    POS posLast = posLoc;
                    DRLGGizmoPlace & tGizmoPlace = tListTransform.GetNext(posLoc);
                    if (i >= nRandChoose)
                    {
                        sSpawnGizmoLocEntry(tEntry, tGizmoPlace, tRng, nEntry);
                        tListTransform.RemoveAt(posLast);
                        break;
                    }
                }

                nSpawnNum--;
            }
        }



        AppGlobals & tAppGlobals = sgtAppGlobals;
        if (tAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FORCE_SPAWN_ALL_GIZMO_LOCATIONS))
        {
            GizmoLocSpawnEntry * ptEntry;
            GizmoLocSpawnEntry tTempEntry;
            GizmoLocSpawnChoice tTempChoice;
            if (nArraySize > 0)
            {
                ptEntry = tType.arEntry;
            }
            else
            {
                tTempEntry.Initialize();

                tTempEntry.nSpawnMin = 1; tTempEntry.nSpawnMax = 1;
                tTempEntry.serChoice.dwOffset = 0; tTempEntry.serChoice.dwSize = sizeof(GizmoLocSpawnChoice);

                tTempEntry.arChoice = &tTempChoice;
                tTempChoice.snonameToSpawn.snoHandle = GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_CHEST);
                tTempChoice.snonameToSpawn.snoGroup = ACTOR_SNO;
                tTempChoice.bForceRandomFacing = FALSE; tTempChoice.flWeight = 10.0f;

                tTempChoice.nMaxTimesPicked = -1;
                ptEntry = &tTempEntry;
            }


            POS posLoc = tListTransform.GetHeadPosition();
            while (posLoc)
            {
                DRLGGizmoPlace & tGizmoPlace = tListTransform.GetNext(posLoc);
                sSpawnGizmoLocEntry(*ptEntry, tGizmoPlace, tRng, -1);
            }
        }
    }
}
// Inference (1848): the walk tests i >= nRandChoose (cmp/b.lt at 1848) and breaks after RemoveAt.
// Inference (1868-1884): debug path builds a one-choice temp entry (GLOBALSNO_MARKER_ACTOR_CHEST, weight 10, unlimited picks).
void DRLGPlaceGizmoLocMarkers(
    SWorldID idSWorld,
    TPlacedSceneList & listPlacedScenes,
    RNG & tRng)
{






    if (GameGetActiveGameCommonData().GameModeGet() != GAMEMODE_PLAYING)
        return;









    PooledSNONameList listLevelAreaSNOs(GetSnoNameListPool());
    sGetLevelAreas(listLevelAreaSNOs, listPlacedScenes);

    POS posLevelArea = listLevelAreaSNOs.GetHeadPosition();
    while (posLevelArea)
    {
        SNO snoLevelArea = listLevelAreaSNOs.GetNext(posLevelArea).snoHandle;
        LevelAreaFindOrCreateBySWorld(idSWorld, snoLevelArea);
        sPlaceGizmoLocMarkersInLevelArea(idSWorld, snoLevelArea, listPlacedScenes, tRng);
    }

}


static void sGetMonsterXPForChampionSpawnGroup(
    const MonsterChampionSpawnGroup & tSpawnGroup,
    float & flXP)
{
    // Inference: line 1942 (flWeight load 1942:21) guards the loop; the compare constant is 0.000001f (rodata float).
    // Inference: tSpawnItem is a value copy (DWARF local type MonsterChampionSpawnItem, not a reference); the null check at 1950 is cbz in the asm.
    if (tSpawnGroup.flWeight > 0.000001f)
    {
        for (int i = 0; i < tSpawnGroup.nSpawnItemCount; i++)
        {
            MonsterChampionSpawnItem tSpawnItem = tSpawnGroup.arSpawnItems[i];
            if (tSpawnItem.snoActor != SNO_NONE)
            {
                ScopedMonsterDefinitionLockR ptDefMonster(ActorGetMonsterSNO(tSpawnItem.snoActor));
                if (ptDefMonster)

                    flXP += ptDefMonster->pctAttributeModifiers[GBA_EXPERIENCE] * tSpawnItem.nSpawnCount;
            }
        }
    }
}



void sGetMonsterXPForChampionGroups(
    const SNO snoMonster,
    float & flXPMin,
    float & flXPMax)
{
    flXPMin = FLT_MAX;
    flXPMax = FLT_MIN;

    ScopedMonsterDefinitionLockR ptDefMonster(snoMonster);
    if (ptDefMonster)
    {
        for (int i = 0; i < ptDefMonster->nChampionSpawnGroupCount; i++)
        {
            const MonsterChampionSpawnGroup & tSpawnGroup = ptDefMonster->arChampionSpawnGroups[i];

            float flSpawnGroupXP = 0.0f;
            sGetMonsterXPForChampionSpawnGroup(tSpawnGroup, flSpawnGroupXP);

            if (flSpawnGroupXP > 0.000001f && flSpawnGroupXP > flXPMax)

                flXPMax = flSpawnGroupXP;


            if (flSpawnGroupXP > 0.000001f && flSpawnGroupXP < flXPMin)

                flXPMin = flSpawnGroupXP;
        }
    }

}


static void sGetMonsterXPForRareMinionSpawnGroup(
    const MonsterMinionSpawnGroup & tSpawnGroup,
    float & flXPMin,
    float & flXPMax)
{
    // Inference: weight guard at 2000 (2000:21 load); compared against 0.0f (fcmp with zero).

    if (tSpawnGroup.nWeight > 0.0f)
    {
        for (int i = 0; i < tSpawnGroup.nSpawnItemCount; i++)
        {
            MonsterMinionSpawnItem tSpawnItem = tSpawnGroup.arSpawnItems[i];
            if (tSpawnItem.snoSpawn != SNO_NONE)
            {
                ScopedMonsterDefinitionLockR ptDefMonster(ActorGetMonsterSNO(tSpawnItem.snoSpawn));
                if (ptDefMonster)
                {
                    flXPMin += ptDefMonster->pctAttributeModifiers[GBA_EXPERIENCE] * tSpawnItem.nSpawnCountMin;
                    flXPMax += ptDefMonster->pctAttributeModifiers[GBA_EXPERIENCE] * tSpawnItem.nSpawnCountMax;
                }
            }
        }
    }
}



void sGetMonsterXPForRareSpawnGroup(
    const SNO snoMonster,
    float & flXPMin,
    float & flXPMax)
{
    flXPMin = FLT_MAX;
    flXPMax = FLT_MIN;

    ScopedMonsterDefinitionLockR ptDefMonster(snoMonster);
    float flXPRareMonsterBase = ptDefMonster->pctAttributeModifiers[GBA_EXPERIENCE];
    // Inference: line 2028 reads the definition before the null check at 2031 (decompile loads +0x12c before testing the pointer; table 2028:33 precedes 2031:9).
    if (ptDefMonster)
    {
        for (int i = 0; i < ptDefMonster->nMinionSpawnGroupCount; i++)
        {
            const MonsterMinionSpawnGroup & tSpawnGroup = ptDefMonster->arMinionSpawnGroups[i];

            float flXPGroupMin = 0.0f;
            float flXPGroupMax = 0.0f;
            sGetMonsterXPForRareMinionSpawnGroup(tSpawnGroup, flXPGroupMin, flXPGroupMax);

            if (flXPGroupMin > 0.000001f && flXPGroupMin < flXPMin)

                flXPMin = flXPGroupMin;

            if (flXPGroupMax > 0.000001f && flXPGroupMax > flXPMax)

                flXPMax = flXPGroupMax;
        }
    }





    if (ptDefMonster->nMinionSpawnGroupCount == 0 || flXPMin >= FLT_MAX ||
        flXPMax <= FLT_MIN)
    {
        flXPMax = 0.0f;
        flXPMin = 0.0f;
    }


    flXPMax = flXPRareMonsterBase + flXPMax;
    flXPMin = flXPRareMonsterBase + flXPMin;
}
// Inference: the condition at 2054/2055 reads nMinionSpawnGroupCount from the (possibly null) definition after the loop; decompile reloads +0x490 there.

void DRLGGetMonsterXPForLevelAreaSpawnItem(
    const LevelAreaSpawnItem & tSpawnItem,
    float & flXPMin,
    float & flXPMax)
{
    flXPMin = 0.0f;
    flXPMax = 0.0f;

    if (tSpawnItem.snonameToSpawn.snoGroup == ACTOR_SNO)
    {
        ScopedMonsterDefinitionLockR ptDefMonster(ActorGetMonsterSNO(tSpawnItem.snonameToSpawn.snoHandle));
        if (ptDefMonster)
        {

            if (tSpawnItem.eMonsterRarity == MR_CHAMPION)

                sGetMonsterXPForChampionGroups(ptDefMonster->tHeader.snoHandle, flXPMin, flXPMax);
            else if (tSpawnItem.eMonsterRarity == MR_RARE)
                sGetMonsterXPForRareSpawnGroup(ptDefMonster->tHeader.snoHandle, flXPMin, flXPMax);
            else
            {

                flXPMin += tSpawnItem.nSpawnCountMin * ptDefMonster->pctAttributeModifiers[GBA_EXPERIENCE];
                flXPMax += tSpawnItem.nSpawnCountMax * ptDefMonster->pctAttributeModifiers[GBA_EXPERIENCE];
            }
        }


    }
}


void DRLGGetMonsterXPForLevelAreaSpawnGroup(
    const LevelAreaSpawnGroup & tSpawnGroup,
    float & flXPMin,
    float & flXPMax)
{
    flXPMin = 0.0f;
    flXPMax = 0.0f;

    for (int i = 0; i < tSpawnGroup.nSpawnItemCount; i++)
    {
        LevelAreaSpawnItem tSpawnItem = tSpawnGroup.arSpawnItems[i];

        float flXPMinItem;
        float flXPMaxItem;
        DRLGGetMonsterXPForLevelAreaSpawnItem(tSpawnItem, flXPMinItem, flXPMaxItem);

        flXPMin += flXPMinItem;
        flXPMax += flXPMaxItem;
    }

    flXPMin *= tSpawnGroup.nSpawnExactCount;
    flXPMax *= tSpawnGroup.nSpawnExactCount;
}
