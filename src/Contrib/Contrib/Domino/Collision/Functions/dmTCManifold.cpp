// Original file: Contrib/Contrib/Domino/Collision/Functions/dmTCManifold.cpp
#include "dmManifold.h"
#include "dmDistance.h"















struct dmTCVertex
{
	dm_vec4 v;
	dmContactID id;
};


struct dmTCAxis
{
	enum Type
	{
		e_faceVertexType = 0,
		e_edgeType = 1,
		e_unknownType = 2
	};
	// No linked standalone body; construction is checkable in dmTCManifold inline copy.
	dmTCAxis() : separation(-dm_vec4_maxFloat), indexA(-1), type(e_unknownType) {}
	dm_vec4 separation;
	dm_int32 indexA;
	Type type;
};


static dm_int32 ClipSegment(dmTCVertex * pDst, const dmTCVertex * pSrc,
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



struct dmTCCollider
{
	void Collide(dmManifold * pManifold, dmGJKCache * pCache, const dmTriangle * pTriangleA, const dmCapsule * pCapsuleB);




	dm_vec4 SeparationEE(dm_int32 edgeA);

	bool ComputeFV(dmManifold * pManifold);
	void ComputeEE(dmManifold * pManifold);

	dmTriangle m_triangleA;
	dmCapsule m_capsuleB;

	dm_vec4 m_radiusA;
	dm_vec4 m_radiusB;

	dm_vec4 m_normalA;
	dm_vec4 m_aWingNormals[3];

	dmTCAxis m_bestAxis;
};


dm_vec4 dmTCCollider::SeparationEE(dm_int32 edgeA)
{
	dm_int32 iv1 = edgeA;
	// Inference: wrap spelled as in ComputeEE; the loop range turns it into cmp #2/eq.
	dm_int32 iv2 = iv1 > 1 ? 0 : iv1 + 1;
	dm_int32 iv3 = iv2 > 1 ? 0 : iv2 + 1;


	dm_vec4 a1 = m_triangleA.m_p[iv1];
	dm_vec4 a2 = m_triangleA.m_p[iv2];
	dm_vec4 a3 = m_triangleA.m_p[iv3];


	dm_vec4 ea = a2 - a1;
	// Inference: eb is the capsule axis (the subtraction is hoisted, no row).
	dm_vec4 eb = m_capsuleB.m_p2 - m_capsuleB.m_p1;
	dm_vec4 normal = dmCross(ea, eb);


	if (dmIsZeroSafe3(normal))
	{
		// Inference: the degenerate result is -dm_vec4_maxFloat (hoisted fneg of the constant).
		return -dm_vec4_maxFloat;
	}


	normal = dmNormalize3(normal);
	dm_vec4 outward = a1 - a3;
	dm_float32 direction = dmDot3_f32(outward, normal);
	if (direction < 0.0f)
	{
		normal = -normal;
	}

	dm_vec4 s = dmDot3(normal, m_capsuleB.m_p1 - a1);
	return s;
}


// NON_MATCHING: 2500 vs 2504 bytes; the original reloads segment[0].v from the stack inside the first inlined ClipSegment, ours keeps it in a register; rest is register naming.
bool dmTCCollider::ComputeFV(dmManifold * pManifold)
{

	dmTCVertex segment[2];

	segment[0].v = m_capsuleB.m_p1;

	segment[0].id.fp.featureA = 0;
	segment[0].id.fp.featureB = 0;
	segment[0].id.fp.typeA = dmFeature::e_face;
	segment[0].id.fp.typeB = dmFeature::e_vertex;
	segment[0].id.fp.triangleIndex = m_triangleA.m_triangleIndex;

	segment[1].v = m_capsuleB.m_p2;

	segment[1].id.fp.featureA = 0;
	segment[1].id.fp.featureB = 1;
	segment[1].id.fp.typeA = dmFeature::e_face;
	segment[1].id.fp.typeB = dmFeature::e_vertex;
	segment[1].id.fp.triangleIndex = m_triangleA.m_triangleIndex;







	dmTCVertex buffer[2];

	dmTCVertex * source = segment;
	dmTCVertex * destination = buffer;

	dm_vec4 p1 = m_triangleA.m_p[0];
	dm_vec4 p2 = m_triangleA.m_p[1];
	dm_vec4 p3 = m_triangleA.m_p[2];

	dm_int32 count = 0;


	dm_vec4 d1 = p2 - p1;
	dm_vec4 sideNormal1 = dmCross(d1, m_normalA);

	sideNormal1 = dmNormalize3(sideNormal1);
	dm_vec4 sideOffset1 = dmDot3(sideNormal1, p1) + m_radiusA;
	count = ClipSegment(destination, source, sideNormal1, sideOffset1, 0);
	dmAssert(count <= 2);
	if (count < 2) return false;
	dmSwap(source, destination);





	dm_vec4 d2 = p3 - p2;
	dm_vec4 sideNormal2 = dmCross(d2, m_normalA);

	sideNormal2 = dmNormalize3(sideNormal2);
	dm_vec4 sideOffset2 = dmDot3(sideNormal2, p2) + m_radiusA;
	count = ClipSegment(destination, source, sideNormal2, sideOffset2, 1);
	dmAssert(count <= 2);
	if (count < 2) return false;
	dmSwap(source, destination);





	dm_vec4 d3 = p1 - p3;
	dm_vec4 sideNormal3 = dmCross(d3, m_normalA);

	sideNormal3 = dmNormalize3(sideNormal3);
	dm_vec4 sideOffset3 = dmDot3(sideNormal3, p3) + m_radiusA;
	count = ClipSegment(destination, source, sideNormal3, sideOffset3, 2);
	dmAssert(count <= 2);
	if (count < 2) return false;
	dmSwap(source, destination);



	// Inference: the face normal is flipped unless m_bestAxis.indexA is zero (select on a scalar compare).
	dm_vec4 normal = m_bestAxis.indexA == 0 ? m_normalA : -m_normalA;

	dm_vec4 aWingNormals[3];
	m_triangleA.GetWingNormals(aWingNormals);

	dm_vec4 totalRadius = m_radiusA + m_radiusB;
	dm_int32 pointCount = 0;
	for (dm_int32 i = 0; i < 2; ++i) // Inference: constant trip count (the original fully unrolls two iterations); count is 2 here.
	{
		dmTCVertex * clip = source + i;
		dmFeaturePair fp = clip->id.fp;

		// Inference: clip points born on an edge are skipped when that wing normal is within 2 degrees of the face normal.
		if (fp.typeA == dmFeature::e_edge)
		{
			dm_int32 edgeIndex = fp.featureA;
			dm_vec4 dot = dmDot3(aWingNormals[edgeIndex], m_normalA);
			if (dmGT(dot, dmVecSet(0.99939083f))) continue;
		}




		dm_vec4 separation = dmDot3(normal, clip->v - p1);

		if (dmLE(separation, totalRadius))
		{
			dmManifoldPoint * pPoint = &pManifold->m_points[pointCount];

			dm_vec4 p = clip->v - m_radiusB * normal;
			pPoint->point = p;
			pPoint->separation = dmGetX(separation - totalRadius);
			pPoint->id = clip->id;
			++pointCount;
		}
	}

	pManifold->m_normal = normal;
	pManifold->m_pointCount = (dm_uint16)pointCount;
	return true;
}


void dmTCCollider::ComputeEE(dmManifold * pManifold)
{
	dm_int32 iv1 = m_bestAxis.indexA;
#line 267
	// Inference: the original signed >1 comparison wraps the next triangle vertex to zero.
	dm_int32 iv2 = iv1 > 1 ? 0 : iv1 + 1;
#line 268
	// Inference: the same signed comparison advances and wraps the third triangle vertex.
	dm_int32 iv3 = iv2 > 1 ? 0 : iv2 + 1;

	// DWARF places both segment declarations at 271; preserve separate source lines.
#line 271
	dmCapsule segmentA;
#line 271
	dmCapsule segmentB;


	segmentA.m_p1 = m_triangleA.m_p[iv1];
	segmentA.m_p2 = m_triangleA.m_p[iv2];
	segmentA.m_radius = 0.0f;  // Inference: 276/277 have no row; both radius stores merge into the 32-bit stp type/radius pairs (197:23) and give the original size.
	segmentB.m_radius = 0.0f;
	segmentB.m_p1 = m_capsuleB.m_p1;
	segmentB.m_p2 = m_capsuleB.m_p2;

	// DWARF places both point declarations at 281; preserve separate source lines.
#line 281
	dm_vec4 cA;
#line 281
	dm_vec4 cB;
#line 281
	// Inference: retain the unused distance return recorded by DWARF at line 282.
	dm_vec4 distance = dmDistance(&cA, &cB, &segmentA, &segmentB);


	dm_vec4 dA = segmentA.m_p2 - segmentA.m_p1;
	dm_vec4 dB = segmentB.m_p2 - segmentB.m_p1;
	dm_vec4 axis = dmCross(dA, dB);

	if (dmIsZeroSafe3(axis))
	{
		// Inference: the original zero-safe failure branches directly to the epilogue.
		return;
	}
	axis = dmNormalize3(axis);


	dm_vec4 d = segmentA.m_p1 - m_triangleA.m_p[iv3];
	dm_vec4 da = dmDot3(d, axis);
	// Inference: choose the axis sign from the original negative-dot predicate.
	dm_vec4 normal = axis;
	if (dmLTZ(da))
	{
		normal = -axis;
	}





	dm_vec4 separation = dmDot3(cB - cA, normal) - (m_radiusA + m_radiusB);

	// Inference: positive separation rejects the contact via the shared epilogue.
	if (dmGTZ(separation))
	{
		return;
	}

	pManifold->m_pointCount = 1;
	pManifold->m_normal = normal;

	pManifold->m_points[0].point = cB - m_radiusB * normal;
	pManifold->m_points[0].separation = dmGetX(separation);

	pManifold->m_points[0].id.key = 0;
	pManifold->m_points[0].id.fp.featureA = m_bestAxis.indexA;
	pManifold->m_points[0].id.fp.featureB = 0;
	pManifold->m_points[0].id.fp.typeA = dmFeature::e_edge;
	pManifold->m_points[0].id.fp.typeB = dmFeature::e_edge;
}
#line 330



void dmTCCollider::Collide(dmManifold * pManifold,
    dmGJKCache * pCache,
    const dmTriangle * pTriangleA,
    const dmCapsule * pCapsuleB)
{

	dm_vec4 axisB = pCapsuleB->m_p2 - pCapsuleB->m_p1;
	if (dmIsZeroSafe3(axisB))
	{
		return;
	}



	pManifold->m_pointCount = 0;

	m_normalA = pTriangleA->GetNormal();


	dm_vec4 side = dmDot3(m_normalA, dm_vec4_oneHalf * (pCapsuleB->m_p1 + pCapsuleB->m_p2) - pTriangleA->m_p[0]);
	if (dmLTZ(side))
	{
		return;
	}


	dmDistanceInput input;
	input.cache = *pCache;
	input.proxyA.Set(pTriangleA);
	input.proxyB.Set(pCapsuleB);
	input.transformA = dmTransformIdentity();
	input.transformB = dmTransformIdentity();
	input.useRadii = false;

	dmDistanceOutput output;
	dmDistance(&output, &input);
	*pCache = output.cache;
	m_triangleA = *pTriangleA;
	m_capsuleB = *pCapsuleB;
	m_radiusA = dmV4(m_triangleA.m_radius);
	m_radiusB = dmV4(m_capsuleB.m_radius);



	dm_vec4 totalRadius = m_radiusA + m_radiusB;
	if (dmGT(output.distance, totalRadius))
	{
		return;
	}



	m_triangleA.GetWingNormals(m_aWingNormals);



	// Inference: no inline dmV4 call survives and no row exists for 390/391; the values are splat constants (0.9, 0x3c066219 immediate), spelled like dmPCManifold.cpp.
	const dm_vec4 k_edgeTol = dmV4(0.9f);
	const dm_vec4 k_absoluteTol = dmV4(0.008202099f);

	dm_vec4 normal = output.pointB - output.pointA;

	// Doubt: second || is at col 91 and first || at col 37 (see doubts); text below gives col 28 for the first.
	if (dmIsZeroSafe3(normal) || dmLT(output.distance, dm_vec4_oneHalf * m_radiusA) || output.cache.GetCount() == 4)
	{




		{
			dm_vec4 s1 = dmDot3(m_normalA, m_capsuleB.m_p1 - m_triangleA.m_p[0]);
			dm_vec4 s2 = dmDot3(m_normalA, m_capsuleB.m_p2 - m_triangleA.m_p[0]);
			dm_vec4 s = dmMin(s1, s2);


			if (dmGT(s, m_bestAxis.separation))
			{
				m_bestAxis.type = dmTCAxis::e_faceVertexType; // Inference: type store is merged with indexA into one str xzr [this+0x140] (row 412:23 is the indexA col); line 410 free, order type/separation/indexA matches the 463-465 stores.
				m_bestAxis.separation = s;
				m_bestAxis.indexA = 0;
			}
		}

		dm_vec4 aEdges[3];
		aEdges[0] = dmNormalize3(m_triangleA.m_p[1] - m_triangleA.m_p[0]);
		aEdges[1] = dmNormalize3(m_triangleA.m_p[2] - m_triangleA.m_p[1]);
		aEdges[2] = dmNormalize3(m_triangleA.m_p[0] - m_triangleA.m_p[2]);



		for (dm_int32 i = 0; i < 3; ++i)
		{


			dm_vec4 s = SeparationEE(i);


			if (dmGT(s, k_edgeTol * m_bestAxis.separation + k_absoluteTol))
			{
				dm_float32 cosAngle = dmDot3_f32(m_normalA, m_aWingNormals[i]);
				dm_float32 sinAngle = dmF32(dmScalarTripleProduct(aEdges[i], m_aWingNormals[i], m_normalA)); // Inference: DWARF types sinAngle as dm_float32 but dmScalarTripleProduct returns dm_vec4; dmF32 is a no-instruction lane-0 extract (its inline DIE would carry no range).
				dm_float32 angle = dmEstAtan2(sinAngle, cosAngle);


				// Inference: operator*/test spelling only (see doubts); semantics from CFG: skip if edge has a neighbour and angle >= -2deg.
				if (m_triangleA.m_aWingIndices[i] != -1 && angle >= -0.034906585f)
				{
					continue;
				}



				if (m_triangleA.m_aWingIndices[i] != -1 && angle < 0.034906585f)
				{

					dm_vec4 c = axisB; // Inference: the original reuses the spilled axisB (str q7, [sp] at 361; ldr q25, [sp] feeds the 453 dot).
					dm_vec4 a = m_normalA;
					dm_vec4 b = m_aWingNormals[i];


					dm_vec4 ac = dmDot3(c, a);
					dm_vec4 bc = dmDot3(c, b);


					if (dmGEZ(ac * bc))
					{
						continue;
					}
				}

				m_bestAxis.type = dmTCAxis::e_edgeType;
				m_bestAxis.separation = s;
				m_bestAxis.indexA = i;
			}
		}


		if (m_bestAxis.type == dmTCAxis::e_faceVertexType)
		{
			ComputeFV(pManifold);
		}
		else if (m_bestAxis.type == dmTCAxis::e_edgeType)
		{
			ComputeEE(pManifold);
		}
		return; // Inference: needed so the code from 496 on can sit at one tab (501/509 are at col 6); see doubts.
	}
















	normal = dmNormalize3(normal);
	dm_float32 dot = dmDot3_f32(m_normalA, normal);
	const dm_float32 k_cos5degrees = 0.9961947f;
	bool clippingSucceeded = false;

	if (dot > k_cos5degrees)
	{

		m_bestAxis.type = dmTCAxis::e_faceVertexType;
		m_bestAxis.indexA = 0;
		clippingSucceeded = ComputeFV(pManifold);
	}

	if (!clippingSucceeded)
	{

		dm_int32 activeA[3] = {0};
		dm_int32 activeB[2] = {0};
		dm_uint8 count = output.cache.GetCount();
		for (dm_int32 i = 0; i < count; ++i)
		{
			activeA[output.cache.indexAs[i]] = 1;
			activeB[output.cache.indexBs[i]] = 1;
		}
		dm_int32 uniqueCountA = activeA[0] + activeA[1] + activeA[2];
		dm_int32 uniqueCountB = activeB[0] + activeB[1];

		dmFeaturePair fp;



		if (uniqueCountA == 1)
		{
			dm_int32 indexA = output.cache.indexAs[0];

			dm_int32 i1 = indexA > 0 ? indexA - 1 : 2;
			dm_int32 i2 = indexA;
			dm_int32 i3 = indexA == 2 ? 0 : indexA + 1;


			if (m_triangleA.m_aWingIndices[i1] != -1)
			{

				dm_vec4 e = m_triangleA.m_p[i2] - m_triangleA.m_p[i1];
				dm_vec4 test = dmScalarTripleProduct(e, normal, m_aWingNormals[i1]);
				if (dmLTZ(test))
				{
					return;
				}
			}


			if (m_triangleA.m_aWingIndices[i2] != -1)
			{

				dm_vec4 e = m_triangleA.m_p[i3] - m_triangleA.m_p[i2];
				dm_vec4 test = dmScalarTripleProduct(e, normal, m_aWingNormals[i2]);
				if (dmLTZ(test))
				{
					return;
				}
			}
			fp.featureA = indexA; // Inference: fp stores are SROA'd into the 631 struct copy, so these lines carry no rows; featureA = indexAs[0], typeA = e_vertex (2) per Ghidra.
			fp.typeA = dmFeature::e_vertex;
		}













		else if (uniqueCountA == 2)
		{
			dm_int32 i2, i1;
			if (activeA[0] && activeA[1])
			{
				i1 = 1;
				i2 = 0;
			}
			else if (activeA[1] && activeA[2])
			{
				i1 = 2;
				i2 = 1;
			}
			else
			{
				i1 = 0;
				i2 = 2;
			}


			if (m_triangleA.m_aWingIndices[i2] != -1)
			{

				dm_vec4 e = m_triangleA.m_p[i1] - m_triangleA.m_p[i2];
				dm_vec4 test = dmScalarTripleProduct(e, normal, m_aWingNormals[i2]);
				if (dmLTZ(test))
				{
					return;
				}
			}
			fp.featureA = i2;
			fp.typeA = dmFeature::e_edge;
		}
		else
		{
			return;
		}
		// Inference: featureB/typeB from the csel/cinc at +0xe04..0xe10 (indexBs[0] if uniqueCountB == 1 else 0; typeB vertex(2)/edge(1)); triangleIndex from the str wzr in the 631 copy.
		fp.featureB = uniqueCountB == 1 ? output.cache.indexBs[0] : 0;
		fp.typeB = uniqueCountB == 1 ? dmFeature::e_vertex : dmFeature::e_edge;
		fp.triangleIndex = 0;












		dm_vec4 p = output.pointB - m_radiusB * normal;
		pManifold->m_pointCount = 1;
		pManifold->m_normal = normal;

		pManifold->m_points[0].id.fp = fp;
		pManifold->m_points[0].point = p;
		pManifold->m_points[0].separation = dmF32(output.distance - totalRadius);
	}
}
#line 636

void dmTCManifold(dmManifold * pManifold,
	dmGJKCache * pCache,
	const dmTriangle * pTriangle,
	const dmCapsule * pCapsule)
{
	dmTCCollider collider;
	collider.Collide(pManifold, pCache, pTriangle, pCapsule);

	// Inference: the original ascending loop assigns the triangle index to every contact.
	for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i)
	{
		pManifold->m_points[i].id.fp.triangleIndex = pTriangle->m_triangleIndex;
	}
}
