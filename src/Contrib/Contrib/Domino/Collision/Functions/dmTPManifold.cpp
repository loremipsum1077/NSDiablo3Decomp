// Original file: Contrib/Contrib/Domino/Collision/Functions/dmTPManifold.cpp
// Triangle versus polytope contact manifold (SAT with clipping).
#line 4 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Functions/dmTPManifold.cpp"
#include "dmDistance.h"
#include "dmManifold.h"
#include "../../Include/dmMath.h"
#include "../../Include/dmPolytope.h"
#include "../../Include/dmPolytopeData.h"
#include "../../Include/dmSerialize.h"
#include "../../Include/dmShapes.h"
#include "../../Include/dmStructures.h"

struct dmTPVertex
{
    dm_vec4 v;
    dm_int32 edgeIndex;
    dmContactID id;
};
extern const dm_vec4 dm_vec4_linearSlop; // Defined in Common/dmMath.cpp; not declared in src headers (as in dmSimpleHull.cpp).
struct dmEdgePair
{
    dm_int32 indexA;
    dm_int32 indexB;
};

struct dmTPAxis
{
    enum Type
    {
        e_unknownType = 0,
        e_faceA = 1,
        e_faceB = 2,
        e_edgeType = 3
    };



    dmTPAxis() { Clear(); }

    void Clear()
    {
        separation = -dm_vec4_maxFloat;
        indexA = -1;
        indexB = -1;
        type = e_unknownType;
    }

    dm_vec4 separation;
    dm_int32 indexA;
    dm_int32 indexB;
    Type type;
};









static dm_int32 sClipPolygon(dmTPVertex * pDst, const dmTPVertex * pSrc, dm_int32 srcCount, dm_vec4Arg normal, dm_vec4Arg offset, dm_int32 edgeIndex)
{
    dm_int32 dstCount = 0;




    if (srcCount > 2)
    {
        // Clip each edge (i1, i2) against the plane.
        dm_int32 i1 = srcCount - 1;
        for (dm_int32 i2 = 0; i2 < srcCount; ++i2)
        {
            // Signed distances of the edge end points.
            dm_vec4 distance1 = dmDot3(normal, pSrc[i1].v) - offset;
            dm_vec4 distance2 = dmDot3(normal, pSrc[i2].v) - offset;


            if (dmLEZ(distance1))
            {
                pDst[dstCount++] = pSrc[i1];
            }


            if (dmLTZ(distance1 * distance2))
            {
                // The edge crosses the plane.
                pDst[dstCount].v = pSrc[i1].v + distance1 / (distance1 - distance2) *
                    (pSrc[i2].v - pSrc[i1].v);


                pDst[dstCount].id.key = 0;

                pDst[dstCount].id.fp.featureA = (dm_uint8)edgeIndex;
                pDst[dstCount].id.fp.featureB = (dm_uint8)pSrc[i1].edgeIndex;
                pDst[dstCount].id.fp.typeA = 1; pDst[dstCount].id.fp.typeB = 1;

                pDst[dstCount].edgeIndex = pSrc[i1].edgeIndex;

                ++dstCount;
            }

            i1 = i2;
        }
    }
    else switch (srcCount)
    {
    case 2:
        {
            dm_vec4 distance1 = dmDot3(normal, pSrc[0].v) - offset;
            dm_vec4 distance2 = dmDot3(normal, pSrc[1].v) - offset;

            if (dmLEZ(distance1)) pDst[dstCount++] = pSrc[0];
            if (dmLEZ(distance2)) pDst[dstCount++] = pSrc[1];

            if (dmLTZ(distance1 * distance2))
            {
                // The edge crosses the plane.
                pDst[dstCount].v = pSrc[0].v + distance1 / (distance1 - distance2) *
                    (pSrc[1].v - pSrc[0].v);

                pDst[dstCount].id.key = 0;

                pDst[dstCount].id.fp.featureA = (dm_uint8)edgeIndex;
                pDst[dstCount].id.fp.featureB = (dm_uint8)pSrc[0].edgeIndex;
                pDst[dstCount].id.fp.typeA = 1; pDst[dstCount].id.fp.typeB = 1;

                ++dstCount;
            }
        }
        break;

    case 1:
        {
            dm_vec4 distance = dmDot3(normal, pSrc[0].v) - offset;
            if (dmLEZ(distance))
            {
                pDst[dstCount++] = pSrc[0];
            }
        }
        break;
    }

    return dstCount;
}


