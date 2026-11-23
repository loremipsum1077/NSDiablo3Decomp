// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmShoulderJoint.cpp
#include <stdio.h>
#include "dmShoulderJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"
#include <string.h>

// Defined in Common/dmMath.cpp:16; no declaration of it in the owner CU (Create loads it at line 101).
extern const dm_vec4 dm_vec4_unitW;













































































void dmShoulderJoint::Create(const dmShoulderJointDef * pDef)
{
    m_localFrameA = pDef->m_localFrameA;
    m_localFrameB = pDef->m_localFrameB;


    m_pivotImpulse = vdupq_n_f32(0.0f);
    m_motorImpulse = vdupq_n_f32(0.0f);
    m_twistImpulse = 0.0f;
    m_coneImpulse = 0.0f;
    m_maxMotorTorque = pDef->m_maxMotorTorque;

    m_motorAngularVelocity = vdupq_n_f32(0.0f);
    m_motorQuaternion = dm_vec4_unitW;

    m_lowerTwistLimit = pDef->m_lowerTwistAngle;
    m_upperTwistLimit = pDef->m_upperTwistAngle;
    // 0x3e32b8c3 / 0x403de44e: 10 and 170 degrees in radians; the spelling of the limits is unknown.
    m_swingLimit = dmClamp(pDef->m_coneAngle, 0.17453294f, 2.9670597f);

    m_motorMode = pDef->m_motorMode;

    if (pDef->m_enableTwistLimit)
    {
        m_twistState = dm_limitInactiveState;
    }
    else
    {
        m_twistState = dm_limitOffState;
    }

    m_coneState = dm_limitInactiveState;

    // One 488-byte memset at line 122 covers m_Ja..m_coneMass; one member per line inferred (14 lines).
    memset(&m_Ja, 0, sizeof(m_Ja));
    memset(&m_Jb, 0, sizeof(m_Jb));
    memset(&m_Ba, 0, sizeof(m_Ba));
    memset(&m_Bb, 0, sizeof(m_Bb));
    memset(&m_Jt, 0, sizeof(m_Jt));
    memset(&m_Bta, 0, sizeof(m_Bta));
    memset(&m_Btb, 0, sizeof(m_Btb));
    memset(&m_Jc, 0, sizeof(m_Jc));
    memset(&m_Bca, 0, sizeof(m_Bca));
    memset(&m_Bcb, 0, sizeof(m_Bcb));
    memset(&m_pivotMass, 0, sizeof(m_pivotMass));
    memset(&m_motorMass, 0, sizeof(m_motorMass));
    memset(&m_twistMass, 0, sizeof(m_twistMass));
    memset(&m_coneMass, 0, sizeof(m_coneMass));

}













































