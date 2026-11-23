// Original file: X/Server/Marker/SMarker.cpp

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommon.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Player.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/NPC.h"
#include "../../Common/InactiveMarker.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Definitions/QuestDefinition.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/PortalCommon.h"
#include "../Actors/Actors.h"
#include "../AI/ActorAI.h"
#include "../Powers/Powers.h"
#include "../ConversationMgr.h"
#include "../System/LevelArea.h"
#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../../Common/Hyperlink.h"
#include "../../Common/World.h"
#include "../../Common/Nav/NavCellHandle.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/Math/Rect.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SScene.h"
#include "../SWorld.h"
#include "../../Common/Definitions/SceneDefinition.h"
#include "../SGameConstants.h"
struct SpawnTrackInfo { SNO snoLevelArea; int32 nSpawnPopulation; int32 nSpawnGroup; int32 nSpawnItem; };  // size 0x10
#include "../../Common/ActorCommonDataSwig.h"
// X/Server/Marker/SMarker.h (not adopted yet)
enum DeactivateStatus {
    DEACTIVATE_YES = 0, DEACTIVATE_ONLY_WITH_SCENE = 1, DEACTIVATE_NO_PLAYER_SEEN = 2, DEACTIVATE_NO_TOWN_PORTAL = 3,
    DEACTIVATE_NO_PET = 4, DEACTIVATE_NO_ESCORT = 5, DEACTIVATE_NO_DOESNT_DEACTIVATE = 6, DEACTIVATE_NO_DELETING = 7,
    DEACTIVATE_NO_DYING = 8, DEACTIVATE_NO_COMPLEX_EFFECT = 9, DEACTIVATE_NO_HAS_TIMER = 10, DEACTIVATE_NO_UNDEFINED_GIZMO = 11,
    DEACTIVATE_NO_SCRIPT = 12, DEACTIVATE_NO_QUEUED_ACTIONS = 13, DEACTIVATE_NO_TALKING = 14, DEACTIVATE_NO_NOT_IN_SCENE = 15,
    DEACTIVATE_NO_UNINTERRUPTABLE_POWER = 16, DEACTIVATE_NO_JUST_CREATED = 17, DEACTIVATE_NO_ATTRIBUTE = 18,
};

// Declared here from their definitions elsewhere (headers not adopted yet).
PowerID ActorPowersGetPrimary(Actor * ptActor);  // X/Server/Actors/ActorPowers.cpp
BOOL SComplexEffectChainSeenRecently(const ActorCommonData & tACD);  // X/Server/SComplexEffect.cpp
BOOL LuaScriptInstanceIsACDInUse(ACDID idACD);  // X/Server/LuaScript
BOOL MarkerKeepActivatedWithScene(const ActorDefinition * pdefActor);  // X/Common/Marker/Marker.cpp
void QuestsLoad(SNO snoQuest, SNO snoLevelArea);  // X/Common/Quests/Quests.cpp
void QuestsRefreshQuestDialogs(SNO snoQuest);  // X/Common/Quests/Quests.cpp
void SQuestsSendUpdateToAllPlayers(SNO snoQuest);  // X/Server/Quests/SQuests.cpp
void SceneGetPhysicalAABBInWorldSpace(const SceneCommonData & tSCD, AABB & aabb);  // X/Common/SceneCommon.cpp
void NavMeshGetCenterPlace(const NavMeshHandle & tNavMeshHandle, WorldPlace & tPlaceCenter);  // NavigationMesh.cpp
BOOL SceneTestSpawnArea(SceneCommonData & tSCD, int32 nSquareIndex, float flRadius, BOOL bElite);  // X/Common/SceneCommon.cpp
uint16 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability);  // NavigationMesh.cpp
BOOL WorldCollSearchForLocation(const WorldPlace & tPlace, uint16 uStaticOneRequiredFlags, uint32 uDynamicFlagsNoOverlap, float flRadius, float flMinDist, float flMaxDist, ACDID idACDIgnore, Vector3D & vFoundLoc, NavCellHandle & hNavCell, int32 nParam, uint16 uStaticFlagsNone, ACDID idACDIgnore2);  // X/Common/WorldColl.cpp
struct ActorAddStruct; struct EncounterAddStruct;
void SpawnGroupInstantiate(const LevelAreaSpawnPopulation & tSpawnPop, int32 nSpawnPopIndex, const LevelAreaSpawnGroup & tSpawnGroup, SSceneID idSScene, uint32 dwActorAddFlags, LevelAreaID idLevelArea, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace, int32 nParam);
void SQuestsCreateMapMarker(const Marker & tMarker, const SSceneID idSScene);  // X/Server/Quests/SQuests.cpp
#include "../../Common/Definitions/MarkerSetDefinition.h"
// DWARF X/Common/InactiveMarker.cpp:1966.
void ActivateMarkerNames(const blz::vector<unsigned int> & aMarkerNames, blz::vector<ACDID> & aActivatedACDs, SNO snoMarkerSet, uint16 nAdventureParentIndex, uint32 nMarkerSetInstanceIndex);
#include "../../Common/Definitions/MonsterDefinition.h"
// DWARF X/Server/Actors/Actors.cpp:5915.
ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);








































static SpawnTrackInfo sgSpawnTrackInfo;




static void sSetSpawnLevelArea(SNO snoLevelArea)
{


    sgSpawnTrackInfo.snoLevelArea = snoLevelArea;
}






























static void sSpawnError(
    LPCSTR szMessage)
{
    ScopedLevelAreaDefinitionLockR ptLevelArea(sgSpawnTrackInfo.snoLevelArea);
    // Inference: unused diagnostic population reference follows the tracked population index.
    LevelAreaSpawnPopulation & tSpawnPop = ptLevelArea->ptServerData->arSpawnPopulations[sgSpawnTrackInfo.nSpawnPopulation];





}





