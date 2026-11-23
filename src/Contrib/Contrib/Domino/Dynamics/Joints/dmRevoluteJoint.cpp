// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmRevoluteJoint.cpp

#include "dmRevoluteJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"
#include <string.h>











































































void dmRevoluteJoint::Create(const dmRevoluteJointDef * pDef)
{
    m_localFrameA = pDef->m_localFrameA;
    m_localFrameB = pDef->m_localFrameB;

    m_motorMode = pDef->m_motorMode;
    m_maxMotorTorque = pDef->m_maxMotorTorque;

    m_limitState = pDef->m_enableLimit ? dm_limitInactiveState : dm_limitOffState;








    m_desiredMotorSpeed = 0.0f;
    m_desiredMotorAngle = 0.0f;

    m_lowerAngle = pDef->m_lowerAngle;
    m_upperAngle = pDef->m_upperAngle;


    m_perpImpulse = vdupq_n_f32(0.0f);
    m_pivotImpulse = vdupq_n_f32(0.0f);
    m_motorImpulse = 0.0f;
    m_limitImpulse = 0.0f;
    m_motorMass = 0.0f;


    m_Bhb = m_Bha = vdupq_n_f32(0.0f);

    memset(&m_Ba, 0, sizeof(m_Ba));
    memset(&m_Bb, 0, sizeof(m_Bb));
}



























