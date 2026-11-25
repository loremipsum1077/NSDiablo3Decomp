// Original file: X/Server/Movement/SActorMovement.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../SGame.h"
#include "../Actors/Actors.h"
#include "../Actors/Projectile.h"
#include "../SGameManager.h"
#include "../../Common/Player.h"
#include "../AI/BrainComponents.h"
#include "../Actors/ActorPowers.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Networking/GameMessage.h"
#include "../PlayerList.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/AnimPlayer.h"
#include "../Map/Minimap.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../AI/BrainStance.h"
#include "SActorMovement.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/MemoryPools.h"
#include "../LuaScript/ScriptEvent.h"
#include "../SGameConstants.h"
#include "../SWorld.h"
#include "../../Sigma/RefString.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/Marker/Marker.h"
#include "../SPlayer.h"
#include "../../Common/Nav/NavCellPath.h"
#include <float.h>

// Declared here from their definitions elsewhere (headers not adopted yet).
void ActorInternalSetCurrentFacing(ActorID idActor, Angle aYaw); void BrainUpCallAtDestination(ActorID idActor, BOOL bStuck, BOOL bWithinRadiusOfTarget, ACDID idACDBlocker);  // X/Server/Actors/Actors.cpp:3755; X/Server/AI/BrainStance.cpp:2281
typedef int32 EffectType;  // X/Server/Actors/ActorEffect.h
void ACDPlayEffect(const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter, const ACDID idACDOwner); BOOL DominoRaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd, Vector3D & vCollidePoint, Vector3D & vCollideNormal, float & flUnitTravel, uint16 uExcludeMask);  // X/Server/Actors/ActorEffect.cpp:104; X/Server/Physics/DominoServer.cpp:317
enum {
    ACTOR_SPHERE_TREE_FLAGS_ALL = 7,
};  // X/Common/Actors/ActorCollision.h
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags); BOOL NavMeshGetHeightAndNormal(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp, const BOOL fPinToSquareBounds, WorldDistance & flHeight, Vector3D & wvNormal);  // X/Common/Actors/ActorCollision.cpp:317; X/Common/Nav/NavigationMesh.cpp:783
BOOL ProjectileTestCollisionAlongLine(ACDID idACDProjectile, const WorldPlace & placeStart, const WorldPlace & placeDest, ACDID & idACDHitInCollision, float & flUnitDistanceTraveled); BOOL AnimPlayerAnimExists(ActorCommonData & tACD, const uint32 dwAnimTag);  // X/Server/Actors/Projectile.cpp:667; X/Common/AnimPlayer.cpp:1898
BOOL DetPathShouldSendMoveMessageBeforeServerUpdate(const DeterministicPathData & tDPath);  // X/Common/DetPaths.cpp:1244
typedef int32 AttribGroupID;  // X/Common/Attributes/FastAttribute.h
void FastAttribGroupApplyDirty(AttribGroupID idAttribGroup); BOOL ReplayIsPlaying();  // X/Common/Attributes/FastAttribute.cpp:3250; X/Server/Replay.cpp:804
BOOL ProjectileCanCollisionWithACD(ACDID idACDProjectile, ActorCommonData & tACDIntersected); BOOL ProjectileHadCollision(ImpactData & tImpactData);  // X/Server/Actors/Projectile.cpp:526; X/Server/Actors/Projectile.cpp:188
PowerID ActorPowersGetOwnerPower(ActorID idActor); AnimReason AnimPlayerGetAnimReason(const ActorCommonData & tACD);  // X/Server/Actors/ActorPowers.cpp:436; X/Common/AnimPlayer.cpp:1739
void SScenePutACDInScene(SSceneID idSScene, ACDID idACD, BOOL bAdd); Angle ACDGetYaw(ACDID idACD);  // X/Server/SScene.cpp:513; X/Common/ActorCommonData.cpp:2997
uint32 DetPathGetUpdateInterval(DeterministicPathType eDType); void BrainStanceSetWalking(ActorID idActor);  // X/Common/DetPaths.cpp:1204; X/Server/AI/BrainStance.cpp:3265
void PlayerBrainTryQueuedPower(ACDID idACDPlayer); BOOL DetPathProcessAdjustFacing(ActorMovement &, DeterministicPathData & tDPath, Angle aNewYaw);  // X/Server/AI/PlayerBrain.cpp:407; X/Common/DetPaths.cpp:1275
float LuaGetMaxPlayerInRangeRadiusInScriptMessageArray(); void PlayersFindInRadius(const WorldPlace & tPlace, const WorldDistance wdRadius, XPooledDataIDList & listPlayers);  // X/Server/LuaScript/LuaScript.cpp:612; X/Server/Player.cpp:3109
void ProximityDetectorCheckActivation(ActorID idTrigger, ActorCommonData & tACDMover, const LineSeg2D & segMove); void ProximityDetectorActivateAtPlace(ActorID idTrigger, ActorCommonData & tACDMover, BOOL bExit);  // X/Server/AI/ProximityDetectorBrain.cpp:779; X/Server/AI/ProximityDetectorBrain.cpp:910
XPooledDataIDList * ActorAIGetTriggerList(); TickBankBucketList & SPlayerGetTickBankList(const Player & tPlayer);  // X/Server/AI/ActorAI.cpp:4561; X/Server/SPlayer.cpp:1681
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame); BOOL WorldPlaceEqual(const WorldPlace & tPlace1, const WorldPlace & tPlace2);  // X/Server/Actors/ActorPowers.cpp:392; X/Common/WorldPlace.cpp:114
void SWorldPlayerWarpToLastCheckpoint(const Player & tPlayer); void ActorInternalSetPosition(Actor * ptActor, const WorldPlace & tPlace);  // X/Server/SWorld.cpp:866; X/Server/Actors/Actors.cpp:4663
const int32 MOVEMENT_VALIDATION_ENABLED_LEVEL = 1;  // DWARF SActorMovement.cpp:65
static XVarScope s_scopeSActorMovement("SActorMovement", NULL);
static XVarScope s_scopeValidation(s_scopeSActorMovement, "Validation");
static XVarUint32 s_varMovementValidationEnabledLevel(s_scopeValidation, "EnabledLevel", 1);
static XVarUint32 s_varMovementValidationEnablePowerTickCheck(s_scopeValidation, "EnablePowerTickCheck", 1);
static XVarUint32 s_varMovementValidationTickToleranceDistance(s_scopeValidation, "TickToleranceDistance", 5);
static XVarFloat s_varMovementValidationTickRateTolerance(s_scopeValidation, "TickRateTolerance", 0.0f);
static XVarUint32 s_varMovementUpdateTicks(s_scopeSActorMovement, "UpdateTicks", 12);
static XVarUint32 s_varMovementUpdateTicksMonster(s_scopeSActorMovement, "UpdateTicksMonster", 24);
XVarUint32 s_varMovementVerboseMessage(s_scopeSActorMovement, "VerboseMessage", 0);

struct SActorMovementGameGlobals {  // size 0x38
    XChainedFixedBlockAllocator<SActorMovement> tSActorMovementAlloc;  // 0x00
    uint32 uDefaultUpdateTicks;  // 0x30
    uint32 uDefaultUpdateTicksMonster;  // 0x34
};
const int32 MOVEMENT_VALIDATION_TICK_TOLERANCE_DISTANCE = 5;  // DWARF SActorMovement.cpp:66 (src lines 66-74 hold the XVars of DWARF 75-83)
const uint32 MOVEMENT_SLOP_TICKS_MIN = 5;  // DWARF SActorMovement.cpp:72
const float MOVEMENT_TICK_RATE_TOLERANCE_DEFAULT = 0.0f;  // DWARF SActorMovement.cpp:73



void SActorMovementInitializeForGame()
{
    SActorMovementGameGlobals *& ptSActorMovementGlobals = SGameManager::GameGet().ptSActorMovementGlobals;
    SGame & tSGame = SGameManager::GameGet();

    ptSActorMovementGlobals = (SActorMovementGameGlobals *)SigmaMemoryNew(sizeof(SActorMovementGameGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptSActorMovementGlobals->tSActorMovementAlloc.Initialize();
    ptSActorMovementGlobals->tSActorMovementAlloc.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptSActorMovementGlobals->tSActorMovementAlloc.SetGrow(XALLOC_GROW_LINEAR, 512);
    ptSActorMovementGlobals->tSActorMovementAlloc.SetBlockCount_Tracked(512);

    ptSActorMovementGlobals->uDefaultUpdateTicks = 12;
    ptSActorMovementGlobals->uDefaultUpdateTicksMonster = 24;
}


void SActorMovementDisposeForGame()
{
    SActorMovementGameGlobals *& ptSActorMovementGlobals = SGameManager::GameGet().ptSActorMovementGlobals;

    ptSActorMovementGlobals->tSActorMovementAlloc.Dispose();

    SGame & tSGame = SGameManager::GameGet();
    SigmaMemoryFree(ptSActorMovementGlobals, tSGame.ptMemoryInterface);
    ptSActorMovementGlobals = NULL;
}



BOOL SActorMovementInitialize(Actor * ptActor, const WorldPlace & placeStart)
{
    SActorMovementGameGlobals * ptSActorMovementGlobals = SGameManager::GameGet().ptSActorMovementGlobals;

    SActorMovement * ptSActorMovement = ptSActorMovementGlobals->tSActorMovementAlloc.Alloc_Tracked();
    BOOL bInitialized = FALSE;
    if (ptSActorMovement)
    {
        if (!ptSActorMovement->Initialize(ptActor->id, placeStart))
        {
            ptSActorMovementGlobals->tSActorMovementAlloc.Free(ptSActorMovement);
            return FALSE;
        }
        bInitialized = TRUE;
        ptActor->ptSActorMovement = ptSActorMovement;
    }
    return bInitialized;
}

// The NX SDK's <math.h> (musl) classifies a float by testing its bits; the build container's libc expands
// isnan() to a compiler builtin. Same workaround as in X/Server/Network/ServerMessageReceive.cpp and
// isfinite() in Sigma/DataStructures/DataStructuresExternal.h, until the SDK libc headers are available.
#ifdef D3_NX_FLOAT_BITS_DEFINED
#undef isnan
#define isnan(x) ((__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000)
#endif




void SActorMovementDispose(Actor * ptActor)
{
    if (ptActor->ptSActorMovement)
    {
        SActorMovementGameGlobals * ptSActorMovementGlobals = SGameManager::GameGet().ptSActorMovementGlobals;
        ptActor->ptSActorMovement->Dispose();
        ptSActorMovementGlobals->tSActorMovementAlloc.Free(ptActor->ptSActorMovement);
        ptActor->ptSActorMovement = NULL;
    }
}








void SActorMovementUpdate(ACDID idACD)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorTryToGet(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptActor && ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->Update();
    }

    ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        FastAttribGroupApplyDirty(ptACD->AttributesGetAttributeGroup());
    }
}






