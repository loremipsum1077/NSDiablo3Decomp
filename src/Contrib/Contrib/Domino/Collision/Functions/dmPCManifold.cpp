#include "dmDistance.h"
#include "dmManifold.h"
#include "../../Include/dmPolytope.h"
#include <float.h>

extern const dm_vec4 dm_vec4_linearSlop; // Defined in Domino/Common/dmMath.cpp; referenced by Collide at line 378.










// Preserve the original filename passed by the assertion on line 244.
#line 19 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Functions/dmPCManifold.cpp"

struct dmPCVertex
{
    dm_vec4 v;
    dmContactID id;
};


struct dmPCAxis
{
    enum Type
    {
        e_faceVertexType,
        e_edgeType,
        e_unknownType
    };

    dmPCAxis() : separation(-dm_vec4_maxFloat), indexA(-1), type(e_unknownType) {}
    dm_vec4 separation;
    dm_int32 indexA;
    Type type;
};


static dm_int32 ClipSegment(dmPCVertex* pDst, const dmPCVertex* pSrc,
    dm_vec4Arg normal, dm_vec4Arg offset, dm_int32 edgeIndex)
{

    dm_int32 dstCount = 0;


    dm_vec4 distance1 = dmDot3(normal, pSrc[0].v) - offset;
    dm_vec4 distance2 = dmDot3(normal, pSrc[1].v) - offset;


    if (dmLEZ(distance1)) pDst[dstCount++] = pSrc[0];
    if (dmLEZ(distance2)) pDst[dstCount++] = pSrc[1];


    if (dmLTZ(distance1 * distance2))
    {

        dm_vec4 a = distance1 / (distance1 - distance2);
        pDst[dstCount].v = pSrc[0].v + a * (pSrc[1].v - pSrc[0].v);


        pDst[dstCount].id.key = 0;

        pDst[dstCount].id.fp.featureA = edgeIndex;
        pDst[dstCount].id.fp.featureB = 0;
        pDst[dstCount].id.fp.typeA = dmFeature::e_edge;
        pDst[dstCount].id.fp.typeB = dmFeature::e_edge;

        ++dstCount;
    }

    return dstCount;
}


struct dmPCCollider
{
    void Collide(dmManifold* pManifold,
        dmGJKCache* pCache,
        const dmPolytope* pPolytopeA,
        const dmCapsule* pCapsuleB);

    dm_vec4 SeparationFV(dm_int32 faceA);
    dm_vec4 SeparationEE(dm_int32 edge);

    bool IsMinkowskiFace(dm_int32 edgeA);

    void ComputeFV(dmManifold*);
    void ComputeEE(dmManifold*);

    void ComputeManifold(dmManifold*);

    const dmPolytope* m_pPolytopeA;
    dmCapsule m_capsuleB;

    dm_vec4 m_radiusA;
    dm_vec4 m_radiusB;

    dmPCAxis m_bestAxis;
};


dm_vec4 dmPCCollider::SeparationFV(dm_int32 faceA)
{

    dm_vec4 planeA = m_pPolytopeA->GetFace(faceA);

    dm_vec4 s1 = dmDot3(planeA, m_capsuleB.m_p1);
    dm_vec4 s2 = dmDot3(planeA, m_capsuleB.m_p2);

    if (dmLT(s1, s2))
    {
        return s1 - dmSplatW(planeA); // The two returns share the subtraction on line 120.
    }


    return s2 - dmSplatW(planeA);
}



dm_vec4 dmPCCollider::SeparationEE(dm_int32 edge)
{
    const dmSubEdge* pEdge = m_pPolytopeA->GetSubEdge(edge);
    const dmSubEdge* pTwin = pEdge->GetTwin();


    dm_vec4 a1 = m_pPolytopeA->GetVertex(pEdge->tail);
    dm_vec4 a2 = m_pPolytopeA->GetVertex(pTwin->tail);



    dm_vec4 ea = a2 - a1;
    dm_vec4 eb = m_capsuleB.m_p2 - m_capsuleB.m_p1;
    dm_vec4 normal = dmCross(ea, eb);

    if (dmLengthSqr3_f32(normal) < FLT_EPSILON)
    {
        return -dm_vec4_maxFloat;
    }

    normal = dmNormalize3(normal);

    dm_vec4 outward = a1 - m_pPolytopeA->GetCentroid();
    dm_vec4 direction = dmDot3(outward, normal);
    if (dmLTZ(direction))
    {
        normal = -normal;
    }

    dm_vec4 s = dmDot3(m_capsuleB.m_p1 - a1, normal);
    return s;
}


