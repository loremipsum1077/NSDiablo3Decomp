#include "../../Include/dmSettings.h"

#include "../../Include/dmPolytope.h"
#include "../../Include/dmPolytopeData.h"
#include "../../Common/dmPoolAllocator.h"

// NON_MATCHING: same 56 bytes, five instructions differ in zero-store scheduling (ours stores m_isTriangle first). On DWARF lines 47-66; all zero stores of 0x20-0x4b carry row 55:14 in the original. Tried three statement orders and a chained count assignment: no change.
#line 47 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dmPolytope::dmPolytope()
{
	m_type = e_polytope;
	m_radius = 0.032808397f;




	m_aVertices = 0;
	m_aPlanes = 0;
	m_aFaceSubEdges = 0;
	m_aSubEdges = 0;
	m_vertexCount = 0;
	m_faceCount = 0;
	m_subEdgeCount = 0;


	m_pAllocator = 0;
	m_isTriangle = false;
}
#line 69 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dmPolytope::~dmPolytope()
{

    if (m_pAllocator)
    {
        m_pAllocator->Free(m_aVertices, m_vertexCount * sizeof(dm_vec4), false);
        m_pAllocator->Free(m_aPlanes, m_faceCount * sizeof(dm_vec4), false);
        m_pAllocator->Free(m_aFaceSubEdges, m_faceCount * sizeof(dm_uint8), false);
        m_pAllocator->Free(m_aSubEdges, m_subEdgeCount * sizeof(dmSubEdge), false);
    }
    else if (!m_isTriangle)
    {
        dmFree(m_aVertices);
        dmFree(m_aPlanes);
        dmFree(m_aFaceSubEdges);
        dmFree(m_aSubEdges);
    }

}

#line 147 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
void dmPolytope::SetAsTriangle(dm_vec4 * aVertices, dm_vec4 * aPlaneBuffer,
    dmSubEdge * aSubEdges, dm_uint8 * aFaceSubEdges)
{
    m_vertexCount = 3;
    m_faceCount = 2;
    m_aVertices = aVertices;
    m_aPlanes = aPlaneBuffer;
    dm_vec4 e1 = dmSub(aVertices[1], aVertices[0]);
    dm_vec4 e2 = dmSub(aVertices[2], aVertices[0]);
    dm_vec4 n = dmCross(e1, e2);
    dm_float32 length = dmLength3_f32(n);
    m_area = 0.5f * length;
    m_volume = 0.0f;
    const dm_float32 dm_epsilon = 1.1920928955078125e-7f;
#line 163 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dmAssert(length > dm_epsilon);
    n = dmNormalize3(n);
    m_aPlanes[0] = n;
    m_aPlanes[1] = dmNegate(n);
    m_centroid = dmMul(dm_vec4_oneThird, dmAdd(dmAdd(m_aVertices[0], m_aVertices[1]), m_aVertices[2]));
    dm_vec4 w = dmDot3(n, m_centroid);
    m_aPlanes[0] = dmSetW(m_aPlanes[0], w);
    m_aPlanes[1] = dmSetW(m_aPlanes[1], dmNegate(w));
    m_subEdgeCount = 6;
    m_aSubEdges = aSubEdges;
    m_aFaceSubEdges = aFaceSubEdges;
    m_isTriangle = true;
}

#line 353 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_vec4 dmPolytope::ComputeSurfaceArea() const
{
    return dmV4(m_area);
}

#line 359 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_vec4 dmPolytope::ComputeVolume() const
{
    return dmV4(m_volume);
}

#line 733 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_float32 dmPolytope::ComputeMinRadius() const
{ // Inference: initial radius is the maximum finite float, from the original constant at 0x7100e3bf08.
	dm_float32 radius = 3.4028234663852886e+38f;
	for (dm_int32 i = 0; i < m_faceCount; ++i)
	{
		dm_float32 r = dmGetW(m_aPlanes[i]) - dmDot3_f32(m_aPlanes[i], m_centroid);

		radius = dmMin(radius, r);
	}

	return radius;
}

#line 747 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_float32 dmPolytope::ComputeMaxRadius() const
{
	dm_float32 radius = 0.0f;
	for (dm_int32 i = 0; i < m_vertexCount; ++i)
	{
		dm_vec4 v = m_aVertices[i];
		radius = dmMax(radius, dmDot3_f32(v, v));
	}
	radius = sqrtf(radius);
	return radius;
}