void SActorMovementSendMoveMessage(const PlayerList & listDestPlayers, ACDID idACD)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->SendMoveMessage(listDestPlayers);
    }
}










void SActorMovementSendMoveMessage(ACDID idACD)
{
    PlayerList listSinglePlayer(PlayerListGetElementPool());
    POS pos;
    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listPlayers, idACD, FALSE);
    pos = listPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayers.GetNext(pos);

        listSinglePlayer.RemoveAll();
        listSinglePlayer.AddTail_Tracked(ptPlayer);
        SActorMovementSendMoveMessage(listSinglePlayer, idACD);
    }
}




inline SActorMovement::~SActorMovement()
{
}




BOOL SActorMovement::Initialize(ActorID idActor, const WorldPlace & placeStart)
{
    if (idActor == ACTORID_INVALID)
        return FALSE;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return FALSE;

    m_idActor = idActor;
    m_ptACD = ptACD;
    m_bIgnoreUpdate = FALSE;
    m_bSnapFacingNextClientUpdate = FALSE;
    m_uLastClientMoveReportedTimeBase = 0;
    m_uLastServerTickTime = GameGetServerTime();
    m_uLastBankSlopTime = GameGetServerTime();
    m_uAckValue = (uint32)-1;
    m_bAcceptNextClientPowerWalk = FALSE;
    m_uCameraSmoothingTime = 0;
    ResetValidationToPlace(placeStart);
    m_pfnMovementBlockedCallback = NULL;

    UpdateFunctionInitialize(UPDATE_ACD_MOVEMENT, ptACD->id, m_tUpdateData);
    ResetCache();

    BOOL bSuccess = ActorMovement::Initialize(placeStart);

    if (ActorGetType(ptACD->tSNOName) == ACTORTYPE_MONSTER)
    {
        SActorMovementGameGlobals * ptSActorMovementGlobals = SGameManager::GameGet().ptSActorMovementGlobals;
        if (ptACD->IsMonsterTeam() && ptACD->eMonsterRarity == MR_NORMAL)
            SetUpdateInterval(ptSActorMovementGlobals->uDefaultUpdateTicksMonster);
        else
            SetUpdateInterval(ptSActorMovementGlobals->uDefaultUpdateTicks);
    }
    return bSuccess;
}

















void SActorMovement::Dispose()
{
    ActorMovement::Dispose();
    ServerRemoveUpdateFunction(m_tUpdateData);
}





inline void SActorMovement::CheckAndSendSnapState(const PlayerList & listDestPlayers)
{
    if (m_eSnapState != SNAP_NONE)
    {
        SendMoveMessageSnapped(listDestPlayers);
        QueueUpdate(1);
    }
}





void SActorMovement::SendMoveMessage(const PlayerList & listDestPlayers)
{
    const PlayerList * ptList = &listDestPlayers;
    BOOL bReplay = ReplayIsPlaying();
    PlayerList listCulledPlayers(PlayerListGetElementPool());


    if (!bReplay && (m_eMovementType == MOVEMENT_TYPE_CLIENT_AUTH || m_eMovementType == MOVEMENT_TYPE_CLIENT_AUTH_FACING))
    {
        for (POS pos = listDestPlayers.GetHeadPosition(); pos; )
        {
            Player * ptPlayer = listDestPlayers.GetNext(pos);
            if (ptPlayer->idPlayerACD != m_ptACD->id)
                listCulledPlayers.AddHead_Tracked(ptPlayer);
        }

        ptList = &listCulledPlayers;
    }


    CheckAndSendSnapState(listDestPlayers);


    MovementType eType = m_eMovementType;
    if (eType == MOVEMENT_TYPE_WALKING_IN_PLACE)
        eType = MOVEMENT_TYPE_WALKING;


    switch (eType)
    {
    case MOVEMENT_TYPE_NONE:
    case MOVEMENT_TYPE_CLIENT_AUTH_FACING:
        SendMoveMessageFacing(*ptList);
        break;

    case MOVEMENT_TYPE_WALKING:
    case MOVEMENT_TYPE_WALK_FINISHING:
    case MOVEMENT_TYPE_CLIENT_AUTH:
        SendMoveMessageNormal(*ptList);
        break;

    case MOVEMENT_TYPE_FIXED_PROJECTILE:
    case MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING:
        SendMoveMessageFixed(*ptList);
        break;

    case MOVEMENT_TYPE_STOPPED_PROJECTILE:
        SendMoveMessageStopped(*ptList);
        break;

    case MOVEMENT_TYPE_ARC:
        SendMoveMessageArc(*ptList);
        break;

    case MOVEMENT_TYPE_DETPATH:
        SendMoveMessageDetPath(*ptList);
        break;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
}



inline BOOL SActorMovement::RefreshAckValue(const PlayerList & listDestPlayers)
{
    if (ACDIsPlayer(m_ptACD->id))
    {
        for (POS pos = listDestPlayers.GetHeadPosition(); pos; )
        {
            Player * ptPlayer = listDestPlayers.GetNext(pos);
            if (ptPlayer->idPlayerACD == m_ptACD->id)
            {
                m_uAckValue = Rand_Internal(SGameGetRNG(), 0xfffd);
                return TRUE;
            }
        }
    }
    return FALSE;
}









void SActorMovement::SendMoveMessageSnapped(const PlayerList & listDestPlayers)
{
    ACDTranslateSnappedMessage tMessage;

    tMessage.ann = ANNFromACD(m_ptACD->id);
    tMessage.aDesiredYaw = ACDGetYaw(m_ptACD->id);
    tMessage.bSnapFacing = m_bSnapFacingNextClientUpdate;
    tMessage.uMoveFlags = (1 << MOVEBIT_SNAP) | (1 << MOVEBIT_DONT_AUTOPLAY_IDLE);
    if (m_eSnapState == SNAP_PENDING_SOFT)
        tMessage.uMoveFlags |= (1 << MOVEBIT_SNAP_SOFT);

    WorldPlace tPlace = m_ptACD->GetPlace();
    tMessage.vDest = tPlace.wp;

    if (RefreshAckValue(listDestPlayers))
    {
        tMessage.uAckValue = m_uAckValue;
    }

    uint32 uNow = GameGetServerTime();
    if (m_uCameraSmoothingTime && m_uCameraSmoothingTime > uNow)
    {
        tMessage.uCameraSmoothingTime = m_uCameraSmoothingTime;
    }
    m_uCameraSmoothingTime = 0;

    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_SNAPPED, &tMessage, sizeof(tMessage));
}





void SActorMovement::SendMoveMessageSync(const PlayerList & listDestPlayers, const BOOL bSnap)
{
    ACDTranslateSyncMessage tMessage;

    tMessage.ann = ANNFromACD(m_ptACD->id);
    WorldPlace tPlace = m_ptACD->GetPlace();
    tMessage.vPos = tPlace.wp;

    if (bSnap)
    {
        tMessage.bSnap = TRUE;

        if (RefreshAckValue(listDestPlayers))
        {
            tMessage.uAckValue = m_uAckValue;
        }
    }

    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_SYNC, &tMessage, sizeof(tMessage));
}





void SActorMovement::SendMoveMessageNormal(const PlayerList & listDestPlayers)
{
    ACDTranslateNormalMessage tMessage;

    tMessage.ann = ANNFromACD(m_ptACD->id);

    if (m_flCurrentSpeed != m_flCachedSpeed || m_bForceCacheSend)
    {
        tMessage.flMovementSpeed = m_flCurrentSpeed;
        m_uCacheFlags |= (1 << MOVECACHE_SPEED);
    }

    float flYaw = ACDGetYaw(m_ptACD->id);
    if (flYaw != m_aCachedYaw || m_bForceCacheSend)
    {
        tMessage.aDesiredYaw = flYaw;
        m_uCacheFlags |= (1 << MOVECACHE_YAW);
    }

    if (m_bSnapFacingNextClientUpdate != m_bCachedSnapFacing || m_bForceCacheSend)
    {
        tMessage.bSnapFacing = m_bSnapFacingNextClientUpdate;
        m_uCacheFlags |= (1 << MOVECACHE_SNAP_FACING);
    }

    uint32 uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
    if (uMoveFlags != m_uCachedMoveFlags || m_bForceCacheSend)
    {
        tMessage.uMoveFlags = uMoveFlags;
        m_uCacheFlags |= (1 << MOVECACHE_MOVEFLAGS);
    }

    uint32 uAnimTag = m_eAnimTag;
    if (uAnimTag != m_uCachedAnimTag || m_bForceCacheSend)
    {
        tMessage.eAnimTag = uAnimTag;
        m_uCacheFlags |= (1 << MOVECACHE_ANIMTAG);
    }

    uint32 dwNow = GetTimeNow();
    if (m_uMinDurationEndStampTime > dwNow)
    {
        tMessage.uWalkInPlaceTicks = m_uMinDurationEndStampTime - dwNow;
    }

    if (m_snoPowerPassability != SNO_NONE)
    {
        tMessage.snoPowerPassability = m_snoPowerPassability;
    }

    WorldPlace tPlace = m_ptACD->GetPlace();
    tMessage.vDest = tPlace.wp;

    if (RefreshAckValue(listDestPlayers))
    {
        tMessage.uAckValue = m_uAckValue;
    }

    if (m_nMaxTicksToWalk >= 0)
    {
        tMessage.nMaxTicksToWalk = Math::Max(m_nMaxTicksToWalk, 1);
    }

    tMessage.eMoveType = m_eMovementType;
    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_NORMAL, &tMessage, sizeof(tMessage));
}







inline void SActorMovement::SendMoveMessageFixed(const PlayerList & listDestPlayers)
{
    ACDTranslateFixedMessage tMessage;
    tMessage.ann = ANNFromACD(m_ptACD->id);
    tMessage.vVel = GetVelocity();
    tMessage.uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
    tMessage.eAnimTag = m_eAnimTag;
    tMessage.snoPowerPassability = m_snoPowerPassability;
    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_FIXED, &tMessage, sizeof(tMessage));
}





void SActorMovement::SendMoveMessageArc(const PlayerList & listDestPlayers)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);

    if (m_bSnapFacingNextClientUpdate)
        SendMoveMessageFacing(listDestPlayers);

    float flDesiredHeight = (m_uMoveFlags & (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT)) ? m_placeDest.wp.z : 0.0f;

    ACDTranslateArcMessage tMessage;
    tMessage.ann = ANNFromACD(m_ptACD->id);
    tMessage.vPos = placeCurr.wp;
    tMessage.vVel = GetVelocity();
    tMessage.uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
    tMessage.eAnimTag = m_eAnimTag;
    tMessage.eLandingAnimTag = m_eLandingAnimTag;
    tMessage.flGravity = m_flGravity;
    tMessage.snoPowerPassability = m_snoPowerPassability;
    tMessage.flUnitBounce = m_flUnitBounce;
    tMessage.flDesiredHeight = flDesiredHeight;
    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_ARC, &tMessage, sizeof(tMessage));
}











