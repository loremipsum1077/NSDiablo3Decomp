// Layout check for X/Common/DetPaths.h against the DWARF. Compiled, not compared.
#include "X/Common/DetPaths.h"

#include <cstddef>

#define CHECK(T, m, off) static_assert(offsetof(T, m) == off, #T "::" #m)

static_assert(sizeof(DPathSinData) == 0x1c, "DPathSinData");
CHECK(DPathSinData, flSpeed, 0x14);
CHECK(DPathSinData, annOwner, 0x18);
static_assert(sizeof(DPathPieData) == 0x18, "DPathPieData");
CHECK(DPathPieData, flStraightSpeed, 0x0c);
CHECK(DPathPieData, flArcSpeed, 0x10);
CHECK(DPathPieData, annOwner, 0x14);
static_assert(sizeof(DeterministicPathData) == 0x78, "DeterministicPathData");
CHECK(DeterministicPathData, tSeed, 0x04);
CHECK(DeterministicPathData, aOriginalYaw, 0x0c);
CHECK(DeterministicPathData, vStartPos, 0x10);
CHECK(DeterministicPathData, vTargetPos, 0x1c);
CHECK(DeterministicPathData, nVar0, 0x28);
CHECK(DeterministicPathData, flVar4, 0x3c);
CHECK(DeterministicPathData, tSinData, 0x40);
CHECK(DeterministicPathData, tPieData, 0x5c);
CHECK(DeterministicPathData, flSpeedMult, 0x74);
