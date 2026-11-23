// Original file: X/Server/AI/SpawnerBrain.cpp




#include "./SpawnerBrain.h"
#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./ProximityDetectorBrain.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/DRLG.h"
#include "../../Common/GameBalance.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Monsters.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"

BOOL FindGBHandleByActor(SNO snoActor, GBHandle & gbHandle);  // X/Common/Definitions/GameBalanceDefinition.cpp:3415
void ProximityInitializeShapeData(ActorID idActor, ProximityShapeData & tShape);  // ProximityDetectorBrain.cpp:89
void ProximityDetectorGetActorsInBoundsList(ActorID idTrigger, XPooledDataIDList & tListDetectedACDs, const ProximityShapeData & tShape, BOOL bPlayersOnly);  // ProximityDetectorBrain.cpp:609

struct SpawnBrainData {  // size 0xb8
    uint32 dwNextSpawnStartTime;  // 0x00
    uint32 dwDoSpawnTick;  // 0x04
    uint32 dwDoSpawnCount;  // 0x08
    uint32 dwCloseTick;  // 0x0c
    uint32 dwDeathTick;  // 0x10
    GBHandle hGB;  // 0x14
    SNOName tSNOName;  // 0x1c
    BOOL bSpawnTriggered;  // 0x24
    int32 nGenerated;  // 0x28
    LevelAreaID idLevelArea;  // 0x2c
    SNO snoEncounter;  // 0x30
    int32 nSpawnLevel;  // 0x34
    BOOL bDelayTimerSet;  // 0x38
    BOOL bSpawnedFirstTime;  // 0x3c
    BOOL bScannedForSpawnedOnReactivation;  // 0x40
    MonsterRarity eMonsterRarity;  // 0x44
    Vector3D wpSpawnPos;  // 0x48
    GBID gbidItem;  // 0x54
    BOOL bItemSpawnerRotateInGroup;  // 0x58
    BOOL bItemSpawnerCurrent;  // 0x5c
    ProximityShapeData tShape;  // 0x60
    GBID arFixedAffixes[4];  // 0x90
    GBID arLockedAffixes[4];  // 0xa0
    int32 nAdditionalAffixCount;  // 0xb0
    BOOL bLockRandomAffixOnSpawn;  // 0xb4
};
static_assert(sizeof(SpawnBrainData) == 0xb8, "SpawnBrainData");

typedef XGrowableMap<SpawnerCountID, SpawnerCountID, unsigned int, unsigned int> SpawnerCountMap;

struct SpawnerGlobals {  // size 0x88
    SpawnerCountMap tSpawnerCountMap;  // 0x00
    uint32 uNextSpawnerCount;  // 0x80
};
static_assert(sizeof(SpawnerGlobals) == 0x88, "SpawnerGlobals");