#include "../../Include/dmCollision.h"
#include "../Functions/dmDistance.h"

#line 365 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dmAABB dmPolytope::GetAABB(const dmMtx & xf) const
{
	dm_vec4 lower =  dm_vec4_maxFloat;
	dm_vec4 upper = -dm_vec4_maxFloat;


	const dm_vec4 *pVertex = m_aVertices;
	const dm_int32 count = m_vertexCount;
	for (dm_int32 i = 0; i < count; ++i)
	{
		dm_vec4 v = dmMul34(xf, *pVertex++);
		lower = dmMin(lower, v);
		upper = dmMax(upper, v);
	}

	dm_vec4 r = dmVecSet(m_radius);
	dmAABB aabb;
	aabb.m_lowerBound = lower - r;
	aabb.m_upperBound = upper + r;
	return aabb;
}

#line 388 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dmAABB dmPolytope::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const
{
	dm_vec4 lower =  dm_vec4_maxFloat;
	dm_vec4 upper = -dm_vec4_maxFloat;


	const dm_int32 count = m_vertexCount;


	for (dm_int32 i = 0; i < count; ++i)
	{

		dm_vec4 v = m_aVertices[i];
		dm_vec4 v1 = dmMul34(xf1, v);
		dm_vec4 v2 = dmMul34(xf2, v);
		lower = dmMin(lower, v1, v2);
		upper = dmMax(upper, v1, v2);
	}

	dm_vec4 r = dmVecSet(m_radius);
	dmAABB aabb;
	aabb.m_lowerBound = lower - r;
	aabb.m_upperBound = upper + r;
	return aabb;
}

#line 498 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
bool dmPolytope::TestOverlap(const dmSphere & sphere) const
{
	dmDistanceInput input;
	input.cache.Clear();
	input.proxyA.Set(m_aVertices, m_vertexCount, dmV4(0.0f));
	input.proxyB.Set(&sphere.m_p, 1, dmV4(sphere.m_radius));
	input.useRadii = true;
	input.transformA = dmTransformIdentity();
	input.transformB = dmTransformIdentity();

	dmDistanceOutput output;
	dmDistance(&output, &input);
	// Inference: the true/false return branches fold into the line 511 vector comparison.
	if (dmEQZ(output.distance))
	{
		return true;
	}

	return false;
}

#line 1083 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_int32 dmPartialPolytope::GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const
{
	dm_vec4 bestVertex = m_aVertices[0];
	dm_vec4 bestDot = dmDot3(direction, bestVertex);
	dmVecI32 bestIndexVec;
#line 1087 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
	dmVecI32 indexVec;
	// Inference: index zero initialization is eliminated; final selected indices are 0, 1, or 2.
	bestIndexVec.i[0] = 0;

	for (dm_int32 i = 1; i < 3; ++i)
	{ // Inference: the vector index assignment is eliminated into scalar selects at line 1102.
		indexVec.i[0] = i;
		dm_vec4 v = m_aVertices[i];
		dm_vec4 dot = dmDot3(direction, v);
		dm_vec4 mask = dmCompareGT(dot, bestDot);
		bestVertex = dmSelect(mask, v, bestVertex);
		bestDot = dmSelect(mask, dot, bestDot); // Inference: the index select below folds into line 1102 scalar selections.
		bestIndexVec.v = dmSelect(mask, indexVec.v, bestIndexVec.v);
	}

	dm_int32 bestIndex = bestIndexVec.i[0];



	*pVertex = bestVertex;
	return bestIndex;
}

#line 714 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_int32 dmPolytope::FindEdge(dm_int32 i1, dm_int32 i2) const
{
	for (dm_int32 i = 0; i < m_subEdgeCount; ++i)
	{
		if (m_aSubEdges[i].tail == i1)
		{
			const dmSubEdge * pTwin = m_aSubEdges[i].GetTwin();
			if (pTwin->tail == i2)
			{
				// Inference: the original AND immediate 0xfffe selects the primary edge index.
				return i & 0xfffe;
			}
		}
	}

	return -1;
}

