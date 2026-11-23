#include <stdio.h>
#include "dmWeldJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"

#line 71
void dmWeldJoint::Create(const dmWeldJointDef* pDef) {
    m_localFrameA = pDef->m_localFrameA;
    m_localFrameB = pDef->m_localFrameB;
    m_angularImpulse = vdupq_n_f32(0.0f);
    m_pivotImpulse = vdupq_n_f32(0.0f);
    m_angularFrequencyHz = pDef->m_angularFrequencyHz;
    m_angularDampingRatio = pDef->m_angularDampingRatio;
    m_linearFrequencyHz = pDef->m_linearFrequencyHz;
    m_linearDampingRatio = pDef->m_linearDampingRatio;
}

#line 86
void dmWeldJoint::InitVelocityConstraints(const dmBody* pBodyA, const dmBody* pBodyB, const dmJoint* pBase, const dmSolverData* pData)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dmTransform xfA = pBodyA->m_transform;
    dmTransform xfB = pBodyB->m_transform;
    m_localCenterA = pBodyA->m_sweep.m_localCenter;
    m_localCenterB = pBodyB->m_sweep.m_localCenter;

    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];

    dm_vec4 h = pData->step.dt;


    {
        dmMtx K = invIA + invIB;
        m_angularMass = dmInvertSym33(K);

        if (m_angularFrequencyHz > 0.0f)
        {
            dm_vec4 qA = pData->aPositions[islandIndexA].q;
            dm_vec4 qB = pData->aPositions[islandIndexB].q;

            dm_vec4 qFA = dmMulQQ(qA, m_localFrameA.q);
            dm_vec4 qFB = dmMulQQ(qB, m_localFrameB.q);






            dm_vec4 qRel = dmMulQQ(dmConjugate(qFA), qFB);
            dm_vec4 qEye = dm_quat_identity;
            dm_vec4 qSign = dmDot4(qEye, qRel);
            qRel = dmSelect_LT(qSign, dmVecSet(0.0f), -qRel, qRel);

            dm_vec4 omegaRel = dm_vec4_two * dmMulQQ(qEye - qRel, dmConjugate(qRel));

            dm_vec4 C = dmRotate(qFA, omegaRel);


            dm_vec4 omega = dmV4(6.28318530717958647692f * m_angularFrequencyHz);


            dm_vec4 mass = dmVecSet(vgetq_lane_f32(m_angularMass.rx, 0), vgetq_lane_f32(m_angularMass.ry, 1), vgetq_lane_f32(m_angularMass.rz, 2));


            dm_vec4 d = (2.0f * m_angularDampingRatio * omega) * mass;


            dm_vec4 k = omega * omega * mass;


            m_angularGamma = h * (d + h * k);
            m_angularGamma = dmSafeReciprocal(m_angularGamma);

            dm_vec4 beta = h * m_angularGamma * k;
            m_angularBias = -(beta * C);

            K.rx += m_angularGamma * dm_vec4_unitX;
            K.ry += m_angularGamma * dm_vec4_unitY;
            K.rz += m_angularGamma * dm_vec4_unitZ;

            m_angularMass = dmInvertSym33(K);
        }
        else
        {
            m_angularBias = vdupq_n_f32(0.0f);
            m_angularGamma = vdupq_n_f32(0.0f);
        }
    }



    {
        dm_vec4 rA = dmRotate(xfA.q, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmRotate(xfB.q, m_localFrameB.p - m_localCenterB);


        m_Ja = dmMtxSetSkew(-rA);
        m_Jb = dmMtxSetSkew(-rB);

        dm_vec4 sumInvMass = dmV4(pBase->m_invMassA + pBase->m_invMassB);

        dmMtx K = -1.0f * (dmMul33(m_Ja, dmMul33(invIA, m_Ja)) + dmMul33(m_Jb, dmMul33(invIB, m_Jb)));
        K.rx += sumInvMass * dm_vec4_unitX;
        K.ry += sumInvMass * dm_vec4_unitY;
        K.rz += sumInvMass * dm_vec4_unitZ;

        m_pivotMass = dmInvertSym33(K);

        if (m_linearFrequencyHz > 0.0f)
        {
            dm_vec4 xA = pData->aPositions[islandIndexA].x;
            dm_vec4 xB = pData->aPositions[islandIndexB].x;

            dm_vec4 C = (xB - xA) + (rB - rA);


            dm_vec4 omega = dmV4(6.28318530717958647692f * m_linearFrequencyHz);


            dm_vec4 mass = dmVecSet(vgetq_lane_f32(m_pivotMass.rx, 0), vgetq_lane_f32(m_pivotMass.ry, 1), vgetq_lane_f32(m_pivotMass.rz, 2));


            dm_vec4 d = (2.0f * m_linearDampingRatio * omega) * mass;


            dm_vec4 k = omega * omega * mass;


            m_linearGamma = h * (d + h * k);
            m_linearGamma = dmSafeReciprocal(m_linearGamma);

            dm_vec4 beta = h * m_linearGamma * k;
            m_linearBias = beta * C;

            K.rx += m_linearGamma * dm_vec4_unitX;
            K.ry += m_linearGamma * dm_vec4_unitY;
            K.rz += m_linearGamma * dm_vec4_unitZ;

            m_pivotMass = dmInvertSym33(K);
        }
        else
        {
            m_linearBias = vdupq_n_f32(0.0f);
            m_linearGamma = vdupq_n_f32(0.0f);
        }
    }


    if (pData->step.useWarmStarting)
    {
        dm_vec4 ratio = pData->step.dtRatio;
        m_pivotImpulse *= ratio;
        m_angularImpulse *= ratio;

        dm_vec4 L1, L2;
        // L1 and L2 have no recorded value; the binary reloads the impulses on rows 230 and 233.


        pData->aVelocities[islandIndexA].v -= pBase->m_invMassA * m_pivotImpulse;
        pData->aVelocities[islandIndexA].w -= dmMul33(invIA, m_angularImpulse - dmMul33(m_Ja, m_pivotImpulse));

        pData->aVelocities[islandIndexB].v += pBase->m_invMassB * m_pivotImpulse;
        pData->aVelocities[islandIndexB].w += dmMul33(invIB, m_angularImpulse - dmMul33(m_Jb, m_pivotImpulse));
    }
    else
    {
        m_pivotImpulse = vdupq_n_f32(0.0f);
    }
}

