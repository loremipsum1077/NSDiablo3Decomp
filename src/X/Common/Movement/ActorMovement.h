// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Movement/ActorMovement.h
// Subset: ActorMovement's virtual functions, in vtable order. The non-virtual members and the
// data members (size 0x1d8) are adopted in full.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../SNO/SNOFiles.h"
#include "../WorldPlace.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../DetPaths.h"
#include "ActorSteer.h"
#include "../Nav/NavCellHandle.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"

typedef int32 ActorAnimationTag;
typedef int32 PowerPassabilityType;  // PowersStruct.h
struct ACDCollSettings;

typedef void (*ActorMoveDoneCallback)(const ActorID);

static const uint32 MOVEFLAG_DONT_SEND_MASK = (1 << 4) | (1 << 5);  // line 312: MOVEBIT_AUTO_UPDATE_YAW, MOVEBIT_REMOTE_MOVEMENT

enum {
    MOVEMENT_TYPE_NONE = 0,
    MOVEMENT_TYPE_WALKING = 1,
    MOVEMENT_TYPE_WALKING_IN_PLACE = 2,
    MOVEMENT_TYPE_WALK_FINISHING = 3,
    MOVEMENT_TYPE_FIXED_PROJECTILE = 4,
    MOVEMENT_TYPE_FIXED_PROJECTILE_COLLIDING = 5,
    MOVEMENT_TYPE_ARC = 6,
    MOVEMENT_TYPE_CLIENT_AUTH = 7,
    MOVEMENT_TYPE_CLIENT_AUTH_FACING = 8,
    MOVEMENT_TYPE_DETPATH = 9,
    MOVEMENT_TYPE_STOPPED_PROJECTILE = 10,
    MOVEMENT_TYPE_COUNT = 11,
};

typedef int32 MovementType;

enum {
    SNAP_NONE = 0,
    SNAP_PENDING_SOFT = 1,
    SNAP_PENDING_NORMAL = 2,
    SNAP_SENT = 3,
    SNAP_COUNT = 4,
};

typedef int32 SnapState;

enum {
    DYNPATH_DIRECTION_NONE = 0,
    DYNPATH_DIRECTION_LEFT = 1,
    DYNPATH_DIRECTION_RIGHT = 2,
    DYNPATH_DIRECTION_COUNT = 3,
};

typedef int32 DynPathDirection;

enum {
    MOVEBIT_IGNORE_COLLISION = 0,
    MOVEBIT_NO_PATHFINDING = 1,
    MOVEBIT_NO_STEERING = 2,
    MOVEBIT_ARRIVING_AT_DESTINATION = 3,
    MOVEBIT_AUTO_UPDATE_YAW = 4,
    MOVEBIT_REMOTE_MOVEMENT = 5,
    MOVEBIT_NO_SNAP_TO_GROUND = 6,
    MOVEBIT_CHECK_VERTICAL = 7,
    MOVEBIT_SNAP = 8,
    MOVEBIT_FROM_POWER = 9,
    MOVEBIT_WAS_STUCK = 10,
    MOVEBIT_DONT_AUTOPLAY_IDLE = 11,
    MOVEBIT_NO_ACCEL = 12,
    MOVEBIT_ARC_UNTIL_DEST_HEIGHT = 13,
    MOVEBIT_USE_DESIRED_HEIGHT_FOR_CAMERA = 14,
    MOVEBIT_ACTIVATE_TRIGGERS = 15,
    MOVEBIT_KNOCKBACK = 16,
    MOVEBIT_USE_WALKING_SPEED = 17,
    MOVEBIT_START_WALK_AFTER_INTRO = 18,
    MOVEBIT_CLIENT_AUTH_POWER = 19,
    MOVEBIT_DYNAMIC_PATHING = 20,
    MOVEBIT_SNAP_SOFT = 21,
    MOVEBIT_IS_PROJECTILE = 22,
    MOVEBIT_USE_CHARGE_STEERING = 23,
    MOVEBIT_NAV_REFLECT = 24,
    MOVEBIT_STOP_ON_IMPACT = 25,
    MOVEBIT_COUNT = 26,
};

