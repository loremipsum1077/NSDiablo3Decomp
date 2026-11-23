// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavMeshInternal.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/Math/MathGeneral.h"

typedef int32 Team;

struct NavMeshSquare {  // size 0x8
    WorldDistance wdSquareZ;  // 0x00
    uint32 dwSquareFlags;  // 0x04
};
static_assert(sizeof(NavMeshSquare) == 0x8, "NavMeshSquare");

struct CompNavMeshSquare {  // size 0x4
    uint16 wdSquareZ;  // 0x00
    uint16 dwSquareFlags;  // 0x02
};
static_assert(sizeof(CompNavMeshSquare) == 0x4, "CompNavMeshSquare");

struct NavMeshConnectedSquare {  // size 0x20
    NavMeshSquare tSquare;  // 0x00
    int32 nSquareIndices[4];  // 0x08
    int32 nSquareX;  // 0x18
    int32 nSquareY;  // 0x1c
};
static_assert(sizeof(NavMeshConnectedSquare) == 0x20, "NavMeshConnectedSquare");