#include "../../Common/AnimPlayer.h"
#include "../../Common/Gizmo.h"
#include "../../Common/Player.h"
#include "../../Common/GameConstants.h"
#include "./BrainComponents.h"
#include <limits.h>
BOOL ACDIsInDeactivatedScene(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:4934
void BrainAPIActorScanAwarenessForTargets(ACDID idACD, XPooledDataIDList & listACDs);  // X/Server/AI/BrainComponents.cpp:8232
void SpawnBrainResetSpawnStartTime(ActorID idActor);  // defined at X/Server/AI/SpawnerBrain.cpp:1438; sStartSpawn (line 708) inlines it
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Nav/NavCellHandle.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/WorldCommonCollision.h"
#include "../Loot/Loot.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Hyperlink.h"
#include "AIStates.h"
#include "../Actors/ActorEffect.h"
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName);  // X/Server/Marker/SMarker.cpp:1231
Angle ACDGetYaw(ACDID idACD);  // X/Common/ActorCommonData.cpp:2997
BOOL ActorGetSpawnParams(ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:5239
ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);  // X/Server/Actors/Actors.cpp:5915
void EncounterAddByLevelArea(SNO snoEncounter, ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:5723
BOOL FlippyFindLandingLocation(const ActorCommonData & tACDLaunching, const SNO snoActorFlippy, WorldPlace & tPlaceLanding, const Player * ptAssignedPlayer, const Vector3D * pvBias, ACDID idACDKiller);  // X/Server/Flippies.cpp:831
void FlippyDropCreateOnActor(const ACDID idACDDropper, const ACDID idACDItem, const WorldPlace & tPlace);  // X/Server/Flippies.cpp:1180
void AISetTether(ActorID idActor, TetherType eTetherType, float flRadius, const WorldPlace & placeTetherPoint);  // X/Server/AI/ActorAI.cpp:3884
const uint32 SPAWNER_COUNT_MAP_POOL_SIZE = 256;  // DWARF line 100; the value is the literal SpawnBrainInitForGame passes to SetHashTableSize_Tracked
BOOL ACDCanBeAffectedByPowers(const ACDID idACD);  // X/Server/AI/ActorAI.cpp:1147
PowerID PowerAPI_StartSpawnAnimPower(const ACDID idACD, uint32 dwAnimTag);  // X/Server/Powers/PowerAPI.cpp:12673
void BrainAPIInheritTargetList(const Actor & tActorFrom, Actor & tActorTo);  // X/Server/AI/BrainComponents.cpp:10506
#include "../../Common/GizmoSwig.h"  // ACTORGIZMO_UNDEFINED (line 592)
#include "../../Common/Actors/ActorEffectCommon.h"  // EFFECT_PLAY_EFFECT_GROUP (line 824)
#include "../../Common/ActorCommonDataSwig.h"  // TEAM_NEUTRAL (line 543)

void SpawnBrainInitForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptSpawnerGlobals = (SpawnerGlobals *)SigmaMemoryNew(sizeof(SpawnerGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    tSGame.ptSpawnerGlobals->uNextSpawnerCount = 0;
    tSGame.ptSpawnerGlobals->tSpawnerCountMap.Initialize(16, FALSE, tSGame.ptMemoryInterface);
    tSGame.ptSpawnerGlobals->tSpawnerCountMap.SetHashTableSize_Tracked(SPAWNER_COUNT_MAP_POOL_SIZE);
}



void SpawnBrainDisposeForGame()

{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptSpawnerGlobals->tSpawnerCountMap.Dispose();
    XFREE(tSGame.ptSpawnerGlobals, tSGame.ptMemoryInterface);
}



static SpawnBrainData & sSpawnerBrainDataGet(Actor * ptActorSpawner)
{

    ActorAI & tAI = *ptActorSpawner->ptAI;


    SpawnBrainData * ptSpawnBrainData = (SpawnBrainData *)tAI.pBrainData;

    return *ptSpawnBrainData;
}




static void sSpawnerBrainDataInitialize(SpawnBrainData & tSpawnerBrainData)
{

    tSpawnerBrainData.dwNextSpawnStartTime = (uint32)-1;
    tSpawnerBrainData.dwDoSpawnTick = (uint32)-1;
    tSpawnerBrainData.dwDoSpawnCount = 0;
    tSpawnerBrainData.dwCloseTick = (uint32)-1;
    tSpawnerBrainData.dwDeathTick = (uint32)-1;

    tSpawnerBrainData.hGB.eType = -1; tSpawnerBrainData.hGB.gbid = -1; tSpawnerBrainData.tSNOName.Set(SNOGROUP_NONE, SNO_NONE);

    tSpawnerBrainData.bSpawnTriggered = FALSE; tSpawnerBrainData.nGenerated = 0;

    tSpawnerBrainData.idLevelArea = -1; tSpawnerBrainData.snoEncounter = SNO_NONE;


    tSpawnerBrainData.nSpawnLevel = 0; tSpawnerBrainData.bDelayTimerSet = FALSE; tSpawnerBrainData.bSpawnedFirstTime = FALSE; tSpawnerBrainData.bScannedForSpawnedOnReactivation = FALSE; tSpawnerBrainData.eMonsterRarity = 0; tSpawnerBrainData.wpSpawnPos = Vector3D(0.0f, 0.0f, 0.0f);




    tSpawnerBrainData.gbidItem = -1;
    tSpawnerBrainData.bItemSpawnerRotateInGroup = FALSE; tSpawnerBrainData.bItemSpawnerCurrent = FALSE;



    for (int32 i = 0; i < 4; i++) { tSpawnerBrainData.arFixedAffixes[i] = -1; tSpawnerBrainData.arLockedAffixes[i] = -1; }



    tSpawnerBrainData.nAdditionalAffixCount = 0; tSpawnerBrainData.bLockRandomAffixOnSpawn = FALSE;
}




































void sIncConcurrentCount(SpawnerCountID idConcurrentCount)

{


    SGame & tSGame = SGameManager::GameGet();
    SpawnerCountMap & tMap = tSGame.ptSpawnerGlobals->tSpawnerCountMap;

    uint32 uCount;
    if (tMap.Lookup(idConcurrentCount, uCount))
    {
        uCount++;
    }
    else { uCount = 1; }

    tMap.SetAt_Tracked(idConcurrentCount, uCount);
}



void sDecConcurrentCount(SpawnerCountID idConcurrentCount)

{


    SGame & tSGame = SGameManager::GameGet();
    SpawnerCountMap & tMap = tSGame.ptSpawnerGlobals->tSpawnerCountMap;

    uint32 uCount;
    if (tMap.Lookup(idConcurrentCount, uCount))
    {
        if (uCount > 0)
        {
            uCount--;
        }

        if (uCount == 0)
        {
            tMap.RemoveAt(idConcurrentCount);
        }
        else
        {
            tMap.SetAt_Tracked(idConcurrentCount, uCount);
        }
    }
}



uint32 sGetConcurrentCount(
    SpawnerCountID idConcurrentCount)
{
    if (idConcurrentCount == SPAWNERCOUNTID_INVALID)
        return 0;
    SGame & tSGame = SGameManager::GameGet();
    SpawnerCountMap & tMap = tSGame.ptSpawnerGlobals->tSpawnerCountMap;

    uint32 uCount;
    if (tMap.Lookup(idConcurrentCount, uCount))
        return uCount;

    return 0;
}





// only sStartSpawn inlines this; its inlined copy (chunk H02) is identical to the original
static SpawnerCountID sGetSpawnerConcurrentCountID(
    ActorCommonData & tACDSpawner)
{
    SpawnerCountID idSpawnerCount = (SpawnerCountID)tACDSpawner.AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
    if (idSpawnerCount == SPAWNERCOUNTID_INVALID)
    {
        SGame & tSGame = SGameManager::GameGet();
        idSpawnerCount = (SpawnerCountID)tSGame.ptSpawnerGlobals->uNextSpawnerCount;
        tSGame.ptSpawnerGlobals->uNextSpawnerCount++;
        tACDSpawner.AttributesSetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID, idSpawnerCount);
    }
    return idSpawnerCount;
}




static void sSetNextSpawnTime(
    Actor * ptActorSpawner)
{
    int32 nSpawnTimeMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_MIN, TAG_MARKER_SPAWN_TIME_MIN);
    int32 nSpawnTimeDelta = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_DELTA, TAG_MARKER_SPAWN_TIME_DELTA);
    int32 nSpawnDelay = RandRange_Internal(SGameGlobalsGet().rng, nSpawnTimeMin, nSpawnTimeDelta);

    uint32 dwNow = GameGetServerTime();
    int32 nTimeDiff = 0;

    SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);
    if (tSpawnerBrainData.dwNextSpawnStartTime != (uint32)-1)
    {   // line 312 column 34 puts the subtraction after a 7-character prefix: (int32) casts
        nTimeDiff = (int32)dwNow - (int32)tSpawnerBrainData.dwNextSpawnStartTime;

        // line 315 columns 24 (<) and 44 (&&) fit the parenthesised first test and TICKS_PER_SECOND
        if ((nTimeDiff < TICKS_PER_SECOND) &&
            nTimeDiff < nSpawnDelay)
            nTimeDiff = -nTimeDiff;
        else
            nTimeDiff = 0;
    }
    // line 322 column 17 is a compound add on nSpawnDelay; line 323 columns 44 (=) and 52 (+)
    nSpawnDelay += nTimeDiff;
    tSpawnerBrainData.dwNextSpawnStartTime = dwNow + nSpawnDelay;
    // the DWARF declares tAppGlobals here (no code survives: presumably a compiled-out trace)
    AppGlobals & tAppGlobals = sgtAppGlobals;





}
// NON_MATCHING: sStartSpawn is 6832 bytes, the original size. Only the loop preheader (line 429) differs: the three hoisted addresses
// (&tActorAdd.tNavMeshHandle, &tActorAdd.hGB, &tSpecifier.hGBItem) and their spills come out in the reverse order (scheduling only).