#line 571 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_int32 dmPolytope::GetSupport(dm_vec4 * pVertex, dm_vec4Arg direction) const
{
	dmAssert(m_vertexCount > 0);

	dm_vec4 bestVertex = m_aVertices[0];
	dm_vec4 bestDot = dmDot3(direction, bestVertex); // Inference: the original zeroes both index vectors before the loop.
	dmVecI32 bestIndexVec = { 0 };
#line 577 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
	dmVecI32 indexVec = { 0 };



	for (dm_int32 i = 1; i < m_vertexCount; ++i)
	{
		indexVec.i[0] = i;
		dm_vec4 v = m_aVertices[i];
		dm_vec4 dot = dmDot3(direction, v);
		dm_vec4 mask = dmCompareGT(dot, bestDot);
		bestVertex = dmSelect(mask, v, bestVertex);
		bestDot = dmSelect(mask, dot, bestDot);
		bestIndexVec.v = dmSelect(mask, indexVec.v, bestIndexVec.v);
	}

	dm_int32 bestIndex = bestIndexVec.i[0];



	*pVertex = bestVertex;
	return bestIndex;
}

#line 601 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
dm_int32 dmPolytope::GetSupportFace(dm_vec4Arg direction, dm_int32 vertexIndex) const
{
	dmAssert(0 < m_faceCount);
	dm_int32 face = -1; // Inference: initial bestValue is the original negative-max-float constant at 0x7100e3bf0c.
	dm_float32 bestValue = -3.4028234663852886e+38f;
	for (dm_int32 i = 0; i < m_faceCount; ++i)
	{
		dm_float32 value = dmDot3_f32(direction, m_aPlanes[i]);
		if (value > bestValue)
		{
			dm_int32 edge0 = m_aFaceSubEdges[i];
			dm_int32 edge = edge0;
			do
			{
				if (m_aSubEdges[edge].tail == vertexIndex)
				{ // Inference: the face/value updates share the line-0 tail of the matched vertex branch.
					bestValue = value;
					face = i;
					break;
				}

				edge = m_aSubEdges[edge].next;
			} while (edge != edge0);
		}
	}


	dmAssert(face >= 0);
	return face;
}

#line 520 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
void dmPolytope::ApplyScale(dm_vec4Arg scale)
{
	for (dm_int32 i = 0; i < m_vertexCount; ++i)
	{
		m_aVertices[i] = scale * m_aVertices[i];
	}



	for (dm_int32 i = 0; i < m_faceCount; ++i)
	{ // Inference: original zero vector seeds both n and p accumulators.
		dm_vec4 n = dmVecSet(0.0f);
		dm_vec4 p = dmVecSet(0.0f);
		// Inference: original edge-loop setup starts at the face subedge, with count zero.
		dm_int32 e0 = m_aFaceSubEdges[i];
		dm_int32 e1 = e0;
		dm_int32 count = 0;

		do
		{
			dm_int32 e2 = m_aSubEdges[e1].next;

			dm_int32 i1 = m_aSubEdges[e1].tail;
			dm_vec4 v1 = m_aVertices[i1];

			dm_int32 i2 = m_aSubEdges[e2].tail;
			dm_vec4 v2 = m_aVertices[i2];

			n += dmCross(v1, v2);
			p += v1;
			++count;
			// Inference: advancing e1 is carried by the next-edge load and loop comparison.
			e1 = e2;
		} while (e1 != e0);

		// Inference: original zero-safe branch skips the face plane update.
		if (dmIsZeroSafe3(n) == true)
		{
			continue;
		}


		n = dmNormalize3(n);
		p *= 1.0f / count;
		dm_vec4 offset = dmDot3(p, n);
		m_aPlanes[i] = n;
		m_aPlanes[i] = dmSetW(m_aPlanes[i], offset);
	}
}