struct dmTPCollider
{
    void Collide(dmManifold * pManifold, dmSATCache * pCache, const dmTriangle * pTriangleA, const dmPolytope * pPolytopeB, const dmTransform & xfB);
    bool TestEdges(dmSATCache * pCache);
    void TestFaceA(dmTPAxis * pAxis, dm_int32 faceIndexA) const;
    dm_vec4 TestFaceB(dmTPAxis * pAxis, dm_int32 faceIndexB) const;
    dm_vec4 TestEdgePair(dmTPAxis * pAxis, const dmEdgePair & pair) const;
    bool IsMinkowskiFace(const dmEdgePair & pair) const;
    bool CollideFaceA(dmManifold * pManifold, const dmTPAxis & axis) const;
    bool CollideFaceB(dmManifold * pManifold, const dmTPAxis & axis) const;
    bool CollideEdges(dmManifold * pManifold, const dmTPAxis & axis, bool cached) const;
    void ComputeManifold(dmManifold * pManifold, const dmTPAxis & axis) const;

    dmMtx m_transformB;
    dmMtx m_invTransformB;
    const dmPartialPolytope * m_pPolytopeA;
    const dmPolytope * m_pPolytopeB;
    dm_vec4 m_totalRadius;
    dm_int32 m_triangleIndex;
    dmTPAxis m_faceAxis;
    dmTPAxis m_edgeAxis;
};






void dmManifold_ReducePoints(dmManifold*, const dmManifoldPoint*, dm_int32); // dmManifold.cpp: DWARF signature




inline void dmTPCollider::TestFaceA(dmTPAxis * pAxis, dm_int32 faceIndexA) const
{

	dmAssert(0 <= faceIndexA && faceIndexA < m_pPolytopeA->m_planeCount);
	dm_vec4 planeA = m_pPolytopeA->m_aPlanes[faceIndexA];


	dm_vec4 axisB = -dmMul33(m_invTransformB, planeA);


	dm_vec4 vertexB;
	dm_int32 vertexIndexB = m_pPolytopeB->GetSupport(&vertexB, axisB);


	dm_vec4 vertexBinA = dmMul34(m_transformB, vertexB);


	dm_vec4 s = dmDot3(planeA, vertexBinA) - dmSplatW(planeA) - m_totalRadius;


	pAxis->indexA = faceIndexA;
	pAxis->indexB = vertexIndexB;
	pAxis->type = dmTPAxis::e_faceA;
	pAxis->separation = s;
}


inline dm_vec4 dmTPCollider::TestFaceB(dmTPAxis * pAxis, dm_int32 faceIndexB) const
{

	dm_vec4 planeB = m_pPolytopeB->GetFace(faceIndexB);


	dm_vec4 axisA = -dmMul33(m_transformB, planeB);


	dm_vec4 vertexA;
	dm_int32 vertexIndexA = m_pPolytopeA->GetSupport(&vertexA, axisA);


	dm_vec4 vertexAinB = dmMul34(m_invTransformB, vertexA);


	dm_vec4 s = dmDot3(planeB, vertexAinB) - dmSplatW(planeB) - m_totalRadius;


	pAxis->indexA = vertexIndexA;
	pAxis->indexB = faceIndexB;
	pAxis->type = dmTPAxis::e_faceB;
	pAxis->separation = s;
	return s;
}



