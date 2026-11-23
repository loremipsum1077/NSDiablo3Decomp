// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmPrismaticJoint.cpp
#include <stdio.h>
#include "dmPrismaticJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"
















































































void dmPrismaticJoint::Create(const dmPrismaticJointDef * pDef)
{

    m_localFrameA = pDef->m_localFrameA;
    m_localFrameB = pDef->m_localFrameB;

    m_limitState = pDef->m_enableLimit ? dm_limitInactiveState : dm_limitOffState;








    m_lowerLimit = pDef->m_lowerLimit;
    m_upperLimit = pDef->m_upperLimit;

    m_motorMode = pDef->m_motorMode;
    m_motorSpeed = 0.0f;
    m_desiredTranslation = 0.0f;
    m_maxMotorForce = pDef->m_maxMotorForce;
}


void dmPrismaticJoint::InitVelocityConstraints(const dmBody * pBodyA, const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    m_localCenterA = pBodyA->m_sweep.m_localCenter;
    m_localCenterB = pBodyB->m_sweep.m_localCenter;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];

    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;


    dm_vec4 qfA = dmMulQQ(qA, m_localFrameA.q);
    m_worldFrameA = dmMtxFromQuat(qfA);
    dm_vec4 T = m_worldFrameA.rx;
    dm_vec4 U = m_worldFrameA.ry;
    dm_vec4 V = m_worldFrameA.rz;

    dm_vec4 xA = pBodyA->m_sweep.m_center;
    dm_vec4 xB = pBodyB->m_sweep.m_center;
    dm_vec4 rA = dmRotate(qA, m_localFrameA.p - m_localCenterA);
    dm_vec4 rB = dmRotate(qB, m_localFrameB.p - m_localCenterB);

    // 142: columns 35/41/47 fit "(xB - xA) + (rB - rA)" starting at column 31.
    dm_vec4 translation = dmDot3((xB - xA) + (rB - rA), V);


    // 146: the original adds rA to the xB - xA of 142 (fadd at column 23).
    m_Jma1 = -dmCross(xB - xA + rA, V);
    m_Jma2 = dmCross(rB, V);
    dm_vec4 IJma1 = dmMul33(invIA, m_Jma1);
    dm_vec4 IJma2 = dmMul33(invIB, m_Jma2);

    {
        dm_vec4 K = invMassA + invMassB + dmDot3(m_Jma1, IJma1) + dmDot3(m_Jma2, IJma2);
        m_motorMass = dmF32(dmSafeReciprocal(K));
        // 155: cmp #2 then #1; the three bias stores merge into one at 171. 157: 0.2f has a 19-char name in the original (column 56).
        if (m_motorMode == dm_motorPositionMode)
        {
            m_motorBias = m_motorSpeed + 0.2f * dmF32(pData->step.inv_dt * (dmV4(m_desiredTranslation) - translation));
        }
        else if (m_motorMode == dm_motorVelocityMode)
        {
            m_motorBias = m_motorSpeed;
        }
        else
        {
            m_motorBias = 0.0f;
        }

    }


    if (m_limitState != dm_limitOffState)
    {
        const dm_vec4 lowerLimit = dmV4(m_lowerLimit);
        const dm_vec4 upperLimit = dmV4(m_upperLimit);
        const dm_vec4 linearSlop = dmV4(0.016404198f);
        // 177: 0x3c866219 (0.016404198f) is materialized as linearSlop; the 2x factor is the dm_vec4_two global (column 44).
        if (dmLT(dmAbs(upperLimit - lowerLimit), dm_vec4_two * linearSlop))
        {
            m_limitState = dm_limitEqualState;
        }
        else if (dmLE(translation, lowerLimit))
        {
            if (m_limitState != dm_limitLowerState)
            {
                m_limitState = dm_limitLowerState;
                m_limitImpulse = 0.0f;
            }
        }
        else if (dmGE(translation, upperLimit))
        {
            if (m_limitState != dm_limitUpperState)
            {
                m_limitState = dm_limitUpperState;
                m_limitImpulse = 0.0f;
            }
        }
        else
        {
            m_limitState = dm_limitInactiveState;
            m_limitImpulse = 0.0f;
        }
    }
    else
    {
        m_limitImpulse = 0.0f;
    }

    // 212 is indented one level deeper (columns 17/19/39): a brace block without locals.
    {


        m_angularMass = dmInvertSym33(invIA + invIB);
    }



    m_Ja11 = -dmCross(xB + rA - xA, T); m_Ja12 = dmCross(rB, T);
    m_Ja21 = -dmCross(xB + rA - xA, U); m_Ja22 = dmCross(rB, U);


    dm_vec4 IJa11 = dmMul33(invIA, m_Ja11), IJa12 = dmMul33(invIB, m_Ja12);
    dm_vec4 IJa21 = dmMul33(invIA, m_Ja21), IJa22 = dmMul33(invIB, m_Ja22);




    dm_vec4 Ka = invMassA + invMassB + dmDot3(m_Ja11, IJa11) + dmDot3(m_Ja12, IJa12);
    dm_vec4 Kb = dmDot3(m_Ja11, IJa21) + dmDot3(m_Ja12, IJa22);
    dm_vec4 Kd = invMassA + invMassB + dmDot3(m_Ja21, IJa21) + dmDot3(m_Ja22, IJa22);


    dm_vec4 d = dmSafeReciprocal(Ka * Kd - Kb * Kb);


    // 237: dmF32 holds an fneg; column 21 is the "*" of "d * -Kb".
    m_mass11 = dmF32(d * Kd);
    m_mass12 = dmF32(d * -Kb);
    m_mass22 = dmF32(d * Ka);



    if (pData->step.useWarmStarting)
    {
        dm_vec4 ratio = pData->step.dtRatio;
        m_angularImpulse *= ratio;
        m_impulse1 *= dmF32(ratio);
        m_impulse2 *= dmF32(ratio);
        m_limitImpulse *= dmF32(ratio);
        m_motorImpulse *= dmF32(ratio);

        {
            // 253: impulse has no location; column 42 is the '+' inside dmV4(...).
            dm_vec4 impulse = dmV4(m_motorImpulse + m_limitImpulse);


            dm_vec4 P = impulse * V;
            pData->aVelocities[islandIndexA].v -= invMassA * P;
            pData->aVelocities[islandIndexB].v += invMassB * P;


            pData->aVelocities[islandIndexA].w += IJma1 * impulse;
            pData->aVelocities[islandIndexB].w += IJma2 * impulse;
        }

        // 267/268 are indented like 257 (column 11): a brace block without locals.
        {
            pData->aVelocities[islandIndexA].w -= dmMul33(invIA, m_angularImpulse);
            pData->aVelocities[islandIndexB].w += dmMul33(invIB, m_angularImpulse);
        }

        {

            dm_vec4 impulse1 = dmV4(m_impulse1);
            dm_vec4 impulse2 = dmV4(m_impulse2);

            dm_vec4 P = impulse1 * T + impulse2 * U;


            pData->aVelocities[islandIndexA].v -= invMassA * P;
            pData->aVelocities[islandIndexB].v += invMassB * P;

            // 284: both products scale by impulse2 in the original (fmul ..., v6.s[0] at columns 48 and 67).
            pData->aVelocities[islandIndexA].w += IJa11 * impulse1 + IJa21 * impulse2;
            pData->aVelocities[islandIndexB].w += IJa12 * impulse2 + IJa22 * impulse2;
        }
    }
    else
    {
        m_impulse1 = 0.0f;
        m_impulse2 = 0.0f;
        m_angularImpulse = dmV4(0.0f);
        m_motorImpulse = 0.0f;
        m_limitImpulse = 0.0f;
    }
}