void SActorMovement::SendMoveMessageDetPath(const PlayerList & listDestPlayers)
{
    switch (m_tDPath.eDPath)
    {
    case DPATH_SIN_WAVE:
    case DPATH_PEDAL_CURVE:
        {
            ACDTranslateDetPathSinMessage tMessage;
            tMessage.ann = ANNFromACD(m_ptACD->id);
            tMessage.eDPath = m_tDPath.eDPath;
            tMessage.tSeed = m_tDPath.tSeed;
            tMessage.aOriginalYaw = m_tDPath.aOriginalYaw;
            tMessage.vStartPos = m_tDPath.vStartPos;
            tMessage.uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
            tMessage.eAnimTag = m_eAnimTag;
            tMessage.snoPowerPassability = m_snoPowerPassability;
            tMessage.nVar0 = m_tDPath.nVar0;
            tMessage.flVar0 = m_tDPath.flVar0;
            tMessage.tSinData = m_tDPath.tSinData;
            tMessage.flSpeedMult = IsServerProjectile() ? ProjectileGetSpeedMultiplier(m_ptACD->id) : 1.0f;
            tMessage.wvVelocity = GetVelocity();
            tMessage.wvVelocity.Normalize();
            tMessage.wvVelocity *= GetDesiredSpeed();
            MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_DETPATH_SIN, &tMessage, sizeof(tMessage));
        }
        break;

    case DPATH_SPIRAL:
    case DPATH_SPIRAL_IMMEDIATE:
        {
            ACDTranslateDetPathSpiralMessage tMessage;
            tMessage.ann = ANNFromACD(m_ptACD->id);
            tMessage.eDPath = m_tDPath.eDPath;
            tMessage.vStartPos = m_tDPath.vStartPos;
            tMessage.vTargetPos = m_tDPath.vTargetPos;
            tMessage.uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
            tMessage.eAnimTag = m_eAnimTag;
            tMessage.flVar0 = m_tDPath.flVar0;
            tMessage.flVar1 = m_tDPath.flVar1;
            tMessage.tSinData = m_tDPath.tSinData;
            tMessage.flSpeedMult = IsServerProjectile() ? ProjectileGetSpeedMultiplier(m_ptACD->id) : 1.0f;
            MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_DETPATH_SPIRAL, &tMessage, sizeof(tMessage));
        }
        break;

    case DPATH_PIE_WEDGE:
        {
            ACDTranslateDetPathPieWedgeMessage tMessage;
            tMessage.ann = ANNFromACD(m_ptACD->id);
            tMessage.vStartPos = m_tDPath.vStartPos;
            tMessage.vFirstTargetPos = m_tDPath.vTargetPos;
            tMessage.uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
            tMessage.eAnimTag = m_eAnimTag;
            tMessage.tPieData = m_tDPath.tPieData;
            tMessage.flCurTime = m_tDPath.flVar3;
            tMessage.flSpeedMult = IsServerProjectile() ? ProjectileGetSpeedMultiplier(m_ptACD->id) : 1.0f;
            MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_DETPATH_PIE_WEDGE, &tMessage, sizeof(tMessage));
        }
        break;

    default:
        {
            ACDTranslateDetPathMessage tMessage;
            tMessage.ann = ANNFromACD(m_ptACD->id);
            tMessage.eDPath = m_tDPath.eDPath;
            tMessage.tSeed = m_tDPath.tSeed;
            tMessage.aOriginalYaw = m_tDPath.aOriginalYaw;
            tMessage.vStartPos = m_tDPath.vStartPos;
            tMessage.uMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
            tMessage.eAnimTag = m_eAnimTag;
            tMessage.snoPowerPassability = m_snoPowerPassability;
            tMessage.nVar0 = m_tDPath.nVar0;
            tMessage.flVar0 = m_tDPath.flVar0;
            tMessage.flVar1 = m_tDPath.flVar1;
            tMessage.flVar2 = m_tDPath.flVar2;
            tMessage.flVar3 = m_tDPath.flVar3;
            tMessage.flVar4 = m_tDPath.flVar4;
            tMessage.flSpeedMult = IsServerProjectile() ? ProjectileGetSpeedMultiplier(m_ptACD->id) : 1.0f;
            tMessage.wvVelocity = GetVelocity();
            tMessage.wvVelocity.Normalize();
            tMessage.wvVelocity *= GetDesiredSpeed();
            MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_DETPATH, &tMessage, sizeof(tMessage));
        }
        break;
    }
}
void SActorMovement::SendMoveMessageFacing(const PlayerList & listDestPlayers)
{
    float flYaw = ACDGetYaw(m_ptACD->id);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        if (ptPlayer)
        {
            int32 nIdx = ptPlayer->nPlayerIndex;
            if (nIdx >= 0 && nIdx < MAX_PLAYERS)
            {
                if (m_bSnapFacingNextClientUpdate || flYaw != m_arCachedFacingAngle[nIdx] || m_arACDCachedFacingPlayer[nIdx] != ptPlayer->idPlayerACD)
                {
                    m_arACDCachedFacingPlayer[nIdx] = ptPlayer->idPlayerACD;
                    m_arCachedFacingAngle[nIdx] = flYaw;

                    PlayerList listThisPlayer(PlayerListGetElementPool());
                    PlayerListGetSingle(listThisPlayer, *ptPlayer);

                    ACDTranslateFacingMessage tMessage;
                    tMessage.ann = ANNFromACD(m_ptACD->id);
                    tMessage.aDesiredYaw = ACDGetYaw(m_ptACD->id);
                    tMessage.bSnapFacing = m_bSnapFacingNextClientUpdate;
                    MessageSendToClient(listThisPlayer, SMSG_ACD_TRANSLATE_FACING, &tMessage, sizeof(tMessage));
                }
            }
        }
    }
}





inline void SActorMovement::SendMoveMessageStopped(const PlayerList & listDestPlayers)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);

    ACDTranslateStoppedMessage tMessage;
    tMessage.ann = ANNFromACD(m_ptACD->id);
    tMessage.vPos = placeCurr.wp;
    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_STOPPED, &tMessage, sizeof(tMessage));
}





void SActorMovement::QueueUpdate(
    uint32 uDesiredTickDelay)
{
    uint32 uDesiredUpdateTick = GameGetServerTime() + uDesiredTickDelay;




    if (m_uNextUpdateTick > uDesiredUpdateTick)
    {
        uDesiredTickDelay = m_uNextUpdateTick - uDesiredUpdateTick;
    }

    m_bIgnoreUpdate = FALSE;

    uDesiredTickDelay = Math::Max(1u, uDesiredTickDelay);
    ServerQueueUpdateFunction(uDesiredTickDelay, m_tUpdateData);
}



BOOL SActorMovement::Update()
{
    if (m_eSnapState != SNAP_NONE)
    {
        if (m_eSnapState == SNAP_SENT)
            m_eSnapState = SNAP_NONE;
        else
            m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    }

    ServerUpdateWasCalled(m_tUpdateData);

    BOOL bSendMoveMessageBeforeUpdate = m_eMovementType == MOVEMENT_TYPE_ARC || (m_eMovementType == MOVEMENT_TYPE_DETPATH && DetPathShouldSendMoveMessageBeforeServerUpdate(m_tDPath));
    BOOL bSentMoveMessageBeforeUpdate = FALSE;
    if (bSendMoveMessageBeforeUpdate && (ShouldSendUpdates() || m_ptACD->IsModifiedSpecific(ACDMODIFIED_MOVEMENT)))
    {
        SActorMovementSendMoveMessage(GetACDID());
        bSentMoveMessageBeforeUpdate = TRUE;
    }

    BOOL bDelete = FALSE;
    if (!m_bIgnoreUpdate)
        bDelete = ActorMovement::Update();

    if (m_ptACD->idFastAttrib != -1 &&
        m_ptACD->AttributesGetInt(ATTRIBUTE_BOUNTY_OBJECTIVE))
    {
        Minimap::ActorAutoPing(m_ptACD->id, -1);
    }

    if (bSentMoveMessageBeforeUpdate)
    {
        m_ptACD->ModifiedClear(ACDMODIFIED_MOVEMENT);
    }
    return bDelete;
}






































void SActorMovement::GetPlaceAndRadius(WorldPlace & tPlace, float & flRadius, BOOL bGetMovingRadius) const
{
    GetPlace(tPlace);
    flRadius = GetRadius(bGetMovingRadius);
}






void SActorMovement::GetPlace(WorldPlace & tPlace) const
{
    WorldLocation & tLoc = m_ptACD->tLocation.tWorldLocation;
    tPlace.idSWorld = tLoc.idSWorld;
    tPlace.wp = m_ptACD->tLocation.tWorldLocation.transform.wp;
}








float SActorMovement::GetRadius(BOOL bGetMovingRadius) const
{
    float flRadius;
    if (m_ptACD->tCollSettings.eCollisionShape == COLLISION_SHAPE_CYLINDER)
    {
        const AxialCylinder & tCyl = *(const AxialCylinder *)m_ptACD->tCollSettings.arAxialCylinderBytes;
        flRadius = tCyl.wdRadius;
    }
    else
        flRadius = m_ptACD->tLocation.tWorldLocation.wcylBounds.wdRadius;
    if (bGetMovingRadius)
        flRadius *= m_flRadiusScalar;
    return flRadius;
}














SSceneID SActorMovement::GetSSceneID() const
{
    SSceneID idSScene = ACDGetCurrentSSceneID(ACDGet(GetACDID()));
    return idSScene;
}





const float WORLD_BORDER_DISTANCE = 1000.0f;
void SActorMovement::SetNewPlace(const WorldPlace & placeNew, BOOL bSnap, BOOL bClientAuth, BOOL bSendToClient)
{
    BOOL bIsPlayer = ACDIsPlayer(m_ptACD->id);
    if (bIsPlayer && m_eMovementType != MOVEMENT_TYPE_CLIENT_AUTH)
    {
        Player * ptPlayer = PlayerGetByACD(GetACDID());
        ptPlayer->dwTickLastVerification = GameGetServerTime();
        ptPlayer->tPlaceLastVerification = placeNew;
        ptPlayer->flMaxSpeedThisVerificationInterval = 0.0f;
    }

    if (!bClientAuth)
        ResetValidationToPlace(placeNew);

    Actor * ptActor = ActorTryToGet(m_idActor);
    SWorld * ptSWorld = SWorldGet(placeNew.idSWorld);
    if (placeNew.wp.x < ptSWorld->vExtMin.x - 1000.0f ||
        placeNew.wp.y < ptSWorld->vExtMin.y - 1000.0f ||
        placeNew.wp.x > ptSWorld->vExtMax.x + 1000.0f ||
        placeNew.wp.y > ptSWorld->vExtMax.y + 1000.0f ||
        Math::Abs(placeNew.wp.z) >= 100000.0f)
    {
        if (!bIsPlayer)
        {
            ActorFlagForDeletion(m_idActor);
            return;
        }
        Player * ptPlayer = PlayerGetByACD(GetACDID());
        if (!bClientAuth && ptPlayer)
        {
            SWorldPlayerWarpToLastCheckpoint(*ptPlayer);
            return;
        }
        {
            WorldPlace placeGround = placeNew;
            SnapPlaceToGround(placeGround);
            ActorInternalSetPosition(ptActor, placeGround);
            if (ShouldSendUpdates())
                m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
        }
    }
    else
    {
        ActorInternalSetPosition(ptActor, placeNew);
        ActorCheckChangeLevelArea(m_idActor);
        ActorCheckNavMeshTypeChange(m_idActor, 0, SWORLDID_INVALID);
        ActorUpdateOrientation(m_idActor, bSendToClient && m_eMovementType == MOVEMENT_TYPE_NONE);
        if (ShouldSendUpdates())
            m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    }
}
























