#include "../Common/ActorCommonData.h"
#include "../Common/Gizmo.h"
#include "../Common/Player.h"
#include "../Common/WorldPlace.h"
#include "LuaScript/ScriptEvent.h"
#include "Map/Minimap.h"
#include "Timers.h"
#include "SGame.h"
#include "../Common/AnimPlayer.h"
#include "../Common/GameCommonData.h"
#include "../Common/GizmoSwig.h"
#include "../Common/SceneCommon.h"
#include "../Common/NephalemRiftCommon.h"
#include "../Common/Networking/GameMessage.h"
#include "PlayerList.h"
#include "Actors/Actors.h"
#include "Powers/PowerAPI.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Common/TagMaps/Tags.h"
#include "SWorld.h"
#include "SPlayer.h"
#include "../Common/Shell/CmdLine.h"
#include "../Sigma/MemoryPools.h"
#include "Actors/ActorEffect.h"
#include "../Common/Actors/ActorEffectCommon.h"
#include "../Common/Definitions/HeroDefinition.h"
#include "../Common/TagMaps/AnimTagsSwig.h"
#include "../Common/TagMaps/MarkerTags.h"
#include "../Common/Definitions/MarkerSetDefinition.h"
#include "../Common/Marker/Marker.h"
#include "../Common/Powers/PowersCommon.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/ActorInventory.h"
#include "../Common/Items.h"
#include "System/LevelArea.h"
#include "AI/BrainComponents.h"
#include "Loot/Loot.h"
#include "../Common/Definitions/SceneDefinition.h"
void SPlayerSetCheckpoint(Player & tPlayer, const CheckpointData & tCheckpoint);  // X/Server/SPlayer.cpp:10893
void GizmoSendHearthPortalToPlayer(Player & tPlayer, ACDID idACDPortal);  // X/Server/SGizmo.cpp:3508
namespace Minimap {
void AddStaticGizmoMarker(ActorCommonData & tACDGizmo);  // X/Server/Map/Minimap.cpp
}
#include "Achievements/AchievementModifierEvaluationLogic.h"
static void sSpawnEncounter(ActorCommonData & tACDGizmo);
static void sCheckForTrap(ActorCommonData & tACD, ActorCommonData & tACDKiller);
BOOL STimedEventHasExpired(const SNO snoTimedEvent);  // X/Server/STimedEvents.cpp
void GizmoReturnPointPortalInitialize(ActorCommonData & tACD);  // X/Server/SGizmo.cpp:3373
BOOL SWorldIsDungeonFinder(SWorldID idSWorld);  // X/Server/SWorld.cpp:3304
SNO ScenesCommonLevelAreaFromPlace(SSceneID idSScene, Vector3D vPosition, BOOL fProcessInactive);  // X/Common/SceneCommon.cpp:1295
void SWorldPlayerWarpToLastSavepoint(const Player & tPlayer);  // X/Server/SWorld.cpp:983
BOOL SWorldPlayerWarpToWaypoint(const ACDID idACDPlayer, const ACDID idACDWaypoint, const int32 nWaypointNumber);  // X/Server/SWorld.cpp:816
void SendPlayerCancelWarp(ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp
void SQuestsOnPlayerInteractWithGizmo(ACDID idACDPlayer, ACDID idACDGizmo);  // X/Server/Quests/SQuests.cpp
void SGizmoAddStaticGizmoPing(ActorCommonData & tACDGizmo);  // X/Server/SGizmo.cpp:3540
void ExperienceDropLootWithinExpRange(ActorCommonData & tACDLootee, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist);  // X/Server/Experience.cpp:1149
BOOL SPlayerSetSavePoint(Player & tPlayer, const SavePointData & tSavePoint);  // X/Server/SPlayer.cpp:10938
BOOL SWorldResolveLocationFromSavePointData(const SavePointData * ptData, WorldPlace & placeLocation);  // X/Server/SWorld.cpp:1697
void ExperienceDropLootForAll(ActorCommonData & tACDLootee, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist);  // X/Server/Experience.cpp
void SPlayerAddNephalemBuffStack(ACDID idACDPlayer);  // X/Server/SPlayer.cpp:12334
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName);  // X/Server/Marker/SMarker.cpp:1231
void ActorRemoveWaypointFromInactiveList(const ActorID idActor);  // X/Server/Actors/Actors.cpp:7085
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // X/Common/Act.cpp:156
void SceneGetSceneToWorldTransform(SSceneID idSScene, PRTransform & transformSceneToWorld);  // X/Common/ScenesCommon.cpp:1620
void PowerSetCoolDown(ActorCommonData & tACD, const uint32 dwCooldown);  // X/Server/Powers/Powers.cpp:1715
namespace BossEncounterManager { BOOL CanTriggerEncounter(SNO snoEncounter); }  // X/Server/System/BossEncounter.cpp:983
static BOOL sGateIsLocked(const ActorCommonData & tACDGate);
static void sGateUpdate(ActorCommonData & tACDGate);
void ACDWarpToACD(ActorCommonData & tACD, const ActorCommonData & tACDTarget, int32 nUnused, float flRadius);
void BrainAPIDoResurrect(ActorID idActor, BOOL bUnused);
void ServerMessageSendForceInterfaceClose(ACDID idACD);
#include "../Common/Nav/NavigationMesh.h"
#include "../Common/Actors/ActorCollision.h"
#include "System/SChallengeRift.h"
#include "../Common/Achievements/AchievementDefines.h"
#include "System/SNephalemRift.h"
#include "../Common/Quests/Quests.h"
#include "SActTransitions.h"
#include "../Client/Console/Online.h"
BOOL ACDCollCollideCircle(SWorldID idSWorld, const Circle & wc, const ACDCollParams & tCollParams, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp:1207
BOOL ACDCollCollideOBB(SWorldID idSWorld, const OBB & tOBB, const ACDCollParams & tCollParams, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // X/Common/ActorCommonCollision.cpp:1228
static void sHandleChestSpawningBehavior(const ActorDefinition * pdefActor, ActorCommonData & tACD, ActorCommonData * ptACDOperator, TimerFunc pfuncLootCallback);  // X/Server/SGizmo.cpp:440
BOOL LootDropLoot(ActorCommonData & tACDLootee, const ACDID idACDLooter, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot, BOOL bSuppressUnluckyBonus);  // X/Server/Loot/Loot.cpp:991
void AnimPlayerSendAnimationToPlayer(ActorCommonData & tACD, uint32 dwAnimationTag, uint32 dwLength, const AnimReason eAnimReason, const Player & tPlayer);  // X/Common/AnimPlayer.cpp:1610
static void sGizmoUpdateStateCharges(ActorCommonData & tACD);  // X/Server/SGizmo.cpp:723
void GrantRestExperience(ActorCommonData & tACDPlayer);  // X/Server/Experience.cpp:794
int32 CountSetBits(const uint8 * lpb, const int32 nLen);  // X/Sigma/BitManip.cpp:87
BOOL CValidateIsACDValidItemInTheWorld(const ACDID idACDItem);  // X/Server/Network/ClientValidation.cpp:1044
BOOL CValidatePlayerCloseEnoughToPickupItem(const ActorCommonData & tACD0, const ActorCommonData & tACD1);  // X/Server/Network/ClientValidation.cpp:473
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:317
BOOL FlippyFindLandingLocation(const ActorCommonData & tACDLaunching, const SNO snoActorFlippy, WorldPlace & tPlaceLanding, const Player * ptAssignedPlayer, const Vector3D * pvBias, ACDID idACDKiller);  // X/Server/Flippies.cpp:831
void ACDRemoveToLimbo(ActorCommonData & tACDItem);  // X/Common/ActorInventory.cpp:3273
void FlippyDropCreateOnActor(const ACDID idACDDropper, const ACDID idACDItem, const WorldPlace & tPlace);  // X/Server/Flippies.cpp:1180
static void sPlayPickupError(Player * ptPlayer, PickupResponse ePickupResponse, ACDID idACDGizmo);  // X/Server/SGizmo.cpp:1287
BOOL SQuestsHasQuestItemBeenFound(GBID gbidItemRequired);  // X/Server/Quests/SQuests.cpp:1281
SNO ActorInventory_GetItemUseSound(const ActorCommonData * ptACDItem);  // X/Common/ActorInventory.cpp:12348
BOOL SGizmoConsumeItemToOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator);  // X/Server/SGizmo.cpp:2528
BOOL ACDIsTrackedInOpenWorld(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9253
void HearthPortalUse(const ActorCommonData & tACDPortal, ActorCommonData & tACDPlayer);  // X/Server/TownPortal.cpp:394
BOOL LootGetLootSpecifiers(const LooteeInfo & tLootee, const ACDID idACDLooter, LootSpecifierArray & arSpecifiers, const TreasureClassDefinition * pdefTreasureClass, const BOOL bIgnoreQuestItems, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot);  // X/Server/Loot/Loot.cpp
ACDID LootCreateItem(LootSpecifier & tSpecifier, const WorldPlace & tPlace, const ActorCommonData * ptACDSource);  // X/Server/Loot/Loot.cpp
void STrickleUpdateActor(const ActorCommonData & tACD);  // X/Server/Network/STrickle.cpp:305
BOOL SWorldIsSetDungeon(SWorldID idSWorld);  // X/Server/SWorld.cpp:3338
void OnSavePointPortalReturnDeleted(ActorCommonData & tACDPortal);  // X/Server/SGizmo.cpp:3460

LevelArea & LevelAreaGet(LevelAreaID idLevelArea);  // X/Server/System/LevelArea.cpp:486
uint32 StringHash(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:58
ActorID GBAdd(ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp
BOOL EncounterResolveToActorSpawnInfo(SNO snoEncounter, RNG & rng, SNOName & tSNOName, int32 & eTeam);  // X/Common/Definitions/EncounterDefinition.cpp
LevelAreaID NavMeshHandleGetLevelAreaID(const NavMeshHandle & tNavMeshHandle, BOOL bForDistribution);  // X/Common/Nav/NavigationMesh.h

static InventorySlotType geGizmoConsumedItemSlotsToCheck[15] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 11, 15, -1 };
void SecretPortalOperate(const ACDID idACDPortal, const ACDID);  // X/Server/TownPortal.cpp:491
namespace BossEncounterManager { void TriggerEncounter(SNO snoEncounter, Player * ptPlayer, SWorldID idSWorldOrigin, BOOL bForceImmediateStart); }  // X/Server/System/BossEncounter.cpp:1087
void SWorldPlayerWarpToReturnPoint(const ACDID idACDPlayer);  // X/Server/SWorld.cpp:996
BOOL SPlayerTryIdentifyAllWithCast(ACDID idACD, ACDID idGizmoTarget);  // X/Server/SPlayer.cpp:12358
BOOL SWorldDungeonFinderIsEmpty();  // X/Server/SWorld.cpp:3092
BOOL WorldIsDungeonFinderWorld(const SNO snoWorld);  // X/Common/World.cpp:886

void SGizmoOperated(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator);  // X/Server/SGizmo.cpp:2489
static void sGizmoSendShrineMessage(ActorCommonData & tACDShrine, ActorCommonData & tACDOperator);  // X/Server/SGizmo.cpp:887 (called at 674)























void GizmoSetDisabled(ActorCommonData & tACD, BOOL bDisabled)
{
    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_DISABLED_BY_SCRIPT, bDisabled);
}
#include "System/SetEncounter.h"
BOOL SWorldHasBeenEntered(SNO snoWorld);  // X/Server/SWorld.cpp:3962
BOOL PlayerIsPlayerGameFlagSet(const Player * ptPlayer, LPCSTR szLabel);  // X/Common/Player.cpp:3461
void SendPlayerWarping(const ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp:1433
BOOL PortalUse(ACDID idACDPortal, ActorCommonData & tACDOperator, const PlayerWarpReason eWarpReason);  // X/Server/TownPortal.cpp:296
namespace Minimap { void RevealPortalMarker(const Actor * ptPortal); }  // X/Server/Map/Minimap.cpp:728
static BOOL sGizmoPollAndSetTimer(
    ActorCommonData & tACD)
{
    uint32 dwTimer = tACD.AttributesGetInt(ATTRIBUTE_DOOR_TIMER);

    uint32 dwNow = GameGetServerTime();

    if (dwTimer != (uint32)-1 && dwTimer > dwNow)
    {
        return FALSE;
    }



    const int32 ACTIVATE_TIMEOUT = 45;
    tACD.AttributesSetInt(ATTRIBUTE_DOOR_TIMER, dwNow + ACTIVATE_TIMEOUT);
    return TRUE;
}




const uint32 PORTAL_COOLDOWN_TICKS = 30;
static void sSetPortalCooldown(
    ActorCommonData & tACDPlayer)
{
    uint32 uNow = GameGetServerTime();
    tACDPlayer.AttributesSetInt(ATTRIBUTE_PORTAL_NEXT_TIME, uNow + PORTAL_COOLDOWN_TICKS);
}




void sDoorRemoveCollision(TimerID, DataID, void * ptACD)
{
    ActorCommonData & tACD = *(ActorCommonData *)ptACD;
    ScriptEventOnDoorCompletelyOpened(tACD.id);
    tACD.EnableCollision(FALSE);
}








static void sGizmoOperateDoor(
    ActorCommonData & tACD,
    const BOOL bForceOperate)
{
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACD, tNavMeshHandle);

    if (NavMeshHandleIsNone(tNavMeshHandle))
    {
        return;  // inferred: no own row; the cbnz at line 210 branches to the helper exit
    }


    BOOL bReady = sGizmoPollAndSetTimer(tACD);

    if (!bForceOperate && !bReady)
    {
        return;  // inferred: no own row; line 218 branches to the helper exit
    }


    if (!bForceOperate)
    {
        if (GizmoDoorIsLocked(tACD))
        {
            return;  // inferred: no own row; line 226 cbnz goes to the helper exit
        }
    }

    BOOL bOpen = GizmoDoorIsOpen(tACD);
    if (bOpen)
    {
        if (!bForceOperate && !GizmoPlayerCanCloseDoor(tACD))
        {
            return;  // inferred: no own row; line 235 cbz goes to the helper exit
        }


        tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 0);
        // initializer inferred: no row; the switch's default path (shape not 1/2) reaches the close code at 293
        BOOL bDoorCanClose = TRUE;

        const WorldLocation & tLoc = tACD.GetWorldLocation();
        const ACDCollSettings & tACDCollSettings = tACD.tCollSettings;
        ACDCollParams tCollParams;

        tCollParams.eTeam = tACDCollSettings.eTeam;
        tCollParams.uObjectNoOverlapFlags = tACDCollSettings.uDynamicFlagsNoOverlap;

        if (tACDCollSettings.eCollisionShape == COLLISION_SHAPE_CYLINDER)
        {


            ACDCollNarrowResult tResult;
            const AxialCylinder & tCyl = tACDCollSettings.GetCyl();
            Circle wc;

            wc.wpCenter.Set(tCyl.wpBase.x, tCyl.wpBase.y);
            wc.wdRadius = tCyl.wdRadius;

            bDoorCanClose = !ACDCollCollideCircle(tLoc.idSWorld, wc, tCollParams, tACD.id, tResult);
        }




        // inferred: the compare against 2 is merged into line 252 (SimplifyCFG turns the chain into one switch)
        else if (tACDCollSettings.eCollisionShape == COLLISION_SHAPE_BOX)
        {
            ACDCollNarrowResult tResult;
            const OBB & tOBB = tACDCollSettings.GetOBB();

            bDoorCanClose = !ACDCollCollideOBB(tLoc.idSWorld, tOBB, tCollParams, tACD.id, tResult);
        }






        // inferred: no own row; collide hit -> 286, miss/default -> 293
        if (!bDoorCanClose)
        {
            tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);
            return;  // inferred: no own row; line 286 ends with b to the helper exit
        }




        tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 0);
        AnimPlayerPlayAnimation(tACD, TAG_ACTOR_ANIMATION_CLOSING, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_NEUTRAL);

        tACD.EnableCollision(TRUE);
    }
    else
    {

        RNG * prng = &SGameGetRNG();
        AnimationTiming tAnimationTiming;
        AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_OPENING, ANIM_USE_ANIM_LENGTH, 1.0f, *prng, tAnimationTiming);

        if (GizmoPlayerCanCloseDoor(tACD))
        {
            ScriptEventOnDoorCompletelyOpened(tACD.id);
            tACD.EnableCollision(FALSE);
        }
        else
        {

            uint32 dwDelay = tAnimationTiming.dwExecuteTime[0];
            TimerRegister(tACD.id, TIMER_OWNER_ACD, dwDelay, &tACD, sDoorRemoveCollision);
        }


        tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);


        tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);

        AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, FALSE, ANIM_REASON_GIZMO, FALSE, FALSE);

        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
    }
}




