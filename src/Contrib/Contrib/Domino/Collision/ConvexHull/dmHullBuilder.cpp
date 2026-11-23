// Original file: Contrib/Contrib/Domino/Collision/ConvexHull/dmHullBuilder.cpp
// Quickhull. Blank lines keep every function on its original line: the
// asserts and allocations bake __LINE__ into the code.
#line 5 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/ConvexHull/dmHullBuilder.cpp"
#include "dmHullBuilder.h"

#include <string.h>

// static dm_int32 dm_hullDebugCount (line 41) only fed debug code that is
// compiled out; the linker dropped it and its value is not recoverable.



























































void dmHullFace::Initialize()
{
    m_pEdge = NULL;
    m_conflicts = NULL;
    m_index = -1;
    m_pPrev = NULL;
    m_pNext = NULL;
    m_state = e_normal;
}



void dmHullFace::Destroy()
{
    dmHullHalfEdge* pEdge = m_pEdge;
    if (pEdge != NULL)
    {
        do
        {
            dmHullHalfEdge* pNext = pEdge->m_pNext;
            dmFree(pEdge);
            pEdge = pNext;
        }
        while (pEdge != NULL && pEdge != m_pEdge);
    }


    m_pEdge = NULL;
}


void dmHullFace::ComputeGeometry()
{
    m_normal = vdupq_n_f32(0.0f);
    m_point = vdupq_n_f32(0.0f);

    m_vertexCount = 0;
    dmHullHalfEdge* pEdge1 = m_pEdge;
    do
    {
        dmHullHalfEdge* pEdge2 = pEdge1->m_pNext;


        dm_vec4 v1 = pEdge1->m_pVertex->m_point;
        dm_vec4 v2 = pEdge2->m_pVertex->m_point;

        m_normal += dmCross(v1, v2);
        m_point += v1;


        ++m_vertexCount;
        pEdge1 = pEdge2;
    }
    while (pEdge1 != m_pEdge);
    m_areaMeasure = dmLengthSqr3(m_normal);

    if (dmIsZeroSafe3(m_normal) == false)
    {
        m_normal = dmAccurateNormalize3(m_normal);
    }
    else
    {
        m_normal = vdupq_n_f32(0.0f);
    }


    m_point *= 1.0f / m_vertexCount;
    m_planeOffset = dmDot3(m_normal, m_point);
}


dmHullFace* dmHullFace::CreateTriangle(dmHullVertex* pv1, dmHullVertex* pv2, dmHullVertex* pv3)
{
    dmHullFace* pFace = (dmHullFace*)dmAlloc(sizeof(dmHullFace));
    pFace->Initialize();
    dmHullHalfEdge* pe1 = (dmHullHalfEdge*)dmAlloc(sizeof(dmHullHalfEdge));
    pe1->Initialize(pv1, pFace);
    dmHullHalfEdge* pe2 = (dmHullHalfEdge*)dmAlloc(sizeof(dmHullHalfEdge));
    pe2->Initialize(pv2, pFace);
    dmHullHalfEdge* pe3 = (dmHullHalfEdge*)dmAlloc(sizeof(dmHullHalfEdge));
    pe3->Initialize(pv3, pFace);


    pe1->m_pPrev = pe3;
    pe1->m_pNext = pe2;

    pe2->m_pPrev = pe1;
    pe2->m_pNext = pe3;

    pe3->m_pPrev = pe2;
    pe3->m_pNext = pe1;


    pFace->m_pEdge = pe1;


    pFace->ComputeGeometry();

    return pFace;
}


void dmHullFace::Validate()
{
    dm_int32 count = 0;
    dmHullHalfEdge* pEdge = m_pEdge;

    dmAssert(m_vertexCount >= 3);

    do
    {
        dmAssert(pEdge->m_pTwin != NULL);

        dmHullHalfEdge* pTwin = pEdge->m_pTwin;

        dmAssert(pTwin->m_pTwin == pEdge);
        dmAssert(pEdge->GetHead() == pTwin->GetTail());
        dmAssert(pEdge->GetTail() == pTwin->GetHead());
        dmAssert(pTwin->m_pFace != NULL);
        dmAssert(pTwin->m_pFace->m_state == e_normal);

        ++count;
        pEdge = pEdge->m_pNext;
    }
    while (pEdge != m_pEdge);

    dmAssert(count == m_vertexCount);
}


dmHullBuilder::dmHullBuilder()
{
    m_vertices = NULL;
    m_vertexCount = 0;

    m_faceList = NULL;
    m_faceCount = 0;

    m_horizonCount = 0;
    m_visibleCount = 0;

    m_orphanList = NULL;
    m_conflictList = NULL;
    m_discardList = NULL;

    m_tolerance = vdupq_n_f32(0.0f);
}


