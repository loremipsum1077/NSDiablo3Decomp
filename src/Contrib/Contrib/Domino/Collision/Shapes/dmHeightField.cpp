#include "../../Include/dmHeightField.h"
#include "../../Include/dmHeightData.h"
#include "../../Include/dmShapes.h"
#line 4 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmHeightField.cpp"


#include <float.h>  // FLT_MAX for RayCast (inferred; the original loads the literal 3.4028235e+38)














dmHeightField::dmHeightField()
{
    m_type = e_mesh; m_radius = 0.0f;
    m_meshType = e_heightField;

    m_triangleCount = 0; m_countX = 0; m_countY = 0; m_cellWidth = 0.0f; m_center = 0.0f; m_scale = 0.0f;

    m_aSamples = NULL;

}




void dmHeightField::Create(const dmHeightData * pData)
{
    m_cellWidth = pData->cellWidth;
    m_countX = pData->countX;
    m_countY = pData->countY;
    m_aSamples = pData->aSamples;
    m_scale = pData->scale;
    m_center = pData->center;
    m_triangleCount = 2 * (m_countX - 1) * (m_countY - 1);
}


bool dmHeightField::IsSolid(dm_int32 index) const
{
    dmAssert(0 <= index && index < m_triangleCount);
    dmAssert(m_countX >= 2 && m_countY >= 2);

    dm_int32 cellIndex = index >> 1;

    dm_int32 iy = cellIndex / (m_countY - 1);


    dm_int32 sampleIndex = iy + cellIndex;
    bool isSolid = (m_aSamples[sampleIndex].flags & dmHeightSample::e_hole) == 0;
    return isSolid;
}


dm_int32 dmHeightField::GetMaterial(dm_int32 index) const
{
    dmAssert(0 <= index && index < m_triangleCount);
    dmAssert(m_countX >= 2 && m_countY >= 2);

    dm_int32 cellIndex = index >> 1;

    dm_int32 iy = cellIndex / (m_countY - 1);


    dm_int32 sampleIndex = iy + cellIndex;
    return m_aSamples[sampleIndex].material;
}









void dmHeightField::GetTriangle(dmTriangle * pTriangle, dm_int32 index) const
{
	dmAssert(0 <= index && index < m_triangleCount);
	dmAssert(m_countX >= 2 && m_countY >= 2);

	dm_int32 cellIndex = index >> 1;
	dm_int32 ix1 = cellIndex / (m_countY - 1);
	dm_int32 iy1 = cellIndex - ix1 * (m_countY - 1);
	dm_int32 ix2 = ix1 + 1;
	dm_int32 iy2 = iy1 + 1;

	dm_float32 z11 = m_aSamples[m_countY * ix1 + iy1].height * m_scale + m_center;
	dm_float32 z12 = m_aSamples[m_countY * ix1 + iy2].height * m_scale + m_center;
	dm_float32 z21 = m_aSamples[m_countY * ix2 + iy1].height * m_scale + m_center;
	dm_float32 z22 = m_aSamples[m_countY * ix2 + iy2].height * m_scale + m_center;

	dm_float32 x1 = ix1 * m_cellWidth;
	dm_float32 y1 = iy1 * m_cellWidth;
	dm_float32 x2 = x1 + m_cellWidth;
	dm_float32 y2 = y1 + m_cellWidth;

	pTriangle->m_materialIndex = m_aSamples[m_countY * ix1 + iy1].material;
	pTriangle->m_gameFlags = 0xFFFF;  // inferred: the original stores 0xffff (no named constant found)


	if ((ix1 & 1) == (iy1 & 1))  // inferred from eor + tbnz bit 0 at column 16 (==)
	{


		if ((index & 1) == 0)
		{
			pTriangle->m_p[0] = dmVecSet(x1, y1, z11);
			pTriangle->m_p[1] = dmVecSet(x2, y1, z21);
			pTriangle->m_p[2] = dmVecSet(x2, y2, z22);
		}
		else
		{
			pTriangle->m_p[0] = dmVecSet(x1, y1, z11);
			pTriangle->m_p[1] = dmVecSet(x2, y2, z22);
			pTriangle->m_p[2] = dmVecSet(x1, y2, z12);
		}
	}
	else
	{


		if ((index & 1) == 0)
		{
			pTriangle->m_p[0] = dmVecSet(x1, y1, z11);
			pTriangle->m_p[1] = dmVecSet(x2, y1, z21);
			pTriangle->m_p[2] = dmVecSet(x1, y2, z12);
		}
		else
		{
			pTriangle->m_p[0] = dmVecSet(x2, y1, z21);
			pTriangle->m_p[1] = dmVecSet(x2, y2, z22);
			pTriangle->m_p[2] = dmVecSet(x1, y2, z12);
		}
	}
}


