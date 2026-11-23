// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmDistanceJoint.cpp

#include "dmDistanceJoint.h"
#include "dmJoint.h"
#include "../dmBody.h"
#include "../dmSolver.h"

// Scalar form of dm_vec4_linearSlop (0.5 cm in feet). The name is invented;
// the DWARF keeps no macros and the code folds the value to an immediate.
#define dm_linearSlop (0.005f / 0.3048f)
























































void dmDistanceJoint::Create(const dmDistanceJointDef * pDef)
{
    m_localAnchorA = pDef->m_localAnchorA;
    m_localAnchorB = pDef->m_localAnchorB;
    m_distance = dmV4(pDef->m_distance);
    m_impulse = vdupq_n_f32(0.0f);
}


void dmDistanceJoint::InitVelocityConstraints(const dmBody * pBodyA, const dmBody * pBodyB, const dmJoint * pBase, const dmSolverData * pData)
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


    dm_vec4 xA = pBodyA->m_sweep.m_center;
    dm_vec4 xB = pBodyB->m_sweep.m_center;
    dm_vec4 rA = dmRotate(qA, m_localAnchorA - m_localCenterA);
    dm_vec4 rB = dmRotate(qB, m_localAnchorB - m_localCenterB);
    // Handle singularity.
    dm_vec4 linearSlop = vdupq_n_f32(dm_linearSlop);

    m_u = xB - xA + (rB - rA);

    if (dmGT(dmLengthSqr3(m_u), linearSlop * linearSlop))
    {
        m_u = dmNormalize3(m_u);
        m_Ja = dmCross(rA, m_u);
        m_Jb = dmCross(rB, m_u);

        // Compute effective mass.
        m_IJa = dmMul33(invIA, m_Ja);
        m_IJb = dmMul33(invIB, m_Jb);

        dm_vec4 K = invMassA + invMassB + dmDot3(m_Ja, m_IJa) + dmDot3(m_Jb, m_IJb);
        m_mass = dmSafeReciprocal(K);
    }
    else
    {

        m_u = vdupq_n_f32(0.0f);
        m_Ja = vdupq_n_f32(0.0f);
        m_Jb = vdupq_n_f32(0.0f);
        m_IJa = vdupq_n_f32(0.0f);
        m_IJb = vdupq_n_f32(0.0f);
        m_mass = vdupq_n_f32(0.0f);
    }

    if (pData->step.useWarmStarting)
    {
        dm_vec4 ratio = pData->step.dtRatio;
        m_impulse *= ratio;

        dm_vec4 P = m_impulse * m_u;

        pData->aVelocities[islandIndexA].v -= invMassA * P;
        pData->aVelocities[islandIndexA].w -= m_impulse * m_IJa;

        pData->aVelocities[islandIndexB].v += invMassB * P;
        pData->aVelocities[islandIndexB].w += m_impulse * m_IJb;
    }
    else
    {
        m_impulse = vdupq_n_f32(0.0f);
    }
}


void dmDistanceJoint::SolveVelocityConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 invMassA = vdupq_n_f32(pBase->m_invMassA);
    dm_vec4 invMassB = vdupq_n_f32(pBase->m_invMassB);

    dm_vec4 vA = pData->aVelocities[islandIndexA].v;
    dm_vec4 wA = pData->aVelocities[islandIndexA].w;
    dm_vec4 vB = pData->aVelocities[islandIndexB].v;
    dm_vec4 wB = pData->aVelocities[islandIndexB].w;

    {
        // Cdot = dot(u, v + cross(w, r))
        dm_vec4 Cdot = dmDot3(vB - vA, m_u) + dmDot3(wB, m_Jb) - dmDot3(wA, m_Ja);
        dm_vec4 impulse = -m_mass * Cdot;
        m_impulse += impulse;

        dm_vec4 P = impulse * m_u;

        vA -= invMassA * P;
        wA -= impulse * m_IJa;

        vB += invMassB * P;
        wB += impulse * m_IJb;
    }

    pData->aVelocities[islandIndexA].v = vA;
    pData->aVelocities[islandIndexA].w = wA;
    pData->aVelocities[islandIndexB].v = vB;
    pData->aVelocities[islandIndexB].w = wB;
}


void dmDistanceJoint::SolvePositionConstraints(dmSolverData * pData, const dmJoint * pBase)
{
    dm_int32 islandIndexA = pBase->m_islandIndexA;
    dm_int32 islandIndexB = pBase->m_islandIndexB;

    dm_vec4 invMassA = vdupq_n_f32(pBase->m_invMassA);
    dm_vec4 invMassB = vdupq_n_f32(pBase->m_invMassB);

    dm_vec4 xA = pData->aPositions[islandIndexA].x;
    dm_vec4 qA = pData->aPositions[islandIndexA].q;
    dm_vec4 xB = pData->aPositions[islandIndexB].x;
    dm_vec4 qB = pData->aPositions[islandIndexB].q;


    dm_vec4 rA = dmRotate(qA, m_localAnchorA - m_localCenterA);
    dm_vec4 rB = dmRotate(qB, m_localAnchorB - m_localCenterB);

    dm_vec4 u = xB - xA + (rB - rA);

    dm_vec4 distance = dmLength3(u);
    dm_vec4 linearSlop = vdupq_n_f32(dm_linearSlop);
    // Already within tolerance.
    if (dmLT(distance, linearSlop))
    {
        return;
    }

    u = dmNormalize3(u);
    dm_vec4 Ja = dmCross(rA, u);
    dm_vec4 Jb = dmCross(rB, u);

    // The world inverse inertia is not available here, so rotate through the local one.

    dm_vec4 Ba = dmRotate(qA, dmMul33(pBase->m_localInvInertiaA, dmInvRotate(qA, Ja)));
    dm_vec4 Bb = dmRotate(qB, dmMul33(pBase->m_localInvInertiaB, dmInvRotate(qB, Jb)));

    // Compute effective mass.

    dm_vec4 K = invMassA + invMassB + dmDot3(Ja, Ba) + dmDot3(Jb, Bb);
    m_mass = dmSafeReciprocal(K);

    dm_vec4 C = distance - m_distance;
    dm_vec4 impulse = -m_mass * C;

    // Apply impulse.
    dm_vec4 P = impulse * u;

    xA -= invMassA * P;
    dm_vec4 wA = impulse * Ba;
    qA -= dm_vec4_oneHalf * dmMulVQ(wA, qA);
    qA = dmNormalize4(qA);

    xB += invMassB * P;
    dm_vec4 wB = impulse * Bb;
    qB += dm_vec4_oneHalf * dmMulVQ(wB, qB);
    qB = dmNormalize4(qB);

    pData->aPositions[islandIndexA].x = xA;
    pData->aPositions[islandIndexA].q = qA;
    pData->aPositions[islandIndexB].x = xB;
    pData->aPositions[islandIndexB].q = qB;
}
