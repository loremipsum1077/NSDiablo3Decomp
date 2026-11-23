// Original file: Contrib/Contrib/Domino/Collision/ConvexHull/dmSimpleHull.cpp
// Edge-collapse and face-clustering simplification of a convex hull. Blank
// lines keep every function on its original line: the asserts and allocations
// bake __LINE__ into the code.
#line 6 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/ConvexHull/dmSimpleHull.cpp"
#include "dmSimpleHull.h"
#include "dmHullBuilder.h"
#include "../../Common/dmArray.h"
#include "../../Include/dmBuffer.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmSerialize.h"

#include <float.h>
#include <limits.h>
#include <new>

struct Vertex;

struct SimpleHullCluster
{
    bool ComputeNormal();
    bool ComputeOffset(const dmHullBuilder * pHull, dm_vec4Arg centroid);

    dm_vec4 m_averageNormal;
    dm_vec4 m_offset;
    dmArray<const dmHullFace *> m_faces;
};

// Defined in Common/dmMath.cpp; belongs in Include/dmMath.h.
extern const dm_vec4 dm_vec4_linearSlop;

struct Triangle
{
    void Create(Vertex * p1, Vertex * p2, Vertex * p3);
    void Deactivate();
    void ComputeNormal();
    void ReplaceVertex(Vertex * pOld, Vertex * pNew);
    bool HasVertex(Vertex * pVertex);

    Vertex * m_p1;
    Vertex * m_p2;
    Vertex * m_p3;
    dm_vec4 m_normal;
    bool m_active;
};





struct Vertex
{
    void Create(dm_vec4Arg point);
    void Deactivate();
    void Add(Triangle * p);
    void Remove(Triangle * p);
    bool Contains(Triangle *);
    void AddNeighborUnique(Vertex * pOther);
    void RemoveNeighbor(Vertex * pOther);
    void RemoveIfNonNeighbor(Vertex * p);
    void ComputeCost();
    dm_float32 ComputeEdgeCost(Vertex * pOther);

    dm_int32 m_index;
    dm_vec4 m_point;
    dmBuffer<Vertex *, 12> m_neighbors;
    dmBuffer<Triangle *, 12> m_faces;
    dm_float32 m_cost;
    Vertex * m_collapseTarget;
    bool m_active;
};






void Triangle::Create(Vertex * p1, Vertex * p2, Vertex * p3)
{
    m_p1 = p1;
    m_p2 = p2;
    m_p3 = p3;

    ComputeNormal();

    p1->Add(this);
    p1->AddNeighborUnique(p2);
    p1->AddNeighborUnique(p3);

    p2->Add(this);
    p2->AddNeighborUnique(p1);
    p2->AddNeighborUnique(p3);

    p3->Add(this);
    p3->AddNeighborUnique(p1);
    p3->AddNeighborUnique(p2);

    m_active = true;
}


void Triangle::Deactivate()
{

    m_p1->Remove(this);
    m_p2->Remove(this);
    m_p3->Remove(this);


    m_p1->RemoveIfNonNeighbor(m_p2);
    m_p2->RemoveIfNonNeighbor(m_p1);
    m_p2->RemoveIfNonNeighbor(m_p3);
    m_p3->RemoveIfNonNeighbor(m_p2);
    m_p3->RemoveIfNonNeighbor(m_p1);
    m_p1->RemoveIfNonNeighbor(m_p3);

    m_p1 = NULL;
    m_p2 = NULL;
    m_p3 = NULL;
    m_active = false;
}


inline bool Triangle::HasVertex(Vertex * pVertex)
{

    return m_p1 == pVertex || m_p2 == pVertex || m_p3 == pVertex;
}

inline void Triangle::ComputeNormal()
{
    dm_vec4 v1 = m_p1->m_point;
    dm_vec4 v2 = m_p2->m_point;
    dm_vec4 v3 = m_p3->m_point;
    m_normal = dmCross(v2 - v1, v3 - v1);
    if (dmIsZeroSafe3(m_normal) == false)
    {
        m_normal = dmAccurateNormalize3(m_normal);
    }
}