void dmShoulderJoint::InitVelocityConstraints(const dmBody * pBodyA, const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData)
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


    if (m_motorMode != dm_motorOffMode)
    {
        m_motorMass = dmInvertSym33(invIA + invIB);


        if (m_motorMode == dm_motorPositionMode)
        {

            dm_vec4 qr = dmMulQQ(dmConjugate(qA), qB);

            if (dmLTZ(dmDot4(qr, m_motorQuaternion)))
            {
                qr = -qr;
            }


            dm_vec4 wp = dm_vec4_two * pData->step.inv_dt * dmMulQQ(m_motorQuaternion - qr, dmConjugate(qr));

            const dm_vec4 k_baumgarte = vdupq_n_f32(0.2f);



            m_motorBias = m_motorAngularVelocity + dmRotate(qA, k_baumgarte * wp);
        }
        else if (m_motorMode == dm_motorVelocityMode)
        {
            m_motorBias = dmRotate(qA, m_motorAngularVelocity);
        }
        else
        {
            m_motorBias = vdupq_n_f32(0.0f);
        }
    }
    else
    {
        m_motorImpulse = vdupq_n_f32(0.0f);
    }

    dm_vec4 qfA = dmMulQQ(qA, m_localFrameA.q);
    dm_vec4 qfB = dmMulQQ(qB, m_localFrameB.q);

    dm_vec4 eCone = dmRotate(qfA, dm_vec4_unitZ);
    dm_vec4 eTwist = dmRotate(qfB, dm_vec4_unitZ);
    dm_vec4 eSwing = dmCross(eCone, eTwist);
    dm_float32 sinSwingAngle = 0.0f;
    dm_float32 cosSwingAngle = dmDot3_f32(eCone, eTwist);
    if (dmIsZeroSafe3(eSwing) == false)
    {
        sinSwingAngle = dmLength3_f32(eSwing);
        eSwing = dmNormalize3(eSwing);
    }

    dm_float32 swingAngle = dmEstAtan2(sinSwingAngle, cosSwingAngle);


    if (m_twistState != dm_limitOffState)
    {
        dm_vec4 qr = dmMulQQ(dmConjugate(qfA), qfB);
        dm_float32 twistAngle = 2.0f * dmEstAtan2(dmGetZ(qr), dmGetW(qr));

        if (twistAngle < -3.14159265f)
        {
            twistAngle += 2.0f * 3.14159265f;
        }
        else if (twistAngle > 3.14159265f)
        {
            twistAngle -= 2.0f * 3.14159265f;
        }
        // 0.069813177f: 4 degrees in radians; the spelling of the tolerance is unknown.

        if (dmAbs(m_upperTwistLimit - m_lowerTwistLimit) < 0.069813177f)
        {
            m_twistState = dm_limitEqualState;
        }
        else if (twistAngle <= m_lowerTwistLimit)
        {
            if (m_twistState != dm_limitLowerState)
            {
                m_twistState = dm_limitLowerState;
                m_twistImpulse = 0.0f;
            }
        }
        else if (twistAngle >= m_upperTwistLimit)
        {
            if (m_twistState != dm_limitUpperState)
            {
                m_twistState = dm_limitUpperState;
                m_twistImpulse = 0.0f;
            }
        }
        else
        {
            m_twistState = dm_limitInactiveState;
            m_twistImpulse = 0.0f;
        }
        // 2.9670597f: 170 degrees, as in Create (line 106).
        if (m_twistState != dm_limitInactiveState && swingAngle < 2.9670597f)
        {
            dm_vec4 ez = dm_vec4_unitZ;
            dm_vec4 vr = qr;
            dm_vec4 sr = dmSplatW(qr);

            dm_vec4 vrz = dmSplatZ(qr);
            dm_vec4 denominator = vrz * vrz + sr * sr;
            m_Jt = dmSafeReciprocal(denominator) * dmRotate(qfA, sr * (ez * sr + dmCross(vr, ez)) + vrz * vr);

            m_Bta = dmMul33(invIA, m_Jt);
            m_Btb = dmMul33(invIB, m_Jt);

            dm_vec4 mass = dmSafeReciprocal(dmDot3(m_Jt, m_Bta + m_Btb));
            m_twistMass = dmF32(mass);
        }
    }
    else
    {
        m_twistImpulse = 0.0f;
    }


    if (swingAngle >= m_swingLimit)
    {
        if (m_coneState != dm_limitUpperState)
        {
            m_coneState = dm_limitUpperState;
            m_coneImpulse = 0.0f;
        }

        m_Jc = eSwing;
        m_Bca = dmMul33(invIA, m_Jc);
        m_Bcb = dmMul33(invIB, m_Jc);

        dm_vec4 mass = dmSafeReciprocal(dmDot3(m_Jc, m_Bca + m_Bcb));
        m_coneMass = dmF32(mass);
    }
    else
    {
        m_coneState = dm_limitInactiveState;
        m_coneImpulse = 0.0f;
    }
    {




        dm_vec4 rA = dmRotate(qA, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmRotate(qB, m_localFrameB.p - m_localCenterB);


        m_Ja = dmMtxSetSkew(-rA);
        m_Jb = dmMtxSetSkew(-rB);



        m_Ba = dmMul33(invIA, m_Ja);
        m_Ba *= -1.0f;

        m_Bb = dmMul33(invIB, m_Jb);
        m_Bb *= -1.0f;

        dm_vec4 sumInvMass = invMassA + invMassB;

        m_pivotMass = dmMul33(m_Ja, m_Ba) + dmMul33(m_Jb, m_Bb);
        m_pivotMass.rx += sumInvMass * dm_vec4_unitX;
        m_pivotMass.ry += sumInvMass * dm_vec4_unitY;
        m_pivotMass.rz += sumInvMass * dm_vec4_unitZ;
        m_pivotMass = dmInvertSym33(m_pivotMass);
    }


    if (pData->step.useWarmStarting)
    {
        dm_vec4 ratio = pData->step.dtRatio;
        m_pivotImpulse *= ratio;
        m_motorImpulse *= ratio;
        m_twistImpulse *= dmF32(ratio);
        m_coneImpulse *= dmF32(ratio);

        pData->aVelocities[islandIndexA].v -= invMassA * m_pivotImpulse;
        pData->aVelocities[islandIndexA].w -= dmMul33(m_Ba, m_pivotImpulse) + dmMul33(invIA, m_motorImpulse);
        pData->aVelocities[islandIndexA].w -= dmV4(m_twistImpulse) * m_Bta + dmV4(m_coneImpulse) * m_Bca;

        pData->aVelocities[islandIndexB].v += invMassB * m_pivotImpulse;
        pData->aVelocities[islandIndexB].w += dmMul33(m_Bb, m_pivotImpulse) + dmMul33(invIB, m_motorImpulse);
        pData->aVelocities[islandIndexB].w += dmV4(m_twistImpulse) * m_Btb + dmV4(m_coneImpulse) * m_Bcb;
    }
    else
    {

        m_pivotImpulse = m_motorImpulse = vdupq_n_f32(0.0f);
        m_twistImpulse = 0.0f;
        m_coneImpulse = 0.0f;
    }
}