void sChestMonsterSpawnerAnimCallback(TimerID, DataID, void * pACD)
{
    ActorCommonData & tACD = *(ActorCommonData *)pACD;
    sSpawnEncounter(tACD);
}







static void sCheckForTrap(ActorCommonData & tACD, ActorCommonData & tACDOperator)
{


    ScopedActorDefinitionLockR pdefActor(tACD.GetActorSNO());


    Target tTarget;
    tTarget.InitializeFromACD(tACDOperator.id, FALSE, FALSE);
    SNO snoSkill = tACD.AttributesGetPower(0);
    if (snoSkill != SNO_NONE)
    {
        float flPercentChanceToTriggerPower = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_CHEST_CHANCE_TO_TRIGGER_TRAP);

        float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);

        BOOL bTriggerPower = flRandom < flPercentChanceToTriggerPower;
        if (bTriggerPower)
        {
            if (!PowerWantsActorTarget(AutoPowerDefinitionR(snoSkill)))
            {

                WorldPlace wpTarget;
                if (tTarget.GetPlace(wpTarget))
                {
                    tTarget.InitializeFromPlace(wpTarget);
                }
            }


            tACD.SetTeam(TEAM_MONSTER_FRIEND);
            tACD.SetTargetable(FALSE);
            PowerStart(tACD.id, snoSkill, tTarget);
        }
    }
}


void sChestSpawnLootCallback(TimerID, DataID, void * pACD)
{
    ActorCommonData & tACD = *(ActorCommonData *)pACD;



    ACDID idACDOperator = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_GIZMO_OPERATOR_ACDID);
    ActorCommonData * ptACDOperator = ACDTryToGet(idACDOperator);
    if (ptACDOperator != NULL && !ACDIsInTieredLootRun(*ptACDOperator))
    {
        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
        SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
        if (snoTreasureClass != SNO_NONE)
        {
            float flCameraBiasDist = tACD.GetRadius() + 7.0f;
            ExperienceDropLootForAll(tACD, idACDOperator, (uint32)-1, FALSE, flCameraBiasDist);
        }
        if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHEST_GRANTS_NV_STACK_ON_OPERATION))
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                SPlayerAddNephalemBuffStack(ptPlayer->idPlayerACD);
            }
        }
        sCheckForTrap(tACD, *ptACDOperator);
    }
}







void sPowerUpSpawnLootCallback(TimerID, DataID, void * pACD)
{
    ActorCommonData & tACD = *(ActorCommonData *)pACD;



    ACDID idACDOperator = (ACDID)tACD.AttributesGetInt(ATTRIBUTE_GIZMO_OPERATOR_ACDID);
    ActorCommonData * ptACDOperator = ACDTryToGet(idACDOperator);
    if (ptACDOperator != NULL)
    {
        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
        SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
        if (snoTreasureClass != SNO_NONE)
        {
            ExperienceDropLootWithinExpRange(tACD, idACDOperator, (uint32)-1, FALSE, 0.0f);
        }
    }
}





static void sHandleChestSpawningBehavior(
    const ActorDefinition * pdefActor,
    ActorCommonData & tACD,
    ActorCommonData * ptACDOperator,
    TimerFunc pfuncLootCallback)
{
    AnimationTiming tAnimationTiming;
    AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_OPENING, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);
    uint32 dwContactDelay = tAnimationTiming.dwExecuteTime[0];
    AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, FALSE, ANIM_REASON_GIZMO, FALSE, FALSE);

    float flPercentChanceToSpawnMonster = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHANCE_TO_SPAWN_MONSTER);
    float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);
    BOOL fSpawnMonster = flRandom < flPercentChanceToSpawnMonster;

    BOOL bSynchronizeSpawn = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SYNCHRONIZE_MONSTER_SPAWN_TO_ANIM);

    if (fSpawnMonster)
    {

        ActorAddStruct tActorAdd;
        SNO snoEncounter = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SPAWNED_ENCOUNTER);
        RNG & rng = SGameGlobalsGet().rng;
        if (EncounterResolveToActorSpawnInfo(snoEncounter, rng, tActorAdd.tSNOName, (int32 &)tActorAdd.eTeam))
        {
            SNO snoActorToSpawn = tActorAdd.tSNOName.snoHandle;
            tACD.AttributesSetInt(ATTRIBUTE_GIZMO_ACTOR_SNO_TO_SPAWN, snoActorToSpawn);
            if (snoActorToSpawn != SNO_NONE)
            {
                ScopedActorDefinitionLockR pdefActorSpawned(snoActorToSpawn);
                float flMinScale = TagMapGetFloat(pdefActorSpawned->hTagMap, TAG_ACTOR_SCALE_MIN);
                float flDeltaScale = TagMapGetFloat(pdefActorSpawned->hTagMap, TAG_ACTOR_SCALE_DELTA);
                float flScaleToUse = RandRangeFloat_Internal(rng, flMinScale, flDeltaScale);
                tACD.AttributesSetFloat(ATTRIBUTE_GIZMO_ACTOR_TO_SPAWN_SCALE, flScaleToUse);
            }
        }
    }
    // inferred: fSpawnMonster && - the !fSpawnMonster branch at 457 skips this test and goes straight to 484
    if (fSpawnMonster && bSynchronizeSpawn)
    {
        TimerRegister(tACD.id, TIMER_OWNER_ACD, dwContactDelay, &tACD, sChestMonsterSpawnerAnimCallback);
    }


    if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_OPEN))
    {
        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
    }

    if (!bSynchronizeSpawn && fSpawnMonster)
    {

        sSpawnEncounter(tACD);
    }

    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_OPERATOR_ACDID, ptACDOperator ? ptACDOperator->id : ACDID_INVALID);
    TimerRegister(tACD.id, TIMER_OWNER_ACD, dwContactDelay, &tACD, pfuncLootCallback);
}




static void sGizmoOperateChest(
    ActorCommonData & tACD,
    ActorCommonData * ptACDOperator)
{
    BOOL bChestOpen = GizmoChestIsOpen(tACD);
    if (bChestOpen)
    {
        return;  // inferred: no own row; the cbnz at line 507 goes to the helper exit
    }


    tACD.AttributesSetInt(ATTRIBUTE_CHEST_OPEN, 1);

    ScopedActorDefinitionLockR pdefActor(tACD.GetActorSNO());

    sHandleChestSpawningBehavior(pdefActor, tACD, ptACDOperator, sChestSpawnLootCallback);


    ACDPlayEffect(tACD.id, EFFECT_OPEN_CHEST, 0, ACDID_INVALID);

    if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHEST_REMOVES_COLLISION_ON_OPERATION))
    {
        tACD.EnableCollision(FALSE);
    }

    Player * ptPlayer = ptACDOperator ? PlayerGetByACD(ptACDOperator->id) : NULL;
    if (ptPlayer)
    {





        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.SetACD(&tACD);

        tEvaluationData.EvaluateEvent(
            ptPlayer,
            Achievement::ACH_EVENT_LOOTED_CHEST,
            tACD.snoActor,
            1);
    }

}



uint32 GetLoreChestUniqueHash(const ActorCommonData & tACD)
{
    const Actor * ptActor = ActorGet((ActorID)tACD.idOwner);
    CRefString sMarkerName;
    SNO snoQuestRange = SNO_NONE;



    if (ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
    {
        ScopedMarkerSetDefinitionLockR ptMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);

        if (ptActor->tMarkerOwner.nMarkerIndex < ptMarkerSet->tMarkerSet.GetArraySize())
        {
            const Marker & tMarker = ptMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];
            snoQuestRange = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_UNLOCKED_QUESTRANGE);
            sMarkerName = MarkerNameGetString(tMarker.dwHash);
        }
    }

    CRefString sWorldPlaceFormatted;
    WorldPlace tPlace = tACD.GetPlace();
    sWorldPlaceFormatted.Format("%f%f%f", tPlace.wp.x, tPlace.wp.y, tPlace.wp.z);

    CRefString sQuestRangeName = SNOToString((SNOGroup)0x3a, snoQuestRange, FALSE);

    CRefString sWorldName = SNOToString((SNOGroup)0x30, WorldCommonGetWorldSNO(tPlace.idSWorld), FALSE);

    CRefString sFinal = sWorldName + sMarkerName + sWorldPlaceFormatted + sQuestRangeName;

    return StringHash(sFinal.str());
}



static void sUpdatePlayerLoreUsed(
    const ActorCommonData & tACD,
    Player * ptPlayer)
{


    uint32 nHash = GetLoreChestUniqueHash(tACD);

    ptPlayer->tLoresUsed.AddTail_Tracked(nHash);
}







static void sGizmoOperateLoreChest(
    ActorCommonData & tACD,
    ActorCommonData * ptACDOperator)
{
    if (!ptACDOperator)
    {
        return;  // inferred: no own row; the cbz at line 606 goes to the helper exit
    }

    Player* ptOriginalPlayer = PlayerGetByACD(ptACDOperator->id);
    if (!ptOriginalPlayer)
    {
        return;  // inferred: no own row; the cbz at line 612 goes to the helper exit
    }

    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CHEST_OPEN, ptOriginalPlayer->idGameConnection);
    if (tACD.AttributesGetInt(tKey))
    {
        return;  // inferred: no own row; the cbnz at line 618 goes to the helper exit
    }

    tACD.AttributesSetInt(tKey, 1);

    ScopedActorDefinitionLockR pdefActor(tACD.GetActorSNO());

    SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
    if (snoTreasureClass != SNO_NONE)
    {
        float flCameraBiasDist = tACD.GetRadius() + 7.0f;
        LootDropLoot(tACD, ptOriginalPlayer->idPlayerACD, ptOriginalPlayer->idPlayerACD, (uint32)-1, FALSE, flCameraBiasDist, 1, NULL, FALSE);
    }



    if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_OPENING))
    {
        AnimPlayerSendAnimationToPlayer(tACD, TAG_ACTOR_ANIMATION_OPENING, ANIM_USE_ANIM_LENGTH, ANIM_REASON_GIZMO, *ptOriginalPlayer);
    }


    for (Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayersAreLocalToEachOther(*ptPlayer, *ptOriginalPlayer))
        {
            sUpdatePlayerLoreUsed(tACD, ptPlayer);
        }
    }


    ACDPlayEffect(tACD.id, EFFECT_OPEN_CHEST, 0, ACDID_INVALID);
}





static void sGizmoOperatePowerUp(
    ActorCommonData & tACD,
    ActorCommonData & tACDOperator)
{
    if (GizmoPowerUpIsInactive(tACD))
    {
        return;  // inferred: no own row; the cbnz at line 662 goes to the helper exit
    }

    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);
    ScopedActorDefinitionLockR pdefActor(tACD.GetActorSNO());

    sHandleChestSpawningBehavior(pdefActor, tACD, &tACDOperator, sPowerUpSpawnLootCallback);

    if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_GIZMO_POWERUP_ACTIVATION_MSG_LABEL))
    {
        sGizmoSendShrineMessage(tACD, tACDOperator);
    }

    if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_POWERUP_HEALS_ON_OPERATE))
    {
        tACDOperator.HealAndRestoreAllies();
    }


    SNO snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_POWER_ON_OPERATE);
    if (snoPower != SNO_NONE)
    {
        Target tTarget;

        tTarget.InitializeFromACD(tACDOperator.id, FALSE, TRUE);

        PowerStart(tACD.id, snoPower, tTarget);
    }
}



