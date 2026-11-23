// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/dmBody.h

#pragma once

#include "../Collision/Functions/dmHomingFunction.h"
template <typename T, int N> class dmBuffer;
#include "../Include/dmMath.h"
#include "../Include/dmSerialize.h"
#include "../Include/dmStructures.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

struct dmAABB;
struct dmWorld;

struct dmBody {  // size 0x1a0
    void Create(const dmBodyDef * pDef);
    void Destroy(dmWorld * pWorld);
    void SetType(dmWorld * pWorld, dmBodyType type);
    dm_int32 CreateFixture(dmWorld * pWorld, const dmFixtureDef * pDef);
    void DestroyFixture(dmWorld * pWorld, dm_int32 fixtureIndex);
    void SetTransform(dmWorld * pWorld, const dmTransform & xf);
    void SetTransformNoSync(const dmTransform & xf);
    void ApplyForce(dm_vec4Arg force, dm_vec4Arg worldPoint, bool wake);
    void ApplyTorque(dm_vec4Arg torque, bool wake);
    void ApplyImpulse(dm_vec4Arg impulse, dm_vec4Arg worldPoint, bool wake);
    void ApplyAngularImpulse(dm_vec4Arg, bool);
    void SetMassProperties(dm_float32 mass, dm_vec4Arg localCenter, const dmMtx & I);
    dm_vec4 GetLinearVelocityFromWorldPoint(dm_vec4Arg worldPoint) const;
    dm_vec4 GetLinearVelocityFromLocalPoint(dm_vec4Arg) const;
    void EnableSleeping(bool flag);
    void SetSleeping(bool flag);
    dm_vec4 ComputeSurfaceArea(dmWorld * pWorld) const;
    dm_vec4 ComputeProjectedSurfaceArea(dmWorld * pWorld, dm_vec4Arg dir) const;
    dm_vec4 ComputeVolume(dmWorld * pWorld) const;
    dm_vec4 ComputeSubmergedVolume(dmWorld * pWorld, dm_vec4 * pCentroid, dm_vec4Arg normal, dm_vec4Arg offset) const;
    dmAABB GetLocalAABB(dmWorld * pWorld) const;
    void StabilizeTree(dmWorld * pWorld, dm_float32 inertiaScale, dm_float32 massRatio);
    void BuildIsland(dmWorld * pWorld, dmBuffer<dmBody *, 64> & buffer);
    void StabilizeTreeRecursive(dmWorld * pWorld, dm_float32 inertiaScale, dm_float32 massRatio, const dmBody * pParent);
    void SetActive(dmWorld *, bool);
    static bool ShouldCollide(dmWorld * pWorld, const dmBody * pBodyA, const dmBody * pBodyB);
    bool IsTouching(dmWorld * pWorld);
    void SetMassFromFixtures(dmWorld * pWorld);
    void SynchronizeTransform();
    void SynchronizeFixtures(dmWorld * pWorld);
#line 309
    void Advance(dm_vec4Arg t)
    {



        if (m_bodyType != dm_staticBody)
        {
            m_sweep.Advance(t);
            m_sweep.m_center = m_sweep.m_center0;
            m_sweep.m_quaternion = m_sweep.m_quaternion0;
            SynchronizeTransform();
        }
    }
#line 50
    void DumpTree(dmWorld *);
    void Dump(dmWorld *, dm_int32, FILE *);
    void DumpJoints(dmWorld * pWorld, const dm_int32 * aNewIndices, FILE * pFile);
    dm_vec4 GetClosestPoint(dmWorld * pWorld, dm_vec4Arg point) const;
    void ApplyScale(dmWorld *, dm_vec4Arg);
    bool IsFast(dm_vec4Arg dt) const;
    union {  // size 0x4
        dm_int32 m_index;  // 0x00
        dm_int32 m_next;  // 0x00
    };  // 0x00
    dmBodyType m_bodyType : 3;  // 0x04
    bool m_isSleeping : 1;  // 0x04
    bool m_enableSleeping : 1;  // 0x04
    bool m_isBullet : 1;  // 0x04
    bool m_isActive : 1;  // 0x04
    bool m_isStabilized : 1;  // 0x04
    dmTransform m_transform;  // 0x10
    dm_vec4 m_linearVelocity;  // 0x30
    dm_vec4 m_angularVelocity;  // 0x40
    dmSweep m_sweep;  // 0x50
    dmMtx m_I;  // 0xb0
    dmMtx m_invI;  // 0xf0
    dm_vec4 m_force;  // 0x130
    dm_vec4 m_torque;  // 0x140
    void * m_pUserData;  // 0x150
    dm_int32 m_fixtureList;  // 0x158
    dm_int32 m_jointListKey;  // 0x15c
    dm_float32 m_mass;  // 0x160
    dm_float32 m_invMass;  // 0x164
    dm_float32 m_gravityScale;  // 0x168
    dm_float32 m_linearDamping;  // 0x16c
    dm_float32 m_angularDamping;  // 0x170
    dm_float32 m_inertiaScale;  // 0x174
    dm_float32 m_sleepTime;  // 0x178
    dm_float32 m_minRadius;  // 0x17c
    dm_float32 m_maxRadius;  // 0x180
    dm_uint32 m_islandStamp;  // 0x184
    dm_uint16 m_revision;  // 0x188
    dm_uint8 m_toiCount;  // 0x18a
    char m_name[16];  // 0x18b
};
static_assert(sizeof(dmBody) == 0x1a0, "dmBody");


