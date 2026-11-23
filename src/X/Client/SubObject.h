// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/SubObject.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Math/Vector.h"

struct VertexOwnerData {  // size 0x188
    int32 nFaceCount;  // 0x00
    int32 pnFaceIndices[96];  // 0x04
    BOOL fIsEdge;  // 0x184
};
static_assert(sizeof(VertexOwnerData) == 0x188, "VertexOwnerData");

struct SubObject;

// Declared here from their definitions in SubObject.cpp (:1350 and :1400).
void SubObjectGetSegmentTriangle(const SubObject & tSubObject, int32 nSegmentIndex, int32 nIndexListIndex, uint16 & wIndex0, uint16 & wIndex1, uint16 & wIndex2);
void SubObjectGetSegmentVertexPos(const SubObject & tSubObject, int32 nSegmentIndex, uint16 wVertexIndex, Vector3D & wpPos);
struct Vector2D;
struct RGBAColor;
struct VertInfluences;
void SubObjectPostprocessDisposeFaceAdjacency(SubObject & tSubObject);  // :1105
int32 SubObjectGetNumTriangles(const SubObject & tSubObject);  // :1124
void SubObjectGetTriangleSegment(const SubObject & tSubObject, const int32 nTriangleIndex, int32 & nSegmentIndex, int32 & nIndexListIndex);  // :1161
int32 SubObjectGetNumSegments(const SubObject & tSubObject);  // :1217
int32 SubObjectGetNumSegmentTriangles(const SubObject & tSubObject, int32 nSegmentIndex);  // :1236
int32 SubObjectGetNumSegmentVertices(const SubObject & tSubObject, int32 nSegmentIndex);  // :1274
void SubObjectGetSegmentIndexListWalkInfo(const SubObject & tSubObject, int32 nSegmentIndex, int32 & nIndexListIndexCount, int32 & nIndexListIndexIncrement);  // :1304
void SubObjectGetSegmentVertexUV(const SubObject & tSubObject, int32 nSegmentIndex, uint16 wVertexIndex, int32 nUVSetIndex, Vector2D & vecUV);  // :1460
void SubObjectGetSegmentVertexColor(const SubObject & tSubObject, int32 nSegmentIndex, uint16 wVertexIndex, int32 nColorSetIndex, RGBAColor & rgbaColor);  // :1550
BOOL SubObjectHasVertexInfluences(const SubObject & tSubObject);  // :1645
void SubObjectGetSegmentVertexInfluences(const SubObject & tSubObject, int32 nSegmentIndex, uint16 wVertexIndex, VertInfluences & tInfluences);  // :1672
