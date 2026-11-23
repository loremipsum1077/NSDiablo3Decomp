#include "dmDistance.h"
#include "dmManifold.h"
#include "../../Include/dmStructures.h"
#include "../../Include/dmShapes.h"

void dmCSManifold(dmManifold* pManifold, const dmCapsule* pCapsuleA, const dmSphere* pSphereB)
{
    dmCapsule segmentA;
    segmentA.m_p1 = pCapsuleA->m_p1;
    segmentA.m_p2 = pCapsuleA->m_p2;
    dmSphere pointB;
    pointB.m_p = pSphereB->m_p;
    dm_vec4 pA, pB;
    dm_vec4 distance = dmDistance(&pA, &pB, &segmentA, &pointB);
    dm_vec4 radiusA = dmV4(pCapsuleA->m_radius);
    dm_vec4 radiusB = dmV4(pSphereB->m_radius);
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
        dm_vec4 axis = segmentA.m_p2 - segmentA.m_p1;
        axis = dmNormalize3(axis);
        dm_float32 ax = dmF32(axis);
        dm_float32 ay = dmGetY(axis);
        dm_float32 az = dmGetZ(axis);
        dm_vec4 normal;
        if (dmGT(dmSplatZ(axis), dm_vec4_oneHalf))
            normal = dmVecSet(az, 0.0f, -ax);
        else
            normal = dmVecSet(ay, -ax, 0.0f);
        pManifold->m_normal = dmNormalize3(normal);
        pManifold->m_points[0].point = pA;
        pManifold->m_points[0].separation = -dmF32(totalRadius);
        return;
    }
    dm_vec4 d = pB - pA;
    dm_vec4 normal = d / distance;
    dm_vec4 point = dm_vec4_oneHalf * (pA + pB + normal * (radiusA - radiusB));
    pManifold->m_normal = normal;
    pManifold->m_points[0].point = point;
    pManifold->m_points[0].separation = dmF32(distance - totalRadius);
}