BOOL SActorMovement::ShouldSendUpdates()
{
    switch (m_eMovementType)
    {
    case MOVEMENT_TYPE_FIXED_PROJECTILE:
    case MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING:
        return GetVelocity().As2D() != m_vLastVel.As2D();

    case MOVEMENT_TYPE_ARC:
        if (!(m_uMoveFlags & (1 << MOVEBIT_ARC_UNTIL_DEST_HEIGHT)) && GetVelocity().As2D() != m_vLastVel.As2D())
            break;
        return FALSE;

    case MOVEMENT_TYPE_DETPATH:
    {
        float flProjSpeedMult = ProjectileGetSpeedMultiplier(GetACDID());
        return m_vLastVel.x != flProjSpeedMult;
    }

    case MOVEMENT_TYPE_STOPPED_PROJECTILE:
        return FALSE;
    }
    return TRUE;
}































void SActorMovement::ForceSendUpdate()
{
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
}




void SActorMovement::SetNewFacing(Angle aFacing, BOOL bSnap, BOOL bSendUpdate)
{
    m_bSnapFacingNextClientUpdate = m_bSnapFacingNextClientUpdate || bSnap;
    ActorInternalSetCurrentFacing(m_idActor, aFacing);
    if (bSendUpdate)
        ForceSendUpdate();
}









ACDID SActorMovement::GetACDID() const
{
    return m_ptACD->id;
}





BOOL SActorMovement::IsServerProjectile() const
{
    return ACDIsProjectile(GetACDID());
}






uint32 SActorMovement::GetTimeNow() const
{
    return GameGetServerTime();
}




void SActorMovement::SnapPlaceToGround(WorldPlace & placeCurrent) const
{
    NavMeshHandle tNavMeshHandle;
    Vector3D wvNormal;
    ScenesCommonNavMeshNodeFromPlace(placeCurrent, &tNavMeshHandle, FALSE, FALSE);
    NavMeshGetHeightAndNormal(tNavMeshHandle, placeCurrent.wp, FALSE, placeCurrent.wp.z, wvNormal);
}





static const float flOOSecondsToFullStuck = 1.0f / 1.5f;
void SActorMovement::Stuck()
{
    float flUpdateSeconds = (float)m_uUpdateInterval * (1.0f / 60.0f);
    float flStuckInc = flUpdateSeconds * flOOSecondsToFullStuck;
    m_tSteer.ModifyStuckUnitValue(flStuckInc);
}




void SActorMovement::GetACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs) const
{
    ACDsIntersectSphere(tPlace, wdRadius, idACDIgnore, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
}







void SActorMovement::StopNow()
{
    if (!IsMoving())
        return;

    if (m_uMoveFlags & (1 << MOVEBIT_KNOCKBACK))
        BrainStanceSetWalking(m_idActor);

    ActorMovement::StopNow();
    m_bIgnoreUpdate = TRUE;
    if (ShouldSendUpdates())
        m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);

    if (m_ptACD)
    {
        if (ACDIsPlayer(m_ptACD->id) || m_ptACD->IsPlayerTeam() || (m_ptACD->IsMonster() && m_ptACD->eMonsterRarity == MR_BOSS) || BrainAPIIsActorExcludedFromPerformanceOptimizations(*m_ptACD))
        {
            if (BrainAPIActorHasQueuedAttack(m_idActor))
                BrainComponentsTriggerUpdate(m_idActor, TRUE);
        }
    }

    PlayerBrainTryQueuedPower(GetACDID());
}





















BOOL SActorMovement::StopProjectile()
{
    if (ActorMovement::StopProjectile())
    {
        m_bIgnoreUpdate = FALSE;
        m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
        return TRUE;
    }
    return FALSE;
}






uint32 SActorMovement::GetEarliestUpdateTickOffset()
{
    Actor * ptActor = ActorTryToGet(m_idActor);
    uint32 uNow = GetTimeNow();
    if (uNow <= ptActor->dwTimeCreated)
        return 1;
    return 0;
}












BOOL SActorMovement::ProjCollisionResponse(
    ImpactData & tImpactData)
{
    BOOL bTestCollisionResponse = TRUE;

    if (tImpactData.idACDIntersected != ACDID_INVALID)
    {



        ActorCommonData * ptACDIntersected = ACDTryToGet(tImpactData.idACDIntersected);
        if (ptACDIntersected)
        {

            // The +0xac branch skips the collision callback when this test fails.
            bTestCollisionResponse = ProjectileCanCollisionWithACD(GetACDID(), *ptACDIntersected);
        }
        else
        {
            bTestCollisionResponse = FALSE;
        }
    }
    // Failed actor lookup and rejection share the false result at +0xfc.




    BOOL bStoppedAtTarget = bTestCollisionResponse && ProjectileHadCollision(tImpactData);
    if (!bStoppedAtTarget)
    {
        if (m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING)
        {
            m_eMovementType = MOVEMENT_TYPE_FIXED_PROJECTILE;
            UpdateFixed();
        }
    }
    return bStoppedAtTarget;
}




void SActorMovement::ProjNavReflectResponse()
{
    ACDID idACDProjectile = GetACDID();
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (!ptACDProjectile)
        return;

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    PowerID idPower = ActorPowersGetOwnerPower(idActorProjectile);
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
        return;

    SpecificPowerSendMessage(idPower, POWER_WALL_BOUNCE, (void *)(intptr_t)idACDProjectile);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, m_ptACD->id, FALSE);

    WorldPlace placeCurrent;
    GetPlace(placeCurrent);

    ACDTranslateFixedUpdateMessage tMessage;
    tMessage.ann = ANNFromACD(m_ptACD->id);
    tMessage.vPos = placeCurrent.wp;
    tMessage.vVel = m_vVel;
    MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_FIXED_UPDATE, &tMessage, sizeof(tMessage));
    m_ptACD->ModifiedClear(ACDMODIFIED_MOVEMENT);
}











void SActorMovement::ProjToadIdleResponse()
{
    ACDID idACDProjectile = GetACDID();
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (!ptACDProjectile)
        return;

    ActorID idActorProjectile = (ActorID)ACDGetOwner(idACDProjectile);
    PowerID idPower = ActorPowersGetOwnerPower(idActorProjectile);
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
        return;

    SpecificPowerSendMessage(idPower, POWER_TOAD_IDLE, (void *)(intptr_t)idACDProjectile);
}






































SNO SActorMovement::GetActorSNO() const
{
    return m_ptACD->snoActor;
}






void SActorMovement::OnSceneChanged(SSceneID idSSceneOld, SSceneID idSSceneNew)
{
    if (SceneCommonDataTryToGet(idSSceneOld))
        SScenePutACDInScene(idSSceneOld, GetACDID(), FALSE);
    if (idSSceneNew != SSCENEID_INVALID)
        SScenePutACDInScene(idSSceneNew, GetACDID(), TRUE);
}










void SActorMovement::UpdateFacing()
{
    if (ShouldSendUpdates())
        m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
}






void SActorMovement::UpdateClientFacing()
{
    if (!m_ptACD->IsModifiedSpecific(ACDMODIFIED_MOVEMENT))
        m_eMovementType = MOVEMENT_TYPE_NONE;
}







void SActorMovement::ArrivingAtDest(uint32 uTicksAway, BOOL bStuck, BOOL bWithinRadiusOfTarget, ACDID idACDBlocker)
{
    if (uTicksAway == 0 && !m_bWaitAtDestination)
    {
        if (m_pfnCallback)
        {
            m_pfnCallback(m_idActor);
            m_pfnCallback = NULL;
        }
        BrainUpCallAtDestination(m_idActor, bStuck, bWithinRadiusOfTarget, idACDBlocker);
    }
}









































































































































































