inline dm_vec4 dmTPCollider::TestEdgePair(dmTPAxis * pAxis, const dmEdgePair & pair) const
{
	pAxis->indexA = pair.indexA;
	pAxis->indexB = pair.indexB;
	pAxis->type = dmTPAxis::e_edgeType;

	const dmSubEdge * pEdgeB = m_pPolytopeB->GetSubEdge(pair.indexB);
	const dmSubEdge * pTwinB = pEdgeB + pEdgeB->twinOffset; // Inference: same twin form as GetEdgeVector, so it CSEs with IsMinkowskiFace; GetSubEdge/GetVertex here are CSE'd with IsMinkowskiFace.


	dm_vec4 a1 = m_pPolytopeA->m_aVertices[pair.indexA];
	dm_vec4 a2 = m_pPolytopeA->m_aVertices[(pair.indexA + 1) % 3];
	dm_vec4 b1 = m_pPolytopeB->GetVertex(pEdgeB->tail);
	dm_vec4 b2 = m_pPolytopeB->GetVertex(pTwinB->tail);


	dm_vec4 ea = a2 - a1;
	dm_vec4 eb = dmMul33(m_transformB, b2 - b1);
	dm_vec4 normalA = dmCross(ea, eb);

	if (dmIsZeroSafe3(normalA))
	{
		// Inference: the shared fsub at +0xb94 negates dm_vec4_maxFloat on this path.
		return -dm_vec4_maxFloat;
	}

	normalA = dmNormalize3(normalA);

	dm_vec4 outwardA = a1 - m_pPolytopeA->m_centroid;
	dm_vec4 direction = dmDot3(normalA, outwardA);
	if (dmLTZ(direction))
	{
		normalA = -normalA;
	}

	dm_vec4 s = dmDot3(normalA, dmMul34(m_transformB, b1) - a1) - m_totalRadius;
	return s;
}






inline bool dmTPCollider::IsMinkowskiFace(const dmEdgePair & pair) const
{
	const dmSubEdge * pEdgeB = m_pPolytopeB->GetSubEdge(pair.indexB);
	const dmSubEdge * pTwinB = pEdgeB + 1; // Inference: twin left is loaded at pEdgeB[1].left; no GetSubEdge inline at 284.


	dm_int32 ia1 = pair.indexA;
	dm_int32 ia2 = (pair.indexA + 1) % 3;
	dm_vec4 a = -m_pPolytopeA->m_aPlanes[0];
	dm_vec4 b = -m_pPolytopeA->m_aPlanes[ia1 + 1];
	dm_vec4 dirA = m_pPolytopeA->m_aVertices[ia2] - m_pPolytopeA->m_aVertices[ia1];

	dm_vec4 c = m_pPolytopeB->GetFace(pEdgeB->left);
	dm_vec4 d = m_pPolytopeB->GetFace(pTwinB->left);
	dm_vec4 dirB = m_pPolytopeB->GetEdgeVector(pair.indexB);






	c = dmMul33(m_transformB, c);
	d = dmMul33(m_transformB, d);
	dirB = dmMul33(m_transformB, dirB);




	dm_vec4 ba = dirA;
	dm_vec4 dc = dirB;
	dm_vec4 cba = dmDot3(c, ba);
	dm_vec4 dba = dmDot3(d, ba);
	dm_vec4 adc = dmDot3(a, dc);
	dm_vec4 bdc = dmDot3(b, dc);

	if (dmGEZ(cba * dba))
	{
		return false;
	}

	if (dmGEZ(adc * bdc))
	{
		return false;
	}

	if (dmLEZ(cba * bdc))
	{
		return false;
	}

	return true;
}


