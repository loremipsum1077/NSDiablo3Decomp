// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Emitter.h
// Subset: the emitter type enum and the Emitter struct (StructureOctree.h is not included).

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Definitions/ParticleDefinition.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"

enum {
    EMIT_UNDEFINED = 0,
    EMIT_POINT = 1,
    EMIT_LINESEGMENT_DEPRECATED = 2,
    EMIT_TRIANGLE_DEPRECATED = 3,
    EMIT_SPHERE_SHELL = 4,
    EMIT_CYLINDER_SHELL = 5,
    EMIT_ACTOR_SUBOBJECT_SHELL = 6,
    EMIT_SCENE_SUBOBJECT_SHELL = 7,
    EMIT_BOX_SHELL = 8,
    EMIT_EVEN_CYLINDER_SHELL = 9,
    EMIT_DOME_SHELL = 10,
    EMIT_ACTOR_SUBOBJECT_SHELL_SEQUENTIAL = 11,
};

struct Emitter {  // size 0x50
    EmitterType eType;  // 0x00
    Vector3D ptPoint;  // 0x04
    WorldDistance wdRadiusMin;  // 0x10
    WorldDistance wdRadiusDelta;  // 0x14
    WorldDistance wdHeightMin;  // 0x18
    WorldDistance wdHeightDelta;  // 0x1c
    Vector3D wvMinBoxExtent;  // 0x20
    Vector3D wvMaxBoxExtent;  // 0x2c
    DataID id;  // 0x38
    uint32 dwSubObjectIndex;  // 0x3c
    uint32 dwTriIndex;  // 0x40
    int32 nSpawnCount;  // 0x44
    int32 nCurCount;  // 0x48
    Angle angleEvenOffset;  // 0x4c
};
static_assert(sizeof(Emitter) == 0x50, "Emitter");
