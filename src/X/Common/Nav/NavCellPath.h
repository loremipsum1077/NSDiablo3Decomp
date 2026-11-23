// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavCellPath.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "NavCellHandle.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Math/Vector.h"

typedef XListMemoryPool<Vector2D> TPathPoint2DPool;

class PathPoint2DList : public XPooledList<Vector2D, const Vector2D &> {  // size 0x28
};
static_assert(sizeof(PathPoint2DList) == 0x28, "PathPoint2DList");

enum {
    PSF_USE_SAFE_BORDER = 0,
};

struct NavCellPathFindData {  // size 0x48
    uint16 uOneRequiredFlags;  // 0x00
    uint16 uMaxNodesToExplore;  // 0x02
    NavCellHandle hNavCellStart;  // 0x04
    Vector3D wpPosStart;  // 0x0c
    NavCellHandle hNavCellEnd;  // 0x18
    Vector3D wpNavCellEnd;  // 0x20
    float flMaxRangeFromDest;  // 0x2c
    uint32 uPathSmoothingFlags;  // 0x30
    uint32 uPathSmoothingIterations;  // 0x34
    PathPoint2DList * ptListPathPoints;  // 0x38
    uint16 uNodesExplored;  // 0x40
};
static_assert(sizeof(NavCellPathFindData) == 0x48, "NavCellPathFindData");

void NavCellPathFindDataInitialize(NavCellPathFindData & tPathFindData);  // NavCellPath.cpp:71
void NavCellPathInitializeForGame();  // NavCellPath.cpp:84
void NavCellPathDisposeForGame();  // NavCellPath.cpp:109
TPathPoint2DPool * NavCellPathGetPointPool();  // NavCellPath.cpp:129
BOOL NavCellFindPath(NavCellPathFindData & tPathFindData);  // NavCellPath.cpp:138