dmHullBuilder::~dmHullBuilder()
{
    Clear();
}


void dmHullBuilder::Clear()
{
    dmFree(m_vertices);

    dmHullFace* pFace = m_faceList;
    while (pFace != NULL)
    {
        dmHullFace* pNext = pFace->m_pNext;
        pFace->Destroy();
        dmFree(pFace);
        pFace = pNext;
    }

    m_vertices = NULL;
    m_vertexCount = 0;

    m_faceList = NULL;
    m_faceCount = 0;

    m_horizonCount = 0;
    m_visibleCount = 0;
    m_orphanList = NULL;
    m_conflictList = NULL;
    m_discardList = NULL;

    m_tolerance = vdupq_n_f32(0.0f);
}


bool dmHullBuilder::Build(const dmHullInput* pInput)
{
    Clear();

    m_vertexCount = pInput->count;

    m_vertices = (dmHullVertex*)dmAlloc(m_vertexCount * sizeof(dmHullVertex));
    memset(m_vertices, 0, m_vertexCount * sizeof(dmHullVertex));


    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        dmHullVertex* pVertex = m_vertices + i;
        pVertex->m_point = pInput->aPoints[i];
        pVertex->m_index = i;
        pVertex->m_originalIndex = i;
    }



    CleanVertices(dmV4(pInput->weldTolerance));

    bool success = CreateInitialSimplex();
    if (success == false)
    {
        return false;
    }

    dm_int32 iteration = 0;
    for (;;)
    {
        dmHullVertex* pApex = GetNextConflictVertex();

        if (pApex == NULL)
        {
            break;
        }


        AddVertexToHull(pApex);

        ++iteration;
    }





    CullUnusedVertices();

    dm_int32 index = 0;
    for (dmHullFace* pFace = m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {
        pFace->m_index = index;
        ++index;
    }

    return true;
}


dm_vec4 dmHullBuilder::ComputeCentroid() const
{
    dmAssert(3 <= m_vertexCount);

    dm_vec4 integral1 = vdupq_n_f32(0.0f);
    dm_vec4 integral2 = vdupq_n_f32(0.0f);

    for (const dmHullFace* pFace = m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {



        dmAssert(3 <= pFace->m_vertexCount);


        const dmHullHalfEdge* pEdge = pFace->m_pEdge;

        dm_vec4 v1 = pEdge->m_pVertex->m_point;
        const dmHullHalfEdge* pEdge1 = pEdge->m_pNext;
        dm_vec4 v2 = pEdge1->m_pVertex->m_point;
        pEdge1 = pEdge1->m_pNext;

        do
        {
            dm_vec4 v3 = pEdge1->m_pVertex->m_point;


            dm_vec4 e1 = v2 - v1;
            dm_vec4 e2 = v3 - v1;
            dm_vec4 d = dmCross(e1, e2);

            dm_vec4 f1 = v1 + v2 + v3;
            dm_vec4 f2 = v1 * v1 + v2 * (v1 + v2) + v3 * f1;

            integral1 += (d * f1)[0];
            integral2 += d * f2;

            v2 = v3;

            pEdge1 = pEdge1->m_pNext;
        }
        while (pEdge1 != pEdge);
    }

    dm_vec4 centroid = vdupq_n_f32(0.0f);
    if (dmIsZeroSafe3(integral1) == false)
    {
        centroid = integral2 * (dm_vec4_oneHalf * dm_vec4_oneHalf) / integral1;
    }

    return centroid;
}



void dmHullBuilder::CleanVertices(dm_vec4Arg weldTolerance)
{
    if (m_vertexCount == 0 || dmLEZ(weldTolerance))
    {
        return;
    }


    dm_vec4 lowerBound = dm_vec4_maxFloat;
    dm_vec4 upperBound = -dm_vec4_maxFloat;

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        lowerBound = dmMin(lowerBound, m_vertices[i].m_point);
        upperBound = dmMax(upperBound, m_vertices[i].m_point);
    }

    dm_vec4 h = upperBound - lowerBound;
    dm_vec4 t = weldTolerance * h;

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        if (m_vertices[i].m_redundant)
        {
            continue;
        }

        dm_vec4 v1 = m_vertices[i].m_point;

        for (dm_int32 j = i + 1; j < m_vertexCount; ++j)
        {
            if (m_vertices[j].m_redundant)
            {
                continue;
            }

            dm_vec4 v2 = m_vertices[j].m_point;
            dm_vec4 delta = dmAbs(v2 - v1);

            if (dmAll_LT3(delta, t))
            {
                m_vertices[j].m_redundant = true;
            }
        }
    }

    dm_int32 count = 0;
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        if (m_vertices[i].m_redundant == false)
        {
            m_vertices[count++] = m_vertices[i];
        }
    }

    m_vertexCount = count;
}