static DeactivateStatus sMarkerCanDeactivateActorIgnorePacks(
    const ActorID idActor,
    ActorCommonData * ptACD)
{
    Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ptACD ? *ptACD : ACDGet(idACD);

    if (tACD.dwPlayerSeenBits != 0)
    {
        return DEACTIVATE_NO_PLAYER_SEEN;
    }


    if (ACDIsPlayer(tACD.id))
    {
        return DEACTIVATE_NO_PLAYER_SEEN;
    }

    GizmoType nGizmoType = tACD.GetGizmoType();
    if (nGizmoType == ACTORGIZMO_TOWN_PORTAL)
    {
        PortalResolveMethod eMethod = ActorGetInheritedTagMapValueInt(ptActor, TAG_ACTOR_GIZMO_PORTAL_RESOLVE_METHOD, TAG_MARKER_PORTAL_RESOLVE_METHOD);
        if (eMethod == PRM_SAVE_POINT_PORTAL)
        {
            return DEACTIVATE_NO_TOWN_PORTAL;
        }
    }





    if (tACD.IsPet())
    {
        return DEACTIVATE_NO_PET;
    }



    if (tACD.AttributesGetInt(ATTRIBUTE_NPC_IS_ESCORTING))
    {
        return DEACTIVATE_NO_ESCORT;
    }



    if (ptActor->dwActorFlags & (1 << ACTOR_DOESNT_DEACTIVATE_BIT))
    {
        return DEACTIVATE_NO_DOESNT_DEACTIVATE;
    }

    if (tACD.AttributesGetInt(ATTRIBUTE_NEVER_DEACTIVATES))
    {
        return DEACTIVATE_NO_ATTRIBUTE;
    }


    if (ptActor->dwActorFlags & ((1 << ACTOR_DELETE_AT_END_OF_UPDATE_BIT) | (1 << ACTOR_DELETING_BIT)))
    {
        return DEACTIVATE_NO_DELETING;
    }

    PowerID idPower = ActorPowersGetPrimary(ptActor);
    Power * ptPower = (idPower != -1) ? PowerTryToGet(idPower) : NULL;
    if (ptPower)
    {
        if (PowerIsUninterruptableDuring(&tACD, AutoPowerDefinitionR(ptPower->snoPower)))
        {
            return DEACTIVATE_NO_UNINTERRUPTABLE_POWER;
        }
    }

    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    if (ptActor->dwActorFlags & (1 << ACTOR_DYING))
    {


        if (!TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_CORPSE_DELETES_INSTEAD_OF_INACTIVE))
        {
            return DEACTIVATE_NO_DYING;
        }
    }



    if (SComplexEffectChainSeenRecently(tACD) == TRUE)
    {
        return DEACTIVATE_NO_COMPLEX_EFFECT;
    }



    if (tACD.dwTimerCount != 0)
    {
        return DEACTIVATE_NO_HAS_TIMER;
    }

    if (tACD.tSNOName.snoGroup == -1)
    {
        switch (tACD.hGB.eType)
        {
        case 17:  // GB_SCENERY
        case 2:  // GB_ITEMS
            break;
        case -1:  // GB_INVALID
            if (nGizmoType == ACTORGIZMO_UNDEFINED)
            {
                return DEACTIVATE_YES;
            }
            break;





        default:
            if (nGizmoType == ACTORGIZMO_UNDEFINED)
            {
                CRefString sActor = SNOToString(1, ptActor->snoActor, FALSE);  // 1: the Actor SNO group


                return DEACTIVATE_NO_UNDEFINED_GIZMO;
            }
            break;
        }
    }

    if (LuaScriptInstanceIsACDInUse(idACD))
    {
        return DEACTIVATE_NO_SCRIPT;
    }



    if (ActorIsUsingAIQueue(idActor))
    {
        return DEACTIVATE_NO_QUEUED_ACTIONS;
    }


    if (NPCIsAnyPlayerTalkingTo(tACD.id) != ACDID_INVALID)
    {
        return DEACTIVATE_NO_TALKING;
    }


    if (ConversationMgr::ACDIsInActiveConversation(tACD.id, NULL))
    {
        return DEACTIVATE_NO_TALKING;
    }


    uint32 dwNow = GameGetServerTime();
    if (ptActor->dwTimeCreated == dwNow)
    {
        return DEACTIVATE_NO_JUST_CREATED;
    }

    SSceneID idSScene = ACDGetCurrentSSceneID(tACD);
    if (idSScene == -1)
    {
        return DEACTIVATE_NO_NOT_IN_SCENE;
    }



    if (tACD.dwFlags & (1 << ACD_KNOWN_WITH_SCENE))
    {
        return DEACTIVATE_ONLY_WITH_SCENE;
    }

    BOOL bKeepWithScene = MarkerKeepActivatedWithScene(pdefActor);

    return bKeepWithScene ? DEACTIVATE_ONLY_WITH_SCENE : DEACTIVATE_YES;





}




static DeactivateStatus sMarkerCanDeactivateAllInPack(
    ACDPack & tACDPack)
{
    const ACDIDListPooled * listPack = tACDPack.GetMemberList();

    POS pos = listPack->GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listPack->GetNext(pos);

        DeactivateStatus nStatus = sMarkerCanDeactivateActorIgnorePacks((ActorID)ACDGetOwner(idACD),

                                                                        NULL);
        if (nStatus != DEACTIVATE_YES)
        {
            return nStatus;
        }
    }

    return DEACTIVATE_YES;
}




DeactivateStatus MarkerCanDeactivateActor(
    const ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);
    ACDID idACD = ptActor->idACD;
    ActorCommonData & tACD = ACDGet(idACD);

    ACDPack * ptACDPack = (tACD.idACDPack != -1) ? ACDPackTryToGet(tACD.idACDPack) : NULL;
    if (ptACDPack)
    {
        DeactivateStatus nStatus = sMarkerCanDeactivateAllInPack(*ptACDPack);
        return nStatus;
    }

    return sMarkerCanDeactivateActorIgnorePacks(idActor, &tACD);

}