void SActorMovement::ClientFace(Angle aYaw, BOOL bSnap)
{
    if (!IsMoving() || (m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
    {
        SNO snoPrimaryPower = ActorPowersGetPrimaryPower(m_idActor);
        if (snoPrimaryPower != SNO_NONE)
        {
            if (!PowerClientControlsFacing(AutoPowerDefinitionR(snoPrimaryPower)))
                return;
        }

        if (m_eSnapState == SNAP_NONE && m_ptACD->IsAlive())
        {
            m_eMovementType = MOVEMENT_TYPE_CLIENT_AUTH_FACING;
            SetNewFacing(aYaw, bSnap, TRUE);
            QueueUpdate(1);
        }
    }
}




















void SActorMovement::MoveTo(const WorldPlace & placeDest, float flSpeed, float flAccel, ActorAnimationTag eAnimTag, uint32 uMoveFlags, float flMaxRangeFromDest, ACDID idACDTarget, SNO snoPowerPassability, uint32 dwWalkInPlaceTicks, int32 nMaxTicksToWalk, float flMaxPathfindingDist, ActorMoveDoneCallback pfnCallback)
{
    BOOL bResetCache;
    if (ACDIsPlayer(m_ptACD->id))
    {
        bResetCache = !(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT));
    }
    else
    {
        AnimReason eAnimReason = AnimPlayerGetAnimReason(*m_ptACD);
        bResetCache = eAnimReason == ANIM_REASON_POWER;
    }
    if (bResetCache)
        ResetCache();

    ActorMovement::MoveTo(placeDest, flSpeed, flAccel, eAnimTag, uMoveFlags, flMaxRangeFromDest, idACDTarget, snoPowerPassability, dwWalkInPlaceTicks, nMaxTicksToWalk, flMaxPathfindingDist, pfnCallback);

    if (m_eMovementType != MOVEMENT_TYPE_NONE && !(m_uMoveFlags & ((1 << MOVEBIT_FROM_POWER) | (1 << MOVEBIT_KNOCKBACK))))
        BrainStanceSetWalking(m_idActor);
}





























ACDID SActorMovement::PerformProjectileCollision(uint32 & uTicksToTravel) const
{
    WorldPlace placeCurr;
    Vector3D vToDest;
    WorldPlace placeDest;
    ACDID idACDHit;
    float flUnitTravel;

    GetPlace(placeCurr);
    ComputeTravelThisIteration(uTicksToTravel, vToDest);
    placeDest.idSWorld = placeCurr.idSWorld;
    placeDest.wp = placeCurr.wp + vToDest;

    ACDID idACD = GetACDID();
    idACDHit = ACDID_INVALID;
    Actor * ptActor = ActorGet(m_idActor);
    if (ptActor->ptProjectile)
    {
        flUnitTravel = 1.0f;
        BOOL bCollided = ProjectileTestCollisionAlongLine(idACD, placeCurr, placeDest, idACDHit, flUnitTravel);
        if (bCollided)
            uTicksToTravel = Math::FloatToInt(flUnitTravel * uTicksToTravel);
        return idACDHit;
    }
    return ACDID_INVALID;
}









void SActorMovement::MoveAtFixedVelocity(Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerPassability)
{
    ActorMovement::MoveAtFixedVelocity(vVel, uMoveFlags, eAnimTag, snoPowerPassability);
    ResetCache();
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
}









BOOL SActorMovement::MoveWithArc(const WorldPlace & placeDest, ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, float flRelativeHeight, SNO snoPowerPassability, float flUnitBounce)
{
    return ActorMovement::MoveWithArc(placeDest, eFlightAnimTag, eLandingAnimTag, uMoveFlags, flGravity, flRelativeHeight, snoPowerPassability, flUnitBounce);
}











BOOL SActorMovement::MoveWithArc(ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, Vector3D & vInitialVel, SNO snoPowerPassability, float flUnitBounce, const WorldPlace * ptPlaceDest)
{
    BOOL bResult = ActorMovement::MoveWithArc(eFlightAnimTag, eLandingAnimTag, uMoveFlags, flGravity, vInitialVel, snoPowerPassability, flUnitBounce, ptPlaceDest);
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    return bResult;
}













void SActorMovement::ReflectArcingProjectile(const Vector3D & vNewVel)
{
    if (IsInArcingMovement())
    {
        m_vVel = vNewVel;
        m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    }
}






void SActorMovement::MoveWithDetPath(DeterministicPathType eDPathType, const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerSource)
{
    if (eDPathType > DPATH_NONE && eDPathType < DPATH_COUNT)
    {
        Angle aOriginalYaw = Math::ATan(vVel.y, vVel.x);
        WorldPlace placeCurr = m_ptACD->GetPlace();
        m_uUpdateInterval = DetPathGetUpdateInterval(eDPathType);
        uint32 uFirstUpdateInterval = GetEarliestUpdateTickOffset();
        StartDetPath(eDPathType, vVel, uMoveFlags, eAnimTag, snoPowerSource, aOriginalYaw, placeCurr.wp, SGameGetRNG(), 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, ProjectileGetSpeedMultiplier(GetACDID()));
        ResetCache();
        m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
        QueueUpdate(uFirstUpdateInterval);
    }
}




























void SActorMovement::MoveWithSinWave(DeterministicPathType eDPathType, const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerSource, const DPathSinData & tSinData)
{
    Angle aOriginalYaw = Math::ATan(vVel.y, vVel.x);
    WorldPlace placeCurr = m_ptACD->GetPlace();
    m_uUpdateInterval = DetPathGetUpdateInterval(eDPathType);
    uint32 uFirstUpdateInterval = GetEarliestUpdateTickOffset();
    StartDetPathSinWave(eDPathType, vVel, uMoveFlags, eAnimTag, snoPowerSource, aOriginalYaw, placeCurr.wp, SGameGetRNG(), 0, 0.0f, tSinData, ProjectileGetSpeedMultiplier(GetACDID()));
    ResetCache();
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    QueueUpdate(uFirstUpdateInterval);
}

























void SActorMovement::MoveWithSpiral(DeterministicPathType eDPathType, const Vector3D & vTarget, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const DPathSinData & tSinData)
{
    WorldPlace placeCurr = m_ptACD->GetPlace();
    m_uUpdateInterval = DetPathGetUpdateInterval(eDPathType);
    uint32 uFirstUpdateInterval = GetEarliestUpdateTickOffset();
    StartDetPathSpiral(eDPathType, placeCurr.wp, vTarget, uMoveFlags, eAnimTag, 0.0f, 0.0f, tSinData, ProjectileGetSpeedMultiplier(GetACDID()));
    ResetCache();
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    QueueUpdate(uFirstUpdateInterval);
}





















void SActorMovement::MoveWithPieWedge(const Vector3D & vTarget, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const DPathPieData & tPieData)
{
    WorldPlace placeCurr = m_ptACD->GetPlace();
    DeterministicPathType eDPathType = DPATH_PIE_WEDGE;
    m_uUpdateInterval = DetPathGetUpdateInterval(eDPathType);
    uint32 uFirstUpdateInterval = GetEarliestUpdateTickOffset();
    StartDetPathPieWedge(placeCurr.wp, vTarget, uMoveFlags, eAnimTag, tPieData, ProjectileGetSpeedMultiplier(GetACDID()), 1.0f);
    ResetCache();
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    QueueUpdate(uFirstUpdateInterval);
}

















void SActorMovement::MoveWithCurvedOffset(const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const ProjectileCurvedOffsetData & tCurvedOffsetData)
{
    Angle aOriginalYaw = Math::ATan(vVel.y, vVel.x);
    WorldPlace placeCurr = m_ptACD->GetPlace();
    m_uUpdateInterval = DetPathGetUpdateInterval(DPATH_CURVED_OFFSET);
    uint32 uFirstUpdateInterval = GetEarliestUpdateTickOffset();
    StartDetPath(DPATH_CURVED_OFFSET, vVel, uMoveFlags, eAnimTag, SNO_NONE, aOriginalYaw, placeCurr.wp, SGameGetRNG(), 0, 0.0f, tCurvedOffsetData.flLateralOffset, 0.0f, 0.0f, 0.0f, ProjectileGetSpeedMultiplier(GetACDID()));
    ResetCache();
    m_ptACD->ModifiedMark(ACDMODIFIED_MOVEMENT);
    QueueUpdate(uFirstUpdateInterval);
}

























BOOL SActorMovement::PathBlockedByObject(ACDID idACD, const Vector2D & vDistUntilColl)
{
    if (m_pfnMovementBlockedCallback)
        return m_pfnMovementBlockedCallback(m_idActor, idACD, vDistUntilColl);
    return FALSE;
}








void UpdateRangeScriptEventsPlayer(ActorCommonData * ptAcdPlayer)
{
    float fTriggerMaxRadius = LuaGetMaxPlayerInRangeRadiusInScriptMessageArray();
    if (fTriggerMaxRadius > 0.0f)
    {
        WorldPlace wpCenter = ptAcdPlayer->GetWorldPlaceCenter();
        POS pos;
        XPooledDataIDList tListDetectedACDs(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(wpCenter, fTriggerMaxRadius, ptAcdPlayer->id, tListDetectedACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
        pos = tListDetectedACDs.GetHeadPosition();
        while (pos)
        {
            ACDID detectedACDID = (ACDID)tListDetectedACDs.GetNext(pos);
            ActorCommonData * ptDetectedAcd = ACDTryToGet(detectedACDID);
            if (ptDetectedAcd)
            {
                const WorldLocation & tDetectedWorldLocation = ptDetectedAcd->GetWorldLocation();
                WorldPlace tDetectedWorldPlace;
                tDetectedWorldPlace.idSWorld = tDetectedWorldLocation.idSWorld;
                tDetectedWorldPlace.wp = tDetectedWorldLocation.transform.wp;
                if (WorldPlaceInSameWorld(tDetectedWorldPlace, wpCenter))
                    ScriptEventOnPlayerInRangeOfActor(ptAcdPlayer->id, detectedACDID, WorldPlaceDistance(tDetectedWorldPlace, wpCenter));
            }
        }
    }
}













static void sUpdateRangeScriptEventsNpc(ActorCommonData * ptAcdNpc)
{
    float fTriggerMaxRadius = LuaGetMaxPlayerInRangeRadiusInScriptMessageArray();
    if (fTriggerMaxRadius > 0.0f)
    {
        WorldPlace wpCenter = ptAcdNpc->GetWorldPlaceCenter();
        POS pos;
        XPooledDataIDList tListDetectedACDs(MemoryPoolsGetDATAIDListPool());
        PlayersFindInRadius(wpCenter, fTriggerMaxRadius, tListDetectedACDs);
        pos = tListDetectedACDs.GetHeadPosition();
        while (pos)
        {
            ACDID detectedACDID = (ACDID)tListDetectedACDs.GetNext(pos);
            ActorCommonData & tDetectedAcd = ACDGet(detectedACDID);
            const WorldLocation & tDetectedWorldLocation = tDetectedAcd.GetWorldLocation();
            WorldPlace tDetectedWorldPlace;
            tDetectedWorldPlace.idSWorld = tDetectedWorldLocation.idSWorld;
            tDetectedWorldPlace.wp = tDetectedWorldLocation.transform.wp;
            if (WorldPlaceInSameWorld(tDetectedWorldPlace, wpCenter))
                ScriptEventOnPlayerInRangeOfActor(detectedACDID, ptAcdNpc->id, WorldPlaceDistance(tDetectedWorldPlace, wpCenter));
        }
    }
}













void SActorMovement::ActivateTriggers(const Vector2D & vPosStart, const Vector2D & vPosEnd) const
{
    if (!m_ptACD->IsLoading())
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        if (!(tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_AI_DISABLED_BIT)))
        {
            XPooledDataIDList * ptTriggerList = ActorAIGetTriggerList();
            POS pos;
            LineSeg2D segMove;
            segMove.Set(vPosStart, vPosEnd);
            pos = ptTriggerList->GetHeadPosition();
            while (pos)
            {
                ActorID idTrigger = (ActorID)ptTriggerList->GetNext(pos);
                ProximityDetectorCheckActivation(idTrigger, *m_ptACD, segMove);
            }
            if (ACDIsPlayer(m_ptACD->id))
            {
                UpdateRangeScriptEventsPlayer(m_ptACD);
                Minimap::DiscoverMinimapMarkers(*m_ptACD);
            }
            else
            {
                sUpdateRangeScriptEventsNpc(m_ptACD);
            }
        }
    }
}







void SActorMovement::ActivateTriggersToDest(const WorldPlace & placeDest)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);
    if (placeCurr.idSWorld == placeDest.idSWorld)
        ActivateTriggers(placeCurr.wp.As2D(), placeDest.wp.As2D());
}






