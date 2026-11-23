// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmStructures.h

#pragma once

#include "dmCollision.h"
#include "dmId.h"
#include "dmMath.h"
#include "dmSerialize.h"

class dmDebugDraw;
struct dmHeightData;
struct dmMeshData;
struct dmPolytopeData;
class dmShape;

enum dmBodyType {
    dm_unknownBody = -1,
    dm_dynamicBody = 0,
    dm_kinematicBody = 1,
    dm_staticBody = 2,
};

enum dmOwnerType {
    dm_unknownOwner = 0,
    dm_bodyOwner = 1,
    dm_phantomOwner = 2,
};

enum dmJointType {
    dm_unknownJoint = 0,
    dm_distanceJoint = 1,
    dm_mouseJoint = 2,
    dm_prismaticJoint = 3,
    dm_revoluteJoint = 4,
    dm_shoulderJoint = 5,
    dm_sphericalJoint = 6,
    dm_weldJoint = 7,
};

enum dmMotorMode {
    dm_motorOffMode = 0,
    dm_motorPositionMode = 1,
    dm_motorVelocityMode = 2,
};

enum dmLimitState {
    dm_limitOffState = 0,
    dm_limitInactiveState = 1,
    dm_limitLowerState = 2,
    dm_limitUpperState = 3,
    dm_limitEqualState = 4,
};

enum dmWake {
    dm_wakeIfChanged = 0,
    dm_leaveAsleep = 1,
};

struct dmFilter {  // size 0x8
    dmFilter();
    dm_uint16 m_categoryBits;  // 0x00
    dm_uint16 m_includeMask;  // 0x02
    dm_int32 m_groupIndex;  // 0x04
};
static_assert(sizeof(dmFilter) == 0x8, "dmFilter");

struct dmWorldDef {  // size 0x50
    dmWorldDef() {}  // no out-of-line copy in the binary; SWorld::SWorld sets every member
    dm_vec4 m_gravity;  // 0x00
    dm_int32 m_bodyCapacity;  // 0x10
    dm_int32 m_phantomCapacity;  // 0x14
    dm_int32 m_fixtureCapacity;  // 0x18
    dm_int32 m_convexContactCapacity;  // 0x1c
    dm_int32 m_meshContactCapacity;  // 0x20
    dm_int32 m_distanceJointCapacity;  // 0x24
    dm_int32 m_mouseJointCapacity;  // 0x28
    dm_int32 m_prismaticJointCapacity;  // 0x2c
    dm_int32 m_revoluteJointCapacity;  // 0x30
    dm_int32 m_shoulderJointCapacity;  // 0x34
    dm_int32 m_sphericalJointCapacity;  // 0x38
    dm_int32 m_weldJointCapacity;  // 0x3c
    dm_int32 m_stackCapacity;  // 0x40
};
static_assert(sizeof(dmWorldDef) == 0x50, "dmWorldDef");

struct dmBodyDef {  // size 0x50
    dmBodyDef();
    dm_vec4 m_position;  // 0x00
    dm_vec4 m_quaternion;  // 0x10
    dmBodyType m_bodyType;  // 0x20
    dm_float32 m_linearDamping;  // 0x24
    dm_float32 m_angularDamping;  // 0x28
    dm_float32 m_inertiaScale;  // 0x2c
    dm_float32 m_gravityScale;  // 0x30
    bool m_enableSleeping;  // 0x34
    bool m_isSleeping;  // 0x35
    bool m_isBullet;  // 0x36
    bool m_isActive;  // 0x37
    const char * m_pName;  // 0x38
    void * m_pUserData;  // 0x40
};
static_assert(sizeof(dmBodyDef) == 0x50, "dmBodyDef");

struct dmPhantomDef {  // size 0x40
    dmPhantomDef();
    void * m_pUserData;  // 0x00
    dm_vec4 m_position;  // 0x10
    dm_vec4 m_quaternion;  // 0x20
    bool m_isActive;  // 0x30
    const char * m_pName;  // 0x38
};
static_assert(sizeof(dmPhantomDef) == 0x40, "dmPhantomDef");

