// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Dynamics/dmSolver.h

#pragma once

#include "../Include/dmMath.h"
#include "../Include/dmSerialize.h"

struct dmBody;
struct dmContact;

struct dmTimeStep {  // size 0x40
    dm_vec4 dt;  // 0x00
    dm_vec4 inv_dt;  // 0x10
    dm_vec4 dtRatio;  // 0x20
    dm_int32 velocityIterations;  // 0x30
    dm_int32 positionIterations;  // 0x34
    bool useWarmStarting;  // 0x38
    bool useSleep;  // 0x39
    bool clearForces;  // 0x3a
};
static_assert(sizeof(dmTimeStep) == 0x40, "dmTimeStep");

struct dmPosition {  // size 0x20
    dm_vec4 x;  // 0x00
    dm_vec4 q;  // 0x10
};
static_assert(sizeof(dmPosition) == 0x20, "dmPosition");

struct dmVelocity {  // size 0x20
    dm_vec4 v;  // 0x00
    dm_vec4 w;  // 0x10
};
static_assert(sizeof(dmVelocity) == 0x20, "dmVelocity");

struct dmSolverData {  // size 0x70
    dmTimeStep step;  // 0x00
    dmPosition * aPositions;  // 0x40
    dmVelocity * aVelocities;  // 0x48
    const dmMtx * aInvIs;  // 0x50
    dmBody * pBodyArrayPtr;  // 0x58
    dmContact * pContactArrayPtr;  // 0x60
};
static_assert(sizeof(dmSolverData) == 0x70, "dmSolverData");


// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(dmTimeStep) == 0x40, "dmTimeStep");
static_assert(offsetof(dmTimeStep, dt) == 0x0, "dmTimeStep");
static_assert(offsetof(dmTimeStep, inv_dt) == 0x10, "dmTimeStep");
static_assert(offsetof(dmTimeStep, dtRatio) == 0x20, "dmTimeStep");
static_assert(offsetof(dmTimeStep, velocityIterations) == 0x30, "dmTimeStep");
static_assert(offsetof(dmTimeStep, positionIterations) == 0x34, "dmTimeStep");
static_assert(offsetof(dmTimeStep, useWarmStarting) == 0x38, "dmTimeStep");
static_assert(offsetof(dmTimeStep, useSleep) == 0x39, "dmTimeStep");
static_assert(offsetof(dmTimeStep, clearForces) == 0x3a, "dmTimeStep");
static_assert(sizeof(dmPosition) == 0x20, "dmPosition");
static_assert(offsetof(dmPosition, x) == 0x0, "dmPosition");
static_assert(offsetof(dmPosition, q) == 0x10, "dmPosition");
static_assert(sizeof(dmVelocity) == 0x20, "dmVelocity");
static_assert(offsetof(dmVelocity, v) == 0x0, "dmVelocity");
static_assert(offsetof(dmVelocity, w) == 0x10, "dmVelocity");
static_assert(sizeof(dmSolverData) == 0x70, "dmSolverData");
static_assert(offsetof(dmSolverData, step) == 0x0, "dmSolverData");
static_assert(offsetof(dmSolverData, aPositions) == 0x40, "dmSolverData");
static_assert(offsetof(dmSolverData, aVelocities) == 0x48, "dmSolverData");
static_assert(offsetof(dmSolverData, aInvIs) == 0x50, "dmSolverData");
static_assert(offsetof(dmSolverData, pBodyArrayPtr) == 0x58, "dmSolverData");
static_assert(offsetof(dmSolverData, pContactArrayPtr) == 0x60, "dmSolverData");