bool dmTPCollider::CollideFaceA(dmManifold* pManifold, const dmTPAxis& axis) const
{
    dm_int32 faceA = axis.indexA;


    if (faceA != 0)
        return false;



    dm_vec4 axisA = m_pPolytopeA->m_aPlanes[0];
    dm_vec4 axisAinB = -dmMul33(m_invTransformB, axisA);
    dm_int32 faceB = m_pPolytopeB->GetSupportFace(axisAinB, axis.indexB);


    const dm_int32 k_maxPoints = 32;


    dmTPVertex incidentFace[64];

    dm_int32 edge1 = m_pPolytopeB->GetFaceEdge(faceB);
    dm_int32 edge2 = m_pPolytopeB->GetNextEdge(edge1);
    dm_int32 terminalEdge = edge2;

    dm_int32 count = 0;

    do
    {
        dmTPVertex* pVertex = &incidentFace[count];

        const dmSubEdge* pEdge1 = m_pPolytopeB->GetSubEdge(edge1);
        dm_vec4 v = m_pPolytopeB->GetVertex(pEdge1->tail);
        pVertex->v = dmMul34(m_transformB, v);
        pVertex->edgeIndex = m_pPolytopeB->GetPrimaryEdge(edge1);

        pVertex->id.key = 0;
        pVertex->id.fp.featureA = (dm_uint8)faceA;
        pVertex->id.fp.featureB = pEdge1->tail;
        pVertex->id.fp.typeA = dmFeature::e_face;
        pVertex->id.fp.typeB = dmFeature::e_vertex;
        pVertex->id.fp.triangleIndex = m_triangleIndex;

        edge1 = edge2;
        edge2 = m_pPolytopeB->GetNextEdge(edge1);

        ++count;
    } while (edge2 != terminalEdge && count < k_maxPoints);








    dmTPVertex buffer[64];

    dmTPVertex* source = incidentFace;
    dmTPVertex* destination = buffer;

    dm_vec4 plane = m_pPolytopeA->m_aPlanes[0];
    dm_vec4 normal = plane;
    dm_vec4 offset = dmSplatW(plane);

    dm_vec4 p1 = m_pPolytopeA->m_aVertices[2];
    for (dm_int32 i = 0; i < 3; ++i)
    {

        dm_vec4 p2 = m_pPolytopeA->m_aVertices[i];
        dm_vec4 d = p2 - p1;
        dm_vec4 sideNormal = dmCross(d, normal);
        sideNormal = dmNormalize3(sideNormal);


        dm_vec4 sideOffset = dmDot3(sideNormal, p1) + m_totalRadius;
        count = sClipPolygon(destination, source, count, sideNormal, sideOffset, i);
        dmAssert(count <= k_maxPoints);
        dmSwap(source, destination);
        p1 = p2;
    }


    dmManifoldPoint points[32];
    dm_int32 pointCount = 0;
    for (dm_int32 i = 0; i < count; ++i)
    {
        dmTPVertex* pVertex = &source[i];

        dm_vec4 separation = dmDot3(normal, pVertex->v) - offset;

        if (dmLE(separation, m_totalRadius) && pointCount < k_maxPoints)
        {

            points[pointCount].point = pVertex->v - separation * normal;
            points[pointCount].separation = dmF32(separation - m_totalRadius);
            points[pointCount].id = pVertex->id;
            points[pointCount].normalImpulse = 0.0f;
            ++pointCount;
        }
    }

    pManifold->m_normal = normal;


    if (pointCount > 4)
    {
        dmManifold_ReducePoints(pManifold, points, pointCount);
    }
    else
    {
        for (dm_int32 i = 0; i < pointCount; ++i)
        {
            pManifold->m_points[i].separation = points[i].separation;
            pManifold->m_points[i].point = points[i].point;
            pManifold->m_points[i].id = points[i].id;
        }
        pManifold->m_pointCount = (dm_uint16)pointCount;
    }

    return true;
}