#line 970 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
void dmPartialPolytope::Create(const dmTriangle * pTri)
{ // Inference: tolerance literal is original immediate float bits 0x35a00000.
	const dm_vec4 tol = dmVecSet(1.1920928955078125e-6f);

	dm_vec4 v1 = pTri->m_p[0];
	dm_vec4 v2 = pTri->m_p[1];
	dm_vec4 v3 = pTri->m_p[2];

	dm_vec4 e1 = v2 - v1;
	dm_vec4 e2 = v3 - v2;
	dm_vec4 e3 = v1 - v3;
	dm_vec4 n = dmCross(e1, e2);
	n = dmNormalize3(n);
	m_aPlanes[0] = dmSetW(n, dmDot3(v1, n));




	dm_vec4 w1 = pTri->m_aWings[0];
	dm_vec4 w2 = pTri->m_aWings[1];
	dm_vec4 w3 = pTri->m_aWings[2];

	if (pTri->m_aShared[0])
	{
		dm_vec4 os1 = dmDot3(n, w1 - v1);
		dm_vec4 wn1 = dmCross(w1 - v1, e1);
		wn1 = dmNormalize3(wn1);
		m_aPlanes[1] = dmSelect_GT(os1, tol, n, wn1);
	}

	else

		m_aPlanes[1] = -n;
	m_aPlanes[1] = dmSetW(m_aPlanes[1], dmDot3(v1, m_aPlanes[1]));


	if (pTri->m_aShared[1])
	{
		dm_vec4 os2 = dmDot3(n, w2 - v2);
		dm_vec4 wn2 = dmCross(w2 - v2, e2);
		wn2 = dmNormalize3(wn2);
		m_aPlanes[2] = dmSelect_GT(os2, tol, n, wn2);
	}

	else

		m_aPlanes[2] = -n;
	m_aPlanes[2] = dmSetW(m_aPlanes[2], dmDot3(v2, m_aPlanes[2]));


	if (pTri->m_aShared[2])
	{
		dm_vec4 os3 = dmDot3(n, w3 - v3);
		dm_vec4 wn3 = dmCross(w3 - v3, e3);
		wn3 = dmNormalize3(wn3);
		m_aPlanes[3] = dmSelect_GT(os3, tol, n, wn3);
	}

	else

		m_aPlanes[3] = -n;
	m_aPlanes[3] = dmSetW(m_aPlanes[3], dmDot3(v3, m_aPlanes[3]));


	m_aVertices[0] = v1;
	m_aVertices[1] = v2;
	m_aVertices[2] = v3;
	m_centroid = dm_vec4_oneThird * (v1 + v2 + v3);
	// Inference: original stores four planes for the triangle and its three wings.
	m_planeCount = 4;
}



#line 1042 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"

bool dmPartialPolytope::CheckAxis(dm_vec4Arg a) const
{
// Inference: literal spelling reproduces original cos5deg bits 0x3f7f064f.
#line 1045 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    const dm_vec4 cos5deg = dmVecSet(0.99619f);
    const dm_vec4 n = m_aPlanes[0];


    dm_vec4 d = dmDot3(n, a);
    if (dmGT(d, cos5deg))
    {
// Inference: the original line-zero true result belongs to this early return.
#line 1052 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        return true;
    }

    dm_vec4 v1 = m_aVertices[0];
    dm_vec4 v2 = m_aVertices[1];
    dm_vec4 v3 = m_aVertices[2];

    dm_vec4 s1 = dmScalarTripleProduct(n, v1 - m_centroid, a);
    dm_vec4 s2 = dmScalarTripleProduct(n, v2 - m_centroid, a);
    dm_vec4 s3 = dmScalarTripleProduct(n, v3 - m_centroid, a);


    if (dmGEZ(s1) && dmLTZ(s2))
    {
        dm_vec4 t1 = dmScalarTripleProduct(v2 - v1, a, m_aPlanes[1]);
// Inference: original shared tail returns the nonnegative triple product for each selected edge.
#line 1067 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        return dmGEZ(t1);
    }


    if (dmGEZ(s2) && dmLTZ(s3))
    {
        dm_vec4 t2 = dmScalarTripleProduct(v3 - v2, a, m_aPlanes[2]);
// Inference: original shared tail returns the nonnegative triple product for each selected edge.
#line 1074 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        return dmGEZ(t2);
    }


    dm_vec4 t3 = dmScalarTripleProduct(v1 - v3, a, m_aPlanes[3]);
    return dmGEZ(t3);
}

#line 303 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"

