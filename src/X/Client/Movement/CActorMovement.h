// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Movement/CActorMovement.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"

class CRefString;
struct ImpactData;
struct WorldPlace;

typedef int32 TiltType;

class CActorMovement : public ActorMovement {  // size 0x268
public:
    CActorMovement() {}  // line 32
    ~CActorMovement();
    BOOL Initialize(RActorID idRActor, const WorldPlace & placeStart);
    virtual void Dispose();  // vtable slot 0
    virtual void GetPlaceAndRadius(WorldPlace & tPlace, float & flRadius, BOOL bGetMovingRadius) const;  // vtable slot 8
    virtual float GetRadius(BOOL bGetMovingRadius) const;  // vtable slot 9
    virtual void GetPlace(WorldPlace & tPlace) const;  // vtable slot 10
    virtual SSceneID GetSSceneID() const;  // vtable slot 12
    virtual ACDID GetACDID() const;  // vtable slot 11
    virtual BOOL IsServerProjectile() const { return FALSE; }  // vtable slot 13, line 55
    virtual void MoveTo(const WorldPlace & placeDest, float flSpeed, float flAccel, ActorAnimationTag eAnimTag, uint32 uMoveFlags, float flMaxRangeFromDest, ACDID idACDTarget, SNO snoPowerPassability, uint32 dwWalkInPlaceTicks, int32 nMaxTicksToWalk, float flMaxPathfindingDist, ActorMoveDoneCallback pfnCallback);  // vtable slot 1
    virtual void UpdateFacing();  // vtable slot 45
    void Interpolate();
    virtual void SetNewFacing(Angle aFacing, BOOL bSnap, BOOL bSendUpdate);  // vtable slot 16
    void ProcessTranslateMessage(ACDTranslateNormalMessage & tMessage);
    void ProcessSnappedMessage(ACDTranslateSnappedMessage & tMessage);
    void ProcessFacingMessage(ACDTranslateFacingMessage & tMessage);
    void ProcessTranslateFixedMessage(ACDTranslateFixedMessage & tMessage);
    void ProcessFixedUpdateMessage(ACDTranslateFixedUpdateMessage & tMessage);
    void ProcessTranslateArcMessage(ACDTranslateArcMessage & tMessage);
    void ProcessTranslateDetPathMessage(ACDTranslateDetPathMessage & tMessage);
    void ProcessTranslateDetPathSinMessage(ACDTranslateDetPathSinMessage & tMessage);
    void ProcessTranslateDetPathSpiralMessage(ACDTranslateDetPathSpiralMessage & tMessage);
    void ProcessTranslateDetPathPieWedgeMessage(ACDTranslateDetPathPieWedgeMessage & tMessage);
    void ProcessTranslateStoppedMessage(ACDTranslateStoppedMessage & tMessage);
    virtual void GetACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs) const;  // vtable slot 14
    virtual void StopNow();  // vtable slot 5
    virtual uint32 GetTimeNow() const;  // vtable slot 15
    virtual void SnapPlaceToGround(WorldPlace & placeCurrent) const;  // vtable slot 19
    void SetTilt(TiltType nTiltType, BOOL bSnap);
    void SyncToPosition(const Vector3D & vPos, const BOOL bSnap);
    virtual SNO GetActorSNO() const;  // vtable slot 26
    virtual void ResetCache() {}  // vtable slot 29, line 147
    virtual void PostMessage() {}  // vtable slot 30, line 150
    virtual void Bounced();  // vtable slot 31
    virtual void ProjToadIdleResponse();  // vtable slot 32
    void SendFacingToServer(BOOL bSnapFacing);
    BOOL GetCameraTargetPos(Vector3D & vPos);
    void SendAckResponse(uint32 uAckValue);
    Angle GetDesiredYaw() { return m_aDesiredYaw; }  // line 173