// NON_MATCHING: 1004 versus 996 bytes; no calls. The first 125 instructions (through the limit block) are identical. From the angular block (389) on, the original interleaves the four IJa dmMul33 products (418-421) with the angular update and the 403/404 dot products; ours schedules them later and needs d8/d9 (save/restore = the 8 bytes). Operand orders and negations re-spelled from columns (327, 331, 332, 342-343, 354, 382-383, 403-404 dmDot3(vB - vA, T/U) with tab columns 27/36/57, 422-423) and dmMul33/dmDot3 on their DWARF shape compiled to identical code.
void dmPrismaticJoint::SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];

    dm_vec4 vA = pData->aVelocities[islandIndexA].v;
    dm_vec4 wA = pData->aVelocities[islandIndexA].w;
    dm_vec4 vB = pData->aVelocities[islandIndexB].v;
    dm_vec4 wB = pData->aVelocities[islandIndexB].w;

    dm_vec4 V = m_worldFrameA.rz;


    dm_vec4 IJma1 = dmMul33(invIA, m_Jma1);
    dm_vec4 IJma2 = dmMul33(invIB, m_Jma2);


    if (m_motorMode != dm_motorOffMode && m_limitState != dm_limitEqualState)
    {

        dm_vec4 jv = dmDot3(V, vB - vA) + dmDot3(m_Jma1, wA) + dmDot3(m_Jma2, wB) - dmV4(m_motorBias);


        dm_vec4 lambda = -dmV4(m_motorMass) * jv;


        dm_vec4 motorImpulse = dmV4(m_motorImpulse);
        dm_vec4 maxImpulse = pData->step.dt * dmV4(m_maxMotorForce);
        dm_vec4 newImpulse = dmClamp(motorImpulse + lambda, -maxImpulse, maxImpulse);
        lambda = newImpulse - motorImpulse;
        m_motorImpulse = dmGetX(newImpulse);


        dm_vec4 P = lambda * V;
        vA -= invMassA * P;
        vB += invMassB * P;


        wA += lambda * IJma1;
        wB += lambda * IJma2;
    }

    // Inference: the unsigned comparison at 347 includes lower, upper and equal limit states.
    if (m_limitState != dm_limitOffState && m_limitState != dm_limitInactiveState)
    {

        dm_vec4 jv = dmDot3(V, vB - vA) + dmDot3(m_Jma1, wA) + dmDot3(m_Jma2, wB);



        dm_vec4 lambda = -dmV4(m_motorMass) * jv;
        dm_vec4 limitImpulse = dmV4(m_limitImpulse);

        if (m_limitState == dm_limitEqualState)
        {
            limitImpulse += lambda;
        }
        else if (m_limitState == dm_limitLowerState)
        {
            dm_vec4 newImpulse = dmMax(limitImpulse + lambda, dmV4(0.0f));
            lambda = newImpulse - limitImpulse;
            limitImpulse = newImpulse;
        }
        else if (m_limitState == dm_limitUpperState)
        {
            dm_vec4 newImpulse = dmMin(limitImpulse + lambda, dmV4(0.0f));
            lambda = newImpulse - limitImpulse;
            limitImpulse = newImpulse;
        }

        m_limitImpulse = dmGetX(limitImpulse);


        dm_vec4 P = lambda * V;
        vA -= invMassA * P;
        vB += invMassB * P;


        wA += lambda * IJma1;
        wB += lambda * IJma2;
    }



    {
        dm_vec4 lambda = dmMul33(m_angularMass, wA - wB);
        m_angularImpulse += lambda;


        wA -= dmMul33(invIA, lambda);
        wB += dmMul33(invIB, lambda);
    }


    {
        dm_vec4 T = m_worldFrameA.rx;
        dm_vec4 U = m_worldFrameA.ry;


        dm_vec4 jv1 = dmDot3(vB - vA, T) + dmDot3(m_Ja11, wA) + dmDot3(m_Ja12, wB);
        dm_vec4 jv2 = dmDot3(vB - vA, U) + dmDot3(m_Ja21, wA) + dmDot3(m_Ja22, wB);


        dm_vec4 lambda1 = dmV4(m_mass11) * -jv1 - dmV4(m_mass12) * jv2;
        dm_vec4 lambda2 = dmV4(m_mass12) * -jv1 - dmV4(m_mass22) * jv2;
        m_impulse1 += dmGetX(lambda1);
        m_impulse2 += dmGetX(lambda2);


        dm_vec4 P = lambda1 * T + lambda2 * U;
        vA -= invMassA * P;
        vB += invMassB * P;


        dm_vec4 IJa11 = dmMul33(invIA, m_Ja11);
        dm_vec4 IJa12 = dmMul33(invIB, m_Ja12);
        dm_vec4 IJa21 = dmMul33(invIA, m_Ja21);
        dm_vec4 IJa22 = dmMul33(invIB, m_Ja22);
        wA += lambda1 * IJa11 + lambda2 * IJa21;
        wB += lambda1 * IJa12 + lambda2 * IJa22;
    }

    pData->aVelocities[islandIndexA].v = vA;
    pData->aVelocities[islandIndexA].w = wA;
    pData->aVelocities[islandIndexB].v = vB;
    pData->aVelocities[islandIndexB].w = wB;
}

