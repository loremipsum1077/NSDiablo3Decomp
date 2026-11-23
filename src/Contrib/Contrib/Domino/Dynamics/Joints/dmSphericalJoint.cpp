// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmSphericalJoint.cpp

#include "dmSphericalJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"


























































void dmSphericalJoint::Create(const dmSphericalJointDef * pDef)
{
    m_localCenterA = m_localCenterB = vdupq_n_f32(0.0f);

    m_localAnchorA = pDef->m_localAnchorA;
    m_localAnchorB = pDef->m_localAnchorB;
    m_pivotImpulse = vdupq_n_f32(0.0f);

    m_frictionTorque = pDef->m_frictionTorque;
    m_frictionImpulse = vdupq_n_f32(0.0f);
}



void dmSphericalJoint::InitVelocityConstraints(const dmBody * pBodyA, const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 qA = pBodyA->m_transform.q;
    dm_vec4 qB = pBodyB->m_transform.q;

    dm_vec4 invMassA = dmV4(pBase->m_invMassA);
    dm_vec4 invMassB = dmV4(pBase->m_invMassB);

    m_localCenterA = pBodyA->m_sweep.m_localCenter;
    m_localCenterB = pBodyB->m_sweep.m_localCenter;

    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];


    {

        dm_vec4 rA = dmRotate(qA, m_localAnchorA - m_localCenterA);
        dm_vec4 rB = dmRotate(qB, m_localAnchorB - m_localCenterB);


        m_Ja = dmMtxSetSkew(-rA);
        m_Jb = dmMtxSetSkew(-rB);

        dm_vec4 sumInvMass = invMassA + invMassB;

        m_pivotMass = dmMulRART(m_Ja, invIA) + dmMulRART(m_Jb, invIB);
        m_pivotMass.rx += sumInvMass * dm_vec4_unitX;
        m_pivotMass.ry += sumInvMass * dm_vec4_unitY;
        m_pivotMass.rz += sumInvMass * dm_vec4_unitZ;
        m_pivotMass = dmInvertSym33(m_pivotMass);
    }


    if (m_frictionTorque > 0.0f)
    {
        m_frictionMass = dmInvertSym33(invIA + invIB);
    }
    else
    {
        m_frictionImpulse = vdupq_n_f32(0.0f);
    }


    if (pData->step.useWarmStarting)
    {
        dm_vec4 ratio = pData->step.dtRatio;
        m_pivotImpulse *= ratio;
        m_frictionImpulse *= ratio;

        dm_vec4 P = m_pivotImpulse;
        dm_vec4 La = dmMulT33(m_Ja, P);
        dm_vec4 Lb = dmMulT33(m_Jb, P);

        pData->aVelocities[islandIndexA].v -= invMassA * P;
        pData->aVelocities[islandIndexA].w -= dmMul33(invIA, La + m_frictionImpulse);

        pData->aVelocities[islandIndexB].v += invMassB * P;
        pData->aVelocities[islandIndexB].w += dmMul33(invIB, Lb + m_frictionImpulse);
    }
    else
    {
        m_pivotImpulse = vdupq_n_f32(0.0f);
        m_frictionImpulse = vdupq_n_f32(0.0f);
    }
}