void MarkerLoadQuestForServerGame(
    const SSceneID idSScene,
    const WorldPlace & placePosition,
    const Quaternion & qOrientation,
    const Marker & tMarker,
    const InactiveMarkerOwner & tMarkerOwner)
{



    if (!MarkerCheckReference(tMarkerOwner.snoMarkerSet, tMarkerOwner.nMarkerIndex, tMarker, 0x1f, tMarker.snoname.snoHandle))
    {
        return;
    }

    ScopedQuestDefinitionLockR pdefQuest(tMarker.snoname.snoHandle);


    if (GameIsOpenWorld() && pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST)
    {
        return;
    }

    SNO snoLevelArea = -1;
    if (!(pdefQuest->dwFlags & (1 << 3)))
    {


        NavMeshHandle tNavMeshHandle;
        ScenesCommonNavMeshNodeFromPlace(placePosition, &tNavMeshHandle, FALSE, FALSE);
        if (!NavMeshHandleIsNone(tNavMeshHandle))
        {




            LevelAreaID idLevelArea = NavMeshHandleGetLevelAreaID(tNavMeshHandle, FALSE);
            snoLevelArea = LevelAreaGetSNO(idLevelArea);
        }
    }



    QuestsLoad(tMarker.snoname.snoHandle, snoLevelArea);


    SQuestsSendUpdateToAllPlayers(tMarker.snoname.snoHandle);


    QuestsRefreshQuestDialogs(tMarker.snoname.snoHandle);


    if (pdefQuest->eQuestType == QUEST_TYPE_EVENT && (pdefQuest->dwFlags & 1))
    {


        SQuestsCreateMapMarker(tMarker, idSScene);
    }
}




enum { MAX_NODES_IN_DISTRIBUTION_ARRAY = 45000 };
// NON_MATCHING: 340 = 340, 8/85 words: only the order of the four zero-initialising movs before each unswitched loop copy (original sets nNodes first). Bit test is `(mask >> nBitIndex) & 1` (instcombine creates the shl at 499:15, t04); tried return instead of break.
static int32 sMarkerFillValidNodeArrayForDistribution(
    SSceneID idSScene,
    LevelAreaID idLevelArea,
    PassabilityType tPassabilityType,
    int32 * pnNavMeshIndexArray)
{


    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    NavMeshInst * ptNavMeshInst = tSCD.ptNavMeshInst;


    int32 nNodes = 0;
    int32 nDWordIndex = 0;
    int32 nBitIndex = 0;

    for (int32 nSquareIndex = 0; nSquareIndex < ptNavMeshInst->nNumSquares; nSquareIndex++)
    {
        BOOL bAllowSpawn = (tSCD.ptSpawnBitMask[nDWordIndex] >> nBitIndex) & 1;

        nBitIndex++;

        if (nBitIndex >= 32)
        {
            nBitIndex = 0;
            nDWordIndex++;
        }

        if (!bAllowSpawn)
            continue;



        NavMeshHandle tNavMeshHandle;
        tNavMeshHandle.nSquareIndex = nSquareIndex;
        tNavMeshHandle.idSScene = idSScene;



        bAllowSpawn = idLevelArea == -1 || NavMeshHandleGetLevelAreaID(tNavMeshHandle, TRUE) == idLevelArea;

        if (!bAllowSpawn)
            continue;


        pnNavMeshIndexArray[nNodes] = nSquareIndex;
        nNodes++;

        if (nNodes >= MAX_NODES_IN_DISTRIBUTION_ARRAY)
        {
            break;
        }

    }

    return nNodes;
}





static BOOL sDistributionPickPlace(
    SSceneID idSScene,
    NavMeshHandle & tNavMeshHandle,
    WorldPlace & tPlace,
    int32 * pnNavMeshIndexArray,
    int32 nNodes,
    float flRadius,
    BOOL bEnforceEliteSeparation)
{
    BOOL bPickedPlace = FALSE;

    // Inference: DWARF gives flRadius DW_AT_const_value 1.25 while both callers pass 0.0f; the original uses 1.25 for the offset, SceneTestSpawnArea and WorldCollSearchForLocation. Line unknown (no row); idiom as Gizmo.cpp:1477.
    flRadius = Math::Max(flRadius, 1.25f);



    const int32 MAX_TRIES_TO_PICK_PLACE = 60;
    const int32 MAX_EASY_OUT_TRIES_TO_PICK_PLACE = 40;



    AABB aabbBoundScene;
    SScene * ptSScene = SSceneGet(idSScene);

    ScopedSceneDefinitionLockR ptSceneDef(ptSScene->tSCD.snoScene);

    SceneGetPhysicalAABBInWorldSpace(ptSScene->tSCD, aabbBoundScene);

    int32 nTriesLeft = MAX_TRIES_TO_PICK_PLACE;
    int32 nEasyOutTriesLeft = MAX_EASY_OUT_TRIES_TO_PICK_PLACE;

    RNG RNGCopy = SGameGlobalsGet().rng;

    while (nEasyOutTriesLeft > 0 && nTriesLeft > 0)
    {
        int32 nRandIndex = RandRange_Internal(SGameGlobalsGet().rng, 0, nNodes - 1);

        tNavMeshHandle.nSquareIndex = pnNavMeshIndexArray[nRandIndex];
        tNavMeshHandle.idSScene = idSScene;
        NavMeshGetCenterPlace(tNavMeshHandle, tPlace);




        Vector3D wpOffset = tPlace.wp - aabbBoundScene.wp;

        wpOffset = Vector3D::Abs(wpOffset);
        wpOffset += Vector3D(flRadius, flRadius, flRadius);

        if (wpOffset.x >= aabbBoundScene.wvExt.x ||
            wpOffset.y >= aabbBoundScene.wvExt.y)
        {
            nEasyOutTriesLeft--;
            continue;
        }

        BOOL bNavMeshOk = SceneTestSpawnArea(ptSScene->tSCD, tNavMeshHandle.nSquareIndex, flRadius, FALSE);

        if (bEnforceEliteSeparation && bNavMeshOk)
        {
            bNavMeshOk = SceneTestSpawnArea(ptSScene->tSCD, tNavMeshHandle.nSquareIndex, flRadius, TRUE);
        }






        if (bNavMeshOk)
        {
            uint16 uStaticOneRequiredFlags = NavMeshGetStaticFlagsForPassability(4);
            uint32 uDynamicFlagsNoOverlap = 0x271;





            Vector3D vFoundLoc(tPlace.wp);
            NavCellHandle hNavCellTarget;
            BOOL bFoundDest = WorldCollSearchForLocation(tPlace, uStaticOneRequiredFlags, uDynamicFlagsNoOverlap, flRadius, 0.0f, 25.0f, ACDID_INVALID, vFoundLoc, hNavCellTarget, 1, 0, ACDID_INVALID);










            if (bFoundDest)
            {
                tPlace.wp = vFoundLoc;
                ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
                bPickedPlace = TRUE;
                break;
            }
        }







        nTriesLeft--;
    }

    SGameGlobalsGet().rng = RNGCopy;

    return bPickedPlace;
}