enum dmFixtureType {
    dm_unknownFixture = -1,
    dm_sphereFixture = 0,
    dm_capsuleFixture = 1,
    dm_polytopeFixture = 2,
    dm_treeMeshFixture = 3,
    dm_heightFieldFixture = 4,
};

struct dmFixtureDef {  // size 0x28
    dmFixtureDef();
    void * m_pUserData;  // 0x00
    dm_float32 m_friction;  // 0x08
    dm_float32 m_restitution;  // 0x0c
    dm_float32 m_rollingResistance;  // 0x10
    dm_float32 m_density;  // 0x14
    dmFilter m_filter;  // 0x18
    bool m_isSensor;  // 0x20
    dmFixtureType m_type;  // 0x24
};
static_assert(sizeof(dmFixtureDef) == 0x28, "dmFixtureDef");

struct dmSphereDef : public dmFixtureDef {  // size 0x50
    dmSphereDef();
    dm_vec4 m_localPosition;  // 0x30
    dm_float32 m_radius;  // 0x40
};
static_assert(sizeof(dmSphereDef) == 0x50, "dmSphereDef");

struct dmCapsuleDef : public dmFixtureDef {  // size 0x60
    dmCapsuleDef();
    dm_vec4 m_p1;  // 0x30
    dm_vec4 m_p2;  // 0x40
    dm_float32 m_radius;  // 0x50
};
static_assert(sizeof(dmCapsuleDef) == 0x60, "dmCapsuleDef");

struct dmPolytopeDef : public dmFixtureDef {  // size 0x60
    dmPolytopeDef();
    bool IsValid() const;
    const dmPolytopeData * m_pPolytope;  // 0x28
    dmTransform m_transform;  // 0x30
    dm_float32 m_scale;  // 0x50
};
static_assert(sizeof(dmPolytopeDef) == 0x60, "dmPolytopeDef");

struct dmTreeMeshDef : public dmFixtureDef {  // size 0x50
    dmTreeMeshDef();
    dm_vec4 m_scale;  // 0x30
    const dmMeshData * m_pData;  // 0x40
};
static_assert(sizeof(dmTreeMeshDef) == 0x50, "dmTreeMeshDef");

struct dmHeightFieldDef : public dmFixtureDef {  // size 0x30
    dmHeightFieldDef();
    const dmHeightData * m_pData;  // 0x28
};
static_assert(sizeof(dmHeightFieldDef) == 0x30, "dmHeightFieldDef");

struct dmJointDef {  // size 0x28
    dmJointDef();
    dmJointType m_type;  // 0x00
    dmBodyId m_bodyIdA;  // 0x04
    dmBodyId m_bodyIdB;  // 0x0c
    void * m_pUserData;  // 0x18
    bool m_collideConnected;  // 0x20
};
static_assert(sizeof(dmJointDef) == 0x28, "dmJointDef");

struct dmDistanceJointDef : public dmJointDef {  // size 0x60
    dmDistanceJointDef();
    dm_vec4 m_localAnchorA;  // 0x30
    dm_vec4 m_localAnchorB;  // 0x40
    dm_float32 m_distance;  // 0x50
};
static_assert(sizeof(dmDistanceJointDef) == 0x60, "dmDistanceJointDef");

struct dmMouseJointDef : public dmJointDef {  // size 0x60
    dmMouseJointDef();
    dm_vec4 m_target;  // 0x30
    dm_vec4 m_localAnchor;  // 0x40
    dm_float32 m_maxForce;  // 0x50
    dm_float32 m_frequencyHz;  // 0x54
    dm_float32 m_dampingRatio;  // 0x58
};
static_assert(sizeof(dmMouseJointDef) == 0x60, "dmMouseJointDef");