BOOL SActorMovement::ActivateTriggersToDestWithPathFind(const WorldPlace & placeDest)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);
    PathPoint2DList listPathPoints;
    listPathPoints.SetPool(NavCellPathGetPointPool());
    BOOL bFoundPath = TRUE;
    PathResult ePathResult = FindPath(placeDest, listPathPoints, TRUE, FALSE, NULL);
    if (ePathResult == PATH_RESULT_FOUND)
    {
        POS pos = listPathPoints.GetHeadPosition();
        Vector2D vLastPoint = placeCurr.wp.As2D();
        while (pos)
        {
            Vector2D & vCurrPoint = listPathPoints.GetNext(pos);
            ActivateTriggers(vLastPoint, vCurrPoint);
            vLastPoint = vCurrPoint;
        }
    }
    else if (ePathResult == PATH_RESULT_NOT_NECESSARY)
    {
        ActivateTriggers(placeCurr.wp.As2D(), placeDest.wp.As2D());
    }
    else
    {
        bFoundPath = FALSE;
    }
    return bFoundPath;
}







void SActorMovement::StopAndSyncPosition(BOOL bSnap, BOOL bPreplayed)
{
    StopNow();

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, m_ptACD->id, bPreplayed);

    CheckAndSendSnapState(listDestPlayers);
    SendMoveMessageSync(listDestPlayers, bSnap);
    SendMoveMessageFacing(listDestPlayers);
    m_ptACD->ModifiedClear(ACDMODIFIED_MOVEMENT);
}







void SActorMovement::ChangeFixedVelocity(Vector3D & vNewVelocity)
{
    if (m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE || m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING)
    {
        WorldPlace placeCurrent;
        GetPlace(placeCurrent);
        GetCurrentInterpolatedPos(placeCurrent.wp);
        Vector3D vVelocity = GetVelocity();
        placeCurrent.wp += vVelocity;
        if (!(m_uMoveFlags & (1 << MOVEBIT_NO_SNAP_TO_GROUND)))
            SnapPlaceToGround(placeCurrent);
        SetNewPlace(placeCurrent, FALSE, FALSE, TRUE);

        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetThatKnowACD(listDestPlayers, m_ptACD->id, FALSE);

        m_vVel = vNewVelocity;
        m_vLastVel = GetVelocity();

        ACDTranslateFixedUpdateMessage tMessage;
        tMessage.ann = ANNFromACD(m_ptACD->id);
        tMessage.vPos = placeCurrent.wp;
        tMessage.vVel = GetVelocity();
        MessageSendToClient(listDestPlayers, SMSG_ACD_TRANSLATE_FIXED_UPDATE, &tMessage, sizeof(tMessage));

        if (m_eMovementType == MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING)
            m_eMovementType = MOVEMENT_TYPE_FIXED_PROJECTILE;

        QueueUpdate(GetEarliestUpdateTickOffset());
        m_ptACD->ModifiedClear(ACDMODIFIED_MOVEMENT);
    }
}



















CRefString SActorMovement::DebugGetDumpString()
{
    float flRadius;
    WorldPlace tPlace;
    Actor * ptActor = ActorTryToGet(m_idActor);
    GetPlaceAndRadius(tPlace, flRadius, FALSE);
    CRefString strResult;
    CRefString strTemp;
    strResult.Format("Actor = %s\n", ptActor ? MarkerNameGetString(ptActor->tName) : "INVALID ACTOR");
    strTemp.Format("SWorldID = 0x%x [%.4f, %.4f, %.4f] Radius = %.4f\n", tPlace.idSWorld, tPlace.wp.x, tPlace.wp.y, tPlace.wp.z, flRadius);
    strResult += strTemp;
    {
        SSceneID idSScene = m_hNavCell.idSScene;
        if (SceneCommonDataTryToGet(idSScene))
            strTemp.Format("NavCell SSceneID (0x%x) [%s], uCellIndex = 0x%x\n", idSScene, SNOToString(SCENE_SNO, SceneCommonDataGet(m_hNavCell.idSScene).snoScene, FALSE).str(), m_hNavCell.uCellIndex);
        else
            strTemp.Format("NavCell SSceneID (0x%x) [%s], uCellIndex = 0x%x\n", idSScene, "INVALID SCENE", m_hNavCell.uCellIndex);
    }
    strResult += strTemp;
    ACDID idACD = GetACDID();
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        SSceneID idSScene = ACDGetCurrentSSceneID(*ptACD);
        if (SceneCommonDataTryToGet(idSScene))
            strTemp.Format("ACD SSceneID (0x%x) [%s]\n", idSScene, SNOToString(SCENE_SNO, SceneCommonDataGet(idSScene).snoScene, FALSE).str());
        else
            strTemp.Format("ACD SSceneID (0x%x) [%s]\n", idSScene, "INVALID SCENE");
    }
    else
    {
        strTemp = "INVALID ACD\n";
    }
    strResult += strTemp;
    NavMeshHandle hNavMeshHandle;
    ScenesCommonNavMeshNodeFromPlace(tPlace, &hNavMeshHandle, FALSE, FALSE);
    {
        SSceneID idSScene = hNavMeshHandle.idSScene;
        if (SceneCommonDataTryToGet(idSScene))
            strTemp.Format("SSceneID from place: (0x%x) [%s]\n", idSScene, SNOToString(SCENE_SNO, SceneCommonDataGet(hNavMeshHandle.idSScene).snoScene, FALSE).str());
        else
            strTemp.Format("SSceneID from place: (0x%x) [%s]\n", idSScene, "INVALID SCENE");
    }
    strResult += strTemp;
    return strResult;
}



void SActorMovement::PrepareToSnapToNewPlace()
{
    ActivateTriggersAtLocation(TRUE);
}





void SActorMovement::SnapToNewPlace(const WorldPlace & placeNew, BOOL bSnapToGround, BOOL bSoftSnap, BOOL bSendToClient, uint32 uCameraSmoothingTicks)
{
    if (IsMoving())
    {
        ActorID idActor = (ActorID)ACDGetOwner(m_ptACD->id);
        SNO snoPowerPrimary = ActorPowersGetPrimaryPower(idActor);
        if (snoPowerPrimary != SNO_NONE)
            ActorPowersInterruptPrimary(idActor, TRUE);
    }

    BOOL bPlayer = ACDIsPlayer(m_ptACD->id);
    if (bPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(m_ptACD->id);
        ptPlayer->dwTickLastVerification = GameGetServerTime();
        ptPlayer->tPlaceLastVerification = placeNew;
        ptPlayer->flMaxSpeedThisVerificationInterval = 0.0f;
        ResetValidationToPlace(placeNew);
    }

    WorldPlace placeCurr;
    GetPlace(placeCurr);
    ActorMovement::SnapToNewPlace(placeNew, bSnapToGround, bSoftSnap, bSendToClient, 0);

    uint32 uNow = GameGetServerTime();
    uint32 uCameraTime = uNow + uCameraSmoothingTicks;
    m_uCameraSmoothingTime = m_uCameraSmoothingTime ? Math::Min(m_uCameraSmoothingTime, uCameraTime) : uCameraTime;

    if (!WorldPlaceEqual(placeCurr, placeNew))
        ActivateTriggers(placeCurr.wp.As2D(), placeNew.wp.As2D());
    else if (!bPlayer || !m_ptACD->IsLoading())
        ActivateTriggersAtLocation(FALSE);

    if (bSoftSnap && m_eSnapState < SNAP_PENDING_NORMAL)
        m_eSnapState = SNAP_PENDING_SOFT;
    else
        m_eSnapState = SNAP_PENDING_NORMAL;
}






































void SActorMovement::ForceBrainUpdate()
{
    BrainStanceForceUpdate(m_idActor);
}




void SActorMovement::ActivateTriggersAtLocation(BOOL bLeaving)
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (!(tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_AI_DISABLED_BIT)))
    {
        XPooledDataIDList * ptTriggerList = ActorAIGetTriggerList();
        POS pos = ptTriggerList->GetHeadPosition();
        while (pos)
        {
            ActorID idTrigger = (ActorID)ptTriggerList->GetNext(pos);
            ProximityDetectorActivateAtPlace(idTrigger, *m_ptACD, bLeaving);
        }
        if (!bLeaving)
        {
            if (ACDIsPlayer(m_ptACD->id))
            {
                UpdateRangeScriptEventsPlayer(m_ptACD);
                Minimap::DiscoverMinimapMarkers(*m_ptACD);
            }
            else
            {
                sUpdateRangeScriptEventsNpc(m_ptACD);
            }
        }
    }
}







BOOL SActorMovement::RaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd, Vector3D & vIntersect, Vector3D & vNormal, float & flUnitTraveled, uint16 uExcludeMask) const
{
    return DominoRaycastAgainstWorld(placeStart, placeEnd, vIntersect, vNormal, flUnitTraveled, uExcludeMask);
}









void SActorMovement::PlayPreplayedAnim(ActorAnimationTag eAnimTag, float flSpeedScalar)
{
    if (AnimPlayerAnimExists(*m_ptACD, eAnimTag))
    {
        ActorID idActor = (ActorID)ACDGetOwner(m_ptACD->id);
        BrainAPIDoPlayAnimation(idActor, eAnimTag, (uint32)-1, FALSE, FALSE, 0.0f, TRUE, flSpeedScalar);
    }
}





void SActorMovement::ResetCache()
{
    m_uCacheFlags = 0;
    m_bForceCacheSend = TRUE;
    m_aCachedYaw = FLT_MAX;
    m_bCachedSnapFacing = FALSE;
    m_flCachedSpeed = FLT_MAX;
    m_uCachedMoveFlags = 0;
    m_uCachedAnimTag = (uint32)-1;
    for (int32 i = 0; i < 4; i++)
    {
        m_arACDCachedFacingPlayer[i] = ACDID_INVALID;
        m_arCachedFacingAngle[i] = 0.0f;
    }
}





void SActorMovement::PostMessage()
{
    if (m_eSnapState != SNAP_NONE)
        m_eSnapState = SNAP_SENT;

    m_bSnapFacingNextClientUpdate = FALSE;
    m_bForceCacheSend = FALSE;

    if (m_uCacheFlags & (1 << MOVECACHE_SPEED))
        m_flCachedSpeed = m_flCurrentSpeed;
    if (m_uCacheFlags & (1 << MOVECACHE_YAW))
        m_aCachedYaw = ACDGetYaw(m_ptACD->id);
    if (m_uCacheFlags & (1 << MOVECACHE_SNAP_FACING))
        m_bCachedSnapFacing = m_bSnapFacingNextClientUpdate;
    if (m_uCacheFlags & (1 << MOVECACHE_MOVEFLAGS))
        m_uCachedMoveFlags = m_uMoveFlags & ~MOVEFLAG_DONT_SEND_MASK;
    if (m_uCacheFlags & (1 << MOVECACHE_ANIMTAG))
        m_uCachedAnimTag = m_eAnimTag;

    m_uCacheFlags = 0;
}
