enum {
    PATH_RESULT_NOT_FOUND = 0,
    PATH_RESULT_NOT_NECESSARY = 1,
    PATH_RESULT_FOUND = 2,
    PATH_RESULT_COUNT = 3,
};

typedef int32 PathResult;

struct ACDTranslateNormalMessage {  // size 0x6c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    XOptional<Vector3D> vDest;  // 0x0c
    XOptional<float> aDesiredYaw;  // 0x1c
    XOptional<int> bSnapFacing;  // 0x24
    XOptional<float> flMovementSpeed;  // 0x2c
    XOptional<unsigned int> uMoveFlags;  // 0x34
    XOptional<unsigned int> eAnimTag;  // 0x3c
    XOptional<unsigned int> uWalkInPlaceTicks;  // 0x44
    XOptional<int> snoPowerPassability;  // 0x4c
    XOptional<unsigned int> uAckValue;  // 0x54
    XOptional<unsigned int> eMoveType;  // 0x5c
    XOptional<int> nMaxTicksToWalk;  // 0x64
};
static_assert(sizeof(ACDTranslateNormalMessage) == 0x6c, "ACDTranslateNormalMessage");

struct ACDTranslateSnappedMessage {  // size 0x34
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vDest;  // 0x0c
    Angle aDesiredYaw;  // 0x18
    BOOL bSnapFacing;  // 0x1c
    uint32 uMoveFlags;  // 0x20
    XOptional<unsigned int> uAckValue;  // 0x24
    XOptional<unsigned int> uCameraSmoothingTime;  // 0x2c
};
static_assert(sizeof(ACDTranslateSnappedMessage) == 0x34, "ACDTranslateSnappedMessage");

struct ACDTranslateSyncMessage {  // size 0x28
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vPos;  // 0x0c
    XOptional<int> bSnap;  // 0x18
    XOptional<unsigned int> uAckValue;  // 0x20
};
static_assert(sizeof(ACDTranslateSyncMessage) == 0x28, "ACDTranslateSyncMessage");

struct ACDTranslateAckMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uAckValue;  // 0x08
};
static_assert(sizeof(ACDTranslateAckMessage) == 0xc, "ACDTranslateAckMessage");

struct ACDTranslateFacingMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Angle aDesiredYaw;  // 0x0c
    BOOL bSnapFacing;  // 0x10
};
static_assert(sizeof(ACDTranslateFacingMessage) == 0x14, "ACDTranslateFacingMessage");

struct ACDTranslateFixedMessage {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vVel;  // 0x0c
    uint32 uMoveFlags;  // 0x18
    ActorAnimationTag eAnimTag;  // 0x1c
    SNO snoPowerPassability;  // 0x20
};
static_assert(sizeof(ACDTranslateFixedMessage) == 0x24, "ACDTranslateFixedMessage");

struct ACDTranslateArcMessage {  // size 0x40
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vPos;  // 0x0c
    Vector3D vVel;  // 0x18
    uint32 uMoveFlags;  // 0x24
    ActorAnimationTag eAnimTag;  // 0x28
    ActorAnimationTag eLandingAnimTag;  // 0x2c
    float flGravity;  // 0x30
    SNO snoPowerPassability;  // 0x34
    float flUnitBounce;  // 0x38
    float flDesiredHeight;  // 0x3c
};
static_assert(sizeof(ACDTranslateArcMessage) == 0x40, "ACDTranslateArcMessage");

struct ACDTranslateDetPathMessage {  // size 0x5c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    DeterministicPathType eDPath;  // 0x0c
    RNG tSeed;  // 0x10
    Vector3D wvVelocity;  // 0x18
    Angle aOriginalYaw;  // 0x24
    Vector3D vStartPos;  // 0x28
    uint32 uMoveFlags;  // 0x34
    ActorAnimationTag eAnimTag;  // 0x38
    SNO snoPowerPassability;  // 0x3c
    int32 nVar0;  // 0x40
    float flVar0;  // 0x44
    float flVar1;  // 0x48
    float flVar2;  // 0x4c
    float flVar3;  // 0x50
    float flVar4;  // 0x54
    float flSpeedMult;  // 0x58
};
static_assert(sizeof(ACDTranslateDetPathMessage) == 0x5c, "ACDTranslateDetPathMessage");