bool dmHullBuilder::CreateInitialSimplex()
{
    if (m_vertexCount < 4)
    {
        return false;
    }

    dm_vec4 minPoint = m_vertices[0].m_point;
    dm_vec4 maxPoint = minPoint;

    dmHullVertex* maxVertices[3];
    dmHullVertex* minVertices[3];
    dmHullFace* faces[4];
    dmHullVertex* simplex[4];

    maxVertices[0] = minVertices[0] = m_vertices;
    maxVertices[1] = minVertices[1] = m_vertices;
    maxVertices[2] = minVertices[2] = m_vertices;

    for (dm_int32 i = 1; i < m_vertexCount; ++i)
    {
        dm_vec4 p = m_vertices[i].m_point;
        dm_vec4 px = dmSplatX(p);
        dm_vec4 py = dmSplatY(p);
        dm_vec4 pz = dmSplatZ(p);
        dm_vec4 maxX = dmSplatX(maxPoint);
        dm_vec4 maxY = dmSplatY(maxPoint);
        dm_vec4 maxZ = dmSplatZ(maxPoint);
        dm_vec4 minX = dmSplatX(minPoint);
        dm_vec4 minY = dmSplatY(minPoint);
        dm_vec4 minZ = dmSplatZ(minPoint);

        if (dmGT(px, maxX))
        {
            maxVertices[0] = m_vertices + i;
        }
        else if (dmLT(px, minX))
        {
            minVertices[0] = m_vertices + i;
        }

        if (dmGT(py, maxY))
        {
            maxVertices[1] = m_vertices + i;
        }
        else if (dmLT(py, minY))
        {
            minVertices[1] = m_vertices + i;
        }

        if (dmGT(pz, maxZ))
        {
            maxVertices[2] = m_vertices + i;
        }
        else if (dmLT(pz, minZ))
        {
            minVertices[2] = m_vertices + i;
        }

        minPoint = dmMin(minPoint, p);
        maxPoint = dmMax(maxPoint, p);
    }


    dm_vec4 d = dmMax(dmAbs(minPoint), dmAbs(maxPoint));
    m_tolerance = dm_vec4_three * dm_vec4_epsilon * (dmSplatX(d) + dmSplatY(d) + dmSplatZ(d));

    dm_int32 index = 0;
    dm_vec4 maxDistance = vdupq_n_f32(0.0f);

    {

        dm_vec4 distance = dmSplatX(maxVertices[0]->m_point - minVertices[0]->m_point);

        if (dmGT(distance, maxDistance))
        {
            maxDistance = distance;
            index = 0;
        }
    }

    {
        dm_vec4 distance = dmSplatY(maxVertices[1]->m_point - minVertices[1]->m_point);

        if (dmGT(distance, maxDistance))
        {
            maxDistance = distance;
            index = 1;
        }
    }

    {
        dm_vec4 distance = dmSplatZ(maxVertices[2]->m_point - minVertices[2]->m_point);

        if (dmGT(distance, maxDistance))
        {
            maxDistance = distance;
            index = 2;
        }
    }

    if (dmLT(maxDistance, m_tolerance))
    {
        return false;
    }



    simplex[0] = maxVertices[index];
    simplex[1] = minVertices[index];

    dm_vec4 axis;
    axis = simplex[1]->m_point - simplex[0]->m_point;
    axis = dmAccurateNormalize3(axis);

    dm_vec4 normal = vdupq_n_f32(0.0f);
    dm_vec4 maxSqr = vdupq_n_f32(0.0f);
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        dm_vec4 dir = m_vertices[i].m_point - simplex[0]->m_point;
        dm_vec4 n = dmCross(axis, dir);


        dm_vec4 lenSqr = dmLengthSqr3(n);
        if (dmGT(lenSqr, maxSqr))
        {
            maxSqr = lenSqr;
            simplex[2] = m_vertices + i;
            normal = n;
        }
    }

    dm_vec4 degenTol = m_tolerance * 100.0f;
    dm_vec4 colSqr = degenTol * degenTol;
    if (dmLT(maxSqr, colSqr))
    {
        return false;
    }


    normal = dmAccurateNormalize3(normal);



    dm_vec4 offset = dmDot3(normal, simplex[2]->m_point);
    maxDistance = vdupq_n_f32(0.0f);
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        dm_vec4 distance = dmAbs(dmDot3(normal, m_vertices[i].m_point) - offset);
        if (dmGT(distance, maxDistance))
        {
            maxDistance = distance;
            simplex[3] = m_vertices + i;
        }
    }
    if (dmLT(maxDistance, degenTol))
    {
        return false;
    }






    if (dmGTZ(dmDot3(normal, simplex[3]->m_point) - offset))
    {
        dmHullVertex* pTemp = simplex[1];
        simplex[1] = simplex[2];
        simplex[2] = pTemp;
    }
    faces[0] = dmHullFace::CreateTriangle(simplex[0], simplex[1], simplex[2]);
    faces[1] = dmHullFace::CreateTriangle(simplex[3], simplex[1], simplex[0]);
    faces[2] = dmHullFace::CreateTriangle(simplex[3], simplex[2], simplex[1]);
    faces[3] = dmHullFace::CreateTriangle(simplex[3], simplex[0], simplex[2]);


    Link(faces[0], 0, faces[1], 1);
    Link(faces[0], 1, faces[2], 1);
    Link(faces[0], 2, faces[3], 1);
    Link(faces[1], 0, faces[2], 2);
    Link(faces[2], 0, faces[3], 2);
    Link(faces[3], 0, faces[1], 2);


    for (dm_int32 i = 0; i < 4; ++i)
    {
        faces[i]->m_pNext = m_faceList;
        if (m_faceList != NULL)
        {
            m_faceList->m_pPrev = faces[i];
        }
        m_faceList = faces[i];

        faces[i]->Validate();
    }




    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        dmHullVertex* pVertex = m_vertices + i;

        if (pVertex == simplex[0] || pVertex == simplex[1] || pVertex == simplex[2] || pVertex == simplex[3])
        {
            continue;
        }

        dmHullFace* pMaxFace = NULL;
        dm_vec4 maxSeparation = m_tolerance;

        for (dm_int32 j = 0; j < 4; ++j)
        {
            dm_vec4 separation = faces[j]->GetSeparation(pVertex->m_point);
            if (dmGT(separation, maxSeparation))
            {
                maxSeparation = separation;
                pMaxFace = faces[j];
            }
        }

        if (pMaxFace != NULL)
        {
            AddConflictVertex(pVertex, pMaxFace);
        }
    }

    return true;
}







