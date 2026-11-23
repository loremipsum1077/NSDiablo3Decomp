// Original file: Contrib/Contrib/Domino/Collision/Functions/dmPPManifold.cpp
#include "dmManifold.h"
#include "../../Include/dmPolytope.h"
#include "dmDistance.h" // dmDistance(capsule, capsule) used by CollideEdges.
extern const dm_vec4 dm_vec4_linearSlop; // Defined in Common/dmMath.cpp; no shared declaration in src.













dm_int32 dm_satCallCount, dm_satCacheTouchCount, dm_satCacheSeparatedCount;


struct dmPPVertex
{
	dm_vec4 v;
	dm_int32 edgeIndex;
	dmContactID id;
};


struct dmPPAxis
{
    enum Type
    {
        e_unknownType = 0,
        e_faceA = 1,
        e_faceB = 2,
        e_edgeType = 3
    };
    // No linked standalone body; construction is checkable in dmPPManifold's inline copy.
    dmPPAxis() { Clear(); }
    // No linked standalone body; these stores are checkable in dmPPManifold's inline copy.
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
struct dmEdgePair;

struct dmEdgePair
{
	dm_int32 indexA;
	dm_int32 indexB;
};


static dm_int32 ClipPolygon(dmPPVertex * pDst, const dmPPVertex * pSrc, dm_int32 srcCount,
	dm_vec4Arg normal, dm_vec4Arg offset, dm_int32 edgeIndex)
{

	dm_int32 dstCount = 0;


	if (srcCount > 2)
	{

		dm_int32 i1 = srcCount - 1;
		for (dm_int32 i2 = 0; i2 < srcCount; i1 = i2, i2++)
		{

			dm_vec4 distance1 = dmDot3(normal, pSrc[i1].v) - offset;
			dm_vec4 distance2 = dmDot3(normal, pSrc[i2].v) - offset;


			if (dmLEZ(distance1))
			{
				pDst[dstCount++] = pSrc[i1];
			}


			if (dmLTZ(distance1 * distance2))
			{

				dm_vec4 a = distance1 / (distance1 - distance2);
				pDst[dstCount].v = pSrc[i1].v + a * (pSrc[i2].v - pSrc[i1].v);


				pDst[dstCount].id.key = 0;

				pDst[dstCount].id.fp.featureA = (dm_uint8)edgeIndex;
				pDst[dstCount].id.fp.featureB = (dm_uint8)pSrc[i1].edgeIndex;
				pDst[dstCount].id.fp.typeA = 1; pDst[dstCount].id.fp.typeB = 1;

				pDst[dstCount].edgeIndex = pSrc[i1].edgeIndex;

				++dstCount;
			}
		}
	}
	else switch (srcCount)
	{
	case 2: {
		dm_vec4 distance1 = dmDot3(normal, pSrc[0].v) - offset;
		dm_vec4 distance2 = dmDot3(normal, pSrc[1].v) - offset;


		if (dmLEZ(distance1)) pDst[dstCount++] = pSrc[0];
		if (dmLEZ(distance2)) pDst[dstCount++] = pSrc[1];


		if (dmLTZ(distance1 * distance2))
		{

			dm_vec4 a = distance1 / (distance1 - distance2);
			pDst[dstCount].v = pSrc[0].v + a * (pSrc[1].v - pSrc[0].v);


			pDst[dstCount].id.key = 0;

			pDst[dstCount].id.fp.featureA = (dm_uint8)edgeIndex;
			pDst[dstCount].id.fp.featureB = (dm_uint8)pSrc[0].edgeIndex;
			pDst[dstCount].id.fp.typeA = 1; pDst[dstCount].id.fp.typeB = 1;


			++dstCount;
		}
	}
		break;

	case 1: {
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
struct dmPolytopeCollider
{
    void Collide(dmManifold * pManifold, dmSATCache * pCache, const dmPolytope * pPolytopeA, const dmPolytope * pPolytopeB, const dmTransform & xfB);

    bool TestEdges(dmSATCache * pCache);

    void TestFaceA(dmPPAxis * pAxis, dm_int32 faceIndexA) const;
    void TestFaceB(dmPPAxis * pAxis, dm_int32 faceIndexB) const;
    void TestEdgePair(dmPPAxis * pAxis, const dmEdgePair & pair) const;

    bool IsMinkowskiFace(const dmEdgePair & pair) const;

    void Clip(dmManifold * pManifold, dm_int32 faceR, const dmPolytope * polyR, dm_int32 faceI, const dmPolytope * polyI, const dmMtx & transformI) const;


    void CollideFaceA(dmManifold * pManifold, const dmPPAxis & axis) const;
    void CollideFaceB(dmManifold * pManifold, const dmPPAxis & axis) const;
    void CollideEdges(dmManifold * pManifold, const dmPPAxis & axis, bool cached) const;

    void ComputeManifold(dmManifold * pManifold, const dmPPAxis & axis) const;

    dmMtx m_transformB;
    dmMtx m_invTransformB;

    const dmPolytope * m_pPolytopeA;
    const dmPolytope * m_pPolytopeB;

    dm_vec4 m_totalRadius;

    dmPPAxis m_faceAxis;
    dmPPAxis m_edgeAxis;
};
static_assert(sizeof(dmPolytopeCollider) == 0xe0, "dmPolytopeCollider");

void dmPolytopeCollider::TestFaceA(dmPPAxis * pAxis, dm_int32 faceIndexA) const
{

	dm_vec4 planeA = m_pPolytopeA->GetFace(faceIndexA);


	dm_vec4 axisB = -dmMul33(m_invTransformB, planeA);


	dm_vec4 vertexB;
	dm_int32 vertexIndexB = m_pPolytopeB->GetSupport(&vertexB, axisB);


	dm_vec4 vertexBinA = dmMul34(m_transformB, vertexB);


	dm_vec4 s = dmDot3(planeA, vertexBinA) - dmSplatW(planeA) - m_totalRadius;


	pAxis->indexA = faceIndexA;
	pAxis->indexB = vertexIndexB;
	pAxis->type = dmPPAxis::e_faceA;
	pAxis->separation = s;
}


void dmPolytopeCollider::TestFaceB(dmPPAxis * pAxis, dm_int32 faceIndexB) const
{

	dm_vec4 planeB = m_pPolytopeB->GetFace(faceIndexB);


	dm_vec4 axisA = -dmMul33(m_transformB, planeB);


	dm_vec4 vertexA;
	dm_int32 vertexIndexA = m_pPolytopeA->GetSupport(&vertexA, axisA);


	dm_vec4 vertexAinB = dmMul34(m_invTransformB, vertexA);


	dm_vec4 s = dmDot3(planeB, vertexAinB) - dmSplatW(planeB) - m_totalRadius;


	pAxis->indexA = vertexIndexA;
	pAxis->indexB = faceIndexB;
	pAxis->type = dmPPAxis::e_faceB;
	pAxis->separation = s;
}


void dmPolytopeCollider::TestEdgePair(dmPPAxis * pAxis, const dmEdgePair & pair) const
{
	pAxis->indexA = pair.indexA;
	pAxis->indexB = pair.indexB;
	pAxis->type = dmPPAxis::e_edgeType;

	const dmSubEdge* pEdgeA = m_pPolytopeA->GetSubEdge(pair.indexA);
	const dmSubEdge* pTwinA = pEdgeA->GetTwin();
	const dmSubEdge* pEdgeB = m_pPolytopeB->GetSubEdge(pair.indexB);
	const dmSubEdge* pTwinB = pEdgeB->GetTwin();


	dm_vec4 a1 = m_pPolytopeA->GetVertex(pEdgeA->tail);
	dm_vec4 a2 = m_pPolytopeA->GetVertex(pTwinA->tail);
	dm_vec4 b1 = m_pPolytopeB->GetVertex(pEdgeB->tail);
	dm_vec4 b2 = m_pPolytopeB->GetVertex(pTwinB->tail);



	dm_vec4 ea = a2 - a1;
	dm_vec4 eb = dmMul33(m_transformB, b2 - b1);
	dm_vec4 normalA = dmCross(ea, eb);

	if (dmIsZeroSafe3(normalA))
	{
		pAxis->separation = -dm_vec4_maxFloat;
		return;
	}

	normalA = dmNormalize3(normalA);

	dm_vec4 outwardA = a1 - m_pPolytopeA->GetCentroid();
	dm_vec4 direction = dmDot3(outwardA, normalA);
	if (dmLTZ(direction))
	{
		normalA = -normalA;
	}

	dm_vec4 s = dmDot3(normalA, dmMul34(m_transformB, b1) - a1) - m_totalRadius;

	pAxis->separation = s;
}



// Inference: twin faces read as pEdgeA + 1 / pEdgeB + 1 (constant +6 byte offset, no GetTwin inline here).
bool dmPolytopeCollider::IsMinkowskiFace(const dmEdgePair & pair) const
{
	const dmSubEdge* pEdgeA = m_pPolytopeA->GetSubEdge(pair.indexA);
	const dmSubEdge* pTwinA = pEdgeA + 1;
	const dmSubEdge* pEdgeB = m_pPolytopeB->GetSubEdge(pair.indexB);
	const dmSubEdge* pTwinB = pEdgeB + 1;

	// The negated A-face normals produce both signed dot products in the original.
	dm_vec4 a = -m_pPolytopeA->GetFace(pEdgeA->left);
	dm_vec4 b = -m_pPolytopeA->GetFace(pTwinA->left);
	dm_vec4 dirA = m_pPolytopeA->GetEdgeVector(pair.indexA);

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


// Inference: EdgeCache is local to TestEdges (DWARF line 342); twin sub-edges read as pEdge + 1 (constant +6 byte offsets).
bool dmPolytopeCollider::TestEdges(dmSATCache * pCache)
{
	m_edgeAxis.Clear();

	struct EdgeCache
	{
		dm_vec4 edgeNormal;
		dm_vec4 twinNormal;
		dm_vec4 edgeVector;
	};



	EdgeCache aEdges[128];
	const dm_int32 subEdgeCountA = m_pPolytopeA->GetSubEdgeCount();
	for (dm_int32 i = 0; i < subEdgeCountA; i += 2)
	{
		const dmSubEdge* pEdgeA = m_pPolytopeA->GetSubEdge(i);
		const dmSubEdge* pTwinA = pEdgeA + 1;

		EdgeCache* pEdgeCache = aEdges + (i >> 1);





		pEdgeCache->edgeNormal = m_pPolytopeA->GetFace(pEdgeA->left);
		pEdgeCache->twinNormal = m_pPolytopeA->GetFace(pTwinA->left);
		pEdgeCache->edgeVector = m_pPolytopeA->GetEdgeVector(i);
	}

	const dm_int32 subEdgeCountB = m_pPolytopeB->GetSubEdgeCount();
	for (dm_int32 i = 0; i < subEdgeCountB; i += 2)
	{
		dm_int32 indexB = i;
		const dmSubEdge* pEdgeB = m_pPolytopeB->GetSubEdge(indexB);
		const dmSubEdge* pTwinB = pEdgeB + 1;
		dm_vec4 c = m_pPolytopeB->GetFace(pEdgeB->left);
		dm_vec4 d = m_pPolytopeB->GetFace(pTwinB->left);
		dm_vec4 dirB = m_pPolytopeB->GetEdgeVector(indexB);


		c = dmMul33(m_transformB, c);
		d = dmMul33(m_transformB, d);
		dirB = dmMul33(m_transformB, dirB);


		// Inference: no negation survives in the original loop (Minkowski-difference signs cancel), so dc is dirB itself.
		dm_vec4 dc = dirB;

		for (dm_int32 j = 0; j < subEdgeCountA >> 1; ++j)
		{
			dm_int32 indexA = j << 1; // Each cached pair starts at its even-numbered sub-edge.


			EdgeCache* pEdgeCache = aEdges + j;
			dm_vec4 a = pEdgeCache->edgeNormal;
			dm_vec4 b = pEdgeCache->twinNormal;



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





			dm_vec4 a1 = m_pPolytopeA->GetTailVertex(indexA);
			dm_vec4 b1 = m_pPolytopeB->GetTailVertex(indexB);
			dm_vec4 outwardA = a1 - m_pPolytopeA->GetCentroid();
			dm_vec4 outwardB = dmMul33(m_transformB, b1 - m_pPolytopeB->GetCentroid());
			dm_vec4 directionA = dmDot3(normalA, outwardA);
			dm_vec4 directionB = dmDot3(normalA, outwardB);
			// Inference: rows 456-458 reuse test1-3 (no locals of their own); the original folds the selection into a bsl.
			test1 = dmCompareGT(dmAbs(directionA), dmAbs(directionB));
			test2 = dmCompareLT(directionA, dmVecSet(0.0f));
			test3 = dmCompareGT(directionB, dmVecSet(0.0f));

			// Inference: flip when the dominant direction points inward; the original spells this with dmOr (row 461).
			dm_vec4 mask = dmOr(dmSelect(test1, dmVecSet(0.0f), test3), dmSelect(test1, test2, dmVecSet(0.0f)));
			dm_vec4 tempNormalA = dmSelect(mask, -normalA, normalA);
			normalA = tempNormalA;




























			dm_vec4 s = dmDot3(normalA, dmMul34(m_transformB, b1) - a1) - m_totalRadius;

			if (dmGTZ(s))
			{

				pCache->type = dmPPAxis::e_edgeType;
				pCache->separation = dmF32(s);
				pCache->indexA = indexA;
				pCache->indexB = indexB;
				return false;
			}

			if (dmGT(s, m_edgeAxis.separation))
			{
				m_edgeAxis.type = dmPPAxis::e_edgeType;
				m_edgeAxis.separation = s;
				m_edgeAxis.indexA = indexA;
				m_edgeAxis.indexB = indexB;
			}
		}
	}

	return true;
}
void dmManifold_ReducePoints(dmManifold * pManifold, const dmManifoldPoint * pPoints, dm_int32 count); // Defined in dmManifold.cpp (DWARF signature).

void dmPolytopeCollider::Clip(dmManifold * pManifold, dm_int32 faceR, const dmPolytope * polyR,
	dm_int32 faceI, const dmPolytope * polyI, const dmMtx & transformI) const
{
	const dm_int32 k_maxPoints = 32;


	dmPPVertex incidentFace[64];

	dm_int32 edge1 = polyI->GetFaceEdge(faceI);
	dm_int32 edge2 = polyI->GetNextEdge(edge1); // The loop terminates when the next edge returns to its initial value.
	dm_int32 terminalEdge = edge2;

	dm_int32 count = 0;

	do
	{
		dmPPVertex * clip = &incidentFace[count];

		const dmSubEdge * pEdge1 = polyI->GetSubEdge(edge1);
		dm_vec4 v = polyI->GetVertex(pEdge1->tail);
		clip->v = dmMul34(transformI, v);
		clip->edgeIndex = polyI->GetPrimaryEdge(edge1);
		clip->id.key = 0;

		clip->id.fp.featureA = (dm_uint8)faceR;
		clip->id.fp.featureB = (dm_uint8)pEdge1->tail;
		clip->id.fp.typeA = dmFeature::e_face; clip->id.fp.typeB = dmFeature::e_vertex;


		edge1 = edge2;
		edge2 = polyI->GetNextEdge(edge1);

		++count;
	} while (edge2 != terminalEdge && count < k_maxPoints);
















	dmPPVertex buffer[64];

	dmPPVertex * source = incidentFace;
	dmPPVertex * destination = buffer;

	dm_vec4 plane = polyR->GetFace(faceR);
	dm_vec4 normal = plane;
	dm_vec4 offset = dmSplatW(plane);

	edge1 = polyR->GetFaceEdge(faceR);
	dm_vec4 p1 = polyR->GetTailVertex(edge1);
	terminalEdge = edge1;

	do
	{
		edge2 = polyR->GetNextEdge(edge1);
		dm_vec4 p2 = polyR->GetTailVertex(edge2);

		dm_vec4 d = p2 - p1;
		dm_vec4 sideNormal = dmCross(d, normal);
		sideNormal = dmNormalize3(sideNormal);


		dm_vec4 sideOffset = dmDot3(sideNormal, p1) + m_totalRadius;
		count = ClipPolygon(destination, source, count, sideNormal, sideOffset, polyR->GetPrimaryEdge(edge1));
		dmAssert(count <= k_maxPoints);
		dmSwap(source, destination);
		p1 = p2;
		edge1 = edge2;

	} while (edge1 != terminalEdge);


	dmManifoldPoint points[32];
	dm_int32 pointCount = 0;
	for (dm_int32 i = 0; i < count; ++i)
	{
		dmPPVertex * clip = &source[i];

		dm_vec4 separation = dmDot3(normal, clip->v) - offset;
		// The original clips to the contact radius and stores the retained points as new.
		if (dmLE(separation, m_totalRadius) && pointCount < k_maxPoints)
		{

			points[pointCount].point = clip->v - separation * normal;
			points[pointCount].separation = dmF32(separation - m_totalRadius);
			points[pointCount].id = clip->id;
			points[pointCount].normalImpulse = 0.0f;
			points[pointCount].isNew = true;

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
}

// Linker-removed standalone body is not checkable; recovered from ComputeManifold's inline instance.
void dmPolytopeCollider::CollideFaceA(dmManifold * pManifold, const dmPPAxis & axis) const
{
    dm_int32 faceA = axis.indexA;
    dm_vec4 axisA = m_pPolytopeA->GetFace(faceA);
    dm_vec4 axisAinB = -dmMul33(m_invTransformB, axisA);
    dm_int32 faceB = m_pPolytopeB->GetSupportFace(axisAinB, axis.indexB);


    Clip(pManifold, faceA, m_pPolytopeA, faceB, m_pPolytopeB, m_transformB);
}

// Linker-removed standalone body is not checkable; recovered from ComputeManifold's inline instance.
void dmPolytopeCollider::CollideFaceB(dmManifold * pManifold, const dmPPAxis & axis) const
{
    dm_int32 faceB = axis.indexB;
    dm_vec4 axisB = m_pPolytopeB->GetFace(faceB);
    dm_vec4 axisBinA = -dmMul33(m_transformB, axisB);
    dm_int32 faceA = m_pPolytopeA->GetSupportFace(axisBinA, axis.indexA);


    Clip(pManifold, faceB, m_pPolytopeB, faceA, m_pPolytopeA, m_invTransformB);


    pManifold->m_normal = -dmMul33(m_transformB, pManifold->m_normal);
    // Inference: ascending point loop follows the line-667 test and increment.
    for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i)
    {
        // Inference: the line-677 aggregate store exchanges the two features and clears triangleIndex.
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
}


void dmPolytopeCollider::CollideEdges(dmManifold * pManifold, const dmPPAxis & axis, bool cached) const
{
	pManifold->m_pointCount = 0;

	dm_int32 edgeA = axis.indexA;
	dm_int32 twinA = m_pPolytopeA->GetTwinIndex(edgeA);
	dm_int32 edgeB = axis.indexB;
	dm_int32 twinB = m_pPolytopeB->GetTwinIndex(edgeB);

	dmCapsule segmentA, segmentB;
	// The first capsule radius is cleared before its endpoints, as in dmPCCollider::ComputeEE.
	segmentA.m_radius = 0.0f;
	segmentA.m_p1 = m_pPolytopeA->GetTailVertex(edgeA);
	segmentA.m_p2 = m_pPolytopeA->GetTailVertex(twinA);
	// Inference: the line-700 store (column 20) clears the radius of the capsule filled on lines 701-702.
	segmentB.m_radius = 0.0f;
	segmentB.m_p1 = m_pPolytopeB->GetTailVertex(edgeB);
	segmentB.m_p2 = m_pPolytopeB->GetTailVertex(twinB);


	segmentB.m_p1 = dmMul34(m_transformB, segmentB.m_p1);
	segmentB.m_p2 = dmMul34(m_transformB, segmentB.m_p2);

	dm_vec4 c1, c2;
	dm_vec4 distance = dmDistance(&c1, &c2, &segmentA, &segmentB);


	if (cached && dmGT(dm_vec4_oneHalf * distance, dmAbs(axis.separation + m_totalRadius)))
	{
		return;
	}

	dm_vec4 d1 = segmentA.m_p2 - segmentA.m_p1;
	dm_vec4 d2 = segmentB.m_p2 - segmentB.m_p1;
	dm_vec4 normal = dmCross(d1, d2);


	if (dmIsZeroSafe3(normal))
	{
		return;
	}

	normal = dmNormalize3(normal);


	dm_vec4 d = segmentA.m_p1 - m_pPolytopeA->GetCentroid();
	dm_vec4 da = dmDot3(d, normal);

	if (dmLTZ(da))
	{
		normal = -normal;
	}

	dm_vec4 separation = dmDot3(c2 - c1, normal) - m_totalRadius;

	pManifold->m_pointCount = 1;
	pManifold->m_normal = normal;

	dm_vec4 point = dm_vec4_oneHalf * (c1 + c2);
	pManifold->m_points[0].point = point;
	pManifold->m_points[0].separation = dmF32(separation);

	pManifold->m_points[0].id.key = 0;

	pManifold->m_points[0].id.fp.featureA = edgeA;
	pManifold->m_points[0].id.fp.featureB = edgeB;
	pManifold->m_points[0].id.fp.typeA = dmFeature::e_edge;
	pManifold->m_points[0].id.fp.typeB = dmFeature::e_edge;
}
#line 755 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Functions/dmPPManifold.cpp"
// Inference: switch cases share line-758 tests; each completed case exits through the shared epilogue.
void dmPolytopeCollider::ComputeManifold(dmManifold * pManifold, const dmPPAxis & axis) const
{
    switch (axis.type)
    {
    case dmPPAxis::e_faceA:
        CollideFaceA(pManifold, axis);
        break;

    case dmPPAxis::e_faceB:
        CollideFaceB(pManifold, axis);
        break;

    case dmPPAxis::e_edgeType:
        CollideEdges(pManifold, axis, false);
        break;

    default:
        dmAssert(false);
    }
}


void dmPolytopeCollider::Collide(dmManifold * pManifold, dmSATCache * pCache,
	const dmPolytope * pPolytopeA, const dmPolytope * pPolytopeB,
	const dmTransform & xfB)
{
	++dm_satCallCount;

	pManifold->m_pointCount = 0;

	m_pPolytopeA = pPolytopeA;
	m_pPolytopeB = pPolytopeB;

	m_totalRadius = dmV4((*pPolytopeA).m_radius + (*pPolytopeB).m_radius);

	m_transformB = dmMtxFromTransform(xfB);
	m_invTransformB = dmInvertOrtho34(m_transformB);


	if (pCache->type == dmPPAxis::e_faceA)
	{
		dm_int32 faceA = pCache->indexA;

		dmPPAxis axis;
		TestFaceA(&axis, faceA);
		if (dmGTZ(axis.separation))
		{
			++dm_satCacheSeparatedCount;
			return;
		}

		if (dmAbs(dmF32(axis.separation) - pCache->separation) < 0.016404198f)
		{
			++dm_satCacheTouchCount;
			CollideFaceA(pManifold, axis);

			if (pManifold->m_pointCount > 0)
			{
				return;
			}
		}
	}
	else if (pCache->type == dmPPAxis::e_faceB)
	{
		dm_int32 faceB = pCache->indexB;

		dmPPAxis axis;
		TestFaceB(&axis, faceB);
		if (dmGTZ(axis.separation))
		{
			++dm_satCacheSeparatedCount;
			return;
		}

		if (dmAbs(dmF32(axis.separation) - pCache->separation) < 0.016404198f)
		{
			++dm_satCacheTouchCount;
			CollideFaceB(pManifold, axis);

			if (pManifold->m_pointCount > 0)
			{
				return;
			}
		}
	}
	else if (pCache->type == dmPPAxis::e_edgeType)
	{
		dm_int32 edgeA = pCache->indexA;
		dm_int32 edgeB = pCache->indexB;

		dmEdgePair pair;
		pair.indexA = edgeA;
		pair.indexB = edgeB;

		bool test = IsMinkowskiFace(pair);
		if (test)
		{
			dmPPAxis axis;
			TestEdgePair(&axis, pair);
			if (dmGTZ(axis.separation))
			{
				++dm_satCacheSeparatedCount;
				return;
			}

			if (dmAbs(dmF32(axis.separation) - pCache->separation) < 0.016404198f)
			{
				++dm_satCacheTouchCount;
				CollideEdges(pManifold, axis, true);

				if (pManifold->m_pointCount > 0)
				{
					return;
				}
			}
		}
	}

	m_faceAxis.Clear();
	m_edgeAxis.Clear();




	const dm_vec4 k_faceTol = dmV4(0.98f);
	const dm_vec4 k_edgeTol = dmV4(0.9f);
	const dm_vec4 k_absoluteTol = dm_vec4_oneHalf * dm_vec4_linearSlop;

	for (dm_int32 i = 0; i < m_pPolytopeA->GetFaceCount(); ++i)
	{
		dm_int32 faceA = i;

		dmPPAxis axis;
		TestFaceA(&axis, faceA);

		if (dmGTZ(axis.separation))
		{

			pCache->type = dmPPAxis::e_faceA;
			pCache->separation = dmF32(axis.separation);
			pCache->indexA = axis.indexA;
			pCache->indexB = axis.indexB;
			return;
		}

		if (dmGT(axis.separation, m_faceAxis.separation))
		{
			m_faceAxis = axis;
		}
	}

	for (dm_int32 i = 0; i < m_pPolytopeB->GetFaceCount(); ++i)
	{
		dm_int32 faceB = i;

		dmPPAxis axis;
		TestFaceB(&axis, faceB);

		if (dmGTZ(axis.separation))
		{

			pCache->type = dmPPAxis::e_faceB;
			pCache->separation = dmF32(axis.separation);
			pCache->indexA = axis.indexA;
			pCache->indexB = axis.indexB;
			return;
		}

		if (dmGT(axis.separation, k_faceTol * m_faceAxis.separation + k_absoluteTol))
		{
			m_faceAxis = axis;
		}
	}

	if (TestEdges(pCache) == false)
	{
		return;
	}





















	ComputeManifold(pManifold, m_faceAxis);
	dm_vec4 faceSeparation = dmV4(0.0f);
	for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i)
	{
		faceSeparation = dmMin(faceSeparation, dmV4(pManifold->m_points[i].separation));
	}

	bool edgeContactExists = m_edgeAxis.type == dmPPAxis::e_edgeType;
	bool edgeBest = dmGT(m_edgeAxis.separation, k_edgeTol * m_faceAxis.separation + k_absoluteTol);
	bool edgeBestClipped = dmGT(m_edgeAxis.separation, k_edgeTol * faceSeparation + k_absoluteTol);





	if (edgeContactExists && ((pManifold->m_pointCount < 2 && edgeBest) || edgeBestClipped))
	{
		pManifold->m_pointCount = 0;
		ComputeManifold(pManifold, m_edgeAxis);
		pCache->separation = dmF32(m_edgeAxis.separation);
		pCache->type = m_edgeAxis.type;
		pCache->indexA = m_edgeAxis.indexA;
		pCache->indexB = m_edgeAxis.indexB;
	}
	else
	{
		pCache->separation = dmF32(m_faceAxis.separation);
		pCache->type = m_faceAxis.type;
		pCache->indexA = m_faceAxis.indexA;
		pCache->indexB = m_faceAxis.indexB;
	}
}


void dmPPManifold(dmManifold * pManifold, dmSATCache * pCache,
    const dmPolytope * pPolytopeA, const dmPolytope * pPolytopeB,
    const dmTransform & xfB)
{
    dmPolytopeCollider collider;
    collider.Collide(pManifold, pCache, pPolytopeA, pPolytopeB, xfB);
}
