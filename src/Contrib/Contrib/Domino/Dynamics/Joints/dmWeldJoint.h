// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmWeldJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include <stddef.h>
#include <stdint.h>

struct dmBody;
struct dmDebugDrawInput;
struct dmJoint;
struct dmSolverData;
struct dmWeldJointDef;
struct dmWorld;

struct dmWeldJoint {  // size 0x1e0
    static bool IsValid(const dmWeldJointDef *);
    void Create(const dmWeldJointDef * pDef);
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
    dmTransform m_localFrameA;  // 0x10
    dmTransform m_localFrameB;  // 0x30
    dm_vec4 m_localCenterA;  // 0x50
    dm_vec4 m_localCenterB;  // 0x60
    dmMtx m_angularMass;  // 0x70
    dm_vec4 m_angularImpulse;  // 0xb0
    dmMtx m_pivotMass;  // 0xc0
    dm_vec4 m_pivotImpulse;  // 0x100
    dmMtx m_Ja;  // 0x110
    dmMtx m_Jb;  // 0x150
    dm_vec4 m_angularGamma;  // 0x190
    dm_vec4 m_angularBias;  // 0x1a0
    dm_vec4 m_linearGamma;  // 0x1b0
    dm_vec4 m_linearBias;  // 0x1c0
    dm_float32 m_angularFrequencyHz;  // 0x1d0
    dm_float32 m_angularDampingRatio;  // 0x1d4
    dm_float32 m_linearFrequencyHz;  // 0x1d8
    dm_float32 m_linearDampingRatio;  // 0x1dc
};
static_assert(sizeof(dmWeldJoint) == 0x1e0, "dmWeldJoint");