// DWARF: EncounterDefinition.cpp:384, Actors.cpp:6027, and SGame.cpp:8769.
BOOL EncounterResolveToActorSpawnInfo(SNO snoEncounter, RNG & tRNG, SNOName & tSNOName, Team & eTeam);
void EncounterAddInitialize(EncounterAddStruct & tEncounterAdd, GBHandleList & listGroupAffixes);
BOOL SGameIsBonusDoubledMonster(SNO snoActor);

static BOOL sActorAddInitializeForSpawnItem(
    ActorAddStruct & tActorAdd,
    const LevelAreaSpawnItem & tItem,
    SSceneID idSScene,
    uint32 dwActorAddFlags,
    LevelAreaID idLevelArea,
    NavMeshHandle & tNavMeshHandle,
    WorldPlace tPlace)
{
    BOOL bSuccess = TRUE;

    tActorAdd.Clear();




    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.eTeam = TEAM_MONSTER_0;
    tActorAdd.idMarkersSScene = idSScene;
    tActorAdd.tMarkersName = MarkerStringHash("DistributionMarker");
    tActorAdd.dwActorAddFlags = dwActorAddFlags | (1 << ACTORADD_USE_NEW_ENCOUNTER_PASSABILITY) | (1 << ACTORADD_IS_FROM_DISTRIBUTION);
    tActorAdd.eMonsterRarity = tItem.eMonsterRarity;
    tActorAdd.idLevelArea = idLevelArea;
    tActorAdd.tNavMeshHandle = tNavMeshHandle;
    tActorAdd.tPlace = tPlace;
    if (tItem.snonameToSpawn.snoGroup == ACTOR_SNO)
    {
        tActorAdd.tSNOName = tItem.snonameToSpawn;
    }
    else
    {


        bSuccess = EncounterResolveToActorSpawnInfo(tItem.snonameToSpawn.snoHandle, SGameGlobalsGet().rng, tActorAdd.tSNOName, tActorAdd.eTeam);
    }
    tActorAdd.snoActor = tActorAdd.tSNOName.snoHandle;

    return bSuccess;
}






static void sActorAddForSpawnItem(
    const LevelAreaSpawnPopulation & tSpawnPop,
    const LevelAreaSpawnItem & tItem,
    ActorAddStruct & tActorAdd,
    EncounterAddStruct & tEncounterAdd)
{
    int32 nSkipDistMin, nSkipDistDelta;

    nSkipDistMin = tItem.nSpawnSpreadMin;
    nSkipDistDelta = tItem.nSpawnSpreadMax - nSkipDistMin;

    if (nSkipDistDelta < 0)
    {
        CRefString sError;

        sError.Format("Spawn item has an invalid spread range: %d to %d\n", tItem.nSpawnSpreadMin, tItem.nSpawnSpreadMax);

        sSpawnError(sError);
        nSkipDistDelta = 0;

    }

    if (nSkipDistMin < 0)
    {
        CRefString sError;

        sError.Format("Spawn item has invalid min spawn spread: %d\n", tItem.nSpawnSpreadMin);

        sSpawnError(sError);
        nSkipDistMin = 0;

    }

    ActorPushSpawnSkipDistance(nSkipDistMin, nSkipDistDelta);



    int32 nSpawnCountMin, nSpawnCountDelta;
    nSpawnCountMin = tItem.nSpawnCountMin;
    nSpawnCountDelta = tItem.nSpawnCountMax - nSpawnCountMin;

    if (nSpawnCountDelta < 0)
    {
        CRefString sError;
        sError.Format("Spawn count has an invalid range: %d to %d\n", tItem.nSpawnCountMin, tItem.nSpawnCountMax);
        sSpawnError(sError);
        nSpawnCountDelta = 0;
    }

    if (nSpawnCountMin < 0)
    {
        CRefString sError;
        sError.Format("Spawn count has invalid min spawn count: %d\n", tItem.nSpawnCountMin);
        sSpawnError(sError);
        nSpawnCountMin = 0;
    }

    int32 nItemCount = RandRange_Internal(SGameGlobalsGet().rng, nSpawnCountMin, nSpawnCountDelta);
    BOOL bFarming = GameUsesMonsterDensityModifiers();

    if (tItem.snonameToSpawn.snoGroup == ACTOR_SNO &&
        tItem.eMonsterRarity == MR_NORMAL && bFarming)
    {

        float flItemCount = nItemCount * tItem.flFarmingUnitMult * tSpawnPop.flFarmingUnitMult;
        nItemCount = Math::FloatToInt(flItemCount) + (RandFloat_Internal(SGameGlobalsGet().rng) < Math::FloatFrac(flItemCount));
    }

    if (nItemCount > 1 && tActorAdd.eMonsterRarity == MR_RARE)
    {
        CRefString sError;

        sError.Format("Only 1 rare monster allowed per spawn group; (item specified allows %d to %d)\n", tItem.nSpawnCountMin, tItem.nSpawnCountMax);

        sSpawnError(sError);
        nItemCount = 1;

    }

    if (SGameIsBonusDoubledMonster(tActorAdd.snoActor))
        ++nItemCount;



    for (int32 i = 0; i < nItemCount; i++)
    {
        GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
    }

    ActorPopSpawnSkipDistance();
}