void dmHullBuilder::AddConflictVertex(dmHullVertex* pVertex, dmHullFace* pFace)
{
    pVertex->m_pConflictFace = pFace;

    if (pFace->m_conflicts == NULL)
    {
        pVertex->m_pPrev = NULL;
        pVertex->m_pNext = m_conflictList;


        if (m_conflictList != NULL)
        {
            m_conflictList->m_pPrev = pVertex;
        }
        m_conflictList = pVertex;
    }
    else
    {
        dmHullVertex* pNext = pFace->m_conflicts;


        pVertex->m_pPrev = pNext->m_pPrev;

        if (pNext->m_pPrev == NULL)
        {
            m_conflictList = pVertex;
        }
        else
        {
            pNext->m_pPrev->m_pNext = pVertex;
        }
        pVertex->m_pNext = pNext;
        pNext->m_pPrev = pVertex;
    }

    pFace->m_conflicts = pVertex;
}


void dmHullBuilder::RemoveConflictVertex(dmHullVertex* pVertex)
{
    dmHullFace* pFace = pVertex->m_pConflictFace;


    if (pFace->m_conflicts == pVertex)
    {
        if (pVertex->m_pNext != NULL && pVertex->m_pNext->m_pConflictFace == pFace)
        {
            pFace->m_conflicts = pVertex->m_pNext;
        }
        else
        {
            pFace->m_conflicts = NULL;
        }
    }


    if (pVertex->m_pPrev != NULL)
    {
        pVertex->m_pPrev->m_pNext = pVertex->m_pNext;
    }

    if (pVertex->m_pNext != NULL)
    {
        pVertex->m_pNext->m_pPrev = pVertex->m_pPrev;
    }

    if (m_conflictList == pVertex)
    {
        m_conflictList = pVertex->m_pNext;
    }


    pVertex->m_pPrev = NULL;
    pVertex->m_pNext = NULL;
    pVertex->m_pConflictFace = NULL;
}