bool dmPCCollider::IsMinkowskiFace(dm_int32 edgeA)
{
    const dmSubEdge* pEdgeA = m_pPolytopeA->GetSubEdge(edgeA);
    const dmSubEdge* pTwinA = pEdgeA->GetTwin();


    dm_vec4 a = m_pPolytopeA->GetFace(pEdgeA->left);
    dm_vec4 b = m_pPolytopeA->GetFace(pTwinA->left);
    dm_vec4 c = m_capsuleB.m_p2 - m_capsuleB.m_p1;







    dm_vec4 ac = dmDot3(a, c);
    dm_vec4 bc = dmDot3(b, c);


    if (dmGEZ(ac * bc))
    {
        return false;
    }

    return true; // dmGEZ at line 179 rejects same-side faces, including a zero dot product.
}


void dmPCCollider::ComputeFV(dmManifold* pManifold)
{
    dm_int32 faceA = m_bestAxis.indexA;


    dmPCVertex segment[2];

    segment[0].v = m_capsuleB.m_p1;
    segment[0].id.key = 0;

    segment[0].id.fp.featureA = faceA;
    segment[0].id.fp.featureB = 0;
    segment[0].id.fp.typeA = dmFeature::e_face;
    segment[0].id.fp.typeB = dmFeature::e_vertex;

    segment[1].v = m_capsuleB.m_p2;
    segment[1].id.key = 0;

    segment[1].id.fp.featureA = faceA;
    segment[1].id.fp.featureB = 1;
    segment[1].id.fp.typeA = dmFeature::e_face;
    segment[1].id.fp.typeB = dmFeature::e_vertex;







    dmPCVertex buffer[2];

    dmPCVertex* source = segment;
    dmPCVertex* destination = buffer;

    dm_vec4 plane = m_pPolytopeA->GetFace(faceA);

    dm_int32 edge1 = m_pPolytopeA->GetFaceEdge(faceA);
    dm_vec4 p1 = m_pPolytopeA->GetTailVertex(edge1);
    dm_int32 terminalEdge = edge1;

    dm_int32 count = 0;
    do
    {
        dm_int32 edge2 = m_pPolytopeA->GetNextEdge(edge1);
        dm_vec4 p2 = m_pPolytopeA->GetTailVertex(edge2);

        dm_vec4 d = p2 - p1;
        dm_vec4 sideNormal = dmCross(d, plane);

        if (!dmIsZeroSafe3(sideNormal))
        {
            sideNormal = dmNormalize3(sideNormal);

            dm_vec4 sideOffset = dmDot3(sideNormal, p1);
            dm_int32 edgeA = m_pPolytopeA->GetPrimaryEdge(edge1);
            count = ClipSegment(destination, source, sideNormal, sideOffset, edgeA);
            dmAssert(count <= 2);
            dmSwap(source, destination); // As in dmTCManifold, the next clip reads the prior destination.
        }

        edge1 = edge2;
        p1 = p2;
    } while (edge1 != terminalEdge && count == 2);


    if (count != 2)
    {
        return;
    }

    dm_vec4 totalRadius = m_radiusA + m_radiusB;
    dm_int32 pointCount = 0;
    for (dm_int32 i = 0; i < 2; ++i) // The shared guard fixes the clipped segment at two vertices.
    {
        dmPCVertex* clip = source + i;

        dm_vec4 separation = dmDot3(plane, clip->v) - dmSplatW(plane) - totalRadius;

        if (dmLEZ(separation))
        {

            dm_vec4 p = clip->v - m_radiusB * plane;
            pManifold->m_points[pointCount].point = p;
            pManifold->m_points[pointCount].separation = dmF32(separation);
            pManifold->m_points[pointCount].id = clip->id;
            ++pointCount;
        }
    }

    pManifold->m_normal = plane;
    pManifold->m_pointCount = (dm_uint16)pointCount;
}