protected:
    virtual void SetNewPlace(const WorldPlace & placeNew, BOOL bSnap, BOOL bClientAuth, BOOL bSendToClient);  // vtable slot 33
    virtual void QueueUpdate(uint32 uDesiredTickDelay);  // vtable slot 34
    void SendPositionToServer(uint32 uTicksToCheck);
    virtual void ArrivingAtDest(uint32 uTicksAway, BOOL, BOOL, ACDID);  // vtable slot 35
    void PlayIdle();
    virtual void Stuck();  // vtable slot 36
    virtual uint32 GetEarliestUpdateTickOffset() { return 0; }  // vtable slot 37, line 202
    void PlayAnimClientOnly(uint32 dwAnimTag);
    virtual void PlayPreplayedAnim(ActorAnimationTag eAnimTag, float flSpeedScalar);  // vtable slot 28
    virtual void UpdateWalking();  // vtable slot 39
    virtual void UpdateWalkingRemote();  // vtable slot 40
    virtual void UpdateWalkingInPlace();  // vtable slot 41
    virtual void UpdateDetPath();  // vtable slot 47
    void UpdateAnim(BOOL bReturnIfMissingIntro);
    void UpdateAnimBlend();
    virtual void UpdateFixed();  // vtable slot 43
    virtual void UpdateArc();  // vtable slot 44
    virtual BOOL ProjCollisionResponse(ImpactData &);  // vtable slot 48
    virtual void ProjNavReflectResponse();  // vtable slot 49
    virtual BOOL RaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd, Vector3D & vIntersect, Vector3D & vNormal, float & flUnitTraveled, uint16 uExcludeMask) const;  // vtable slot 27
    void PlayCurrentAnim(float flUnitStartTime, AnimReason eAnimReason);
    virtual ACDID PerformProjectileCollision(uint32 & uTicksToTravel) const;  // vtable slot 38
    virtual void ForceSendUpdate() {}  // vtable slot 51, line 256
    virtual CRefString DebugGetDumpString();  // vtable slot 21
    RActorID m_idRActor;  // 0x1d8
    uint32 m_uNextClientUpdateTime;  // 0x1dc
    uint32 m_uNextPositionSendTime;  // 0x1e0
    uint32 m_uMoveStopTime;  // 0x1e4
    uint32 m_uNextBounceTime;  // 0x1e8
    Angle m_aDesiredYaw;  // 0x1ec
    Angle m_aYawVelocity;  // 0x1f0
    BOOL m_bTurn180;  // 0x1f4
    BOOL m_bTurn180Trigger;  // 0x1f8
    float m_flTurn180AnimScale;  // 0x1fc
    float m_flCurrentCamHeight;  // 0x200
    uint32 m_uCamStartTime;  // 0x204
    uint32 m_uCamEndTime;  // 0x208
    Vector3D m_vCamStartPos;  // 0x20c
    Vector3D m_vOldPlace;  // 0x218
    Angle m_aOldYaw;  // 0x224
    Vector3D m_vNewPlace;  // 0x228
    Angle m_aNewYaw;  // 0x234
    float m_flNewPlaceTime;  // 0x238
    float m_flNewYawTime;  // 0x23c
    Vector3D m_vCachedDest;  // 0x240
    WorldDistance m_flCachedMovementSpeed;  // 0x24c
    Angle m_aCachedDesiredYaw;  // 0x250
    uint32 m_uCachedMoveFlags;  // 0x254
    uint32 m_uCachedAnimTag;  // 0x258
    Vector3D m_vCachedPositionSent;  // 0x25c
};
static_assert(sizeof(CActorMovement) == 0x268, "CActorMovement");


// Functions defined in this file (bodies are not in the DWARF):
//   line    32  CActorMovement::CActorMovement()  [inlined into 1 functions]
//   line    55  BOOL CActorMovement::IsServerProjectile() const  [0x71001e9eb0]
//   line   147  void CActorMovement::ResetCache()  [0x71001e9f20]
//   line   150  void CActorMovement::PostMessage()  [0x71001e9f30]
//   line   173  Angle CActorMovement::GetDesiredYaw()  [inlined into 1 functions]
//   line   202  uint32 CActorMovement::GetEarliestUpdateTickOffset()  [0x71001e9f40]
//   line   256  void CActorMovement::ForceSendUpdate()  [0x71001e9f60]
