#include "../../Include/dmTreeMesh.h"
#include "../../Include/dmMeshData.h"
#include "../../Include/dmCollision.h"
#include "../../Include/dmShapes.h"
#line 5 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmTreeMesh.cpp"















// dmMeshData.h:37 declares this; the DWARF defines it here (not declared inline).
inline const dmMeshNode* dmMeshNode::GetChild1() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
	return this + 1;
}

// dmMeshData.h:38 declares this; the DWARF defines it here (not declared inline).
inline const dmMeshNode* dmMeshNode::GetChild2() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
	dmAssert(triangleCount == 0);
	return this + index;
}


static dm_vec4 sConvert(const dmFloat3& f)
{
	return dmVecSet(f.x, f.y, f.z);
}


dmTreeMesh::dmTreeMesh()
{
	m_type = e_mesh; m_radius = 0.0f;
	m_meshType = e_treeMesh; m_triangleCount = 0;

	m_scale = dm_vec4_one; m_invScale = dm_vec4_one;




	m_pRoot = NULL; m_aVertices = NULL; m_aTriangles = NULL; m_nodeCount = 0; m_vertexCount = 0; m_height = 0;






}


void dmTreeMesh::Create(const dmMeshData * pData, dm_vec4Arg scale)
{
	m_scale = scale;
	m_invScale = dmSafeReciprocal(scale);

	m_center = sConvert(pData->m_center);
	m_extents = sConvert(pData->m_extents);
	m_unquantize = sConvert(pData->m_unquantize);
	// inferred: dmMeshData stores dmFloat4 vertices; the column (56) fits this cast

	m_aVertices = reinterpret_cast<const dm_vec4*>(pData->m_aVertices);
	m_aTriangles = pData->m_aTriangles;
	m_pRoot = pData->m_pRoot;

	m_vertexCount = pData->m_vertexCount;
	m_triangleCount = pData->m_triangleCount;
	m_nodeCount = pData->m_nodeCount;
	m_height = pData->m_height;
}


void dmTreeMesh::GetTriangleFull(dmTriangle * pTriangle, dm_int32 index) const
{
	dmAssert(0 <= index && index < m_triangleCount);
	const dmMeshTriangle* pt = m_aTriangles + index;


	pTriangle->m_p[0] = m_scale * (m_aVertices[pt->i1] + m_center);
	pTriangle->m_p[1] = m_scale * (m_aVertices[pt->i2] + m_center);
	pTriangle->m_p[2] = m_scale * (m_aVertices[pt->i3] + m_center);


	// inferred: -1 marks a missing wing (no named constant in the DWARF)
	if (pt->w1 != -1)
	{
		pTriangle->m_aWings[0] = m_scale * (m_aVertices[pt->w1] + m_center);
		pTriangle->m_aShared[0] = true;
	}
	else
	{
		// inferred from the stored register: the opposite vertex
		pTriangle->m_aWings[0] = pTriangle->m_p[2];
		pTriangle->m_aShared[0] = false;
	}
	if (pt->w2 != -1)
	{
		pTriangle->m_aWings[1] = m_scale * (m_aVertices[pt->w2] + m_center);
		pTriangle->m_aShared[1] = true;
	}
	else
	{
		pTriangle->m_aWings[1] = pTriangle->m_p[0];
		pTriangle->m_aShared[1] = false;
	}

	if (pt->w3 != -1)
	{
		pTriangle->m_aWings[2] = m_scale * (m_aVertices[pt->w3] + m_center);
		pTriangle->m_aShared[2] = true;
	}
	else
	{
		pTriangle->m_aWings[2] = pTriangle->m_p[1];
		pTriangle->m_aShared[2] = false;
	}

	pTriangle->m_aIndices[0] = pt->i1;
	pTriangle->m_aIndices[1] = pt->i2;
	pTriangle->m_aIndices[2] = pt->i3;

	pTriangle->m_aWingIndices[0] = pt->w1;
	pTriangle->m_aWingIndices[1] = pt->w2;
	pTriangle->m_aWingIndices[2] = pt->w3;

	pTriangle->m_triangleIndex = index;
	pTriangle->m_materialIndex = pt->materialIndex;
	pTriangle->m_gameFlags = pt->collisionFlags;
}


