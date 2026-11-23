// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmStructures.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

struct dmBody;
struct dmDistanceJoint;
struct dmMouseJoint;
struct dmPrismaticJoint;
struct dmRevoluteJoint;
struct dmShoulderJoint;
struct dmSolverData;
struct dmSphericalJoint;
struct dmWeldJoint;
struct dmWorld;

struct dmSpecificJointArrays {  // size 0x38
    dmDistanceJoint * m_distanceJointArrayPtr;  // 0x00
    dmMouseJoint * m_mouseJointArrayPtr;  // 0x08
    dmPrismaticJoint * m_prismaticJointArrayPtr;  // 0x10
    dmRevoluteJoint * m_revoluteJointArrayPtr;  // 0x18
    dmShoulderJoint * m_shoulderJointArrayPtr;  // 0x20
    dmSphericalJoint * m_sphericalJointArrayPtr;  // 0x28
    dmWeldJoint * m_weldJointArrayPtr;  // 0x30
};
static_assert(sizeof(dmSpecificJointArrays) == 0x38, "dmSpecificJointArrays");

struct dmJoint {  // size 0xd0
    struct Edge {  // size 0xc
        dm_int32 m_bodyIndex;  // 0x00
        dm_int32 m_nextKey;  // 0x04
        dm_int32 m_prevKey;  // 0x08
    };
    static bool IsValid(dmWorld *, const dmJointDef *);
    bool Create(dmWorld * pWorld, const dmJointDef * pDef);
    void Destroy(dmWorld * pWorld);
    dm_vec4 GetAnchorA(dmWorld *) const;
    dm_vec4 GetAnchorB(dmWorld *) const;
    dm_float32 GetLeverArmA(dmWorld * pWorld) const;
    dm_float32 GetLeverArmB(dmWorld * pWorld) const;
    void Draw(dmWorld *, const dmDebugDrawInput *);
    void InitVelocityConstraints(dmSpecificJointArrays & specificJointArrays, const dmBody * pBodyA, const dmBody * pBodyB, const dmSolverData * pData);
    void SolveVelocityConstraints(dmSpecificJointArrays & specificJointArrays, dmSolverData * pData);
    void SolvePositionConstraints(dmSpecificJointArrays & specificJointArrays, dmSolverData * pData);
    void ApplyScale(dmWorld *, dm_vec4Arg, dm_int32);
    void Dump(const dmWorld *, dm_int32, dm_int32, FILE *);
    dmMtx m_localInvInertiaA;  // 0x00
    dmMtx m_localInvInertiaB;  // 0x40
    dm_float32 m_invMassA;  // 0x80
    dm_float32 m_invMassB;  // 0x84
    dm_float32 m_maxLeverArmA;  // 0x88
    dm_float32 m_maxLeverArmB;  // 0x8c
    dm_uint32 m_islandStamp;  // 0x90
    dmJointType m_type;  // 0x94
    dm_int32 m_subIndex;  // 0x98
    union {  // size 0x4
        dm_int32 m_next;  // 0x00
        dm_int32 m_index;  // 0x00
    };  // 0x9c
    dmJoint::Edge m_edges[2];  // 0xa0
    dm_int32 m_islandIndexA;  // 0xb8
    dm_int32 m_islandIndexB;  // 0xbc
    dm_uint16 m_revision;  // 0xc0
    bool m_collideConnected;  // 0xc2
};
static_assert(sizeof(dmJoint) == 0xd0, "dmJoint");


// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(dmSpecificJointArrays) == 0x38, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_distanceJointArrayPtr) == 0x0, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_mouseJointArrayPtr) == 0x8, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_prismaticJointArrayPtr) == 0x10, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_revoluteJointArrayPtr) == 0x18, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_shoulderJointArrayPtr) == 0x20, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_sphericalJointArrayPtr) == 0x28, "dmSpecificJointArrays");
static_assert(offsetof(dmSpecificJointArrays, m_weldJointArrayPtr) == 0x30, "dmSpecificJointArrays");
static_assert(sizeof(dmJoint) == 0xd0, "dmJoint");
static_assert(offsetof(dmJoint, m_localInvInertiaA) == 0x0, "dmJoint");
static_assert(offsetof(dmJoint, m_localInvInertiaB) == 0x40, "dmJoint");
static_assert(offsetof(dmJoint, m_invMassA) == 0x80, "dmJoint");
static_assert(offsetof(dmJoint, m_invMassB) == 0x84, "dmJoint");
static_assert(offsetof(dmJoint, m_maxLeverArmA) == 0x88, "dmJoint");
static_assert(offsetof(dmJoint, m_maxLeverArmB) == 0x8c, "dmJoint");
static_assert(offsetof(dmJoint, m_islandStamp) == 0x90, "dmJoint");
static_assert(offsetof(dmJoint, m_type) == 0x94, "dmJoint");
static_assert(offsetof(dmJoint, m_subIndex) == 0x98, "dmJoint");
static_assert(offsetof(dmJoint, m_edges) == 0xa0, "dmJoint");
static_assert(offsetof(dmJoint, m_islandIndexA) == 0xb8, "dmJoint");
static_assert(offsetof(dmJoint, m_islandIndexB) == 0xbc, "dmJoint");
static_assert(offsetof(dmJoint, m_revision) == 0xc0, "dmJoint");
static_assert(offsetof(dmJoint, m_collideConnected) == 0xc2, "dmJoint");