// Functions defined in this file (bodies are not in the DWARF):
//   line   223  dm_vec4 dmBody::GetLinearVelocityFromWorldPoint(dm_vec4Arg worldPoint) const  [inlined into 2 functions]
//   line   235  void dmBody::ApplyForce(dm_vec4Arg force, dm_vec4Arg worldPoint, bool wake)  [inlined into 1 functions]
//   line   250  void dmBody::ApplyTorque(dm_vec4Arg torque, bool wake)  [inlined into 1 functions]
//   line   264  void dmBody::ApplyImpulse(dm_vec4Arg impulse, dm_vec4Arg worldPoint, bool wake)  [inlined into 1 functions]
//   line   302  void dmBody::SynchronizeTransform()  [inlined into 3 functions]
//   line   309  void dmBody::Advance(dm_vec4Arg t)  [inlined into 1 functions]
//   line   324  void dmBody::SetSleeping(bool flag)  [inlined into 10 functions]

// dmBody.h:324. Written from dmJoint::Destroy, whose calls pass false; the
// flag == true path (lines 327-330) is not visible there.
inline void dmBody::SetSleeping(bool flag)
{
    if (flag == m_isSleeping)
    {
        return;
    }

    m_isSleeping = flag;

    if (flag == false)
    {
        m_sleepTime = 0.0f;
    }

    m_linearVelocity = dmVecSet(0.0f);
    m_angularVelocity = dmVecSet(0.0f);
    m_force = dmVecSet(0.0f);
    m_torque = dmVecSet(0.0f);
}

// dmBody.h:302 (DW_INL_inlined only, no concrete DIE). Written from the inlined copy in
// dmIsland::Solve: header rows 304:16 (q/p store) and 305:35 (subtraction), dmRotate at 305.
inline void dmBody::SynchronizeTransform()
{
    m_transform.q = m_sweep.m_quaternion;
    m_transform.p = m_sweep.m_center - dmRotate(m_transform.q, m_sweep.m_localCenter);
}

// dmBody.h:223. Native operators: original rows 225:26 (+) and 225:66 (-), only dmCross inlined (DWARF).
inline dm_vec4 dmBody::GetLinearVelocityFromWorldPoint(dm_vec4Arg worldPoint) const
{
    return m_linearVelocity + dmCross(m_angularVelocity, worldPoint - m_sweep.m_center);
}

// dmBody.h:235.
inline void dmBody::ApplyForce(dm_vec4Arg force, dm_vec4Arg worldPoint, bool wake)
{
    if (wake && m_isSleeping)
    {
        SetSleeping(false);
    }

    if (m_isSleeping == false)
    {
        m_force += force;
        m_torque = dmAdd(m_torque, dmCross(dmSub(worldPoint, m_sweep.m_center), force));
    }
}

// dmBody.h:250.
inline void dmBody::ApplyTorque(dm_vec4Arg torque, bool wake)
{
    if (wake && m_isSleeping)
    {
        SetSleeping(false);
    }

    if (m_isSleeping == false)
    {
        m_torque += torque;
    }
}

// dmBody.h:264.
inline void dmBody::ApplyImpulse(dm_vec4Arg impulse, dm_vec4Arg worldPoint, bool wake)
{
    if (wake && m_isSleeping)
    {
        SetSleeping(false);
    }

    if (m_isSleeping == false)
    {
        m_linearVelocity = dmAdd(m_linearVelocity, dmMul(impulse, dmVecSet(m_invMass)));
        dm_vec4 T = dmCross(dmSub(worldPoint, m_sweep.m_center), impulse);
        dm_vec4 localT = dmInvRotate(m_transform.q, T);
        dm_vec4 localAngularVelocityDelta = dmMul33(m_invI, localT);
        m_angularVelocity = dmAdd(m_angularVelocity, dmRotate(m_transform.q, localAngularVelocityDelta));
    }
}