void dmPCCollider::ComputeEE(dmManifold* pManifold)
{
    dm_int32 edgeA = m_bestAxis.indexA;
    dm_int32 twinA = m_pPolytopeA->GetTwinIndex(edgeA);

    dmCapsule segmentA, segmentB;
    // The two capsule radii are cleared before the endpoint assignments, as in the matching constructor and the line-293 store.
    segmentA.m_radius = 0.0f;
    segmentA.m_p1 = m_pPolytopeA->GetTailVertex(edgeA);
    segmentA.m_p2 = m_pPolytopeA->GetTailVertex(twinA);

    segmentB.m_radius = 0.0f;
    segmentB.m_p1 = m_capsuleB.m_p1;
    segmentB.m_p2 = m_capsuleB.m_p2;

    dm_vec4 c1, c2;
    dm_vec4 distance = dmDistance(&c1, &c2, &segmentA, &segmentB);


    dm_vec4 d1 = segmentA.m_p2 - segmentA.m_p1;
    dm_vec4 d2 = segmentB.m_p2 - segmentB.m_p1;
    dm_vec4 axis = dmCross(d1, d2);

    if (dmLengthSqr3_f32(axis) < FLT_EPSILON)
    {
        return;
    }

    axis = dmNormalize3(axis);

    dm_vec4 d = segmentA.m_p1 - m_pPolytopeA->GetCentroid();
    dm_vec4 da = dmDot3(d, axis);

    dm_vec4 normal;
    if (dmLTZ(da))
    {
        normal = -axis;
    }
    else
    {
        normal = axis;
    }

    dm_vec4 separation = dmDot3(c2 - c1, normal) - (m_radiusA + m_radiusB);


    pManifold->m_pointCount = 1;
    pManifold->m_normal = normal;

    dm_vec4 point = dm_vec4_oneHalf * (c1 + c2);
    pManifold->m_points[0].point = point;
    pManifold->m_points[0].separation = dmF32(separation);

    pManifold->m_points[0].id.key = 0;

    pManifold->m_points[0].id.fp.featureA = edgeA;
    pManifold->m_points[0].id.fp.featureB = 0;
    pManifold->m_points[0].id.fp.typeA = dmFeature::e_edge;
    pManifold->m_points[0].id.fp.typeB = dmFeature::e_edge;
}