bool dmTPCollider::CollideFaceB(dmManifold * pManifold, const dmTPAxis & axis) const
{
    dm_int32 faceB = axis.indexB;
    dm_vec4 plane = m_pPolytopeB->GetFace(faceB);
    dm_vec4 normal = plane;
    dm_vec4 normalA = -dmMul33(m_transformB, normal);

    if (!m_pPolytopeA->CheckAxis(normalA))
        return false;





    const dm_int32 k_maxPoints = 32;


    dmTPVertex incidentFace[64];
    dm_int32 count = 3;

    for (dm_int32 i = 0; i < 3; ++i)
    {
        dmTPVertex* pVertex = &incidentFace[i];

        dm_vec4 v = m_pPolytopeA->m_aVertices[i];
        pVertex->v = dmMul34(m_invTransformB, v);
        pVertex->edgeIndex = i;

        pVertex->id.key = 0;
        pVertex->id.fp.featureA = (dm_uint8)faceB;
        pVertex->id.fp.featureB = (dm_uint8)i;
        pVertex->id.fp.typeA = dmFeature::e_face;
        pVertex->id.fp.typeB = dmFeature::e_vertex;
        pVertex->id.fp.triangleIndex = m_triangleIndex;
    }







    dmTPVertex buffer[64];

    dmTPVertex* source = incidentFace;
    dmTPVertex* destination = buffer;

    dm_vec4 offset = dmSplatW(plane);

    dm_int32 edge1 = m_pPolytopeB->GetFaceEdge(faceB);
    dm_vec4 p1 = m_pPolytopeB->GetTailVertex(edge1);
    dm_int32 terminalEdge = edge1;

    do
    {
        dm_int32 edge2 = m_pPolytopeB->GetNextEdge(edge1);
        dm_vec4 p2 = m_pPolytopeB->GetTailVertex(edge2);

        dm_vec4 d = p2 - p1;
        dm_vec4 sideNormal = dmCross(d, normal);
        sideNormal = dmNormalize3(sideNormal);


        dm_vec4 sideOffset = dmDot3(sideNormal, p1) + m_totalRadius;
        count = sClipPolygon(destination, source, count, sideNormal, sideOffset, m_pPolytopeB->GetPrimaryEdge(edge1));
        dmAssert(count <= k_maxPoints);
        dmSwap(source, destination);
        edge1 = edge2;
        p1 = p2;
    }
    while (edge1 != terminalEdge);


    dmManifoldPoint points[32];
    dm_int32 pointCount = 0;
    for (dm_int32 i = 0; i < count; ++i)
    {
        dmTPVertex* pVertex = &source[i];

        dm_vec4 separation = dmDot3(normal, pVertex->v) - offset;

        if (dmLE(separation, m_totalRadius) && pointCount < k_maxPoints)
        {

            points[pointCount].point = pVertex->v - separation * normal;
            points[pointCount].separation = dmF32(separation - m_totalRadius);
            points[pointCount].id = pVertex->id;
            points[pointCount].normalImpulse = 0.0f;
            ++pointCount;
        }
    }

    pManifold->m_normal = normal;


    if (pointCount > 4)
    {
        dmManifold_ReducePoints(pManifold, points, pointCount);
    }
    else
    {
        for (dm_int32 i = 0; i < pointCount; ++i)
        {
            pManifold->m_points[i].separation = points[i].separation;
            pManifold->m_points[i].point = points[i].point;
            pManifold->m_points[i].id = points[i].id;
        }
        pManifold->m_pointCount = (dm_uint16)pointCount;
    }


    pManifold->m_normal = -dmMul33(m_transformB, pManifold->m_normal);

    for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i)
    {

        dmFeaturePair fp1 = pManifold->m_points[i].id.fp;
        dmFeaturePair fp2;
        fp2.featureA = fp1.featureB;
        fp2.featureB = fp1.featureA;
        fp2.typeA = fp1.typeB;
        fp2.typeB = fp1.typeA;
        fp2.triangleIndex = 0;
        pManifold->m_points[i].id.fp = fp2;


        pManifold->m_points[i].point = dmMul34(m_transformB, pManifold->m_points[i].point);
    }

    return true;
}


bool dmTPCollider::CollideEdges(dmManifold * pManifold, const dmTPAxis & axis, bool cached) const
{
    pManifold->m_pointCount = 0;

    dm_int32 edgeB = axis.indexB;
    dm_int32 twinB = m_pPolytopeB->GetTwinIndex(edgeB);

    dmCapsule segmentA, segmentB;

    segmentA.m_radius = 0.0f;
    segmentA.m_p1 = m_pPolytopeA->m_aVertices[axis.indexA];
    segmentA.m_p2 = m_pPolytopeA->m_aVertices[(axis.indexA + 1) % 3];

    segmentB.m_radius = 0.0f;
    segmentB.m_p1 = m_pPolytopeB->GetTailVertex(edgeB);
    segmentB.m_p2 = m_pPolytopeB->GetTailVertex(twinB);


    segmentB.m_p1 = dmMul34(m_transformB, segmentB.m_p1);
    segmentB.m_p2 = dmMul34(m_transformB, segmentB.m_p2);

    dm_vec4 d1 = segmentA.m_p2 - segmentA.m_p1;
    dm_vec4 d2 = segmentB.m_p2 - segmentB.m_p1;
    dm_vec4 normal = dmCross(d1, d2);


    if (dmIsZeroSafe3(normal))
    {
        return false;
    }

    normal = dmNormalize3(normal);


    dm_vec4 d = segmentA.m_p1 - m_pPolytopeA->m_centroid;
    dm_vec4 da = dmDot3(d, normal);

    if (dmLTZ(da))
    {
        normal = -normal;
    }


    if (m_pPolytopeA->CheckAxis(normal) == false)
    {
        return false;
    }

    dm_vec4 c1, c2;
    dm_vec4 distance = dmDistance(&c1, &c2, &segmentA, &segmentB);


    if (cached && dmGT(dm_vec4_oneHalf * distance, dmAbs(axis.separation + m_totalRadius)))
    {
        return true;
    }

    dm_vec4 separation = dmDot3(c2 - c1, normal) - m_totalRadius;

    pManifold->m_pointCount = 1;
    pManifold->m_normal = normal;

    dm_vec4 point = dm_vec4_oneHalf * (c1 + c2);
    pManifold->m_points[0].point = point;
    pManifold->m_points[0].separation = dmF32(separation);

    pManifold->m_points[0].id.key = 0;

    pManifold->m_points[0].id.fp.featureA = axis.indexA;
    pManifold->m_points[0].id.fp.featureB = axis.indexB;
    pManifold->m_points[0].id.fp.typeA = dmFeature::e_edge;
    pManifold->m_points[0].id.fp.typeB = dmFeature::e_edge;
    pManifold->m_points[0].id.fp.triangleIndex = m_triangleIndex;

    return true;
}

