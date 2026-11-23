// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Movement/SActorMovement.h
// The class and its data members (size 0x290).

#pragma once

#include "../../Common/Movement/ActorMovement.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/DetPaths.h"
#include "../SGame.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"

class ActorCommonData;
class CRefString;
struct ImpactData;
class PlayerList;
struct ProjectileCurvedOffsetData;
struct WorldPlace;

typedef BOOL (*MovementBlockedCallback)(ActorID, ACDID, const Vector2D &);

enum {
    MOVECACHE_YAW = 0,
    MOVECACHE_SNAP_FACING = 1,
    MOVECACHE_SPEED = 2,
    MOVECACHE_MOVEFLAGS = 3,
    MOVECACHE_ANIMTAG = 4,
    MOVECACHE_COUNT = 5,
};

enum {
    MOVEVALIDATION_OK = 0,
    MOVEVALIDATION_IGNORE = 1,
    MOVEVALIDATION_SNAP = 2,
    MOVEVALIDATION_LOW = 3,
    MOVEVALIDATION_MODERATE = 4,
    MOVEVALIDATION_SERIOUS = 5,
    MOVEVALIDATION_COUNT = 6,
};

typedef int32 MoveValidationError;

class SActorMovement : public ActorMovement {  // size 0x290
public:
    virtual void StopNow();  // vtable slot 5
    virtual void GetACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs) const;  // vtable slot 14
    virtual uint32 GetTimeNow() const;  // vtable slot 15
    virtual void SetNewFacing(Angle aFacing, BOOL bSnap, BOOL bSendUpdate);  // vtable slot 16
    virtual void PrepareToSnapToNewPlace();  // vtable slot 17
    virtual void SnapToNewPlace(const WorldPlace & placeNew, BOOL bSnapToGround, BOOL bSoftSnap, BOOL bSendToClient, uint32 uCameraSmoothingTicks);  // vtable slot 18

    SActorMovement() {}  // line 50
    ~SActorMovement();
    BOOL Initialize(ActorID idActor, const WorldPlace & placeStart);
    virtual void Dispose();  // vtable slot 0
    void SendMoveMessage(const PlayerList & listDestPlayers);
    BOOL Update();
    void Postpone();
    virtual void UpdateFacing();  // vtable slot 45
    virtual void UpdateClientFacing();  // vtable slot 46
    virtual void GetPlaceAndRadius(WorldPlace & tPlace, float & flRadius, BOOL bGetMovingRadius) const;  // vtable slot 8
    virtual float GetRadius(BOOL bGetMovingRadius) const;  // vtable slot 9
    virtual void GetPlace(WorldPlace & tPlace) const;  // vtable slot 10
    virtual SSceneID GetSSceneID() const;  // vtable slot 12
    virtual ACDID GetACDID() const;  // vtable slot 11
    virtual BOOL IsServerProjectile() const;  // vtable slot 13
    virtual BOOL StopProjectile();  // vtable slot 7
    void ClientMove(WorldPlace & placeDest, float flSpeed, ActorAnimationTag eAnimTag, Angle aYaw);
    void ClientFace(Angle aYaw, BOOL bSnap);
    virtual void MoveTo(const WorldPlace & placeDest, float flSpeed, float flAccel, ActorAnimationTag eAnimTag, uint32 uMoveFlags, float flMaxRangeFromDest, ACDID idACDTarget, SNO snoPowerPassability, uint32 dwWalkInPlaceTicks, int32 nMaxTicksToWalk, float flMaxPathfindingDist, ActorMoveDoneCallback pfnCallback);  // vtable slot 1
    virtual void MoveAtFixedVelocity(Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerPassability);  // vtable slot 2
    virtual BOOL MoveWithArc(const WorldPlace & placeDest, ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, float flRelativeHeight, SNO snoPowerPassability, float flUnitBounce);  // vtable slot 3
    virtual BOOL MoveWithArc(ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, Vector3D & vInitialVel, SNO snoPowerPassability, float flUnitBounce, const WorldPlace * ptPlaceDest);  // vtable slot 4
    void ReflectArcingProjectile(const Vector3D & vNewVel);
    void MoveWithDetPath(DeterministicPathType eDPathType, const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerSource);
    void MoveWithSinWave(DeterministicPathType eDPathType, const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerSource, const DPathSinData & tSinData);
    void MoveWithSpiral(DeterministicPathType eDPathType, const Vector3D & vTarget, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const DPathSinData & tSinData);
    void MoveWithPieWedge(const Vector3D & vTarget, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const DPathPieData & tPieData);
    void MoveWithCurvedOffset(const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const ProjectileCurvedOffsetData & tCurvedOffsetData);
    virtual void SnapPlaceToGround(WorldPlace & placeCurrent) const;  // vtable slot 19
    void SetMovementBlockedCallback(MovementBlockedCallback pfnNewCallback) { m_pfnMovementBlockedCallback = pfnNewCallback; }  // line 200
    virtual BOOL PathBlockedByObject(ACDID idACD, const Vector2D & vDistUntilColl);  // vtable slot 20
    virtual void ActivateTriggers(const Vector2D & vPosStart, const Vector2D & vPosEnd) const;  // vtable slot 22
    virtual void ActivateTriggersToDest(const WorldPlace & placeDest);  // vtable slot 23
    BOOL ActivateTriggersToDestWithPathFind(const WorldPlace & placeDest);
    virtual void ActivateTriggersAtLocation(BOOL bLeaving);  // vtable slot 24
    void StopAndSyncPosition(BOOL bSnap, BOOL bPreplayed);
    void ChangeFixedVelocity(Vector3D & vNewVelocity);
    virtual void ForceBrainUpdate();  // vtable slot 25
    virtual SNO GetActorSNO() const;  // vtable slot 26
    virtual void ResetCache();  // vtable slot 29
    virtual void PostMessage();  // vtable slot 30
    virtual void Bounced();  // vtable slot 31
    virtual void ProjToadIdleResponse();  // vtable slot 32
    MoveValidationError ValidateClientMovementMessage(uint32 uTickBase, uint32 uTicksAhead, const Vector3D & vDest, float flSpeed, SNO snoPower, CRefString & strError);
    static void UpdateServerVariables();
    void ResetValidationToCurrentPlace();
    void ResetValidationToPlace(const WorldPlace & placeNew);
    void AcceptAckValue(uint32 uAckValue);
    void ForceAcceptNextClientPowerWalk() { m_bAcceptNextClientPowerWalk = TRUE; }  // line 278
    CRefString GetVerboseMovementMessage();
    void AddTickDebt(int32 nTicks);
    BOOL DetPathAdjustFacing(Angle aNewYaw);
