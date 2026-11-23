// Layout check for X/Common/DRLGTypes.h against the DWARF. Compiled, not compared.
#include "X/Common/DRLGTypes.h"

#include <cstddef>

#define CHECK(T, m, off) static_assert(offsetof(T, m) == off, #T "::" #m)

static_assert(sizeof(DRLGPlacedScene) == 0x168, "DRLGPlacedScene");
CHECK(DRLGPlacedScene, wp, 0xdc);
CHECK(DRLGPlacedScene, snoname, 0xe8);
CHECK(DRLGPlacedScene, nTileType, 0xf0);
CHECK(DRLGPlacedScene, nTileSet, 0xf4);
CHECK(DRLGPlacedScene, posParentScene, 0xf8);
CHECK(DRLGPlacedScene, snoSceneGroup, 0x100);
CHECK(DRLGPlacedScene, idSScene, 0x104);
CHECK(DRLGPlacedScene, snoMarkerSetSrc, 0x108);
CHECK(DRLGPlacedScene, nMarkerSetSrcIndex, 0x10c);
CHECK(DRLGPlacedScene, nSpecialTypesPlaced, 0x110);
CHECK(DRLGPlacedScene, listAppliedLabels, 0x118);
CHECK(DRLGPlacedScene, listPotentialLabels, 0x140);