dm_vec4 dmPolytope::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const
{ // Inference: original zero vector seeds the projected-area accumulator.
	dm_vec4 area = dmVecSet(0.0f);

	for (dm_int32 i = 0; i < m_faceCount; ++i)
	{




		dm_int32 faceEdge1 = GetFaceEdge(i);
		// Inference: the first fan vertex uses the face starting edge.
		dm_int32 faceEdge = faceEdge1;
		dm_vec4 v1 = GetTailVertex(faceEdge);

		faceEdge = GetNextEdge(faceEdge);
		dm_vec4 v2 = GetTailVertex(faceEdge);

		faceEdge = GetNextEdge(faceEdge);

		do
		{
			dm_vec4 v3 = GetTailVertex(faceEdge);


			dm_vec4 e1 = v2 - v1;
			dm_vec4 e2 = v3 - v1;
			dm_vec4 d = dmCross(e1, e2);


			dm_vec4 candidate = dmDot3(dir, d);


			area += dmMax(candidate, dmVecSet(0.0f));

			// Inference: original shared loop tail carries the last triangle vertex forward.
			v2 = v3;
			faceEdge = GetNextEdge(faceEdge);
		} while (faceEdge != faceEdge1);
	}



	return dm_vec4_oneHalf * area;

}

#line 418 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"

bool dmPolytope::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const
{
    dm_vec4 p1 = dmMulT(xf, pInput->p1);
    dm_vec4 p2 = dmMulT(xf, pInput->p2);
    // Inference: original zero vector initializes the clipping lower bound.
    dm_vec4 lower = dmVecSet(0.0f);
    dm_vec4 upper = pInput->maxLambda;

    const dm_vec4 slop = pInput->slop;
    // Inference: s aliases p1 without generating an instruction.
    dm_vec4 s = p1;
    dm_vec4 d = p2 - p1;
    dm_int32 index = -1;

    dm_int32 faceCount = m_faceCount;


    for (dm_int32 i = 0; i < faceCount; ++i)
    {




        dm_vec4 offset = dmSplatW(m_aPlanes[i]) + slop;
        dm_vec4 numerator = offset - dmDot3(s, m_aPlanes[i]);
        dm_vec4 denominator = dmDot3(d, m_aPlanes[i]);


        if (dmIsZeroSafe3(denominator))
        {
            // Inference: negative numerator takes the original shared false return.
            if (dmLTZ(numerator))
                return false;
        }
        else
        {








            if (dmLTZ(denominator) && dmLT(numerator, lower * denominator))
            {
                index = i;
                lower = numerator / denominator;
            }



            else if (dmGTZ(denominator) && dmLT(numerator, upper * denominator))
            {

                upper = numerator / denominator;
            }
        }

        // Inference: disjoint clipping bounds take the original shared false return.
        if (dmGT(lower, upper))
            return false;
    }



    dmAssert(dmGEZ(lower) && dmLE(lower, pInput->maxLambda));
    // Inference: line-zero signed index test gates the successful hit stores.
    if (index >= 0)
    {
        pOutput->lambda = lower;
        pOutput->normal = dmRotate(xf.q, m_aPlanes[index]);
        // Inference: original true result is merged into the rotation/store tail.
        return true;
    }
    return false;
}