static BOOL sSpawnGroupExactInstantantiate(
    const SpawnGroupExactInstance & tInstance,
    const SceneCommonData & tSCD,
    LevelAreaID idLevelArea,
    int32 * pnNavMeshIndexArray,
    int32 nNodes,
    BOOL bEnforceEliteSeparation)
{


    SNO snoLevelAreaSpawn = tInstance.snoLevelAreaSpawn;
    if (snoLevelAreaSpawn == -1)
    {
        snoLevelAreaSpawn = LevelAreaGetLevelAreaForSpawns(tInstance.snoLevelAreaOrig);
    }
    ScopedLevelAreaDefinitionLockR ptLevelAreaDef(snoLevelAreaSpawn);
    if (nNodes < 1)
    {
        return FALSE;
    }








    if (tInstance.nPopIndex >= ptLevelAreaDef->ptServerData->nSpawnPopulationCount)
    {
        return FALSE;
    }





    const LevelAreaSpawnPopulation & tSpawnPop = ptLevelAreaDef->ptServerData->arSpawnPopulations[tInstance.nPopIndex];


    if (tInstance.nGroupIndex >= tSpawnPop.nSpawnGroupCount)
    {
        return FALSE;
    }





    sSetSpawnLevelArea(snoLevelAreaSpawn);



    const LevelAreaSpawnGroup & tSpawnGroup = tSpawnPop.arSpawnGroups[tInstance.nGroupIndex];
    uint32 dwActorAddFlags = 0x10;
    NavMeshHandle tNavMeshHandle;
    WorldPlace tPlace;
    float flGroupRadius = 0.0f;

    BOOL bPickedPlace = sDistributionPickPlace(tSCD.idSScene, tNavMeshHandle, tPlace, pnNavMeshIndexArray, nNodes, flGroupRadius, bEnforceEliteSeparation && tSpawnGroup.eType != 0);

    if (sgtAppGlobals.dwDebugFlags & (1 << 17))
    {
        CRefString sLevelArea = HyperlinkCreateFromSNO(SNOName(22, snoLevelAreaSpawn), NULL);
        CRefString sScene = HyperlinkCreateFromSNO(SNOName(33, tSCD.snoScene), NULL);



    }

    if (bPickedPlace)
    {
        SGame & tSGame = SGameManager::GameGet();
        if (!SWorldIsTieredDungeonFinder(tPlace.idSWorld) ||
            tSGame.ptSGameGlobals == NULL ||
            (tSGame.ptSGameGlobals->dwFlags2 & (3 << 8)) == 0)
        {



            SpawnGroupInstantiate(tSpawnPop, tInstance.nPopIndex, tSpawnGroup, tSCD.idSScene, dwActorAddFlags, idLevelArea, tNavMeshHandle, tPlace, -1);
        }


        return TRUE;
    }

    SNO snoWorld = WorldCommonGetWorldSNO(tSCD.idSWorld);
    if (!WorldIsDungeonFinderPlaceHolder(snoWorld))
    {



        CRefString strError("Could not pick a valid spot to put spawn group: ");
        CRefString strType;
        const LevelAreaSpawnItem & tItem = tSpawnGroup.arSpawnItems[0];
        if (tItem.snonameToSpawn.snoGroup == 15) // Original block order and string order: Encounter, Actor, Invalid (rodata 0xe1bdbc/0xe1bdcc/0xe1bdd8; 890:47 fits the Encounter literal).
        {
            strType.Format("Encounter [%s],", SNOToString(tItem.snonameToSpawn.snoGroup, tItem.snonameToSpawn.snoHandle, FALSE).str());
        }
        else if (tItem.snonameToSpawn.snoGroup == 1)
        {
            strType.Format("Actor [%s],", SNOToString(tItem.snonameToSpawn.snoGroup, tItem.snonameToSpawn.snoHandle, FALSE).str());
        }
        else
        {
            strType.Format("Invalid Group! [%s] is not an encounter or an actor!,", SNOToString(tItem.snonameToSpawn.snoGroup, tItem.snonameToSpawn.snoHandle, FALSE).str());
        }

        CRefString strRadius;
        strRadius.Format(" Radius = %.4f\n", flGroupRadius);

        strError += strType;
        strError += strRadius;

        sSpawnError(strError.str());


    }

    return FALSE;
}
// DWARF: SWorld.cpp:2206 and System/LevelArea.cpp:560.
BOOL SWorldSpawnGroupExactInstanceIsInScene(SSceneID idSScene, LevelAreaID idLevelArea, const SpawnGroupExactInstance & tSpawnGroupExactInstance);
int32 LevelAreaGetPopulationIndex(LevelAreaID idLevelArea);


static void sSpawnGroupDensityInstantantiate(
    SSceneID idSScene,
    LevelAreaID idLevelArea,
    const LevelAreaSpawnPopulation & tSpawnPop,
    int32 nSpawnPopIndex,
    const LevelAreaSpawnGroup & tSpawnGroup,
    int32 * pnNavMeshIndexArray,
    int32 nNodes)
{
    uint32 dwActorAddFlags = 1 << ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS;
    NavMeshHandle tNavMeshHandle;
    WorldPlace tPlace;
    float flGroupRadius = 0.0f;

    BOOL bPickedPlace = sDistributionPickPlace(idSScene, tNavMeshHandle, tPlace, pnNavMeshIndexArray, nNodes, flGroupRadius, FALSE);

    if (bPickedPlace)
    {
        SpawnGroupInstantiate(tSpawnPop, nSpawnPopIndex, tSpawnGroup, idSScene, dwActorAddFlags, idLevelArea, tNavMeshHandle, tPlace, -1);
    }
    else
    {
        sSpawnError("");  // Inference: DWARF inlines sSpawnError at 941 with no local message; the text leaves no bytes.


    }
}


static void sMarkersInstantiateForSpecialDistribution(
    SSceneID idSScene,
    LevelAreaID idLevelArea,
    int32 * pnNavMeshIndexArray,
    int32 nNodes)
{
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    SWorld * ptSWorld = SWorldGet(tSCD.idSWorld);
    SNO snoLevelArea = LevelAreaGetSNO(idLevelArea);

    SNO snoWorld = ptSWorld->tWCD.snoWorld;
    BOOL bEnforceEliteSeparation = WorldIsDungeonFinderPlaceHolder(snoWorld);


    POS pos = ptSWorld->listSpawnGroupFailed.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        const SpawnGroupExactInstance & tInstance = ptSWorld->listSpawnGroupFailed.GetNext(pos);
        if (snoLevelArea != SNO_NONE &&
            tInstance.snoLevelAreaOrig == snoLevelArea)
        {
            BOOL bSuccess = sSpawnGroupExactInstantantiate(tInstance, tSCD, idLevelArea, pnNavMeshIndexArray, nNodes, bEnforceEliteSeparation);
            if (bSuccess)
            {
                ptSWorld->listSpawnGroupFailed.RemoveAt(posLast);
            }
        }
    }

    pos = ptSWorld->listSpawnGroupExact.GetHeadPosition();
    while (pos)
    {
        const SpawnGroupExactInstance & tInstance = ptSWorld->listSpawnGroupExact.GetNext(pos);

        BOOL bInScene = SWorldSpawnGroupExactInstanceIsInScene(idSScene, idLevelArea, tInstance);
        if (!bInScene)
        {
            continue;
        }

        BOOL bSuccess = sSpawnGroupExactInstantantiate(tInstance, tSCD, idLevelArea, pnNavMeshIndexArray, nNodes, bEnforceEliteSeparation);
        if (!bSuccess)
        {
            ptSWorld->listSpawnGroupFailed.AddTail_Tracked(tInstance);
        }
    }
}