void dmHullBuilder::Link(dmHullFace* pFace1, dm_int32 edge1, dmHullFace* pFace2, dm_int32 edge2)
{
    dmHullHalfEdge* pEdge1 = pFace1->m_pEdge;
    for (dm_int32 i = 0; i < edge1; ++i)
    {
        pEdge1 = pEdge1->m_pNext;
    }

    dmHullHalfEdge* pEdge2 = pFace2->m_pEdge;
    for (dm_int32 i = 0; i < edge2; ++i)
    {
        pEdge2 = pEdge2->m_pNext;
    }

    dmAssert(pEdge1->m_pTwin == NULL);
    dmAssert(pEdge2->m_pTwin == NULL);

    dmHullHalfEdge::Link(pEdge1, pEdge2);
}




void dmHullBuilder::ResolveOrphanedVertices()
{
    dmHullVertex* pNextVertex;


    for (dmHullVertex* pVertex = m_orphanList; pVertex != NULL; pVertex = pNextVertex)
    {
        pNextVertex = pVertex->m_pNext;

        dm_vec4 maxSeparation = m_tolerance;
        dmHullFace* pMaxFace = NULL;

        for (dm_int32 i = 0; i < m_horizonCount; ++i)
        {
            dmHullFace* pNewFace = m_newFaces[i];

            if (pNewFace->m_state == dmHullFace::e_discarded)
            {
                continue;
            }


            dmAssert(pNewFace->m_state == dmHullFace::e_normal);

            dm_vec4 separation = pNewFace->GetSeparation(pVertex->m_point);
            if (dmGT(separation, maxSeparation))
            {
                maxSeparation = separation;
                pMaxFace = pNewFace;
            }
        }



        if (pMaxFace != NULL)
        {
            AddConflictVertex(pVertex, pMaxFace);
        }
        else
        {
            pVertex->m_pPrev = NULL;
            pVertex->m_pNext = NULL;
            pVertex->m_pConflictFace = NULL;
        }
    }

    m_orphanList = NULL;
}





void dmHullBuilder::TransferConflictVertices(dmHullFace* pFace, dmHullFace* pAbsorbingFace)
{
    dmHullVertex* pv1 = pFace->m_conflicts;
    if (pv1 == NULL)
    {
        return;
    }

    dmHullVertex* pv2 = pv1;


    while (pv2->m_pNext != NULL && pv2->m_pNext->m_pConflictFace == pFace) pv2 = pv2->m_pNext;





    if (pv1->m_pPrev != NULL)
    {
        pv1->m_pPrev->m_pNext = pv2->m_pNext;
    }

    if (pv2->m_pNext != NULL)
    {
        pv2->m_pNext->m_pPrev = pv1->m_pPrev;
    }

    if (pv1 == m_conflictList)
    {
        m_conflictList = pv2->m_pNext;
    }

    pv1->m_pPrev = NULL;
    pv2->m_pNext = NULL;

    if (pAbsorbingFace == NULL)
    {
        pv2->m_pNext = m_orphanList;
        m_orphanList = pv1;
    }
    else
    {





        dmHullVertex* pVertex = pv1;
        while (pVertex != NULL)
        {
            dmHullVertex* pNext = pVertex->m_pNext;

            pVertex->m_pPrev = NULL;
            pVertex->m_pNext = NULL;
            pVertex->m_pConflictFace = NULL;
            dm_vec4 separation = pAbsorbingFace->GetSeparation(pVertex->m_point);
            if (dmGT(separation, m_tolerance))
            {
                AddConflictVertex(pVertex, pAbsorbingFace);
            }
            else
            {


                pVertex->m_pPrev = NULL;
                pVertex->m_pNext = m_orphanList;
                m_orphanList = pVertex;
            }

            pVertex = pNext;
        }
    }

    pFace->m_conflicts = NULL;
}



void dmHullBuilder::DiscardFace(dmHullFace* pFace)
{
    dmAssert(pFace->m_state != dmHullFace::e_discarded);


    if (pFace->m_pPrev != NULL)
    {
        pFace->m_pPrev->m_pNext = pFace->m_pNext;
    }

    if (pFace->m_pNext != NULL)
    {
        pFace->m_pNext->m_pPrev = pFace->m_pPrev;
    }

    if (m_faceList == pFace)
    {
        m_faceList = pFace->m_pNext;
    }


    pFace->m_pPrev = NULL;
    pFace->m_pNext = m_discardList;
    m_discardList = pFace;

    pFace->m_state = dmHullFace::e_discarded;
}


dm_vec4 dmHullBuilder::GetOppositeFaceSeparation(dmHullHalfEdge* pEdge)
{
    return pEdge->m_pFace->GetSeparation(pEdge->m_pTwin->m_pFace->m_point);
}