// NON_MATCHING: 4940 versus 4940 bytes, frame 0x1a0 as the original (o02: one-line mass11/12/22 declaration and "a * -e + b * e" lambdas at 595-598 from columns removed 28 bytes and the 0x20 extra frame). About 1140 instructions differ by register numbering and load scheduling only (opcode counts equal but 2 str/ldp versus stp/ldr and 2 mov versus movi zero); the original DIE order loads xB/qB before xA/qA and keeps no location for C (539) and mass12 (595).
void dmPrismaticJoint::SolvePositionConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dm_vec4 xA = pData->aPositions[islandIndexA].x;
    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 xB = pData->aPositions[islandIndexB].x;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;


    dmMtx localFrameA = dmMtxFromQuat(m_localFrameA.q);

    // Inference: the zero-state branch at 450 skips only the limit block.
    if (m_limitState != dm_limitOffState)
    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);

        dm_vec4 rA = dmMul33(Ra, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmMul33(Rb, m_localFrameB.p - m_localCenterB);

        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);

        dm_vec4 V = dmMul33(Ra, localFrameA.rz);


        m_Jma1 = -dmCross(xB + rB - xA, V);
        m_Jma2 = dmCross(rB, V);

        dm_vec4 IJma1 = dmMul33(invIA, m_Jma1);
        dm_vec4 IJma2 = dmMul33(invIB, m_Jma2);


        dm_vec4 K = dmSafeReciprocal(invMassA + invMassB + dmDot3(m_Jma1, IJma1) + dmDot3(m_Jma2, IJma2));
        dm_vec4 translation = dmDot3(V, xB - xA + (rB - rA));
        // Inference: zero C when no limit comparison holds; the original carries zero through the unmatched tail.