void Triangle::ReplaceVertex(Vertex * pOld, Vertex * pNew)
{



    if (m_p1 == pOld)
    {
        m_p1 = pNew;
    }
    else if (m_p2 == pOld)
    {
        m_p2 = pNew;
    }
    else
    {

        m_p3 = pNew;
    }

    pOld->Remove(this);
    pNew->Add(this);






    pOld->RemoveIfNonNeighbor(m_p1);
    m_p1->RemoveIfNonNeighbor(pOld);
    pOld->RemoveIfNonNeighbor(m_p2);
    m_p2->RemoveIfNonNeighbor(pOld);
    pOld->RemoveIfNonNeighbor(m_p3);
    m_p3->RemoveIfNonNeighbor(pOld);


    m_p1->AddNeighborUnique(m_p2);
    m_p1->AddNeighborUnique(m_p3);
    m_p2->AddNeighborUnique(m_p1);
    m_p2->AddNeighborUnique(m_p3);
    m_p3->AddNeighborUnique(m_p1);
    m_p3->AddNeighborUnique(m_p2);


    ComputeNormal();
}


inline void Vertex::Create(dm_vec4Arg point)
{
    m_point = point;
    m_index = -1;
    m_collapseTarget = NULL;
    m_active = true;
    m_cost = 0.0f;
}


void Vertex::Deactivate()
{
    m_faces.SetCount(0);

    for (dm_int32 i = 0; i < m_neighbors.GetCount(); ++i)
    {
        m_neighbors[i]->RemoveNeighbor(this);
    }

    m_neighbors.SetCount(0);
    m_active = false;
}


inline void Vertex::Add(Triangle * p)
{
    m_faces.PushBack(p);
}


inline void Vertex::Remove(Triangle * p)
{
    dm_int32 count = m_faces.GetCount();
    for (dm_int32 i = 0; i < count; ++i)
    {
        if (m_faces[i] == p)
        {
            m_faces[i] = m_faces[count - 1];
            m_faces.SetCount(count - 1);
            return;
        }
    }
}


















inline void Vertex::AddNeighborUnique(Vertex * pOther)
{
    for (dm_int32 i = 0; i < m_neighbors.GetCount(); ++i)
    {
        if (m_neighbors[i] == pOther)
        {
            return;
        }
    }

    m_neighbors.PushBack(pOther);
}


inline void Vertex::RemoveNeighbor(Vertex * pOther)
{
    dm_int32 count = m_neighbors.GetCount();
    for (dm_int32 i = 0; i < count; ++i)
    {
        if (m_neighbors[i] == pOther)
        {
            m_neighbors[i] = m_neighbors[count - 1];
            m_neighbors.SetCount(count - 1);
            return;
        }
    }
}




void Vertex::RemoveIfNonNeighbor(Vertex * p)
{

    dm_int32 count = m_neighbors.GetCount();
    dm_int32 index = -1;
    for (dm_int32 i = 0; i < count; ++i)
    {
        if (m_neighbors[i] == p)
        {
            index = i;
            break;
        }
    }


    if (index == -1)
    {
        return;
    }

    for (dm_int32 i = 0; i < m_faces.GetCount(); ++i)
    {
        if (m_faces[i]->HasVertex(p))
        {
            return;
        }
    }

    m_neighbors[index] = m_neighbors[count - 1];
    m_neighbors.SetCount(count - 1);
}



inline void Vertex::ComputeCost()
{
    m_collapseTarget = NULL;
    m_cost = FLT_MAX;

    for (dm_int32 i = 0; i < m_neighbors.GetCount(); ++i)
    {
        dm_float32 cost = ComputeEdgeCost(m_neighbors[i]);
        if (cost < m_cost)
        {
            m_cost = cost;
            m_collapseTarget = m_neighbors[i];
        }
    }
}


dm_float32 Vertex::ComputeEdgeCost(Vertex * pOther)
{

    dmBuffer<Triangle *, 12> sides;
    for (dm_int32 i = 0; i < m_faces.GetCount(); ++i)
    {
        if (m_faces[i]->HasVertex(pOther))
        {
            sides.PushBack(m_faces[i]);
        }
    }

    dm_float32 curvature = 0.0f;
    for (dm_int32 i = 0; i < m_faces.GetCount(); ++i)
    {
        dm_float32 minCurvature = 1.0f;

        for (dm_int32 j = 0; j < sides.GetCount(); ++j)
        {
            dm_float32 dot = dmDot3_f32(m_faces[i]->m_normal, sides[j]->m_normal);
            minCurvature = dmMin(minCurvature, (1.0f - dot) * 0.5f);
        }

        curvature = dmMax(curvature, minCurvature);
    }

    dm_float32 length = dmLength3_f32(pOther->m_point - m_point);
    dm_float32 cost = curvature * length;


    return cost;
}





