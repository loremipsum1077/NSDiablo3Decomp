// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Movement/ActorSteer.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Math/MathGeneral.h"

class ActorCommonData;
class ActorMovement;
struct WorldCollParams;
struct WorldCollResult;
struct WorldPlace;

enum {
    STEER_RESULT_NORMAL = 0,
    STEER_RESULT_FULL_STEERING = 1,
    STEER_RESULT_STOP = 2,
    STEER_RESULT_FULL_STEERING_ALLOW_180 = 3,
    STEER_RESULT_COUNT = 4,
};

typedef int32 SteerResult;

enum {
    STEER_DIRECTION_NONE = 0,
    STEER_DIRECTION_LEFT = 1,
    STEER_DIRECTION_RIGHT = 2,
    STEER_DIRECTION_AWAY = 3,
    STEER_DIRECTION_COUNT = 4,
};

typedef int32 SteerDirection;

class ActorSteer {  // size 0x40
public:
    BOOL Initialize(ActorMovement * ptMovement);
    SteerResult ModifyAccelForSteering(Vector2D & vAccel, BOOL bUseCosAngLimit);
    void Reset180Steering(const Vector2D & vNewAccel);
    void ResetStuckUnitValue() { m_flUnitStuck = 0.0f; }  // line 60
    void ModifyStuckUnitValue(float flModifyAmount) { m_flUnitStuck = Math::Pin(m_flUnitStuck + flModifyAmount, 0.0f, 1.0f); }  // line 61
    float GetStuckUnitValue() { return m_flUnitStuck; }  // line 62
    uint32 GetLookAheadTicks();
    void SetLookAheadTicks(uint32 uLookAheadTicks);
    float GetMaxSteeringDist();
    void SetMaxSteeringDist(float flNewDist);
    float GetUnitSteer();
    void SetUnitSteer(float flUnitSteer) { m_flUnitSteer = flUnitSteer; }  // line 71
    uint32 Get180SteeringHoldTicks();
    void Set180SteeringHoldTicks(uint32 uHoldTicks);
    void SetIterations(uint32 uIterations);
    BOOL HuggedThisTick();
protected:
    SteerResult SlideOBB(ACDID idACD, const WorldCollParams & tWorldCollParams, ActorCommonData & tACDHit, const WorldCollResult & tResult, const WorldPlace & placeCurr, float flRadius, const Vector2D & vDest, BOOL bUseCosAngLimit, const Vector2D & vToDest, BOOL bMoving, float flAccelMag, float flMaxSteeringDist, Vector2D & vNewAccel);
    SteerDirection SlideOBBSimple(const ActorCommonData &, const WorldPlace &, const Vector2D &, float, const Vector3D &, BOOL, Vector2D &);
    SteerResult SlideSphere(ActorCommonData & tACDHit, const WorldCollResult & tResult, const WorldPlace & placeCurr, const Vector2D & vToDest, float flAccelMag, float flMaxSteeringDist, Vector2D & vNewAccel);
    SteerResult SlideStatic(const WorldCollResult & tResult, const Vector3D & vPos, float flRadius, BOOL bMoving, uint32 uTicksToMove, BOOL bUseCosAngLimit, float flMaxSteeringDist, const Vector2D & vToDest, Vector2D & vNewAccel);
    BOOL CheckPath(ACDID, const WorldCollParams &, const Vector2D &, const Vector2D &, SWorldID, float, WorldCollResult *);
    void SetHugTarget(ACDID, SteerDirection);
    void ClearHugTarget();
    ActorCommonData * GetHugTarget();
    uint32 m_uLookAheadTicks;  // 0x00
    float m_flMaxSteeringDistance;  // 0x04
    float m_flUnitSteer;  // 0x08
    uint32 m_uNext180SteeringAllowedTime;  // 0x0c
    uint32 m_uTicksToHold180Steering;  // 0x10
    Vector2D m_vLastSteering;  // 0x14
    float m_flUnitStuck;  // 0x1c
    uint32 m_uIterations;  // 0x20
    ACDID m_idACDHugTarget;  // 0x24
    uint32 m_uHugTargetExpirationTime;  // 0x28
    uint32 m_uHugTargetLastTime;  // 0x2c
    SteerDirection m_eHugSteerDirection;  // 0x30
    ActorMovement * m_ptMovement;  // 0x38
};
static_assert(sizeof(ActorSteer) == 0x40, "ActorSteer");


// Functions defined in this file (bodies are not in the DWARF):
//   line    60  void ActorSteer::ResetStuckUnitValue()  [inlined into 1 functions]
//   line    61  void ActorSteer::ModifyStuckUnitValue(float flModifyAmount)  [inlined into 2 functions]
//   line    62  float ActorSteer::GetStuckUnitValue()  [inlined into 1 functions]
//   line    65  void ActorSteer::SetLookAheadTicks(uint32 uLookAheadTicks)  [inlined into 1 functions]
//   line    68  void ActorSteer::SetMaxSteeringDist(float flNewDist)  [inlined into 1 functions]
//   line    71  void ActorSteer::SetUnitSteer(float flUnitSteer)  [inlined into 1 functions]
//   line    74  void ActorSteer::Set180SteeringHoldTicks(uint32 uHoldTicks)  [inlined into 1 functions]
//   line    76  void ActorSteer::SetIterations(uint32 uIterations)  [inlined into 1 functions]

#line 65 "X/Common/Movement/ActorSteer.h"
inline void ActorSteer::SetLookAheadTicks(uint32 uLookAheadTicks) { m_uLookAheadTicks = uLookAheadTicks; }
#line 68 "X/Common/Movement/ActorSteer.h"
inline void ActorSteer::SetMaxSteeringDist(float flNewDist) { m_flMaxSteeringDistance = flNewDist; }
#line 74 "X/Common/Movement/ActorSteer.h"
inline void ActorSteer::Set180SteeringHoldTicks(uint32 uHoldTicks) { m_uTicksToHold180Steering = uHoldTicks; }
#line 76 "X/Common/Movement/ActorSteer.h"
inline void ActorSteer::SetIterations(uint32 uIterations) { m_uIterations = uIterations; }