static BOOL sStartSpawn(
    Actor * ptActorSpawner,
    int32 nCount,
    BOOL bTryToPlayOpeningAnimation)
{


	ACDID idACDSpawner = ptActorSpawner->idACD;
	ActorCommonData & tACDSpawner = ACDGet(idACDSpawner);
	WorldPlace tPlaceSpawner = tACDSpawner.GetPlace();
	Quaternion qOrientationSpawner = tACDSpawner.GetOrientation();
	SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);
	SSceneID idSScene = ACDGetCurrentSSceneID(tACDSpawner);
	Team eTeam = tACDSpawner.GetTeam();
    uint32 dwNow = GameGetServerTime();
    // 351-366: the names are padded to the call columns (the original alignment is not recoverable)
	BOOL        bExactLocation = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_EXACT_LOCATION, TAG_MARKER_SPAWN_EXACT_LOCATION);
	uint32 dwSpawnStayOpenTime = ActorGetInheritedTagMapValueDWORD(ptActorSpawner, TAG_ACTOR_SPAWN_STAY_OPEN_TIME, TAG_MARKER_SPAWN_STAY_OPEN_TIME);
	int32 nMaxGenerated = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_MAX_GENERATED, TAG_MARKER_SPAWN_MAX_GENERATED);
	int32 nMaxConcurrent = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_MAX_COUNT, TAG_MARKER_SPAWN_MAX_COUNT);
	BOOL           bSpawnNear = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_NEAR, TAG_MARKER_SPAWN_NEAR);
	BOOL           bSpawnedGivesLoot = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_SPAWNED_MONSTERS_GIVE_LOOT, TAG_MARKER_SPAWN_SPAWNED_MONSTERS_GIVE_LOOT);
	BOOL           bSpawnedGivesXP = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_SPAWNED_MONSTERS_GIVE_XP, TAG_MARKER_SPAWN_SPAWNED_MONSTERS_GIVE_XP);
	BOOL           bSpawnedInheritsTargetList = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_SPAWNED_MONSTERS_INHERIT_TARGET_LIST, TAG_MARKER_SPAWN_SPAWNED_MONSTERS_INHERIT_TARGET_LIST);
	BOOL           bSpawnedAIStartsInSpecialState = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_SPAWNED_MONSTERS_START_IN_SPECIAL_STATE, TAG_MARKER_SPAWN_SPAWNED_MONSTERS_START_IN_SPECIAL_STATE);
	AIStateType     eStartAIState = (AIStateType)ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_SPAWNED_MONSTERS_AI_STATE, TAG_MARKER_SPAWN_SPAWNED_MONSTERS_AI_STATE);
	SNO    snoEffectGroupActorToActor = ActorGetInheritedTagMapValueSNO(ptActorSpawner, TAG_ACTOR_SPAWN_EFFECT_ACTOR_TO_ACTOR, TAG_MARKER_SPAWN_EFFECT_ACTOR_TO_ACTOR);
	float    flTetherRadius = ActorGetInheritedTagMapValueFloat(ptActorSpawner, TAG_ACTOR_AI_TETHER_RADIUS, TAG_MARKER_AI_TETHER_RADIUS);
	TetherType    eTetherType = (TetherType)ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_AI_TETHER_TYPE, TAG_MARKER_AI_TETHER_TYPE);
	int32    dwSpawnDelayBeforeDeath = ActorGetInheritedTagMapValueDWORD(ptActorSpawner, TAG_ACTOR_SPAWN_DELAY_BEFORE_DEATH, TAG_MARKER_SPAWN_DELAY_BEFORE_DEATH);
	WeaponClass     eStartWeaponClass = (WeaponClass)ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_SPAWNED_MONSTERS_WEAPON_CLASS, TAG_MARKER_SPAWN_SPAWNED_MONSTERS_WEAPON_CLASS);
	BOOL          bUseLinkedLocations = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_USE_LINKED_MARKERS, TAG_MARKER_SPAWN_USE_LINKED_MARKERS);

    GBHandleList listGroupAffixes(GBGetHandlePool());
    EncounterAddStruct tEncounterAdd;
    EncounterAddInitialize(tEncounterAdd, listGroupAffixes);



    ScopedActorDefinitionLockR ptDefActor(ptActorSpawner->snoActor);
    uint32 dwSpawnAnimTag = TagMapGetDWORD(ptDefActor->hTagMap, TAG_ACTOR_GIZMO_SPAWNED_MONSTER_ANIMTAG);
    float flDesiredSpawnOffset = TagMapGetFloat(ptDefActor->hTagMap, TAG_ACTOR_SPAWN_DESIRED_OFFSET);
    SNO snoEffectGroupCast = TagMapGetSNO(ptDefActor->hTagMap, TAG_ACTOR_SPAWN_CAST_EFFECT);


	if (bTryToPlayOpeningAnimation == TRUE)
	{
		uint32 dwOpeningAnimTag = TAG_ACTOR_ANIMATION_OPENING;
		if (AnimPlayerAnimExists(tACDSpawner, dwOpeningAnimTag))
		{

			AnimPlayerPlayAnimation(tACDSpawner, dwOpeningAnimTag, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_SPAWN, 0.0f, FALSE, 1.0f);


			AnimationTiming tAnimationTiming;
			AnimPlayerGetAnimationTiming(tACDSpawner, dwOpeningAnimTag, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);

            if (AnimPlayerAnimExists(tACDSpawner, TAG_ACTOR_ANIMATION_OPEN))
            {

                AnimPlayerSetIdleAnimation(tACDSpawner, TAG_ACTOR_ANIMATION_OPEN);
            }



            tSpawnerBrainData.dwDoSpawnTick = dwNow + tAnimationTiming.dwExecuteTime[0];


            tSpawnerBrainData.dwCloseTick = dwNow + tAnimationTiming.dwCycleTime + dwSpawnStayOpenTime;

            tACDSpawner.GetContactOffset(tSpawnerBrainData.wpSpawnPos, 0);

			tSpawnerBrainData.dwDoSpawnCount = nCount;
			return FALSE;
		}
	}



    XPooledDataIDList listLinkedACDs(MemoryPoolsGetDATAIDListPool());
    if (bUseLinkedLocations)
    {
        MarkerGetACDsLinkedByActor(ptActorSpawner->id, listLinkedACDs, NULL);
        if (listLinkedACDs.Count() <= 0)
            bUseLinkedLocations = FALSE;
    }


    // 424 and 851: condition column 10 with the statement at 9 (inferred: a space inside the parentheses)
    if ( bSpawnNear )
    {
        ActorPushSpawnSkipDistance(0, 0);
    }

	for (int32 i = 0; i < nCount; i++)
	{
		ActorIDListPooled listActors(MemoryPoolsGetDATAIDListPool());

		Quaternion qOrientation;
		qOrientation.FromYaw(RandAngle(SGameGlobalsGet().rng));

		ActorCommonData * ptACDLocation = NULL;
        if (bUseLinkedLocations)
        {
            int32 _nCount = listLinkedACDs.Count();
            int32 nRandIdx = RandRange_Internal(SGameGlobalsGet().rng, 0, _nCount - 1);
            int32 nCurrCount = 0;
            POS pos = listLinkedACDs.GetHeadPosition();
            while (pos)
            {
                ACDID idACDCurr = (ACDID)listLinkedACDs.GetNext(pos);
                if (nCurrCount >= nRandIdx)  // columns 21 (condition) and 32 (>=); the add before the compare is the ++ on 451
                {
                    ptACDLocation = ACDTryToGet(idACDCurr);
                    break;
                }
                nCurrCount++;
            }
        }


        ActorAddStruct tActorAdd;
        if (ptACDLocation)
        {
            tActorAdd.tPlace = ptACDLocation->GetPlace();
            tActorAdd.qOrientation = ptACDLocation->GetOrientation();
        }
        else
        {
            tActorAdd.tPlace = tPlaceSpawner;
            tActorAdd.tPlace.wp = tSpawnerBrainData.wpSpawnPos;
            tActorAdd.qOrientation = qOrientation;
        }
        tActorAdd.idMarkersSScene = idSScene;
        tActorAdd.eTeam = eTeam;
        tActorAdd.tMarkersName = MarkerStringHash("Spawned");
        tActorAdd.plistActors = &listActors;
        tActorAdd.dwSpawnAnimTag = dwSpawnAnimTag;
        tActorAdd.uPrimaryGroup = tACDSpawner.uGroupPrimary;
        tActorAdd.uSecondaryGroup = tACDSpawner.uGroupSecondary;
        tActorAdd.bIsSummon = TRUE;

        BOOL bHasLockedAffixes = FALSE;
        for (int32 j = 0; j < 4; j++)
        {
            GBID gbidLockedAffix = tSpawnerBrainData.arLockedAffixes[j];
            if (gbidLockedAffix != GBID_NONE)
            {
                tActorAdd.pMonsterSpawnerAffixGBIDs[j] = gbidLockedAffix;
                bHasLockedAffixes = TRUE;
            }
        }


        if (!bHasLockedAffixes)
        {
            tActorAdd.pMonsterSpawnerAffixGBIDs[0] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX0, TAG_MARKER_SPAWN_FIXED_AFFIX0);
            tActorAdd.pMonsterSpawnerAffixGBIDs[1] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX1, TAG_MARKER_SPAWN_FIXED_AFFIX1);
            tActorAdd.pMonsterSpawnerAffixGBIDs[2] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX2, TAG_MARKER_SPAWN_FIXED_AFFIX2);
            tActorAdd.pMonsterSpawnerAffixGBIDs[3] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX3, TAG_MARKER_SPAWN_FIXED_AFFIX3);
            tActorAdd.nSpawnerAdditionalAffixes = tSpawnerBrainData.nAdditionalAffixCount;
        }

        BOOL bOrientationSet = FALSE;

        if (flDesiredSpawnOffset > EPSILON)
        {
            BOOL bSuccessfullyFoundSpot = FALSE;

            ActorMovement *ptMove = tACDSpawner.GetActorMovement();
            if (ptMove)
            {
                float flDesiredSpawnOffsetArc = TagMapGetFloat(ptDefActor->hTagMap, TAG_ACTOR_SPAWN_ARC);
                float flStartAngle = flDesiredSpawnOffsetArc * RandFloat_Internal(SGameGlobalsGet().rng);  // operand order from the fmul; columns 38/71 not reproduced
                float flBaseAngle = ACDGetYaw(tACDSpawner.id);

                const float SPAWN_OFFSET_ANGLE_INC = EIGHTH_CIRCLE * 0.5f;  // value pi/8 (0.3926991) from the rodata; the spelling is inferred
                const float SPAWN_SEARCH_RADIUS = 5.0f;
                for (float flAngle = 0.0f; flAngle <= flDesiredSpawnOffsetArc; flAngle += SPAWN_OFFSET_ANGLE_INC)
                {
                    float flCheckAngle = Math::NormalizeAngle(flAngle + flStartAngle + flBaseAngle);
                    WorldPlace placeCheck(tPlaceSpawner);
                    placeCheck.wp.x += Math::Cos(flCheckAngle) * flDesiredSpawnOffset;
                    placeCheck.wp.y += Math::Sin(flCheckAngle) * flDesiredSpawnOffset;

                    float flRadius;
                    if (ptDefActor->tCollData.eCollisionShape == COLLISION_SHAPE_CYLINDER)
                    {
                        flRadius = ActorGetCollisionCylinderRadius(ptDefActor);
                    }
                    else
                    {
                        flRadius = ptDefActor->wcylBounds.wdRadius;
                    }
                    flRadius = Math::Max(0.01f, flRadius);

                    Vector3D vFoundLoc(placeCheck.wp);
                    NavCellHandle hNavCellTarget;
                    BOOL bFoundDest = WorldCollSearchForLocation(
                        placeCheck,
                        (uint16)ptDefActor->tCollData.tCollFlags.uStaticOneRequiredFlags,
                        0x60,
                        flRadius,
                        0.0f,
                        Math::Max(flRadius + 1.0f, SPAWN_SEARCH_RADIUS),
                        ACDID_INVALID,
                        vFoundLoc,
                        hNavCellTarget,
                        TEAM_NEUTRAL, 0, ACDID_INVALID);
                    if (bFoundDest)
                    {
                        bSuccessfullyFoundSpot = TRUE;
                        tActorAdd.tPlace.wp = vFoundLoc;
                        qOrientationSpawner.FromYaw(flCheckAngle);
                        bOrientationSet = TRUE;
                        bExactLocation = TRUE;
                        break;
                    }
                }
            }




            if (!bSuccessfullyFoundSpot) continue;

        }



		if (bExactLocation)
		{
			tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK);
			tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_PERTURB);
			tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
            if (ptACDLocation == NULL)
            {
                tActorAdd.qOrientation = qOrientationSpawner;
            }
		}

        tActorAdd.eMonsterRarity = tSpawnerBrainData.eMonsterRarity;
        // 578: the compares sit at column 13 (condition start) with the load at 40, so this is an if chain folded into a switch, not a switch statement
        if (tSpawnerBrainData.tSNOName.snoGroup == MONSTER_SNO)
        {


                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        else if (tSpawnerBrainData.tSNOName.snoGroup == ACTOR_SNO)
        {
                tActorAdd.tSNOName = tSpawnerBrainData.tSNOName;
                tActorAdd.snoActor = tSpawnerBrainData.tSNOName.snoHandle;

                ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

                GizmoType eGizmoType = ActorGetGizmoType(tSpawnerBrainData.tSNOName.snoHandle);
                if (eGizmoType != ACTORGIZMO_UNDEFINED)
                {
                    if ( ActorGetSpawnParams(tActorAdd) )
                    {
                        ActorAdd(tActorAdd);
                    }
                    else
                    {
                        CRefString sSpawner = HyperlinkCreateFromSNO(SNOName(ACTOR_SNO, ptActorSpawner->snoActor), NULL);
                        CRefString sActor = HyperlinkCreateFromSNO(tSpawnerBrainData.tSNOName, NULL);

                    }
                }
                else
                {
                    ActorID idActorSpawn = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
                    if (tSpawnerBrainData.bLockRandomAffixOnSpawn && !bHasLockedAffixes)
                    {
                        ActorCommonData * ptACDSpawn = ACDTryToGet(ACDFindByActor(idActorSpawn));
                        if (ptACDSpawn)
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                tSpawnerBrainData.arLockedAffixes[j] = ptACDSpawn->pMonsterAffixGBIDs[j];
                            }
                        }
                    }
                }

        }


        else if (tSpawnerBrainData.tSNOName.snoGroup == SNOGROUP_NONE)
        {


            if (tSpawnerBrainData.hGB.eType != -1)  // the GB handle is valid; no named constant for -1 found
            {
            tActorAdd.hGB = tSpawnerBrainData.hGB;

            if (!GBIDExists(tActorAdd.hGB))
                return TRUE;
                ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

							GBAdd(tActorAdd);
            }









		else if (tSpawnerBrainData.snoEncounter != SNO_NONE)
		{
			tActorAdd.idLevelArea = tSpawnerBrainData.idLevelArea;

			EncounterAddByLevelArea(
				tSpawnerBrainData.snoEncounter,
				tActorAdd);

			tSpawnerBrainData.nSpawnLevel += 1;
			tSpawnerBrainData.nSpawnLevel = Math::Pin(tSpawnerBrainData.nSpawnLevel, -1, 17);
		}
		else if (tSpawnerBrainData.gbidItem != GBID_NONE)
		{

            if (tSpawnerBrainData.bItemSpawnerRotateInGroup)
            {
                ACDGlobals *ptACDGlobals = GameGetActiveGameCommonData().ptACDGlobals;
                ACDGroup tACDGroup;
                if (ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(tACDSpawner.uGroupPrimary, tACDGroup))
                {
                    if (tACDGroup.ptList)
                    {
                        ACDID idACDActiveSpawner = ACDID_INVALID;
                        ACDID idACDNext = idACDSpawner;  // the original loads idACDSpawner here (both before the loop and for an empty list)
                        POS pos = tACDGroup.ptList->GetHeadPosition();
                        while (pos)
                        {
                            ACDID idACDGroupMember = tACDGroup.ptList->GetNext(pos);
                            Actor *ptActorGroupMember = ActorTryToGet((ActorID)ACDGetOwner(idACDGroupMember));
                            if (ptActorGroupMember)
                            {
                                SpawnBrainData & tGroupMemberSpawnBrainData = sSpawnerBrainDataGet(ptActorGroupMember);
                                if (tGroupMemberSpawnBrainData.bItemSpawnerCurrent)
                                {
                                    idACDActiveSpawner = idACDGroupMember;

                                    if (pos)
                                    {
                                        idACDNext = tACDGroup.ptList->GetAt(pos);  // 685 reads the node without advancing pos (the loop re-reads it at 674) and has no inline record
                                    }
                                    else
                                    {
                                        idACDNext = tACDGroup.ptList->GetHead();
                                    }
                                }
                            }
                        }
                        // 695 column 65 is the ||; 704 column 50 puts the block one level deeper
                        if (idACDActiveSpawner == ACDID_INVALID || idACDActiveSpawner == idACDSpawner)
                        {







                            Actor *ptActorNext = ActorGet((ActorID)ACDGetOwner(idACDNext));
                            SpawnBrainData & tSpawnerBrainDataNext = sSpawnerBrainDataGet(ptActorNext);
                            tSpawnerBrainData.bItemSpawnerCurrent = FALSE;
                            tSpawnerBrainDataNext.bItemSpawnerCurrent = TRUE;
                            SpawnBrainResetSpawnStartTime((ActorID)ACDGetOwner(idACDNext));
                            tSpawnerBrainDataNext.bDelayTimerSet = FALSE;
                        }
                        else return TRUE;
                    }
                }
            }
            GBHandle hGBToDrop; hGBToDrop.eType = GB_ITEMS;
            hGBToDrop.gbid = tSpawnerBrainData.gbidItem;


            LootSpecifier tSpecifier;
            tSpecifier.hGBItem = hGBToDrop;
            tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
            tSpecifier.eIQL = IQL_NORMAL;
            tSpecifier.fAutoIdentify = TRUE;
            tSpecifier.nGoldAmount = 0;
            tSpecifier.nAffixLevel = tACDSpawner.AttributesGetInt(ATTRIBUTE_LEVEL);
            LootDetermineInherentAffixes(tSpecifier);
            ACDID idACDItem = LootCreateItem(tSpecifier, cgplaceNull, NULL);
            if (idACDItem != ACDID_INVALID)
            {
                ActorCommonData & tACDItem = ACDGet(idACDItem);
                WorldPlace tPlace;
                if (FlippyFindLandingLocation(tACDSpawner, tACDItem.snoActor, tPlace, NULL, NULL, ACDID_INVALID))
                {
                    FlippyDropCreateOnActor(tACDSpawner.id, idACDItem, tPlace);
                    listActors.AddTail_Tracked((ActorID)ACDGetOwner(idACDItem));
                }
                else
                {
                    ActorFlagForDeletion((ActorID)ACDGetOwner(tACDItem.id)); return TRUE;
                }
            }
            else
                return TRUE;
		}

        }

















        POS pos = listActors.GetHeadPosition();
		while (pos)
		{
            ActorID idSpawned = (ActorID)listActors.GetNext(pos);

            Actor *ptActorSpawned = ActorGet(idSpawned);
            ACDID idACDSpawned = ptActorSpawned->idACD;
            ActorCommonData & tACDSpawned = ACDGet(idACDSpawned);

            if (nMaxConcurrent > 0)
            {
                SpawnerCountID idConcurrentCount = sGetSpawnerConcurrentCountID(tACDSpawner);
                tACDSpawned.AttributesSetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID, idConcurrentCount);

                sIncConcurrentCount(idConcurrentCount);
            }


            if (!bSpawnedGivesLoot)

                tACDSpawned.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);

            if (!bSpawnedGivesXP)

                tACDSpawned.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);


            if (bSpawnedAIStartsInSpecialState)

                tACDSpawned.AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);


            if (eStartAIState >= 0 && eStartAIState < AISTATE_TYPE_COUNT)

                AIStateSetState(idSpawned, eStartAIState, FALSE);



            if (flTetherRadius > 0.0f)

                AISetTether(idSpawned, eTetherType, flTetherRadius, tPlaceSpawner);



            if (!tACDSpawned.IsItem() && !bOrientationSet)

                ActorSetOrientation(idSpawned, tACDSpawner.GetOrientation(), TRUE);


            if (ACDCanBeAffectedByPowers(idACDSpawned) && dwSpawnAnimTag == (uint32)-1)

            {

                PowerAPI_StartSpawnAnimPower(idACDSpawned, TAG_ACTOR_ANIMATION_GETTING_SUMMONED);
            }



            if (snoEffectGroupCast != SNO_NONE)

                ACDPlayEffect(tACDSpawner.id, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroupCast, ACDID_INVALID);

            if (snoEffectGroupActorToActor != SNO_NONE)

                ACDPlayEffectGroupACDToACD(snoEffectGroupActorToActor, tACDSpawner.id, idACDSpawned);


            if (bSpawnedInheritsTargetList)

                BrainAPIInheritTargetList(*ptActorSpawner, *ptActorSpawned);


            if (eStartWeaponClass >= 0 && eStartWeaponClass < NUM_WEAPON_CLASSES)

                tACDSpawned.AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, eStartWeaponClass);


            tACDSpawned.AttributesSetInt(ATTRIBUTE_SPAWNED_BY_ACDID, tACDSpawner.id);

            tACDSpawned.fHasSpawner = (tACDSpawner.id != ACDID_INVALID);
		}


        tSpawnerBrainData.nGenerated++;

	}

    if ( bSpawnNear )
    {
        ActorPopSpawnSkipDistance();
    }

	int32 nSpawnQuantityMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_QUANTITY_MIN, TAG_MARKER_SPAWN_QUANTITY_MIN);


	if (nMaxGenerated > 0 && (tSpawnerBrainData.nGenerated >= nMaxGenerated || (tSpawnerBrainData.nGenerated + nSpawnQuantityMin) > nMaxGenerated))
	{
        if (dwSpawnDelayBeforeDeath > 0)
        {
            tSpawnerBrainData.dwDeathTick = dwNow + dwSpawnDelayBeforeDeath;
        }
        else
        {
            BrainAPIDoDeath(ptActorSpawner->id, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
	}
	return TRUE;

}



static BOOL sSpawnActorScan(
    ActorID idActor)
{
	ACDID idACD = ActorGet(idActor)->idACD;

	XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
	BrainAPIActorScanAwarenessForTargets(idACD, listACDs);

	POS pos = listACDs.GetHeadPosition();
	while (pos)
	{
		ACDID idACDTarget = (ACDID)listACDs.GetNext(pos);
		// line 889 column 8 is the call one column after the condition start: a negated test
		if (!ACDIsPlayer(idACDTarget))
			continue;

		return TRUE;
	}

	return FALSE;

}




void sSetSpawnTimer(
    ActorID idActor,
    int32 nSpawnTimeMin,
    int32 nSpawnTimeDelta)
{
	Actor * ptActorSpawner = ActorGet(idActor);
	SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);

	uint32 dwNow = GameGetServerTime();


	if (nSpawnTimeMin != 0 || nSpawnTimeDelta != 0)
	{
		int32 nDelay = RandRange_Internal(SGameGlobalsGet().rng, nSpawnTimeMin, nSpawnTimeDelta);
		tSpawnerBrainData.dwNextSpawnStartTime = dwNow + nDelay;
	}
	else
	{
		tSpawnerBrainData.dwNextSpawnStartTime = dwNow;
	}
}





