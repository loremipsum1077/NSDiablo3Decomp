// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/dmWorld.h

#pragma once

#include "../Collision/BroadPhase/dmBroadPhase.h"
#include "../Common/dmAtomic.h"
#include "../Common/dmPool.h"
#include "../Common/dmPoolAllocator.h"
#include "../Common/dmStackAllocator.h"
// dmBuffer is only used by reference here; its template body is not needed.
template <typename T, int N> class dmBuffer;
class dmContactListener;
class dmRayCastPointCollector;
class dmShapeCastPointCollector;
#include "../Include/dmMath.h"
#include "../Include/dmSerialize.h"
#include "../Include/dmStructures.h"

struct dmAABB;
struct dmColor;
class dmDestructionListener;
struct dmFixture;
struct dmHeightMaterial;
class dmJobDispatcher;
struct dmMutex;
class dmShape;
struct dmTOIJob;
struct dmTimeStep;

struct dmWorld {  // size 0x490
    void Create(const dmWorldDef * pDef);
    void Destroy();
    dm_int32 CreateBody(const dmBodyDef * pDef);
    void DestroyBody(dm_int32 index);
    dm_int32 CreatePhantom(const dmPhantomDef * pDef);
    void DestroyPhantom(dm_int32 index);
    dm_int32 CreateJoint(const dmJointDef * pDef);
    void DestroyJoint(dm_int32 index);
    void Step(dm_vec4Arg dt, dm_int32 velocityIterations, dm_int32 positionIterations, bool clearForces);
    void ClearForces();
    void DebugDraw(const dmDebugDrawInput *);
    void QueryAABB(dmBuffer<dmFixtureId, 32> & fixtures, const dmAABB & aabb, const dmQueryFilter & filter);
    bool RayCastClosest(dmWorldRayCastOutput * pOutput, const dmWorldRayCastInput * pInput);
    bool RayCastAny(const dmWorldRayCastInput * pInput);
    void RayCast(dmRayCastPointCollector * pCollector, const dmWorldRayCastInput * pInput);
    void ShapeCast(dmShapeCastPointCollector *, const dmWorldShapeCastInput *);
    bool IsLocked() const;
    void Validate();
    void ShiftOrigin(dm_vec4Arg origin);
    void GetProfile(dmProfile * pProfile);
    void ResetProfile();
    void GetCounters(dmCounters *);
    void Collide(dm_int32 index, dm_int32 final, dmMutex * pMutex);
    void ParallelCollide();
    void AddPair(dm_uint64 userIndexA, dm_uint64 userIndexB);
    void CreateContact(dm_int32 fixtureIndexA, dm_int32 fixtureIndexB);
    void DestroyContact(dm_int32 index);
    void Solve(const dmTimeStep & step);
    void ParallelTOI(dm_float32 * aTOIs, bool * aTOIHits);
    void ProcessTOIJob(dmTOIJob * pJob);
    void SolveTOI(const dmTimeStep & step);
    void ResetIslandStamps();
    void DrawShape(const dmDebugDrawInput *, const dmFixture *, const dmTransform &, const dmColor &);
    void DrawMeshShape(const dmDebugDrawInput *, const dmShape *, const dmTransform &, const dmColor &, dm_uint64);
    void Dump();
    static dmWorld * s_aWorlds[65534];
    dm_int16 m_index;  // 0x00
    dmBroadPhase m_broadPhase;  // 0x08
    dmPoolAllocator m_poolAllocator;  // 0x58
    dmStackAllocator m_stackAllocator;  // 0xe0
    dmProfile m_profile;  // 0x110
    dm_int32 m_stepCount;  // 0x164
    dm_float32 m_totalStepTime;  // 0x168
    dmAtomic_float32 m_broadPhaseTimer;  // 0x16c
    dmAtomic_float32 m_setupContraintTimer;  // 0x170
    dmAtomic_float32 m_velocityConstraintTimer;  // 0x174
    dmAtomic_float32 m_positionConstraintTimer;  // 0x178
    dmCounters m_counters;  // 0x17c
    dmPool<dmBody> m_bodies;  // 0x250
    dmPool<dmPhantom> m_phantoms;  // 0x270
    dmPool<dmFixture> m_fixtures;  // 0x290
    dmPool<dmContact> m_contacts;  // 0x2b0
    dmPool<dmMeshContact> m_meshContacts;  // 0x2d0
    dmPool<dmConvexContact> m_convexContacts;  // 0x2f0
    dmPool<dmJoint> m_joints;  // 0x310
    dmPool<dmDistanceJoint> m_distanceJoints;  // 0x330
    dmPool<dmMouseJoint> m_mouseJoints;  // 0x350
    dmPool<dmPrismaticJoint> m_prismaticJoints;  // 0x370
    dmPool<dmRevoluteJoint> m_revoluteJoints;  // 0x390
    dmPool<dmShoulderJoint> m_shoulderJoints;  // 0x3b0
    dmPool<dmSphericalJoint> m_sphericalJoints;  // 0x3d0
    dmPool<dmWeldJoint> m_weldJoints;  // 0x3f0
    dm_vec4 m_inv_dt0;  // 0x410
    dm_vec4 m_gravity;  // 0x420
    dmContactListener * m_pContactListener;  // 0x430
    dmDestructionListener * m_pDestructionListener;  // 0x438
    dmJobDispatcher * m_pJobDispatcher;  // 0x440
    dm_int32 * m_aTOIBodies;  // 0x448
    dm_int32 m_toiBodyCount;  // 0x450
    dm_uint32 m_islandStamp;  // 0x454
    dm_vec4 * m_aDebugVertices;  // 0x458
    dm_int32 m_debugVertexCapacity;  // 0x460
    dmColor * m_aDebugColors;  // 0x468
    dm_int32 m_debugColorCapacity;  // 0x470
    dmHeightMaterial * m_aMaterials;  // 0x478
    dm_int32 m_materialCount;  // 0x480
    dmAtomic_s32 m_lock;  // 0x484
    bool m_newContacts : 1;  // 0x488
    bool m_useSleep : 1;  // 0x488
    bool m_useWarmStarting : 1;  // 0x488
    bool m_useContinuous : 1;  // 0x488
    bool m_useSubStepping : 1;  // 0x488
    bool m_isStepComplete : 1;  // 0x488
};
static_assert(sizeof(dmWorld) == 0x490, "dmWorld");

