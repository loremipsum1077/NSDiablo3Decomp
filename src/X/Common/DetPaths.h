// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/DetPaths.h
// Deterministic-path data structs on the DWARF layouts; the copy ctors and assignments the DWARF lists are the implicit ones.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Random.h"

enum {
    DPATH_NONE = 0,
    DPATH_CHARGED_BOLT = 1,
    DPATH_PLAGUE_OF_TOADS = 2,
    DPATH_CHAKRAM_LEFT = 3,
    DPATH_CHAKRAM_RIGHT = 4,
    DPATH_SIN_WAVE = 5,
    DPATH_PEDAL_CURVE = 6,
    DPATH_SPIRAL = 7,
    DPATH_PLAGUE_OF_TOADS_STRAIGHT = 8,
    DPATH_PIE_WEDGE = 9,
    DPATH_CURVED_OFFSET = 10,
    DPATH_FIRESTORM = 11,
    DPATH_SPIRAL_IMMEDIATE = 12,
    DPATH_COUNT = 13,
};

typedef int32 DeterministicPathType;

struct DPathSinData {  // size 0x1c
    float flSinIncPerTick;  // 0x00
    float flSinIncAccel;  // 0x04
    float flLateralMaxDistance;  // 0x08
    float flOOLateralDistanceToScale;  // 0x0c
    float flLateralStartDistance;  // 0x10
    float flSpeed;  // 0x14
    ACDNetworkName annOwner;  // 0x18
};
static_assert(sizeof(DPathSinData) == 0x1c, "DPathSinData");

struct DPathPieData {  // size 0x18
    Vector3D vTargetPos2;  // 0x00
    float flStraightSpeed;  // 0x0c
    float flArcSpeed;  // 0x10
    ACDNetworkName annOwner;  // 0x14
};
static_assert(sizeof(DPathPieData) == 0x18, "DPathPieData");

struct DeterministicPathData {  // size 0x78
    DeterministicPathType eDPath;  // 0x00
    RNG tSeed;  // 0x04
    Angle aOriginalYaw;  // 0x0c
    Vector3D vStartPos;  // 0x10
    Vector3D vTargetPos;  // 0x1c
    int32 nVar0;  // 0x28
    float flVar0;  // 0x2c
    float flVar1;  // 0x30
    float flVar2;  // 0x34
    float flVar3;  // 0x38
    float flVar4;  // 0x3c
    DPathSinData tSinData;  // 0x40
    DPathPieData tPieData;  // 0x5c
    float flSpeedMult;  // 0x74
};
static_assert(sizeof(DeterministicPathData) == 0x78, "DeterministicPathData");

typedef int32 DeterministicPathType;