#line 203 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
// NON_MATCHING: 6 of 278 instructions differ (register/scheduling) after the dmPolytope.h accessor bodies landed.
void dmPolytope::ComputeMass(dmMassData * pMassData, dm_float32 density) const
{


    // Inference: original prologue zero vectors initialize the four integrals.
    dm_vec4 integral1 = dmVecSet(0.0f);
    dm_vec4 integral2 = dmVecSet(0.0f);
    dm_vec4 integral3 = dmVecSet(0.0f);
    dm_vec4 integral4 = dmVecSet(0.0f);
    for (dm_int32 i = 0; i < m_faceCount; ++i)
    {




        dm_int32 faceEdge1 = GetFaceEdge(i);
        // Inference: first fan edge seeds the traversal without its own line row.
        dm_int32 faceEdge = faceEdge1;
        dm_vec4 v1 = GetTailVertex(faceEdge);

        faceEdge = GetNextEdge(faceEdge);
        dm_vec4 v2 = GetTailVertex(faceEdge);

        faceEdge = GetNextEdge(faceEdge);

        do
        {
            dm_vec4 v3 = GetTailVertex(faceEdge);


            dm_vec4 e1 = v2 - v1;
            dm_vec4 e2 = v3 - v1;
            dm_vec4 d = dmCross(e1, e2);

            dm_vec4 f1;
#line 238 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 f2;
#line 238 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 f3;
#line 238 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 g1;
#line 238 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 g2;
#line 238 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 g3;
            dm_vec4 temp0;
#line 239 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 temp1;
#line 239 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            dm_vec4 temp2;
            temp0 = v1 + v2;
            f1 = temp0 + v3;
    // Inference: invariant v1 square is hoisted from this source line in the original.
#line 242 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
            temp1 = v1 * v1;
            temp2 = temp1 + v2 * temp0;
            f2 = temp2 + v3 * f1;
            f3 = v1 * temp1 + v2 * temp2 + v3 * f2;
            g1 = f2 + v1 * (v1 + f1);
            g2 = f2 + v2 * (v2 + f1);
            g3 = f2 + v3 * (v3 + f1);

            dm_vec4 w1 = dmVecSet(dmGetY(v1), dmGetZ(v1), dmGetX(v1));
            dm_vec4 w2 = dmVecSet(dmGetY(v2), dmGetZ(v2), dmGetX(v2));
            dm_vec4 w3 = dmVecSet(dmGetY(v3), dmGetZ(v3), dmGetX(v3));

            integral1 += dmSplatX(d * f1);
            integral2 += d * f2;
            integral3 += d * f3;
            integral4 += d * (w1 * g1 + w2 * g2 + w3 * g3);

            // Inference: shared loop tail advances the previous fan vertex.
            v2 = v3;
            faceEdge = GetNextEdge(faceEdge);
        } while (faceEdge != faceEdge1);
    }

    // Inference: reciprocal literals reproduce original immediate float constants.
    const dm_vec4 oneOver6 = dmVecSet(1.0f / 6.0f);
    const dm_vec4 oneOver24 = dmVecSet(1.0f / 24.0f);
    const dm_vec4 oneOver60 = dmVecSet(1.0f / 60.0f);
    const dm_vec4 oneOver120 = dmVecSet(1.0f / 120.0f);

    integral1 *= oneOver6;
    integral2 *= oneOver24;
    integral3 *= oneOver60;
    integral4 *= oneOver120;

    dm_vec4 volume = integral1;
    // Inference: original zero result initializes the centroid for nonpositive volume.
#line 277 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dm_vec4 centroid = dmVecSet(0.0f);

    if (dmGTZ(volume))
    {
        centroid = integral2 / volume;
    }






    dm_float32 xx = dmGetX(integral3);
#line 289 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dm_float32 yy = dmGetY(integral3);
#line 289 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dm_float32 zz = dmGetZ(integral3);
    dm_float32 xy = dmGetX(integral4);
#line 290 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dm_float32 zx = dmGetZ(integral4);
#line 290 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dm_float32 yz = dmGetY(integral4);
    // Inference: no original initialization of I.p is observable; retain default initialization.
    dmMtx I;
    I.rx = dmVecSet(yy + zz, -xy, -zx);
    I.ry = dmVecSet(-xy, xx + zz, -yz);
    I.rz = dmVecSet(-zx, -yz, xx + yy);


    pMassData->mass = density * dmGetX(volume);
    pMassData->center = centroid;
    pMassData->I = density * I;
}



#line 24 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
// DWARF 07de6751: no standalone body to check; recovered from its real Create inline instances.
static dm_vec4 sConvert(const dmFloat3 & f)
{
    return dmVecSet(f.x, f.y, f.z);
}

#line 90 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"

