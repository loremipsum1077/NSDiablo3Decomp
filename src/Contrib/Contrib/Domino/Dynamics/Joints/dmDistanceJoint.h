// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmDistanceJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

struct dmBody;
struct dmDebugDrawInput;
struct dmDistanceJointDef;
struct dmJoint;
struct dmSolverData;
struct dmWorld;

struct dmDistanceJoint {  // size 0xd0
    static bool IsValid(const dmDistanceJointDef *);
    void Create(const dmDistanceJointDef * pDef);
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
    dm_vec4 m_u;  // 0x50
    dm_vec4 m_Ja;  // 0x60
    dm_vec4 m_Jb;  // 0x70
    dm_vec4 m_IJa;  // 0x80
    dm_vec4 m_IJb;  // 0x90
    dm_vec4 m_impulse;  // 0xa0
    dm_vec4 m_distance;  // 0xb0
    dm_vec4 m_mass;  // 0xc0
};
static_assert(sizeof(dmDistanceJoint) == 0xd0, "dmDistanceJoint");


// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(dmDistanceJoint) == 0xd0, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_localAnchorA) == 0x10, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_localAnchorB) == 0x20, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_localCenterA) == 0x30, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_localCenterB) == 0x40, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_u) == 0x50, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_Ja) == 0x60, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_Jb) == 0x70, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_IJa) == 0x80, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_IJb) == 0x90, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_impulse) == 0xa0, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_distance) == 0xb0, "dmDistanceJoint");
static_assert(offsetof(dmDistanceJoint, m_mass) == 0xc0, "dmDistanceJoint");
