#include "ClothUpdate.h"
#include "../Sigma/Math/MathGeneral.h"
#include <float.h>
#include <math.h>





















static void sApplyWind(
    const ClothUpdateInputParams & tInputParams,
    ClothUpdateInputOutputParams & tInputOutputParams,
    float flDeltaTime)
{
    if (tInputParams.flWindFactor <= 0.0f)
        return;








    // DWARF constant locations: fudge 0x40000000; density 0x3b1f40a3.
    const float cflFudgeFactor = 2.0f;
    const float cflAirDensity = 0.00243f;



    for (int32 i = 0; i < tInputParams.nFaceCount; ++i)
    {
        const ClothFace * ptFace = &tInputParams.ptFaces[i];
        ClothVertex * pv1 = &tInputOutputParams.ptVertices[ptFace->nVertex1];
        ClothVertex * pv2 = &tInputOutputParams.ptVertices[ptFace->nVertex2];
        ClothVertex * pv3 = &tInputOutputParams.ptVertices[ptFace->nVertex3];


        Vector3D wvRelativeWind = tInputParams.wvWind - 0.333f * (pv1->wv + pv2->wv + pv3->wv);
        Vector3D wvWindDir = wvRelativeWind;
        float flWindSpeed = wvWindDir.Normalize();
        if (flWindSpeed < FLT_EPSILON || flWindSpeed >= FLT_MAX)
            continue;



        float flPressureFactor = flWindSpeed * flWindSpeed * (tInputParams.flWindFactor * (0.5f * cflAirDensity) * cflFudgeFactor);
        Vector3D wvWindImpulse = wvWindDir * (flPressureFactor * flDeltaTime);

        float flAbsDot = Math::Abs(Vector3D::DotProduct(wvWindDir, ptFace->wvNormal));

        Vector3D wvImpulse = (0.333f * ptFace->flRestArea * flAbsDot) * wvWindImpulse;

        pv1->wv += pv1->flInvMass * wvImpulse;
        pv2->wv += pv2->flInvMass * wvImpulse;
        pv3->wv += pv3->flInvMass * wvImpulse;
    }
}



template <int fContiguous, int fFast>
static void sDisplaceDynVertices(
    ClothIsland * ptIsland,
    ClothVertex * ptVertices,
    const PRSTransform & displacement,
    float flFraction,
    int32 nIslandIndex)
{


    for (int32 j = ptIsland->m_nMinVertexIndex; j <= ptIsland->m_nMaxVertexIndex; ++j)
    {
        ClothVertex * pv = &ptVertices[j];
        if (!fContiguous && pv->nIslandIndex != nIslandIndex)
            continue;

        Vector3D wpNew = displacement * pv->wp;
        pv->wp += flFraction * (wpNew - pv->wp);
        if (fFast)
            pv->wv.Set(0.0f, 0.0f, 0.0f);
    }
}








BOOL sApplyStaple(
    const ClothUpdateInputParams & tInputParams,
    ClothUpdateInputOutputParams & tInputOutputParams)
{

    BOOL fFast = FALSE;
    BOOL fRelax = tInputParams.fRelax;

    for (int32 i = 0; i < tInputParams.nIslandCount; ++i)
    {
        ClothIsland * ptIsland = &tInputParams.ptIslands[i];
        if (ptIsland->m_nStapleCount == 0)
            continue;





        float flFraction = tInputParams.flVelocityFromParentFraction;





        if (fRelax || ptIsland->m_maxDispacementSqr > 1.0f)
        {
            flFraction = 1.0f;
            fFast = TRUE;
        }


        PRSTransform displacement = ptIsland->m_transform2 * ptIsland->m_transform1.Inverse();


        if (ptIsland->m_fContiguous)
        {
            if (fFast)
                sDisplaceDynVertices<1, 1>(ptIsland, tInputOutputParams.ptVertices, displacement, flFraction, i);
            else
                sDisplaceDynVertices<1, 0>(ptIsland, tInputOutputParams.ptVertices, displacement, flFraction, i);
        }
        else
        {
            if (fFast)
                sDisplaceDynVertices<0, 1>(ptIsland, tInputOutputParams.ptVertices, displacement, flFraction, i);
            else
                sDisplaceDynVertices<0, 0>(ptIsland, tInputOutputParams.ptVertices, displacement, flFraction, i);
        }
        ptIsland->m_transform1 = ptIsland->m_transform2;
    }
    return fFast;
}
















