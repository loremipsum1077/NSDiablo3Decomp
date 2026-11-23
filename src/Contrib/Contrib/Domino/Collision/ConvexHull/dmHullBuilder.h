// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Collision/ConvexHull/dmHullBuilder.h
// Blank lines keep the inline helpers on their original lines: the asserts in
// dmHullHalfEdge::Link bake __LINE__ (384, 385) into their callers.
#line 6 "../../..\\Domino/Collision/ConvexHull/dmHullBuilder.h"
#pragma once

#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmSettings.h"

struct dmHullFace;

struct dmHullVertex {  // size 0x40
    dm_vec4 m_point;  // 0x00
    dm_int32 m_index;  // 0x10
    dm_int32 m_originalIndex;  // 0x14
    dmHullVertex * m_pPrev;  // 0x18
    dmHullVertex * m_pNext;  // 0x20
    dmHullFace * m_pConflictFace;  // 0x28
    bool m_redundant;  // 0x30
};
static_assert(sizeof(dmHullVertex) == 0x40, "dmHullVertex");

struct dmHullHalfEdge {  // size 0x28
    void Initialize(dmHullVertex * pTailVertex, dmHullFace * pLeftFace);
    dmHullVertex * GetTail();
    const dmHullVertex * GetTail() const;
    dmHullVertex * GetHead();
    dmHullFace * GetOppositeFace();
    dm_vec4 Length();
    dm_vec4 LengthSquared();
    static void Link(dmHullHalfEdge * pEdge1, dmHullHalfEdge * pEdge2);
    dmHullVertex * m_pVertex;  // 0x00
    dmHullFace * m_pFace;  // 0x08
    dmHullHalfEdge * m_pPrev;  // 0x10
    dmHullHalfEdge * m_pNext;  // 0x18
    dmHullHalfEdge * m_pTwin;  // 0x20
};
static_assert(sizeof(dmHullHalfEdge) == 0x28, "dmHullHalfEdge");

struct dmHullFace {  // size 0x70
    enum State {
        e_normal = 0,
        e_discarded = 1,
        e_visible = 2,
        e_concave = 3,
    };

    void Initialize();
    void Destroy();
    void ComputeGeometry();
    static dmHullFace * CreateTriangle(dmHullVertex * pv1, dmHullVertex * pv2, dmHullVertex * pv3);
















































    dm_vec4 GetSeparation(dm_vec4Arg p)
    {
        return dmSub(dmDot3(m_normal, p), m_planeOffset);
    }
    void Validate();
    dm_vec4 Flatten();
    dmHullHalfEdge * m_pEdge;  // 0x00
    dmHullVertex * m_conflicts;  // 0x08
    dmHullFace * m_pPrev;  // 0x10
    dmHullFace * m_pNext;  // 0x18
    dm_vec4 m_normal;  // 0x20
    dm_vec4 m_point;  // 0x30
    dm_vec4 m_areaMeasure;  // 0x40
    dm_vec4 m_planeOffset;  // 0x50
    dm_int32 m_vertexCount;  // 0x60
    dm_int32 m_index;  // 0x64
    dmHullFace::State m_state;  // 0x68
};
static_assert(sizeof(dmHullFace) == 0x70, "dmHullFace");

struct dmHullInput {  // size 0x10
    const dm_vec4 * aPoints;  // 0x00
    dm_int32 count;  // 0x08
    dm_float32 weldTolerance;  // 0x0c
    void Dump() const;
};
static_assert(sizeof(dmHullInput) == 0x10, "dmHullInput");

class dmHullBuilder {  // size 0xc60
public:
    dmHullBuilder();
    ~dmHullBuilder();
    bool Build(const dmHullInput * pInput);
    bool IsValid();
    void Dump() const;
    friend struct SimpleHull;  // dmSimpleHull.cpp reads m_faceList directly









































    dm_int32 GetVertexCount() const
    {
        return m_vertexCount;
    }

    const dmHullVertex * GetVertex(dm_int32 index) const
    {
        return m_vertices + index;
    }


    dm_int32 GetFaceCount() const
    {
        return m_faceCount;
    }

    const dmHullFace * GetFaceList() const
    {
        return m_faceList;
    }