static void sSpawnTrigger(
    ActorID idActor)
{
	Actor * ptActorSpawner = ActorGet(idActor);
	SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);


	tSpawnerBrainData.bSpawnTriggered = TRUE;

	uint32 dwSpawnInitialDelayMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_INITIAL_DELAY_MIN, TAG_MARKER_SPAWN_INITIAL_DELAY_MIN);
	uint32 dwSpawnInitialDelayDelta = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_INITIAL_DELAY_DELTA, TAG_MARKER_SPAWN_INITIAL_DELAY_DELTA);


	sSetSpawnTimer(idActor, dwSpawnInitialDelayMin, dwSpawnInitialDelayDelta);
}



void SpawnBrainInitialize(ActorID idActor)

{
    Actor * ptActorSpawner = ActorGet(idActor);


    ActorAI & tAI = *ptActorSpawner->ptAI;



    tAI.pBrainData = SigmaMemoryNew(sizeof(SpawnBrainData), 4, SGameManager::GameGet().ptMemoryInterface, TRUE);
    SpawnBrainData & tSpawnerBrainData = *(SpawnBrainData *)tAI.pBrainData;


    sSpawnerBrainDataInitialize(tSpawnerBrainData);

    ProximityInitializeShapeData(idActor, tSpawnerBrainData.tShape);

    ActorCommonData & tACDSpawner = ACDGet(ptActorSpawner->idACD);
    tACDSpawner.GetContactOffset(tSpawnerBrainData.wpSpawnPos, 0);

    SNO snoMonsterActor = ActorGetInheritedTagMapValueSNO(ptActorSpawner, TAG_ACTOR_SPAWN_ACTOR_SNO, TAG_MARKER_SPAWN_ACTOR_SNO);
    tSpawnerBrainData.gbidItem = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_ITEM, TAG_MARKER_SPAWN_ITEM);
    tSpawnerBrainData.bItemSpawnerRotateInGroup = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_ITEM_ROTATE_IN_GROUP, TAG_MARKER_SPAWN_ITEM_ROTATE_IN_GROUP);

    tSpawnerBrainData.arFixedAffixes[0] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX0, TAG_MARKER_SPAWN_FIXED_AFFIX0);
    tSpawnerBrainData.arFixedAffixes[1] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX1, TAG_MARKER_SPAWN_FIXED_AFFIX1);
    tSpawnerBrainData.arFixedAffixes[2] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX2, TAG_MARKER_SPAWN_FIXED_AFFIX2);
    tSpawnerBrainData.arFixedAffixes[3] = ActorGetInheritedTagMapValueGBID(ptActorSpawner, TAG_ACTOR_SPAWN_FIXED_AFFIX3, TAG_MARKER_SPAWN_FIXED_AFFIX3);
    tSpawnerBrainData.nAdditionalAffixCount = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_ADDITIONAL_AFFIX_COUNT, TAG_MARKER_SPAWN_ADDITIONAL_AFFIX_COUNT);
    tSpawnerBrainData.bLockRandomAffixOnSpawn = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_LOCK_RANDOM_AFFIXES_ON_CREATE, TAG_MARKER_SPAWN_LOCK_RANDOM_AFFIXES_ON_CREATE);

    if (tSpawnerBrainData.bItemSpawnerRotateInGroup)
    {

        ACDGlobals * ptACDGlobals = GameGetActiveGameCommonData().ptACDGlobals;
        ACDGroup tACDGroup;
        if (ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(tACDSpawner.uGroupPrimary, tACDGroup))
        {
            if (tACDGroup.nCount == 1 && tACDGroup.ptList != NULL)
            {


                tSpawnerBrainData.bItemSpawnerCurrent = TRUE;
            }
        }
    }


    if (tACDSpawner.IsMonster())
    {
        snoMonsterActor = MonsterGetSummonActor(tACDSpawner.tSNOName, 0);
    }

    SNO snoEncounter = ActorGetInheritedTagMapValueSNO(ptActorSpawner, TAG_ACTOR_SPAWN_ENCOUNTER, TAG_MARKER_SPAWN_ENCOUNTER);

    tSpawnerBrainData.eMonsterRarity = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_RARITY, TAG_MARKER_SPAWN_RARITY);

    if (snoEncounter != SNO_NONE)
    {
        tSpawnerBrainData.snoEncounter = snoEncounter;
    }
    else if (snoMonsterActor != SNO_NONE)
    {

        GBHandle hGB;
        if (FindGBHandleByActor(snoMonsterActor, hGB))
        {
            tSpawnerBrainData.hGB = hGB;
        }
        else
        {




            tSpawnerBrainData.tSNOName.Set(ACTOR_SNO, snoMonsterActor);
        }
    }
}