struct ACDTranslateDetPathSinMessage {  // size 0x68
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    DeterministicPathType eDPath;  // 0x0c
    RNG tSeed;  // 0x10
    Vector3D wvVelocity;  // 0x18
    Angle aOriginalYaw;  // 0x24
    Vector3D vStartPos;  // 0x28
    uint32 uMoveFlags;  // 0x34
    ActorAnimationTag eAnimTag;  // 0x38
    SNO snoPowerPassability;  // 0x3c
    int32 nVar0;  // 0x40
    float flVar0;  // 0x44
    DPathSinData tSinData;  // 0x48
    float flSpeedMult;  // 0x64
};
static_assert(sizeof(ACDTranslateDetPathSinMessage) == 0x68, "ACDTranslateDetPathSinMessage");

struct ACDTranslateDetPathSpiralMessage {  // size 0x58
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    DeterministicPathType eDPath;  // 0x0c
    Vector3D vStartPos;  // 0x10
    Vector3D vTargetPos;  // 0x1c
    uint32 uMoveFlags;  // 0x28
    ActorAnimationTag eAnimTag;  // 0x2c
    float flVar0;  // 0x30
    float flVar1;  // 0x34
    DPathSinData tSinData;  // 0x38
    float flSpeedMult;  // 0x54
};
static_assert(sizeof(ACDTranslateDetPathSpiralMessage) == 0x58, "ACDTranslateDetPathSpiralMessage");

struct ACDTranslateDetPathPieWedgeMessage {  // size 0x4c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vStartPos;  // 0x0c
    Vector3D vFirstTargetPos;  // 0x18
    uint32 uMoveFlags;  // 0x24
    ActorAnimationTag eAnimTag;  // 0x28
    DPathPieData tPieData;  // 0x2c
    float flCurTime;  // 0x44
    float flSpeedMult;  // 0x48
};
static_assert(sizeof(ACDTranslateDetPathPieWedgeMessage) == 0x4c, "ACDTranslateDetPathPieWedgeMessage");

struct ACDTranslateFixedUpdateMessage {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vPos;  // 0x0c
    Vector3D vVel;  // 0x18
};
static_assert(sizeof(ACDTranslateFixedUpdateMessage) == 0x24, "ACDTranslateFixedUpdateMessage");

struct ACDTranslateStoppedMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    Vector3D vPos;  // 0x0c
};
static_assert(sizeof(ACDTranslateStoppedMessage) == 0x18, "ACDTranslateStoppedMessage");

struct ACDClientTranslateMessage {  // size 0x34
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uTickBase;  // 0x08
    uint32 uTicksAhead;  // 0x0c
    Vector3D vDest;  // 0x10
    Angle aDesiredYaw;  // 0x1c
    float flMovementSpeed;  // 0x20
    uint32 uMoveFlags;  // 0x24
    uint32 eAnimTag;  // 0x28
    XOptional<int> snoPower;  // 0x2c
};
static_assert(sizeof(ACDClientTranslateMessage) == 0x34, "ACDClientTranslateMessage");

class PathPoint2DList;
struct ACDCollParams;
class CRefString;
struct ImpactData;
struct RGBAColor;
struct WorldCollParams;
struct WorldCollResult;
class ActorCommonData;
struct ACDCollSettings;