void SGizmoOperateSwitch(
    ActorCommonData & tACD,
    const BOOL bSet)
{
    if (bSet)
    {
        tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);

        AnimPlayerPlayAnimation(tACD, TAG_ACTOR_ANIMATION_OPENING, (uint32)-1, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);

        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_OPEN))
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
    }
    else
    {
        tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 0);

        AnimPlayerPlayAnimation(tACD, TAG_ACTOR_ANIMATION_CLOSING, (uint32)-1, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_NEUTRAL);
    }
}




static void sGizmoUpdateStateCharges(
    ActorCommonData & tACD)
{

    int32 nNumChargesLeft = Math::Max(tACD.AttributesGetInt(ATTRIBUTE_GIZMO_CHARGES) - 1, 0);
    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_CHARGES, nNumChargesLeft);

    // inferred: truth test of the clamped count; the binary compares the raw charges (>= 2), which only this form folds to
    int32 nNewState = nNumChargesLeft ? 2 : 1;
    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, nNewState);
    STrickleUpdateActor(tACD);
    // inferred: == 0 (not <= 0): the binary reuses Max's compare (raw charges > 1)
    if (nNumChargesLeft == 0)
    {
        AnimPlayerPlayAnimation(tACD, TAG_ACTOR_ANIMATION_OPENING, (uint32)-1, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
    }
}

static void sGizmoOperateHealingWell(
    ActorCommonData & tACD,
    ActorCommonData & tACDOperator)
{
    if (GizmoHealingWellIsEmpty(tACD))
    {
        return;  // inferred: no own row; the cbnz at line 746 goes to the helper exit
    }

    tACDOperator.HealAndRestoreAllies();

    sGizmoUpdateStateCharges(tACD);


    uint32 dwNow = GameGetServerTime();
    if (PowerGetCoolDown(tACD) < dwNow)
    {
        ScopedActorDefinitionLockR ptActorDef(tACD.GetActorSNO());
        int32 nRechargeTime = TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_RECHARGE_TIME);
        PowerSetCoolDown(tACD, dwNow + nRechargeTime);
    }
}




static void sGizmoOperatePoolOfReflection(
    ActorCommonData & tACDGizmo,
    ActorCommonData & tACDOperator)
{

    if (GizmoHealingWellIsEmpty(tACDGizmo))
    {
        return;  // inferred: no own row; the cbnz at line 773 goes to the helper exit
    }


    float flRadius = 75.0f;  // inferred: 5625.0f literal at line 0 = flRadius * flRadius
    float flRadiusSq = flRadius * flRadius;
    WorldPlace wpGizmo = tACDGizmo.GetWorldPlaceCenter();

    for (Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        ActorCommonData* ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        // inferred: no own row; the NULL test is merged into ACDTryToGet's (ccmp/b.eq at line 0 -> 783 next)
        if (!ptACDPlayer)
        {
            continue;
        }

        if (ACDGetLevelArea(*ptACDPlayer) == ACDGetLevelArea(tACDGizmo) ||
            (ptACDPlayer->IsInSWorldID(wpGizmo.idSWorld) &&
             Vector3D::Distance2DSquared(wpGizmo.wp, ptACDPlayer->GetWorldPlaceCenter().wp) <= flRadiusSq))
        {



            GrantRestExperience(*ptACDPlayer);
        }
    }

    tACDOperator.HealAndRestoreAllies();


    sGizmoUpdateStateCharges(tACDGizmo);


    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(tACDGizmo.id);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, tACDGizmo.id, FALSE);
    MessageSendToClient(listDestPlayers, SMSG_ACTIVATE_POOL_OF_REFLECTION, &tMessage, sizeof(tMessage));
}




static void sGizmoOperateObjectiveFinder(
    ActorCommonData & tACDGizmo,
    ActorCommonData & tACDOperator)
{

    if (GizmoObjectiveFinderIsUsed(tACDGizmo))
    {
        return;  // inferred: no own row; the cbnz at line 825 goes to the helper exit
    }


    sGizmoUpdateStateCharges(tACDGizmo);


    ANNDataMessage tMsg;
    tMsg.ann = ANNFromACD(tACDGizmo.id);
    PlayerList listPlayers(PlayerListGetElementPool());
    Player* ptPlayer = PlayerGetByACD(tACDOperator.id);
    if (ptPlayer)
    {
        listPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listPlayers, SMSG_OBJECTIVE_FINDER_ACTIVATED, &tMsg, sizeof(tMsg));
    }
}




static void sGizmoOperateWeeklyChallengeInspect(
    ActorCommonData & tACDGizmo,
    ActorCommonData & tACDOperator)
{

    ANNDataMessage tMsg;
    tMsg.ann = ANNFromACD(tACDGizmo.id);
    PlayerList listPlayers(PlayerListGetElementPool());
    Player* ptPlayer = PlayerGetByACD(tACDOperator.id);
    if (ptPlayer)
    {
        listPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listPlayers, SMSG_CHALLENGE_RIFT_INSPECT_ARMORRACK, &tMsg, sizeof(tMsg));
    }
}




static void SGizmoOperateWeeklyChallengeReward(
    ActorCommonData & tACDGizmo,
    ActorCommonData & tACDOperator)
{


    Player* ptPlayer = PlayerGetByACD(tACDOperator.id);
    if (ptPlayer)
    {
        ChallengeRift::GrantRewardToPlayerIfEligible(ptPlayer);
    }
}








static void sGizmoSendShrineMessage(
    ActorCommonData & tACDShrine,
    ActorCommonData & tACDOperator)
{
    Player* ptPlayer = PlayerGetByACD(tACDOperator.id);
    if (!ptPlayer)
    {
        return;  // inferred: no own row; the cbz at line 892 skips the rest of the helper
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, tACDShrine.id, FALSE);

    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(tACDShrine.id);
    MessageSendToClient(listDestPlayers, SMSG_SHRINE_TEXT_DISPLAY, &tMessage, sizeof(tMessage));
}



void sGizmoOperateHeadstone(ActorCommonData & tACDHeadstone, ACDID idACDOperator, BOOL bAlreadyConfirmed)
{


    ACDNetworkName idANNVictim = (ACDNetworkName)tACDHeadstone.AttributesGetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN);
    ACDID idACDVictim = ACDFromANN(idANNVictim);

    ActorCommonData & tACDVictim = ACDGet(idACDVictim);
    BOOL bPlayerVictim = ACDIsPlayer(idACDVictim);


    if (tACDVictim.IsAlive())
    {
        return;
    }

    Player * ptPlayerOperator = PlayerGetByACD(idACDOperator);


    if (!bAlreadyConfirmed && bPlayerVictim)
    {


        tACDVictim.AttributesSetInt(ATTRIBUTE_WAITING_TO_ACCEPT_RESURRECTION, TRUE);


        ACDPlayEffect(tACDHeadstone.id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_HEADSTONE_OPERATED), ACDID_INVALID);
        return;
    }

    if (tACDVictim.IsPet())
    {
        ACDID idACDOwner = ACDGetPetOwner(tACDVictim.id);
        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);


        if (ptACDOwner && ptACDOwner->GetSWorldID() != tACDHeadstone.GetSWorldID())
        {

            ACDWarpToACD(tACDHeadstone, *ptACDOwner, 0, 40.0f);
        }



        ACDWarpToACD(tACDVictim, tACDHeadstone, 0, 40.0f);
    }

    ActorID idActorResurrected = (ActorID)ACDGetOwner(tACDVictim.id);
    WorldPlace placeResurrect = tACDHeadstone.GetPlace();
    Quaternion qResurrect = tACDHeadstone.GetOrientation();
    ActorSnapToPosition(idActorResurrected, placeResurrect, TRUE, TRUE);
    ActorSetOrientation(idActorResurrected, qResurrect, TRUE);

    ActorID idVictim = (ActorID)ACDGetOwner(tACDVictim.id);
    BrainAPIDoResurrect(idVictim, FALSE);


    if (bPlayerVictim)
    {
        ServerMessageSendForceInterfaceClose(tACDVictim.id);
        SPlayerWarped(tACDVictim, (PlayerWarpReason)0, NULL, ACDID_INVALID, 0.0f, TRUE);


        if (ptPlayerOperator)
        {




            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayerOperator);
            tEvaluationData.SetACD(&tACDVictim);

            tEvaluationData.EvaluateEvent(ptPlayerOperator, 501 /* ACH_EVENT_RESURRECT_PLAYER */, PlayerGetPlayerClass(&tACDVictim), 1);
        }

    }

    ACDPlayEffect(tACDVictim.id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_RESURRECTION), ACDID_INVALID);



}














inline BOOL sWaypointCanBeActivated(ActorCommonData & tACDWaypoint, int32 & nWaypointNumber, SNO & snoLevelArea)
{
    if (GameIsOpenWorld())
    {
        return FALSE;
    }

    snoLevelArea = ACDGetLevelArea(tACDWaypoint);
    nWaypointNumber = ActGetWaypointByLevelArea(snoLevelArea, FALSE);
    return nWaypointNumber != -1;
}














BOOL GizmoIsWaypointActivated(ActorCommonData & tACDWaypoint, BOOL bRequirePair, SNO * psnoLevelArea, int32 * pnWaypointNumber)
{
    int32 nWaypointNumber;
    SNO snoLevelArea;






    if (!sWaypointCanBeActivated(tACDWaypoint, nWaypointNumber, snoLevelArea))
    {
        return TRUE;
    }

    if (psnoLevelArea != NULL)
    {
        *psnoLevelArea = snoLevelArea;
    }
    if (pnWaypointNumber != NULL)
    {
        *pnWaypointNumber = nWaypointNumber;
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (bRequirePair)
    {
        return tSGameGlobals.WaypointIsActive(nWaypointNumber);
    }
    return (tSGameGlobals.dwActivatedWaypoints >> nWaypointNumber) & 1;
}


void GizmoActivateWaypoint(ActorCommonData & tACDWaypoint, const ACDID idACDDetected)
{




    if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }


    BOOL bReady = sGizmoPollAndSetTimer(tACDWaypoint);
    if (!bReady)
    {
        return;
    }

    SNO snoLevelArea;
    int32 nWaypointNumber;

    if (GizmoIsWaypointActivated(tACDWaypoint, FALSE, &snoLevelArea, &nWaypointNumber))
    {
        return;
    }

    ACDID idACDPlayerDetected = ACDID_INVALID;
    Player * ptPlayerDetected = ACDGetControllingPlayer(idACDDetected);
    if (ptPlayerDetected != NULL)
    {
        idACDPlayerDetected = ptPlayerDetected->idPlayerACD;
    }

    WaypointActivatedMessage tMessage;
    tMessage.annWaypoint = ANNFromACD(tACDWaypoint.id);
    tMessage.annActivatingPlayer = ANNFromACD(idACDPlayerDetected);
    tMessage.snoLevelArea = snoLevelArea;
    tMessage.bAnnounce = TRUE;
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
        MessageSendToClient(listDestPlayers, SMSG_ACTIVATE_WAYPOINT, &tMessage, sizeof(tMessage));
    }

    ActorID idWaypointActor = (ActorID)ACDGetOwner(tACDWaypoint.id);
    if (idWaypointActor != ACTORID_INVALID)
    {
        ActorRemoveWaypointFromInactiveList(idWaypointActor);
    }

    SPlayerActivateWaypoint(nWaypointNumber, TRUE);
    Minimap::ActivateWaypointMarker(snoLevelArea);
}










static void sGizmoOperateWaypoint(
    ActorCommonData & tACDWaypoint,
    ActorCommonData & tACDOperator)
{
    Player* ptPlayer = PlayerGetByACD(tACDOperator.id);



    if ((GameIsOpenWorld() ||
        CountSetBits((const uint8 *)&ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints, sizeof(uint32)) > 1 ||
        ptPlayer->tHeroStateData.tSavedData.tLastSavePoint.IsValid()) == FALSE)
    {
        PlayerDisplayErrorMessage(tACDOperator.id, GAMEERROR_NO_OTHER_WAYPOINTS);
        return;  // inferred: no own row; line 1137 ends with b to the helper exit
    }

    int32 nActive = tACDWaypoint.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);
    if (nActive)
    {
        tACDWaypoint.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 0);

        AnimPlayerPlayAnimation(tACDWaypoint, TAG_ACTOR_ANIMATION_OPENING, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(tACDWaypoint, TAG_ACTOR_ANIMATION_OPEN);
    }

    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(tACDWaypoint.id);
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_OPERATE_WAYPOINT, &tMessage, sizeof(tMessage));
}




void GizmoSetCheckpoint(Player & tPlayer, ActorCommonData & tACDGizmo)
{
    CheckpointData tCheckpoint;



    tCheckpoint.place = tACDGizmo.GetPlace();
    tCheckpoint.qOrientation = tACDGizmo.GetOrientation();
    SPlayerSetCheckpoint(tPlayer, tCheckpoint);
}



static void sUpdateActiveReturnPortal()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();



    ActorCommonData * ptACDReturnPortal = ACDTryToGet(tSGameGlobals.idACDReturnPointPortal);
    if (ptACDReturnPortal != NULL)
    {
        ptACDReturnPortal->SetEnabled(tSGameGlobals.tReturnPoint.IsValid());
    }
}



