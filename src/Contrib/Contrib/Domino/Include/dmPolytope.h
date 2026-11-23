// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmPolytope.h

#pragma once

#include "dmMath.h"
#include "dmSerialize.h"
#include "dmShapes.h"

struct dmAABB;
struct dmPolytopeData;
class dmPoolAllocator;
struct dmRayCastInput;
struct dmRayCastOutput;
struct dmSubEdge;

class dmPolytope : public dmShape {  // size 0x70
public:
    dmPolytope();
    virtual ~dmPolytope();  // vtable slot 0
    void Create(const dmPolytopeData * pData, const dmTransform & xf, dm_float32 scale, dmPoolAllocator * pAllocator);
    void Create(const dmPolytopeData *, dmPoolAllocator *);
    void SetAsTriangle(dm_vec4 * aVertices, dm_vec4 * aPlaneBuffer, dmSubEdge * aSubEdges, dm_uint8 * aFaceSubEdges);
    virtual void ComputeMass(dmMassData * pMassData, dm_float32 density) const;  // vtable slot 5. Inference: defining DIE 07deb68d inherits class header line 72; the body loads m_centroid.
    virtual dm_vec4 ComputeCentroid() const { return m_centroid; } // Original dmPolytope.h:72; vtable slot 6.
    virtual dm_vec4 ComputeSurfaceArea() const;  // vtable slot 7
    virtual dm_vec4 ComputeVolume() const;  // vtable slot 8
    virtual dm_vec4 ComputeProjectedSurfaceArea(dm_vec4Arg dir) const;  // vtable slot 9
    virtual dm_vec4 ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & xf, dm_vec4Arg normal, dm_vec4Arg offset) const;  // vtable slot 10
    virtual dmAABB GetAABB(const dmMtx & xf) const;  // vtable slot 2
    virtual dmAABB GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const;  // vtable slot 3
    virtual bool RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const;  // vtable slot 4
    virtual bool TestOverlap(const dmSphere & sphere) const;  // vtable slot 11
    virtual void ApplyScale(dm_vec4Arg scale);  // vtable slot 12
    dm_float32 ComputeMinRadius() const;
    dm_float32 ComputeMaxRadius() const;
    dm_float32 GetArea() const;
    dm_float32 GetVolume() const;
    dm_vec4 GetCentroid() const;
    dm_int32 GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const;
    dm_int32 GetSupportFace(dm_vec4Arg direction, dm_int32 vertexIndex) const;
    dm_int32 GetVertexCount() const;
    dm_int32 GetFaceCount() const;
    dm_int32 GetEdgeCount() const;
    dm_int32 GetSubEdgeCount() const;
    dm_vec4 GetVertex(dm_int32 index) const;
    dm_vec4 GetFace(dm_int32 index) const;
    const dmSubEdge * GetSubEdge(dm_int32 index) const;
    const dm_vec4 * GetVertices() const;
    const dm_vec4 * GetFaces() const;
    const dmSubEdge * GetSubEdges() const;
    const dm_uint8 * GetFaceSubEdges() const;
    dm_vec4 GetEdgeVector(dm_int32 index) const;
    dm_int32 GetPrimaryEdge(dm_int32 edgeIndex) const;
    dm_int32 GetTwinIndex(dm_int32 edgeIndex) const;
    dm_int32 GetNextEdge(dm_int32 edgeIndex) const;
    dm_int32 GetFaceEdge(dm_int32 faceIndex) const;
    dm_vec4 GetTailVertex(dm_int32 edgeIndex) const;
    dm_vec4 GetLeftFace(dm_int32) const;
    bool DetermineFeature(dmFeature *, const dm_int32 *, dm_int32) const;
    dm_int32 FindEdge(dm_int32 i1, dm_int32 i2) const;
    void Dump() const;
protected:
    dm_vec4 m_centroid;  // 0x10
    dm_vec4 * m_aVertices;  // 0x20
    dm_vec4 * m_aPlanes;  // 0x28
    dm_uint8 * m_aFaceSubEdges;  // 0x30
    dmSubEdge * m_aSubEdges;  // 0x38
    dm_int32 m_vertexCount;  // 0x40
    dm_int32 m_faceCount;  // 0x44
    dm_int32 m_subEdgeCount;  // 0x48
    dm_float32 m_volume;  // 0x4c
    dm_float32 m_area;  // 0x50
    dmPoolAllocator * m_pAllocator;  // 0x58
    bool m_isTriangle;  // 0x60
};
static_assert(sizeof(dmPolytope) == 0x70, "dmPolytope");

struct dmPartialPolytope {  // size 0x90
    void Create(const dmTriangle * pTri);
    bool CheckAxis(dm_vec4Arg a) const;
    dm_int32 GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const;
    dm_vec4 m_aVertices[3];  // 0x00
    dm_vec4 m_aPlanes[4];  // 0x30
    dm_vec4 m_centroid;  // 0x70
    dm_int32 m_planeCount;  // 0x80
};
static_assert(sizeof(dmPartialPolytope) == 0x90, "dmPartialPolytope");