void dmHullBuilder::ConnectHalfEdges(dmHullHalfEdge* pEdge, dmHullHalfEdge* pEdgeNext)
{
    dmAssert(pEdge->m_pFace == pEdgeNext->m_pFace);


    if (pEdge->GetOppositeFace() == pEdgeNext->GetOppositeFace())
    {
        dmHullFace* pFace = pEdge->m_pFace;
        dmHullFace* pOpposite = pEdge->GetOppositeFace();
        dmHullHalfEdge* pTwin;







        if (pFace->m_pEdge == pEdgeNext)
        {
            pFace->m_pEdge = pEdge;
        }


        if (pOpposite->m_vertexCount == 3)
        {



            pTwin = pEdge->m_pTwin->m_pNext->m_pTwin;


            TransferConflictVertices(pOpposite, pFace);


            DiscardFace(pOpposite);
        }
        else
        {
            dmHullHalfEdge* pOldTwin = pEdge->m_pTwin;




            pTwin = pOldTwin->m_pPrev;
            dmAssert(pTwin == pEdgeNext->m_pTwin);

            if (pOpposite->m_pEdge == pOldTwin)
            {

                pOpposite->m_pEdge = pTwin;
            }


            pTwin->m_pNext = pOldTwin->m_pNext;
            pTwin->m_pNext->m_pPrev = pTwin;


            dmFree(pOldTwin);


            pOpposite->ComputeGeometry();
        }


        pEdge->m_pNext = pEdgeNext->m_pNext;
        pEdge->m_pNext->m_pPrev = pEdge;


        dmFree(pEdgeNext);


        pEdge->m_pTwin = pTwin;
        pTwin->m_pTwin = pEdge;
    }
    else
    {

        pEdgeNext->m_pPrev = pEdge;
        pEdge->m_pNext = pEdgeNext;
    }
}












void dmHullBuilder::MergeTwoFaces(dmHullHalfEdge* pEdge)
{
    dmHullFace* pFace = pEdge->m_pFace;
    dmHullFace* pOpposite = pEdge->GetOppositeFace();


    pOpposite->m_pEdge = NULL;

    dmHullHalfEdge* pEdgePrev = pEdge->m_pPrev;
    dmHullHalfEdge* pEdgeNext = pEdge->m_pNext;
    dmHullHalfEdge* pTwin = pEdge->m_pTwin;
    dmHullHalfEdge* pTwinPrev = pTwin->m_pPrev;
    dmHullHalfEdge* pTwinNext = pTwin->m_pNext;



    while (pEdgePrev->m_pTwin == pTwinNext)
    {
        pEdgePrev = pEdgePrev->m_pPrev;
        pTwinNext = pTwinNext->m_pNext;
    }




    while (pEdgeNext->m_pTwin == pTwinPrev)
    {


        pTwinPrev = pTwinPrev->m_pPrev;
        pEdgeNext = pEdgeNext->m_pNext;
    }

    {




        dmHullHalfEdge* pSharedEdge = pEdgePrev->m_pNext;
        while (pSharedEdge != pEdgeNext)
        {
            dmHullHalfEdge* pNukedEdge = pSharedEdge;
            pSharedEdge = pSharedEdge->m_pNext;
            dmFree(pNukedEdge);
        }
        pEdgePrev->m_pNext = NULL;
        pEdgeNext->m_pPrev = NULL;


        pSharedEdge = pTwinPrev->m_pNext;
        while (pSharedEdge != pTwinNext)
        {
            dmHullHalfEdge* pNukedEdge = pSharedEdge;
            pSharedEdge = pSharedEdge->m_pNext;
            dmFree(pNukedEdge);
        }
        pTwinPrev->m_pNext = NULL;
        pTwinNext->m_pPrev = NULL;
    }


    for (dmHullHalfEdge* p = pTwinNext; p != NULL; p = p->m_pNext)
    {
        p->m_pFace = pFace;
    }


    pFace->m_pEdge = pEdgeNext;






    ConnectHalfEdges(pTwinPrev, pEdgeNext);







    ConnectHalfEdges(pEdgePrev, pTwinNext);





    pFace->ComputeGeometry();


    TransferConflictVertices(pOpposite, pFace);


    DiscardFace(pOpposite);
}



