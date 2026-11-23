#include "dmTimeOfImpact.h"

// Original diagnostic counters, owned by dmTimeOfImpact.cpp.
dm_int32 dm_toiCallCount;
dm_int32 dm_toiIterCount;
dm_int32 dm_toiIterMax;
dm_int32 dm_toiRootIterCount;
dm_int32 dm_toiRootIterMax;

void dmTimeOfImpact(dmTOIOutput * pOutput, const dmTOIInput * pInput)
{
    ++dm_toiCallCount;
    pOutput->iterationCount = 0;
    pOutput->maxRootIterationCount = 0;
    dmSweep sweepA = pInput->sweepA;
    dmSweep sweepB = pInput->sweepB;
    dm_vec4 origin = sweepA.m_center0;
    sweepA.m_center0 -= origin;
    sweepA.m_center -= origin;
    sweepB.m_center0 -= origin;
    sweepB.m_center -= origin;

    const dmGJKProxy * pProxyA = &pInput->proxyA;
    const dmGJKProxy * pProxyB = &pInput->proxyB;
    const dm_vec4 radius = pProxyA->GetRadius() + pProxyB->GetRadius();
    // Constants recovered from the original instructions (world-space tolerances).
    const dm_vec4 target = dmMax(dmVecSet(0.0164041984f), radius - dmVecSet(0.049212597f));
    const dm_vec4 tolerance = dmVecSet(0.0041010496f);
    const dm_vec4 rootTolerance = dmVecSet(0.00041010496f);
    dm_vec4 t1 = dmVecSet(0.0f);
    dmDistanceInput distanceInput;
    distanceInput.cache = pInput->cache;
    distanceInput.useRadii = false;
    distanceInput.proxyA = *pProxyA;
    distanceInput.proxyB = *pProxyB;
    dm_int32 distanceIter = 0;
    const dm_int32 maxDistanceIter = 20;
    bool useFastEdges = true;
    dmDistanceOutput distanceOutput;
    for (;;)
    {
        ++dm_toiIterCount;
        ++pOutput->iterationCount;
        dmTransform xfA = sweepA.GetTransform(t1);
        dmTransform xfB = sweepB.GetTransform(t1);
        distanceInput.transformA = xfA;
        distanceInput.transformB = xfB;
        dmDistance(&distanceOutput, &distanceInput);
        distanceInput.cache = distanceOutput.cache;
        pOutput->cache = distanceOutput.cache;
        pOutput->pointA = dmAdd(origin, distanceOutput.pointA);
        pOutput->pointB = dmAdd(origin, distanceOutput.pointB);
        if (dmEQZ(distanceOutput.distance) || distanceOutput.cache.GetCount() == 4)
        {
            pOutput->exitCode = dm_toiOverlap;
            pOutput->eta = t1;
            break;
        }
        if (dmLT(distanceOutput.distance, target + tolerance))
        {
            pOutput->exitCode = dm_toiHit;
            pOutput->eta = t1;
            break;
        }
        dmHomingFunction fcn;
        fcn.Initialize(&distanceOutput, pProxyA, sweepA, pProxyB, sweepB, useFastEdges, t1);
        bool done = false;
        dm_vec4 t2 = dm_vec4_one;
        dm_int32 deepestPointIter = 0;
        useFastEdges = true;
        for (;;)
        {
            dm_int32 indexA, indexB;
            dm_vec4 s2;
            if (!fcn.FindMinSeparation(&s2, &indexA, &indexB, t2))
            {
                useFastEdges = false;
                break;
            }
            if (dmGT(s2, target + tolerance))
            {
                pOutput->exitCode = dm_toiMiss;
                pOutput->eta = dm_vec4_one;
                done = true;
                break;
            }
            if (dmGT(s2, target - tolerance))
            {
                t1 = t2;
                break;
            }
            dm_vec4 s1;
            if (!fcn.Evaluate(&s1, indexA, indexB, t1))
            {
                useFastEdges = false;
                break;
            }
            if (dmLT(s1, target - tolerance))
            {
                pOutput->exitCode = dm_toiOverlap;
                pOutput->eta = t1;
                done = true;
                break;
            }
            if (dmLE(s1, target + tolerance))
            {
                pOutput->exitCode = dm_toiHit;
                pOutput->eta = t1;
                done = true;
                break;
            }
            dm_int32 rootIterCount = 0;
            dm_vec4 a1 = t1, a2 = t2;
            for (;;)
            {
                dm_vec4 t;
                if (rootIterCount & 1)
                    t = a1 + (target - s1) * (a2 - a1) / (s2 - s1);
                else
                    t = dm_vec4_oneHalf * (a1 + a2);
                dm_vec4 s;
                bool status = fcn.Evaluate(&s, indexA, indexB, t);
                if (!status)
                {
                    useFastEdges = false;
                    break;
                }
                if (dmLT(dmAbs(s - target), rootTolerance))
                {
                    t2 = t;
                    break;
                }
                if (dmGT(s, target))
                {
                    a1 = t;
                    s1 = s;
                }
                else
                {
                    a2 = t;
                    s2 = s;
                }
                ++rootIterCount;
                if (rootIterCount == 50) break;
            }
            pOutput->maxRootIterationCount = dmMax(pOutput->maxRootIterationCount, rootIterCount);
            dm_toiRootIterMax = dmMax(dm_toiRootIterMax, rootIterCount);
            if (!useFastEdges) break;
            ++deepestPointIter;
            if (deepestPointIter == 32) break;
        }
        if (!useFastEdges) continue;
        ++distanceIter;
        if (done) break;
        if (distanceIter == maxDistanceIter)
        {
            pOutput->exitCode = dm_toiHit;
            pOutput->eta = t1;
            break;
        }
        useFastEdges = true;
    }
    dm_toiIterMax = dmMax(dm_toiIterMax, distanceIter);
}