void dmHeightField::GetTriangleTransformed(dmTriangle * pTriangle, dm_int32 index, const dmTransform & xf) const
{
	GetTriangle(pTriangle, index);
	pTriangle->m_p[0] = dmMul(xf, pTriangle->m_p[0]);
	pTriangle->m_p[1] = dmMul(xf, pTriangle->m_p[1]);
	pTriangle->m_p[2] = dmMul(xf, pTriangle->m_p[2]);
}


void dmHeightField::GetTriangleFull(dmTriangle * pTriangle, dm_int32 index) const
{
	dmAssert(0 <= index && index < m_triangleCount);
	dmAssert(m_countX >= 2 && m_countY >= 2);

	dm_int32 cellIndex = index >> 1;
	dm_int32 ix1 = cellIndex / (m_countY - 1);
	dm_int32 iy1 = cellIndex - ix1 * (m_countY - 1);
	dm_int32 ix2 = ix1 + 1;
	dm_int32 iy2 = iy1 + 1;


	dm_int32 ix0 = ix1 - 1;
	dm_int32 iy0 = iy1 - 1;
	dm_int32 ix3 = ix1 + 2;
	dm_int32 iy3 = iy1 + 2;


	dm_int32 ix0c = dmMax(ix0, 0);
	dm_int32 iy0c = dmMax(iy0, 0);
	dm_int32 ix3c = dmMin(m_countX - 1, ix3);
	dm_int32 iy3c = dmMin(m_countY - 1, iy3);

	dm_int32 i11 = m_countY * ix1 + iy1;
	dm_int32 i12 = m_countY * ix1 + iy2;
	dm_int32 i21 = m_countY * ix2 + iy1;
	dm_int32 i22 = m_countY * ix2 + iy2;


	dm_int32 i01 = m_countY * ix0c + iy1;
	dm_int32 i02 = m_countY * ix0c + iy2;
	dm_int32 i10 = m_countY * ix1 + iy0c;
	dm_int32 i13 = m_countY * ix1 + iy3c;
	dm_int32 i20 = m_countY * ix2 + iy0c;
	dm_int32 i23 = m_countY * ix2 + iy3c;
	dm_int32 i31 = m_countY * ix3c + iy1;
	dm_int32 i32 = m_countY * ix3c + iy2;

	dm_float32 z11 = m_aSamples[i11].height * m_scale + m_center;
	dm_float32 z12 = m_aSamples[i12].height * m_scale + m_center;
	dm_float32 z21 = m_aSamples[i21].height * m_scale + m_center;
	dm_float32 z22 = m_aSamples[i22].height * m_scale + m_center;


	dm_float32 z01 = m_aSamples[i01].height * m_scale + m_center;
	dm_float32 z02 = m_aSamples[i02].height * m_scale + m_center;
	dm_float32 z10 = m_aSamples[i10].height * m_scale + m_center;
	dm_float32 z13 = m_aSamples[i13].height * m_scale + m_center;
	dm_float32 z20 = m_aSamples[i20].height * m_scale + m_center;
	dm_float32 z23 = m_aSamples[i23].height * m_scale + m_center;
	dm_float32 z31 = m_aSamples[i31].height * m_scale + m_center;
	dm_float32 z32 = m_aSamples[i32].height * m_scale + m_center;

	dm_float32 x1 = ix1 * m_cellWidth;
	dm_float32 y1 = iy1 * m_cellWidth;
	dm_float32 x0 = x1 - m_cellWidth;
	dm_float32 y0 = y1 - m_cellWidth;
	dm_float32 x2 = x1 + m_cellWidth;
	dm_float32 y2 = y1 + m_cellWidth;
	dm_float32 x3 = x2 + m_cellWidth;
	dm_float32 y3 = y2 + m_cellWidth;


	if ((ix1 & 1) == (iy1 & 1))
	{


		if ((index & 1) == 0)
		{

			pTriangle->m_p[0] = dmVecSet(x1, y1, z11);
			pTriangle->m_p[1] = dmVecSet(x2, y1, z21);
			pTriangle->m_p[2] = dmVecSet(x2, y2, z22);

			pTriangle->m_aIndices[0] = i11 + m_countY + 1;
			pTriangle->m_aIndices[1] = i21 + m_countY + 1;
			pTriangle->m_aIndices[2] = i22 + m_countY + 1;

			pTriangle->m_aWings[0] = dmVecSet(x2, y0, z20);
			pTriangle->m_aWings[1] = dmVecSet(x3, y1, z31);
			pTriangle->m_aWings[2] = dmVecSet(x1, y2, z12);

			pTriangle->m_aWingIndices[0] = m_countY * ix2 + iy0 + m_countY + 1;
			pTriangle->m_aWingIndices[1] = m_countY * ix3 + iy1 + m_countY + 1;
			pTriangle->m_aWingIndices[2] = m_countY * ix1 + iy2 + m_countY + 1;
		}
		else
		{

			pTriangle->m_p[0] = dmVecSet(x1, y1, z11);
			pTriangle->m_p[1] = dmVecSet(x2, y2, z22);
			pTriangle->m_p[2] = dmVecSet(x1, y2, z12);

			pTriangle->m_aIndices[0] = i11 + m_countY + 1;
			pTriangle->m_aIndices[1] = i22 + m_countY + 1;
			pTriangle->m_aIndices[2] = i12 + m_countY + 1;

			pTriangle->m_aWings[0] = dmVecSet(x2, y1, z21);
			pTriangle->m_aWings[1] = dmVecSet(x1, y3, z13);
			pTriangle->m_aWings[2] = dmVecSet(x0, y2, z02);

			pTriangle->m_aWingIndices[0] = m_countY * ix2 + iy1 + m_countY + 1;
			pTriangle->m_aWingIndices[1] = m_countY * ix1 + iy3 + m_countY + 1;
			pTriangle->m_aWingIndices[2] = m_countY * ix0 + iy2 + m_countY + 1;
		}
	}
	else
	{


		if ((index & 1) == 0)
		{

			pTriangle->m_p[0] = dmVecSet(x1, y1, z11);
			pTriangle->m_p[1] = dmVecSet(x2, y1, z21);
			pTriangle->m_p[2] = dmVecSet(x1, y2, z12);

			pTriangle->m_aIndices[0] = i11 + m_countY + 1;
			pTriangle->m_aIndices[1] = i21 + m_countY + 1;
			pTriangle->m_aIndices[2] = i12 + m_countY + 1;

			pTriangle->m_aWings[0] = dmVecSet(x1, y0, z10);
			pTriangle->m_aWings[1] = dmVecSet(x2, y2, z22);
			pTriangle->m_aWings[2] = dmVecSet(x0, y1, z01);

			pTriangle->m_aWingIndices[0] = m_countY * ix1 + iy0 + m_countY + 1;
			pTriangle->m_aWingIndices[1] = m_countY * ix2 + iy2 + m_countY + 1;
			pTriangle->m_aWingIndices[2] = m_countY * ix0 + iy1 + m_countY + 1;
		}
		else
		{

			pTriangle->m_p[0] = dmVecSet(x2, y1, z21);
			pTriangle->m_p[1] = dmVecSet(x2, y2, z22);
			pTriangle->m_p[2] = dmVecSet(x1, y2, z12);

			pTriangle->m_aIndices[0] = i21 + m_countY + 1;
			pTriangle->m_aIndices[1] = i22 + m_countY + 1;
			pTriangle->m_aIndices[2] = i12 + m_countY + 1;

			pTriangle->m_aWings[0] = dmVecSet(x3, y2, z32);
			pTriangle->m_aWings[1] = dmVecSet(x2, y3, z23);
			pTriangle->m_aWings[2] = dmVecSet(x1, y1, z11);

			pTriangle->m_aWingIndices[0] = m_countY * ix3 + iy2 + m_countY + 1;
			pTriangle->m_aWingIndices[1] = m_countY * ix2 + iy3 + m_countY + 1;
			pTriangle->m_aWingIndices[2] = m_countY * ix1 + iy1 + m_countY + 1;
		}
	}

	pTriangle->m_aShared[0] = true;
	pTriangle->m_aShared[1] = true;
	pTriangle->m_aShared[2] = true;





	pTriangle->m_triangleIndex = index;
	pTriangle->m_materialIndex = m_aSamples[m_countY * ix1 + iy1].material;
}