struct dmPrismaticJointDef : public dmJointDef {  // size 0x90
    dmPrismaticJointDef();
    dmTransform m_localFrameA;  // 0x30
    dmTransform m_localFrameB;  // 0x50
    dm_float32 m_lowerLimit;  // 0x70
    dm_float32 m_upperLimit;  // 0x74
    bool m_enableLimit;  // 0x78
    dmMotorMode m_motorMode;  // 0x7c
    dm_float32 m_maxMotorForce;  // 0x80
};
static_assert(sizeof(dmPrismaticJointDef) == 0x90, "dmPrismaticJointDef");

struct dmRevoluteJointDef : public dmJointDef {  // size 0x90
    dmRevoluteJointDef();
    dmTransform m_localFrameA;  // 0x30
    dmTransform m_localFrameB;  // 0x50
    dm_float32 m_lowerAngle;  // 0x70
    dm_float32 m_upperAngle;  // 0x74
    dm_float32 m_maxMotorTorque;  // 0x78
    dmMotorMode m_motorMode;  // 0x7c
    bool m_enableLimit;  // 0x80
};
static_assert(sizeof(dmRevoluteJointDef) == 0x90, "dmRevoluteJointDef");

struct dmShoulderJointDef : public dmJointDef {  // size 0x90
    dmShoulderJointDef();
    dmTransform m_localFrameA;  // 0x30
    dmTransform m_localFrameB;  // 0x50
    dm_float32 m_lowerTwistAngle;  // 0x70
    dm_float32 m_upperTwistAngle;  // 0x74
    dm_float32 m_coneAngle;  // 0x78
    dm_float32 m_maxMotorTorque;  // 0x7c
    dmMotorMode m_motorMode;  // 0x80
    bool m_enableTwistLimit;  // 0x84
};
static_assert(sizeof(dmShoulderJointDef) == 0x90, "dmShoulderJointDef");

struct dmSphericalJointDef : public dmJointDef {  // size 0x60
    dmSphericalJointDef();
    dm_vec4 m_localAnchorA;  // 0x30
    dm_vec4 m_localAnchorB;  // 0x40
    dm_float32 m_frictionTorque;  // 0x50
};
static_assert(sizeof(dmSphericalJointDef) == 0x60, "dmSphericalJointDef");

struct dmWeldJointDef : public dmJointDef {  // size 0x80
    dmWeldJointDef();
    dmTransform m_localFrameA;  // 0x30
    dmTransform m_localFrameB;  // 0x50
    dm_float32 m_angularFrequencyHz;  // 0x70
    dm_float32 m_linearFrequencyHz;  // 0x74
    dm_float32 m_angularDampingRatio;  // 0x78
    dm_float32 m_linearDampingRatio;  // 0x7c
};
static_assert(sizeof(dmWeldJointDef) == 0x80, "dmWeldJointDef");

struct dmQueryFilter {  // size 0x4
    dmQueryFilter();
    dm_uint16 m_includeMask;  // 0x00
    dm_uint16 m_excludeMask;  // 0x02
};
static_assert(sizeof(dmQueryFilter) == 0x4, "dmQueryFilter");

struct dmWorldRayCastInput {  // size 0x30
    dmWorldRayCastInput();
    dm_vec4 p1;  // 0x00
    dm_vec4 p2;  // 0x10
    dmQueryFilter filter;  // 0x20
    dm_uint16 triangleMask;  // 0x24
    void * pUserData;  // 0x28
};
static_assert(sizeof(dmWorldRayCastInput) == 0x30, "dmWorldRayCastInput");

struct dmWorldRayCastOutput {  // size 0x40
    dm_vec4 p;  // 0x00
    dm_vec4 normal;  // 0x10
    dm_float32 fraction;  // 0x20
    dm_int32 triangleIndex;  // 0x24
    dm_uint16 materialID;  // 0x28
    dmFixtureId fixtureId;  // 0x2c
};
static_assert(sizeof(dmWorldRayCastOutput) == 0x40, "dmWorldRayCastOutput");

