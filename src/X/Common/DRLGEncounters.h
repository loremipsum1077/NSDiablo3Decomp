// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/DRLGEncounters.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Definitions/WorldDefinition.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XTemplate.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"

struct DRLGSceneGroup {  // size 0x108
    SceneSpecification tSceneSpec;  // 0x00
    SNO snoSceneGroup;  // 0xdc
    uint32 dwSceneGroupFlags;  // 0xe0
    Vector3D wp;  // 0xe4
    POS posParentScene;  // 0xf0
    SNO snoPlacedScene;  // 0xf8
    SNO snoMarkerSet;  // 0xfc
    int32 nMarkerIndex;  // 0x100
    int32 nLevelAreaIndex;  // 0x104
};
static_assert(sizeof(DRLGSceneGroup) == 0x108, "DRLGSceneGroup");

typedef XListMemoryPool<DRLGSceneGroup> TDRLGSceneGroupPool;

typedef XPooledList<DRLGSceneGroup, const DRLGSceneGroup &> TDRLGSceneGroupList;

struct DRLGGizmoPlace {  // size 0x2c
    SSceneID idSScene;  // 0x00
    PRTransform transform;  // 0x04
    SNO snoLevelArea;  // 0x20
    SNO snoMarkerSet;  // 0x24
    int32 nMarkerIndex;  // 0x28
};
static_assert(sizeof(DRLGGizmoPlace) == 0x2c, "DRLGGizmoPlace");

typedef XListMemoryPool<DRLGGizmoPlace> TDRLGGizmoPlacePool;

typedef XPooledList<DRLGGizmoPlace, const DRLGGizmoPlace &> TDRLGGizmoPlaceList;


// Functions defined in this file (bodies are not in the DWARF):
//   line    43  DRLGGizmoPlace & DRLGGizmoPlace::operator=(const DRLGGizmoPlace &)  [inlined into 2 functions]