inline void dmTPCollider::ComputeManifold(dmManifold * pManifold, const dmTPAxis & axis) const
{
	switch (axis.type)
	{
	case dmTPAxis::e_faceA:
		CollideFaceA(pManifold, axis);
		break;

	case dmTPAxis::e_faceB:
		CollideFaceB(pManifold, axis);
		break;

	case dmTPAxis::e_edgeType:
		CollideEdges(pManifold, axis, false);
		break;

	default:
		dmAssert(false);
	}
}


bool dmTPCollider::TestEdges(dmSATCache * pCache)
{
	m_edgeAxis.Clear();

	struct EdgeCache
	{
		dm_vec4 leftNormal;
		dm_vec4 rightNormal;
		dm_vec4 edgeVector;
	};







	EdgeCache aEdges[3];
	aEdges[0].leftNormal = m_pPolytopeA->m_aPlanes[0];
	aEdges[0].rightNormal = m_pPolytopeA->m_aPlanes[1];
	aEdges[0].edgeVector = m_pPolytopeA->m_aVertices[1] - m_pPolytopeA->m_aVertices[0];

	aEdges[1].leftNormal = m_pPolytopeA->m_aPlanes[0];
	aEdges[1].rightNormal = m_pPolytopeA->m_aPlanes[2];
	aEdges[1].edgeVector = m_pPolytopeA->m_aVertices[2] - m_pPolytopeA->m_aVertices[1];

	aEdges[2].leftNormal = m_pPolytopeA->m_aPlanes[0];
	aEdges[2].rightNormal = m_pPolytopeA->m_aPlanes[3];
	aEdges[2].edgeVector = m_pPolytopeA->m_aVertices[0] - m_pPolytopeA->m_aVertices[2];

	const dm_int32 subEdgeCountB = m_pPolytopeB->GetSubEdgeCount();
	for (dm_int32 indexB = 0; indexB < subEdgeCountB; indexB += 2)
	{
		const dmSubEdge * pEdgeB = m_pPolytopeB->GetSubEdge(indexB);
		const dmSubEdge * pTwinB = pEdgeB + 1; // Inference: twin left is loaded at pEdgeB + 4 bytes; no GetTwin/GetSubEdge inline at 724.
		dm_vec4 c = m_pPolytopeB->GetFace(pEdgeB->left);
		dm_vec4 d = m_pPolytopeB->GetFace(pTwinB->left);
		dm_vec4 dirB = m_pPolytopeB->GetEdgeVector(indexB);


		c = dmMul33(m_transformB, c);
		d = dmMul33(m_transformB, d);
		dirB = dmMul33(m_transformB, dirB);


		// Inference: dc has no row of its own; the dot products at 753/754 use the transformed dirB.
		dm_vec4 dc = dirB;

		for (dm_int32 indexA = 0; indexA < 3; ++indexA)
		{

			EdgeCache * pEdgeCache = aEdges + indexA;
			dm_vec4 a = pEdgeCache->leftNormal;
			dm_vec4 b = pEdgeCache->rightNormal;



			dm_vec4 dirA = pEdgeCache->edgeVector;



			dm_vec4 cba = dmDot3(c, dirA);
			dm_vec4 dba = dmDot3(d, dirA);
			dm_vec4 adc = dmDot3(a, dc);
			dm_vec4 bdc = dmDot3(b, dc);

			dm_vec4 test1 = cba * dba;
			dm_vec4 test2 = adc * bdc;
			dm_vec4 test3 = cba * bdc;

			dm_vec4 m = dmMax(test1, test2, test3);
			if (dmGEZ(m))
			{
				continue;
			}

			dm_vec4 normalA = dmCross(dirA, dirB);

			if (dmIsZeroSafe3(normalA))
			{
				continue;
			}

			normalA = dmNormalize3(normalA);





			dm_vec4 a1 = m_pPolytopeA->m_aVertices[indexA];
			dm_vec4 b1 = m_pPolytopeB->GetTailVertex(indexB);
			dm_vec4 outwardA = a1 - m_pPolytopeA->m_centroid;
			dm_vec4 outwardB = dmMul33(m_transformB, b1 - m_pPolytopeB->GetCentroid());
			dm_vec4 directionA = dmDot3(normalA, outwardA);
			dm_vec4 directionB = dmDot3(normalA, outwardB);
			// Inference 786-791: names useA/flipA/flipB and the zero splat are invented (the DWARF has no locals here and only dmOr at 791); this OR of two selects reproduces the original mvn/bsl exactly.
			dm_vec4 useA = dmCompareGT(dmAbs(directionA), dmAbs(directionB));
			dm_vec4 flipA = dmCompareLT(directionA, vdupq_n_f32(0.0f));
			dm_vec4 flipB = dmCompareGT(directionB, vdupq_n_f32(0.0f));


			dm_vec4 mask = dmOr(dmSelect(useA, vdupq_n_f32(0.0f), flipB), dmSelect(useA, flipA, vdupq_n_f32(0.0f)));
			dm_vec4 tempNormalA = dmSelect(mask, -normalA, normalA);
			normalA = tempNormalA;


			dm_vec4 s = dmDot3(normalA, dmMul34(m_transformB, b1) - a1) - m_totalRadius;

			if (dmGTZ(s))
			{

				pCache->type = dmTPAxis::e_edgeType;
				pCache->separation = dmF32(s);
				pCache->indexA = (dm_uint8)indexA;
				pCache->indexB = (dm_uint8)indexB;
				return false;
			}

			if (dmGT(s, m_edgeAxis.separation))
			{

				m_edgeAxis.type = dmTPAxis::e_edgeType;
				m_edgeAxis.separation = s;
				m_edgeAxis.indexA = indexA;
				m_edgeAxis.indexB = indexB;
			}
		}
	}

	return true;
}