void dmPolytope::Create(const dmPolytopeData * pData, const dmTransform & xf, dm_float32 scale, dmPoolAllocator * pAllocator)
{
    dmAssert(m_pAllocator == NULL && m_aVertices == NULL && m_isTriangle == false);

    dm_vec4 s = dmV4(scale);
    m_centroid = dmMul(xf, s * sConvert(pData->m_centroid));
    m_vertexCount = pData->m_vertexCount;
    m_faceCount = pData->m_faceCount;
    m_subEdgeCount = pData->m_subEdgeCount;
    m_volume = scale * scale * scale * pData->m_volume;
    m_area = scale * scale * pData->m_area;

    if (pAllocator)
    {
        m_pAllocator = pAllocator;
    // Inference: typed allocation casts convert the allocator void* result to the DWARF field types; no pointer arithmetic.
#line 106 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aVertices = (dm_vec4 *)m_pAllocator->Allocate(m_vertexCount * sizeof(dm_vec4), false);
    // Inference: allocator void* result is converted to the DWARF dm_vec4* field type.
#line 107 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aPlanes = (dm_vec4 *)m_pAllocator->Allocate(m_faceCount * sizeof(dm_vec4), false);
    // Inference: allocator void* result is converted to the DWARF dm_uint8* field type.
#line 108 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aFaceSubEdges = (dm_uint8 *)m_pAllocator->Allocate(m_faceCount, false);
    // Inference: allocator void* result is converted to the DWARF dmSubEdge* field type.
#line 109 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aSubEdges = (dmSubEdge *)m_pAllocator->Allocate(m_subEdgeCount * sizeof(dmSubEdge), false);
    }
    else
    {
    // Inference: dmAlloc void* result is converted to the DWARF dm_vec4* field type.
#line 113 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aVertices = (dm_vec4 *)dmAlloc(m_vertexCount * sizeof(dm_vec4));
    // Inference: dmAlloc void* result is converted to the DWARF dm_vec4* field type.
#line 114 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aPlanes = (dm_vec4 *)dmAlloc(m_faceCount * sizeof(dm_vec4));
    // Inference: dmAlloc void* result is converted to the DWARF dm_uint8* field type.
#line 115 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aFaceSubEdges = (dm_uint8 *)dmAlloc(m_faceCount);
    // Inference: dmAlloc void* result is converted to the DWARF dmSubEdge* field type.
#line 116 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        m_aSubEdges = (dmSubEdge *)dmAlloc(m_subEdgeCount * sizeof(dmSubEdge));
    }

    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        m_aVertices[i] = dmMul(xf, s * sConvert(pData->m_aVertices[i]));
    }

    for (dm_int32 i = 0; i < m_faceCount; ++i)
    {

        dmPlane plane = pData->m_aPlanes[i];
        dm_vec4 scaledPlane = dmVecSet(plane.normal.x, plane.normal.y, plane.normal.z, scale * plane.offset);
        m_aPlanes[i] = dmTransformPlane(xf, scaledPlane);
        m_aFaceSubEdges[i] = pData->m_aFaceSubEdges[i];
    }

    for (dm_int32 i = 0; i < m_subEdgeCount; ++i)
    {
        m_aSubEdges[i] = pData->m_aSubEdges[i];
    }
}

#line 760 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
// No standalone original body to check; DWARF 07de96fc and real caller inline instructions supply this helper.
static dm_vec4 sTetrahedronVolume(
    dm_vec4 * c,
    const dm_vec4 & p,
    const dm_vec4 & v1,
    const dm_vec4 & v2,
    const dm_vec4 & v3)
{
    dm_vec4 a = v2 - v1;
    dm_vec4 b = v3 - v1;
    dm_vec4 r = p - v1;

    dm_vec4 volume = dm_vec4_oneHalf * dm_vec4_oneThird * dmScalarTripleProduct(r, b, a);
    *c += dm_vec4_oneHalf * dm_vec4_oneHalf * volume * (v1 + v2 + v3 + p);
    // Inference: original returns the computed volume via the caller shared tail.
    return volume;
}


static dm_vec4 sClipTriangle(dm_vec4 * c, dm_vec4Arg p,
    dm_vec4Arg v1, dm_vec4Arg v2, dm_vec4Arg v3,
    dm_vec4Arg d1, dm_vec4Arg d2, dm_vec4Arg d3)
{

    dm_vec4 vc1 = v1 + d1 / (d1 - d2) * (v2 - v1);
    // Inference: original branch sums begin with a zero-volume accumulator.
#line 785 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
    dm_vec4 volume = dmVecSet(0.0f);

    if (dmLTZ(d1))
    {
        if (dmLTZ(d3))
        {

            dm_vec4 vc2 = v2 + d2 / (d2 - d3) * (v3 - v2);
            volume += sTetrahedronVolume(c, p, vc1, vc2, v1);
            volume += sTetrahedronVolume(c, p, vc2, v3, v1);
        }
        else
        {

            dm_vec4 vc2 = v1 + d1 / (d1 - d3) * (v3 - v1);
            volume += sTetrahedronVolume(c, p, vc1, vc2, v1);
        }
    }
    else
    {
        if (dmLTZ(d3))
        {

            dm_vec4 vc2 = v1 + d1 / (d1 - d3) * (v3 - v1);
            volume += sTetrahedronVolume(c, p, vc1, v2, v3);
            volume += sTetrahedronVolume(c, p, vc1, v3, vc2);
        }
        else
        {

            dm_vec4 vc2 = v2 + d2 / (d2 - d3) * (v3 - v2);
            volume += sTetrahedronVolume(c, p, vc1, v2, vc2);
        }
    }

    return volume;
}