// protected in the original:
    virtual void SetNewPlace(const WorldPlace & placeNew, BOOL bSnap, BOOL bClientAuth, BOOL bSendToClient);  // vtable slot 33
    virtual void QueueUpdate(uint32 uDesiredTickDelay);  // vtable slot 34
    virtual void ArrivingAtDest(uint32 uTicksAway, BOOL bStuck, BOOL bWithinRadiusOfTarget, ACDID idACDBlocker);  // vtable slot 35
    virtual void Stuck();  // vtable slot 36
    virtual uint32 GetEarliestUpdateTickOffset();  // vtable slot 37
    void SendMoveMessageSnapped(const PlayerList & listDestPlayers);
    void SendMoveMessageSync(const PlayerList & listDestPlayers, const BOOL bSnap);
    void SendMoveMessageNormal(const PlayerList & listDestPlayers);
    void SendMoveMessageFixed(const PlayerList & listDestPlayers);
    void SendMoveMessageArc(const PlayerList & listDestPlayers);
    void SendMoveMessageDetPath(const PlayerList & listDestPlayers);
    void SendMoveMessageFacing(const PlayerList & listDestPlayers);
    void SendMoveMessageStopped(const PlayerList & listDestPlayers);
    void CheckAndSendSnapState(const PlayerList & listDestPlayers);
    virtual BOOL ProjCollisionResponse(ImpactData & tImpactData);  // vtable slot 48
    virtual void ProjNavReflectResponse();  // vtable slot 49
    BOOL ShouldSendUpdates();
    virtual void ForceSendUpdate();  // vtable slot 51
    virtual void OnSceneChanged(SSceneID idSSceneOld, SSceneID idSSceneNew);  // vtable slot 50
    virtual ACDID PerformProjectileCollision(uint32 & uTicksToTravel) const;  // vtable slot 38
    virtual BOOL RaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd, Vector3D & vIntersect, Vector3D & vNormal, float & flUnitTraveled, uint16 uExcludeMask) const;  // vtable slot 27
    virtual void PlayPreplayedAnim(ActorAnimationTag eAnimTag, float flSpeedScalar);  // vtable slot 28
    virtual CRefString DebugGetDumpString();  // vtable slot 21
    BOOL RefreshAckValue(const PlayerList & listDestPlayers);
    float GetDistSqrFromLastProjectedSegment(const Vector2D & vPos, Vector2D & vProjectedPoint);
    float GetDistSqrFromLastLookAheadSegment(const Vector2D & vPos);
    ActorID m_idActor;  // 0x1d8
    ActorCommonData * m_ptACD;  // 0x1e0
    UpdateData m_tUpdateData;  // 0x1e8
    BOOL m_bIgnoreUpdate;  // 0x208
    BOOL m_bSnapFacingNextClientUpdate;  // 0x20c
    BOOL m_bAcceptNextClientPowerWalk;  // 0x210
    MovementBlockedCallback m_pfnMovementBlockedCallback;  // 0x218
    uint32 m_uLastClientMoveReportedTimeBase;  // 0x220
    SWorldID m_idLastClientReportedWorld;  // 0x224
    Vector2D m_vLastClientReportedStartFromProjectedSegment;  // 0x228
    Vector2D m_vLastClientReportedStartFromLookAhead;  // 0x230
    Vector2D m_vLastClientReportedDest;  // 0x238
    uint32 m_uLastServerTickTime;  // 0x240
    uint32 m_uAckValue;  // 0x244
    uint32 m_uLastBankSlopTime;  // 0x248
    uint32 m_uCameraSmoothingTime;  // 0x24c
    uint32 m_uCacheFlags;  // 0x250
    BOOL m_bForceCacheSend;  // 0x254
    Angle m_aCachedYaw;  // 0x258
    BOOL m_bCachedSnapFacing;  // 0x25c
    float m_flCachedSpeed;  // 0x260
    uint32 m_uCachedMoveFlags;  // 0x264
    uint32 m_uCachedAnimTag;  // 0x268
    ACDID m_arACDCachedFacingPlayer[4];  // 0x26c
    Angle m_arCachedFacingAngle[4];  // 0x27c
};
static_assert(sizeof(SActorMovement) == 0x290, "SActorMovement");

struct Actor;
void SActorMovementInitializeForGame();  // SActorMovement.cpp:87
void SActorMovementDisposeForGame();  // SActorMovement.cpp:104
BOOL SActorMovementInitialize(Actor * ptActor, const WorldPlace & placeStart);  // SActorMovement.cpp:117
void SActorMovementDispose(Actor * ptActor);  // SActorMovement.cpp:147
void SActorMovementUpdate(ACDID idACD);  // SActorMovement.cpp:165
void SActorMovementSendMoveMessage(const PlayerList & listDestPlayers, ACDID idACD);  // SActorMovement.cpp:187
void SActorMovementSendMoveMessage(ACDID idACD);  // SActorMovement.cpp:206
void UpdateRangeScriptEventsPlayer(ActorCommonData * ptAcdPlayer);  // SActorMovement.cpp:1944
