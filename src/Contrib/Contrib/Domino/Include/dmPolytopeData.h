#pragma once
#include "dmMath.h"
#include "dmSerialize.h"

// Original dmPolytopeData.h:9. Other polytope-data operations remain unadopted.
struct dmSubEdge
{
    dm_int8 twinOffset;
    dm_uint8 tail;
    dm_uint8 left;
    dm_uint8 next; // Inference: defining DIE 07de9320 inherits header line 27 inside this class; twinOffset arithmetic is inlined in FindEdge.
    dmSubEdge* GetTwin() { return this + twinOffset; } // Original dmPolytopeData.h:27.
    const dmSubEdge* GetTwin() const { return this + twinOffset; } // Original dmPolytopeData.h:28 (inlined into TestEdgePair).
};
static_assert(sizeof(dmSubEdge) == 4, "dmSubEdge");

// dmPolytopeData.h:19, layout from the DWARF; only the members DominoClient.cpp
// uses (the out-of-line constructor, destructor and SetAsBox of
// Collision/Shapes/dmPolytopeBuilder.cpp) are relied on.
struct dmPolytopeData {  // size 0x40
    enum {
        e_maxVertices = 255,
        e_maxFaces = 256,
        e_maxEdges = 128,
    };
    dmPolytopeData();
    ~dmPolytopeData();
    void Initialize();
    void Destroy();
    bool SetAsHull(const dm_vec4 * pPoints, dm_int32 count, dm_float32 weldTolerance, dm_float32 edgeTolerance, dm_float32 faceTolerance);
    void SetAsBox(dm_vec4Arg);
    void SetAsBox(const dmTransform & xf, dm_vec4Arg extents);
    void SetAsTetrahedron(dm_float32);
    void SetAsCylinder(dm_vec4Arg, dm_vec4Arg, dm_float32);
    void Transform(const dmTransform &, dm_vec4Arg);
    bool IsValid() const;
    dmFloat3 * m_aVertices;  // 0x00
    dmPlane * m_aPlanes;  // 0x08
    dmSubEdge * m_aSubEdges;  // 0x10
    dm_uint8 * m_aFaceSubEdges;  // 0x18
    dmFloat3 m_centroid;  // 0x20
    dm_int32 m_vertexCount;  // 0x2c
    dm_int32 m_faceCount;  // 0x30
    dm_int32 m_subEdgeCount;  // 0x34
    dm_float32 m_volume;  // 0x38
    dm_float32 m_area;  // 0x3c
private:
    void ComputeEdges(const dm_int32 * faceVertexCounts, const dm_int32 * pFaceVertexIndices);
    void ComputeCentroid();
    dm_int32 GetTwinIndex(dm_int32 edgeIndex) const;
    dm_int32 GetNextEdgeIndex(dm_int32 edgeIndex) const;
    dm_int32 GetTailIndex(dm_int32 edgeIndex) const;
    dm_vec4 GetTailVertex(dm_int32 edgeIndex) const;
};
static_assert(sizeof(dmPolytopeData) == 0x40, "dmPolytopeData");