bool dmHullBuilder::MergeNonConvexFaces(dmHullFace* pFace, dm_int32 mergeType)
{
    dmHullHalfEdge* pEdge = pFace->m_pEdge;
    bool concave = false;
    dm_vec4 tolerance = m_tolerance;
    dm_vec4 sinSlop = vdupq_n_f32(0.0348995f);

    do
    {








        dmHullHalfEdge* pTwin = pEdge->m_pTwin;
        dmHullFace* pOpposite = pTwin->m_pFace;


        dm_vec4 separation1 = GetOppositeFaceSeparation(pEdge);
        dm_vec4 separation2 = GetOppositeFaceSeparation(pTwin);

        dm_vec4 n1 = pEdge->m_pFace->m_normal;
        dm_vec4 n2 = pOpposite->m_normal;





        dm_vec4 e = pTwin->m_pVertex->m_point - pEdge->m_pVertex->m_point;
        dm_vec4 sinTheta = vdupq_n_f32(0.0f);
        if (dmIsZeroSafe3(e) == false)
        {
            e = dmAccurateNormalize3(e);
            sinTheta = dmDot3(dmCross(n1, n2), e);
        }

        bool merge = false;
        if (mergeType == 1)
        {
            if ((dmGT(separation1, -tolerance) || dmGT(separation2, -tolerance)) && dmLT(sinTheta, sinSlop))
            {
                merge = true;
            }
        }
        else
        {




            if (dmGT(pFace->m_areaMeasure, pOpposite->m_areaMeasure))
            {
                if (dmGT(separation1, -tolerance) && dmLT(sinTheta, sinSlop))
                {
                    merge = true;
                }
                else if (dmGT(separation2, -tolerance) && dmLT(sinTheta, sinSlop))
                {
                    concave = true;
                }
            }
            else
            {
                if (dmGT(separation2, -tolerance) && dmLT(sinTheta, sinSlop))
                {
                    merge = true;
                }
                else if (dmGT(separation1, -tolerance) && dmLT(sinTheta, sinSlop))
                {
                    concave = true;
                }
            }
        }



        if (merge)
        {
            MergeTwoFaces(pEdge);
            return true;
        }
        pEdge = pEdge->m_pNext;
    }
    while (pEdge != pFace->m_pEdge);

    if (concave)
    {
        pFace->m_state = dmHullFace::e_concave;
    }

    return false;
}


void dmHullBuilder::SearchForHorizon(dm_vec4Arg apex, dmHullHalfEdge* pEdge0, dmHullFace* pFace)
{
    pFace->m_state = dmHullFace::e_visible;
    m_visibleFaces[m_visibleCount++] = pFace;

    dmHullHalfEdge* pEdge;


    if (pEdge0 == NULL)
    {
        pEdge0 = pFace->m_pEdge;
        pEdge = pEdge0;
    }
    else
    {



        pEdge = pEdge0->m_pNext;
    }

    do
    {


        dmHullFace* pOpposite = pEdge->GetOppositeFace();


        if (pOpposite->m_state == dmHullFace::e_normal)
        {
            if (dmGT(pOpposite->GetSeparation(apex), m_tolerance))
            {



                SearchForHorizon(apex, pEdge->m_pTwin, pOpposite);
            }
            else
            {
                m_horizonEdges[m_horizonCount++] = pEdge;
            }
        }

        pEdge = pEdge->m_pNext;
    }
    while (pEdge != pEdge0);
}


void dmHullBuilder::CreateHorizonCone(dmHullVertex* pApex)
{




    dmAssert(m_horizonCount < e_maxHorizonCount);

    dmHullHalfEdge* pLeadingEdge = NULL;
    dmHullHalfEdge* pStartingEdge = NULL;
    for (dm_int32 i = 0; i < m_horizonCount; ++i)
    {
        dmHullHalfEdge* pHorizonEdge = m_horizonEdges[i];


        dmHullFace* pFace = dmHullFace::CreateTriangle(pApex, pHorizonEdge->GetTail(), pHorizonEdge->GetHead());
        m_newFaces[i] = pFace;


        pFace->m_pNext = m_faceList;
        if (m_faceList != NULL)
        {
            m_faceList->m_pPrev = pFace;
        }
        m_faceList = pFace;



        dmHullHalfEdge* pBottomEdge = pFace->m_pEdge->m_pNext;
        dmHullHalfEdge* pOppositeEdge = pHorizonEdge->m_pTwin;
        dmHullHalfEdge::Link(pBottomEdge, pOppositeEdge);


        if (pLeadingEdge != NULL)
        {
            dmHullHalfEdge::Link(pBottomEdge->m_pPrev, pLeadingEdge);
        }
        else
        {
            pStartingEdge = pBottomEdge->m_pPrev;
        }

        pLeadingEdge = pBottomEdge->m_pNext;
    }


    dmHullHalfEdge::Link(pLeadingEdge, pStartingEdge);


    for (dm_int32 i = 0; i < m_horizonCount; ++i)
    {
        m_newFaces[i]->Validate();
    }
}


