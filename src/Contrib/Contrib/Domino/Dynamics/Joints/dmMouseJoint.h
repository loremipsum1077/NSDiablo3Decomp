// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmMouseJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

struct dmBody;
struct dmDebugDrawInput;
struct dmJoint;
struct dmMouseJointDef;
struct dmSolverData;
struct dmWorld;

struct dmMouseJoint {  // size 0x120
    static bool IsValid(const dmMouseJointDef *);
    void Create(const dmMouseJointDef * pDef);
    void InitVelocityConstraints(const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData);
    void SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase);
    void ApplyScale(dm_vec4Arg, dm_int32);
    void Draw(const dmWorld *, const dmJoint *, const dmDebugDrawInput *) const;
    union {  // size 0x4
        dm_int32 m_next;  // 0x00
        dm_int32 m_index;  // 0x00
    };  // 0x00
    dm_vec4 m_localAnchor;  // 0x10
    dm_vec4 m_target;  // 0x20
    dm_vec4 m_impulse;  // 0x30
    dmMtx m_J;  // 0x40
    dmMtx m_B;  // 0x80
    dmMtx m_mass;  // 0xc0
    dm_vec4 m_bias;  // 0x100
    dm_float32 m_gamma;  // 0x110
    dm_float32 m_maxForce;  // 0x114
    dm_float32 m_frequencyHz;  // 0x118
    dm_float32 m_dampingRatio;  // 0x11c
};
static_assert(sizeof(dmMouseJoint) == 0x120, "dmMouseJoint");