void GizmoSetSavePoint(Player & tPlayer, ActorCommonData & tACDGizmo, const int32 nSavepointNum, BOOL bCreatesPortal)
{


    SWorldID idSWorld = tACDGizmo.GetSWorldID();


    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld == NULL || ptSWorld->tWCD.snoWorld == SNO_NONE)
    {
        return;
    }

    SavePointData tSavePoint;
    tSavePoint.snoWorld = ptSWorld->tWCD.snoWorld;
    tSavePoint.nNum = nSavepointNum;
    tSavePoint.bCreatesPortal = bCreatesPortal;
    if (SPlayerSetSavePoint(tPlayer, tSavePoint))
    {
        PlayerList listThisPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listThisPlayer, tPlayer);
        SavePointInfoMessage tMessage;
        tMessage.snoLevelArea = ACDGetLevelArea(tACDGizmo);
        MessageSendToClient(listThisPlayer, SMSG_SAVEPOINT_INFO, &tMessage, sizeof(tMessage));
        if (tPlayer.bIsPrimaryPlayerServerOnly)
        {
            ACDPlayEffectForPlayer(tPlayer.idPlayerACD, tPlayer.idPlayerACD, EFFECT_SAVEPOINT_ACTIVATED, 0);
        }
    }
}

























void GizmoUseWaypoint(
    Player & tPlayer,
    ACDID idACDWaypoint,
    int32 nWaypointNumber)
{
    ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    // 1249-1252 inferred: the NULL test leaves no row (folded into the inlined ACDTryToGet branches)
    if (ptACDPlayer == NULL)
    {
        return;
    }
    sSetPortalCooldown(*ptACDPlayer);



    if (nWaypointNumber == -1)
    {
        SWorldPlayerWarpToLastSavepoint(tPlayer);
        return;
    }

    BOOL bAlreadyActived = SGameGlobalsGet().WaypointIsActive(nWaypointNumber);
	if (!bAlreadyActived)
	{
        // inferred: this call is tail-merged with the one at 1278 in the original (its rows are line 0)
        SendPlayerCancelWarp(tPlayer.idPlayerACD);
        return;
	}



	BOOL bResult = SWorldPlayerWarpToWaypoint(tPlayer.idPlayerACD, idACDWaypoint, nWaypointNumber);
	if (!bResult)
	{

        SendPlayerCancelWarp(tPlayer.idPlayerACD);
	}



}



static void sPlayPickupError(
    Player * ptPlayer,
    PickupResponse ePickupResponse,
    ACDID idACDGizmo)
{

    ActorCommonData & tACDItem = ACDGet(idACDGizmo);
    int32 nPlayerIndex = tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_PLAYER_RECIPIENT);
    Player * ptPlayerOwner = PlayerGetByPlayerIndex(nPlayerIndex);
    // inferred: the NULL fallback is a csel (1297), no own rows for 1298-1300
    if (ptPlayerOwner == NULL)
    {
        ptPlayerOwner = ptPlayer;
    }


    if (ptPlayerOwner)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, *ptPlayerOwner);
        ACDPickupFailedMessage tMessage;
        tMessage.bIsCurrency = ItemIsCurrency(ACDGet(idACDGizmo));
        tMessage.ePickupResponse = ePickupResponse;
        tMessage.annPickee = ANNFromACD(ptPlayerOwner->idPlayerACD);
        MessageSendToClient(listDestPlayers, SMSG_ACD_PICKUP_FAILED, &tMessage, sizeof(tMessage));
    }
}








BOOL GizmoDoVacuumPickupAttractorFX(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, SNO snoAttractorEffectGroup)
{


    if (ACDGetDistanceSquared(tACDItem, tACDOwner, TRUE) <= 64.0f)
        return FALSE;





    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY);
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.eTeam = TEAM_NEUTRAL;
    tActorAdd.tPlace = tACDItem.GetPlace();
    tActorAdd.qOrientation = tACDItem.GetOrientation();
    tActorAdd.flScale = 1.0f;
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, tActorAdd.snoActor);

    ActorID idActorProxy = ActorAdd(tActorAdd);

    ACDID idACDProxy = ACDFindByActor(idActorProxy);
    if (idACDProxy == ACDID_INVALID)
    {
        return FALSE;
    }

    ACDPlayEffectGroupACDToACD(snoAttractorEffectGroup, idACDProxy, tACDOwner.id);


    ActorDeleteSoon(idActorProxy, 30);

    return TRUE;
}

static ACDID sGetPickeeForItem(
    const ActorCommonData & tACDGizmo,
    const ActorCommonData & tACDOperator)
{


    ACDID pickeeID = ACDID_INVALID;
    int32 nPlayerIndex = tACDGizmo.AttributesGetInt(ATTRIBUTE_ITEM_PLAYER_RECIPIENT);

    if(nPlayerIndex == -1)
    {
        pickeeID = tACDOperator.id;  // inferred: the load is a line-0 row with column 33, which fits this line
        return pickeeID;  // inferred: no own row; ptPlayerOwner/ptPlayerOperator are function-scope, so no else block
    }
    Player* ptPlayerOwner = PlayerGetByPlayerIndex(nPlayerIndex);
    Player* ptPlayerOperator = PlayerGetByACD(tACDOperator.id);
    if(ptPlayerOwner && (ptPlayerOperator->idGameConnection == ptPlayerOwner->idGameConnection))
    {
        pickeeID = ptPlayerOwner->idPlayerACD;
    }

    if(pickeeID == ACDID_INVALID || ACDTryToGet(pickeeID) == NULL)
    {
        if(GameIsSeasonal())
        {
            pickeeID = ACDID_INVALID;  // inferred: no own row; seasonal callers skip the pickup (branch to their exit)
        }
        else
        {
            pickeeID = ptPlayerOperator->idPlayerACD;
        }
    }

    return pickeeID;
}






static void sGizmoPickupItem(
    ActorCommonData & tACDGizmo,
    ActorCommonData & tACDOperator)
{

    Player *ptPlayer = PlayerGetByACD(tACDOperator.id);



    if (!ACDIsPlayer(tACDOperator.id))
    {
        return;  // inferred: no own row; the cbz at line 1409 branches to the helper exit
    }



    if (!CValidateIsACDValidItemInTheWorld(tACDGizmo.id))
    {
        return;  // inferred: no own row; the cbz at line 1416 branches to the helper exit
    }



    if (!CValidatePlayerCloseEnoughToPickupItem(tACDOperator, tACDGizmo))
    {
        return;  // inferred: no own row; the cbz at line 1423 branches to the helper exit
    }


    WorldPlace tOldPlace = tACDGizmo.GetPlace();
    PickupResponse ePickupResponse;

    ACDID idPickee = sGetPickeeForItem(tACDGizmo, tACDOperator);


    if (idPickee == ACDID_INVALID)
    {
        return;  // inferred: no own row; line 1435 b.eq goes to the helper exit
    }


    ePickupResponse = SACDInventoryPickup(idPickee, tACDGizmo.id, EFFECT_PLAY_PICKUP_SOUND, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE);

    if (ePickupResponse != PICKUP_RESPONSE_OK &&
        ePickupResponse != PICKUP_RESPONSE_INVALID &&
        ePickupResponse != PICKUP_RESPONSE_UNUSABLE_GLYPH &&
        tACDGizmo.GetLocationType() == AL_WORLD)
    {
        Player *ptPlayerLoc = PlayerGetByACD(tACDOperator.id);
        if (ePickupResponse == PICKUP_RESPONSE_OWNED_BY_OTHER)
        {
            sPlayPickupError(ptPlayerLoc, ePickupResponse, tACDGizmo.id);
        }
        else
        {

            WorldPlace tPlace;
            if (FlippyFindLandingLocation(tACDOperator, tACDGizmo.snoActor, tPlace, NULL, NULL, ACDID_INVALID))
            {

                if (tACDGizmo.AttributesGetInt(ATTRIBUTE_FLIPPY_ID) == -1)
                {

                    ACDRemoveToLimbo(tACDGizmo);

                    FlippyDropCreateOnActor(tACDOperator.id, tACDGizmo.id, tPlace);
                }

                sPlayPickupError(ptPlayerLoc, ePickupResponse, tACDGizmo.id);
            }
            else
            {

                ActorFlagForDeletion((ActorID)ACDGetOwner(tACDGizmo.id));
            }
        }
        return;  // inferred: no own row; both arms end in a branch to the helper exit
    }



    ScopedItemDefinitionLockR pdefItem(tACDGizmo.hGB.gbid);
    if (pdefItem && pdefItem->eVacuumPickup)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;



        XPooledDataIDList listVacuumPickupACDs(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(tOldPlace, pdefGlobal->ptServerData->flVacuumPickupRadius, tACDGizmo.id, listVacuumPickupACDs, (1 << ACTOR_SPHERE_TREE_ITEMS_IN_WORLD));

        POS pos = listVacuumPickupACDs.GetHeadPosition();
        while (pos)
        {

            POS posRemoval = pos;
            ACDID idACDItem = (ACDID)listVacuumPickupACDs.GetNext(pos);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (!ptACDItem || !ptACDItem->IsItem())
            {
                listVacuumPickupACDs.RemoveAt(posRemoval);  // inferred: the RemoveAt calls are tail-merged into one line-0 call
                continue;
            }

            switch (pdefItem->eVacuumPickup)
            {
            case 1:

                if (ptACDItem->hGB.gbid != tACDGizmo.hGB.gbid)
                {
                    listVacuumPickupACDs.RemoveAt(posRemoval);  // inferred: tail-merged line-0 RemoveAt
                }
                break;

            case 2:


                if (!ItemIsGem(ptACDItem->hGB.gbid))
                {
                    listVacuumPickupACDs.RemoveAt(posRemoval);  // inferred: tail-merged line-0 RemoveAt
                }
                break;

            default:
                listVacuumPickupACDs.RemoveAt(posRemoval);  // inferred: b.ne at line 1504 (line 0) goes to the merged RemoveAt
                break;
            }
        }




        if (listVacuumPickupACDs.Count() > 0)
        {

            ACDPlayEffect(tACDOperator.id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_VACUUM_PICKUP), ACDID_INVALID);


            int32 nItemVacuumAttractorsCount = 3;
            while (nItemVacuumAttractorsCount > 0 && listVacuumPickupACDs.Count() > 0)
            {

                int32 nRandomItem = Rand_Internal(SGameGlobalsGet().rng, listVacuumPickupACDs.Count() - 1);
                ACDID idRandomItem = (ACDID)listVacuumPickupACDs.GetHead();

                POS posRand = listVacuumPickupACDs.GetHeadPosition();
                POS posRemoval = posRand;
                while (nRandomItem > 0 && posRand)
                {
                    posRemoval = posRand;  // inferred: register copy, no row of its own
                    idRandomItem = (ACDID)listVacuumPickupACDs.GetNext(posRand);
                    --nRandomItem;
                }


                ActorCommonData * ptACDRandomItem = ACDTryToGet(idRandomItem);
                if (ptACDRandomItem)
                {
                    BOOL bPlayedFX = GizmoDoVacuumPickupAttractorFX(*ptACDRandomItem, tACDOperator, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_VACUUM_PICKUP_ATTRACTOR));
                    if (bPlayedFX)
                        nItemVacuumAttractorsCount--;  // inferred: cset/sub carry line 1559 (if-converted)
                }



                ACDID idPickee = sGetPickeeForItem(*ptACDRandomItem, tACDOperator);
                if (idPickee != ACDID_INVALID)
                {
                    SACDInventoryPickup(idPickee, idRandomItem, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE);
                }


                listVacuumPickupACDs.RemoveAt(posRemoval);
            }


            pos = listVacuumPickupACDs.GetHeadPosition();
            while (pos)
            {
                ACDID idACDItem = (ACDID)listVacuumPickupACDs.GetNext(pos);
                ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                if (ptACDItem)
                {
                    ACDID idPickee = sGetPickeeForItem(*ptACDItem, tACDOperator);
                    if (idPickee != ACDID_INVALID)
                    {
                        SACDInventoryPickup(idPickee, idACDItem, EFFECT_INVALID, TRUE, FALSE, NULL, FALSE, FALSE, FALSE, TRUE);
                    }
                }
            }
        }
    }
}




static void sGizmoOperateStash(
    ActorCommonData & tACD,
    ActorCommonData & tACDOperator)
{
	Player *ptPlayer = PlayerGetByACD(tACDOperator.id);


    ptPlayer->idACDStashLastOperated = tACD.id;

	PlayerList listDestPlayers(PlayerListGetElementPool());
	PlayerListGetSingle(listDestPlayers, *ptPlayer);

	ANNDataMessage tMessage;
	tMessage.ann = ANNFromACD(tACD.id);

	MessageSendToClient(listDestPlayers, SMSG_SHARED_STASH_OPEN, &tMessage, sizeof(tMessage));
}





static void sGizmoOperateMailbox(
    ActorCommonData & tACD,
    ActorCommonData & tACDOperator)
{
    Player *ptPlayer = PlayerGetByACD(tACDOperator.id);


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);

    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);

    MessageSendToClient(listDestPlayers, SMSG_MAILBOX_OPEN, &tMessage, sizeof(tMessage));
}





static void sGizmoOperateBanner(
    ActorCommonData & tACD,
    ActorCommonData & tACDOperator)
{


    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACD.snoActor);
    Player *ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);
    if (!ptPlayerDest)
    {
        return;  // inferred: no own row; the cbz at line 1647 branches to the helper exit
    }

    SPlayerWarpToOtherPlayer(PlayerGetByACD(tACDOperator.id), ptPlayerDest);
}