#line 242
void dmWeldJoint::SolveVelocityConstraints(dmSolverData* pData, const dmJoint* pBase) {
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;
    dm_vec4 vA = pData->aVelocities[islandIndexA].v;
    dm_vec4 wA = pData->aVelocities[islandIndexA].w;
    dm_vec4 vB = pData->aVelocities[islandIndexB].v;
    dm_vec4 wB = pData->aVelocities[islandIndexB].w;
    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];
    {
        dm_vec4 Cdot = wB - wA;
        dm_vec4 impulse = -dmMul33(m_angularMass, Cdot + m_angularBias + m_angularGamma * m_angularImpulse);
        wA -= dmMul33(invIA, impulse);
        wB += dmMul33(invIB, impulse);
        m_angularImpulse += impulse;
    }
    {
        dm_vec4 Cdot = vB + dmMul33(m_Jb, wB) - vA - dmMul33(m_Ja, wA);
        dm_vec4 impulse = -dmMul33(m_pivotMass, Cdot + m_linearBias + m_linearGamma * m_pivotImpulse);
        vA -= pBase->m_invMassA * impulse;
        wA += dmMul33(invIA, dmMul33(m_Ja, impulse));
        vB += pBase->m_invMassB * impulse;
        wB -= dmMul33(invIB, dmMul33(m_Jb, impulse));
        m_pivotImpulse += impulse;
    }
    pData->aVelocities[islandIndexA].v = vA;
    pData->aVelocities[islandIndexA].w = wA;
    pData->aVelocities[islandIndexB].v = vB;
    pData->aVelocities[islandIndexB].w = wB;
}


#line 286
void dmWeldJoint::SolvePositionConstraints(dmSolverData* pData, const dmJoint* pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 xA = pData->aPositions[islandIndexA].x;
    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 xB = pData->aPositions[islandIndexB].x;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;


    if (m_angularFrequencyHz == 0.0f)
    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);

        dm_vec4 qFA = dmMulQQ(qA, m_localFrameA.q);
        dm_vec4 qFB = dmMulQQ(qB, m_localFrameB.q);






        dm_vec4 qRel = dmMulQQ(dmConjugate(qFA), qFB);
        dm_vec4 qEye = dm_quat_identity;
        dm_vec4 dot = dmDot4(qEye, qRel);
        qRel = dmSelect_LT(dot, dmVecSet(0.0f), -qRel, qRel);

        dm_vec4 omegaRel = dm_vec4_two * dmMulQQ(qEye - qRel, dmConjugate(qRel));

        dm_vec4 C = -dmRotate(qFA, omegaRel);

        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);

        dmMtx K = invIA + invIB;

        dm_vec4 impulse = dmSolveSym33(K, -C);

        dm_vec4 wA = dmMul33(invIA, impulse);
        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        dm_vec4 wB = dmMul33(invIB, impulse);
        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }


    if (m_linearFrequencyHz == 0.0f)
    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);


        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);


        dm_vec4 rA = dmMul33(Ra, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmMul33(Rb, m_localFrameB.p - m_localCenterB);

        dm_vec4 C = (xB - xA) + rB - rA;


        dmMtx Ja = dmMtxSetSkew(-rA);
        dmMtx Jb = dmMtxSetSkew(-rB);





        dmMtx Ba = dmMul33(invIA, Ja);
        Ba *= -1.0f;
        dmMtx Bb = dmMul33(invIB, Jb);
        Bb *= -1.0f;

        dm_vec4 sumInvMass = dmV4(pBase->m_invMassA + pBase->m_invMassB);
        dmMtx K = dmMul33(Ja, Ba) + dmMul33(Jb, Bb);
        K.rx += sumInvMass * dm_vec4_unitX;
        K.ry += sumInvMass * dm_vec4_unitY;
        K.rz += sumInvMass * dm_vec4_unitZ;

        dm_vec4 impulse = dmSolveSym33(K, -C);

        xA -= pBase->m_invMassA * impulse;
        dm_vec4 wA = dmMul33(Ba, impulse);
        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        xB += pBase->m_invMassB * impulse;
        dm_vec4 wB = dmMul33(Bb, impulse);
        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }

    pData->aPositions[islandIndexA].x = xA;
    pData->aPositions[islandIndexA].q = qA;
    pData->aPositions[islandIndexB].x = xB;
    pData->aPositions[islandIndexB].q = qB;
}
