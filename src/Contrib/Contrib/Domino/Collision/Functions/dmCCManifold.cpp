#include "dmDistance.h"
#include "dmManifold.h"
#include "../../Include/dmStructures.h"
#include "../../Include/dmShapes.h"












void dmCCManifold(dmManifold* pManifold, const dmCapsule* pCapsuleA, const dmCapsule* pCapsuleB)
{
    dm_vec4 pointA, pointB;
    dmCapsule segmentA, segmentB;
    segmentA.m_p1 = pCapsuleA->m_p1;
    segmentA.m_p2 = pCapsuleA->m_p2;
    segmentA.m_radius = 0.0f;  // Inference: lines 23/26 have no row; the stp w22, wzr type/radius pairs (197:23) and the 400-byte size need a radius store after the endpoints.
    segmentB.m_p1 = pCapsuleB->m_p1;
    segmentB.m_p2 = pCapsuleB->m_p2;
    segmentB.m_radius = 0.0f;

    dm_vec4 distance = dmDistance(&pointA, &pointB, &segmentA, &segmentB);
    dm_vec4 radiusA = dmV4(pCapsuleA->m_radius);
    dm_vec4 radiusB = dmV4(pCapsuleB->m_radius);
    dm_vec4 totalRadius = radiusA + radiusB;

    if (dmGT(distance, totalRadius))
    {
        pManifold->m_pointCount = 0;
        return;
    }

    pManifold->m_pointCount = 1;
    pManifold->m_points[0].id.key = 0;

    if (dmLT(distance, dm_vec4_epsilon))
    {

        pManifold->m_normal = dm_vec4_unitX;
    }
    else
    {
        pManifold->m_normal = dmNormalize3(pointB - pointA);
    }

    dm_vec4 point = pointA + (distance + radiusA - radiusB) * dm_vec4_oneHalf * pManifold->m_normal;
    pManifold->m_points[0].point = point;
    pManifold->m_points[0].separation = dmF32(distance - totalRadius);
}