static BOOL sHandleLockingBehavior(
    ActorCommonData & tACDGizmo,
    ActorCommonData * ptACDOperator)
{
	if (!ptACDOperator)
    {
        return TRUE;  // inferred: no own row; the cbz at line 1662 branches to the helper's TRUE exit
    }


    Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
    if (!ptPlayer)
    {
        return TRUE;  // inferred: no own row; the cbz at line 1669 branches to the helper's TRUE exit
    }


	GizmoType nGizmoType = tACDGizmo.GetGizmoType();
    if ((nGizmoType == ACTORGIZMO_CHEST && GizmoChestIsOpen(tACDGizmo)) ||
        (nGizmoType == ACTORGIZMO_DOOR && GizmoDoorIsOpen(tACDGizmo) && !GizmoPlayerCanCloseDoor(tACDGizmo)))
    {
        return FALSE;  // inferred: no own row; line 1676 branches to the function exit
    }


    ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
    StringLabelHandle hLockedLabel = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_LOCKED_MSG_LABEL);
    BOOL fShowVagueMessage = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SHOW_VAGUE_ITEM_REQUIRED_MESSAGE);
    GBID gbidItemRequired = TagMapGetGBID(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_REQUIRED_TO_OPERATE);
    int32 numItemRequired = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_NUM_REQUIRED_TO_OPERATE);

    if (GizmoLockedByQuestRange(tACDGizmo))
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, *ptPlayer);

        DWordDataMessage tMsg;
        tMsg.dwData = hLockedLabel;
        MessageSendToClient(listDestPlayers, SMSG_DISPLAY_VAGUE_LOCKED_ERROR, &tMsg, sizeof(tMsg));
        return FALSE;  // inferred: no own row; the listDestPlayers destructor carries the closing brace (1698)
    }

    if (gbidItemRequired == GBID_NONE)
    {
        return TRUE;  // inferred: no own row; line 1700 b.eq goes to its own unlock + TRUE exit
    }


    BOOL bFound = FALSE;
    ActorCommonData * ptACDItem = NULL;

    if (ItemHasLabel(gbidItemRequired, (ItemLabel)0x2c))
    {


        bFound = SQuestsHasQuestItemBeenFound(gbidItemRequired);
    }
    else
    {
        GBHandle hGBItem;
        hGBItem.Set(2, gbidItemRequired);

        int32 nCount = ACDInventoryGetItemCount(*ptACDOperator, hGBItem, geGizmoConsumedItemSlotsToCheck, FALSE, FALSE);
        if (nCount >= numItemRequired) bFound = TRUE;  // inferred: the compare is line 0 col 24 and branches straight to the found path (no cset)
    }









    if (!bFound)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayers, *ptPlayer);

        if (fShowVagueMessage)
        {
            DWordDataMessage tMsg;
            tMsg.dwData = hLockedLabel;
            MessageSendToClient(listDestPlayers, SMSG_DISPLAY_VAGUE_LOCKED_ERROR, &tMsg, sizeof(tMsg));
        }
        else
        {
            GBIDDataMessage tMsg;
            tMsg.gbidData = gbidItemRequired;

            MessageSendToClient(listDestPlayers, SMSG_DISPLAY_ITEM_REQUIRED_ERROR, &tMsg, sizeof(tMsg));
        }

        return FALSE;  // inferred: no own row; the destructor at 1753 and the unlock path after it

    }
    else
    {
        SNO snoSound = ActorInventory_GetItemUseSound(ptACDItem);
        if (snoSound != SNO_NONE)
        {
            ACDPlayEffect(tACDGizmo.id, EFFECT_PLAY_SOUND, snoSound, ACDID_INVALID);
        }
    }

    return TRUE;  // inferred: no own row; the unlock at 1765 then the helper's TRUE exit

}




static BOOL sGizmoIsUnlocked(
    ActorCommonData & tACDGizmo,
    ActorCommonData * ptACDOperator)
{
	GizmoType nGizmoType = tACDGizmo.GetGizmoType();

	if (nGizmoType == ACTORGIZMO_CHEST ||
	    nGizmoType == ACTORGIZMO_DOOR ||
	    nGizmoType == ACTORGIZMO_SWITCH ||
	    nGizmoType == ACTORGIZMO_WEEKLYCHALLENGE_SWITCH ||
	    nGizmoType == ACTORGIZMO_LOOTRUN_SWITCH)
	{

		if (!sHandleLockingBehavior(tACDGizmo, ptACDOperator))
		{
			return FALSE;  // inferred: no own row; the inlined helper's FALSE paths branch to the function exit
		}
	}


	switch (nGizmoType) {
		case ACTORGIZMO_SWITCH: {
			ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
			BOOL fCanOperateMultipleTimes = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SWITCH_OPERATE_MULTIPLE);

			if (!fCanOperateMultipleTimes &&
				GizmoSwitchIsOn(tACDGizmo))
			{
				return FALSE;  // inferred: no own row; cbz at 1795 col 8 -> unlock (1800) -> function exit
			}
		}
		break;

		case ACTORGIZMO_PORTAL: {

			ActorID idActorPortal = (ActorID)ACDGetOwner(tACDGizmo.id);
        Actor *ptActorPortal = ActorGet(idActorPortal);

        ResolvedPortalDestination tResolvedDest;
        ActorResolvePortalDestination(ptActorPortal, tResolvedDest);

        if (WorldIsDungeonFinderPlaceHolder(tResolvedDest.snoWorld))
        {

            Player * ptPlayer = PlayerGetByACD(ptACDOperator->id);
            if (ptPlayer && !NephalemRiftManager::EnterRift(tACDGizmo.id, ptPlayer))
            {
                return FALSE;  // inferred: no own row; cbz at 1815 col 17 goes to the function exit
            }
        }
		}
		break;
	}

	return TRUE;  // inferred: no own row; every TRUE path branches to B01 line 2128
}


void SGizmoOperatePortal(
    ActorCommonData & tACDGizmo,
    ActorCommonData * ptACDOperator)
{
    if (!ptACDOperator)
    {
        return;
    }

    sSetPortalCooldown(*ptACDOperator);

    ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
    uint32 dwAnimTag = ActorDefinitionGetTagMapValueDWORD(TAG_ACTOR_GIZMO_PORTAL_OPERATE_ANIMATION, tACDGizmo.snoActor);

    Player* ptPlayer = PlayerGetByACD(ptACDOperator->id);
    if (!ptPlayer)
    {
        return;
    }

    SNO snoChallenge = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_CHALLENGE_SNO, tACDGizmo.snoActor);
    if (snoChallenge != SNO_NONE)
    {


        ScopedQuestDefinitionLockR pdefQuest(snoChallenge);
        if (!SWorldHasBeenEntered(pdefQuest->snoChallengeWorld))
        {
            if (!ptPlayer->bChallengeAccepted || ptPlayer->idACDActiveChallengePortal != tACDGizmo.id)
            {
                ptPlayer->idACDActiveChallengePortal = tACDGizmo.id;
                ptPlayer->bChallengeAccepted = FALSE;

                SNODataMessage tMessage;
                tMessage.snoData = snoChallenge;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetSingle(listDestPlayers, ptACDOperator->id);
                MessageSendToClient(listDestPlayers, SMSG_CHALLENGE_START, &tMessage, sizeof(tMessage));
                // inferred: no own row; the send's block exits to the pdefActor unlock (+0x740)
                return;
            }
            // inferred: 1870/1871 run when the player already accepted this portal's challenge
            ptPlayer->idACDActiveChallengePortal = ACDID_INVALID;
            ptPlayer->bChallengeAccepted = FALSE;
        }
    }

    // inferred: bare block (ptACDPortal..ptSWorld are in a lexical block 1877-1948 at 8-space columns)
    {
        ActorCommonData* ptACDPortal = ACDTryToGet(tACDGizmo.id);
        ActorID idActorPortal = (ActorID)ACDGetOwner(tACDGizmo.id);
        Actor* ptActorPortal = ActorGet(idActorPortal);

        ResolvedPortalDestination tResolvedDest;
        ActorResolvePortalDestination(ptActorPortal, tResolvedDest);

        SWorldID idSWorld = SWorldFindBySNO(tResolvedDest.snoWorld);
        SWorld * ptSWorld = SWorldTryToGet(idSWorld);
        // inferred: ptSWorld && at 1887 and 1893 (columns 12/38/50 and 13/35); the NULL test is folded into SWorldTryToGet's
        if(ptSWorld && tResolvedDest.snoWorld == GlobalSNOGet(GLOBALSNO_WORLD_GREED_REALM))
        {
            SGameGlobals& tSGameGlobals = SGameGlobalsGet();
            tSGameGlobals.idGreedPortal = tACDGizmo.id;
        }

        if (ptSWorld && ptSWorld->hWorldQuestDescriptionLabel != (StringLabelHandle)-1)
        {
            if(SetEncounterManager::IsAnyEncounterActive())
            {
                if(SetEncounterManager::CanEnterSetDungeon(ptPlayer))
                {
                    SetEncounterManager::PortPlayer(ptPlayer, WARP_SET_DUNGEON);
                }
                else
                {

                    if (SetEncounterManager::IsDungeonClosing())
                    {
                        PlayerDisplayErrorMessage(ptACDOperator->id, GAMEERROR_SET_DUNGEON_CANNOT_ENTER_CLOSING);
                    }
                    else
                    {
                        SimpleMessage tMessage;
                        PlayerList listDestPlayers(PlayerListGetElementPool());
                        PlayerListGetSingle(listDestPlayers, ptACDOperator->id);
                        MessageSendToClient(listDestPlayers, SMSG_SET_DUNGEON_CANNOT_START, &tMessage, sizeof(tMessage));
                    }
                }
                // inferred: no own row; every path of this block goes to the function exit
                return;
            }

            if(SetEncounterManager::IsInviteInProgress())
            {
                return;
            }


            if (!PlayerIsPlayerGameFlagSet(ptPlayer, "Sets_Generic_Worthy"))
            {
                PlayerDisplayErrorMessage(ptACDOperator->id, GAMEERROR_SET_DUNGEON_CANNOT_ENTER_NOT_WORTHY);
                return;
            }


            ptSWorld->tReturnPortalLocation.placeReturn = ptACDOperator->GetPlace();


            ptPlayer->idACDActiveChallengePortal = ptACDPortal->id;
            ptPlayer->bChallengeAccepted = FALSE;

            SetDungeonDialogMessage tMessage;
            tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
            tMessage.nLabelDescription = ptSWorld->hWorldQuestDescriptionLabel;
            tMessage.nLabelTitle = ptSWorld->hWorldQuestTitleLabel;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, ptACDOperator->id);
            MessageSendToClient(listDestPlayers, SMSG_SET_DUNGEON_START, &tMessage, sizeof(tMessage));
            // inferred: no own row; the destructor at 1948 branches to the function exit
            return;
        }
    }

    if (dwAnimTag == (uint32)-1)
    {
        SendPlayerWarping(ptACDOperator->id);

        BOOL bIsElevator = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_PORTAL_IS_ELEVATOR);
        BOOL bIsTownPortal = tACDGizmo.GetGizmoType() == ACTORGIZMO_TOWN_PORTAL;
        PlayerWarpReason eWarpReason;
        if (bIsElevator)
        {
            eWarpReason = WARP_ELEVATOR;
        }
        else if (bIsTownPortal)
        {
            eWarpReason = WARP_TOWN_PORTAL;
        }
        else if (snoChallenge != SNO_NONE)
        {
            eWarpReason = WARP_CHALLENGE;
        }
        else eWarpReason = WARP_PORTAL;
        ptPlayer->eWarpReason = eWarpReason;
        if (!PortalUse(tACDGizmo.id, *ptACDOperator, eWarpReason))
        {
            SendPlayerCancelWarp(ptACDOperator->id);
        }
    }
    else
    {
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = ptACDOperator->id;
        tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_PORTAL_WITH_ANIMATION);
        tPowerStartData.dwAnimTagToUse = dwAnimTag;
        tPowerStartData.tTarget.InitializeFromACD(tACDGizmo.id, FALSE, FALSE);
        PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);


        ScopedActorDefinitionLockR pdefActor2(ActorGet((ActorID)ACDGetOwner(tACDGizmo.id))->snoActor);
        const WorldLocation locPortal = tACDGizmo.GetWorldLocation();
        WorldPlace wp = tACDGizmo.GetPlace();
        wp.wp = locPortal.transform * pdefActor2->wpLocationPowerSrc;

        ptACDOperator->SetFacing(wp, TRUE);
    }

    const Actor* ptActor = ActorTryToGet((ActorID)tACDGizmo.idOwner);
    if (ptActor)
    {

        Minimap::RevealPortalMarker(ptActor);
    }
}