class ActorMovement {  // size 0x1d8
    // vtable pointer
public:
    virtual void Dispose();  // vtable slot 0
    virtual void MoveTo(const WorldPlace & placeDest, float flSpeed, float flAccel, ActorAnimationTag eAnimTag, uint32 uMoveFlags, float flMaxRangeFromDest, ACDID idACDTarget, SNO snoPowerPassability, uint32 dwWalkInPlaceTicks, int32 nMaxTicksToWalk, float flMaxPathfindingDist, ActorMoveDoneCallback pfnCallback);  // vtable slot 1
    virtual void MoveAtFixedVelocity(Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerPassability);  // vtable slot 2
    virtual BOOL MoveWithArc(const WorldPlace & placeDest, ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, float flRelativeHeight, SNO snoPowerPassability, float flUnitBounce);  // vtable slot 3
    virtual BOOL MoveWithArc(ActorAnimationTag eFlightAnimTag, ActorAnimationTag eLandingAnimTag, uint32 uMoveFlags, float flGravity, Vector3D & vInitialVel, SNO snoPowerPassability, float flUnitBounce, const WorldPlace * ptPlaceDest);  // vtable slot 4
    virtual void StopNow();  // vtable slot 5
    virtual void StopSoon();  // vtable slot 6
    virtual BOOL StopProjectile();  // vtable slot 7
    virtual void GetPlaceAndRadius(WorldPlace &, float &, BOOL) const = 0;  // vtable slot 8
    virtual float GetRadius(BOOL) const = 0;  // vtable slot 9
    virtual void GetPlace(WorldPlace &) const = 0;  // vtable slot 10
    virtual ACDID GetACDID() const = 0;  // vtable slot 11
    virtual SSceneID GetSSceneID() const = 0;  // vtable slot 12
    virtual BOOL IsServerProjectile() const = 0;  // vtable slot 13
    virtual void GetACDsIntersectSphere(const WorldPlace &, const WorldDistance, const ACDID, XPooledDataIDList &) const = 0;  // vtable slot 14
    virtual uint32 GetTimeNow() const = 0;  // vtable slot 15
    virtual void SetNewFacing(Angle, BOOL, BOOL) = 0;  // vtable slot 16
    virtual void PrepareToSnapToNewPlace() {}  // line 486;  // vtable slot 17
    virtual void SnapToNewPlace(const WorldPlace & placeNew, BOOL bSnapToGround, BOOL bSoftSnap, BOOL bSendToClient, uint32 uCameraSmoothingTicks);  // vtable slot 18
    virtual void SnapPlaceToGround(WorldPlace &) const = 0;  // vtable slot 19
    virtual BOOL PathBlockedByObject(ACDID, const Vector2D &) { return FALSE; }  // line 530;  // vtable slot 20
    virtual CRefString DebugGetDumpString() = 0;  // vtable slot 21
    virtual void ActivateTriggers(const Vector2D &, const Vector2D &) const {}  // line 535;  // vtable slot 22
    virtual void ActivateTriggersToDest(const WorldPlace &) {}  // line 539;  // vtable slot 23
    virtual void ActivateTriggersAtLocation(BOOL) {}  // line 542;  // vtable slot 24
    virtual void ForceBrainUpdate() {}  // line 551;  // vtable slot 25
    virtual SNO GetActorSNO() const = 0;  // vtable slot 26
    virtual BOOL RaycastAgainstWorld(const WorldPlace &, const WorldPlace &, Vector3D &, Vector3D &, float &, uint16) const = 0;  // vtable slot 27
    virtual void PlayPreplayedAnim(ActorAnimationTag, float) = 0;  // vtable slot 28
    virtual void ResetCache() = 0;  // vtable slot 29
    virtual void PostMessage() = 0;  // vtable slot 30
    virtual void Bounced() = 0;  // vtable slot 31
    virtual void ProjToadIdleResponse() = 0;  // vtable slot 32
    virtual void SetNewPlace(const WorldPlace &, BOOL, BOOL, BOOL) = 0;  // vtable slot 33
    virtual void QueueUpdate(uint32) = 0;  // vtable slot 34
    virtual void ArrivingAtDest(uint32, BOOL, BOOL, ACDID) = 0;  // vtable slot 35
    virtual void Stuck() = 0;  // vtable slot 36
    virtual uint32 GetEarliestUpdateTickOffset() = 0;  // vtable slot 37
    virtual ACDID PerformProjectileCollision(uint32 &) const = 0;  // vtable slot 38
    virtual void UpdateWalking();  // vtable slot 39
    virtual void UpdateWalkingRemote();  // vtable slot 40
    virtual void UpdateWalkingInPlace();  // vtable slot 41
    virtual void UpdateStoppedProjectile();  // vtable slot 42
    virtual void UpdateFixed();  // vtable slot 43
    virtual void UpdateArc();  // vtable slot 44
    virtual void UpdateFacing() {}  // vtable slot 45; defined in the class (DWARF: size 4, no decl line of its own)
    virtual void UpdateClientFacing() {}  // line 707;  // vtable slot 46
    virtual void UpdateDetPath();  // vtable slot 47
    virtual BOOL ProjCollisionResponse(ImpactData &) = 0;  // vtable slot 48
    virtual void ProjNavReflectResponse() = 0;  // vtable slot 49
    virtual void OnSceneChanged(SSceneID idSSceneOld, SSceneID idSSceneNew);  // vtable slot 50
    virtual void ForceSendUpdate() = 0;  // vtable slot 51