struct dmWorldShapeCastInput {  // size 0x50
    const dmShape * pShape;  // 0x00
    dmTransform transform;  // 0x10
    dm_vec4 translation;  // 0x30
    dm_float32 tolerance;  // 0x40
    dmQueryFilter filter;  // 0x44
    void * pUserData;  // 0x48
};
static_assert(sizeof(dmWorldShapeCastInput) == 0x50, "dmWorldShapeCastInput");

struct dmProfile {  // size 0x54
    dm_float32 step;  // 0x00
    dm_float32 aveStep;  // 0x04
    dm_float32 maxStep;  // 0x08
    dm_float32 collide;  // 0x0c
    dm_float32 maxCollide;  // 0x10
    dm_float32 solve;  // 0x14
    dm_float32 maxSolve;  // 0x18
    dm_float32 toi;  // 0x1c
    dm_float32 maxTOI;  // 0x20
    dm_float32 report;  // 0x24
    dm_float32 broadPhase;  // 0x28
    dm_float32 setupContraints;  // 0x2c
    dm_float32 velocityConstraints;  // 0x30
    dm_float32 positionConstraints;  // 0x34
    dm_int32 dynamicCount;  // 0x38
    dm_int32 kinematicCount;  // 0x3c
    dm_int32 staticCount;  // 0x40
    dm_int32 awakeCount;  // 0x44
    dm_int32 proxyCount;  // 0x48
    dm_int32 treeHeight;  // 0x4c
    dm_float32 areaRatio;  // 0x50
};
static_assert(sizeof(dmProfile) == 0x54, "dmProfile");

struct dmCounters {  // size 0xd0
    dm_int32 bodyCount;  // 0x00
    dm_int32 maxBodyCount;  // 0x04
    dm_int32 bodyCapacity;  // 0x08
    dm_int32 phantomCount;  // 0x0c
    dm_int32 phantomCapacity;  // 0x10
    dm_int32 maxPhantomCount;  // 0x14
    dm_int32 fixtureCount;  // 0x18
    dm_int32 fixtureCapacity;  // 0x1c
    dm_int32 maxFixtureCount;  // 0x20
    dm_int32 convexContactCount;  // 0x24
    dm_int32 maxConvexContactCount;  // 0x28
    dm_int32 meshContactCount;  // 0x2c
    dm_int32 maxMeshContactCount;  // 0x30
    dm_int32 distanceJointCount;  // 0x34
    dm_int32 distanceJointCapacity;  // 0x38
    dm_int32 maxDistanceJointCount;  // 0x3c
    dm_int32 mouseJointCount;  // 0x40
    dm_int32 mouseJointCapacity;  // 0x44
    dm_int32 maxMouseJointCount;  // 0x48
    dm_int32 prismaticJointCount;  // 0x4c
    dm_int32 prismaticJointCapacity;  // 0x50
    dm_int32 maxPrismaticJointCount;  // 0x54
    dm_int32 revoluteJointCount;  // 0x58
    dm_int32 revoluteJointCapacity;  // 0x5c
    dm_int32 maxRevoluteJointCount;  // 0x60
    dm_int32 shoulderJointCount;  // 0x64
    dm_int32 shoulderJointCapacity;  // 0x68
    dm_int32 maxShoulderJointCount;  // 0x6c
    dm_int32 sphericalJointCount;  // 0x70
    dm_int32 sphericalJointCapacity;  // 0x74
    dm_int32 maxSphericalJointCount;  // 0x78
    dm_int32 weldJointCount;  // 0x7c
    dm_int32 weldJointCapacity;  // 0x80
    dm_int32 maxWeldJointCount;  // 0x84
    dm_int32 manifoldCount;  // 0x88
    dm_int32 polytopeCount;  // 0x8c
    dm_int32 polytopeFaceCount;  // 0x90
    dm_int32 treeMeshCount;  // 0x94
    dm_int32 treeMeshTriangleCount;  // 0x98
    dm_int32 maxTriangleCacheCount;  // 0x9c
    dm_int32 maxStackAllocation;  // 0xa0
    dm_int32 maxJobCount;  // 0xa4
    dm_int32 stackBytes;  // 0xa8
    dm_int32 poolBytes;  // 0xac
    dm_int32 poolFreeBytes;  // 0xb0
    dm_int32 broadPhaseBytes;  // 0xb4
    dm_int32 bodyBytes;  // 0xb8
    dm_int32 phantomBytes;  // 0xbc
    dm_int32 fixtureBytes;  // 0xc0
    dm_int32 contactBytes;  // 0xc4
    dm_int32 jointBytes;  // 0xc8
    dm_int32 totalBytes;  // 0xcc
};
static_assert(sizeof(dmCounters) == 0xd0, "dmCounters");