dmAABB dmHeightField::GetAABB(const dmMtx & xf) const
{
	dm_vec4 lower = dm_vec4_maxFloat;
	dm_vec4 upper = -dm_vec4_maxFloat;

	for (dm_int32 i = 0; i < m_countX; ++i)
	{
		dm_float32 x = i * m_cellWidth;
		for (dm_int32 j = 0; j < m_countY; ++j)
		{
			dm_float32 y = j * m_cellWidth;
			dm_float32 z = m_scale * m_aSamples[i * m_countY + j].height + m_center;
			dm_vec4 p = dmVecSet(x, y, z);
			p = dmMul34(xf, p);
			lower = dmMin(lower, p);
			upper = dmMax(upper, p);
		}
	}

	dm_vec4 r = dmV4(m_radius);
	dmAABB aabb;
	aabb.m_lowerBound = lower - r;
	aabb.m_upperBound = upper + r;
	return aabb;
}

// NON_MATCHING: 524 vs 524 bytes, same calls and flow; 31/131 insns differ only in where index+1 is computed (ours before the first PushBack, original after its store) and the resulting branch offsets. index++ is inferred; PushBack(index)/PushBack(index + 1) makes LSR pick index+1 as the IV (55/131). Tried base/index operand orders, ++index, index | 1.
void dmHeightField::Query(dmBuffer<int, 128> * pBuffer, const dmAABB & aabb) const
{

	dm_vec4 lower = aabb.m_lowerBound;
	dm_vec4 upper = aabb.m_upperBound;
	dm_int32 ix1 = (dm_int32)floorf(dmGetX(lower) / m_cellWidth);
	dm_int32 iy1 = (dm_int32)floorf(dmGetY(lower) / m_cellWidth);
	dm_int32 ix2 = (dm_int32)floorf(dmGetX(upper) / m_cellWidth);
	dm_int32 iy2 = (dm_int32)floorf(dmGetY(upper) / m_cellWidth);

	for (dm_int32 ix = ix1; ix <= ix2; ++ix)
	{
		if (ix < 0 || m_countX - 2 < ix)
		{
			continue;
		}

		dm_int32 base = 2 * ix * (m_countY - 1);

		for (dm_int32 iy = iy1; iy <= iy2; ++iy)
		{
			if (iy < 0 || m_countY - 2 < iy)
			{
				continue;
			}

			if (m_aSamples[ix * m_countY + iy].flags & dmHeightSample::e_hole)
			{
				continue;
			}

			dm_int32 index = base + 2 * iy;

			pBuffer->PushBack(index++);
			pBuffer->PushBack(index);
		}
	}
}


