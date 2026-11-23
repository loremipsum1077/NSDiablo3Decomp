#include "dmHomingFunction.h"
#include "dmDistance.h"

void dmSweep::Advance(dm_vec4Arg alpha)
{
    if (dmLT(m_alpha0, alpha) && dmGT(dmSub(dm_vec4_one, m_alpha0), dmVecSet(1.1920928955078125e-7f)))
    {
        dm_vec4 eta = dmDiv(dmSub(alpha, m_alpha0), dmSub(dm_vec4_one, m_alpha0));
        m_center0 = dmAdd(m_center0, dmMul(eta, dmSub(m_center, m_center0)));
        m_quaternion0 = dmQuatNLerp(m_quaternion0, m_quaternion, eta);
        m_alpha0 = alpha;
    }
}

// Counting shape from the original rows: countA/countB declared = 3 (DWARF 130/131), one || test (133 cols 15/26/59)
// then a nested && test (135 col 27); faceB negates the rotated/normalized axis (shared fneg before the store).
void dmHomingFunction::Initialize(const dmDistanceOutput * pDistance, const dmGJKProxy * pProxyA,
    const dmSweep & sweepA, const dmGJKProxy * pProxyB, const dmSweep & sweepB,
    bool useFastEdges, dm_vec4Arg t1)
{
    m_pProxyA = pProxyA;
    m_pProxyB = pProxyB;
    m_sweepA = sweepA;
    m_sweepB = sweepB;
    const dmGJKCache * pCache = &pDistance->cache;
    dm_int32 count = pCache->GetCount();
    if (count == 1)
    {
        m_type = e_points;
        m_axis = pDistance->normal;
        return;
    }
    dmTransform xfA = m_sweepA.GetTransform(t1);
    dmTransform xfB = m_sweepB.GetTransform(t1);
    if (count == 2)
    {
        if (pCache->indexBs[0] == pCache->indexBs[1])
        {
            m_type = e_faceA;
            m_localPoint = dmMulT(xfA, pDistance->pointA);
            m_axis = dmInvRotate(xfA.q, pDistance->normal);
        }
        else
        {
            m_type = e_faceB;
            m_localPoint = dmMulT(xfB, pDistance->pointB);
            m_axis = -dmInvRotate(xfB.q, pDistance->normal);
        }
    }
    else if (count == 3)
    {
        dm_int32 indexA1 = pCache->indexAs[0];
        dm_int32 indexA2 = pCache->indexAs[1];
        dm_int32 indexA3 = pCache->indexAs[2];
        dm_int32 indexB1 = pCache->indexBs[0];
        dm_int32 indexB2 = pCache->indexBs[1];
        dm_int32 indexB3 = pCache->indexBs[2];
        dm_int32 countA = 3;
        dm_int32 countB = 3;
        if (indexA1 == indexA2 || indexA1 == indexA3 || indexA2 == indexA3)
        {
            if (indexA1 == indexA2 && indexA2 == indexA3)
                countA = 1;
            else
            {
                countA = 2;
                if (indexA1 == indexA2) indexA2 = indexA3;
            }
        }
        if (indexB1 == indexB2 || indexB1 == indexB3 || indexB2 == indexB3)
        {
            if (indexB1 == indexB2 && indexB2 == indexB3)
                countB = 1;
            else
            {
                countB = 2;
                if (indexB1 == indexB2) indexB2 = indexB3;
            }
        }
        if (countA == 2 && countB == 2 && useFastEdges)
        {
            m_type = e_edges;
            dm_vec4 localPointA1 = pProxyA->GetVertex(indexA1);
            dm_vec4 localPointA2 = pProxyA->GetVertex(indexA2);
            dm_vec4 localPointB1 = pProxyB->GetVertex(indexB1);
            dm_vec4 localPointB2 = pProxyB->GetVertex(indexB2);
            m_localAxisA = localPointA2 - localPointA1;
            m_localAxisB = localPointB2 - localPointB1;
            m_axis = pDistance->normal;
            dm_vec4 axis = dmCross(dmRotate(xfA.q, m_localAxisA), dmRotate(xfB.q, m_localAxisB));
            if (!dmIsZeroSafe3(axis))
            {
                if (dmLTZ(dmDot3(axis, m_axis)))
                    m_localAxisB = -m_localAxisB;
                return;
            }
        }
        if (countA >= countB)
        {
            m_type = e_faceA;
            m_localPoint = dmMulT(xfA, pDistance->pointA);
            m_axis = dmInvRotate(xfA.q, pDistance->normal);
            m_axis = dmNormalize3(m_axis);
        }
        else
        {
            m_type = e_faceB;
            m_localPoint = dmMulT(xfB, pDistance->pointB);
            m_axis = dmInvRotate(xfB.q, pDistance->normal);
            m_axis = -dmNormalize3(m_axis);
        }
    }
}

