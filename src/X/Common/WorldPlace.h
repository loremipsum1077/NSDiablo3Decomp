// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/WorldPlace.h

#pragma once

#include "CommonTypes.h"
#include "../Sigma/Math/Vector.h"

struct WorldPlace {  // size 0x10
    Vector3D wp;  // 0x00
    SWorldID idSWorld;  // 0x0c
};
static_assert(sizeof(WorldPlace) == 0x10, "WorldPlace");

static const WorldPlace cgplaceNull = { Vector3D(0.0f, 0.0f, 0.0f), SWORLDID_INVALID };  // line 23

WorldDistance WorldPlaceDistanceSquared(const WorldPlace & tPlace1, const WorldPlace & tPlace2);
BOOL WorldPlaceInSameWorld(const WorldPlace & tPlace1, const WorldPlace & tPlace2);


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  WorldPlace::WorldPlace(const WorldPlace &)  [inlined into 126 functions]
//   line    17  WorldPlace & WorldPlace::operator=(const WorldPlace &)  [inlined into 333 functions]
//   line    23  static void __cxx_global_var_init.1()  [inline]
//   line    23  static void __cxx_global_var_init.2()  [inline]

// Variables defined in this file:
//   line    23  static const WorldPlace cgplaceNull

WorldDistance WorldPlaceDistance(const WorldPlace & tPlace1, const WorldPlace & tPlace2);
WorldDistance WorldPlaceXYPlaneDistance(const WorldPlace & tPlace1, const WorldPlace & tPlace2);  // WorldPlace.cpp:54
void WorldPlaceGetVectorBetween(const WorldPlace & tPlace1, const WorldPlace & tPlace2, Vector3D & wvBetween);  // WorldPlace.cpp:90
void WorldPlaceOffset(const WorldPlace & placeStart, const Vector3D & wvBetween, WorldPlace & placeResult);  // WorldPlace.cpp:102