    dm_vec4 ComputeCentroid() const;
    dm_vec4 GetTolerance() const;
private:
    // The DWARF keeps no enumerators for this one; the name comes from the
    // assert text "m_horizonCount < e_maxHorizonCount".
    enum
    {
        e_maxHorizonCount = 128
    };

    void Clear();
    void CleanVertices(dm_vec4Arg weldTolerance);
    bool CreateInitialSimplex();
    void AddConflictVertex(dmHullVertex * pVertex, dmHullFace * pFace);
    void RemoveConflictVertex(dmHullVertex * pVertex);
    void ResolveOrphanedVertices();
    void TransferConflictVertices(dmHullFace * pFace, dmHullFace * pAbsorbingFace);
    dm_vec4 GetOppositeFaceSeparation(dmHullHalfEdge * pEdge);
    void Link(dmHullFace * pFace1, dm_int32 edge1, dmHullFace * pFace2, dm_int32 edge2);
    void ConnectHalfEdges(dmHullHalfEdge * pEdge, dmHullHalfEdge * pEdgeNext);
    void MergeTwoFaces(dmHullHalfEdge * pEdge);
    void DiscardFace(dmHullFace * pFace);
    bool MergeNonConvexFaces(dmHullFace * pFace, dm_int32 mergeType);
    void SearchForHorizon(dm_vec4Arg apex, dmHullHalfEdge * pEdge0, dmHullFace * pFace);
    void CreateHorizonCone(dmHullVertex * pApex);
    void DestroyVisibleFaces();
    dmHullVertex * GetNextConflictVertex();
    void AddVertexToHull(dmHullVertex * pApex);
    void CullUnusedVertices();
    void RecordDebugData(dmHullVertex *);
    dmHullVertex * m_vertices;  // 0x00
    dm_int32 m_vertexCount;  // 0x08
    dmHullFace * m_faceList;  // 0x10
    dm_int32 m_faceCount;  // 0x18
    dmHullHalfEdge * m_horizonEdges[e_maxHorizonCount];  // 0x20
    dm_int32 m_horizonCount;  // 0x420
    dmHullFace * m_newFaces[e_maxHorizonCount];  // 0x428
    dmHullFace * m_visibleFaces[e_maxHorizonCount];  // 0x828
    dm_int32 m_visibleCount;  // 0xc28
    dmHullFace * m_discardList;  // 0xc30
    dmHullVertex * m_conflictList;  // 0xc38
    dmHullVertex * m_orphanList;  // 0xc40
    dm_vec4 m_tolerance;  // 0xc50
};
static_assert(sizeof(dmHullBuilder) == 0xc60, "dmHullBuilder");

struct dmHullDebugData {  // size 0x28210
    dm_vec4 normals[1024];  // 0x00
    dm_vec4 facePoints[1024];  // 0x4000
    dm_int32 faces[1024][32];  // 0x8000
    dm_int32 horizon[128];  // 0x28000
    dm_int32 apex;  // 0x28200
    dm_int32 faceCount;  // 0x28204
};
static_assert(sizeof(dmHullDebugData) == 0x28210, "dmHullDebugData");










































































inline void dmHullHalfEdge::Initialize(dmHullVertex * pTailVertex, dmHullFace * pLeftFace)
{
    m_pVertex = pTailVertex;
    m_pFace = pLeftFace;

    m_pPrev = NULL;
    m_pNext = NULL;
    m_pTwin = NULL;
}

inline dmHullVertex * dmHullHalfEdge::GetTail()
{
    return m_pVertex;
}

inline const dmHullVertex * dmHullHalfEdge::GetTail() const
{
    return m_pVertex;
}



inline dmHullVertex * dmHullHalfEdge::GetHead()
{
    return m_pNext->m_pVertex;
}


inline dmHullFace * dmHullHalfEdge::GetOppositeFace()
{
    return m_pTwin->m_pFace;
}


















inline void dmHullHalfEdge::Link(dmHullHalfEdge * pEdge1, dmHullHalfEdge * pEdge2)
{
    pEdge1->m_pTwin = pEdge2;
    pEdge2->m_pTwin = pEdge1;

    dmAssert(pEdge1->m_pNext->m_pVertex == pEdge2->m_pVertex);
    dmAssert(pEdge2->m_pNext->m_pVertex == pEdge1->m_pVertex);
}