void dmTPCollider::Collide(dmManifold * pManifold, dmSATCache * pCache,
	const dmTriangle * pTriangleA, const dmPolytope * pPolytopeB, const dmTransform & xfB)
{
	pManifold->m_pointCount = 0;


	{
		dm_vec4 v1 = pTriangleA->m_p[0];
		dm_vec4 v2 = pTriangleA->m_p[1];
		dm_vec4 v3 = pTriangleA->m_p[2];

		dm_vec4 c = dmMul(xfB, pPolytopeB->ComputeCentroid());

		dm_vec4 e1 = v2 - v1;
		dm_vec4 e2 = v3 - v2;
		dm_vec4 n = dmCross(e1, e2);


		if (dmLTZ(dmDot3(c - v1, n)))
		{
			return;
		}
	}

	dmPartialPolytope partialA;
	partialA.Create(pTriangleA);

	m_pPolytopeA = &partialA;
	m_pPolytopeB = pPolytopeB;

	m_triangleIndex = pTriangleA->m_triangleIndex;
	// Inference: 0.032808397f (1 cm in feet, the dmTriangle default radius) is a rodata literal; its name is unknown.
	m_totalRadius = dmV4(0.032808397f + pPolytopeB->m_radius);

	m_transformB = dmMtxFromTransform(xfB);
	m_invTransformB = dmInvertOrtho34(m_transformB);

	// Inference: 0.016404198f (0.5 cm in feet) below is a rodata literal; its name is unknown.
	if (pCache->type == dmTPAxis::e_faceA)
	{
		dm_int32 faceA = pCache->indexA;
		dmAssert(0 <= faceA && faceA < 4);
		dmTPAxis axis;
		TestFaceA(&axis, faceA);
		if (dmGTZ(axis.separation))
		{
			return;
		}

		if (dmAbs(dmF32(axis.separation) - pCache->separation) < 0.016404198f)
		{
			bool admissible = CollideFaceA(pManifold, axis);

			if (pManifold->m_pointCount > 0 || admissible == false)
			{
				return;
			}
		}
	}
	else if (pCache->type == dmTPAxis::e_faceB)
	{
		dm_int32 faceB = pCache->indexB;
		dmAssert(0 <= faceB && faceB < m_pPolytopeB->GetFaceCount());
		dmTPAxis axis;
		TestFaceB(&axis, faceB);
		if (dmGTZ(axis.separation))
		{
			return;
		}

		if (dmAbs(dmF32(axis.separation) - pCache->separation) < 0.016404198f)
		{
			bool admissible = CollideFaceB(pManifold, axis);

			if (pManifold->m_pointCount > 0 || admissible == false)
			{
				return;
			}
		}
	}
	else if (pCache->type == dmTPAxis::e_edgeType)
	{
		dm_int32 edgeA = pCache->indexA;
		dm_int32 edgeB = pCache->indexB;

		dmEdgePair pair;
		pair.indexA = edgeA;
		pair.indexB = edgeB;

		bool test = IsMinkowskiFace(pair);
		if (test)
		{
			dmTPAxis axis;
			axis.separation = TestEdgePair(&axis, pair);
			if (dmGTZ(axis.separation))
			{
				return;
			}

			if (dmAbs(dmF32(axis.separation) - pCache->separation) < 0.016404198f)
			{
				bool admissible = CollideEdges(pManifold, axis, true);

				if (pManifold->m_pointCount > 0 || admissible == false)
				{
					return;
				}
			}
		}
	}

	m_faceAxis.Clear();
	m_edgeAxis.Clear();




	const dm_vec4 k_absoluteTol = dm_vec4_oneHalf * dm_vec4_linearSlop;
	const dm_vec4 k_edgeTol = dm_vec4_linearSlop;

	for (dm_int32 i = 0; i < 4; ++i) // Inference: literal 4 = size of dmPartialPolytope::m_aPlanes; original loop is cmp 3 / b.lo
	{
		dm_int32 faceA = i;
		dmTPAxis axis;
		TestFaceA(&axis, faceA);

		if (dmGTZ(axis.separation))
		{

			pCache->type = dmTPAxis::e_faceA;
			pCache->separation = vgetq_lane_f32(axis.separation, 0);
			pCache->indexA = (dm_uint8)axis.indexA;
			pCache->indexB = (dm_uint8)axis.indexB;
			return;
		}


		if (dmGT(axis.separation, m_faceAxis.separation + k_absoluteTol))
		{
			m_faceAxis = axis;
		}
	}

	for (dm_int32 i = 0; i < m_pPolytopeB->GetFaceCount(); ++i)
	{
		dm_int32 faceB = i;
		dmTPAxis axis;
		TestFaceB(&axis, faceB);

		if (dmGTZ(axis.separation))
		{

			pCache->type = dmTPAxis::e_faceB;
			pCache->separation = vgetq_lane_f32(axis.separation, 0);
			pCache->indexA = (dm_uint8)axis.indexA;
			pCache->indexB = (dm_uint8)axis.indexB;
			return;
		}

		if (dmGT(axis.separation, m_faceAxis.separation + k_absoluteTol))
		{
			m_faceAxis = axis;
		}
	}

	if (TestEdges(pCache) == false)
	{
		return;
	}



	if (m_edgeAxis.type == dmTPAxis::e_edgeType && dmGT(m_edgeAxis.separation, m_faceAxis.separation + k_edgeTol))
	{
		ComputeManifold(pManifold, m_edgeAxis);
		pCache->separation = dmF32(m_edgeAxis.separation);
		pCache->type = (dm_uint8)m_edgeAxis.type;
		pCache->indexA = (dm_uint8)m_edgeAxis.indexA;
		pCache->indexB = (dm_uint8)m_edgeAxis.indexB;
	}
	else
	{
		ComputeManifold(pManifold, m_faceAxis);
		pCache->separation = dmF32(m_faceAxis.separation);
		pCache->type = (dm_uint8)m_faceAxis.type;
		pCache->indexA = (dm_uint8)m_faceAxis.indexA;
		pCache->indexB = (dm_uint8)m_faceAxis.indexB;
	}
}


void dmTPManifold(dmManifold * pManifold, dmSATCache * pCache,
                  const dmTriangle * pTriangle,
                  const dmPolytope * pPolytope,
                  const dmTransform & xfB)
{
    dmTPCollider collider;
    collider.Collide(pManifold, pCache, pTriangle, pPolytope, xfB);
}
