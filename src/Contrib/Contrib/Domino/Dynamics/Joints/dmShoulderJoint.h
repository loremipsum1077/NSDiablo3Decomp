// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmShoulderJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmStructures.h"
#include <stddef.h>
#include <stdint.h>

struct dmBody;
struct dmJoint;
struct dmSolverData;
struct dmWorld;

struct dmShoulderJoint {  // size 0x2e0
    static bool IsValid(const dmShoulderJointDef *);
    void Create(const dmShoulderJointDef * pDef);
    dm_float32 GetConeAngle(const dmWorld *, const dmJoint *) const;
    dm_float32 GetTwistAngle(const dmWorld * pWorld, const dmJoint * pBase) const;
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
    dm_vec4 m_pivotImpulse;  // 0x70
    dm_vec4 m_motorImpulse;  // 0x80
    dm_float32 m_twistImpulse;  // 0x90
    dm_float32 m_coneImpulse;  // 0x94
    dmMtx m_Ja;  // 0xa0
    dmMtx m_Jb;  // 0xe0
    dmMtx m_Ba;  // 0x120
    dmMtx m_Bb;  // 0x160
    dm_vec4 m_Jt;  // 0x1a0
    dm_vec4 m_Bta;  // 0x1b0
    dm_vec4 m_Btb;  // 0x1c0
    dm_vec4 m_Jc;  // 0x1d0
    dm_vec4 m_Bca;  // 0x1e0
    dm_vec4 m_Bcb;  // 0x1f0
    dmMtx m_pivotMass;  // 0x200
    dmMtx m_motorMass;  // 0x240
    dm_float32 m_twistMass;  // 0x280
    dm_float32 m_coneMass;  // 0x284
    dm_float32 m_maxMotorTorque;  // 0x288
    dm_vec4 m_motorAngularVelocity;  // 0x290
    dm_vec4 m_motorQuaternion;  // 0x2a0
    dm_vec4 m_motorBias;  // 0x2b0
    dm_float32 m_lowerTwistLimit;  // 0x2c0
    dm_float32 m_upperTwistLimit;  // 0x2c4
    dm_float32 m_swingLimit;  // 0x2c8
    dmMotorMode m_motorMode;  // 0x2cc
    dmLimitState m_twistState;  // 0x2d0
    dmLimitState m_coneState;  // 0x2d4
};
static_assert(sizeof(dmShoulderJoint) == 0x2e0, "dmShoulderJoint");

