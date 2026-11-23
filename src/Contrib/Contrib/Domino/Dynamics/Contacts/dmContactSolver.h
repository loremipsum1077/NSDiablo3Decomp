// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/Domino/Dynamics/Contacts/dmContactSolver.h

#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

struct dmManifold;
struct dmSolverData;
class dmStackAllocator;
struct dmWorld;

struct dmVelocityConstraintPoint {  // size 0x60
    dm_vec4 Ja;  // 0x00
    dm_vec4 Ba;  // 0x10
    dm_vec4 Jb;  // 0x20
    dm_vec4 Bb;  // 0x30
    dm_vec4 mass_gamma_beta_impulse;  // 0x40
    dm_vec4 results;  // 0x50
};
static_assert(sizeof(dmVelocityConstraintPoint) == 0x60, "dmVelocityConstraintPoint");

struct dmContactVelocityConstraint {  // size 0x1f0
    dmMtx invInertiaA;  // 0x00
    dmMtx invInertiaB;  // 0x40
    dmMtx rollingMass;  // 0x80
    dm_vec4 normal;  // 0xc0
    dm_vec4 tangent1;  // 0xd0
    dm_vec4 tangent2;  // 0xe0
    dm_vec4 tangentAnchor;  // 0xf0
    dm_vec4 Jt1A;  // 0x100
    dm_vec4 Jt1B;  // 0x110
    dm_vec4 Bt1B;  // 0x120
    dm_vec4 Bt1A;  // 0x130
    dm_vec4 Jt2A;  // 0x140
    dm_vec4 Jt2B;  // 0x150
    dm_vec4 Bt2A;  // 0x160
    dm_vec4 Bt2B;  // 0x170
    dm_vec4 invMass;  // 0x180
    dm_vec4 impulse;  // 0x190
    dm_vec4 rollingImpulse;  // 0x1a0
    dm_vec4 meff;  // 0x1b0
    dm_vec4 material;  // 0x1c0
    dmManifold * pManifold;  // 0x1d0
    dm_int32 fixtureIndexA;  // 0x1d8
    dm_int32 fixtureIndexB;  // 0x1dc
    dm_int32 indexA;  // 0x1e0
    dm_int32 indexB;  // 0x1e4
    dm_int32 firstPointIndex;  // 0x1e8
    dm_int32 pointCount;  // 0x1ec
};
static_assert(sizeof(dmContactVelocityConstraint) == 0x1f0, "dmContactVelocityConstraint");

struct dmPositionConstraintPoint {  // size 0x30
    dm_vec4 localAnchorA;  // 0x00
    dm_vec4 localAnchorB;  // 0x10
    dm_vec4 sep_emass;  // 0x20
};
static_assert(sizeof(dmPositionConstraintPoint) == 0x30, "dmPositionConstraintPoint");

struct dmContactPositionConstraint {  // size 0xb0
    dmMtx invInertiaA_Local;  // 0x00
    dmMtx invInertiaB_Local;  // 0x40
    dm_vec4 normal;  // 0x80
    dm_vec4 mass;  // 0x90
    dm_int32 indexA;  // 0xa0
    dm_int32 indexB;  // 0xa4
    dm_int32 firstPointIndex;  // 0xa8
    dm_int32 pointCount;  // 0xac
};
static_assert(sizeof(dmContactPositionConstraint) == 0xb0, "dmContactPositionConstraint");

struct dmContactSolverDef {  // size 0x18
    dmWorld * pWorld;  // 0x00
    const dm_int32 * aContacts;  // 0x08
    dm_int32 contactCount;  // 0x10
};
static_assert(sizeof(dmContactSolverDef) == 0x18, "dmContactSolverDef");

struct dmContactSolver {  // size 0x38
    void Create(dmContactSolverDef * pDef, dmStackAllocator * pAllocator);
    void Destroy(dmStackAllocator * pAllocator);
    void Setup(const dmSolverData * pData);
    void SolveVelocityConstraints(const dmSolverData * pData);
    void StoreImpulses();
    bool SolvePositionConstraints(const dmSolverData * pData, dm_vec4Arg baumgarte);
    const dm_int32 * m_aContacts;  // 0x00
    dm_int32 m_contactCount;  // 0x08
    dmContactVelocityConstraint * m_aVelocityConstraints;  // 0x10
    dmContactPositionConstraint * m_aPositionConstraints;  // 0x18
    dmVelocityConstraintPoint * m_aVelocityConstraintPoints;  // 0x20
    dmPositionConstraintPoint * m_aPositionConstraintPoints;  // 0x28
    dm_int32 m_constraintCount;  // 0x30
    dm_int32 m_pointCount;  // 0x34
};
static_assert(sizeof(dmContactSolver) == 0x38, "dmContactSolver");