void dmPCCollider::Collide(dmManifold* pManifold,
    dmGJKCache* pCache,
    const dmPolytope* pPolytopeA,
    const dmCapsule* pCapsuleB)
{
    pManifold->m_pointCount = 0;

    dmDistanceInput input;
    input.cache = *pCache;
    input.proxyA.Set(pPolytopeA);
    input.proxyB.Set(pCapsuleB);
    input.transformA = dmTransformIdentity();
    input.transformB = dmTransformIdentity();
    input.useRadii = false;

    dmDistanceOutput output;

    dmDistance(&output, &input);
    *pCache = output.cache;

    m_radiusA = dmV4(pPolytopeA->m_radius);
    m_radiusB = dmV4(pCapsuleB->m_radius);
    dm_vec4 totalRadius = m_radiusA + m_radiusB;


    if (dmGT(output.distance, totalRadius))
    {
        return;
    }




    const dm_vec4 k_edgeTol = dmV4(0.9f);
    const dm_vec4 k_absoluteTol = dm_vec4_oneHalf * dm_vec4_linearSlop;

    m_pPolytopeA = pPolytopeA;
    m_capsuleB = *pCapsuleB;

    dm_vec4 normal = output.pointB - output.pointA;

    if (dmIsZeroSafe3(normal) || dmLT(output.distance, m_radiusA * dm_vec4_oneHalf) || output.cache.GetCount() == 4)
    {


        for (dm_int32 i = 0; i < pPolytopeA->GetFaceCount(); ++i)
        {
            dm_int32 faceA = i;

            dm_vec4 s = SeparationFV(faceA);
            if (dmGT(s, totalRadius))
            {
                return;
            }

            if (dmGT(s, m_bestAxis.separation))
            {
                m_bestAxis.type = dmPCAxis::e_faceVertexType;
                m_bestAxis.separation = s;
                m_bestAxis.indexA = faceA;
            }
        }

        for (dm_int32 i = 0; i < pPolytopeA->GetSubEdgeCount(); i += 2)
        {
            dm_int32 edgeA = i;

            bool test = IsMinkowskiFace(edgeA);
            if (!test)
            {
                continue;
            }

            dm_vec4 s = SeparationEE(edgeA);

            if (dmGT(s, totalRadius))
            {
                return;
            }

            if (dmGT(s, k_edgeTol * m_bestAxis.separation + k_absoluteTol))
            {
                m_bestAxis.type = dmPCAxis::e_edgeType;
                m_bestAxis.separation = s;
                m_bestAxis.indexA = edgeA;
            }
        }


        if (m_bestAxis.type == dmPCAxis::e_faceVertexType)
        {
            ComputeFV(pManifold);
        }
        else if (m_bestAxis.type == dmPCAxis::e_edgeType)
        {
            ComputeEE(pManifold);
        }
        return;
    }


    normal = dmNormalize3(normal);

    dm_float32 maxDot = -FLT_MAX; // The original loads the negative maximum float directly from its constant pool.
    dm_int32 bestFace = 0;

    for (dm_int32 i = 0; i < pPolytopeA->GetFaceCount(); ++i)
    {
        dm_vec4 n = pPolytopeA->GetFace(i);
        dm_float32 dot = dmDot3_f32(normal, n);
        if (dot > maxDot)
        {
            maxDot = dot;
            bestFace = i;
        }
    }

    const dm_float32 k_cos5Degrees = 0.9961947f;
    if (maxDot > k_cos5Degrees)
    {

        m_bestAxis.type = dmPCAxis::e_faceVertexType;
        m_bestAxis.indexA = bestFace;
        ComputeFV(pManifold);
    }

    if (pManifold->m_pointCount == 0)
    {



        dmFeaturePair fp;


        dm_int32 iA1, iA2 = -1, iB1, iB2 = -1; // The absent second feature uses -1 in the original selection.
        dm_int32 uniqueCountA = 1, uniqueCountB = 1;
        iA1 = output.cache.indexAs[0];
        iB1 = output.cache.indexBs[0];

        dm_uint8 count = output.cache.GetCount();
        if (count == 2)
        {
            if (output.cache.indexAs[1] != iA1)
            {
                iA2 = output.cache.indexAs[1];
                ++uniqueCountA;
            }

            if (output.cache.indexBs[1] != iB1)
            {
                iB2 = output.cache.indexBs[1];
                ++uniqueCountB;
            }
        }
        else if (count == 3)
        {
            if (output.cache.indexAs[1] != iA1)
            {
                iA2 = output.cache.indexAs[1];
                ++uniqueCountA;
            }
            else if (output.cache.indexAs[2] != iA1)
            {
                iA2 = output.cache.indexAs[2];
                ++uniqueCountA;
            }

            if (output.cache.indexBs[1] != iB1)
            {
                iB2 = output.cache.indexBs[1];
                ++uniqueCountB;
            }
            else if (output.cache.indexBs[2] != iB1)
            {
                iB2 = output.cache.indexBs[2];
                ++uniqueCountB;
            }
        }

        if (uniqueCountA == 1)
        {
            fp.featureA = iA1;
            fp.typeA = dmFeature::e_vertex;
        }
        else
        {

            dm_int32 edgeIndex = m_pPolytopeA->FindEdge(iA1, iA2);




            if (edgeIndex < 0)
            {
                fp.featureA = 0;
                fp.typeA = dmFeature::e_unknown;
            }
            else
            {
                fp.featureA = edgeIndex;
                fp.typeA = dmFeature::e_edge;
            }
        }


        if (uniqueCountB == 1)
        {
            fp.featureB = iB1;
            fp.typeB = dmFeature::e_vertex;
        }
        else
        {
            fp.featureB = 0;
            fp.typeB = dmFeature::e_edge;
        }
        fp.triangleIndex = 0;

        dm_vec4 p = output.pointB - m_radiusB * normal;
        pManifold->m_pointCount = 1;
        pManifold->m_normal = normal;

        pManifold->m_points[0].id.fp = fp;
        pManifold->m_points[0].point = p;
        pManifold->m_points[0].separation = dmF32(output.distance - totalRadius);
    }
}


void dmPCManifold(dmManifold* pManifold,
    dmGJKCache* pCache,
    const dmPolytope* pPolytopeA,
    const dmCapsule* pCapsuleB)
{
    dmPCCollider collider;
    collider.Collide(pManifold, pCache, pPolytopeA, pCapsuleB);
}