struct dmDebugDrawInput {  // size 0x40
    dmDebugDrawInput();
    dmAABB m_region;  // 0x00
    dmDebugDraw * m_pDraw;  // 0x20
    dm_int32 m_maxDrawCount;  // 0x28
    bool m_drawBox;  // 0x2c
    bool m_drawFixtures;  // 0x2d
    bool m_drawTransparent;  // 0x2e
    bool m_drawJoints;  // 0x2f
    bool m_drawBroadPhase;  // 0x30
    bool m_drawPairs;  // 0x31
    bool m_drawCenterOfMass;  // 0x32
    bool m_drawContacts;  // 0x33
    bool m_drawPolytopeIndices;  // 0x34
    bool m_drawMeshIndices;  // 0x35
    dm_float32 m_massMultiplier;  // 0x38
};
static_assert(sizeof(dmDebugDrawInput) == 0x40, "dmDebugDrawInput");

struct dmFeaturePair {  // size 0x8
    dm_uint8 featureA;  // 0x00
    dm_uint8 featureB;  // 0x01
    dm_uint8 typeA;  // 0x02
    dm_uint8 typeB;  // 0x03
    dm_int32 triangleIndex;  // 0x04
};
static_assert(sizeof(dmFeaturePair) == 0x8, "dmFeaturePair");

union dmContactID {  // size 0x8
    dmFeaturePair fp;  // 0x00
    dm_uint64 key;  // 0x00
};
static_assert(sizeof(dmContactID) == 0x8, "dmContactID");

struct dmManifoldPoint {  // size 0x30
    dm_vec4 point;  // 0x00
    dm_float32 separation;  // 0x10
    dm_float32 normalImpulse;  // 0x14
    dmContactID id;  // 0x18
    bool isNew;  // 0x20
};
static_assert(sizeof(dmManifoldPoint) == 0x30, "dmManifoldPoint");

struct dmManifold {  // size 0x100
    dmManifoldPoint m_points[4];  // 0x00
    dm_vec4 m_normal;  // 0xc0
    dm_vec4 m_tangentImpulse;  // 0xd0
    dm_vec4 m_rollingImpulse;  // 0xe0
    dm_float32 m_twistImpulse;  // 0xf0
    dm_int32 m_pointCount;  // 0xf4
};
static_assert(sizeof(dmManifold) == 0x100, "dmManifold");


// Functions defined in this file (bodies are not in the DWARF):
//   line   204  dmBodyDef::dmBodyDef()  [inlined into 7 functions]
//   line   317  dmFixtureDef::dmFixtureDef()  [inlined into 4 functions]
//   line   360  dmSphereDef::dmSphereDef()  [inlined into 1 functions]
//   line   377  dmCapsuleDef::dmCapsuleDef()  [inlined into 1 functions]
//   line   395  dmPolytopeDef::dmPolytopeDef()  [inlined into 2 functions]
//   line   420  dmTreeMeshDef::dmTreeMeshDef()  [inlined into 2 functions]
//   line   452  dmJointDef::dmJointDef()  [inlined into 1 functions]
//   line   506  dmMouseJointDef::dmMouseJointDef()  [inline]
//   line   572  dmRevoluteJointDef::dmRevoluteJointDef()  [inlined into 1 functions]
//   line   612  dmShoulderJointDef::dmShoulderJointDef()  [inlined into 1 functions]
//   line   659  dmSphericalJointDef::dmSphericalJointDef()  [inlined into 1 functions]
//   line   681  dmWeldJointDef::dmWeldJointDef()  [inlined into 1 functions]
//   line   711  dmQueryFilter::dmQueryFilter()  [inlined into 1 functions]
//   line   726  dmWorldRayCastInput::dmWorldRayCastInput()  [inlined into 2 functions]