void ClothUpdateBoundingSphere(
    const ClothUpdateInputParams & tInputParams,
    ClothUpdateInputOutputParams & tInputOutputParams)
{

    Vector3D wpMin(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3D wpMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int32 i = 0; i < tInputParams.nVertexCount; ++i)
    {
        const ClothVertex * pv = &tInputOutputParams.ptVertices[i];
        wpMin = Vector3D::Min(wpMin, pv->wp);
        wpMax = Vector3D::Max(wpMax, pv->wp);
    }
    tInputOutputParams.wsBoundingSphere.wpCenter = 0.5f * (wpMin + wpMax);




    float flRadiusSquared = 0.0f;

    for (int32 i = 0; i < tInputParams.nVertexCount; ++i)
    {
        const ClothVertex * pv = &tInputOutputParams.ptVertices[i];
        float flDistanceSquared = Vector3D::DistanceSquared(pv->wp, tInputOutputParams.wsBoundingSphere.wpCenter);
        flRadiusSquared = Math::Max(flRadiusSquared, flDistanceSquared);
    }
    tInputOutputParams.wsBoundingSphere.wdRadius = Math::Sqrt(flRadiusSquared);
}





void ClothUpdateFaceNormals(
    const ClothUpdateInputParams & tInputParams,
    ClothUpdateInputOutputParams & tInputOutputParams)
{
    for (int32 i = tInputParams.nStapleCount; i < tInputParams.nVertexCount; ++i)
        tInputOutputParams.ptVertices[i].wvNormal.Set(0.0f, 0.0f, 0.0f);



    for (int32 i = 0; i < tInputParams.nFaceCount; ++i)
    {
        ClothFace * ptFace = &tInputParams.ptFaces[i];
        ClothVertex * pv1 = &tInputOutputParams.ptVertices[ptFace->nVertex1];
        ClothVertex * pv2 = &tInputOutputParams.ptVertices[ptFace->nVertex2];
        ClothVertex * pv3 = &tInputOutputParams.ptVertices[ptFace->nVertex3];

        Vector3D wvEdge1 = pv2->wp - pv1->wp;
        Vector3D wvEdge2 = pv3->wp - pv1->wp;
        ptFace->wvNormal = Vector3D::CrossProduct(wvEdge1, wvEdge2);
        ptFace->wvNormal.Normalize();
        if (ptFace->nVertex1 >= tInputParams.nStapleCount)
            pv1->wvNormal += ptFace->wvNormal;
        if (ptFace->nVertex2 >= tInputParams.nStapleCount)
            pv2->wvNormal += ptFace->wvNormal;
        if (ptFace->nVertex3 >= tInputParams.nStapleCount)
            pv3->wvNormal += ptFace->wvNormal;
    }









    for (int32 i = 0; i < tInputParams.nVertexCount; ++i)
        tInputOutputParams.ptVertices[i].wvNormal.Normalize();
}






