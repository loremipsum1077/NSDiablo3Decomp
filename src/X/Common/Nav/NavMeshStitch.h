// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavMeshStitch.h

#pragma once

#include "../CommonTypes.h"
#include "../../Sigma/Math/Vector.h"
#include "../WorldPlace.h"

struct WorldCommonData;

struct NavMeshStitchAABBLookup {  // size 0x20
    IVector2D tSize;  // 0x00
    Vector2D vGridSize;  // 0x08
    Vector2D wpGridOffset;  // 0x10
    SSceneID * arScenes;  // 0x18
};
static_assert(sizeof(NavMeshStitchAABBLookup) == 0x20, "NavMeshStitchAABBLookup");

SSceneID NavMeshStitchWorldPlaceToScene(WorldCommonData & tWCD, const WorldPlace & tWorldPlace, BOOL bIncludeInactive);
int32 NavMeshStitchScenesInAABB(SWorldID idSWorld, Vector2D wpAABBMin, Vector2D wpAABBMax, SSceneID * arSceneIDs, int32 nMaxSceneIDCount);