dmAABB dmTreeMesh::GetAABB(const dmMtx & transform) const
{
	dmAABB aabb;
	if (m_vertexCount == 0)
	{
		aabb.m_lowerBound = -dm_vec4_one;
		aabb.m_upperBound = dm_vec4_one;
		return aabb;
	}


	dmMtx A;
	A.rx = dmSplatX(m_scale) * transform.rx;
	A.ry = dmSplatY(m_scale) * transform.ry;
	A.rz = dmSplatZ(m_scale) * transform.rz;
	A.p = transform.p;

	dm_vec4 lower =  dm_vec4_maxFloat;
	dm_vec4 upper = -dm_vec4_maxFloat;

	for (dm_int32 i = 0; i < m_vertexCount; ++i)
	{

		dm_vec4 p = dmMul34(A, m_aVertices[i] + m_center);
		lower = dmMin(lower, p);
		upper = dmMax(upper, p);
	}

	dm_vec4 r = dmV4(m_radius);
	aabb.m_lowerBound = lower - r;
	aabb.m_upperBound = upper + r;
	return aabb;
}


void dmTreeMesh::Query(dmBuffer<int, 128> * pBuffer, const dmAABB & aabbIn) const
{
	if (m_pRoot == NULL)
	{
		return;
	}


	dmAABB aabb;
	aabb.m_lowerBound = m_invScale * aabbIn.m_lowerBound - m_center;
	aabb.m_upperBound = m_invScale * aabbIn.m_upperBound - m_center;

	const dm_int32 k_stackSize = 128;
	if (m_height >= k_stackSize)
	{
		dmAssert(false);
		return;
	}

	const dm_vec4 unquantize = m_unquantize;

	const dmMeshNode* nodeStack[k_stackSize];
	dm_int32 stackCount = 0;
	nodeStack[stackCount++] = m_pRoot;

	while (stackCount != 0)
	{
		const dmMeshNode* pNode = nodeStack[--stackCount];

		dmAABB b;





















		b.m_lowerBound = unquantize * dmVecSet(pNode->lowerX, pNode->lowerY, pNode->lowerZ);
		b.m_upperBound = unquantize * dmVecSet(pNode->upperX, pNode->upperY, pNode->upperZ);


		if (dmTestOverlap(aabb, b) == false)
		{
			continue;
		}

		if (pNode->triangleCount == 0)
		{
			nodeStack[stackCount++] = pNode->GetChild2();
			nodeStack[stackCount++] = pNode->GetChild1();
		}
		else
		{
			dm_int32 index = pNode->index;
			dm_int32 count = pNode->triangleCount;

			for (dm_int32 i = 0; i < count; ++i)
			{

				const dmMeshTriangle* pt = m_aTriangles + index + i;
				dm_vec4 v1 = m_aVertices[pt->i1];
				dm_vec4 v2 = m_aVertices[pt->i2];
				dm_vec4 v3 = m_aVertices[pt->i3];

				dmAABB triangleAABB;
				triangleAABB.m_lowerBound = dmMin(v1, v2, v3);
				triangleAABB.m_upperBound = dmMax(v1, v2, v3);

				bool overlap = dmTestOverlap(aabb, triangleAABB);

				if (overlap)
				{
					pBuffer->PushBack(index + i);
				}
			}
		}
	}
	// inferred: the DWARF keeps this local (line 268) with no code of its own

	dm_int32 triangleCount = pBuffer->GetCount();
	(void)triangleCount;



}


