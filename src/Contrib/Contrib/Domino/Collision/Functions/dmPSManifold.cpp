#include "dmDistance.h"
#include "dmManifold.h"
#include "../../Include/dmStructures.h"
#include "../../Include/dmPolytope.h"

void dmPSManifold(dmManifold* pManifold, dmGJKCache* pCache,
                  const dmPolytope* pPolytope, const dmSphere* pSphere)
{
    dmDistanceInput input;
    input.cache = *pCache;
    input.proxyA.Set(pPolytope);
    input.proxyB.Set(pSphere);
    input.transformA = dmTransformIdentity();
    input.transformB = dmTransformIdentity();
    input.useRadii = false;
    dmDistanceOutput output;
    dmDistance(&output, &input);
    *pCache = output.cache;
    dm_vec4 totalRadius = dmV4(pPolytope->m_radius + pSphere->m_radius);
    if (dmGT(output.distance, totalRadius))
    {
        pManifold->m_pointCount = 0;
        return;
    }
    pManifold->m_pointCount = 1;
    pManifold->m_points[0].id.key = 0;
    dm_vec4 normal = output.pointB - output.pointA;
    // Original contact fallback threshold is one tenth of the linear slop.
    if (dmIsZeroSafe3(normal) || dmLT(output.distance, dmV4(0.0016404198f)))
    {
        dm_vec4 bestPlane = pPolytope->GetFace(0);
        dm_vec4 point = pSphere->m_p;
        dm_vec4 bestSeparation = dmDot3(bestPlane, point) - dmSplatW(bestPlane);
        for (dm_int32 i = 1; i < pPolytope->GetFaceCount(); ++i)
        {
            dm_vec4 plane = pPolytope->GetFace(i);
            dm_vec4 separation = dmDot3(plane, point) - dmSplatW(plane);
            if (dmGT(separation, bestSeparation))
            {
                bestPlane = plane;
                bestSeparation = separation;
            }
        }
        pManifold->m_normal = bestPlane;
        pManifold->m_points[0].point = point - totalRadius * bestPlane;
        pManifold->m_points[0].separation = dmF32(bestSeparation - totalRadius);
        return;
    }
    normal = dmNormalize3(normal);
    dm_vec4 point = pSphere->m_p - totalRadius * normal;
    pManifold->m_normal = normal;
    pManifold->m_points[0].point = point;
    pManifold->m_points[0].separation = dmF32(output.distance - totalRadius);
}
