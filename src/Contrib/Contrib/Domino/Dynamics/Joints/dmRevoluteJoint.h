// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmRevoluteJoint.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmStructures.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct dmBody;
struct dmJoint;
struct dmSolverData;
struct dmWorld;

struct dmRevoluteJoint {  // size 0x2c0
    static bool IsValid(const dmRevoluteJointDef *);
    void Create(const dmRevoluteJointDef * pDef);
    dm_float32 GetAngle(const dmWorld * pWorld, const dmJoint * pBase) const;
    dm_float32 GetSpeed(dmWorld *, dmJoint *) const;
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
    dmMtx m_pivotMass;  // 0x70
    dm_vec4 m_Jh;  // 0xb0
    dm_vec4 m_Bha;  // 0xc0
    dm_vec4 m_Bhb;  // 0xd0
    dmMtx m_Ja;  // 0xe0
    dmMtx m_Jb;  // 0x120
    dmMtx m_Ba;  // 0x160
    dmMtx m_Bb;  // 0x1a0
    dmMtx m_Jp;  // 0x1e0
    dmMtx m_perpMass;  // 0x220
    dm_float32 m_motorMass;  // 0x260
    dm_vec4 m_perpImpulse;  // 0x270
    dm_vec4 m_pivotImpulse;  // 0x280
    dm_float32 m_motorImpulse;  // 0x290
    dm_float32 m_limitImpulse;  // 0x294
    dm_float32 m_motorBias;  // 0x298
    dmMotorMode m_motorMode;  // 0x29c
    dm_float32 m_maxMotorTorque;  // 0x2a0
    dm_float32 m_desiredMotorSpeed;  // 0x2a4
    dm_float32 m_desiredMotorAngle;  // 0x2a8
    dmLimitState m_limitState;  // 0x2ac
    dm_float32 m_lowerAngle;  // 0x2b0
    dm_float32 m_upperAngle;  // 0x2b4
};
static_assert(sizeof(dmRevoluteJoint) == 0x2c0, "dmRevoluteJoint");