bool dmTreeMesh::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const
{
	if (m_pRoot == NULL)
	{
		return false;
	}

	dm_vec4 lambda = pInput->maxLambda;

	dm_vec4 s = m_invScale * dmMulT(xf, pInput->p1) - m_center;
	dm_vec4 t = m_invScale * dmMulT(xf, pInput->p2) - m_center;
	dm_vec4 d = t - s;

	dm_vec4 f = s + lambda * d;
	dmAABB rayAABB;
	rayAABB.m_lowerBound = dmMin(s, f);
	rayAABB.m_upperBound = dmMax(s, f);

	dm_vec4 dx = dmSplatX(d);
	dm_vec4 dy = dmSplatY(d);
	dm_vec4 dz = dmSplatZ(d);
	dm_vec4 ds[3] = { dx, dy, dz };


	const dm_int32 k_stackSize = 128;
	dmAssert(m_height < k_stackSize);

	const dm_vec4 unquantize = m_unquantize;

	const dmMeshNode* aStack[k_stackSize];
	dm_int32 count = 0;
	aStack[count++] = m_pRoot;

	bool anyHit = false;
	dm_int32 nodeVisitCount = 0;
	dm_int32 leafVisitCount = 0;

	while (count > 0)
	{
		const dmMeshNode* pNode = aStack[--count];

		dmAABB b;






















		b.m_lowerBound = unquantize * dmVecSet(pNode->lowerX, pNode->lowerY, pNode->lowerZ);
		b.m_upperBound = unquantize * dmVecSet(pNode->upperX, pNode->upperY, pNode->upperZ);


		if (dmTestOverlap(b, rayAABB) == false)
		{
			continue;
		}

		dm_vec4 h = b.GetExtents();
		dm_vec4 c = b.GetCenter();
		dm_vec4 sc = s - c;
		dm_vec4 edgeSeparation = dmAbs(dmCross(sc, d)) - dmModifiedCross(h, dmAbs(d));
		if (dmAny_GT3(edgeSeparation, vdupq_n_f32(0.0f)))
		{
			continue;
		}


		if (pNode->triangleCount == 0)
		{
			const dm_int32 axisIndex = pNode->axis;

			if (dmGTZ(ds[axisIndex]))
			{
				aStack[count++] = pNode->GetChild2();
				aStack[count++] = pNode->GetChild1();
			}
			else
			{
				aStack[count++] = pNode->GetChild1();
				aStack[count++] = pNode->GetChild2();
			}

			++nodeVisitCount;
		}
		else
		{
			++leafVisitCount;

			dm_int32 triangleCount = pNode->triangleCount;
			dm_int32 firstTriangleIndex = pNode->index;
			const dmMeshTriangle* pTri = m_aTriangles + firstTriangleIndex;
			for (dm_int32 i = 0; i < triangleCount; ++i, ++pTri)
			{
				if (pTri->collisionFlags & pInput->triangleMask)
				{
					dmRayCastOutput output;

					dmTriangle triangle;
					triangle.m_p[0] = m_aVertices[pTri->i1];
					triangle.m_p[1] = m_aVertices[pTri->i2];
					triangle.m_p[2] = m_aVertices[pTri->i3];

					bool hit = triangle.RayCast(&output, s, t, lambda);

					if (hit)
					{
						anyHit = true;


						dmTriangle scaledTriangle;
						scaledTriangle.m_p[0] = m_scale * triangle.m_p[0];
						scaledTriangle.m_p[1] = m_scale * triangle.m_p[1];
						scaledTriangle.m_p[2] = m_scale * triangle.m_p[2];
						dm_vec4 scaledNormal = scaledTriangle.GetNormal();

						pOutput->normal = dmRotate(xf.q, scaledNormal);
						pOutput->lambda = output.lambda;
						pOutput->triangleIndex = firstTriangleIndex + i;
						pOutput->materialID = m_aTriangles[firstTriangleIndex + i].materialIndex;
						lambda = output.lambda;

						f = s + lambda * d;
						rayAABB.m_lowerBound = dmMin(s, f);
						rayAABB.m_upperBound = dmMax(s, f);
					}
				}
			}
		}
	}







	pOutput->nodeVisitCount = nodeVisitCount;
	pOutput->leafVisitCount = leafVisitCount;

	return anyHit;
}

















































































void dmTreeMesh::GetTriangle(dmTriangle * pTriangle, dm_int32 index) const
{
	dmAssert(0 <= index && index < m_triangleCount);
	const dmMeshTriangle* pt = m_aTriangles + index;


	pTriangle->m_p[0] = m_scale * (m_aVertices[pt->i1] + m_center);
	pTriangle->m_p[1] = m_scale * (m_aVertices[pt->i2] + m_center);
	pTriangle->m_p[2] = m_scale * (m_aVertices[pt->i3] + m_center);
}


void dmTreeMesh::GetTriangleTransformed(dmTriangle * pTriangle, dm_int32 index, const dmTransform & xf) const
{
	dmAssert(0 <= index && index < m_triangleCount);



	const dmMeshTriangle* pt = m_aTriangles + index;

	pTriangle->m_p[0] = dmMul(xf, m_scale * (m_aVertices[pt->i1] + m_center));
	pTriangle->m_p[1] = dmMul(xf, m_scale * (m_aVertices[pt->i2] + m_center));
	pTriangle->m_p[2] = dmMul(xf, m_scale * (m_aVertices[pt->i3] + m_center));
}





























void dmTreeMesh::ApplyScale(dm_vec4Arg scale)
{
	m_scale = scale * m_scale;
	m_invScale = m_invScale / scale;
}