void SpawnBrainDispose(ActorID idActor)

{
    Actor * ptActorSpawner = ActorGet(idActor);
    ActorAI & tAI = *ptActorSpawner->ptAI;

    XFREE(tAI.pBrainData, SGameManager::GameGet().ptMemoryInterface);
}












































static BOOL sSpawnerMaintainSpawnList(
    ActorID idActor)
{
	Actor * ptActorSpawner = ActorGet(idActor);
	SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);

	int32 nSpawnTimeMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_MIN, TAG_MARKER_SPAWN_TIME_MIN);
	int32 nSpawnTimeDelta = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_DELTA, TAG_MARKER_SPAWN_TIME_DELTA);
	int32 nMaxConcurrent = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_MAX_COUNT, TAG_MARKER_SPAWN_MAX_COUNT);
	BOOL bTimerIsADelay = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_IS_A_DELAY, TAG_MARKER_SPAWN_TIME_IS_A_DELAY);

	ACDID idACD = ACDFindByActor(idActor);
	ActorCommonData & tACD = ACDGet(idACD);
	SpawnerCountID idSpawnerCount = (SpawnerCountID)tACD.AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
	int32 nCurCount = sGetConcurrentCount(idSpawnerCount);


    if (tSpawnerBrainData.bItemSpawnerRotateInGroup)
    {
        ACDGlobals * ptACDGlobals = GameGetActiveGameCommonData().ptACDGlobals;
        ACDGroup tACDGroup;
        if (ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(tACD.uGroupPrimary, tACDGroup))
        {
            if (tACDGroup.ptList != NULL)
            {
                POS pos = tACDGroup.ptList->GetHeadPosition();
                while (pos)
                {
                    ACDID idACDGroupMember = tACDGroup.ptList->GetNext(pos);
                    ActorCommonData * ptACDGroupMember = ACDTryToGet(idACDGroupMember);
                    if (ptACDGroupMember)  // line 1121 (condition at column 29) sits one level deeper than 1117
                    {
                        SpawnerCountID idGroupMemberSpawnerCount = (SpawnerCountID)ptACDGroupMember->AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
                        if (idGroupMemberSpawnerCount != SPAWNERCOUNTID_INVALID && ptACDGroupMember->id != idACD)
                        {
                            nCurCount += sGetConcurrentCount(idGroupMemberSpawnerCount);
                        }
                    }
                }
            }
        }
    }

	if (nCurCount < nMaxConcurrent)
	{
		if (bTimerIsADelay && tSpawnerBrainData.bDelayTimerSet == FALSE && tSpawnerBrainData.bSpawnedFirstTime)
		{
			tSpawnerBrainData.bDelayTimerSet = TRUE;
			sSetSpawnTimer(idActor, nSpawnTimeMin, nSpawnTimeDelta);
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}






BOOL sSpawnCheckShape(ActorID idActor)

{
    Actor * ptActorSpawner = ActorGet(idActor);
    SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());


    ProximityDetectorGetActorsInBoundsList(idActor, listACDs, tSpawnerBrainData.tShape, TRUE);



    return listACDs.Count() > 0;
}