static void sMarkersInstantiateForDistributionDensity(
    SSceneID idSScene,
    LevelAreaID idLevelArea,
    int32 * pnNavMeshIndexArray,
    int32 nNodes)
{
    SNO snoLevelArea = LevelAreaGetSNO(idLevelArea);
    snoLevelArea = LevelAreaGetLevelAreaForSpawns(snoLevelArea);
    if (snoLevelArea == SNO_NONE)
    {
        return;
    }

    int32 nSpawnPop = LevelAreaGetPopulationIndex(idLevelArea);


    if (nSpawnPop == -1)
    {
        return;
    }

    const LevelAreaDefinition * ptLevelAreaDef = (const LevelAreaDefinition *)gptGroupDefLevelArea->LockR(snoLevelArea, FALSE);  // Inference: no DefinitionLockR/UnlockR DIE in the original here (direct group calls)

    if (ptLevelAreaDef->ptServerData->nSpawnPopulationCount == 0)
    {
        gptGroupDefLevelArea->UnlockR((void *&)ptLevelAreaDef);
        return;
    }



    const LevelAreaSpawnPopulation & tSpawnPop = ptLevelAreaDef->ptServerData->arSpawnPopulations[nSpawnPop];

    sSetSpawnLevelArea(snoLevelArea);

    // Inference: the original uses unity density when farming modifiers are disabled.
    float flSpawnDensityMult = 1.0f;
    if (GameUsesMonsterDensityModifiers())
    {
        flSpawnDensityMult = tSpawnPop.flFarmingDensityMult;
    }










    LevelArea & tLevelArea = LevelAreaGet(idLevelArea);
    POS posSpawnGroupIndexes = tLevelArea.listSpawnGroupIndexes.GetHeadPosition();
    while (posSpawnGroupIndexes)
    {
        int32 i = tLevelArea.listSpawnGroupIndexes.GetNext(posSpawnGroupIndexes);

        const LevelAreaSpawnGroup & tSpawnGroup = tSpawnPop.arSpawnGroups[i];

        if (tSpawnGroup.eSpawnDistribution == DISTRIBUTION_DENSITY &&
            tSpawnGroup.flSpawnDensity > 0.0f)
        {
            if (tSpawnGroup.flSpawnChance < 1.0f - EPSILON)
            {
                if (RandFloat_Internal(SGameGlobalsGet().rng) >= tSpawnGroup.flSpawnChance)
                {
                    continue;
                }
            }

            // Inference: density normalization uses original constants 1600 and 10.
            const float cflGridSizeSqr = 1600.0f;
            const float flMaxSpawnDensity = 10.0f;
            float flSpawnDensity = tSpawnGroup.flSpawnDensity * flSpawnDensityMult;

            if (flSpawnDensity > flMaxSpawnDensity)
            {
                CRefString sLevelArea = HyperlinkCreateFromSNO(SNOName(LEVELAREA_SNO, snoLevelArea), NULL);
                CRefString sScene = HyperlinkCreateFromSNO(SNOName(SCENE_SNO, SceneCommonDataGet(idSScene).snoScene), NULL);






                flSpawnDensity = flMaxSpawnDensity;
            }


            float flMeanPerNode = flSpawnDensity / cflGridSizeSqr;
            float flSpawnCount = flMeanPerNode * nNodes;
            int32 nSpawnCount = Math::FloatToInt(flSpawnCount);


            float flFraction = flSpawnCount - nSpawnCount;
            float flRand = RandFloat_Internal(SGameGlobalsGet().rng);

            if (flRand < flFraction)
            {
                nSpawnCount++;
            }

            if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_SPECIAL_MARKER_CREATION))
            {
                CRefString sLevelArea = HyperlinkCreateFromSNO(SNOName(LEVELAREA_SNO, snoLevelArea), NULL);
                CRefString sScene = HyperlinkCreateFromSNO(SNOName(SCENE_SNO, SceneCommonDataGet(idSScene).snoScene), NULL);



            }



            for (int32 j = 0; j < nSpawnCount; j++)
            {
                sSpawnGroupDensityInstantantiate(idSScene, idLevelArea, tSpawnPop, nSpawnPop, tSpawnGroup, pnNavMeshIndexArray, nNodes);
            }
        }
    }

    gptGroupDefLevelArea->UnlockR((void *&)ptLevelAreaDef);
}



// The spawn-chance test compares the rand first: the original reloads flSpawnChance after RandGetDWord (b.ge).
void MarkersInstantiateForDistribution(
    SSceneID idSScene)
{
    if (GameActiveGameCommonDataIsClient())
    {
        return;
    }

    uint32 dwGameFlags = SGameGlobalsGet().dwFlags;

    if (dwGameFlags & (1 << SGAMEGLOBALS_MONSTER_SPAWNING_DISABLED_BIT))
    {
        return;
    }

    SScene * ptSScene = SSceneGet(idSScene);
    SceneCommonData & tSCD = ptSScene->tSCD;

    if (!tSCD.ptNavMeshInst)
    {
        return;
    }

    for (int32 i = 0; i < 4; i++)
    {
        LevelAreaID idLevelArea = ptSScene->arLevelAreaIds[i];

        if (idLevelArea != LEVELAREAID_INVALID)
        {
            PassabilityType tPassabilityType = PASSABILITY_NEW_ENCOUNTER;
            int32 pnNavMeshIndexArray[MAX_NODES_IN_DISTRIBUTION_ARRAY];
            int32 nNodes = sMarkerFillValidNodeArrayForDistribution(idSScene, idLevelArea, tPassabilityType, pnNavMeshIndexArray);


            sMarkersInstantiateForSpecialDistribution(idSScene, idLevelArea, pnNavMeshIndexArray, nNodes);


            sMarkersInstantiateForDistributionDensity(idSScene, idLevelArea, pnNavMeshIndexArray, nNodes);
        }
    }
}