static float sInvSqrt(
    float x)
{
    float xhalf = 0.5f * x;
    int32 i = *(int32 *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}




struct PreCalcCapsule
{
    Vector3D wp1;
    Vector3D wvAxis;
    float flRadius;
    float flLength;
};

static void sApplyCapsuleConstraints(
    int32 nCapsuleCount,
    const Capsule * aCapsules,
    int32 nStapleCount,
    int32 nVertexCount,
    ClothVertex * ptVertices)
{
    PreCalcCapsule aPreCalcCapsules[CLOTH_MAX_COLLISION_CAPSULES];
    // No early return on nCapsuleCount: the original has no row at 285/286; its only count test is the 288 loop guard.


    for (int32 i = 0; i < nCapsuleCount; ++i)
    {
        const Capsule * ptCapsule = &aCapsules[i];
        PreCalcCapsule * ptPreCalcCapsule = &aPreCalcCapsules[i];
        ptPreCalcCapsule->wp1 = ptCapsule->wp1;
        Vector3D wvAxis = ptCapsule->wp2 - ptCapsule->wp1;
        ptPreCalcCapsule->wvAxis = wvAxis;
        ptPreCalcCapsule->flLength = ptPreCalcCapsule->wvAxis.Normalize();
        ptPreCalcCapsule->flRadius = ptCapsule->flRadius;
    }


    for (int32 j = nStapleCount; j < nVertexCount; ++j)
    {
        ClothVertex * pv = &ptVertices[j];
        Vector3D wp = pv->wp;

        const ClothVertex * ptParent = &ptVertices[pv->nParentIndex];
        const Vector3D & wpParent = ptParent->wp;

        for (int32 i = 0; i < nCapsuleCount; ++i)
        {
            const PreCalcCapsule * ptPreCalcCapsule = &aPreCalcCapsules[i];





            const Vector3D & wpEndPoint = ptPreCalcCapsule->wp1;
            const Vector3D & wvAxis = ptPreCalcCapsule->wvAxis;
            float flLength = ptPreCalcCapsule->flLength;
            float flRadius = ptPreCalcCapsule->flRadius;

            Vector3D wvOffset = wpParent - wpEndPoint;
            float fld = Vector3D::DotProduct(wvOffset, wvAxis);
            fld = Math::Pin(fld, 0.0f, flLength);
            Vector3D wpCenter = wpEndPoint + fld * wvAxis;

            Vector3D wvRadialAxis = wpParent - wpCenter;

            wvRadialAxis *= sInvSqrt(wvRadialAxis.MagnitudeSquared());

            float flDist = Vector3D::DotProduct(wp - wpCenter, wvRadialAxis) - flRadius;
            if (flDist < 0.0f)
            {
                wp -= flDist * wvRadialAxis;
            }
        }

        pv->wp = wp;
    }
}





static void sApplyConstraints(
    const ClothUpdateInputParams & tInputParams,
    ClothUpdateInputOutputParams & tInputOutputParams,
    float flDeltaTime)
{
    const float cflKs = Math::Pin(tInputParams.flStretchingStiffness, 0.0f, 1.0f);
    const float cflKsh = Math::Pin(tInputParams.flHorizontalStiffness, 0.0f, 1.0f);
    const float cflKb = Math::Pin(tInputParams.flBendingStiffness, 0.0f, 1.0f);


    for (int32 i = 0; i < tInputParams.nDistanceConstraintCount; ++i)
    {
        const ClothConstraint * pc = &tInputParams.ptDistanceConstraints[i];

        ClothVertex * pv1 = &tInputOutputParams.ptVertices[pc->nVertex1];
        ClothVertex * pv2 = &tInputOutputParams.ptVertices[pc->nVertex2];

        Vector3D wvDelta = pv2->wp - pv1->wp;



        float flrr = pc->flRestLengthSquared;
        float flApproxSqrt = (-2.0f * flrr) / (flrr + Vector3D::DotProduct(wvDelta, wvDelta)) + 1.0f;


        const float cflK = cflKsh * pc->flHorizontal + cflKs * (1.0f - pc->flHorizontal);
        float flDeltaScale = cflK * flApproxSqrt * flDeltaTime * 60.0f;
        float flScale1 = pc->flScale1 * flDeltaScale;
        float flScale2 = pc->flScale2 * flDeltaScale;
        // Original rows 375-377 / 379-381: one statement per component (no inlined Vector3D operators in this block).
        pv1->wp.x += wvDelta.x * flScale1;
        pv1->wp.y += wvDelta.y * flScale1;
        pv1->wp.z += wvDelta.z * flScale1;

        pv2->wp.x -= wvDelta.x * flScale2;
        pv2->wp.y -= wvDelta.y * flScale2;
        pv2->wp.z -= wvDelta.z * flScale2;
    }


    if (cflKb > 0.0f)
    {
        for (int32 i = 0; i < tInputParams.nBendingConstraintCount; ++i)
        {
            const ClothConstraint * pc = &tInputParams.ptBendingConstraints[i];

            ClothVertex * pv1 = &tInputOutputParams.ptVertices[pc->nVertex1];
            ClothVertex * pv2 = &tInputOutputParams.ptVertices[pc->nVertex2];

            Vector3D wvDelta = pv2->wp - pv1->wp;



            float flrr = pc->flRestLengthSquared;
            float flApproxSqrt = (-2.0f * flrr) / (flrr + Vector3D::DotProduct(wvDelta, wvDelta)) + 1.0f;


            if (flApproxSqrt < 0.0f)
            {
                wvDelta *= cflKb * flApproxSqrt * flDeltaTime * 60.0f;
                Vector3D wvDelta1 = pc->flScale1 * wvDelta;
                Vector3D wvDelta2 = -pc->flScale2 * wvDelta;
                pv1->wp += wvDelta1;
                pv2->wp += wvDelta2;
            }
        }
    }


    for (int32 i = 0; i < tInputParams.nDistanceConstraintCount; ++i)
    {
        const ClothConstraint * pc = &tInputParams.ptDistanceConstraints[i];

        ClothVertex * pv1 = &tInputOutputParams.ptVertices[pc->nVertex1];
        ClothVertex * pv2 = &tInputOutputParams.ptVertices[pc->nVertex2];

        Vector3D wvDelta = pv2->wp - pv1->wp;



        float flrr = pc->flRestLengthSquared;
        float flApproxSqrt = (-2.0f * flrr) / (flrr + Vector3D::DotProduct(wvDelta, wvDelta)) + 1.0f;


        const float cflK = cflKsh * pc->flHorizontal + cflKs * (1.0f - pc->flHorizontal);
        wvDelta *= cflK * flApproxSqrt * flDeltaTime * 60.0f;

        Vector3D wvDelta1 = pc->flScale1 * wvDelta;
        Vector3D wvDelta2 = -pc->flScale2 * wvDelta;
        pv1->wp += wvDelta1;
        pv2->wp += wvDelta2;
    }


    sApplyCapsuleConstraints(tInputParams.nCapsuleCount, tInputParams.aCapsules, tInputParams.nStapleCount, tInputParams.nVertexCount, tInputOutputParams.ptVertices);


    for (int32 i = 0; i < tInputParams.nPlaneCount; ++i)
    {
        const Plane plane = tInputParams.aPlanes[i];

        for (int32 j = tInputParams.nStapleCount; j < tInputParams.nVertexCount; ++j)
        {
            ClothVertex * pv = &tInputOutputParams.ptVertices[j];

            float flSeparation = plane.wdD + Vector3D::DotProduct(pv->wp, plane.wvNormal);
            if (flSeparation < 0.01f)
            {
                pv->wp += (0.01f - flSeparation) * plane.wvNormal;
                pv->wvCollisionNormal = plane.wvNormal;
                pv->bCollided = TRUE;
            }
        }
    }
}



// NON_MATCHING: 2972 bytes both, 8 insns differ: scheduling only (sApplyWind 63 flPressureFactor product order; pv2->wp.z load hoisted above the x/y stp at 381). Distance-loop updates are per component on 375-381 (original rows).
void ClothUpdate(
    const ClothUpdateInputParams & tInputParams,
    ClothUpdateInputOutputParams & tInputOutputParams,
    float flDeltaTime)
{
    BOOL fTeleport = sApplyStaple(tInputParams, tInputOutputParams);
    float flInvDeltaT = 1.0f / flDeltaTime;

    Vector3D wvBaseAcc(0.0f, 0.0f, -43.2f);
    wvBaseAcc.z += tInputParams.flAirBuoyancy;




    if (!fTeleport)
    {
        sApplyWind(tInputParams, tInputOutputParams, flDeltaTime);
    }

    const float cflDampingFactor = expf(tInputParams.flDamping * (-1.0f / 60.0f));
    const float cflFrictionFactor = expf(tInputParams.flFriction * (-1.0f / 60.0f));

    for (int32 i = tInputParams.nStapleCount; i < tInputParams.nVertexCount; ++i)
    {
        ClothVertex * pv = &tInputOutputParams.ptVertices[i];


        pv->wp0 = pv->wp;


        pv->wv *= cflDampingFactor;


        pv->wv += flDeltaTime * wvBaseAcc;
        pv->wp += pv->wv * flDeltaTime;

        pv->bCollided = FALSE;
    }






    sApplyConstraints(tInputParams, tInputOutputParams, flDeltaTime);


    for (int32 i = tInputParams.nStapleCount; i < tInputParams.nVertexCount; ++i)
    {
        ClothVertex * pv = &tInputOutputParams.ptVertices[i];


        pv->wv = flInvDeltaT * (pv->wp - pv->wp0);

        if (pv->bCollided)
        {
            float flDot = Vector3D::DotProduct(pv->wv, pv->wvCollisionNormal);
            Vector3D wvNormalVelocity = flDot * pv->wvCollisionNormal;
            Vector3D wvTangentVelocity = pv->wv - wvNormalVelocity;
            pv->wv = wvNormalVelocity + cflFrictionFactor * wvTangentVelocity;
        }
    }


    Vector3D wvRelativeWind = tInputParams.wvWind - tInputParams.wvActorVelocity;
    tInputOutputParams.flAverageVelocity = wvRelativeWind.Magnitude();
    ClothUpdateBoundingSphere(tInputParams, tInputOutputParams);
    ClothUpdateFaceNormals(tInputParams, tInputOutputParams);
}








