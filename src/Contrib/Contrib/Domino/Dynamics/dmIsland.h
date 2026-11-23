// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/dmIsland.h

#pragma once

#include "Contacts/dmContactSolver.h"
#include "Joints/dmJoint.h"
#include "dmSolver.h"
#include "../Include/dmMath.h"
#include "../Include/dmSerialize.h"

class dmAtomic_float32;
struct dmBody;
struct dmContact;
class dmContactListener;
struct dmFixture;
class dmStackAllocator;
struct dmWorld;

struct dmIslandDef {  // size 0x80
    dmIslandDef() {}  // line 30; inferred empty body: the DWARF declares the constructor here, the binary has no out-of-line copy and no inlined code for it (Solve 563-570 / SolveTOI store every member directly)
    dmTimeStep step;  // 0x00
    dmWorld * pWorld;  // 0x40
    dm_int32 * aBodies;  // 0x48
    dm_int32 bodyCount;  // 0x50
    dm_int32 * aContacts;  // 0x58
    dm_int32 contactCount;  // 0x60
    dm_int32 * aJoints;  // 0x68
    dm_int32 jointCount;  // 0x70
};
static_assert(sizeof(dmIslandDef) == 0x80, "dmIslandDef");

struct dmIsland {  // size 0x180
    void Create(dmIslandDef * pDef);
    void Destroy();
    void Solve();
    void SolveTOI();
    void Report();
    static void SolveFcn(void * pParam);
    static void SolveTOIFcn(void * pParam);
    dm_int16 m_worldIndex;  // 0x00
    dm_vec4 m_gravity;  // 0x10
    dmBody * m_bodyArrayPtr;  // 0x20
    dm_int32 m_bodyCeiling;  // 0x28
    dmJoint * m_jointArrayPtr;  // 0x30
    dm_int32 m_jointCeiling;  // 0x38
    dmFixture * m_fixtureArray;  // 0x40
    dmContact * m_contactArrayPtr;  // 0x48
    dmContactListener * m_pContactListener;  // 0x50
    dmStackAllocator * m_pStackAllocator;  // 0x58
    dmAtomic_float32 * m_pSetupContraintTimer;  // 0x60
    dmAtomic_float32 * m_pVelocityConstraintTimer;  // 0x68
    dmAtomic_float32 * m_pPositionConstraintTimer;  // 0x70
    dmSpecificJointArrays m_specificJointArrays;  // 0x78
    dmContactSolver m_contactSolver;  // 0xb0
    dmTimeStep m_step;  // 0xf0
    dm_int32 * m_aBodies;  // 0x130
    dm_int32 * m_aContacts;  // 0x138
    dm_int32 * m_aJoints;  // 0x140
    dmPosition * m_aPositions;  // 0x148
    dmVelocity * m_aVelocities;  // 0x150
    dmMtx * m_aInvIs;  // 0x158
    dm_int32 m_bodyCount;  // 0x160
    dm_int32 m_jointCount;  // 0x164
    dm_int32 m_contactCount;  // 0x168
    dmIsland * m_pNext;  // 0x170
};
static_assert(sizeof(dmIsland) == 0x180, "dmIsland");