bool dmHeightField::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const
{

	dm_vec4 p1 = dmMulT(xf, pInput->p1);
	dm_vec4 p2 = dmMulT(xf, pInput->p2);
	dm_float32 p1x = dmGetX(p1), p1y = dmGetY(p1);
	dm_float32 p2x = dmGetX(p2), p2y = dmGetY(p2);


	dm_int32 ix1 = (dm_int32)floorf(p1x / m_cellWidth);
	dm_int32 iy1 = (dm_int32)floorf(p1y / m_cellWidth);


	dm_int32 ix2 = (dm_int32)floorf(p2x / m_cellWidth);
	dm_int32 iy2 = (dm_int32)floorf(p2y / m_cellWidth);


	dm_float32 xLo = m_cellWidth * ix1;
	dm_float32 xHi = xLo + m_cellWidth;


	dm_float32 yLo = m_cellWidth * iy1;
	dm_float32 yHi = yLo + m_cellWidth;


	dm_int32 dix, diy;


	dm_float32 tx, ty;


	dm_float32 deltaX, deltaY;


	if (ix1 < ix2)
	{
		dix = 1;
		dm_float32 factor = 1.0f / dmAbs(p2x - p1x);
		tx = (xHi - p1x) * factor;
		deltaX = m_cellWidth * factor;
	}
	else if (ix2 < ix1)
	{
		dix = -1;
		dm_float32 factor = 1.0f / dmAbs(p2x - p1x);
		tx = (p1x - xLo) * factor;
		deltaX = m_cellWidth * factor;
	}
	else
	{
		dix = 0;
		tx = FLT_MAX;  // inferred: the original loads 3.4028235e+38 (no named constant in the DWARF)
		deltaX = 0.0f;
	}

	if (iy1 < iy2)
	{
		diy = 1;
		dm_float32 factor = 1.0f / dmAbs(p2y - p1y);
		ty = (yHi - p1y) * factor;
		deltaY = m_cellWidth * factor;
	}
	else if (iy2 < iy1)
	{
		diy = -1;
		dm_float32 factor = 1.0f / dmAbs(p2y - p1y);
		ty = (p1y - yLo) * factor;
		deltaY = m_cellWidth * factor;
	}
	else
	{
		diy = 0;
		ty = FLT_MAX;  // inferred: the original loads 3.4028235e+38 (no named constant in the DWARF)
		deltaY = 0.0f;
	}

	dm_vec4 maxLambda = pInput->maxLambda;

	bool haveHit = false;

	dm_int32 ix = ix1, iy = iy1;

	for (;;)
	{

		if (ix >= 0 && ix < m_countX-1 && iy >= 0 && iy < m_countY-1)
		{
			bool isSolid = (m_aSamples[m_countY * ix + iy].flags & dmHeightSample::e_hole) == 0;

			if (isSolid)
			{

				dmRayCastInput input;
				dmRayCastOutput output;
				input.p1 = p1;
				input.p2 = p2;
				input.maxLambda = maxLambda;

				bool hit = RayCast(&output, &input, ix, iy);
				if (hit)
				{
					haveHit = true;
					pOutput->normal = dmRotate(xf.q, output.normal);
					pOutput->triangleIndex = output.triangleIndex;
					pOutput->materialID = output.materialID;
					pOutput->lambda = output.lambda;
					maxLambda = output.lambda;
				}
			}
		}




		if (tx > dmF32(maxLambda) + 0.01f && ty > dmF32(maxLambda) + 0.01f)  // inferred: 0.01f literal (no named constant found)
		{
			break;
		}


		if (tx <= ty)
		{
			if (ix == ix2)
			{
				break;
			}

			tx += deltaX;
			ix += dix;
		}
		else
		{
			if (iy == iy2)
			{
				break;
			}
			ty += deltaY;
			iy += diy;
		}
	}

	return haveHit;
}

