// dmStructures.h:711, written from its inlined copy in DominoWorld::QueryAABB
// (one 32-bit store of 0x0000FFFF attributed to line 713).
inline dmQueryFilter::dmQueryFilter()
{
    m_includeMask = 0xFFFF;
    m_excludeMask = 0;
}

// dmStructures.h:102. No copy of this constructor leaves a store of its own:
// every caller assigns all three members afterwards (DominoClient.cpp:1338-1340
// for the polytope filter, 1002 for the mesh filter word), and no CU has a DIE
// for it, so the body is written empty (inference).
inline dmFilter::dmFilter()
{
}

// dmStructures.h:204, written from its seven inlined copies in DominoClient.cpp
// (DeferCreateBodyFromPhysMeshFunc 0x317b68, CreateBodyFromAABB 0x3182cc,
// CreateBodyFromBone 0x3190dc) and DominoServer.cpp (sCreatePhysMeshBody).
// Surviving stores: m_quaternion = dm_quat_identity (line 208, the load at
// 0x318378), the damping pair as one 8-byte zero store (209), the scale pair as
// one 8-byte store of 1.0f/1.0f (211), the four flags as one 32-bit store of
// 0x01000001 (213), m_pName = NULL (218). Lines 206-207 and 217 (m_position,
// m_bodyType, m_pUserData by member order) leave nothing in any copy because
// every caller assigns them, so they are not written (inference).
inline dmBodyDef::dmBodyDef()
{
    m_quaternion = dm_quat_identity;
    m_linearDamping = 0.0f;
    m_angularDamping = 0.0f;
    m_inertiaScale = 1.0f;
    m_gravityScale = 1.0f;
    m_enableSleeping = true;
    m_isSleeping = false;
    m_isBullet = false;
    m_isActive = true;
    m_pName = NULL;
}

// dmStructures.h:317, written from its inlined copies (CreatePhysMeshFixture
// 0x3178b0, CreateBodyFromAABB 0x3183d8, sCreateShape 0x318b18). Surviving
// stores: m_rollingResistance = 0.2f (line 322, 0x3e4ccccd at offset 0x10) and
// m_isSensor = false (324). The other members (lines 319-321, 323, 325 by
// member order) are assigned by every caller and leave nothing (inference).
inline dmFixtureDef::dmFixtureDef()
{
    m_rollingResistance = 0.2f;
    m_isSensor = false;
}

// dmStructures.h:420, written from CreatePhysMeshFixture (0x3178bc) and
// DominoServer.cpp sCreatePhysMeshBody: m_type = dm_treeMeshFixture (line 422).
// m_scale and m_pData are assigned by both callers (inference).
inline dmTreeMeshDef::dmTreeMeshDef()
{
    m_type = dm_treeMeshFixture;
}

// dmStructures.h:726, written from DominoWorld::RayCast (0x31add8) and
// DominoServer.cpp DominoRaycastAgainstWorld: triangleMask = 0xFFFF (line 729)
// is the only store; p1, p2 and pUserData are assigned by both callers (inference).
inline dmWorldRayCastInput::dmWorldRayCastInput()
{
    triangleMask = 0xFFFF;
}

// dmStructures.h:360, written from sCreateShape (0x318b1c): m_type =
// dm_sphereFixture (line 362) is the only store; m_localPosition and m_radius
// are assigned by the caller (inference).
inline dmSphereDef::dmSphereDef()
{
    m_type = dm_sphereFixture;
}

