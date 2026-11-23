// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/StructureOctree.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/Octree.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Ray.h"
#include "../Sigma/Math/Triangle.h"
#include "../Sigma/Math/Vector.h"

struct AppearanceDefinition;
struct EnvAppData;
class XSphereNode;

enum IntersectFlags {
    INTERSECTFLAGS_IGNORE_NOT_WALKABLE = 0,
    INTERSECTFLAGS_IGNORE_NON_COLLIDABLE_BIT = 1,
    INTERSECTFLAGS_NO_CULL = 2,
    INTERSECTFLAGS_IGNORE_NOT_RENDERED = 3,
    INTERSECTFLAGS_IGNORE_NON_LIQUID_SURFACES_BIT = 4,
    INTERSECTFLAGS_LIGHT_MAPPING_BIT = 5,
    INTERSECTFLAGS_IGNORE_FADE_SURFACES_BIT = 6,
    INTERSECTFLAGS_IGNORE_SURFACE_LOOKUP_BIT = 7,
    INTERSECTFLAGS_ASYNC_SURFACE_LOOKUP_BIT = 8,
    INTERSECTFLAGS_INCLUDE_HOLODECK_SCENES_BIT = 9,
};

typedef XListMemoryPool<EnvAppData> TEnvAppDataPool;

typedef int32 DataID;

typedef XListMemoryPool<int> DataIDMemoryPool;

typedef XListMemoryPool<const XSphereNode *> SphereNodeMemoryPool;

struct StructureOctree {  // size 0x10
    Octree * ptOctree;  // 0x00
    AppearanceDefinition * ptAppearance;  // 0x08
};
static_assert(sizeof(StructureOctree) == 0x10, "StructureOctree");

// The inline StructureIntersectData(SWorldID) constructor (line 54) has no body in the DWARF; it is
// written from its inlined copy in IntersectGround (X/Client/Selection.cpp:1127). The parameter name
// is not recorded.
struct StructureIntersectData {  // size 0xa0
    StructureIntersectData(SWorldID idSWorldIn) {
        SigmaMemoryClear(this, sizeof(*this));
        idSWorld = idSWorldIn;
        idSScene = -1;
        wpIntersect = cgwpNull;
        wdIntersect = 3.4028235e+38f;
        wvNormal = cgwvNull;
        snoAppearance = -1; nSubobject = -1; nSegment = -1; nIndexListIndex = -1; idACDHit = ACDID_INVALID;
    }
    SWorldID idSWorld;  // 0x00
    LineSeg3D wline;  // 0x04
    uint32 dwIntersectFlags;  // 0x20
    SSceneID idSScene;  // 0x24
    TEnvAppDataPool * ptEnvAppDataPool;  // 0x28
    DataIDMemoryPool * ptDataIDPool;  // 0x30
    SphereNodeMemoryPool * ptSphereNodePool;  // 0x38
    Vector3D wpIntersect;  // 0x40
    WorldDistance wdIntersect;  // 0x4c
    Vector3D wvNormal;  // 0x50
    OctreePrimitive tPrimitive;  // 0x5c
    Triangle3D wtriIntersect;  // 0x64
    SNO snoAppearance;  // 0x88
    int32 nSubobject;  // 0x8c
    int32 nSegment;  // 0x90
    int32 nIndexListIndex;  // 0x94
    ACDID idACDHit;  // 0x98
};
static_assert(sizeof(StructureIntersectData) == 0xa0, "StructureIntersectData");

void StructureOctreeGetIndexFromUserData(uint32 uUserData, int32 & nIndex0, int32 & nIndex1);
BOOL StructureOctreeIntersectLineSeg(const StructureOctree & tStructOctree, StructureIntersectData & tStructIntersect);