struct SimpleHull
{
    SimpleHull();
    ~SimpleHull();
    bool Build(dmHullBuilder * pHull, const dmSimpleHullInput * pInput);
    bool SimplifyEdges(dmHullBuilder * pHull, const dmHullBuilder * pBase, dm_int32 maxCount, dm_float32 edgeTolerance, dm_float32 weldTolerance);
    bool SimplifyFaces(dmHullBuilder * pHull, const dmHullBuilder * pBase, dm_float32 faceTolerance);
    void BuildVertices(const dmHullBuilder * pHull);
    void BuildTriangles(const dmHullBuilder * pHull);
    void ComputeEdgeCost(Vertex *, Vertex *);
    void CollapseEdge(Vertex * p1, Vertex * p2);

    Vertex * m_pVertices;
    Triangle * m_pTriangles;
    dm_int32 m_vertexCount;
    dm_int32 m_triangleCount;
    dm_int32 m_activeVertexCount;
    dm_int32 m_activeTriangleCount;
};





inline SimpleHull::SimpleHull()
{

    m_pVertices = NULL;
    m_pTriangles = NULL;
    m_vertexCount = 0;
    m_triangleCount = 0;
    m_activeVertexCount = 0;
    m_activeTriangleCount = 0;
}



inline SimpleHull::~SimpleHull()
{

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        m_pVertices[i].~Vertex();
    }
    dmFree(m_pVertices);

    dmFree(m_pTriangles);
}


bool SimpleHull::SimplifyEdges(dmHullBuilder * pHull, const dmHullBuilder * pBase, dm_int32 maxCount, dm_float32 edgeTolerance, dm_float32 weldTolerance)
{

    BuildVertices(pBase);
    BuildTriangles(pBase);


    dm_vec4 lowerBound = dm_vec4_maxFloat;
    dm_vec4 upperBound = -dm_vec4_maxFloat;

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        lowerBound = dmMin(lowerBound, m_pVertices[i].m_point);
        upperBound = dmMax(upperBound, m_pVertices[i].m_point);
    }

    dm_vec4 w = upperBound - lowerBound;


    dm_float32 edgeTol = (edgeTolerance / 3.0f) * (vpadds_f32(vget_low_f32(w)) + dmGetZ(w));


    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        m_pVertices[i].ComputeCost();
    }




    dm_int32 maxVertices = maxCount == 0 ? INT_MAX : maxCount;

    for (;;)
    {



        dm_int32 minIndex = -1;
        dm_float32 minCost = FLT_MAX;
        for (dm_int32 i = 0; i < m_vertexCount; ++i)
        {
            if (m_pVertices[i].m_active == false)
            {
                continue;
            }

            if (m_pVertices[i].m_cost < minCost)
            {
                minIndex = i;

                minCost = m_pVertices[i].m_cost;
            }
        }





        if ((minCost > edgeTol && m_activeVertexCount <= maxVertices) || m_activeVertexCount < 4 ||
            m_activeTriangleCount < 5)
        {
            break;
        }


        Vertex * p = m_pVertices + minIndex;
        CollapseEdge(p, p->m_collapseTarget);
    }

    dm_vec4 * pActiveVertices = (dm_vec4 *)dmAlloc(m_activeVertexCount * sizeof(dm_vec4));
    dm_int32 count = 0;
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        if (m_pVertices[i].m_active)
        {



            pActiveVertices[count] = m_pVertices[i].m_point;
            m_pVertices[i].m_index = count;
            ++count;
        }
    }



    dmHullInput input;
    input.count = m_activeVertexCount;
    input.aPoints = pActiveVertices;
    input.weldTolerance = weldTolerance;

    bool success = pHull->Build(&input);

    dmFree(pActiveVertices);
    return success;
}



inline bool SimpleHullCluster::ComputeNormal()
{
    dm_vec4 n = vdupq_n_f32(0.0f);

    for (dm_int32 i = 0; i < m_faces.GetCount(); ++i)
    {
        n += m_faces[i]->m_areaMeasure * m_faces[i]->m_normal;
    }

    if (dmIsZeroSafe3(n))
    {
        return false;
    }


    m_averageNormal = dmAccurateNormalize3(n);
    return true;
}