// dmStructures.h:377, written from sCreateShape (0x318c34): m_type =
// dm_capsuleFixture (line 379) and one zero fill of m_p2 and m_radius that
// survives as stp xzr/str wzr carrying line 381 (inference: the two zero
// stores were merged into one memset, which the caller's later stores do not
// remove). m_p1 leaves nothing, so it is not written (inference).
inline dmCapsuleDef::dmCapsuleDef()
{
    m_type = dm_capsuleFixture;
    m_p2 = vdupq_n_f32(0.0f);
    m_radius = 0.0f;
}

// dmStructures.h:395, written from CreateBodyFromAABB (0x3183d4) and
// sCreateShape (0x318e0c): m_type = dm_polytopeFixture (397), m_transform.p
// zeroed (399), m_transform.q = dm_quat_identity (400), m_scale = 1.0f (401).
// m_pPolytope (line 398 by member order) is assigned by both callers and
// leaves nothing (inference).
inline dmPolytopeDef::dmPolytopeDef()
{
    m_type = dm_polytopeFixture;
    m_transform.p = vdupq_n_f32(0.0f);
    m_transform.q = dm_quat_identity;
    m_scale = 1.0f;
}

// The joint definitions below are written from their copies inlined into
// DominoWorld::CreateConstraint (DominoClient.cpp:1969/1987/2017/2038, original
// 0x31a140-0x31a378). Each has only an abstract DIE in the original (external,
// no concrete DIE), so it was declared inline. Only stores that survive in that
// copy are written; members the caller always assigns leave nothing (inference).

// dmStructures.h:452 (copy nested in dmShoulderJointDef): m_pUserData = NULL
// (457:15, str xzr at +0x18) and m_collideConnected = false (458:22, strb at +0x20).
// Lines 454-456 (m_type, m_bodyIdA, m_bodyIdB by member order) leave nothing (inference).
inline dmJointDef::dmJointDef()
{
    m_pUserData = NULL;
    m_collideConnected = false;
}

// dmStructures.h:572: m_type = dm_revoluteJoint (574:10, 4); one 16-byte zero
// store over +0x70..+0x7c (m_lowerAngle, m_upperAngle, m_maxMotorTorque,
// m_motorMode) and m_enableLimit = false (+0x80), both attributed to 577:16.
inline dmRevoluteJointDef::dmRevoluteJointDef()
{
    m_type = dm_revoluteJoint;
    m_lowerAngle = 0.0f;
    m_upperAngle = 0.0f;
    m_maxMotorTorque = 0.0f;
    m_motorMode = dm_motorOffMode;
    m_enableLimit = false;
}

// dmStructures.h:612: m_type = dm_shoulderJoint (614:10, 5); one 8-byte zero
// store at +0x7c (m_maxMotorTorque, m_motorMode) and m_enableTwistLimit = false
// (+0x84), attributed to 621:22. The twist and cone angles are assigned by the
// caller and leave nothing (inference).
inline dmShoulderJointDef::dmShoulderJointDef()
{
    m_type = dm_shoulderJoint;
    m_maxMotorTorque = 0.0f;
    m_motorMode = dm_motorOffMode;
    m_enableTwistLimit = false;
}

// dmStructures.h:659: m_type = dm_sphericalJoint (661:10, 6) and
// m_frictionTorque = 0.0f (663:18, +0x50).
inline dmSphericalJointDef::dmSphericalJointDef()
{
    m_type = dm_sphericalJoint;
    m_frictionTorque = 0.0f;
}

// dmStructures.h:681: m_type = dm_weldJoint (683:10, 7); one 16-byte store at
// +0x70: m_angularFrequencyHz = m_linearFrequencyHz = 0.0f (686:24) and
// m_angularDampingRatio = m_linearDampingRatio = 0.7f (0x3f333333, 688:25).
inline dmWeldJointDef::dmWeldJointDef()
{
    m_type = dm_weldJoint;
    m_angularFrequencyHz = 0.0f;
    m_linearFrequencyHz = 0.0f;
    m_angularDampingRatio = 0.7f;
    m_linearDampingRatio = 0.7f;
}