#line 473
        dm_vec4 C = dmV4(0.0f);
        const dm_vec4 lowerLimit = dmV4(m_lowerLimit);
        const dm_vec4 upperLimit = dmV4(m_upperLimit);
        // Inference: correction is 0.25 from fmov in clamp at 481; no named constant survives in this CU.
#line 476
        const dm_vec4 maxLinearCorrection = dmV4(0.25f);
        // Inference: slop is float bits 0x3c866219, materialized in the lower/upper clamp at 485/489.
#line 477
        const dm_vec4 slop = dmV4(0.016404198482632637f);

        if (m_limitState == dm_limitEqualState)
        {
            C = dmClamp(translation, -maxLinearCorrection, maxLinearCorrection);
        }
        else if (dmLE(translation, lowerLimit))
        {
            C = dmClamp(translation - lowerLimit + slop, -maxLinearCorrection, dmV4(0.0f));
        }
        else if (dmGE(translation, upperLimit))
        {
            C = dmClamp(translation - upperLimit - slop, dmV4(0.0f), maxLinearCorrection);
        }
        // Inference: the 0x3e4ccccd constant at 491 is the 0.2 correction factor; no CU name survives.
#line 491
        C *= 0.2f;


        dm_vec4 lambda = -(K * C);

        dm_vec4 P = lambda * V;
        xA -= invMassA * P;
        xB += invMassB * P;

        dm_vec4 wA = lambda * IJma1;
        qA += dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        dm_vec4 wB = lambda * IJma2;
        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }


    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);

        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);



        dmMtx K = dmInvertSym33(invIA + invIB);

        dm_vec4 qFA = dmMulQQ(qA, m_localFrameA.q);
        dm_vec4 qFB = dmMulQQ(qB, m_localFrameB.q);
        dm_vec4 qI = dm_quat_identity;
        dm_vec4 qR = dmMulQQ(dmConjugate(qFA), qFB);

        // Inference: lane-W sign test selects the shortest quaternion rotation (527-529).
        if (dmGetW(qR) < 0.0f)
        {
            qR = -qR;
        }





        dm_vec4 omega = dm_vec4_two * dmMulQQ(qI - qR, dmConjugate(qR));


        dm_vec4 C = dmRotate(qFA, omega);


        dm_vec4 lambda = dmMul33(K, C);


        dm_vec4 wA = dmMul33(invIA, lambda);
        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        dm_vec4 wB = dmMul33(invIB, lambda);
        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }


    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);

        dm_vec4 rA = dmMul33(Ra, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmMul33(Rb, m_localFrameB.p - m_localCenterB);

        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);


        dm_vec4 worldT = dmMul33(Ra, localFrameA.rx);
        dm_vec4 worldU = dmMul33(Ra, localFrameA.ry);


        dm_vec4 pR = xB - xA + (rB - rA);
        dm_vec4 error1 = dmDot3(worldT, pR);
        dm_vec4 error2 = dmDot3(worldU, pR);


        m_Ja11 = -dmCross(xB + rB - xA, worldT); m_Ja12 = dmCross(rB, worldT);
        m_Ja21 = -dmCross(xB + rB - xA, worldU); m_Ja22 = dmCross(rB, worldU);


        dm_vec4 IJa11 = dmMul33(invIA, m_Ja11);
        dm_vec4 IJa12 = dmMul33(invIB, m_Ja12);
        dm_vec4 IJa21 = dmMul33(invIA, m_Ja21);
        dm_vec4 IJa22 = dmMul33(invIB, m_Ja22);




        dm_vec4 Ka = invMassA + invMassB + dmDot3(m_Ja11, IJa11) + dmDot3(m_Ja12, IJa12);
        dm_vec4 Kb = dmDot3(m_Ja11, IJa21) + dmDot3(m_Ja12, IJa22);
        dm_vec4 Kd = invMassA + invMassB + dmDot3(m_Ja21, IJa21) + dmDot3(m_Ja22, IJa22);


        dm_vec4 d = dmSafeReciprocal(Ka * Kd - Kb * Kb);

        // DWARF: mass11, mass12, mass22 share line 595; columns 22/39/57 give one declaration.
#line 595
        dm_vec4 mass11 = d * Kd, mass12 = d * Kb, mass22 = d * Ka;





#line 596
        // Columns 28/38/47 on 597 and 598: "a * -e + b * e" (the original spelling).
        dm_vec4 lambda1 = mass11 * -error1 + mass12 * error2;
        dm_vec4 lambda2 = mass22 * -error2 + mass12 * error1;


        dm_vec4 P = lambda1 * worldT + lambda2 * worldU;
        xA -= invMassA * P;
        xB += invMassB * P;


        dm_vec4 wA = lambda1 * IJa11 + lambda2 * IJa21;
        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        dm_vec4 wB = lambda1 * IJa12 + lambda2 * IJa22;
        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }

    pData->aPositions[islandIndexA].x = xA;
    pData->aPositions[islandIndexA].q = qA;
    pData->aPositions[islandIndexB].x = xB;
    pData->aPositions[islandIndexB].q = qB;
}