void MarkersInstantiateFailedDistribution(
    SceneCommonData & tSCD,
    BOOL bEnforceEliteSeparation)
{
    uint32 dwGameFlags = SGameGlobalsGet().dwFlags;
    if (dwGameFlags & 1)
    {
        return;
    }




    if (tSCD.ptNavMeshInst == NULL)
    {
        return;
    }



    SScene * ptSScene = SSceneGet(tSCD.idSScene);
    SWorld * ptSWorld = SWorldGet(tSCD.idSWorld);



    for (int32 i = 0; i < 4; i++)
    {
        LevelAreaID idLevelArea = ptSScene->arLevelAreaIds[i];
        if (idLevelArea != -1)
        {
            int32 pnNavMeshIndexArray[MAX_NODES_IN_DISTRIBUTION_ARRAY];
            SNO snoLevelArea = LevelAreaGetSNO(idLevelArea);
            POS pos = ptSWorld->listSpawnGroupFailed.GetHeadPosition();
            int32 nNodes = 0;
            BOOL bInitialized = FALSE;
            PassabilityType tPassabilityType = 4;

            while (pos)
            {
                POS posLast = pos;
                const SpawnGroupExactInstance & tInstance = ptSWorld->listSpawnGroupFailed.GetNext(pos);
                if (snoLevelArea != -1 && tInstance.snoLevelAreaOrig == snoLevelArea)
                {
                    if (!bInitialized)
                    {
                        nNodes = sMarkerFillValidNodeArrayForDistribution(tSCD.idSScene, idLevelArea, tPassabilityType, pnNavMeshIndexArray);
                        bInitialized = TRUE;
                    }

                    BOOL bSuccess = sSpawnGroupExactInstantantiate(tInstance, tSCD, idLevelArea, pnNavMeshIndexArray, nNodes, bEnforceEliteSeparation);
                    if (bSuccess)
                    {
                        ptSWorld->listSpawnGroupFailed.RemoveAt(posLast);
                    }
                }
            }
        }
    }
}
// NON_MATCHING: 1808 vs 1816 bytes; register allocation and shared cleanup scheduling differ; postfix loops and reversed null ternary unchanged.
int32 MarkerGetACDsLinkedByActor(
    const ActorID idActor,
    XPooledDataIDList & listACDs,
    LPCSTR pszGroupName)
{
    listACDs.RemoveAll();

    Actor * ptActor = ActorGet(idActor);
    if (ptActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
    {
        // Inference: absent marker ownership takes the original shared zero-result exit.
        return 0;
    }
    ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
    const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
    if (tMarker.nLinkCount == 0)
    {
        // Inference: a marker without links takes the shared zero-result cleanup exit.
        return 0;
    }
    blz::vector<int> aFoundArray;
    for (int32 i = 0; i < tMarker.nLinkCount; i++)
    {
        aFoundArray.push_back(0);
    }

    uint32 tGroupName = (pszGroupName == NULL) ? 0 : MarkerStringHash(pszGroupName);

    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        Actor * ptLinkActor = tIt.ptElement;

        if (ptActor->tMarkerOwner.snoMarkerSet == ptLinkActor->tMarkerOwner.snoMarkerSet &&
            ptActor->tMarkerOwner.nAdventureParentIndex == ptLinkActor->tMarkerOwner.nAdventureParentIndex &&
            ptActor->tMarkerOwner.nMarkerSetInstanceIndex == ptLinkActor->tMarkerOwner.nMarkerSetInstanceIndex)
        {
            const Marker & tLinkMarker = pdefMarkerSet->tMarkerSet[ptLinkActor->tMarkerOwner.nMarkerIndex];
            for (int32 i = 0; i < tMarker.nLinkCount; i++)
            {
                const MarkerLink & tLink = tMarker.ptMarkerLinks[i];
                if (tLink.dwHash == tLinkMarker.dwHash &&
                    (tGroupName == 0 || tLink.dwGroupHash == tGroupName))
                {
                    listACDs.AddTail_Tracked(ptLinkActor->idACD);
                    aFoundArray[i] = 1;
                }
            }
        }
    }


    blz::vector<unsigned int> aLinkedMarkersToActivate;
    for (int32 i = 0; i < tMarker.nLinkCount; i++)
    {
        const MarkerLink & tLink = tMarker.ptMarkerLinks[i];
        if (!aFoundArray[i] &&
            (tGroupName == 0 || tLink.dwGroupHash == tGroupName))
        {
            aLinkedMarkersToActivate.push_back(tLink.dwHash);
        }
    }

    blz::vector<ACDID> aActivatedMarkerACDResults;

    ActivateMarkerNames(aLinkedMarkersToActivate, aActivatedMarkerACDResults, ptActor->tMarkerOwner.snoMarkerSet, ptActor->tMarkerOwner.nAdventureParentIndex, ptActor->tMarkerOwner.nMarkerSetInstanceIndex);

    size_t nActivatedACDs = aActivatedMarkerACDResults.size();
    for (uint32 i = 0; i < nActivatedACDs; i++)
    {
        ACDID idACD = aActivatedMarkerACDResults[i];
        if (idACD != ACDID_INVALID)
        {
            listACDs.AddTail_Tracked(idACD);
        }
    }

    return listACDs.Count();
}



