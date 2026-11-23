// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavZoneIntersect.h

#pragma once

#include "NavCellHandle.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

struct NavZoneInstance;

struct NavCellCollideResult {  // size 0x30
    BOOL bCollided;  // 0x00
    float flT;  // 0x04
    Vector2D wvNormal;  // 0x08
    Vector2D wpCollision;  // 0x10
    NavCellHandle hNavCellCollision;  // 0x18
    Vector2D wpEnd;  // 0x20
    NavCellHandle hNavCellEnd;  // 0x28
};
static_assert(sizeof(NavCellCollideResult) == 0x30, "NavCellCollideResult");

enum {
    NAV_ZONE_COLL_ROUNDED_CORNERS = 0,
    NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY = 1,
    NAV_ZONE_COLL_IGNORE_FLAGS = 2,
    NAV_ZONE_COLL_IGNORE_INVALID_SCENES = 3,
    NAV_ZONE_COLL_IGNORE_REQUIRED_FLAG = 4,
};

void NavZoneCellQueryAABB(const NavZoneInstance * ptZoneInstance, const Vector2D & wpBoxMin, const Vector2D & wpBoxMax, uint16 uOneRequiredFlags, uint16 uPreventFlags, uint32 uCollFlags, uint32 * arNavCells, uint32 & uNavCellCount, BOOL & bHitChild);