void SpawnBrainUpdate(
    ActorID idActor)
{

	if (ActorHasBeenKilled(idActor))
		return;




	Actor * ptActorSpawner = ActorGet(idActor);
	ACDID idACDSpawner = ptActorSpawner->idACD;
	ActorCommonData & tACDSpawner = ACDGet(idACDSpawner);

	SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);



    if (tSpawnerBrainData.gbidItem != GBID_NONE)
    {
        tACDSpawner.AttributesSetFloat(ATTRIBUTE_SPAWNER_COUNTDOWN_PERCENT, 1.0f);
    }

    if (ACDIsInDeactivatedScene(tACDSpawner))
    {
		tSpawnerBrainData.bDelayTimerSet = FALSE;
		tSpawnerBrainData.dwDoSpawnTick = (uint32)-1;
		tSpawnerBrainData.dwCloseTick = (uint32)-1;
		return;
    }

    if (GizmoIsDisabled(tACDSpawner))
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();

    if (tSpawnerBrainData.dwDeathTick != (uint32)-1)
    {

        if (dwNow >= tSpawnerBrainData.dwDeathTick)
        {
            BrainAPIDoDeath(ptActorSpawner->id, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
        return;
    }



    int32 nMaxConcurrent = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_MAX_COUNT, TAG_MARKER_SPAWN_MAX_COUNT);
    int32 nSpawnQuantityMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_QUANTITY_MIN, TAG_MARKER_SPAWN_QUANTITY_MIN);
    int32 nSpawnQuantityDelta = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_QUANTITY_DELTA, TAG_MARKER_SPAWN_QUANTITY_DELTA);
    int32 nMaxGenerated = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_MAX_GENERATED, TAG_MARKER_SPAWN_MAX_GENERATED);
    MonsterRarity eMonsterRarity = (MonsterRarity)ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_RARITY, TAG_MARKER_SPAWN_RARITY);



	if (tSpawnerBrainData.dwDoSpawnTick != (uint32)-1)
	{
		if (dwNow < tSpawnerBrainData.dwDoSpawnTick)
			return;
			sStartSpawn(ptActorSpawner, tSpawnerBrainData.dwDoSpawnCount, FALSE);
            sSetNextSpawnTime(ptActorSpawner);
			tSpawnerBrainData.dwDoSpawnTick = (uint32)-1;
	}








	if (tSpawnerBrainData.dwCloseTick != (uint32)-1)
	{
		if (dwNow < tSpawnerBrainData.dwCloseTick)
			return;

			AnimPlayerPlayAnimation(tACDSpawner, TAG_ACTOR_ANIMATION_CLOSING, ANIM_USE_ANIM_LENGTH, TRUE, ANIM_REASON_SPAWN, 0.0f, FALSE, 1.0f);


			AnimPlayerSetIdleAnimation(tACDSpawner, TAG_ACTOR_ANIMATION_NEUTRAL);


			tSpawnerBrainData.dwCloseTick = (uint32)-1;
	}








	if (tSpawnerBrainData.bSpawnTriggered == FALSE)
	{
		SpawnerTriggerType eType = (SpawnerTriggerType)ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TRIGGER_TYPE, TAG_MARKER_SPAWN_TRIGGER_TYPE);

		BOOL bSpawn = FALSE;
        switch (eType)
        {
        case 1:  // no enumerator names in src or the DWARF (SymbolicValue.cpp gSpawnerTriggerTypes): awareness scan
            bSpawn = sSpawnActorScan(idActor);
            break;






        case 0:  // proximity shape
            bSpawn = sSpawnCheckShape(idActor);
            break;

        case 2:  // immediate
            bSpawn = TRUE;
            break;
        }

        if (!bSpawn)
            return;
		sSpawnTrigger(idActor);
	}









	if (nMaxConcurrent > 0)
	{
		BOOL bContinue = sSpawnerMaintainSpawnList(idActor);
		if (!bContinue)
			return;
	}






    if (tSpawnerBrainData.gbidItem != GBID_NONE)
    {













        int32 nTimeLeft = tSpawnerBrainData.dwNextSpawnStartTime - dwNow;
        int32 nSpawnTimeMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_MIN, TAG_MARKER_SPAWN_TIME_MIN);
        tACDSpawner.AttributesSetFloat(ATTRIBUTE_SPAWNER_COUNTDOWN_PERCENT, (float)Math::Pin(nTimeLeft, 0, nSpawnTimeMin) / (float)nSpawnTimeMin);

        if (tSpawnerBrainData.bItemSpawnerRotateInGroup && !tSpawnerBrainData.bItemSpawnerCurrent)
        {
            tACDSpawner.AttributesSetFloat(ATTRIBUTE_SPAWNER_COUNTDOWN_PERCENT, 1.0f);
        }
    }


	if (dwNow < tSpawnerBrainData.dwNextSpawnStartTime)
	{
		return;
	}





	tSpawnerBrainData.bDelayTimerSet = FALSE;
	tSpawnerBrainData.bSpawnedFirstTime = TRUE;
    SpawnerCountID idSpawnerCount = (SpawnerCountID)tACDSpawner.AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
    int32 nCurCount = sGetConcurrentCount(idSpawnerCount);
    BOOL bSpawnElitesIncrementally = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_GENERATE_ELITES_INCREMENTALLY, TAG_MARKER_SPAWN_GENERATE_ELITES_INCREMENTALLY);
    BOOL bSpawnMaxGeneratedForElitePack = eMonsterRarity != MR_NORMAL && eMonsterRarity != MR_NONE && !bSpawnElitesIncrementally;

    int32 nTotalAvailableToSpawn = INT_MAX;
    int32 nConcurrentAvailableToSpawn = INT_MAX;
    int32 nSpawnQuantity;
    if (nMaxGenerated > 0)
    {
        nTotalAvailableToSpawn = Math::Max(nMaxGenerated - tSpawnerBrainData.nGenerated, 0);
    }





    if (nMaxConcurrent > 0)
    {
        nConcurrentAvailableToSpawn = Math::Max(nMaxConcurrent - nCurCount, 0);
    }





    if (!bSpawnMaxGeneratedForElitePack)
    {

        if (nTotalAvailableToSpawn < nSpawnQuantityMin || nConcurrentAvailableToSpawn < nSpawnQuantityMin)
        {
            return;
        }
    }


    int32 nSpawnMaxForBothCaps = Math::Min(nConcurrentAvailableToSpawn, nTotalAvailableToSpawn);
    if (nSpawnMaxForBothCaps < nSpawnQuantityMin + nSpawnQuantityDelta)
    {
        nSpawnQuantityDelta = nSpawnMaxForBothCaps - nSpawnQuantityMin;
    }

    nSpawnQuantityDelta = Math::Max(nSpawnQuantityDelta, 0);
    nSpawnQuantity = RandRange_Internal(SGameGlobalsGet().rng, nSpawnQuantityMin, nSpawnQuantityDelta);


    if (bSpawnMaxGeneratedForElitePack)
    {
        nSpawnQuantity = nMaxGenerated;
    }
	// the DWARF declares tAppGlobals here (no code survives: presumably a compiled-out trace)
	AppGlobals & tAppGlobals = sgtAppGlobals;








	BOOL bDidActualSpawn = sStartSpawn(ptActorSpawner, nSpawnQuantity, TRUE);
	if (bDidActualSpawn == TRUE)
	{
        sSetNextSpawnTime(ptActorSpawner);
	}
}