void dmRevoluteJoint::InitVelocityConstraints(const dmBody * pBodyA, const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dmMtx invIa = pData->aInvIs[islandIndexA];
    dmMtx invIb = pData->aInvIs[islandIndexB];

    m_localCenterA = pBodyA->m_sweep.m_localCenter;
    m_localCenterB = pBodyB->m_sweep.m_localCenter;
    // 160/161: column 35 is the member after "pBodyA->m_transform."; the loads read body +0x10.
    dm_vec4 qA = pBodyA->m_transform.q;
    dm_vec4 qB = pBodyB->m_transform.q;
    dm_vec4 qfA = dmMulQQ(qA, m_localFrameA.q);
    dm_vec4 qfB = dmMulQQ(qB, m_localFrameB.q);
    dm_vec4 qr = dmMulQQ(dmConjugate(qfA), qfB);
    // 166: dmRotate's dmCross loads dm_vec4_unitZ through the GOT.
    dm_vec4 axis = dmRotate(qfA, dm_vec4_unitZ);
    // Hinge axis for the motor and the limit; columns of 170-173 show a second indent level
    // (a brace block without locals, so the DWARF has no lexical block for it).
    {
        m_Jh = axis;
        m_Bha = dmMul33(pData->aInvIs[islandIndexA], m_Jh);
        m_Bhb = dmMul33(pData->aInvIs[islandIndexB], m_Jh);
        m_motorMass = dmF32(dmSafeReciprocal(dmDot3(m_Jh, m_Bha + m_Bhb)));
    }

    // Perpendicular angular constraint (lexical block 178-192).
    {
        dm_vec4 vr = qr;
        dm_vec4 sr = dmSplatW(qr);
        dm_vec4 ex = dm_vec4_unitX;
        dm_vec4 ey = dm_vec4_unitY;
        // 183/184: columns 13 (rhs), 29 (*), 48 (*), 53 (+); same Jacobian shape as dmShoulderJoint's twist.
        m_Jp.rx = dm_vec4_oneHalf * dmRotate(qfA, ex * sr + dmCross(vr, ex));
        m_Jp.ry = dm_vec4_oneHalf * dmRotate(qfA, ey * sr + dmCross(vr, ey));
        m_Jp.rz = vdupq_n_f32(0.0f); // no row: the transpose never loads rz, so it was a constant
        m_Jp.p = vdupq_n_f32(0.0f);
        m_Jp = dmTranspose33(m_Jp);



        dmMtx K = dmMulRART(m_Jp, invIa + invIb);
        m_perpMass = dmInvertSym22(K);
    }

    // Point to point constraint (lexical block 197-218).
    {
        dm_vec4 rA = dmRotate(qA, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmRotate(qB, m_localFrameB.p - m_localCenterB);


        m_Ja = dmMtxSetSkew(-rA);
        m_Jb = dmMtxSetSkew(-rB);


        // 206/209: column 25 is a reload of pData->aInvIs, not the invIa copy.
        m_Ba = dmMul33(pData->aInvIs[islandIndexA], m_Ja);
        m_Ba *= -1.0f;

        m_Bb = dmMul33(pData->aInvIs[islandIndexB], m_Jb);
        m_Bb *= -1.0f;

        dm_vec4 sumInvMass = invMassA + invMassB;

        m_pivotMass = dmMul33(m_Ja, m_Ba) + dmMul33(m_Jb, m_Bb);
        m_pivotMass.rx += sumInvMass * dm_vec4_unitX;
        m_pivotMass.ry += sumInvMass * dm_vec4_unitY;
        m_pivotMass.rz += sumInvMass * dm_vec4_unitZ;
        m_pivotMass = dmInvertSym33(m_pivotMass);
    }


    dm_float32 angle = 2.0f * dmEstAtan2(dmGetZ(qr), dmGetW(qr));
    if (angle < -3.14159265f)
    {
        angle += 2.0f * 3.14159265f;
    }
    else if (angle > 3.14159265f)
    {
        angle -= 2.0f * 3.14159265f;
    }


    if (m_limitState != dm_limitOffState)
    {
        // 0.069813177f: 4 degrees, as in dmShoulderJoint; the original spelling is unknown.
        if (dmAbs(m_upperAngle - m_lowerAngle) < 0.069813177f)
        {
            m_limitState = dm_limitEqualState;
        }
        else if (angle <= m_lowerAngle)
        {
            if (m_limitState != dm_limitLowerState)
            {
                m_limitState = dm_limitLowerState;
                m_limitImpulse = 0.0f;
            }
        }
        else if (angle >= m_upperAngle)
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

    // 274 has no row: its compare is attributed to 269.
    m_motorBias = 0.0f;
    if (m_motorMode == dm_motorPositionMode)
    {
        const dm_float32 k_baumgarte = 0.2f;
        m_motorBias = m_desiredMotorSpeed + k_baumgarte * dmF32(pData->step.inv_dt) * (m_desiredMotorAngle - angle);
    }
    else if (m_motorMode == dm_motorVelocityMode)
    {
        m_motorBias = m_desiredMotorSpeed;
    }
    else
    {
        m_motorImpulse = 0.0f;
    }


    if (pData->step.useWarmStarting)
    {
        dm_vec4 ratio = pData->step.dtRatio;
        m_pivotImpulse *= ratio;
        m_perpImpulse *= ratio;
        m_limitImpulse *= dmF32(ratio);
        m_motorImpulse *= dmF32(ratio);

        dm_vec4 T = dmMulT33(m_Jp, m_perpImpulse);

        pData->aVelocities[islandIndexA].v -= invMassA * m_pivotImpulse;
        pData->aVelocities[islandIndexA].w -= dmMul33(m_Ba, m_pivotImpulse);
        pData->aVelocities[islandIndexA].w -= dmMul33(invIa, T);
        pData->aVelocities[islandIndexA].w -= dmV4(m_motorImpulse + m_limitImpulse) * m_Bha;

        pData->aVelocities[islandIndexB].v += invMassB * m_pivotImpulse;
        pData->aVelocities[islandIndexB].w += dmMul33(m_Bb, m_pivotImpulse);
        pData->aVelocities[islandIndexB].w += dmMul33(invIb, T);
        pData->aVelocities[islandIndexB].w += dmV4(m_motorImpulse + m_limitImpulse) * m_Bhb;
    }
    else
    {
        // 308/309 have no rows of their own: their stores merge into 307's.
        m_perpImpulse = m_pivotImpulse = vdupq_n_f32(0.0f);
        m_motorImpulse = 0.0f;
        m_limitImpulse = 0.0f;
    }
}


void dmRevoluteJoint::SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 vA = pData->aVelocities[islandIndexA].v;
    dm_vec4 wA = pData->aVelocities[islandIndexA].w;
    dm_vec4 vB = pData->aVelocities[islandIndexB].v;
    dm_vec4 wB = pData->aVelocities[islandIndexB].w;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dmMtx invIa = pData->aInvIs[islandIndexA];
    dmMtx invIb = pData->aInvIs[islandIndexB];


    if (m_motorMode != dm_motorOffMode && m_limitState != dm_limitEqualState)
    {
        dm_float32 Cdot = dmDot3_f32(wB - wA, m_Jh) - m_motorBias;
        dm_float32 impulse = -m_motorMass * Cdot;

        dm_float32 maxImpulse = dmF32(pData->step.dt) * m_maxMotorTorque;
        dm_float32 newImpulse = dmClamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
        impulse = newImpulse - m_motorImpulse;
        m_motorImpulse = newImpulse;

        wA -= m_Bha * impulse;
        wB += m_Bhb * impulse;
    }


    if (m_limitState != dm_limitOffState && m_limitState != dm_limitInactiveState)
    {
        dm_float32 Cdot = dmDot3_f32(wB - wA, m_Jh);
        dm_float32 impulse = -m_motorMass * Cdot;

        switch (m_limitState) {
        case dm_limitEqualState:
            m_limitImpulse += impulse;
            break;
        case dm_limitLowerState:
            {
                dm_float32 newImpulse = dmMax(m_limitImpulse + impulse, 0.0f);
                impulse = newImpulse - m_limitImpulse;
                m_limitImpulse = newImpulse;
            }
            break;
        case dm_limitUpperState: {
                dm_float32 newImpulse = dmMin(m_limitImpulse + impulse, 0.0f);
                impulse = newImpulse - m_limitImpulse;
                m_limitImpulse = newImpulse;
            } break;
        }
        wA -= m_Bha * impulse;
        wB += m_Bhb * impulse;
    }

    // Perpendicular angular constraint.
    {
        dm_vec4 Cdot = dmMul33(m_Jp, wB - wA);
        dm_vec4 impulse = dmMul33(m_perpMass, -Cdot);
        m_perpImpulse += impulse;

        dm_vec4 L = dmMulT33(m_Jp, impulse);


        wA -= dmMul33(invIa, L);
        wB += dmMul33(invIb, L);
    }

    // Point to point constraint.
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


void dmRevoluteJoint::SolvePositionConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    dm_vec4 xA = pData->aPositions[islandIndexA].x;
    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 xB = pData->aPositions[islandIndexB].x;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;


    if (m_limitState != dm_limitOffState)
    {
        dm_vec4 qfA = dmMulQQ(qA, m_localFrameA.q);
        dm_vec4 qfB = dmMulQQ(qB, m_localFrameB.q);
        dm_vec4 qr = dmMulQQ(dmConjugate(qfA), qfB);
        dm_float32 angle = 2.0f * dmEstAtan2(dmGetZ(qr), dmGetW(qr));

        if (angle < -3.14159265f)
        {
            angle += 2.0f * 3.14159265f;
        }
        else if (angle > 3.14159265f)
        {
            angle -= 2.0f * 3.14159265f;
        }

        dm_vec4 impulse = vdupq_n_f32(0.0f);

        dm_vec4 J = dmRotate(qfA, dm_vec4_unitZ);



        dm_vec4 Ba = dmRotate(qA, dmMul33(pBase->m_localInvInertiaA, dmInvRotate(qA, J)));
        dm_vec4 Bb = dmRotate(qB, dmMul33(pBase->m_localInvInertiaB, dmInvRotate(qB, J)));

        dm_vec4 mass = dmSafeReciprocal(dmDot3(J, Ba + Bb));

        if (m_limitState == dm_limitEqualState)
        {
            // 448/454/460: 0.2f has a 19-character name in the original (columns 55/70); 0.5235988f is 30 degrees, 0.034906585f 2 degrees.
            dm_float32 C = 0.2f * dmClamp(angle, -0.5235988f, 0.5235988f);
            impulse = -mass * dmV4(C);
        }
        else if (angle <= m_lowerAngle)
        {

            dm_float32 C = 0.2f * dmClamp(angle - m_lowerAngle + 0.034906585f, -0.5235988f, 0.0f);
            impulse = -mass * dmV4(C);
        }
        else if (angle >= m_upperAngle)
        {

            dm_float32 C = 0.2f * dmClamp(angle - m_upperAngle - 0.034906585f, 0.0f, 0.5235988f);
            impulse = -mass * dmV4(C);
        }

        dm_vec4 wA = impulse * Ba;
        dm_vec4 wB = impulse * Bb;

        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }

    // Perpendicular angular constraint (lexical block 476-512).
    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);

        dm_vec4 qfA = dmMulQQ(qA, m_localFrameA.q);
        dm_vec4 qfB = dmMulQQ(qB, m_localFrameB.q);
        dm_vec4 qr = dmMulQQ(dmConjugate(qfA), qfB);

        dm_vec4 vr = qr;
        dm_vec4 sr = dmSplatW(qr);

        dm_vec4 ex = dm_vec4_unitX;
        dm_vec4 ey = dm_vec4_unitY;
        // 489-494: same rows as InitVelocityConstraints 183-187 (rz and p have no rows there either).
        dmMtx Jp;
        Jp.rx = dm_vec4_oneHalf * dmRotate(qfA, ex * sr + dmCross(vr, ex));
        Jp.ry = dm_vec4_oneHalf * dmRotate(qfA, ey * sr + dmCross(vr, ey));
        Jp.rz = vdupq_n_f32(0.0f);
        Jp.p = vdupq_n_f32(0.0f);
        Jp = dmTranspose33(Jp);


        dmMtx invIa = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIb = dmMulRART(Rb, pBase->m_localInvInertiaB);
        dmMtx K = dmMulRART(Jp, invIa + invIb);
        // 501 has no row: C is qr itself (the x/y lanes the transposed Jp reads).
        dm_vec4 C = qr;
        dm_vec4 lambda = dmSolveSym22(K, -C);

        dm_vec4 impulse = dmMulT33(m_Jp, lambda);

        dm_vec4 wA = dmMul33(invIa, impulse);
        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        dm_vec4 wB = dmMul33(invIb, impulse);
        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }


    // Point to point constraint (lexical block 518-553).
    {
        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);


        dmMtx invIA = dmMulRART(Ra, pBase->m_localInvInertiaA);
        dmMtx invIB = dmMulRART(Rb, pBase->m_localInvInertiaB);


        dm_vec4 rA = dmMul33(Ra, m_localFrameA.p - m_localCenterA);
        dm_vec4 rB = dmMul33(Rb, m_localFrameB.p - m_localCenterB);
        // 529: columns 19/25/30 put the first operator inside parentheses.
        dm_vec4 C = (xB - xA) + rB - rA;


        dmMtx Ja, Jb;
        Ja = dmMtxSetSkew(-rA);
        Jb = dmMtxSetSkew(-rB);

        dm_vec4 sumInvMass = invMassA + invMassB;
        m_pivotMass = dmMulRART(Ja, invIA) + dmMulRART(Jb, invIB);
        m_pivotMass.rx += sumInvMass * dm_vec4_unitX;
        m_pivotMass.ry += sumInvMass * dm_vec4_unitY;
        m_pivotMass.rz += sumInvMass * dm_vec4_unitZ;

        dm_vec4 impulse = dmSolveSym33(m_pivotMass, -C);
        xA -= invMassA * impulse;
        dm_vec4 wA = dmMul33(invIA, dmMulT33(Ja, impulse));

        qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
        qA = dmNormalize4(qA);

        xB += invMassB * impulse;
        dm_vec4 wB = dmMul33(invIB, dmMulT33(Jb, impulse));

        qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
        qB = dmNormalize4(qB);
    }

    pData->aPositions[islandIndexA].x = xA;
    pData->aPositions[islandIndexA].q = qA;
    pData->aPositions[islandIndexB].x = xB;
    pData->aPositions[islandIndexB].q = qB;
}
