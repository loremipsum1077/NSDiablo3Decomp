#include "dmDistance.h"
#include "dmManifold.h"
#include "../../Include/dmStructures.h"
#include "../../Include/dmPolytope.h"
#include "../../Include/dmPolytopeData.h"

void dmTSManifold(dmManifold* pManifold, dmGJKCache* pCache,
                  const dmTriangle* pTriangleA, const dmSphere* pSphereB)
{
    pManifold->m_pointCount = 0;
    dm_vec4 normalA = pTriangleA->GetNormal();
    if (dmDot3_f32(pSphereB->m_p - pTriangleA->m_p[0], normalA) < 0.0f)
        return;

    dm_vec4* aVertices = const_cast<dm_vec4*>(pTriangleA->m_p);
    dm_vec4 aPlaneBuffer[2];
    dmSubEdge aSubEdges[6] = {
        {1, 0, 0, 2}, {-1, 1, 1, 5},
        {1, 1, 0, 4}, {-1, 2, 2, 1},
        {1, 2, 0, 0}, {-1, 0, 3, 3}
    };
    dm_uint8 aFaceSubEdges[2] = {0, 1};
    dmPolytope trianglePolytope;
    trianglePolytope.SetAsTriangle(aVertices, aPlaneBuffer, aSubEdges, aFaceSubEdges);
    dmPSManifold(pManifold, pCache, &trianglePolytope, pSphereB);
    for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i)
        pManifold->m_points[i].id.fp.triangleIndex = pTriangleA->m_triangleIndex;
}