void dmSphericalJoint::SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 vA = pData->aVelocities[islandIndexA].v;
    dm_vec4 wA = pData->aVelocities[islandIndexA].w;
    dm_vec4 vB = pData->aVelocities[islandIndexB].v;
    dm_vec4 wB = pData->aVelocities[islandIndexB].w;

    dmMtx invIA = pData->aInvIs[islandIndexA];
    dmMtx invIB = pData->aInvIs[islandIndexB];

    dm_vec4 invMassA = vdupq_n_f32(pBase->m_invMassA);
    dm_vec4 invMassB = vdupq_n_f32(pBase->m_invMassB);

    // Point to point constraint.
    {
        dm_vec4 Cdot = vB + dmMul33(m_Jb, wB) - vA - dmMul33(m_Ja, wA);
        dm_vec4 impulse = dmMul33(m_pivotMass, -Cdot);

        vA -= invMassA * impulse;
        wA -= dmMul33(invIA, dmMulT33(m_Ja, impulse));
        vB += invMassB * impulse;
        wB += dmMul33(invIB, dmMulT33(m_Jb, impulse));

        m_pivotImpulse += impulse;
    }

    // Friction.
    if (m_frictionTorque > 0.0f)
    {
        dm_vec4 Cdot = wB - wA;
        dm_vec4 impulse = dmMul33(m_frictionMass, -Cdot);

        dm_vec4 newImpulse = m_frictionImpulse + impulse;
        dm_vec4 maxImpulse = vmulq_n_f32(pData->step.dt, m_frictionTorque);
        if (dmGT(dmLengthSqr3(newImpulse), maxImpulse * maxImpulse) && dmIsZeroSafe3(newImpulse) == false)
        {
            newImpulse = maxImpulse * dmNormalize3(newImpulse);
        }

        impulse = newImpulse - m_frictionImpulse;
        m_frictionImpulse = newImpulse;

        wA -= dmMul33(invIA, impulse);
        wB += dmMul33(invIB, impulse);
    }

    pData->aVelocities[islandIndexA].v = vA;
    pData->aVelocities[islandIndexA].w = wA;
    pData->aVelocities[islandIndexB].v = vB;
    pData->aVelocities[islandIndexB].w = wB;
}


void dmSphericalJoint::SolvePositionConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 xA = pData->aPositions[islandIndexA].x;
    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 xB = pData->aPositions[islandIndexB].x;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;

    dm_vec4 invMassA = vdupq_n_f32(pBase->m_invMassA);
    dm_vec4 invMassB = vdupq_n_f32(pBase->m_invMassB);

    {

        dmMtx Ra = dmMtxFromQuat(qA);
        dmMtx Rb = dmMtxFromQuat(qB);


        dm_vec4 rLocalA = m_localAnchorA - m_localCenterA;
        dm_vec4 rLocalB = m_localAnchorB - m_localCenterB;


        dm_vec4 rA = dmMul33(Ra, rLocalA);
        dm_vec4 rB = dmMul33(Rb, rLocalB);
        dm_vec4 C = xB - xA + rB - rA;


        dmMtx Sa = dmMtxSetSkew(rLocalA);
        dmMtx Sb = dmMtxSetSkew(rLocalB);


        dmMtx Ga = dmMul33(Ra, Sa);
        dmMtx Gb = dmMul33(Rb, Sb);


        dm_vec4 sumInvMass = invMassA + invMassB;
        m_pivotMass = dmMulRART(Ga, pBase->m_localInvInertiaA) + dmMulRART(Gb, pBase->m_localInvInertiaB);
        m_pivotMass.rx += sumInvMass * dm_vec4_unitX;
        m_pivotMass.ry += sumInvMass * dm_vec4_unitY;
        m_pivotMass.rz += sumInvMass * dm_vec4_unitZ;


        dm_vec4 impulse = dmSolveSym33(m_pivotMass, -C);

        xA -= invMassA * impulse;
        xB += invMassB * impulse;



        dm_vec4 wLocalA = -dmMul33(pBase->m_localInvInertiaA, dmMulT33(Ga, impulse));
        qA -= dm_vec4_oneHalf * dmMulQV(qA, wLocalA);
        qA = dmNormalize4(qA);

        dm_vec4 wLocalB = -dmMul33(pBase->m_localInvInertiaB, dmMulT33(Gb, impulse));
        qB += dm_vec4_oneHalf * dmMulQV(qB, wLocalB);
        qB = dmNormalize4(qB);
    }

    pData->aPositions[islandIndexA].x = xA;
    pData->aPositions[islandIndexA].q = qA;
    pData->aPositions[islandIndexB].x = xB;
    pData->aPositions[islandIndexB].q = qB;
}