void dmShoulderJoint::SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 vA = pData->aVelocities[islandIndexA].v;
    dm_vec4 wA = pData->aVelocities[islandIndexA].w;
    dm_vec4 vB = pData->aVelocities[islandIndexB].v;
    dm_vec4 wB = pData->aVelocities[islandIndexB].w;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];


    if (m_motorMode != dm_motorOffMode && m_maxMotorTorque > 0.0f)
    {
        dm_vec4 Cdot = wB - wA - m_motorBias;
        dm_vec4 impulse = dmMul33(m_motorMass, -Cdot);

        dm_vec4 newImpulse = m_motorImpulse + impulse;
        dm_vec4 maxImpulse = pData->step.dt * m_maxMotorTorque;
        if (dmGT(dmLengthSqr3(newImpulse), dmLengthSqr3(maxImpulse)))
        {
            newImpulse = maxImpulse * dmNormalize3(newImpulse);
        }

        impulse = newImpulse - m_motorImpulse;
        m_motorImpulse = newImpulse;

        wA -= dmMul33(invIA, impulse);
        wB += dmMul33(invIB, impulse);
    }


    if (m_twistState != dm_limitOffState && m_twistState != dm_limitInactiveState)
    {
        dm_vec4 Cdot = dmDot3(wB - wA, m_Jt);
        dm_vec4 impulse = -(m_twistMass * Cdot);  // the original negates the product (fneg after the splat), so the parentheses are inferred

        switch (m_twistState) {
        case dm_limitEqualState:
            m_twistImpulse += dmF32(impulse);
            break;
        case dm_limitLowerState:
            {
                dm_vec4 newImpulse = dmMax(dmV4(m_twistImpulse) + impulse, vdupq_n_f32(0.0f));
                impulse = newImpulse - m_twistImpulse;
                m_twistImpulse = dmF32(newImpulse);
            }
            break;
        case dm_limitUpperState: {
                dm_vec4 newImpulse = dmMin(dmV4(m_twistImpulse) + impulse, vdupq_n_f32(0.0f));
                impulse = newImpulse - m_twistImpulse;
                m_twistImpulse = dmF32(newImpulse);
            } break;
        }
        wA -= impulse * m_Bta;
        wB += impulse * m_Btb;
    }


    if (m_coneState == dm_limitUpperState)
    {
        dm_vec4 Cdot = dmDot3(wB - wA, m_Jc);
        dm_vec4 impulse = -(m_coneMass * Cdot);  // parentheses inferred as on line 436

        dm_vec4 newImpulse = dmMin(dmV4(m_coneImpulse) + impulse, vdupq_n_f32(0.0f));
        impulse = newImpulse - m_coneImpulse;
        m_coneImpulse = dmF32(newImpulse);

        wA -= impulse * m_Bca;
        wB += impulse * m_Bcb;
    }


    {
        dm_vec4 Cdot = vB - vA + dmMul33(m_Jb, wB) - dmMul33(m_Ja, wA);
        dm_vec4 impulse = dmMul33(m_pivotMass, -Cdot);
        m_pivotImpulse += impulse;

        vA -= invMassA * impulse;
        wA -= dmMul33(m_Ba, impulse);
        vB += invMassB * impulse;
        wB += dmMul33(m_Bb, impulse);
    }

    pData->aVelocities[islandIndexA].v = vA;
    pData->aVelocities[islandIndexA].w = wA;
    pData->aVelocities[islandIndexB].v = vB;
    pData->aVelocities[islandIndexB].w = wB;
}


