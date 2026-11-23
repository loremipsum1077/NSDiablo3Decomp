// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Math/ConvexHull.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Files/XSerializeData.h"
#include "Matrix.h"
#include "Vector.h"

struct PRTransform;
struct Plane;

struct IndexedTriangle {  // size 0xc
    int32 a;  // 0x00
    int32 b;  // 0x04
    int32 c;  // 0x08
};
static_assert(sizeof(IndexedTriangle) == 0xc, "IndexedTriangle");

struct ConvexHull {  // size 0x80
    int32 nVertexCount;  // 0x00
    SerializeData serVertices;  // 0x04
    Vector3D * ptVertices;  // 0x10
    int32 nTriangleCount;  // 0x18
    SerializeData serTriangles;  // 0x1c
    IndexedTriangle * ptTriangles;  // 0x28
    int32 nPlaneCount;  // 0x30
    SerializeData serPlanes;  // 0x34
    Plane * ptPlanes;  // 0x40
    Matrix3x3 mInertiaTensor;  // 0x48
    Vector3D wvCenterOfMass;  // 0x6c
    float flArea;  // 0x78
    float flVolume;  // 0x7c
    float SubmergedVolume(Vector3D *, const Plane &, const PRTransform &, float) const;
    void PreCompute();
};
static_assert(sizeof(ConvexHull) == 0x80, "ConvexHull");

class XTypeDescriptor;

// Defined in ConvexHull.cpp (lines 20 and 36).
extern const XTypeDescriptor * DT_INDEXEDTRIANGLE;
extern const XTypeDescriptor * DT_CONVEXHULL;


// Functions defined in this file (bodies are not in the DWARF):
//   line    33  ConvexHull::ConvexHull(ConvexHull &&)  [inlined into 1 functions]
//   line    33  ConvexHull & ConvexHull::operator=(const ConvexHull &)  [inlined into 1 functions]