inline bool SimpleHullCluster::ComputeOffset(const dmHullBuilder * pHull, dm_vec4Arg centroid)
{

    dm_int32 vertexCount = pHull->GetVertexCount();
    m_offset = -dm_vec4_maxFloat;
    for (dm_int32 j = 0; j < vertexCount; ++j)
    {
        const dm_vec4 & p = pHull->GetVertex(j)->m_point;
        m_offset = dmMax(m_offset, dmDot3(m_averageNormal, p - centroid));
    }

    if (dmLT(m_offset, dm_vec4_zeroSafe))
    {
        return false;
    }

    return true;
}









bool SimpleHull::SimplifyFaces(dmHullBuilder * pHull, const dmHullBuilder * pBase, dm_float32 faceTolerance)
{
    dm_int32 faceCount = pBase->GetFaceCount();
    SimpleHullCluster * aClusters = (SimpleHullCluster *)dmAlloc(faceCount * sizeof(SimpleHullCluster));


    dm_int32 index = 0;
    for (const dmHullFace * pFace = pBase->m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {
        aClusters[index].m_faces.Create();
        aClusters[index].m_faces.PushBack(pFace);
        aClusters[index].ComputeNormal();
        ++index;
    }

    dm_float32 cosTol = cosf(faceTolerance);
    dm_int32 clusterCount = faceCount;
    dm_int32 iter = 0;
    for (;;)
    {
        dm_float32 bestDot = -FLT_MAX;
        dm_int32 bestI = 0;
        dm_int32 bestJ = 0;
        for (dm_int32 i = 0; i < clusterCount; ++i)
        {
            if (aClusters[i].m_faces.GetCount() == 0)
            {
                continue;
            }

            dm_vec4 ni = aClusters[i].m_averageNormal;

            for (dm_int32 j = i + 1; j < clusterCount; ++j)
            {
                if (aClusters[j].m_faces.GetCount() == 0)
                {
                    continue;
                }

                dm_vec4 nj = aClusters[j].m_averageNormal;
                dm_float32 dot = dmDot3_f32(ni, nj);
                if (dot > bestDot)
                {
                    bestDot = dot;
                    bestI = i;
                    bestJ = j;
                }
            }
        }

        if (bestDot < cosTol)
        {
            break;
        }


        SimpleHullCluster * pCluster1 = aClusters + bestI;
        SimpleHullCluster * pCluster2 = aClusters + bestJ;
        for (dm_int32 i = 0; i < pCluster2->m_faces.GetCount(); ++i)
        {
            pCluster1->m_faces.PushBack(pCluster2->m_faces[i]);
        }

        pCluster2->m_faces.Destroy();
        bool success = pCluster1->ComputeNormal();
        if (success == false)
        {
            dmLog("Domino Warning: SimpleHull::SimplifyFaces failed to compute a cluster normal");



            for (dm_int32 i = 0; i < clusterCount; ++i)
            {
                aClusters[i].m_faces.Destroy();
            }
            dmFree(aClusters);
            return false;
        }


        ++iter;

        if (iter == faceCount)
        {
            dmLog("Domino Warning: SimpleHull::SimplifyFaces exceeded the iteration limit");



            for (dm_int32 i = 0; i < clusterCount; ++i)
            {
                aClusters[i].m_faces.Destroy();
            }
            dmFree(aClusters);
            return false;
        }
    }



    dm_vec4 centroid = pBase->ComputeCentroid();



    for (dm_int32 i = 0; i < clusterCount; ++i)
    {
        if (aClusters[i].m_faces.GetCount() == 0)
        {
            while (i < --clusterCount && aClusters[clusterCount].m_faces.GetCount() == 0)
            {
            }


            aClusters[i] = aClusters[clusterCount];
        }


        if (aClusters[i].m_faces.GetCount() > 0)
        {
            bool success = aClusters[i].ComputeOffset(pBase, centroid);
            if (success == false)
            {
                dmLog("Domino Warning: SimpleHull::SimplifyFaces failed to compute a cluster offset");



                for (dm_int32 i = 0; i < clusterCount; ++i)
                {
                    aClusters[i].m_faces.Destroy();
                }
                dmFree(aClusters);
                return false;
            }
        }
    }





    if (clusterCount < 4)
    {
        dmLog("Domino Warning: SimpleHull::SimplifyFaces obtained less than 4 clusters");



        for (dm_int32 i = 0; i < clusterCount; ++i)
        {
            aClusters[i].m_faces.Destroy();
        }
        dmFree(aClusters);
        return false;
    }




    dm_vec4 * aPoints = (dm_vec4 *)dmAlloc(clusterCount * sizeof(dm_vec4));
    for (dm_int32 i = 0; i < clusterCount; ++i)
    {
        aPoints[i] = aClusters[i].m_averageNormal / aClusters[i].m_offset;
    }


    for (dm_int32 i = 0; i < clusterCount; ++i)
    {
        aClusters[i].m_faces.Destroy();
    }

    dmFree(aClusters);


    dmHullInput input;
    input.count = clusterCount;
    input.aPoints = aPoints;
    input.weldTolerance = 0.0f;

    dmHullBuilder halfSpaceHull;
    bool success = halfSpaceHull.Build(&input);
    dmFree(aPoints);
    if (success == false)
    {


        dmLog("Domino Warning: SimpleHull::SimplifyFaces failed to compute half-space hull");
        return false;
    }



    dm_int32 count = halfSpaceHull.GetFaceCount();
    aPoints = (dm_vec4 *)dmAlloc(count * sizeof(dm_vec4));
    const dmHullFace * pFace = halfSpaceHull.GetFaceList();
    index = 0;
    while (pFace != NULL)
    {

        aPoints[index] = centroid + pFace->m_normal / pFace->m_planeOffset;
        pFace = pFace->m_pNext;
        ++index;
    }



    input.count = count;
    input.aPoints = aPoints;
    input.weldTolerance = 0.0f;

    success = pHull->Build(&input);
    dmFree(aPoints);

    if (success == false)
    {
        return false;
    }



    dm_vec4 tolerance = dm_vec4_linearSlop * 0.1f;
    const dm_vec4 toleranceSqr = tolerance * tolerance;
    for (pFace = pHull->m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {
        const dmHullHalfEdge * pEdge = pFace->m_pEdge;
        do
        {
            dm_vec4 v1 = pEdge->m_pVertex->m_point;
            dm_vec4 v2 = pEdge->m_pTwin->m_pVertex->m_point;
            dm_vec4 lengthSqr = dmLengthSqr3(v2 - v1);

            if (dmLT(lengthSqr, toleranceSqr))
            {
                dmLog("Domino Warning: SimpleHull::SimplifyFaces failed due to a short edge");
                return false;
            }

            pEdge = pEdge->m_pNext;
        } while (pEdge != pFace->m_pEdge);
    }

    return true;
}


inline void SimpleHull::BuildVertices(const dmHullBuilder * pHull)
{
    m_vertexCount = pHull->GetVertexCount();
    m_pVertices = (Vertex *)dmAlloc(m_vertexCount * sizeof(Vertex));

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        new (m_pVertices + i) Vertex;
        m_pVertices[i].Create(pHull->GetVertex(i)->m_point);
    }

    m_activeVertexCount = m_vertexCount;
}


void SimpleHull::BuildTriangles(const dmHullBuilder * pHull)
{
    m_triangleCount = 0;

    for (const dmHullFace * pFace = pHull->m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {


        const dmHullHalfEdge * pEdge = pFace->m_pEdge;
        dm_int32 faceVertexCount = 0;
        do
        {
            pEdge = pEdge->m_pNext;
            ++faceVertexCount;
        } while (pEdge != pFace->m_pEdge);

        dmAssert(3 <= faceVertexCount);
        m_triangleCount += faceVertexCount - 2;
    }

    m_pTriangles = (Triangle *)dmAlloc(m_triangleCount * sizeof(Triangle));

    dm_int32 count = 0;
    for (const dmHullFace * pFace = pHull->m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {
        const dmHullHalfEdge * pEdge = pFace->m_pEdge;
        dm_int32 i1 = pEdge->m_pVertex->m_index;

        pEdge = pEdge->m_pNext;
        dm_int32 i2 = pEdge->m_pVertex->m_index;

        pEdge = pEdge->m_pNext;

        do
        {
            dm_int32 i3 = pEdge->m_pVertex->m_index;

            dmAssert(count < m_triangleCount);
            m_pTriangles[count].Create(m_pVertices + i1, m_pVertices + i2, m_pVertices + i3);


            ++count;

            pEdge = pEdge->m_pNext;
            i2 = i3;
        } while (pEdge != pFace->m_pEdge);
    }

    dmAssert(count == m_triangleCount);

    m_activeTriangleCount = m_triangleCount;
}






void SimpleHull::CollapseEdge(Vertex * p1, Vertex * p2)
{
    if (p2 == NULL)
    {
        p1->m_active = false;

        return;
    }



    dmBuffer<Vertex *, 8> neighbors;
    for (dm_int32 i = 0; i < p1->m_neighbors.GetCount(); ++i)
    {
        neighbors.PushBack(p1->m_neighbors[i]);
    }



    dm_int32 count = p1->m_faces.GetCount();
    for (dm_int32 i = count - 1; i >= 0; --i)
    {
        if (p1->m_faces[i]->HasVertex(p2))
        {
            p1->m_faces[i]->Deactivate();
            --m_activeTriangleCount;
        }
    }



    count = p1->m_faces.GetCount();
    for (dm_int32 i = count - 1; i >= 0; --i)
    {
        p1->m_faces[i]->ReplaceVertex(p1, p2);
    }


    p1->Deactivate();
    --m_activeVertexCount;


    for (dm_int32 i = 0; i < neighbors.GetCount(); ++i)
    {
        neighbors[i]->ComputeCost();
    }
}


bool SimpleHull::Build(dmHullBuilder * pHull, const dmSimpleHullInput * pInput)
{
    dmAssert(pInput->count > 3);


    dm_vec4 lowerBound = dm_vec4_maxFloat, upperBound = -dm_vec4_maxFloat;
    for (dm_int32 i = 0; i < pInput->count; ++i)
    {
        lowerBound = dmMin(lowerBound, pInput->pPoints[i]);
        upperBound = dmMax(upperBound, pInput->pPoints[i]);
    }
    dm_vec4 h = (upperBound - lowerBound) * dm_vec4_oneHalf;
    dm_vec4 c = (upperBound + lowerBound) * dm_vec4_oneHalf;
    dm_vec4 scale = dmLength3(h);
    if (dmIsZeroSafe3(scale))
    {
        return false;
    }
    dm_vec4 invScale = dmSafeReciprocal(scale);
    dm_vec4 * pPoints = (dm_vec4 *)dmAlloc(pInput->count * sizeof(dm_vec4));
    for (dm_int32 i = 0; i < pInput->count; ++i)
    {
        pPoints[i] = invScale * (pInput->pPoints[i] - c);
    }


    dmHullInput hullInput;
    hullInput.aPoints = pPoints;
    hullInput.count = pInput->count;
    hullInput.weldTolerance = pInput->weldTolerance;

    bool success = pHull->Build(&hullInput);
    dmFree(pPoints);


    if (success == false)
    {
        return false;
    }



    dmHullBuilder * pCurrentHull = pHull;
    dmHullBuilder edgeHull;
    if (pInput->edgeTolerance > 0.0f)
    {
        pCurrentHull = &edgeHull;
        success = SimplifyEdges(&edgeHull, pHull, pInput->maxCount, pInput->edgeTolerance, pInput->weldTolerance);
        if (success == false)
        {
            dmSimpleHullInput simpleInput = *pInput;
            simpleInput.edgeTolerance = 0.0f;
            return Build(pHull, &simpleInput);
        }
    }




    dmHullBuilder faceHull;
    if (pInput->faceTolerance > 0.0f)
    {
        success = SimplifyFaces(&faceHull, pCurrentHull, pInput->faceTolerance);
        if (success)
        {
            pCurrentHull = &faceHull;
        }
    }


    dm_int32 count = pCurrentHull->GetVertexCount();
    dm_vec4 * pBuffer = (dm_vec4 *)dmAlloc(count * sizeof(dm_vec4));
    for (dm_int32 i = 0; i < count; ++i)
    {
        const dmHullVertex * pVertex = pCurrentHull->GetVertex(i);
        pBuffer[i] = c + scale * pVertex->m_point;
    }

    hullInput.aPoints = pBuffer;
    hullInput.count = count;
    hullInput.weldTolerance = 0.0f;

    pHull->Build(&hullInput);
    dmFree(pBuffer);


    return true;
}

bool dmCreateSimpleHull(dmHullBuilder * pHull, const dmSimpleHullInput * pInput)
{

    SimpleHull simpleHull;
    return simpleHull.Build(pHull, pInput);
}