bool dmHomingFunction::FindMinSeparation(dm_vec4 * pSeparation, dm_int32 * pIndexA, dm_int32 * pIndexB, dm_vec4Arg t)
{
    dmTransform xfA = m_sweepA.GetTransform(t);
    dmTransform xfB = m_sweepB.GetTransform(t);
    switch (m_type)
    {
    case e_points:
    {
        dm_vec4 axisA = dmInvRotate(xfA.q, m_axis);
        dm_vec4 axisB = dmInvRotate(xfB.q, dmNegate(m_axis));
        dm_vec4 localPointA, localPointB;
        *pIndexA = m_pProxyA->GetSupport(&localPointA, axisA);
        *pIndexB = m_pProxyB->GetSupport(&localPointB, axisB);
        dm_vec4 pointA = dmMul(xfA, localPointA);
        dm_vec4 pointB = dmMul(xfB, localPointB);
        *pSeparation = dmDot3(dmSub(pointB, pointA), m_axis);
        return true;
    }
    case e_faceA:
    {
        dm_vec4 normal = dmRotate(xfA.q, m_axis);
        dm_vec4 pointA = dmMul(xfA, m_localPoint);
        dm_vec4 axisB = dmInvRotate(xfB.q, dmNegate(normal));
        *pIndexA = -1;
        dm_vec4 localPointB;
        *pIndexB = m_pProxyB->GetSupport(&localPointB, axisB);
        dm_vec4 pointB = dmMul(xfB, localPointB);
        *pSeparation = dmDot3(dmSub(pointB, pointA), normal);
        return true;
    }
    case e_faceB:
    {
        dm_vec4 normal = dmRotate(xfB.q, m_axis);
        dm_vec4 pointB = dmMul(xfB, m_localPoint);
        dm_vec4 axisA = dmInvRotate(xfA.q, dmNegate(normal));
        *pIndexB = -1;
        dm_vec4 localPointA;
        *pIndexA = m_pProxyA->GetSupport(&localPointA, axisA);
        dm_vec4 pointA = dmMul(xfA, localPointA);
        *pSeparation = dmDot3(dmSub(pointA, pointB), normal);
        return true;
    }
    case e_edges:
    {
        dm_vec4 axis = dmCross(dmRotate(xfA.q, m_localAxisA), dmRotate(xfB.q, m_localAxisB));
        if (dmLEZ(dmDot3(axis, m_axis)) || dmIsZeroSafe3(axis)) break;
        axis = dmNormalize3(axis);
        dm_vec4 axisA = dmInvRotate(xfA.q, axis);
        dm_vec4 axisB = dmInvRotate(xfB.q, dmNegate(axis));
        dm_vec4 localPointA, localPointB;
        *pIndexA = m_pProxyA->GetSupport(&localPointA, axisA);
        *pIndexB = m_pProxyB->GetSupport(&localPointB, axisB);
        dm_vec4 pointA = dmMul(xfA, localPointA);
        dm_vec4 pointB = dmMul(xfB, localPointB);
        *pSeparation = dmDot3(dmSub(pointB, pointA), axis);
        return true;
    }
    }
    *pIndexA = -1;
    *pIndexB = -1;
    *pSeparation = dmVecSet(0.0f);
    return false;
}

bool dmHomingFunction::Evaluate(dm_vec4 * pSeparation, dm_int32 indexA, dm_int32 indexB, dm_vec4Arg t)
{
    dmTransform xfA = m_sweepA.GetTransform(t);
    dmTransform xfB = m_sweepB.GetTransform(t);
    switch (m_type)
    {
    case e_points:
    {
        dm_vec4 localPointA = m_pProxyA->GetVertex(indexA);
        dm_vec4 localPointB = m_pProxyB->GetVertex(indexB);
        dm_vec4 pointA = dmMul(xfA, localPointA);
        dm_vec4 pointB = dmMul(xfB, localPointB);
        *pSeparation = dmDot3(dmSub(pointB, pointA), m_axis);
        return true;
    }
    case e_faceA:
    {
        dm_vec4 normal = dmRotate(xfA.q, m_axis);
        dm_vec4 pointA = dmMul(xfA, m_localPoint);
        dm_vec4 localPointB = m_pProxyB->GetVertex(indexB);
        dm_vec4 pointB = dmMul(xfB, localPointB);
        *pSeparation = dmDot3(dmSub(pointB, pointA), normal);
        return true;
    }
    case e_faceB:
    {
        dm_vec4 normal = dmRotate(xfB.q, m_axis);
        dm_vec4 pointB = dmMul(xfB, m_localPoint);
        dm_vec4 localPointA = m_pProxyA->GetVertex(indexA);
        dm_vec4 pointA = dmMul(xfA, localPointA);
        *pSeparation = dmDot3(dmSub(pointA, pointB), normal);
        return true;
    }
    case e_edges:
    {
        dm_vec4 axis = dmCross(dmRotate(xfA.q, m_localAxisA), dmRotate(xfB.q, m_localAxisB));
        if (dmLEZ(dmDot3(axis, m_axis)) || dmIsZeroSafe3(axis)) break;
        axis = dmNormalize3(axis);
        dm_vec4 localPointA = m_pProxyA->GetVertex(indexA);
        dm_vec4 localPointB = m_pProxyB->GetVertex(indexB);
        dm_vec4 pointA = dmMul(xfA, localPointA);
        dm_vec4 pointB = dmMul(xfB, localPointB);
        *pSeparation = dmDot3(dmSub(pointB, pointA), axis);
        return true;
    }
    }
    *pSeparation = dmVecSet(0.0f);
    return false;
}