void SActorMovement::Bounced()
{
    ACDPlayEffect(m_ptACD->id, EFFECT_PROJECTILE_BOUNCE, 0, ACDID_INVALID);
}


const int32 MAX_BANKED_TICKS = 240;
const int32 BANK_LIFETIME = 120;
const int32 MAX_BANK_ITEMS = 240;


static void sAddTicksToBank(
    TickBankBucketList & listTickBanks,
    int32 nTicks)
{
    if (nTicks != 0 && listTickBanks.Count() < MAX_BANK_ITEMS)
    {
        TickBankBucket tBucket;
        tBucket.dwTick = GameGetServerTime() + BANK_LIFETIME;
        tBucket.nStoredTicks = nTicks;
        listTickBanks.AddTail_Tracked(tBucket);
    }
}



static void sPruneTickBank(
    TickBankBucketList & listTickBanks)
{
    uint32 uNow = GameGetServerTime();


    POS pos = listTickBanks.GetHeadPosition();
    while (pos)
    {
        TickBankBucket & tBucket = listTickBanks.GetNext(pos);
        if (uNow >= tBucket.dwTick)
        {
            listTickBanks.RemoveHead();
        }
        else
        {
            break;
        }
    }
}



static uint32 sGetTotalTicksInBank(
    TickBankBucketList & listTickBanks)
{
    int32 nCount = 0;

    sPruneTickBank(listTickBanks);

    POS pos = listTickBanks.GetHeadPosition();
    while (pos)
    {
        TickBankBucket & tBucket = listTickBanks.GetNext(pos);
        nCount += tBucket.nStoredTicks;
    }

    return Math::Max(nCount, 0);
}





static void sWithdrawFromTickBank(
    TickBankBucketList & listTickBanks,
    int32 nTicks)
{
    sPruneTickBank(listTickBanks);

    POS pos = listTickBanks.GetHeadPosition();
    while (pos && nTicks > 0)
    {
        TickBankBucket & tBucket = listTickBanks.GetNext(pos);
        if (nTicks >= tBucket.nStoredTicks)
        {
            nTicks -= tBucket.nStoredTicks;
            listTickBanks.RemoveHead();
        }
        else
        {
            tBucket.nStoredTicks -= nTicks;
            nTicks = 0;
        }
    }
}



void SActorMovement::AddTickDebt(int32 nTicks)
{
    if (nTicks > 0)
    {
        Player * ptPlayer = PlayerGetByACD(GetACDID());
        if (ptPlayer)
        {
            TickBankBucketList & listTickBanks = SPlayerGetTickBankList(*ptPlayer);
            sWithdrawFromTickBank(listTickBanks, nTicks);
        }
    }
}















BOOL SActorMovement::DetPathAdjustFacing(Angle aNewYaw)
{
    if (m_eMovementType == MOVEMENT_TYPE_DETPATH)
    {
        if (DetPathProcessAdjustFacing(*this, m_tDPath, aNewYaw))
        {
            ForceSendUpdate();
            return TRUE;
        }
    }
    return FALSE;
}


SNO PlayerBrainGetQueuedPower(const ACDID idACDPlayer);  // X/Server/AI/PlayerBrain.cpp:476
float SPlayerGetHighestSpeedValue(Player & tPlayer);  // X/Server/SPlayer.cpp:11148
TickRateTracker & SPlayerGetTickRateTracker(const Player & tPlayer);  // X/Server/SPlayer.cpp:1695
void SPlayerRecordAcceptedSpeedValue(Player & tPlayer, float flValue);  // X/Server/SPlayer.cpp:11126

const float MOVEMENT_SPEED_EPSILON = 0.01f;
const float MOVEMENT_MAX_RANGE_SNAP_DISTANCE = 15.0f;
const int32 MOVEMENT_BANK_SLOP_TICKS = 60;

static BOOL sClientMovementMessagePlayerCannotMove(
    const ActorCommonData & tACDPlayer)
{
    if (tACDPlayer.IsImmobilized())
        return TRUE;



    if (tACDPlayer.IsAttached())
        return TRUE;



    if (tACDPlayer.IsFeared())
        return TRUE;



    if (tACDPlayer.IsStunned())
        return TRUE;



    if (tACDPlayer.IsBlind())
        return TRUE;



    if (!tACDPlayer.IsAlive())
        return TRUE;

    return FALSE;
}



































CRefString SActorMovement::GetVerboseMovementMessage()
{
    return CRefString();
}


























































































float SActorMovement::GetDistSqrFromLastProjectedSegment(
    const Vector2D & vPos,
    Vector2D & vProjectedPoint)
{
    Vector2D vDelta = m_vLastClientReportedDest - m_vLastClientReportedStartFromProjectedSegment;
    Vector2D vToPos = vPos - m_vLastClientReportedStartFromProjectedSegment;
    if (vDelta.MagnitudeSquared() < EPSILON * EPSILON)
    {
        vProjectedPoint = m_vLastClientReportedStartFromProjectedSegment;

        return vToPos.MagnitudeSquared();
    }

    float flLength = vDelta.Normalize();
    float flProjDist = Vector2D::DotProduct(vToPos, vDelta);
    flProjDist = Math::Pin(flProjDist, 0.0f, flLength);
    vProjectedPoint = vDelta * flProjDist;
    vProjectedPoint += m_vLastClientReportedStartFromProjectedSegment;

    Vector2D vToProjPoint = vProjectedPoint - vPos;
    return vToProjPoint.MagnitudeSquared();
}

float SActorMovement::GetDistSqrFromLastLookAheadSegment(
    const Vector2D & vPos)
{
    Vector2D vDelta = m_vLastClientReportedDest - m_vLastClientReportedStartFromLookAhead;
    Vector2D vToPos = vPos - m_vLastClientReportedStartFromLookAhead;
    if (vDelta.MagnitudeSquared() < EPSILON * EPSILON)
    {
        return vToPos.MagnitudeSquared();
    }

    float flLength = vDelta.Normalize();
    float flProjDist = Vector2D::DotProduct(vToPos, vDelta);
    flProjDist = Math::Pin(flProjDist, 0.0f, flLength);
    Vector2D vProjectedPoint = vDelta * flProjDist;
    vProjectedPoint += m_vLastClientReportedStartFromLookAhead;

    Vector2D vToProjPoint = vProjectedPoint - vPos;
    return vToProjPoint.MagnitudeSquared();
}