void SpawnBrainActorDeleting(Actor & tActor)

{


    if (!ActorFlagIsSet(tActor.id, ACTOR_DEACTIVATING))
    {
        ACDID idACD = ACDFindByActor(tActor.id);
        ActorCommonData & tACD = ACDGet(idACD);
        SpawnerCountID idConcurrentCount = (SpawnerCountID)tACD.AttributesGetInt(ATTRIBUTE_SPAWNER_CONCURRENT_COUNT_ID);
        if (idConcurrentCount != SPAWNERCOUNTID_INVALID)
        {
            sDecConcurrentCount(idConcurrentCount);
        }
    }
}


// inlined only into sStartSpawn, where the copy (chunk H03) is identical; the linker dropped the original out-of-line copy
void SpawnBrainResetSpawnStartTime(
    ActorID idActor)
{

    Actor * ptActorSpawner = ActorGet(idActor);
    int32 nSpawnTimeMin = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_MIN, TAG_MARKER_SPAWN_TIME_MIN);
    int32 nSpawnTimeDelta = ActorGetInheritedTagMapValueInt(ptActorSpawner, TAG_ACTOR_SPAWN_TIME_DELTA, TAG_MARKER_SPAWN_TIME_DELTA);
    int32 nSpawnDelay = RandRange_Internal(SGameGlobalsGet().rng, nSpawnTimeMin, nSpawnTimeDelta);

    uint32 dwNow = GameGetServerTime();

    SpawnBrainData & tSpawnerBrainData = sSpawnerBrainDataGet(ptActorSpawner);
    tSpawnerBrainData.dwNextSpawnStartTime = dwNow + nSpawnDelay;
}