// Functions defined in this file (bodies are not in the DWARF):
//   line    72  dm_vec4 dmPolytope::ComputeCentroid() const  [0x7100bf73e0]
//   line   122  dm_vec4 dmPolytope::GetCentroid() const  [inlined into 6 functions]
//   line   134  dm_int32 dmPolytope::GetVertexCount() const  [inlined into 1 functions]
//   line   139  dm_int32 dmPolytope::GetFaceCount() const  [inlined into 4 functions]
//   line   149  dm_int32 dmPolytope::GetSubEdgeCount() const  [inlined into 3 functions]
//   line   155  dm_vec4 dmPolytope::GetVertex(dm_int32 index) const  [inlined into 3 functions]
//   line   162  dm_vec4 dmPolytope::GetFace(dm_int32 index) const  [inlined into 10 functions]
//   line   169  const dmSubEdge * dmPolytope::GetSubEdge(dm_int32 index) const  [inlined into 6 functions]
//   line   176  const dm_vec4 * dmPolytope::GetVertices() const  [inlined into 1 functions]
//   line   201  dm_vec4 dmPolytope::GetEdgeVector(dm_int32 index) const  [inlined into 4 functions]
//   line   215  dm_int32 dmPolytope::GetPrimaryEdge(dm_int32 edgeIndex) const  [inlined into 4 functions]
//   line   223  dm_int32 dmPolytope::GetTwinIndex(dm_int32 edgeIndex) const  [inlined into 3 functions]
//   line   231  dm_int32 dmPolytope::GetNextEdge(dm_int32 edgeIndex) const  [inlined into 7 functions]
//   line   238  dm_int32 dmPolytope::GetFaceEdge(dm_int32 faceIndex) const  [inlined into 7 functions]
//   line   245  dm_vec4 dmPolytope::GetTailVertex(dm_int32 edgeIndex) const  [inlined into 10 functions]

inline dm_int32 dmPolytope::GetVertexCount() const { return m_vertexCount; }
inline const dm_vec4 * dmPolytope::GetVertices() const { return m_aVertices; }
inline dm_int32 dmPolytope::GetFaceCount() const { return m_faceCount; }
inline dm_vec4 dmPolytope::GetFace(dm_int32 index) const { return m_aPlanes[index]; }

// dmPolytope.h:223-251, written from dmTPCollider::CollideEdges. The asserts
// record the original header path and line, as dmBuffer.h does.
#include "dmPolytopeData.h"

#line 223 "../../..\\Domino/Include/dmPolytope.h"
inline dm_int32 dmPolytope::GetTwinIndex(dm_int32 edgeIndex) const
{

    dm_int32 twinOffset = m_aSubEdges[edgeIndex].twinOffset;

    return edgeIndex + twinOffset;
}

#line 245 "../../..\\Domino/Include/dmPolytope.h"
inline dm_vec4 dmPolytope::GetTailVertex(dm_int32 edgeIndex) const
{
    dmAssert(0 <= edgeIndex && edgeIndex < m_subEdgeCount);
    dm_int32 tail = m_aSubEdges[edgeIndex].tail;
    dmAssert(tail < m_vertexCount);
    return m_aVertices[tail];
}

// dmPolytope.h:122-241, the remaining accessor bodies (DW_INL_inlined only, no
// concrete DIE). Body lines and columns from the inlined rows in
// dmTPManifold.cpp/dmPPManifold.cpp; assert strings at 0x7100e35e1b/0x7100e35e6a.
#line 122 "../../..\\Domino/Include/dmPolytope.h"
inline dm_vec4 dmPolytope::GetCentroid() const
{
    return m_centroid;
}

#line 149 "../../..\\Domino/Include/dmPolytope.h"
inline dm_int32 dmPolytope::GetSubEdgeCount() const
{
    return m_subEdgeCount;
}

#line 155 "../../..\\Domino/Include/dmPolytope.h"
inline dm_vec4 dmPolytope::GetVertex(dm_int32 index) const
{

    return m_aVertices[index];
}

#line 169 "../../..\\Domino/Include/dmPolytope.h"
inline const dmSubEdge * dmPolytope::GetSubEdge(dm_int32 index) const
{

    return m_aSubEdges + index;
}

#line 201 "../../..\\Domino/Include/dmPolytope.h"
inline dm_vec4 dmPolytope::GetEdgeVector(dm_int32 index) const
{


    const dmSubEdge * pEdge = m_aSubEdges + index;
    const dmSubEdge * pTwin = pEdge + pEdge->twinOffset;




    return dmSub(m_aVertices[pTwin->tail], m_aVertices[pEdge->tail]);
}

#line 215 "../../..\\Domino/Include/dmPolytope.h"
inline dm_int32 dmPolytope::GetPrimaryEdge(dm_int32 edgeIndex) const
{
    dmAssert(0 <= edgeIndex && edgeIndex < m_subEdgeCount);
    return edgeIndex & 0xfffe;
}

#line 231 "../../..\\Domino/Include/dmPolytope.h"
inline dm_int32 dmPolytope::GetNextEdge(dm_int32 edgeIndex) const
{
    dmAssert(0 <= edgeIndex && edgeIndex < m_subEdgeCount);
    return m_aSubEdges[edgeIndex].next;
}

#line 238 "../../..\\Domino/Include/dmPolytope.h"
inline dm_int32 dmPolytope::GetFaceEdge(dm_int32 faceIndex) const
{
    dmAssert(0 <= faceIndex && faceIndex < m_faceCount);
    return m_aFaceSubEdges[faceIndex];
}