void GizmoOperate(
    ActorCommonData & tACDGizmo,
    ActorCommonData * ptACDOperator,
    BOOL bForceOperation,
    BOOL bAlreadyConfirmed)
{
    ACDID idACDOperator = ptACDOperator ? ptACDOperator->id : ACDID_INVALID;
	if (!ScriptEventOnGizmoCanOperate(tACDGizmo.id, idACDOperator))
    {
        return;  // inferred: no own row; the cbz at 2014 goes to the function exit
    }

    GizmoType nGizmoType = tACDGizmo.GetGizmoType();
    switch (nGizmoType)
    {
        case ACTORGIZMO_NECRO_CORPSE:
        case ACTORGIZMO_POWERUP:
            if (!tACDGizmo.IsOperatableGizmo(ACDID_INVALID, FALSE))
            {
                return;  // inferred: no own row; cbz at 2024 goes to the function exit
            }
            break;
        default:
            break;  // inferred: no own row; b.ne at line 0 skips to 2034
    }


	if (ptACDOperator)
	{
		if (!ACDGizmoCanOperate(tACDGizmo, *ptACDOperator, bForceOperation))
		{
            if (nGizmoType == ACTORGIZMO_WEEKLYCHALLENGE_SWITCH && GameIsWeeklyChallenge() && NephalemRiftManager::IsTieredRiftActive())
            {
                PlayerDisplayErrorMessage(ptACDOperator->id, GAMEERROR_CHALLENGE_RIFT_ALREADY_STARTED);
            }
            return;  // inferred: no own row; every arm of 2038 ends at the function exit
		}
	}


    if (!bAlreadyConfirmed)
    {
        BOOL bConfirm = FALSE;  // inferred: no own row (constant folded into the switch arms)
        switch (nGizmoType)
        {
            case ACTORGIZMO_ACT_TRANSITION_OBJECT:
                bConfirm = TRUE;  // inferred: no own row (line-0 constant on the 2050 dispatch)
                break;

            case ACTORGIZMO_HEADSTONE:
                break;  // inferred: no own row; dispatch goes straight to 2122

            case ACTORGIZMO_SAVEPOINT_PORTAL:
            {

                SGameGlobals& tSGameGlobals = SGameGlobalsGet();
                bConfirm = tSGameGlobals.tReturnPoint.idACDReturnPortal == ACDID_INVALID;
            }
            break;
            case ACTORGIZMO_PORTAL: {
                ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
                bConfirm = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CONFIRM);
                if (bConfirm)
                {
                    SNO snoTimedEvent = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TIMED_EVENT);
                    if (snoTimedEvent != SNO_NONE)
                    {
                        if (STimedEventHasExpired(snoTimedEvent))
                            bConfirm = FALSE;  // inferred: no own row (csel on line 2074)
                    }
                }
                break;
            }


            case ACTORGIZMO_BANNER:
            {
                if (ptACDOperator)
                {
                    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACDGizmo.snoActor);
                    Player *ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);

                    Player *ptPlayerOperator = PlayerGetByACD(ptACDOperator->id);

                    if (ptPlayerDest && ptPlayerOperator && !PlayersAreLocalToEachOther(*ptPlayerOperator, *ptPlayerDest))
                        bConfirm = TRUE;  // inferred: no own row (cset on line 2091)
                }
                break;
            }





            default:
            {

                ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
                bConfirm = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CONFIRM);
            }
            break;
        }

        if (bConfirm)
        {
            Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(tACDGizmo.id);
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, *ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_CONFIRM_OPERATION, &tMessage, sizeof(tMessage));
            return;  // inferred: no own row; the destructor at 2119 then the function exit
        }
    }

    if (!sGizmoIsUnlocked(tACDGizmo, ptACDOperator))
    {
        return;  // inferred: no own row; the inlined helper's FALSE paths go to the function exit
    }


    BOOL bConsumed = SGizmoConsumeItemToOperate(tACDGizmo, ptACDOperator);
    if (!bConsumed)
    {
        return;  // inferred: no own row; cbz at 2129 goes to the function exit
    }

    if (GameIsOpenWorld() && ACDIsTrackedInOpenWorld(tACDGizmo))
    {

        QuestsAddOpenWorldTrackedSNO(tACDGizmo.snoActor);
    }

    if (!tACDGizmo.IsItem())
    {
        tACDGizmo.AttributesSetInt(ATTRIBUTE_GIZMO_OPERATOR_ACDID, idACDOperator);
    }





	{
		Achievement::EvaluationData tEvaluationData;
		tEvaluationData.SetPlayer(PlayerGetByACD(idACDOperator));

		if (tEvaluationData.GetPlayer())
		{
			tEvaluationData.SetACD(&tACDGizmo);

			if (nGizmoType == ACTORGIZMO_POWERUP)
			{
				ScopedActorDefinitionLockR pdefActor(tACDGizmo.GetActorSNO());
				tEvaluationData.SetPower(TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_POWER_ON_OPERATE));
			}

			tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_INTERACT, nGizmoType, TRUE);
		}
	}

    switch (nGizmoType)
    {
        case ACTORGIZMO_DOOR:
            sGizmoOperateDoor(tACDGizmo, bForceOperation);
            break;

        case ACTORGIZMO_CHEST:
            sGizmoOperateChest(tACDGizmo, ptACDOperator);
            break;
        case ACTORGIZMO_LOOTRUN_SWITCH:
            {
                if (!SWorldDungeonFinderIsEmpty())
                {
                    if (ptACDOperator)
                    {
                        PlayerDisplayErrorMessage(ptACDOperator->id, GAMEERROR_LOOTRUN_PARTY_INSIDE);
                    }
                    break;  // inferred: no own row; 2182 and the cbz at 2180 both go to 2484
                }

                Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);

                if (bForceOperation)
                {
                    SGizmoOperateSwitch(tACDGizmo, TRUE);

                    NephalemRiftManager::PlayerSetPaidForRift(ptPlayer, 1, 1, 0);
                    NephalemRiftManager::SetRiftActive(TRUE);
                }
                else
                {


                    NephalemRiftManager::InitiateRift(tACDGizmo.id, ptPlayer);
                    return;  // inferred: no own row; the b after 2200 goes to 2485, skipping SGizmoOperated
                }
            }
            break;
















        case ACTORGIZMO_WEEKLYCHALLENGE_SWITCH:
            {
                SGameGlobals& tSGameGlobals = SGameGlobalsGet();
                if (Blizzard::Time::ToUnixTime64(Console::Online::GetNetworkTimestamp()) > tSGameGlobals.tChallengeRift.nEndTime)
                {
                    PlayerDisplayErrorMessage(ptACDOperator->id, GAMEERROR_CHALLENGE_RIFT_NUMBER_MISMATCH);
                    return;  // inferred: no own row; the b at 2226 goes to 2485
                }



                int32 nRiftTier = 1;

                Actor *ptActorGizmo = ActorGet((ActorID)tACDGizmo.idOwner);
                if (ptActorGizmo->tMarkerOwner.snoMarkerSet != SNO_NONE)
                {
                    ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActorGizmo->tMarkerOwner.snoMarkerSet);
                    const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptActorGizmo->tMarkerOwner.nMarkerIndex];
                    nRiftTier = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_GIZMO_WEEKLY_CHALLENGE_RIFT_TIER);
                }




                Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
                NephalemRiftManager::StartWeeklyChallengeRift(tACDGizmo.id, nRiftTier, ptPlayer);
            }
            break;

        case ACTORGIZMO_SWITCH:
            SGizmoOperateSwitch(tACDGizmo, TRUE);
            break;

        case ACTORGIZMO_PORTAL:
        case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
            if (ptACDOperator)
            {
                Player *ptPlayerOperator = PlayerGetByACD(ptACDOperator->id);
                if (ptPlayerOperator)
                {
                    ActorID idActorPortal = (ActorID)ACDGetOwner(tACDGizmo.id);
                    Actor *ptActorPortal = ActorTryToGet(idActorPortal);
                    if (ptActorPortal)
                    {
                        ResolvedPortalDestination tResolvedDest;
                        ActorResolvePortalDestination(ptActorPortal, tResolvedDest);
                        SWorldID idSWorld = SWorldFindBySNO(tResolvedDest.snoWorld);
                        if (idSWorld != SWORLDID_INVALID)
                        {
                            if (!SWorldPlayerAllowed(idSWorld, *ptPlayerOperator))
                            {
                                PlayerDisplayErrorMessage(ptACDOperator->id, GAMEERROR_PLAYER_NOT_ALLOWED_IN_WORLD);
                                return;  // inferred: no own row; the b at 2272 goes to 2485
                            }
                        }



                        if (WorldIsDungeonFinderWorld(tResolvedDest.snoWorld) && !NephalemRiftManager::IsAnyRiftActive())
                        {
                            return;  // inferred: no own row; the cbz at 2279 col 29 goes to 2485
                        }
                    }
                }
            }
            // fall through  (inferred: TOWN_PORTAL's jump-table entry is the same line-0 SGizmoOperatePortal call)
        case ACTORGIZMO_TOWN_PORTAL:
            SGizmoOperatePortal(tACDGizmo, ptACDOperator);  // inferred line: the call is line 0 (col 29), tail-merged
            break;

        case ACTORGIZMO_DUNGEON_PORTAL:
        case ACTORGIZMO_RETURN_PORTAL: {
			SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_USE_DUNGEON_STONE);
			if (snoPower == SNO_NONE)
				break;  // inferred: no own row; b.eq at 2294 goes to 2484


            Target tTarget;
            tTarget.InitializeFromACD(tACDGizmo.id, FALSE, FALSE);


                uint32 uFailureReason = PowerFailureReasons(*ptACDOperator, AutoPowerDefinitionR(snoPower), &tTarget, FALSE, FALSE);
                if (uFailureReason)
                    break;  // inferred: no own row; cbnz at 2303 goes to 2484



				PowerStart(ptACDOperator->id, snoPower, tTarget);
        }
        break;

        case ACTORGIZMO_SAVEPOINT_PORTAL:

			if (ptACDOperator)

				SWorldPlayerWarpToReturnPoint(ptACDOperator->id);
            break;

        case ACTORGIZMO_BOSS_PORTAL:
            {

			if (ptACDOperator)
			{
                       SNO snoEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, tACDGizmo.snoActor);
                       Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
                       if (snoEncounter != SNO_NONE && ptPlayer)

                           BossEncounterManager::TriggerEncounter(snoEncounter, ptPlayer, tACDGizmo.GetSWorldID(), FALSE);
			}
            }
            break;


        case ACTORGIZMO_WAYPOINT:

            sGizmoOperateWaypoint(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_ITEM_INTERNAL:

            sGizmoPickupItem(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_HEALING_WELL:

            sGizmoOperateHealingWell(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_POWERUP:

            sGizmoOperatePowerUp(tACDGizmo, *ptACDOperator);
            break;









        case ACTORGIZMO_HEARTH_PORTAL:

			HearthPortalUse(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_SHARED_STASH:

            sGizmoOperateStash(tACDGizmo, *ptACDOperator);
            break;







        case ACTORGIZMO_SECRET_PORTAL:

			SecretPortalOperate(tACDGizmo.id, ptACDOperator->id);
            break;









        case ACTORGIZMO_HEADSTONE:

			sGizmoOperateHeadstone(tACDGizmo, idACDOperator, bAlreadyConfirmed);
            break;

        case ACTORGIZMO_ACT_TRANSITION_OBJECT:
            {

              Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
			if (ptPlayer)

				SActTransitionStart(ptPlayer, FALSE);
            }
            break;


        case ACTORGIZMO_BANNER:

            sGizmoOperateBanner(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_LORE_CHEST:

            sGizmoOperateLoreChest(tACDGizmo, ptACDOperator);
            break;

        case ACTORGIZMO_IDENTIFY_ALL:
            {

                if (SPlayerTryIdentifyAllWithCast(ptACDOperator->id, tACDGizmo.id))
                {

                    Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
                    if (ptPlayer)
                    {
                    AnimPlayerSendAnimationToPlayer(tACDGizmo, TAG_ACTOR_ANIMATION_OPENING, (uint32)-1, ANIM_REASON_GIZMO, *ptPlayer);
                    }
                }
            }
            break;

        case ACTORGIZMO_RECREATE_GAME_WITH_PARTY:
            {
                Player *ptPlayer = PlayerGetByACD(ptACDOperator->id);
                if (ptPlayer)
                {

                    SActTransitionStart(ptPlayer, TRUE);
                }
            }
            break;



        case ACTORGIZMO_MAILBOX:

            sGizmoOperateMailbox(tACDGizmo, *ptACDOperator);
            break;



        case ACTORGIZMO_POOL_OF_REFLECTION:

            sGizmoOperatePoolOfReflection(tACDGizmo, *ptACDOperator);
            break;





        case ACTORGIZMO_OBJECTIVE_FINDER:

            sGizmoOperateObjectiveFinder(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_WEEKLYCHALLENGE_INSPECT_ARMORRACK:

            sGizmoOperateWeeklyChallengeInspect(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_WEEKLYCHALLENGE_REWARD:

            SGizmoOperateWeeklyChallengeReward(tACDGizmo, *ptACDOperator);
            break;

        case ACTORGIZMO_SIGN:
            return;  // inferred: no own row; SIGN's jump-table entry is 2485 (skips SGizmoOperated)

        default:
            break;  // inferred: no own row; the default entry goes to 2484
    }


    SGizmoOperated(tACDGizmo, ptACDOperator);
}



void SGizmoOperated(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator)
{
    ACDID idACDOperator = ACDID_INVALID;
    if (ptACDOperator != NULL)
    {
        idACDOperator = ptACDOperator->id;
        SQuestsOnPlayerInteractWithGizmo(idACDOperator, tACDGizmo.id);
    }

    ScriptEventOnGizmoOperated(tACDGizmo.id, idACDOperator);

    GizmoType nGizmoType = tACDGizmo.GetGizmoType();
    if (nGizmoType != 5)
    {
        PowerAPI_BuffApplyOnGizmoOperatedEffects(idACDOperator, tACDGizmo);
    }



    if (!ActorFlaggedForDeletion((ActorID)ACDGetOwner(tACDGizmo.id)))
    {
        tACDGizmo.AttributesSetInt(ATTRIBUTE_GIZMO_OPERATED, 1);
        tACDGizmo.AttributesSetInt(ATTRIBUTE_QUEST_MONSTER, 0);
        if (tACDGizmo.AttributesGetInt(ATTRIBUTE_OPERATABLE_STORY_GIZMO) && !tACDGizmo.IsOperatableGizmo(ACDID_INVALID, FALSE))
        {
            tACDGizmo.AttributesSetInt(ATTRIBUTE_OPERATABLE_STORY_GIZMO, 0);
        }
    }



    SGizmoAddStaticGizmoPing(tACDGizmo);
}






BOOL SGizmoConsumeItemToOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator)
{


    ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
    BOOL bConsumedOnOperation = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_CONSUMED_ON_OPERATION);
    if (bConsumedOnOperation)
    {
        GBID gbidItemRequired = TagMapGetGBID(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_REQUIRED_TO_OPERATE);
        if (!ItemHasLabel(gbidItemRequired, (ItemLabel)0x2c))
        {
            int32 numItemRequired = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_ITEM_NUM_REQUIRED_TO_OPERATE);
            if (ptACDOperator == NULL && numItemRequired > 0)
            {
                return FALSE;
            }

            int32 nRequiredCount;
            GBHandle hGBItem;
            hGBItem.Set(2, gbidItemRequired);

            for (nRequiredCount = numItemRequired; nRequiredCount > 0; )
            {
                int32 nItemCount;
                ACDID idACDItem = ACDID_INVALID;
                ACDInventoryGetItem(*ptACDOperator, hGBItem, geGizmoConsumedItemSlotsToCheck, idACDItem, nItemCount, FALSE);
                ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);





                if (!ptACDItem)
                {
                    return FALSE;

                }
                SACDInventoryDeleteItem(*ptACDItem, nRequiredCount, TRUE, FALSE);
                nRequiredCount -= nItemCount;
            }
        }
    }
    return TRUE;
}
void GizmoUpdate(ActorCommonData & tACDGizmo)
{

    ActorID idActor = (ActorID)ACDGetOwner(tACDGizmo.id);
    Actor * ptActor = ActorGet(idActor);

    GizmoType nGizmoType = tACDGizmo.GetGizmoType();
    switch (nGizmoType)
    {


    case ACTORGIZMO_HEALING_WELL:
        {   int32 nWellEmptiness = tACDGizmo.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);

            if (nWellEmptiness)
            {   uint32 dwNow = GameGetServerTime();
                uint32 dwCoolDown = PowerGetCoolDown(tACDGizmo);
                if (dwNow > dwCoolDown)
                {


                    ScopedActorDefinitionLockR ptActorDef(tACDGizmo.GetActorSNO());
                    int32 nNumChargesMax = TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_NUM_CHARGES);
                    int32 nRechargeTime = TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_RECHARGE_TIME);
                    int32 nNumChargesLeft = tACDGizmo.AttributesGetInt(ATTRIBUTE_GIZMO_CHARGES);

                    while (nNumChargesLeft < nNumChargesMax && dwCoolDown < dwNow)
                    {
                        nNumChargesLeft++;
                        dwCoolDown += nRechargeTime;
                    }

                    tACDGizmo.AttributesSetInt(ATTRIBUTE_GIZMO_CHARGES, nNumChargesLeft);


                    int32 nNewState = nNumChargesLeft == nNumChargesMax ? 0 : 2;
                    tACDGizmo.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, nNewState);


                    if (nWellEmptiness == 1)
                    {
                        AnimPlayerPlayAnimation(tACDGizmo, TAG_ACTOR_ANIMATION_CLOSING, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
                        AnimPlayerSetIdleAnimation(tACDGizmo, TAG_ACTOR_ANIMATION_NEUTRAL);
                    }



                    if (nNumChargesLeft != nNumChargesMax)
                    {
                        PowerSetCoolDown(tACDGizmo, dwCoolDown);
                    }
                }
            }

            ServerQueueUpdateFunction(1, ptActor->tGizmoUpdateData);
            return;
        }

    case ACTORGIZMO_GATE:
        if (!sGateIsLocked(tACDGizmo))
        {
            sGateUpdate(tACDGizmo);
        }
        return;



    case ACTORGIZMO_PORTAL: case ACTORGIZMO_PAGE_OF_FATE_PORTAL: case ACTORGIZMO_DUNGEON_PORTAL: case ACTORGIZMO_RETURN_PORTAL:
        {

            SNO snoTimedEvent = tACDGizmo.AttributesGetInt(ATTRIBUTE_CONTROLLING_TIMEDEVENT_SNO);
            if (snoTimedEvent == SNO_NONE) return;

            if (STimedEventHasExpired(snoTimedEvent))
            {
                tACDGizmo.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);

                AnimPlayerPlayAnimation(tACDGizmo, TAG_ACTOR_ANIMATION_DEATH_01, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
                AnimPlayerSetIdleAnimation(tACDGizmo, TAG_ACTOR_ANIMATION_DEAD_01);
                return;
            }

        }
        break;




    case ACTORGIZMO_BOSS_PORTAL:
        {
            SNO snoEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, tACDGizmo.snoActor);
            tACDGizmo.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, BossEncounterManager::CanTriggerEncounter(snoEncounter));
            ServerQueueUpdateFunction(60, ptActor->tGizmoUpdateData);
            return;
        }


    case ACTORGIZMO_BANNER:
        {
            int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACDGizmo.snoActor);
            Player * ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);

            ActorCommonData * ptACDPlayer = NULL; if (ptPlayerDest) ptACDPlayer = ACDTryToGet(ptPlayerDest->idPlayerACD);


            if (ptACDPlayer) { tACDGizmo.AttributesSetInt(ATTRIBUTE_BANNER_USABLE, !ACDIsInTown(*ptACDPlayer)); tACDGizmo.AttributesSetInt(ATTRIBUTE_BANNER_PLAYER_IN_COMBAT, ptACDPlayer->IsInCombat()); }
            else { tACDGizmo.AttributesSetInt(ATTRIBUTE_BANNER_USABLE, FALSE); tACDGizmo.AttributesSetInt(ATTRIBUTE_BANNER_PLAYER_IN_COMBAT, FALSE); }



            ServerQueueUpdateFunction(60, ptActor->tGizmoUpdateData);
            return;
        }



    default:
        return;
    }




    ServerQueueUpdateFunction(1, ptActor->tGizmoUpdateData);
}












































