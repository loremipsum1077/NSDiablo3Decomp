// The Domino entry points the game calls, as the 2.6.2 symbols mangle them.
// The DWARF does not record which public header declared them; the file name
// here is invented. Definitions live in Dynamics/dmBody.cpp and
// Dynamics/dmFixture.cpp.

#pragma once

#include "dmCollision.h"
#include "dmId.h"
#include "dmMath.h"
#include "dmStructures.h"

bool operator!=(dmFixtureId a, dmFixtureId b);

extern const dmFixtureId dm_nullFixtureId;

dmBodyType dmBody_GetType(dmBodyId id);
void dmBody_SetType(dmBodyId id, dmBodyType type);
void dmBody_SetTransform(dmBodyId id, const dmTransform & xf);
void dmBody_SetTransformNoSync(dmBodyId id, const dmTransform & xf);
dm_vec4 dmBody_GetQuaternion(dmBodyId id);
dm_vec4 dmBody_GetPosition(dmBodyId id);
void dmBody_SetPosition(dmBodyId id, dm_vec4Arg p);
dm_vec4 dmBody_GetCenter(dmBodyId id);
void dmBody_StabilizeTree(dmBodyId id, dm_float32 inertiaFactor, dm_float32 massRatio);
dm_float32 dmBody_GetMass(dmBodyId id);
dm_vec4 dmBody_GetLocalCenter(dmBodyId id);
bool dmBody_IsSleeping(dmBodyId id);
void dmBody_SetSleeping(dmBodyId id, bool flag);
void dmBody_SetLinearVelocity(dmBodyId id, dm_vec4Arg v);
void dmBody_SetAngularVelocity(dmBodyId id, dm_vec4Arg w);
dm_vec4 dmBody_GetLinearVelocity(dmBodyId id);
dm_vec4 dmBody_GetAngularVelocity(dmBodyId id);
dm_vec4 dmBody_GetWorldPointLinearVelocity(dmBodyId id, dm_vec4Arg worldPoint);
void dmBody_ApplyForce(dmBodyId id, dm_vec4Arg force, dm_vec4Arg point, dmWake wake);
void dmBody_ApplyTorque(dmBodyId id, dm_vec4Arg torque, dmWake wake);
void dmBody_ApplyImpulse(dmBodyId id, dm_vec4Arg impulse, dm_vec4Arg point, dmWake wake);
dm_vec4 dmBody_GetClosestPoint(dmBodyId id, dm_vec4Arg point);
dm_vec4 dmBody_ComputeProjectedSurfaceArea(dmBodyId id, dm_vec4Arg direction);
dm_vec4 dmBody_ComputeSubmergedVolume(dmBodyId id, dm_vec4 * pCentroid, dm_vec4Arg normal, dm_vec4Arg offset);
dmFixtureId dmBody_GetFirstFixture(dmBodyId id);
dmFixtureId dmBody_GetNextFixture(dmFixtureId id);

dmFixtureType dmFixture_GetType(dmFixtureId id);
dmAABB dmFixture_GetAABB(dmFixtureId id);
dm_vec4 dmFixture_GetClosestPoint(dmFixtureId id, dm_vec4Arg point);
void * dmFixture_GetUserData(dmFixtureId id);
dm_vec4 dmFixture_GetProjectedArea(dmFixtureId id, dm_vec4Arg direction);

// World and joint entry points DominoClient.cpp calls (Dynamics/dmWorld.cpp, Dynamics/Joints/dmJoint.cpp).
class dmJobDispatcher;
class dmContactListener;
bool operator==(dmWorldId id1, dmWorldId id2);
bool operator==(dmJointId id1, dmJointId id2);
dmWorldId dmWorld_Create(const dmWorldDef * pDef);
void dmWorld_Destroy(dmWorldId id);
void dmWorld_SetJobDispatcher(dmWorldId id, dmJobDispatcher * pDispatcher);
void dmWorld_SetListener(dmWorldId id, dmContactListener * pListener);
void dmWorld_EnableContinuous(dmWorldId id, bool flag);
void dmWorld_Step(dmWorldId id, dm_float32 timeStep, dm_int32 velocityIterations, dm_int32 positionIterations, bool flag);
void dmWorld_Lock(dmWorldId id);
void dmWorld_Unlock(dmWorldId id);
dmJointType dmJoint_GetType(dmJointId id);
dmBodyId dmJoint_GetBodyA(dmJointId id);
dmBodyId dmJoint_GetBodyB(dmJointId id);
void dmJoint_SetMotorMode(dmJointId id, dmMotorMode mode, dmWake wake);
void dmJoint_SetMaxMotorTorque(dmJointId id, dm_float32 maxTorque, dmWake wake);
void dmRevoluteJoint_SetDesiredMotorAngle(dmJointId id, dm_vec4Arg q, dmWake wake);
void dmShoulderJoint_SetDesiredMotorOrientation(dmJointId id, dm_vec4Arg q, dmWake wake);
bool operator!=(dmJointId id1, dmJointId id2);
bool dmJoint_IsValid(dmJointId id);
void dmWorld_DestroyJoint(dmJointId id);
dmBodyId dmFixture_GetOwnerBody(dmFixtureId id);
void * dmBody_GetUserData(dmBodyId id);

// Dynamics/dmWorld.cpp and Dynamics/dmBody.cpp entry points the DominoWorld
// create, ray cast and query functions call.
template <typename T, int N> class dmBuffer;
bool dmWorld_RayCastClosest(dmWorldId id, dmWorldRayCastOutput * pOutput, const dmWorldRayCastInput * pInput);
bool operator==(dmBodyId id1, dmBodyId id2);  // Dynamics/dmBody.cpp:25
bool operator==(dmFixtureId id1, dmFixtureId id2);  // Dynamics/dmFixture.cpp:8
dmBodyId dmWorld_CreateBody(dmWorldId id, const dmBodyDef * pDef);
dmJointId dmWorld_CreateJoint(dmWorldId id, const dmJointDef * pDef);
dmFixtureId dmBody_CreateFixture(dmBodyId id, const dmFixtureDef * pDef);
dm_vec4 dmBody_ComputeSurfaceArea(dmBodyId id);
dm_vec4 dmBody_ComputeVolume(dmBodyId id);
void dmWorld_QueryAABB(dmWorldId id, dmBuffer<dmFixtureId, 32> & fixtures, dm_vec4Arg lowerBound, dm_vec4Arg upperBound, const dmQueryFilter & filter);
