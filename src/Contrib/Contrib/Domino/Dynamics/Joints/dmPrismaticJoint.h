// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmPrismaticJoint.h

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

struct dmPrismaticJoint {  // size 0x1c0
    static bool IsValid(const dmPrismaticJointDef *);
    void Create(const dmPrismaticJointDef * pDef);
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
    dmMtx m_worldFrameA;  // 0x70
    dm_vec4 m_Ja11;  // 0xb0
    dm_vec4 m_Ja12;  // 0xc0
    dm_vec4 m_Ja21;  // 0xd0
    dm_vec4 m_Ja22;  // 0xe0
    dm_float32 m_mass11;  // 0xf0
    dm_float32 m_mass12;  // 0xf4
    dm_float32 m_mass22;  // 0xf8
    dm_float32 m_impulse1;  // 0xfc
    dm_float32 m_impulse2;  // 0x100
    dmMtx m_angularMass;  // 0x110
    dm_vec4 m_angularImpulse;  // 0x150
    dmMotorMode m_motorMode;  // 0x160
    dm_float32 m_motorSpeed;  // 0x164
    dm_float32 m_maxMotorForce;  // 0x168
    dm_float32 m_motorMass;  // 0x16c
    dm_float32 m_motorBias;  // 0x170
    dm_float32 m_desiredTranslation;  // 0x174
    dm_vec4 m_Jma1;  // 0x180
    dm_vec4 m_Jma2;  // 0x190
    dm_float32 m_motorImpulse;  // 0x1a0
    dmLimitState m_limitState;  // 0x1a4
    dm_float32 m_lowerLimit;  // 0x1a8
    dm_float32 m_upperLimit;  // 0x1ac
    dm_float32 m_limitImpulse;  // 0x1b0
};
static_assert(sizeof(dmPrismaticJoint) == 0x1c0, "dmPrismaticJoint");