    ~ActorMovement();
    BOOL Initialize(const WorldPlace & placeStart);
    BOOL Update();
    void StopWalk();
    void FaceTarget(const Vector3D & vTarget, BOOL bSnap);
    void PositionChanged(const WorldPlace & placeCurr, SSceneID idSScene);
    BOOL PerformCollision(const Vector3D & vToDest, WorldCollResult & tResult) const;
    ACDID PerformCollision(uint32 & uTicksToTravel, WorldCollResult * tOutResult) const;
    BOOL PerformCollisionAgainstPhysicsMesh(uint32 & uTicksToTravel, ImpactData & tImpactData) const;
    Vector3D Reflect(const Vector3D & vVel, const Vector3D & vNormal);
    void GetCurrentInterpolatedPos(Vector3D & vPos) const;
    void UpdateDetPathPlace(uint32 uTravelTicks);
    BOOL ProjCollisionResponse(ACDID idACDObject);
    BOOL ShouldIgnoreStuckCollision() const;
    void StartDetPath(DeterministicPathType eDPathType, const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerPassability, Angle aOriginalYaw, const Vector3D & vStartPos, const RNG & tSeed, int32 nVar0, float flVar0, float flVar1, float flVar2, float flVar3, float flVar4, float flSpeedMult);
    void StartDetPathSinWave(DeterministicPathType eDPathType, const Vector3D & vVel, uint32 uMoveFlags, ActorAnimationTag eAnimTag, SNO snoPowerPassability, Angle aOriginalYaw, const Vector3D & vStartPos, const RNG & tSeed, int32 nVar0, float flVar0, const DPathSinData & tSinData, float flSpeedMult);
    void StartDetPathSpiral(DeterministicPathType eDPathType, const Vector3D & vStartPos, const Vector3D & vTargetPos, uint32 uMoveFlags, ActorAnimationTag eAnimTag, float flVar0, float flVar1, const DPathSinData & tSinData, float flSpeedMult);
    void StartDetPathPieWedge(const Vector3D & vStartPos, const Vector3D & vFirstTargetPos, uint32 uMoveFlags, ActorAnimationTag eAnimTag, const DPathPieData & tPieData, float flStartRunTime, float flSpeedMult);
    Vector3D GetVelocity() const;
    void ComputeTravelThisIteration(uint32 uTicks, Vector3D & vResult) const;
    BOOL CollideAgainstHeight(uint32 & uTicksToTravel) const;
    BOOL TargetWillBeReached(const Vector3D & vToCurrentWalkTarget, uint32 & uTicksToTravel, Vector3D * pvArrivalPos) const;
    static float ComputeFallingTicksToHitTarget(float flGravity, float flInitialSpeed, float flDesiredPositionOffset);
    uint32 GetUpdateInterval() const { return IsUsingChargeSteering() ? Math::Min(m_uUpdateInterval, (uint32)3) : m_uUpdateInterval; }  // line 392
    void SetUpdateInterval(uint32 uUpdateInterval) { m_uUpdateInterval = uUpdateInterval; }  // line 393
    Vector3D GetBaseVelocity() const { return m_vVel; }  // line 403
    ACDID GetTargetACD() const { return m_idACDTarget; }  // line 401
    float GetAccelMag() const { return m_flAccelMag; }  // line 390
    float GetDesiredSpeed() const { return m_flDesiredSpeed; }  // line 405
    const Vector3D & GetCurrentDestinationPos() const { return m_vCurrPathDest; }  // line 409
    const NavCellHandle & GetNavCell() const { return m_hNavCell; }  // line 431
    ActorSteer & GetSteer() { return m_tSteer; }  // line 433
    void SetIgnoreCollision(BOOL bIgnore) {  // line 436
        if (bIgnore)
            m_uMoveFlags |= (1 << MOVEBIT_IGNORE_COLLISION);
        else
            m_uMoveFlags &= ~(1 << MOVEBIT_IGNORE_COLLISION);
    }
    MovementType GetMovementType() { return m_eMovementType; }  // line 590
    void SetMinDistToWalk(float flMinDist);  // Original declaration: ActorMovement.h:399.
    BOOL IsMoving() const;  // line 308
    BOOL HasFailedDynamicPath();  // ActorMovement.cpp
    PathResult FindPath(const WorldPlace & placeTarget, PathPoint2DList & listPathPoints, uint16 uStaticOneRequiredFlags, uint16 uStaticPreventFlags, ACDCollParams * ptACDCollParams) const;  // ActorMovement.cpp
    void GetCollSettings(ACDCollSettings & tCollSettings) const;  // ActorMovement.cpp
    void GetCollSettings(SNO snoPower, PowerPassabilityType ePowerPassabilityType, ACDCollSettings & tCollSettings) const;  // line 355
    SNO GetPowerPassability() const { return m_snoPowerPassability; }  // line 453
    BOOL IsRemoteMovement() const { return m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT); }  // line 447
    BOOL IsArrivingAtFinalDest() const { return m_uMoveFlags & (1 << MOVEBIT_ARRIVING_AT_DESTINATION); }  // line 448
    BOOL ChecksVertical() const { return m_uMoveFlags & (1 << MOVEBIT_CHECK_VERTICAL); }  // line 449
    BOOL IsInArcingMovement() const { return m_eMovementType == MOVEMENT_TYPE_ARC; }  // line 454
    BOOL IsUsingChargeSteering() const { return m_uMoveFlags & (1 << MOVEBIT_USE_CHARGE_STEERING); }  // line 456
    void SetStopOnImpact(BOOL bStopOnImpact);  // line 445
    PathResult FindDynamicPath(const WorldPlace & placeStart, float flRadius, const Vector2D & vDest, Vector2D & vNewDest, const DynPathDirection eDirectionLast, DynPathDirection & eDirectionChosen, const ACDCollSettings & tACDCollSettings) const;
    BOOL CheckLineForACDBlockers(const Vector2D & vStart, const Vector2D & vDest, SWorldID idSWorld, float flRadius, ACDCollParams & tACDCollParams) const;
    BOOL CheckCylinderTeleportBlocker(const Vector2D & vStart, const Vector2D & vDest, float flRadius, const ActorCommonData & tACDBlocker) const;
    BOOL CheckDynPath(const WorldCollParams & tWorldCollParams, const Vector2D & vStart, const Vector2D & vEnd, SWorldID idSWorld, float flRadius, ACDID idACDIgnore, WorldCollResult & tResult) const;
    BOOL FindDynPathInDirection(const WorldPlace & placeOrig, float flRadius, const Vector2D & vFinalDest, float flMaxDist, DynPathDirection eDirection, const ACDCollSettings & tACDCollSettings, Vector2D & vFirstNode, float & flDistUsed) const;
    void FindNextSteerPointOBB(const ActorCommonData & tACDOBB, const Vector2D & vImpactPos, float flRadius, const Vector3D & vCollNormal, DynPathDirection eDirection, Vector2D & vResult) const;
    void FindNextSteerPointCylinder(const ActorCommonData & tACDCyl, const Vector2D & vImpactPos, float flRadius, DynPathDirection eDirection, Vector2D & vResult) const;
    void FindNextSteerPointStatic(const Vector2D & vImpactPos, const Vector2D & vImpactNormal, DynPathDirection eDirection, Vector2D & vResult) const;

    // Data members.
    uint32 m_uUpdateInterval;  // 0x08
    float m_flDesiredSpeed;  // 0x0c
    float m_flCurrentSpeed;  // 0x10
    float m_flAccelMag;  // 0x14
    float m_flMaxRangeFromDest;  // 0x18
    float m_flUnitBounce;  // 0x1c
    float m_flRadiusScalar;  // 0x20
    ActorAnimationTag m_eAnimTag;  // 0x24
    ActorAnimationTag m_eLandingAnimTag;  // 0x28
    uint32 m_uMoveFlags;  // 0x2c
    ACDID m_idACDTarget;  // 0x30
    SNO m_snoPowerPassability;  // 0x34
    MovementType m_eMovementType;  // 0x38
    DynPathDirection m_eLastDynPathDirection;  // 0x3c
    BOOL m_bFailedToFindDynamicPath;  // 0x40
    WorldPlace m_lastFindSource;  // 0x44
    WorldPlace m_lastFindTarget;  // 0x54
    uint32 m_uLastFindTime;  // 0x64
    WorldPlace m_placeStart;  // 0x68
    Vector3D m_vInitialVelocity;  // 0x78
    WorldPlace m_placeDest;  // 0x84
    Vector3D m_vCurrPathDest;  // 0x94
    Vector3D m_vCurrSteerVel;  // 0xa0
    int32 m_nCurrCell;  // 0xac
    Vector3D m_vVel;  // 0xb0
    Vector3D m_vLastVel;  // 0xbc
    float m_flGravity;  // 0xc8
    float m_flDesiredHeight;  // 0xcc
    uint32 m_uNextUpdateTick;  // 0xd0
    uint32 m_uMinTicksToAllowTravel;  // 0xd4
    NavCellHandle m_hNavCell;  // 0xd8
    SnapState m_eSnapState;  // 0xe0
    float m_flMinDistToWalk;  // 0xe4
    float m_flMaxPathfindingDist;  // 0xe8
    WorldPlace m_placeLastStamp;  // 0xec
    uint32 m_uLastStampTime;  // 0xfc
    float m_flLastStampSpeed;  // 0x100
    uint32 m_uMinDurationEndStampTime;  // 0x104
    BOOL m_bWaitAtDestination;  // 0x108
    int32 m_nMaxTicksToWalk;  // 0x10c
    ActorMoveDoneCallback m_pfnCallback;  // 0x110
    DeterministicPathData m_tDPath;  // 0x118
    ActorSteer m_tSteer;  // 0x190
    uint32 m_uPathfindThrottleTicks;  // 0x1d0
    float m_flPathfindThrottleRadius;  // 0x1d4
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(ActorMovement) == 0x1d8, "ActorMovement");

// ActorMovement.h:445. Bit 25 is MOVEBIT_STOP_ON_IMPACT (ActorMovement.h:285, enum not adopted yet).
inline void ActorMovement::SetStopOnImpact(BOOL bStopOnImpact)
{
    if (bStopOnImpact) {
        m_uMoveFlags |= (1 << 25);
    } else {
        m_uMoveFlags &= ~(1 << 25);
    }
}

#line 399 "X/Common/Movement/ActorMovement.h"
inline void ActorMovement::SetMinDistToWalk(float flMinDist) { m_flMinDistToWalk = flMinDist; }