MoveValidationError SActorMovement::ValidateClientMovementMessage(
    uint32 uTickBase,
    uint32 uTicksAhead,
    const Vector3D & vDest,
    float flSpeed,
    SNO snoPower,
    CRefString & strError)
{
    uint32 uValidationLevel = MOVEMENT_VALIDATION_ENABLED_LEVEL;
    uint32 uDistanceSlopTicks = MOVEMENT_VALIDATION_TICK_TOLERANCE_DISTANCE;
    BOOL bVerboseMessage = FALSE;
    float flTickRateTolerance = MOVEMENT_TICK_RATE_TOLERANCE_DEFAULT;
















    Player* ptPlayer = PlayerGetByACD(GetACDID());
    if (!ptPlayer)
    {

        strError = "No Player";
        return MOVEVALIDATION_MODERATE;
    }

    // 12: no named constant for this limit in the DWARF (a #define in the original, presumably)
    if (uTicksAhead > 12)
    {

        strError.Format("Predicting %d ticks ahead", uTicksAhead);
        return MOVEVALIDATION_SERIOUS;
    }


    if (uTickBase == 0)
    {



        strError = "TickBaseZero";
        return MOVEVALIDATION_MODERATE;
    }


    if (!isfinite(vDest.x) ||
        !isfinite(vDest.y) ||
        !isfinite(vDest.z))
    {
        strError.Format("[%.2f, %.2f, %.2f] has infinite float values!", vDest.x, vDest.y, vDest.z);
        return MOVEVALIDATION_SNAP;
    }


    SWorld * ptSWorld = SWorldTryToGet(m_ptACD->GetSWorldID());
    if (ptSWorld &&
        (vDest.x < ptSWorld->vExtMin.x - WORLD_BORDER_DISTANCE ||
         vDest.y < ptSWorld->vExtMin.y - WORLD_BORDER_DISTANCE ||
         vDest.x > ptSWorld->vExtMax.x + WORLD_BORDER_DISTANCE ||
         vDest.y > ptSWorld->vExtMax.y + WORLD_BORDER_DISTANCE ||
         Math::Abs(vDest.z) >= 100000.0f ||
         isnan(vDest.z)))
    {
        strError.Format("[%.2f, %.2f, %.2f] outside of world!", vDest.x, vDest.y, vDest.z);
        return MOVEVALIDATION_SNAP;
    }


    if (sClientMovementMessagePlayerCannotMove(*m_ptACD))
    {
        return MOVEVALIDATION_SNAP;
    }






    ActorMovement * ptMove = m_ptACD->GetActorMovement();
    if (ptMove && ptMove->IsInArcingMovement())
    {
        return MOVEVALIDATION_IGNORE;
    }




    if (snoPower != SNO_NONE && !m_bAcceptNextClientPowerWalk)
    {
        SNO snoPowerPrimary = ActorPowersGetPrimaryPower((ActorID)ACDGetOwner(m_ptACD->id));
        if (snoPowerPrimary != snoPower &&
            GlobalSNOGet(GLOBALSNO_POWERS_WALK) != snoPower)
        {

            SNO snoPowerQueued = PlayerBrainGetQueuedPower(m_ptACD->id);
            if (snoPowerQueued == snoPower)
            {
                PlayerBrainTryQueuedPower(m_ptACD->id);
            }

            snoPowerPrimary = ActorPowersGetPrimaryPower((ActorID)ACDGetOwner(m_ptACD->id));
        }

        if (snoPowerPrimary != snoPower)
        {
            return MOVEVALIDATION_IGNORE;
        }
    }



    m_bAcceptNextClientPowerWalk = FALSE;

    WorldPlace placeCurr;
    GetPlace(placeCurr);
    float flMaxSpeed = SPlayerGetHighestSpeedValue(*ptPlayer);
    if (flSpeed > (flMaxSpeed + MOVEMENT_SPEED_EPSILON))
    {
        if (uTicksAhead == 0)
        {
            Vector2D vDelta = vDest.As2D() - placeCurr.wp.As2D();
            if (vDelta.MagnitudeSquared() < EPSILON * EPSILON)
            {
                return MOVEVALIDATION_IGNORE;
            }
        }





        strError.Format("Invalid speed! %.2f Max = %.2f", flSpeed, flMaxSpeed);
        return MOVEVALIDATION_LOW;
    }




    uint32 uNow = GameGetServerTime();

    TickBankBucketList & listTickBanks = SPlayerGetTickBankList(*ptPlayer);






    if (uNow < m_uLastServerTickTime)
    {

        strError.Format("Server %d ticks behind", m_uLastServerTickTime - uNow);
        return MOVEVALIDATION_SERIOUS;
    }


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();





    int32 nServerDelayTicks = tSGameGlobals.uTicksSinceLastMessage;
    if (uNow > m_uLastServerTickTime)
    {

        nServerDelayTicks += nServerDelayTicks >> 1;
    }
    else
        nServerDelayTicks = 0;
    int32 nTrueElapsedServerTicks = uNow - m_uLastServerTickTime;
    nTrueElapsedServerTicks += nServerDelayTicks;
    int32 nElapsedServerTicks = Math::Min(nTrueElapsedServerTicks, MAX_BANKED_TICKS);
    if (nTrueElapsedServerTicks > 0)
        sAddTicksToBank(listTickBanks, nElapsedServerTicks);

    m_uLastServerTickTime = uNow;

    if (m_uAckValue != (uint32)-1)
    {
        return MOVEVALIDATION_IGNORE;
    }






    if (m_uLastClientMoveReportedTimeBase != 0)
    {
        if (m_uLastClientMoveReportedTimeBase > uTickBase)
        {

            strError.Format("Client %d ticks behind", m_uLastClientMoveReportedTimeBase - uTickBase);
            return MOVEVALIDATION_MODERATE;
        }


        TickRateTracker & tTickRateTracker = SPlayerGetTickRateTracker(*ptPlayer);
        tTickRateTracker.AddBucket((int32)(uTickBase - m_uLastClientMoveReportedTimeBase), nTrueElapsedServerTicks);












        int32 nElapsedClientTicks = (int32)(uTickBase - m_uLastClientMoveReportedTimeBase);
        nElapsedClientTicks = Math::Min(nElapsedClientTicks, 12);

        uint32 uTotalTicksInBank = sGetTotalTicksInBank(listTickBanks);










        if (nElapsedClientTicks > (int32)uTotalTicksInBank)
        {
            BOOL bAllow = FALSE;
            if (nElapsedClientTicks <= 12)
            {




                if (uNow >= m_uLastBankSlopTime + MOVEMENT_BANK_SLOP_TICKS)
                {
                    m_uLastBankSlopTime = uNow;
                    sAddTicksToBank(listTickBanks, nElapsedClientTicks);
                    bAllow = TRUE;
                }
            }

            if (!bAllow)
            {

                strError.Format("Client elapsed %d, bank has %d", nElapsedClientTicks, uTotalTicksInBank);
                return MOVEVALIDATION_LOW;
            }
        }




        int32 nTicksToMove = nElapsedClientTicks + uTicksAhead;

        if (placeCurr.idSWorld != m_idLastClientReportedWorld)
        {
            ResetValidationToPlace(placeCurr);
        }

        float flMaxDist = flMaxSpeed * (float)(nTicksToMove + uDistanceSlopTicks);
        float flMaxDistSnap = flMaxDist + MOVEMENT_MAX_RANGE_SNAP_DISTANCE;

        Vector2D vProjPoint;
        float flMagSqProj = GetDistSqrFromLastProjectedSegment(vDest.As2D(), vProjPoint);
        float flMagSqLookAhead = GetDistSqrFromLastLookAheadSegment(vDest.As2D());
        if (flMagSqProj > flMaxDistSnap * flMaxDistSnap && flMagSqLookAhead > flMaxDistSnap * flMaxDistSnap)
        {


            strError.Format("Out of Snap Range! %.1f %.1f Max = %.1f", flMagSqProj, flMagSqLookAhead, flMaxDist);
            return MOVEVALIDATION_SNAP;
        }



        else if (flMagSqProj > flMaxDist * flMaxDist && flMagSqLookAhead > flMaxDist * flMaxDist)
        {


            strError.Format("Out of Range! %.1f %.1f Max = %.1f", flMagSqProj, flMagSqLookAhead, flMaxDist);
            return MOVEVALIDATION_IGNORE;
        }




        Vector2D vDeltaFromLastLookAhead = m_vLastClientReportedStartFromLookAhead - vDest.As2D();
        if (vDeltaFromLastLookAhead.MagnitudeSquared() <= EPSILON * EPSILON)
        {
            m_vLastClientReportedStartFromLookAhead = vDest.As2D();
        }
        else
        {
            vDeltaFromLastLookAhead.Normalize();
            vDeltaFromLastLookAhead *= flMaxSpeed * (float)uTicksAhead;
            m_vLastClientReportedStartFromLookAhead = vDeltaFromLastLookAhead + vDest.As2D();
        }
        m_vLastClientReportedDest = vDest.As2D();
        m_vLastClientReportedStartFromProjectedSegment = vProjPoint;

        sWithdrawFromTickBank(listTickBanks, nElapsedClientTicks);



        SPlayerRecordAcceptedSpeedValue(*ptPlayer, flSpeed);
    }
    else
    {

        TickBankBucketList & listTickBanksLoc = SPlayerGetTickBankList(*ptPlayer);
        sAddTicksToBank(listTickBanksLoc, 12);
    }

    m_uLastClientMoveReportedTimeBase = uTickBase;
    m_uLastServerTickTime = uNow;
    return MOVEVALIDATION_OK;

}



void SActorMovement::ResetValidationToCurrentPlace()
{
    WorldPlace placeCurr;
    GetPlace(placeCurr);
    ResetValidationToPlace(placeCurr);
}




inline void SActorMovement::ResetValidationToPlace(const WorldPlace & placeNew)
{
    m_idLastClientReportedWorld = placeNew.idSWorld;
    m_vLastClientReportedStartFromProjectedSegment = placeNew.wp.As2D();
    m_vLastClientReportedStartFromLookAhead = placeNew.wp.As2D();
    m_vLastClientReportedDest = placeNew.wp.As2D();
}




void SActorMovement::AcceptAckValue(uint32 uAckValue)
{
    if (m_uAckValue == uAckValue)
        m_uAckValue = (uint32)-1;
}






void SActorMovement::UpdateServerVariables()
{
}

#include "../AI/ActorAI.h"

BOOL ActorPowersPrimaryIsNotInterruptable(ActorID idActor);
BOOL PowerActorCanWalk(ActorID idActor);
void BrainUpCallOnWalk(ActorID idActor);
void BrainTriggerUpdate(ActorID idActor);
ActorAI * ActorTryToGetAI(const ActorID idActor);

void SActorMovement::ClientMove(WorldPlace & placeDest, float flSpeed, ActorAnimationTag eAnimTag, Angle aYaw)
{
    if ((IsMoving() && !(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT))) || m_eSnapState != 0 || !m_ptACD->IsAlive())
        return;
    ActorID idActor = m_ptACD ? (ActorID)m_ptACD->idOwner : ACTORID_INVALID;
    WorldPlace placeCurr = m_ptACD->GetPlace();
    BOOL bSnap = ActorPowersPrimaryIsNotInterruptable(idActor);
    SNO snoPowerPrimary = ActorPowersGetPrimaryPower(idActor);
    if (snoPowerPrimary != -1)
    {
        PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPowerPrimary), m_ptACD->id);
        if (eTemplate == LOOPING_ANIMATION_POWER || eTemplate == GIZMO_POWER)
            PowerForceStop(ActorPowersGetPrimary(idActor), TRUE);
    }
    BOOL bAlreadyClientWalking = PowerShouldUseClientWalking(*m_ptACD, AutoPowerDefinitionR(snoPowerPrimary));
    DeSyncReason eReason = 0;
    if (!bSnap || bAlreadyClientWalking)
    {
        Vector3D vDelta = placeDest.wp - placeCurr.wp;
        if (vDelta.MagnitudeSquared() > 75.0f * 75.0f)
        {
            bSnap = TRUE;
            eReason = 3;
        }
        else if (BrainGetStanceType(idActor) != STANCE_GETHIT && !PowerActorCanWalk(idActor))
        {
            bSnap = TRUE;
            eReason = 2;
        }
        else
            bSnap = FALSE;
    }
    if (bSnap)
    {
        Player * ptPlayer = ACDGetControllingPlayer(m_ptACD->id);
        PlayerDeSyncSnapMessage tMessage;
        tMessage.placeSnap = placeCurr;
        tMessage.eReason = eReason;
        tMessage.snoPowerCurrent = snoPowerPrimary;
        tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, (GameMessageType)0x10b, &tMessage, sizeof(tMessage));
        return;
    }
    SnapPlaceToGround(placeDest);
    BOOL bDoTriggerToDest = TRUE;
    if (ACDIsInLootRun(*m_ptACD))
    {
        Player * ptPlayer = ACDGetControllingPlayer(m_ptACD->id);
        if (!ptPlayer)
            bDoTriggerToDest = FALSE;
    }
    if (bDoTriggerToDest)
        ActivateTriggersToDest(placeDest);
    SWorldID idSWorldCurr = m_ptACD->GetSWorldID();
    if (idSWorldCurr != placeDest.idSWorld)
        return;
    if (IsMoving() && !(m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
        return;
    WorldPlace placeAfterTrigger = m_ptACD->GetPlace();
    Vector2D vDelta = placeAfterTrigger.wp.As2D() - placeCurr.wp.As2D();
    if (vDelta.MagnitudeSquared() > EPSILON * EPSILON)
        return;
    GetPlace(m_placeLastStamp);
    GetCurrentInterpolatedPos(m_placeLastStamp.wp);
    m_uLastStampTime = GetTimeNow();
    m_flLastStampSpeed = flSpeed;
    m_eMovementType = MOVEMENT_TYPE_CLIENT_AUTH;
    SetNewPlace(placeDest, FALSE, TRUE, TRUE);
    m_flDesiredSpeed = flSpeed;
    m_flCurrentSpeed = flSpeed;
    m_eAnimTag = eAnimTag;
    SetNewFacing(aYaw, FALSE, TRUE);
    m_nMaxTicksToWalk = -1;
    m_ptACD->PickupNearby();
    BrainUpCallOnWalk(m_idActor);
    m_uMoveFlags = 1 << MOVEBIT_ARRIVING_AT_DESTINATION;
    QueueUpdate(1);
    if (bAlreadyClientWalking)
        m_uMinDurationEndStampTime = m_uLastStampTime + 20;
    Player * ptPlayer = PlayerGetByACD(m_ptACD->id);
    if (ptPlayer)
    {
        for (int32 i = 0; i <= ptPlayer->nMaxUsedPetIndex; ++i)
        {
            if (ptPlayer->tPets[i].idACD != ACDID_INVALID)
            {
                ActorID idActorPet = (ActorID)ACDGetOwner(ptPlayer->tPets[i].idACD);
                ActorAI * ptAI = ActorTryToGetAI(idActorPet);
                if (ptAI)
                {
                    BrainTriggerUpdate(idActorPet);
                    BrainAPITriggerUpdate(idActorPet, TRUE);
                    ServerQueueUpdateFunction(0, ptAI->tAIUpdateData);
                }
            }
        }
    }
}

