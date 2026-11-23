#include "dmManifold.h"
#include "../../Include/dmStructures.h"

#include <float.h>

// Scalar form of dm_vec4_linearSlop (0.5 cm in feet). The name is invented;
// the DWARF keeps no macros.
#define dm_linearSlop (0.005f / 0.3048f)

// Reduces a contact cloud to at most four points: the deepest point, the point
// farthest from it, the point that maximizes the triangle area and the point
// that lies farthest outside that triangle.




void dmManifold_ReducePoints(dmManifold* pManifold, const dmManifoldPoint* pPoints, dm_int32 count)
{





	const dm_float32 k_tol = 4.0f * dm_linearSlop;
	const dm_float32 k_tolSqr = 16.0f * dm_linearSlop * dm_linearSlop;


	dm_float32 k_bias = 0.99f;


	dm_int32 bestIndex = -1;
	dm_float32 bestValue = FLT_MAX;
	for (dm_int32 i = 0; i < count; ++i)
	{
		dm_float32 value = pPoints[i].separation;

		if (k_bias * value < bestValue)
		{
			bestValue = value;
			bestIndex = i;
		}
	}



	pManifold->m_points[0] = pPoints[bestIndex];

	// Reset order inferred (no rows); mirrors the declarations at 31/32.
	bestIndex = -1;
	bestValue = -FLT_MAX;
	for (dm_int32 i = 0; i < count; ++i)
	{

		dm_vec4 r = pPoints[i].point - pManifold->m_points[0].point;

		dm_float32 value = dmDot3_f32(r, r);
		if (k_bias * value > bestValue)
		{
			bestValue = value;
			bestIndex = i;
		}
	}


	if (bestValue < k_tolSqr)
	{
		pManifold->m_pointCount = 1;
		return;
	}

	pManifold->m_points[1] = pPoints[bestIndex];


	dm_vec4 p1 = pManifold->m_points[0].point, p2 = pManifold->m_points[1].point;
	dm_vec4 e = p2 - p1;
	bestIndex = -1;
	bestValue = -FLT_MAX;
	for (dm_int32 i = 0; i < count; ++i)
	{
		dm_vec4 r = pPoints[i].point - p1;
		dm_float32 value = dmDot3_f32(dmCross(e, r), pManifold->m_normal);
		dm_float32 absValue = dmAbs(value);
		if (k_bias * absValue > bestValue)
		{
			bestValue = absValue;
			bestIndex = i;
		}
	}


	if (bestValue < k_tolSqr)
	{
		pManifold->m_pointCount = 2;
		return;
	}

	pManifold->m_points[2] = pPoints[bestIndex];

	// 101: the p3 - p1 subtraction sits at column 78, which fits this spelling.
	dm_vec4 p3 = pManifold->m_points[2].point;
	dm_float32 signedArea = dmDot3_f32(pManifold->m_normal, dmCross(p2 - p1, p3 - p1));
	dm_float32 invArea = 1.0f / signedArea;
	bestIndex = -1;
	bestValue = FLT_MAX;
	for (dm_int32 i = 0; i < count; ++i)
	{
		dm_vec4 p = pPoints[i].point;
		// 111-113: the p - x subtractions sit at column 79, which fits this spelling.


		dm_float32 u = invArea * dmDot3_f32(pManifold->m_normal, dmCross(p3 - p2, p - p2));
		dm_float32 v = invArea * dmDot3_f32(pManifold->m_normal, dmCross(p1 - p3, p - p3));
		dm_float32 w = invArea * dmDot3_f32(pManifold->m_normal, dmCross(p2 - p1, p - p1));
		dm_float32 value = dmMin(u, v, w);

		if (k_bias * value < bestValue)
		{
			bestValue = value;
			bestIndex = i;
		}
	}


	// 125: dmAbs is inlined here (DWARF) and the multiply sits at column 36.
	dm_float32 area = dmAbs(signedArea*bestValue);
	if (bestValue > 0.0f || area < k_tolSqr)
	{
		pManifold->m_pointCount = 3;
		return;
	}

	pManifold->m_points[3] = pPoints[bestIndex];
	pManifold->m_pointCount = 4;
}
