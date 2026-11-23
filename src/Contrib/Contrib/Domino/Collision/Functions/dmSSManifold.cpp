#include "../../Include/dmShapes.h"
#include "../../Include/dmStructures.h"
#include "../../Include/dmCollision.h"
#include "dmManifold.h"












void dmSSManifold(dmManifold* pManifold, const dmSphere* pSphereA, const dmSphere* pSphereB)
{
    dm_vec4 point1 = pSphereA->m_p;
    dm_vec4 point2 = pSphereB->m_p;

    dm_vec4 d = point2 - point1;
    dm_vec4 dd = dmDot3(d, d);
    dm_vec4 radiusA = dmV4(pSphereA->m_radius);
    dm_vec4 radiusB = dmV4(pSphereB->m_radius);
    dm_vec4 totalRadius = radiusA + radiusB;

    if (dmGT(dd, dmSplatX(totalRadius * totalRadius))) {

        pManifold->m_pointCount = 0;
        return;
    }

    pManifold->m_pointCount = 1;
    pManifold->m_points[0].id.key = 0;

    if (dmLT(dd, dm_vec4_epsilon * dm_vec4_epsilon)) {

        pManifold->m_normal = dm_vec4_unitY;
        pManifold->m_points[0].point = point1;
        pManifold->m_points[0].separation = -dmF32(totalRadius);
        return;
    }

    dm_vec4 distance = dmSqrt(dd);
    dm_vec4 normal = dmNormalize3(d);
    dm_vec4 point = (point1 + point2 + normal * (radiusA - radiusB)) * dm_vec4_oneHalf;

    pManifold->m_normal = normal;
    pManifold->m_points[0].point = point;
    pManifold->m_points[0].separation = dmF32(distance - totalRadius);
}