static void sSpawnEncounter(
    ActorCommonData & tACDGizmo)
{
    ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
    SNO snoEncounter = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SPAWNED_ENCOUNTER);
    uint32 dwSpawnAnimTag = TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SPAWNED_MONSTER_ANIMTAG);
    BOOL bExactLocation = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_SPAWN_EXACT_LOCATION);

    if (snoEncounter != SNO_NONE)
    {
        WorldPlace placeSpawner = tACDGizmo.GetPlace();
        SSceneID idSScene = ACDGetCurrentSSceneID(tACDGizmo);
        uint32 dwAddFlags = 0;

        ActorAddStruct tActorAdd;
        tActorAdd.idMarkersSScene = idSScene;
        tActorAdd.dwActorAddFlags = dwAddFlags;
        tActorAdd.tPlace = placeSpawner;
        tActorAdd.tMarkersName = MarkerStringHash("GizmoMonsterSpawner");
        tActorAdd.dwSpawnAnimTag = dwSpawnAnimTag;

        if (bExactLocation)
        {
            // 2764-2766 inferred: rows at 2764 (load) and 2766 (orr/str), both at the statement start column 13
            tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) |
                (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
            tActorAdd.qOrientation = tACDGizmo.GetOrientation();
        }

        tACDGizmo.GetContactOffset(tActorAdd.tPlace.wp, 0);

        ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

        tActorAdd.idLevelArea = NavMeshHandleGetLevelAreaID(tActorAdd.tNavMeshHandle, FALSE);

        if ( EncounterResolveToActorSpawnInfo(snoEncounter, SGameGlobalsGet().rng, tActorAdd.tSNOName, (int32 &)tActorAdd.eTeam))
        {
            if ( tActorAdd.tSNOName.snoGroup == ACTOR_SNO)
            {
                tActorAdd.snoActor = tActorAdd.tSNOName.snoHandle;
            }

            tActorAdd.flScale = tACDGizmo.AttributesGetFloat(ATTRIBUTE_GIZMO_ACTOR_TO_SPAWN_SCALE);

            GBAdd(tActorAdd);
        }
    }
}




void GizmoCreate(ActorCommonData & tACD)
{

    BOOL fIsStoryTrigger = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_IS_STORY_TRIGGER, tACD.snoActor);
    if (fIsStoryTrigger &&
        tACD.IsOperatableGizmo(ACDID_INVALID, FALSE))
    {
        tACD.AttributesSetInt(ATTRIBUTE_OPERATABLE_STORY_GIZMO, TRUE);
    }

    BOOL bStartsDisabled = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_STARTS_DISABLED, tACD.snoActor);
    if (bStartsDisabled)
    {

        GizmoSetDisabled(tACD, TRUE);
    }


    {
        Actor * ptActor = ActorGet((ActorID)tACD.idOwner);
        InactiveMarkerOwner * ptMarkerOwner = &ptActor->tMarkerOwner;

        if (ptMarkerOwner->snoMarkerSet != SNO_NONE)
        {
            ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptMarkerOwner->snoMarkerSet);

            if (ptMarkerOwner->nMarkerIndex >= 0 && ptMarkerOwner->nMarkerIndex < ptMarkerSetDef->tMarkerSet.GetArraySize())
            {
                const Marker & tMarker = ptMarkerSetDef->tMarkerSet[ptMarkerOwner->nMarkerIndex];
                BOOL bUnusable = TagMapGetInt(tMarker.hTagMap, TAG_MARKER_UNUSABLE_BY_TRIAL_CHARACTERS);
                tACD.AttributesSetInt(ATTRIBUTE_IS_CONTENT_RESTRICTED_ACTOR, bUnusable);
            }
        }
    }
}





void GizmoRestoreState(ActorCommonData & tACD)
{
    GizmoType nGizmoType = tACD.GetGizmoType();
    switch (nGizmoType)
    {
    case ACTORGIZMO_DOOR:
        if (GizmoDoorIsOpen(tACD))
        {
            tACD.EnableCollision(FALSE);
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
        break;

    case ACTORGIZMO_CHEST:
        if (GizmoChestIsOpen(tACD))
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
        break;

    case ACTORGIZMO_PORTAL:
    case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
    case ACTORGIZMO_DUNGEON_PORTAL:
    case ACTORGIZMO_RETURN_PORTAL:
        {
            SNO snoTimedEvent = tACD.AttributesGetInt(ATTRIBUTE_CONTROLLING_TIMEDEVENT_SNO);
            if (snoTimedEvent != SNO_NONE && STimedEventHasExpired(snoTimedEvent))
            {
                tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);
                AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_DEAD_01);
            }
        }
        break;

    case ACTORGIZMO_WAYPOINT:
        {
            int32 nActive = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);
            if (nActive == 0)
            {
                AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
            }
        }
        break;

    case ACTORGIZMO_HEALING_WELL:
    case ACTORGIZMO_POOL_OF_REFLECTION:
        if (GizmoHealingWellIsEmpty(tACD))
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
        break;

    case ACTORGIZMO_POWERUP:
        if (GizmoPowerUpIsInactive(tACD))
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
        break;

    case ACTORGIZMO_SWITCH:
    case ACTORGIZMO_PRESSUREPLATE:
    case ACTORGIZMO_LOOTRUN_SWITCH:
        if (GizmoSwitchIsOn(tACD))
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
        break;

    case ACTORGIZMO_DESTROY_ON_PROXIMITY:
        if (tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == 1)
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_DEAD_01);
        }
        break;

    case ACTORGIZMO_SAVEPOINT_PORTAL:
        GizmoReturnPointPortalInitialize(tACD);
        break;

    case ACTORGIZMO_OBJECTIVE_FINDER:
        if (GizmoObjectiveFinderIsUsed(tACD))
        {
            AnimPlayerSetIdleAnimation(tACD, TAG_ACTOR_ANIMATION_OPEN);
        }
        break;
    }
}





































void sCheckForTrapCallback(TimerID, DataID idOwner, void * ptData)
{




    ActorCommonData * ptACD = ACDTryToGet((ACDID)idOwner);
    if (ptACD != NULL)
    {
        ActorCommonData * ptACDKiller = (ActorCommonData *)ptData;
        sCheckForTrap(*ptACD, *ptACDKiller);
    }
}




void ActorCommonData::GizmoOnDeath(
	ACDID idACDKiller)
{


	GizmoType eGizmoTypeLoc = GetGizmoType();

	ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);

	switch (eGizmoTypeLoc)
	{
	case ACTORGIZMO_BREAKABLECHEST:
		{


			ScopedActorDefinitionLockR pdefActor(GetActorSNO());
			float flPercentChanceToSpawnMonster = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CHANCE_TO_SPAWN_MONSTER);
			float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);
			BOOL fSpawnMonster = flRandom < flPercentChanceToSpawnMonster;
			if (fSpawnMonster)
			{
				sSpawnEncounter(*this);
			}

			SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
			if (snoTreasureClass != SNO_NONE && !SWorldIsSetDungeon(this->GetSWorldID()))
			{
				ExperienceDropLootWithinExpRange(*this, idACDKiller, (uint32)-1, FALSE, 0.0f);
			}

			if (ptACDKiller)
			{
				if (AnimPlayerAnimExists(*this, TAG_ACTOR_ANIMATION_DEATH_01))
				{
					RNG *prng = &SGameGetRNG();
					AnimationTiming tAnimationTiming;
					AnimPlayerGetAnimationTiming(*this, TAG_ACTOR_ANIMATION_DEATH_01, ANIM_USE_ANIM_LENGTH, 1.0f, *prng, tAnimationTiming);
					uint32 dwDelay = tAnimationTiming.dwExecuteTime[0];
					TimerRegister(this->id, TIMER_OWNER_ACD, dwDelay, ptACDKiller, sCheckForTrapCallback);
				}
				else
				{
					sCheckForTrap(*this, *ptACDKiller);
				}
			}
		}
		break;

	case ACTORGIZMO_DESTROY_ON_PROXIMITY:

		AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);
		break;


	case ACTORGIZMO_TOWN_PORTAL:

		OnSavePointPortalReturnDeleted(*this);
		break;
	}


}



BOOL GizmoRequiresOperator(const GizmoType eGizmoType)
{
    switch (eGizmoType)
    {
    case 2:
    case 4:
    case 5:
    case 9:
    case 10:
    case 11:
    case 12:
    case 18:
    case 25:
    case 33:
    case 47:
    case 62:
    case 66:
    case 69:
    case 70:
    case 71:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 82:
        return TRUE;
    default:
        return FALSE;
    }
}







static BOOL sGateIsLocked(const ActorCommonData & tACDGate)
{
    float flPos = tACDGate.AttributesGetFloat(ATTRIBUTE_GATE_POSITION);
    return flPos >= 1.0f;
}