void dmHullBuilder::DestroyVisibleFaces()
{
    for (dm_int32 i = 0; i < m_visibleCount; ++i)
    {
        dmHullFace* pNuke = m_visibleFaces[i];
        dmAssert(pNuke->m_state == dmHullFace::e_visible);


        TransferConflictVertices(pNuke, NULL);


        if (pNuke->m_pPrev != NULL)
        {
            pNuke->m_pPrev->m_pNext = pNuke->m_pNext;
        }

        if (pNuke->m_pNext != NULL)
        {
            pNuke->m_pNext->m_pPrev = pNuke->m_pPrev;
        }

        if (pNuke == m_faceList)
        {
            m_faceList = pNuke->m_pNext;
        }


        pNuke->Destroy();
        dmFree(pNuke);
        m_visibleFaces[i] = NULL;
    }
}



dmHullVertex* dmHullBuilder::GetNextConflictVertex()
{
    dmHullVertex* pVertex = m_conflictList;
    if (pVertex == NULL)
    {
        return NULL;
    }









    dmHullFace* pConflictFace = pVertex->m_pConflictFace;



    dmAssert(pVertex == pConflictFace->m_conflicts);

    dm_vec4 maxSeparation = vdupq_n_f32(0.0f);
    dmHullVertex* pConflictVertex = NULL;
    while (pVertex != NULL && pVertex->m_pConflictFace == pConflictFace)
    {
        dm_vec4 separation = pConflictFace->GetSeparation(pVertex->m_point);
        if (dmGT(separation, maxSeparation))
        {
            maxSeparation = separation;
            pConflictVertex = pVertex;
        }
        pVertex = pVertex->m_pNext;
    }

    return pConflictVertex;
}









void dmHullBuilder::AddVertexToHull(dmHullVertex* pApex)
{
    m_orphanList = NULL;


    dmHullFace* pConflictFace = pApex->m_pConflictFace;


    RemoveConflictVertex(pApex);


    m_horizonCount = 0;
    m_visibleCount = 0;
    SearchForHorizon(pApex->m_point, NULL, pConflictFace);





    CreateHorizonCone(pApex);


    DestroyVisibleFaces();



    for (dm_int32 i = 0; i < m_horizonCount; ++i)
    {
        dmHullFace* pFace = m_newFaces[i];

        if (pFace->m_state == dmHullFace::e_normal)
        {
            while (MergeNonConvexFaces(pFace, 0));
        }
    }



    for (dm_int32 i = 0; i < m_horizonCount; ++i)
    {
        dmHullFace* pFace = m_newFaces[i];

        if (pFace->m_state == dmHullFace::e_concave)
        {
            pFace->m_state = dmHullFace::e_normal;
            while (MergeNonConvexFaces(pFace, 1));
        }
    }



    ResolveOrphanedVertices();



    m_horizonCount = 0;
    dmHullFace* pNuke = m_discardList;
    while (pNuke != NULL)
    {
        dmHullFace* pNext = pNuke->m_pNext;
        pNuke->Destroy();
        dmFree(pNuke);
        pNuke = pNext;
    }
    m_discardList = NULL;
}




void dmHullBuilder::CullUnusedVertices()
{
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        m_vertices[i].m_index = -1;
    }



    m_faceCount = 0;
    for (dmHullFace* pFace = m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {
        dmHullHalfEdge* pEdge = pFace->m_pEdge;
        do
        {
            pEdge->m_pVertex->m_index = 0;
            pEdge = pEdge->m_pNext;
        }
        while (pEdge != pFace->m_pEdge);
        ++m_faceCount;
    }

    dm_int32 count = 0;
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        if (m_vertices[i].m_index == 0)
        {
            m_vertices[i].m_index = count;
            ++count;
        }
    }


    for (dmHullFace* pFace = m_faceList; pFace != NULL; pFace = pFace->m_pNext)
    {
        dmHullHalfEdge* pEdge = pFace->m_pEdge;
        do
        {
            dm_int32 index = pEdge->m_pVertex->m_index;
            dmAssert(0 <= index);
            pEdge->m_pVertex = m_vertices + index;
            pEdge = pEdge->m_pNext;
        }
        while (pEdge != pFace->m_pEdge);
    }

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        dm_int32 index = m_vertices[i].m_index;
        if (index >= 0)
        {
            dmAssert(index <= i);
            m_vertices[index] = m_vertices[i];
        }
    }

    m_vertexCount = count;
}