bool dmHeightField::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, dm_int32 ix1, dm_int32 iy1) const
{
	if (ix1 < 0 || m_countX <= ix1 || iy1 < 0 || m_countY <= iy1)
	{
		return false;
	}

	dm_int32 ix2 = ix1 + 1;
	dm_int32 iy2 = iy1 + 1;

	dm_float32 z11 = m_aSamples[m_countY * ix1 + iy1].height * m_scale + m_center;
	dm_float32 z12 = m_aSamples[m_countY * ix1 + iy2].height * m_scale + m_center;
	dm_float32 z21 = m_aSamples[m_countY * ix2 + iy1].height * m_scale + m_center;
	dm_float32 z22 = m_aSamples[m_countY * ix2 + iy2].height * m_scale + m_center;

	dm_float32 x1 = ix1 * m_cellWidth;
	dm_float32 y1 = iy1 * m_cellWidth;
	dm_float32 x2 = x1 + m_cellWidth;
	dm_float32 y2 = y1 + m_cellWidth;

	bool haveHit = false;
	dm_vec4 p1 = pInput->p1;
	dm_vec4 p2 = pInput->p2;
	dm_vec4 maxLambda = pInput->maxLambda;


	if ((ix1 & 1) == (iy1 & 1))
	{




		{
			dmTriangle triangle;
			triangle.m_p[0] = dmVecSet(x1, y1, z11);
			triangle.m_p[1] = dmVecSet(x2, y1, z21);
			triangle.m_p[2] = dmVecSet(x2, y2, z22);

			dmRayCastOutput output;
			bool hit = triangle.RayCast(&output, p1, p2, maxLambda);
			if (hit)
			{
				haveHit = true;
				*pOutput = output;
				pOutput->triangleIndex = 2 * ((m_countY-1) * ix1 + iy1);
				pOutput->materialID = m_aSamples[m_countY * ix1 + iy1].material;
				maxLambda = output.lambda;
			}
		}

		{
			dmTriangle triangle;
			triangle.m_p[0] = dmVecSet(x1, y1, z11);
			triangle.m_p[1] = dmVecSet(x2, y2, z22);
			triangle.m_p[2] = dmVecSet(x1, y2, z12);

			dmRayCastOutput output;
			bool hit = triangle.RayCast(&output, p1, p2, maxLambda);
			if (hit)
			{
				haveHit = true;
				*pOutput = output;
				pOutput->triangleIndex = 2 * ((m_countY-1) * ix1 + iy1) + 1;
				pOutput->materialID = m_aSamples[m_countY * ix1 + iy1].material;
				maxLambda = output.lambda;
			}
		}
	}
	else
	{




		{
			dmTriangle triangle;
			triangle.m_p[0] = dmVecSet(x1, y1, z11);
			triangle.m_p[1] = dmVecSet(x2, y1, z21);
			triangle.m_p[2] = dmVecSet(x1, y2, z12);

			dmRayCastOutput output;
			bool hit = triangle.RayCast(&output, p1, p2, maxLambda);
			if (hit)
			{
				haveHit = true;
				*pOutput = output;
				pOutput->triangleIndex = 2 * ((m_countY-1) * ix1 + iy1);
				pOutput->materialID = m_aSamples[m_countY * ix1 + iy1].material;
				maxLambda = output.lambda;
			}
		}

		{
			dmTriangle triangle;
			triangle.m_p[0] = dmVecSet(x2, y1, z21);
			triangle.m_p[1] = dmVecSet(x2, y2, z22);
			triangle.m_p[2] = dmVecSet(x1, y2, z12);

			dmRayCastOutput output;
			bool hit = triangle.RayCast(&output, p1, p2, maxLambda);
			if (hit)
			{
				haveHit = true;
				*pOutput = output;
				pOutput->triangleIndex = 2 * ((m_countY-1) * ix1 + iy1) + 1;
				pOutput->materialID = m_aSamples[m_countY * ix1 + iy1].material;
				maxLambda = output.lambda;
			}
		}
	}

	return haveHit;
}