static void sGateUpdate(ActorCommonData & tACDGate)
{

    float flPos = tACDGate.AttributesGetFloat(ATTRIBUTE_GATE_POSITION);
    float flVel = tACDGate.AttributesGetFloat(ATTRIBUTE_GATE_VELOCITY);

    flPos += flVel;
    flPos = flPos < 0.0f ? 0.0f : (flPos > 1.0f ? 1.0f : flPos);


    tACDGate.AttributesSetFloat(ATTRIBUTE_GATE_POSITION, flPos);

    BOOL fStop = FALSE;
    if (flPos >= 1.0f)
    {   tACDGate.EnableCollision(FALSE);

        ScriptEventOnGateGizmoOpened(tACDGate.id);
        fStop = TRUE;
    }
    else if (flPos <= 0.0f)
    {
        AnimPlayerSetIdleAnimation(tACDGate, TAG_ACTOR_ANIMATION_NEUTRAL);
        fStop = TRUE;
    }
    else
    {
        AnimPlayerSetIdleAnimation(tACDGate, TAG_ACTOR_ANIMATION_OPENING);
    }
    if (fStop)
    {
        tACDGate.AttributesSetFloat(ATTRIBUTE_GATE_VELOCITY, 0.0f);
    }
    else
    {   ActorID idActor = (ActorID)ACDGetOwner(tACDGate.id); Actor * ptActor = ActorGet(idActor);
        ServerQueueUpdateFunction(1, ptActor->tGizmoUpdateData);
    }
}




void GateTriggeringACDCountChanged(ActorCommonData & tACDGate, const int32 nDelta)
{



    if (sGateIsLocked(tACDGate))
    {
        return;
    }

    int32 nOldCount = tACDGate.AttributesGetInt(ATTRIBUTE_GIZMO_TRIGGERING_COUNT);
    int32 nNewCount = nOldCount + nDelta;
    tACDGate.AttributesSetInt(ATTRIBUTE_GIZMO_TRIGGERING_COUNT, nNewCount);

    ActorID idActor = (ActorID)ACDGetOwner(tACDGate.id);
    Actor * ptActor = ActorGet(idActor);
    if (nOldCount == 0 && nNewCount > 0)
    {
        int32 nTimeTicks = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_GATE_OPEN_TIME, ptActor->snoActor);
        float flVel = nTimeTicks ? 1.0f / nTimeTicks : 1.0f;
        tACDGate.AttributesSetFloat(ATTRIBUTE_GATE_VELOCITY, flVel);
        ACDPlayEffect(tACDGate.id, EFFECT_GATE_OPENING, 0, ACDID_INVALID);
        ServerQueueUpdateFunction(1, ptActor->tGizmoUpdateData);
    }
    else if (nOldCount > 0 && nNewCount == 0)
    {
        BOOL fNeverCloses = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_GATE_NEVER_CLOSES, ptActor->snoActor);
        if (fNeverCloses)
        {
            tACDGate.AttributesSetFloat(ATTRIBUTE_GATE_VELOCITY, 0.0f);
        }
        else
        {
            int32 nTimeTicks = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_GATE_CLOSE_TIME, ptActor->snoActor);
            float flVel = nTimeTicks ? 1.0f / nTimeTicks : 1.0f;
            tACDGate.AttributesSetFloat(ATTRIBUTE_GATE_VELOCITY, -flVel);
            ACDPlayEffect(tACDGate.id, EFFECT_GATE_CLOSING, 0, ACDID_INVALID);
        }
    }
}









void GizmoPressurePlateCountChanged(const ActorID idActor, const int32 nDelta, const int32 nCount, const BOOL fPassingThrough)
{





    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData & tACD = ACDGet(idACD);
    if (fPassingThrough)
    {
        return;
    }
    SGizmoOperateSwitch(tACD, nCount != 0);

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idActor, listACDs, NULL);
    if (listACDs.Count() > 0)
    {
        POS pos = listACDs.GetHeadPosition();
        while (pos != NULL)
        {
            ACDID idACDLinked = (ACDID)listACDs.GetNext(pos);
            ActorCommonData & tACDLinked = ACDGet(idACDLinked);
            GizmoType eGizmoType = tACDLinked.GetGizmoType();
            if (eGizmoType == ACTORGIZMO_GATE)
            {
                GateTriggeringACDCountChanged(tACDLinked, nDelta);
            }
        }
    }
}







void sReformingDone(TimerID, DataID, void * ptACD)
{
    ActorCommonData & tACD = *(ActorCommonData *)ptACD;
    tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 0);
    tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);
}










void GizmoOperateReformingDestroyable(ActorCommonData & tACD)
{

    if (tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) != 1)
    {


        AnimationTiming tAnimationTiming;

        RNG * prng = &SGameGetRNG();

        AnimPlayerGetAnimationTiming(tACD, TAG_ACTOR_ANIMATION_DEATH_01, ANIM_USE_ANIM_LENGTH, 1.0f, *prng, tAnimationTiming);


        uint32 dwDelay = tAnimationTiming.dwCycleTime;
        TimerRegister(tACD.id, TIMER_OWNER_ACD, dwDelay, &tACD, sReformingDone);


        tACD.AttributesSetInt(ATTRIBUTE_GIZMO_STATE, 1);

        tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);

        AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, FALSE, ANIM_REASON_GIZMO, FALSE, FALSE);
    }
}

static void sPlaceLoot(const WorldPlace & placeLoot, const Player & tPlayer, SNO snoTreasureClass, int32 nLevelAreaLevel)
{   LooteeInfo tLootee; tLootee.tDebugName = MarkerStringHash("Placed Loot");



    tLootee.snoTreasureClass = snoTreasureClass;

    tLootee.tLooteeParams.nLevel = nLevelAreaLevel; tLootee.flNoDropScalar = 0.0f; tLootee.hGBDebug = cghgbNone;

    LootSpecifierArray arLootSpecifiers;
    if (LootGetLootSpecifiers(tLootee, tPlayer.idPlayerACD, arLootSpecifiers, NULL, FALSE, 1, NULL))
    {   int32 nNumItems = arLootSpecifiers.GetArraySize();


        for (int32 i = 0; i < nNumItems; i++)
        {


            arLootSpecifiers[i].fAutoIdentify = TRUE;

            ACDID idACDItem = LootCreateItem(arLootSpecifiers[i], placeLoot, NULL);
            if (idACDItem != ACDID_INVALID)
            {
                ActorCommonData & tACDItem = ACDGet(idACDItem);
                tACDItem.SetAssignedHeroID(&tPlayer);
            }
        }



    }


}



void GizmoPlaceLootForScene(const SceneCommonData & tSCD, const Player & tPlayer)
{

    ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
    PRTransform transformSceneToWorld; SceneGetSceneToWorldTransform(tSCD.idSScene, transformSceneToWorld);
    WorldPlace placeMarker;
    placeMarker.idSWorld = tSCD.idSWorld;


    int32 nLevelAreaLevel = GameGetMonsterLevel();

    int32 nMarkerSets = pdefScene->arMarkerSets.GetArraySize();


    for (int32 i = 0; i < nMarkerSets; i++)
    {
        SNO snoMarkerSet = pdefScene->arMarkerSets[i];


        ScopedMarkerSetDefinitionLockR pdefMarkerSet(snoMarkerSet);
        GBID gbidLabel = pdefMarkerSet->nLabel; if (gbidLabel != -1 && !tSCD.listAppliedLabels.Find(gbidLabel))
        {
            continue;
        }



        for (int32 j = 0; j < pdefMarkerSet->nSpecialIndexCount; j++)
        {
            int32 nIndex = pdefMarkerSet->pwSpecialIndexList[j];




            const Marker & tMarker = pdefMarkerSet->tMarkerSet[nIndex];
            if (tMarker.eType == 0)
            {

                GizmoType eGizmoType = (GizmoType)ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_TYPE, tMarker.snoname.snoHandle);
                if (eGizmoType == ACTORGIZMO_PLACED_LOOT)
                {
                    ScopedActorDefinitionLockR pdefActor(tMarker.snoname.snoHandle);
                    SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
                    if (snoTreasureClass != SNO_NONE)
                    {

                        placeMarker.wp = transformSceneToWorld * tMarker.transform.wp; sPlaceLoot(placeMarker, tPlayer, snoTreasureClass, nLevelAreaLevel);
                    }
                }
            }
        }
    }




    POS pos = tSCD.listGizmoLocLoot.GetHeadPosition();
    while (pos)
    {


        GizmoLocPlacement & tPlacement = (GizmoLocPlacement &)tSCD.listGizmoLocLoot.GetNext(pos);
        const SNOName & snoname = tPlacement.snoname;
        ScopedActorDefinitionLockR pdefActor(snoname.snoHandle); SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
        if (snoTreasureClass != SNO_NONE)
        {
            placeMarker.wp = transformSceneToWorld * tPlacement.tPlace.transform.wp; sPlaceLoot(placeMarker, tPlayer, snoTreasureClass, nLevelAreaLevel);
        }
    }
}



void GizmoReturnPointPortalInitialize(ActorCommonData & tACD)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.idACDReturnPointPortal = tACD.id;







    sUpdateActiveReturnPortal();
}
void GizmoSendSavePointInfoToPlayer(const PlayerList & listDestPlayers, const SavePointData & tSavePoint)
{



    if (!tSavePoint.IsValid())
    {
        return;
    }

    WorldPlace placeSavePoint;
    if (SWorldResolveLocationFromSavePointData(&tSavePoint, placeSavePoint))
    {
        NavMeshHandle tNavMeshHandle;
        ScenesCommonNavMeshNodeFromPlace(placeSavePoint, &tNavMeshHandle, TRUE, FALSE);
        SNO snoLevelArea = ScenesCommonLevelAreaFromPlace(tNavMeshHandle.idSScene, placeSavePoint.wp, FALSE);
        if (snoLevelArea != SNO_NONE)
        {
            SavePointInfoMessage tMessage;
            tMessage.snoLevelArea = snoLevelArea;
            MessageSendToClient(listDestPlayers, SMSG_SAVEPOINT_INFO, &tMessage, sizeof(tMessage));
        }
    }
}
void SetCurrentSavePointPortalReturn(const ActorCommonData & tACDDestination)
{
    SSceneID idSScene = ACDGetCurrentSSceneID(tACDDestination);

    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);

    SWorld * ptSWorld = SWorldGet(tSCD.idSWorld);
    SNO snoWorld = WorldCommonGetWorldSNO(ptSWorld->id);
    if (snoWorld == SNO_NONE)
    {
        return;
    }


    LevelAreaID idLevelArea = ACDGetLevelAreaID(tACDDestination);
    if (idLevelArea == -1)
    {
        return;
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    ActorCommonData * ptACDOldPortal = ACDTryToGet(tSGameGlobals.tReturnPoint.idACDReturnPortal);


    if (ptACDOldPortal)
    {   ActorID idActorOldPortal = (ActorID)ptACDOldPortal->idOwner; BrainAPIDoDeath(idActorOldPortal, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }


    tSGameGlobals.tReturnPoint.Reset();
    tSGameGlobals.tReturnPoint.snoWorld = snoWorld;
    tSGameGlobals.tReturnPoint.idACDReturnPortal = tACDDestination.id;


    LevelArea & tLevelArea = LevelAreaGet(idLevelArea);
    ReturnPointInfoMessage tMessage;
    tMessage.snoLevelArea = tLevelArea.snoLevelArea;

    PlayerList listAllPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listAllPlayers, -1, FALSE);
    MessageSendToClient(listAllPlayers, SMSG_RETURNPOINT_INFO, &tMessage, sizeof(tMessage));


    sUpdateActiveReturnPortal();
}



void OnSavePointPortalReturnDeleted(ActorCommonData & tACDPortal)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.tReturnPoint.idACDReturnPortal == tACDPortal.id)
    {



        tSGameGlobals.tReturnPoint.Reset();

        ReturnPointInfoMessage tMessage;
        tMessage.snoLevelArea = SNO_NONE;
        PlayerList listAllPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listAllPlayers, -1, FALSE);
        MessageSendToClient(listAllPlayers, SMSG_RETURNPOINT_INFO, &tMessage, sizeof(tMessage));



        sUpdateActiveReturnPortal();
    }
}



void GizmoSetHearthPortal(Player & tPlayer, const WorldPlace & placeReturn, const BOOL bSendToClient)
{




    tPlayer.tHearthPortalInfo.place = placeReturn;
    tPlayer.tHearthPortalInfo.snoLastWaypointLevelArea = placeReturn.idSWorld != SWORLDID_INVALID ? tPlayer.snoLastWaypointLevelArea : SNO_NONE;
    if (bSendToClient)
    {
        GizmoSendHearthPortalToPlayer(tPlayer, ACDID_INVALID);
    }
}











void GizmoSendHearthPortalToPlayer(
    Player & tPlayer,
    const ACDID idACDPortal)
{
    WorldPlace placeReturn = tPlayer.tHearthPortalInfo.place;

    SNO snoLevelArea = SNO_NONE;
    if (placeReturn.idSWorld != SWORLDID_INVALID)
    {
        NavMeshHandle tNavMeshHandle;
        if (ScenesCommonNavMeshNodeFromPlace(placeReturn, &tNavMeshHandle, TRUE, FALSE))
        {
            snoLevelArea = ScenesCommonLevelAreaFromPlace(tNavMeshHandle.idSScene, placeReturn.wp, FALSE);
        }
    }

    tPlayer.tHearthPortalInfo.snoPortalLevelArea = snoLevelArea;

    HearthPortalInfoMessage tMessage;
    tMessage.snoLevelArea = snoLevelArea;
    tMessage.annPortal = ANNFromACD(idACDPortal);
    tMessage.bResetUI = TRUE;
    tMessage.bIsTieredRift = SWorldIsDungeonFinder(placeReturn.idSWorld) && NephalemRift::IsTieredRiftActive();


    PlayerList listSinglePlayer(PlayerListGetElementPool());
    PlayerListGetSingle(listSinglePlayer, tPlayer);
    MessageSendToClient(listSinglePlayer, SMSG_HEARTHPORTAL_INFO, &tMessage, sizeof(tMessage));
}



void SGizmoAddStaticGizmoPing(ActorCommonData & tACDGizmo)
{
    GizmoType eGizmoType = tACDGizmo.GetGizmoType();
    if (GizmoUsesStaticMinimapPing(eGizmoType))
    {
        Minimap::AddStaticGizmoMarker(tACDGizmo);
    }
}