void sSpawnChampionPack(
    ActorAddStruct & tActorAdd,
    EncounterAddStruct & tEncounterAdd)
{
    SNO snoActorBase = tActorAdd.snoActor;
    if (snoActorBase == SNO_NONE)
    {
        CRefString sError;
        sError.Format("sSpawnChampionPack had bad actor!");
        sSpawnError(sError); // Inference: failure returns through the original shared cleanup tail.
        return;
    }

    ScopedActorDefinitionLockR pdefActor(snoActorBase);
    if (pdefActor->snoMonster == SNO_NONE)
    {
        CRefString sError;
        sError.Format("sSpawnChampionPack actor (%s) has invalid monster SNO!", SNOToString(ACTOR_SNO, tActorAdd.snoActor, FALSE).str());
        sSpawnError(sError); // Inference: failure returns through the original shared cleanup tail.
        return;
    }

    ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);

    if (pdefMonster->nChampionSpawnGroupCount <= 0)
    {
        CRefString sError;
        sError.Format("sSpawnChampionPack actor (%s) has no champion group data!", SNOToString(ACTOR_SNO, tActorAdd.snoActor, FALSE).str());
        sSpawnError(sError); // Inference: failure returns through the original shared cleanup tail.
        return;
    }

    float flWeightTotal = 0.0f;
    for (int32 i = 0; i < pdefMonster->nChampionSpawnGroupCount; i++)
    {
        const MonsterChampionSpawnGroup & tSpawnGroup = pdefMonster->arChampionSpawnGroups[i];
        flWeightTotal += tSpawnGroup.flWeight;
    }

    if (flWeightTotal < EPSILON)
    {
        CRefString sError;
        sError.Format("sSpawnChampionPack actor (%s) has a total of zero weights!", SNOToString(ACTOR_SNO, tActorAdd.snoActor, FALSE).str());
        sSpawnError(sError); // Inference: failure returns through the original shared cleanup tail.
        return;
    }

    float flRandValue = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, flWeightTotal); // Inference: the original accumulator starts at zero before selecting the winning group.
    float flCurrentWeight = 0.0f;
    const MonsterChampionSpawnGroup * ptWinningGroup = NULL;
    for (int32 i = 0; i < pdefMonster->nChampionSpawnGroupCount; i++)
    {
        const MonsterChampionSpawnGroup & tSpawnGroup = pdefMonster->arChampionSpawnGroups[i];
        if (tSpawnGroup.flWeight > EPSILON)
        {
            flCurrentWeight += tSpawnGroup.flWeight;
            if (flRandValue <= flCurrentWeight)
            {
                ptWinningGroup = &tSpawnGroup;
                break;
            }
        }
    }

    if (!ptWinningGroup)
    {
        CRefString sError;
        sError.Format("sSpawnChampionPack actor (%s) could not find winning group!", SNOToString(ACTOR_SNO, tActorAdd.snoActor, FALSE).str());
        sSpawnError(sError); // Inference: failure returns through the original shared cleanup tail.
        return;
    }


    if (ptWinningGroup->nSpawnItemCount <= 0)
    {
        CRefString sError;
        sError.Format("sSpawnChampionPack actor (%s) had 0 spawn items in a champion group!", SNOToString(ACTOR_SNO, tActorAdd.snoActor, FALSE).str());
        sSpawnError(sError); // Inference: failure returns through the original shared cleanup tail.
        return;
    }

    for (int32 i = 0; i < ptWinningGroup->nSpawnItemCount; i++)
    {
        int32 nSkipDistMin = 0;
        int32 nSkipDistDelta = 0;
        ActorPushSpawnSkipDistance(nSkipDistMin, nSkipDistDelta);
        const MonsterChampionSpawnItem & tSpawnItem = ptWinningGroup->arSpawnItems[i];
        if (tSpawnItem.snoActor != SNO_NONE)
        {
            tActorAdd.tSNOName.snoHandle = tSpawnItem.snoActor;
            for (int32 j = 0; j < tSpawnItem.nSpawnCount; j++)
            {
                GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
            }
        }

        ActorPopSpawnSkipDistance();
    }

    tActorAdd.snoActor = snoActorBase;
}



// NON_MATCHING: 1300 vs 1304 bytes; on DWARF lines; only the prologue differs: the two hoisted EncounterResolve argument addresses (&tActorAdd.tSNOName, &tActorAdd.eTeam) are stored with one stp where the original uses two str. Tried the || form of the item filter.
void SpawnGroupInstantiate(
    const LevelAreaSpawnPopulation & tSpawnPop,
    int32 nSpawnPopIndex,
    const LevelAreaSpawnGroup & tSpawnGroup,
    SSceneID idSScene,
    uint32 dwActorAddFlags,
    LevelAreaID idLevelArea,
    NavMeshHandle & tNavMeshHandle,
    WorldPlace & tPlace,
    int32 nOnlySpawnItem)
{

















    for (int32 i = 0; i < tSpawnGroup.nSpawnItemCount; ++i)
    {
        LevelAreaSpawnItem & tItem = tSpawnGroup.arSpawnItems[i];

        if (tItem.eMonsterRarity == MR_RARE_MINION)
        {
            continue;
        }

        if (nOnlySpawnItem != -1 && i != nOnlySpawnItem)
        {
            continue;
        }



        ActorAddStruct tActorAdd;
        EncounterAddStruct tEncounterAdd;
        GBHandleList listGroupAffixes(GBGetHandlePool());

        EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

        if (sActorAddInitializeForSpawnItem(tActorAdd, tItem, idSScene, dwActorAddFlags, idLevelArea, tNavMeshHandle, tPlace))
        {
            tActorAdd.eTeam = tSpawnGroup.eTeam;

            if (tItem.eMonsterRarity == MR_CHAMPION)
            {
                sSpawnChampionPack(tActorAdd, tEncounterAdd);
                break;
            }

            sActorAddForSpawnItem(tSpawnPop, tItem, tActorAdd, tEncounterAdd);

            if (tItem.eMonsterRarity == MR_RARE)
            {
                for (int32 j = 0; j < tSpawnGroup.nSpawnItemCount; ++j)
                {
                    LevelAreaSpawnItem & tItemMinion = tSpawnGroup.arSpawnItems[j];

                    if (tItemMinion.eMonsterRarity == MR_RARE_MINION)
                    {


                        if (sActorAddInitializeForSpawnItem(tActorAdd, tItemMinion, idSScene, dwActorAddFlags, idLevelArea, tNavMeshHandle, tPlace))
                        {
                            tActorAdd.eTeam = tSpawnGroup.eTeam;

                            sActorAddForSpawnItem(tSpawnPop, tItemMinion, tActorAdd, tEncounterAdd);
                        }
                    }
                }
            }
        }
    }
}