dm_vec4 dmPolytope::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & xf, dm_vec4Arg normal, dm_vec4Arg offset) const
{







    dm_vec4 localNormal = dmInvRotate(xf.q, normal);
    dm_vec4 localOffset = offset - dmDot3(normal, xf.p);


    dmAssert(m_vertexCount <= dmPolytopeData::e_maxVertices);
    dm_vec4 ds[255];

    // Inference: the original initializes both counters to zero before the vertex loop.
    dm_int32 submergedCount = 0;
    dm_int32 sampleVert = 0;
    for (dm_int32 i = 0; i < m_vertexCount; ++i)
    {
        ds[i] = dmDot3(localNormal, m_aVertices[i]) - localOffset;
        if (dmLT(ds[i], -dm_vec4_epsilon))
        {
            ++submergedCount;
            sampleVert = i;
        }
    }


    if (submergedCount == 0)
    {
        *pCentroid = dmVecSet(0.0f);
    // Inference: original shared exit returns zero after clearing the centroid.
#line 859 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        return dmVecSet(0.0f);
    }






    dm_vec4 p = m_aVertices[sampleVert] - ds[sampleVert] * localNormal;

    // Inference: original zero vectors initialize volume and weighted centroid accumulators.
    dm_vec4 volume = dmVecSet(0.0f);
    dm_vec4 c = dmVecSet(0.0f);


    for (dm_int32 i = 0; i < m_faceCount; ++i)
    {




        dm_int32 faceEdge1 = GetFaceEdge(i);
        // Inference: fan traversal starts at the face edge.
        dm_int32 faceEdge = faceEdge1;
        dm_int32 i1 = m_aSubEdges[faceEdge].tail;
        dm_vec4 v1 = m_aVertices[i1];
        dm_vec4 d1 = ds[i1];

        faceEdge = GetNextEdge(faceEdge);
        dm_int32 i2 = m_aSubEdges[faceEdge].tail;
        dm_vec4 v2 = m_aVertices[i2];
        dm_vec4 d2 = ds[i2];

        faceEdge = GetNextEdge(faceEdge);

        do
        {
            dm_int32 i3 = m_aSubEdges[faceEdge].tail;
            dm_vec4 v3 = m_aVertices[i3];
            dm_vec4 d3 = ds[i3];


            if (dmLTZ(d1 * d2))
            {

                volume += sClipTriangle(&c, p, v1, v2, v3, d1, d2, d3);
            }
            else if (dmLTZ(d1 * d3))
            {

                volume += sClipTriangle(&c, p, v3, v1, v2, d3, d1, d2);
            }
            else if (dmLTZ(d2 * d3))
            {

                volume += sClipTriangle(&c, p, v2, v3, v1, d2, d3, d1);
            }
            else if (dmLTZ(d1) || dmLTZ(d2) || dmLTZ(d3))
            {

                volume += sTetrahedronVolume(&c, p, v1, v2, v3);
            }


            // Inference: shared loop tail advances the fan vertex and its signed distance.
            v2 = v3;
            d2 = d3;
            faceEdge = GetNextEdge(faceEdge);
        } while (faceEdge != faceEdge1);
    }




    if (dmLT(volume, dm_vec4_epsilon))
    {
    // Inference: original shared zero-volume tail clears the centroid.
#line 935 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        *pCentroid = dmVecSet(0.0f);
    // Inference: original shared exit returns zero after clearing the centroid.
#line 936 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp"
        return dmVecSet(0.0f);
    }


    c = c / volume;


    *pCentroid = dmMul(xf, c);
    // Inference: original final result is volume after transforming its centroid.
    return volume;
}