void dmShoulderJoint::SolvePositionConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 xA = pData->aPositions[islandIndexA].x;
    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 xB = pData->aPositions[islandIndexB].x;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dm_vec4 qfA = dmMulQQ(qA, m_localFrameA.q);
    dm_vec4 qfB = dmMulQQ(qB, m_localFrameB.q);

    dm_vec4 eCone = dmRotate(qfA, dm_vec4_unitZ);
    dm_vec4 eTwist = dmRotate(qfB, dm_vec4_unitZ);
    dm_vec4 eSwing = dmCross(eCone, eTwist);
    dm_float32 sinSwingAngle = 0.0f;
    dm_float32 cosSwingAngle = dmDot3_f32(eCone, eTwist);
    if (dmIsZeroSafe3(eSwing) == false)
    {
        sinSwingAngle = dmLength3_f32(eSwing);
    }

    dm_float32 swingAngle = dmEstAtan2(sinSwingAngle, cosSwingAngle);


    if (m_twistState != dm_limitOffState && swingAngle < 2.9670597f)
    {
        dm_vec4 qr = dmMulQQ(dmConjugate(qfA), qfB);
        dm_float32 twistAngle = 2.0f * dmEstAtan2(dmGetZ(qr), dmGetW(qr));
        if (twistAngle < -3.14159265f)
        {
            twistAngle += 2.0f * 3.14159265f;
        }
        else if (twistAngle > 3.14159265f)
        {
            twistAngle -= 2.0f * 3.14159265f;
        }

        dm_vec4 ez = dm_vec4_unitZ;
        dm_vec4 vr = qr;
        dm_vec4 vrz = dmSplatZ(qr);
        dm_vec4 sr = dmSplatW(qr);
        dm_vec4 denominator = vrz * vrz + sr * sr;
        dm_vec4 Jt = dmSafeReciprocal(denominator) * dmRotate(qfA, sr * (ez * sr + dmCross(vr, ez)) + vrz * vr);


        dm_vec4 Ba = dmRotate(qA, dmMul33(pBase->m_localInvInertiaA, dmInvRotate(qA, Jt)));
        dm_vec4 Bb = dmRotate(qB, dmMul33(pBase->m_localInvInertiaB, dmInvRotate(qB, Jt)));

        dm_vec4 mass = dmSafeReciprocal(dmDot3(Jt, Ba + Bb));


        dm_vec4 impulse = vdupq_n_f32(0.0f);
        if (m_twistState == dm_limitEqualState)
        {
            // 0.2f: the original spells this factor with a 19-character name (columns 39/60 of line 558); 0.5235988f is 30 degrees. Inferred: -mass * dmV4(C) here and at 559/566, since the original computes mass (545) only after the branches.
            dm_float32 C = 0.2f * dmClamp(twistAngle, -0.5235988f, 0.5235988f);
            impulse = -mass * dmV4(C);
        }
        else if (twistAngle <= m_lowerTwistLimit)
        {
            // 0.034906585f: 2 degrees.
            dm_float32 C = 0.2f * dmClamp(twistAngle - m_lowerTwistLimit + 0.034906585f, -0.5235988f, 0.0f);
            impulse = -mass * dmV4(C);
        }
        else if (twistAngle >= m_upperTwistLimit)
        {

            dm_float32 C = 0.2f *
                dmClamp(twistAngle - m_upperTwistLimit - 0.034906585f, 0.0f, 0.5235988f);
            impulse = -mass * dmV4(C);
        }

        dm_vec4 wA = impulse * Ba;
        dm_vec4 wB = impulse * Bb;

        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }


    qfA = dmMulQQ(qA, m_localFrameA.q);
    qfB = dmMulQQ(qB, m_localFrameB.q);

    eCone = dmRotate(qfA, dm_vec4_unitZ);
    eTwist = dmRotate(qfB, dm_vec4_unitZ);
    eSwing = dmCross(eCone, eTwist);
    sinSwingAngle = 0.0f;
    cosSwingAngle = dmDot3_f32(eCone, eTwist);
    if (dmIsZeroSafe3(eSwing) == false)
    {
        sinSwingAngle = dmLength3_f32(eSwing);
        eSwing = dmNormalize3(eSwing);
    }
    swingAngle = dmEstAtan2(sinSwingAngle, cosSwingAngle);


    if (swingAngle >= m_swingLimit)
    {
        dm_vec4 J = eSwing;


        dm_vec4 Ba = dmRotate(qA, dmMul33(pBase->m_localInvInertiaA, dmInvRotate(qA, J)));
        dm_vec4 Bb = dmRotate(qB, dmMul33(pBase->m_localInvInertiaB, dmInvRotate(qB, J)));

        dm_vec4 mass = dmSafeReciprocal(dmDot3(J, Ba + Bb));

        dm_float32 C = 0.2f * dmClamp(swingAngle - m_swingLimit - 0.034906585f, 0.0f, 0.5235988f);
        dm_vec4 impulse = -(mass * C);

        dm_vec4 wA = impulse * Ba;
        dm_vec4 wB = impulse * Bb;

        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }
    {


        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);


        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);


        dm_vec4 rA = dmMul33(Ra, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmMul33(Rb, m_localFrameB.p - m_localCenterB);

        dm_vec4 C = xB - xA + rB - rA;


        dmMtx Ja = dmMtxSetSkew(-rA);
        dmMtx Jb = dmMtxSetSkew(-rB);

        dm_vec4 sumInvMass = invMassA + invMassB;
        m_pivotMass = dmMulRART(Ja, invIA) + dmMulRART(Jb, invIB);
        m_pivotMass.rx += sumInvMass * dm_vec4_unitX;
        m_pivotMass.ry += sumInvMass * dm_vec4_unitY;
        m_pivotMass.rz += sumInvMass * dm_vec4_unitZ;

        dm_vec4 impulse = dmSolveSym33(m_pivotMass, -C);
        xA -= invMassA * impulse;
        dm_vec4 wA = dmMul33(invIA, dmMulT33(Ja, impulse));

        xB += invMassB * impulse;
        dm_vec4 wB = dmMul33(invIB, dmMulT33(Jb, impulse));

        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }

    pData->aPositions[islandIndexA].x = xA;
    pData->aPositions[islandIndexA].q = qA;
    pData->aPositions[islandIndexB].x = xB;
    pData->aPositions[islandIndexB].q = qB;
}