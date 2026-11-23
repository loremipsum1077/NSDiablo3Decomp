// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmSphericalJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

struct dmBody;
struct dmDebugDrawInput;
struct dmJoint;
struct dmSolverData;
struct dmSphericalJointDef;
struct dmWorld;

struct dmSphericalJoint {  // size 0x180
    static bool IsValid(const dmSphericalJointDef *);
    void Create(const dmSphericalJointDef * pDef);
    void InitVelocityConstraints(const dmBody * pBodyA, const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData);
    void SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase);
    void SolvePositionConstraints(dmSolverData * pData, const dmJoint * pBase);
    void ApplyScale(dm_vec4Arg, dm_int32);
    void Draw(const dmWorld *, const dmJoint *, const dmDebugDrawInput *) const;
    void Dump(const dmJoint *, dm_int32, dm_int32, FILE *) const;
    union {  // size 0x4
        dm_int32 m_next;  // 0x00
        dm_int32 m_index;  // 0x00
    };  // 0x00
    dm_vec4 m_localAnchorA;  // 0x10
    dm_vec4 m_localAnchorB;  // 0x20
    dm_vec4 m_localCenterA;  // 0x30
    dm_vec4 m_localCenterB;  // 0x40
    dmMtx m_pivotMass;  // 0x50
    dm_vec4 m_pivotImpulse;  // 0x90
    dmMtx m_Ja;  // 0xa0
    dmMtx m_Jb;  // 0xe0
    dmMtx m_frictionMass;  // 0x120
    dm_vec4 m_frictionImpulse;  // 0x160
    dm_float32 m_frictionTorque;  // 0x170
};
static_assert(sizeof(dmSphericalJoint) == 0x180, "dmSphericalJoint");


// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(dmSphericalJoint) == 0x180, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_localAnchorA) == 0x10, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_localAnchorB) == 0x20, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_localCenterA) == 0x30, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_localCenterB) == 0x40, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_pivotMass) == 0x50, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_pivotImpulse) == 0x90, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_Ja) == 0xa0, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_Jb) == 0xe0, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_frictionMass) == 0x120, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_frictionImpulse) == 0x160, "dmSphericalJoint");
static_assert(offsetof(dmSphericalJoint, m_frictionTorque) == 0x170, "dmSphericalJoint");
