#include "Contrib/Contrib/Domino/Common/dmStackAllocator.h"
#include "Contrib/Contrib/Domino/Dynamics/Contacts/dmContactSolver.h"
#include "Contrib/Contrib/Domino/Dynamics/Contacts/dmContact.h"
#include "Contrib/Contrib/Domino/Dynamics/dmSolver.h"
#include "Contrib/Contrib/Domino/Dynamics/dmWorld.h"
#include "Contrib/Contrib/Domino/Dynamics/dmBody.h"

// 0.5 cm in feet; the name is invented (see Collision/Functions/dmManifold.cpp).
#define dm_linearSlop (0.005f / 0.3048f)

























































void dmContactSolver::Create(dmContactSolverDef * pDef, dmStackAllocator * pAllocator)
{
    m_aContacts = pDef->aContacts;
    m_contactCount = pDef->contactCount;

    m_constraintCount = 0;
    m_pointCount = 0;
    for (dm_int32 i = 0; i < m_contactCount; ++i)
    {
        dm_int32 index = m_aContacts[i];
        dmContact * pContact = pDef->pWorld->m_contacts.Get(index);

        m_constraintCount += pContact->m_manifoldCount;

        dmManifold * aManifolds = pContact->m_aManifolds;
        for (dm_int32 j = 0; j < pContact->m_manifoldCount; ++j)
        {
            dmManifold * pManifold = aManifolds + j;

            m_pointCount += pManifold->m_pointCount;
        }
    }

    m_aVelocityConstraints = (dmContactVelocityConstraint *)pAllocator->Allocate(m_constraintCount * sizeof(dmContactVelocityConstraint));
    m_aPositionConstraints = (dmContactPositionConstraint *)pAllocator->Allocate(m_constraintCount * sizeof(dmContactPositionConstraint));
    m_aVelocityConstraintPoints = (dmVelocityConstraintPoint *)pAllocator->Allocate(m_pointCount * sizeof(dmVelocityConstraintPoint));
    m_aPositionConstraintPoints = (dmPositionConstraintPoint *)pAllocator->Allocate(m_pointCount * sizeof(dmPositionConstraintPoint));
}


void dmContactSolver::Destroy(dmStackAllocator * pAllocator)
{
    pAllocator->Free(m_aPositionConstraintPoints);
    pAllocator->Free(m_aVelocityConstraintPoints);
    pAllocator->Free(m_aPositionConstraints);
    pAllocator->Free(m_aVelocityConstraints);

    m_aPositionConstraintPoints = 0;
    m_aVelocityConstraintPoints = 0;
    m_aPositionConstraints = 0;
    m_aVelocityConstraints = 0;
    m_constraintCount = 0;
    m_pointCount = 0;
}



void dmContactSolver::Setup(const dmSolverData * pData)
{
    dmVelocity * aVelocities = pData->aVelocities;
    const dmMtx * aInvIs = pData->aInvIs;
    dm_vec4 dt = pData->step.dt;
    dm_vec4 dtRatio = pData->step.dtRatio;
    bool warmStart = pData->step.useWarmStarting;

    // Soft contact parameters
    const dm_vec4 hz = dmVecSet(10.0f);
    const dm_vec4 zeta = dmVecSet(0.7f);
    const dm_vec4 softRatio = dmVecSet(10.0f);
    const dm_vec4 maxC = dmVecSet(0.25f);
    const dm_vec4 threshold = dmVecSet(1.0f / 0.3048f);
    dm_int32 count = 0;
    dm_int32 pointIndex = 0;
    for (dm_int32 i = 0; i < m_contactCount; ++i)
    {
        dm_int32 contactIndex = m_aContacts[i];
        dmContact * pContact = pData->pContactArrayPtr + contactIndex;

        dm_int32 fixtureIndexA = pContact->m_edges[0].m_fixtureIndex;
        dm_int32 fixtureIndexB = pContact->m_edges[1].m_fixtureIndex;

        dm_int32 ownerIndexA = pContact->m_edges[0].m_ownerIndex;
        dm_int32 ownerIndexB = pContact->m_edges[1].m_ownerIndex;
        const dmBody * pBodyA = pData->pBodyArrayPtr + ownerIndexA;
        const dmBody * pBodyB = pData->pBodyArrayPtr + ownerIndexB;

        dm_vec4 vA0 = pBodyA->m_linearVelocity;
        dm_vec4 wA0 = pBodyA->m_angularVelocity;
        dm_vec4 vB0 = pBodyB->m_linearVelocity;
        dm_vec4 wB0 = pBodyB->m_angularVelocity;

        dm_int32 indexA = pContact->m_islandIndexA;
        dm_int32 indexB = pContact->m_islandIndexB;

        dmBodyType typeA = pBodyA->m_bodyType;
        dmBodyType typeB = pBodyB->m_bodyType;

        dm_vec4 qA = pBodyA->m_transform.q;
        dm_vec4 qB = pBodyB->m_transform.q;

        dm_vec4 centerA = pBodyA->m_sweep.m_center;
        dm_vec4 centerB = pBodyB->m_sweep.m_center;

        dmMtx invInertiaA_Local = pBodyA->m_invI;
        dmMtx invInertiaA = aInvIs[indexA];
        dmMtx invInertiaB_Local = pBodyB->m_invI;
        dmMtx invInertiaB = aInvIs[indexB];

        dm_vec4 invMass = dmVecSet(pBodyA->m_invMass, pBodyB->m_invMass, pBodyA->m_mass, pBodyB->m_mass);

        dm_vec4 invMassA = dmSplatX(invMass);
        dm_vec4 invMassB = dmSplatY(invMass);
        dm_vec4 massA = dmSplatZ(invMass);
        dm_vec4 massB = dmSplatW(invMass);

        bool soft = false;
        if (typeA == dm_dynamicBody && typeB == dm_dynamicBody)
        {
            soft = dmGT(massA, massB * softRatio) || dmGT(massB, massA * softRatio);
        }
        soft = soft || typeA == dm_kinematicBody;

        dm_int32 manifoldCount = pContact->m_manifoldCount;
        dmManifold * aManifolds = pContact->m_aManifolds;

        dm_vec4 rawMaterial = dmVecSet(pContact->m_friction, pContact->m_restitution, pContact->m_rollingResistance, 0.0f);
        dm_vec4 friction = dmSplatX(rawMaterial);
        dm_vec4 restitution = dmSplatY(rawMaterial);
        dm_vec4 rollingResistance = dmSplatZ(rawMaterial);

        dm_vec4 vA = aVelocities[indexA].v;
        dm_vec4 wA = aVelocities[indexA].w;
        dm_vec4 vB = aVelocities[indexB].v;
        dm_vec4 wB = aVelocities[indexB].w;
        for (dm_int32 j = 0; j < manifoldCount; ++j)
        {
            dmManifold * pManifold = aManifolds + j;
            dm_int32 pointCount = pManifold->m_pointCount;
            const dm_vec4 normal = pManifold->m_normal;

            dmContactVelocityConstraint * vc = m_aVelocityConstraints + count;
            dmContactPositionConstraint * pc = m_aPositionConstraints + count;
            vc->indexA = indexA;
            vc->indexB = indexB;

            vc->invMass = invMass;


            vc->invInertiaA = invInertiaA;
            vc->invInertiaB = invInertiaB;

            vc->normal = normal;
            vc->pManifold = pManifold;

            vc->pointCount = pointCount;

            vc->fixtureIndexA = fixtureIndexA;
            vc->fixtureIndexB = fixtureIndexB;

            pc->indexA = indexA;
            pc->indexB = indexB;
            pc->normal = normal;
            pc->pointCount = pointCount;
            if (soft)
            {
                // Soft contacts are not position corrected.
                pc->pointCount = 0;
            }




            pc->invInertiaA_Local = invInertiaA_Local;
            pc->invInertiaB_Local = invInertiaB_Local;
            pc->mass = invMass;

            dmVelocityConstraintPoint * pFirstVelocity = m_aVelocityConstraintPoints + pointIndex;

            dmPositionConstraintPoint * pFirstPosition = m_aPositionConstraintPoints + pointIndex;

            vc->firstPointIndex = pointIndex;
            pc->firstPointIndex = pointIndex;

            pointIndex += pointCount;

            dm_vec4 invPointCount = dmVecSet(1.0f / pointCount);
            dm_vec4 tangentAnchor = vdupq_n_f32(0.0f);
            for (dm_int32 k = 0; k < pointCount; ++k)
            {
                dmManifoldPoint * mp = pManifold->m_points + k;
                dmVelocityConstraintPoint * vcp = pFirstVelocity + k;
                dmPositionConstraintPoint * pcp = pFirstPosition + k;

                vcp->mass_gamma_beta_impulse = vdupq_n_f32(0.0f);
                vcp->results = vdupq_n_f32(0.0f);
                pcp->sep_emass = vdupq_n_f32(0.0f);

                dm_vec4 separation = dmVecSet(mp->separation);
                pcp->sep_emass = dmSetX(pcp->sep_emass, separation);

                // Anchors relative to the centers of mass
                dm_vec4 rA = mp->point - centerA;
                dm_vec4 rB = mp->point - centerB;
                pcp->localAnchorA = dmInvRotate(qA, rA);
                pcp->localAnchorB = dmInvRotate(qB, rB);

                tangentAnchor = tangentAnchor + mp->point;

                // Normal Jacobians
                vcp->Ja = dmCross(rA, normal);
                vcp->Jb = dmCross(rB, normal);
                vcp->Ba = dmMul33(invInertiaA, vcp->Ja);
                vcp->Bb = dmMul33(invInertiaB, vcp->Jb);

                dm_vec4 massIa = dmDot3(vcp->Ja, vcp->Ba);
                dm_vec4 massIb = dmDot3(vcp->Jb, vcp->Bb);

                dm_vec4 kNormal = invMassA + invMassB + massIa + massIb;
                dm_vec4 normalMass = dmSafeReciprocal(kNormal);

                dm_vec4 bias = vdupq_n_f32(0.0f);
                if (soft)
                {
                    dm_vec4 omega = dm_vec4_2pi * hz;
                    dm_vec4 C = dmMax(separation, -maxC);

                    dm_vec4 d = dm_vec4_two * normalMass * zeta * omega;
                    dm_vec4 ks = normalMass * omega * omega;

                    dm_vec4 gamma = dmSafeReciprocal(dt * (d + dt * ks));
                    bias = C * (dt * ks * gamma);

                    vcp->mass_gamma_beta_impulse = dmSetY(vcp->mass_gamma_beta_impulse, gamma);
                    vcp->mass_gamma_beta_impulse = dmSetZ(vcp->mass_gamma_beta_impulse, bias);

                    normalMass = dmSafeReciprocal(kNormal + gamma);
                }

                vcp->mass_gamma_beta_impulse = dmSetX(vcp->mass_gamma_beta_impulse, normalMass);

                dm_vec4 equalizedMass = dmSafeReciprocal(massA * (invMassA + massIa) + massB * (invMassB + massIb));
                pcp->sep_emass = dmSetY(pcp->sep_emass, equalizedMass);

                // Relative velocity at contact

                dm_vec4 vRel = vB0 + dmCross(wB0, rB) - vA0 - dmCross(wA0, rA);
                dm_vec4 vn = dmDot3(normal, vRel);
                if (dmLTZ(vn + threshold))
                {
                    bias = bias + restitution * vn;
                    vcp->mass_gamma_beta_impulse = dmSetZ(vcp->mass_gamma_beta_impulse, bias);
                }




                if (warmStart)
                {
                    dm_vec4 normalImpulse = dtRatio * dmVecSet(mp->normalImpulse);
                    vcp->mass_gamma_beta_impulse = dmSetW(vcp->mass_gamma_beta_impulse, normalImpulse);

                    dm_vec4 P = normal * normalImpulse;
                    vA = vA - invMassA * P;
                    wA = wA - normalImpulse * vcp->Ba;
                    vB = vB + invMassB * P;
                    wB = wB + normalImpulse * vcp->Bb;
                }

                // Diagnostics




                dm_vec4 vt = vRel - normal * vn;

                dm_vec4 isNew = mp->isNew ? dm_vec4_one : vdupq_n_f32(0.0f);


                vcp->results = dmVecSet(isNew, vn, dmLength3(vt));
            }

            tangentAnchor = invPointCount * tangentAnchor;

            vc->tangentAnchor = tangentAnchor;

            dm_vec4 tangentAnchorA = tangentAnchor - centerA;
            dm_vec4 tangentAnchorB = tangentAnchor - centerB;

            // Central friction
            dmComputeAlignedBasis(&vc->tangent1, &vc->tangent2, normal);


            vc->Jt1A = dmCross(tangentAnchorA, vc->tangent1);
            vc->Jt1B = dmCross(tangentAnchorB, vc->tangent1);
            vc->Jt2A = dmCross(tangentAnchorA, vc->tangent2);
            vc->Jt2B = dmCross(tangentAnchorB, vc->tangent2);
            vc->Bt1A = dmMul33(invInertiaA, vc->Jt1A);
            vc->Bt1B = dmMul33(invInertiaB, vc->Jt1B);
            vc->Bt2A = dmMul33(invInertiaA, vc->Jt2A);
            vc->Bt2B = dmMul33(invInertiaB, vc->Jt2B);

            // Tangent mass
            dm_vec4 m11 = invMassA + invMassB + dmDot3(vc->Jt1A, vc->Bt1A) + dmDot3(vc->Jt1B, vc->Bt1B);
            dm_vec4 m12 = dmDot3(vc->Jt1A, vc->Bt2A) + dmDot3(vc->Jt1B, vc->Bt2B);
            dm_vec4 m22 = invMassA + invMassB + dmDot3(vc->Jt2A, vc->Bt2A) + dmDot3(vc->Jt2B, vc->Bt2B);
            dm_vec4 det = m11 * m22 - m12 * m12;
            det = dmSafeReciprocal(det);
            dm_vec4 mt11 = m22 * det;
            dm_vec4 mt12 = -m12 * det;
            dm_vec4 mt22 = m11 * det;

            // Twist mass
            dm_vec4 twistMass1 = dmDot3(normal, dmMul33(vc->invInertiaA, normal));
            dm_vec4 twistMass2 = dmDot3(normal, dmMul33(vc->invInertiaB, normal));
            dm_vec4 twistMass = dmSafeReciprocal(twistMass1 + twistMass2);


            vc->meff = dmVecSet(mt11, mt12, mt22, twistMass);

            // Twist friction radius
            dm_vec4 twistRadius = vdupq_n_f32(0.0f);
            for (dm_int32 k = 0; k < pointCount; ++k)
            {
                dm_vec4 r = pManifold->m_points[k].point - tangentAnchor;
                twistRadius = twistRadius + dmDot3(r, r);
            }
            twistRadius = dmSqrt(invPointCount * twistRadius);
            dm_vec4 twistFriction = dm_vec4_oneHalf * friction * twistRadius;

            vc->material = dmVecSet(friction, twistFriction, rollingResistance);

            // Rolling resistance
            if (dmGTZ(rollingResistance))
            {
                vc->rollingMass = dmInvertSym33(vc->invInertiaA + vc->invInertiaB);
            }
            else
            {
                vc->rollingMass.rx = vdupq_n_f32(0.0f);
                vc->rollingMass.ry = vdupq_n_f32(0.0f);
                vc->rollingMass.rz = vdupq_n_f32(0.0f);
                vc->rollingMass.p = vdupq_n_f32(0.0f);
            }

            if (warmStart)
            {
                dm_vec4 tangentImpulse1 = dtRatio * dmDot3(pManifold->m_tangentImpulse, vc->tangent1);
                dm_vec4 tangentImpulse2 = dtRatio * dmDot3(pManifold->m_tangentImpulse, vc->tangent2);
                dm_vec4 twistImpulse = dtRatio * dmVecSet(pManifold->m_twistImpulse);
                vc->impulse = dmVecSet(tangentImpulse1, tangentImpulse2, twistImpulse);
                vc->rollingImpulse = dtRatio * pManifold->m_rollingImpulse;
                dm_vec4 P = tangentImpulse1 * vc->tangent1 + tangentImpulse2 * vc->tangent2;
                dm_vec4 LA = tangentImpulse1 * vc->Jt1A + tangentImpulse2 * vc->Jt2A + twistImpulse * normal + vc->rollingImpulse;
                dm_vec4 LB = tangentImpulse1 * vc->Jt1B + tangentImpulse2 * vc->Jt2B + twistImpulse * normal + vc->rollingImpulse;

                vA = vA - invMassA * P;
                wA = wA - dmMul33(invInertiaA, LA);
                vB = vB + invMassB * P;
                wB = wB + dmMul33(invInertiaB, LB);
            }
            else
            {


                vc->impulse = vdupq_n_f32(0.0f);
                vc->rollingImpulse = vdupq_n_f32(0.0f);
            }

            ++count;
        }

        pData->aVelocities[indexA].v = vA;
        pData->aVelocities[indexA].w = wA;
        pData->aVelocities[indexB].v = vB;
        pData->aVelocities[indexB].w = wB;
    }



}


void dmContactSolver::SolveVelocityConstraints(const dmSolverData * pData)
{
    dmVelocity * aVelocities = pData->aVelocities;

    const dm_int32 constraintCount = m_constraintCount;
    for (dm_int32 i = 0; i < constraintCount; ++i)
    {
        dmContactVelocityConstraint * vc = m_aVelocityConstraints + i;
        dm_int32 indexA = vc->indexA;
        dm_int32 indexB = vc->indexB;
        const dm_vec4 normal = vc->normal;
        const dmMtx invInertiaA = vc->invInertiaA;
        const dmMtx invInertiaB = vc->invInertiaB;
        const dm_vec4 invMassA = dmSplatX(vc->invMass);
        const dm_vec4 invMassB = dmSplatY(vc->invMass);

        dm_vec4 vA = aVelocities[indexA].v;
        dm_vec4 wA = aVelocities[indexA].w;
        dm_vec4 vB = aVelocities[indexB].v;
        dm_vec4 wB = aVelocities[indexB].w;

        // Sum the normal impulses for the friction limits.
        dm_vec4 totalNormalImpulse = vdupq_n_f32(0.0f);


        for (dm_int32 j = 0; j < vc->pointCount; ++j)
        {
            const dmVelocityConstraintPoint * vcp = m_aVelocityConstraintPoints + vc->firstPointIndex + j;
            dm_vec4 normalImpulse = dmSplatW(vcp->mass_gamma_beta_impulse);
            totalNormalImpulse = totalNormalImpulse + normalImpulse;
        }

        // Rolling resistance
        {
            dm_vec4 impulse = dmMul33(vc->rollingMass, wA - wB);
            dm_vec4 newImpulse = vc->rollingImpulse + impulse;
            dm_vec4 rollingResistance = dmSplatZ(vc->material);
            dm_vec4 maxRolling = rollingResistance * totalNormalImpulse;

            // Clamp the rolling impulse to a sphere.
            dm_vec4 newImpulseSqr = dmLengthSqr3(newImpulse);
            dm_vec4 maxCompare = dmCompareGT(newImpulseSqr, maxRolling * maxRolling);
            dm_vec4 zeroCompare = dmCompareGT(newImpulseSqr, dm_vec4_zeroSafe);
            newImpulse = dmSelect(dmAnd(maxCompare, zeroCompare), maxRolling * dmNormalize3(newImpulse, newImpulseSqr), newImpulse);
            impulse = newImpulse - vc->rollingImpulse;


            wA = wA - dmMul33(invInertiaA, impulse);
            wB = wB + dmMul33(invInertiaB, impulse);


            vc->rollingImpulse = newImpulse;
        }

        // Friction at the manifold center
        {




            dm_vec4 tangent1 = vc->tangent1;
            dm_vec4 tangent2 = vc->tangent2;

            // Relative tangent velocities
            dm_vec4 vt1 = dmDot3(vB - vA, tangent1) + dmDot3(vc->Jt1B, wB) - dmDot3(vc->Jt1A, wA);
            dm_vec4 vt2 = dmDot3(vB - vA, tangent2) + dmDot3(vc->Jt2B, wB) - dmDot3(vc->Jt2A, wA);

            // Solve the 2x2 tangent block
            dm_vec4 mt11 = dmSplatX(vc->meff);
            dm_vec4 mt12 = dmSplatY(vc->meff);
            dm_vec4 mt22 = dmSplatZ(vc->meff);

            dm_vec4 impulse = vdupq_n_f32(0.0f);
            impulse = dmSetX(impulse, -(mt11 * vt1 + mt12 * vt2));
            impulse = dmSetY(impulse, -(mt12 * vt1 + mt22 * vt2));

            dm_vec4 tangentImpulse1 = dmSplatX(vc->impulse);
            dm_vec4 tangentImpulse2 = dmSplatY(vc->impulse);

            dm_vec4 tangentImpulse = vdupq_n_f32(0.0f);
            tangentImpulse = dmSetX(tangentImpulse, tangentImpulse1);
            tangentImpulse = dmSetY(tangentImpulse, tangentImpulse2);

            // Clamp the accumulated impulse to the friction disk.
            dm_vec4 friction = dmSplatX(vc->material);
            dm_vec4 maxFriction = friction * totalNormalImpulse;
            dm_vec4 newImpulse = tangentImpulse + impulse;


            dm_vec4 newImpulseSqr = dmLengthSqr3(newImpulse);
            dm_vec4 maxCompare = dmCompareGT(newImpulseSqr, maxFriction * maxFriction);
            dm_vec4 zeroCompare = dmCompareGT(newImpulseSqr, dm_vec4_zeroSafe);
            newImpulse = dmSelect(dmAnd(maxCompare, zeroCompare), maxFriction * dmNormalize3(newImpulse, newImpulseSqr), newImpulse);
            impulse = newImpulse - tangentImpulse;

            dm_vec4 impulseX = dmSplatX(impulse);
            dm_vec4 impulseY = dmSplatY(impulse);

            dm_vec4 P = tangent1 * impulseX + tangent2 * impulseY;
            dm_vec4 LA = vc->Bt1A * impulseX + vc->Bt2A * impulseY;
            dm_vec4 LB = vc->Bt1B * impulseX + vc->Bt2B * impulseY;

            vA = vA - invMassA * P;
            wA = wA - LA;
            vB = vB + invMassB * P;
            wB = wB + LB;

            dm_vec4 newImpulseX = dmSplatX(newImpulse);
            dm_vec4 newImpulseY = dmSplatY(newImpulse);



            vc->impulse = dmSetY(dmSetX(vc->impulse, newImpulseX), newImpulseY);
        }

        // Twist friction
        {


            dm_vec4 twistFriction = dmSplatY(vc->material);

            dm_vec4 Cdot = dmDot3(normal, wB - wA);
            dm_vec4 twistMass = dmSplatW(vc->meff);
            dm_vec4 impulse = twistMass * Cdot;

            dm_vec4 maxImpulse = twistFriction * totalNormalImpulse;
            dm_vec4 twistImpulse = dmSplatZ(vc->impulse);
            dm_vec4 newImpulse = dmClamp(twistImpulse - impulse, -maxImpulse, maxImpulse);
            impulse = newImpulse - twistImpulse;
            dm_vec4 L = normal * impulse;


            wA = wA - dmMul33(invInertiaA, L);
            wB = wB + dmMul33(invInertiaB, L);


            vc->impulse = dmSetZ(vc->impulse, newImpulse);
        }

        // Normal constraints

        const dm_int32 pointCount = vc->pointCount;


        for (dm_int32 j = 0; j < pointCount; ++j)
        {
            dmVelocityConstraintPoint * vcp = m_aVelocityConstraintPoints + vc->firstPointIndex + j;
            dm_vec4 normalMass = dmSplatX(vcp->mass_gamma_beta_impulse);
            dm_vec4 gamma = dmSplatY(vcp->mass_gamma_beta_impulse);
            dm_vec4 bias = dmSplatZ(vcp->mass_gamma_beta_impulse);
            dm_vec4 normalImpulse = dmSplatW(vcp->mass_gamma_beta_impulse);

            // Relative normal velocity


            dm_vec4 Cdot = normal * (vB - vA) + wB * vcp->Jb - wA * vcp->Ja;
            Cdot = dmSplatX(Cdot) + dmSplatY(Cdot) + dmSplatZ(Cdot);

            // Soft constraint impulse
            dm_vec4 impulse = normalMass * (Cdot + bias + gamma * normalImpulse);

            // Clamp the accumulated impulse
            dm_vec4 newImpulse = dmMax(normalImpulse - impulse, vdupq_n_f32(0.0f));
            impulse = newImpulse - normalImpulse;

            // Apply contact impulse
            dm_vec4 P = normal * impulse;
            vA = vA - invMassA * P;
            wA = wA - impulse * vcp->Ba;
            vB = vB + invMassB * P;
            wB = wB + impulse * vcp->Bb;

            vcp->mass_gamma_beta_impulse = dmSetW(vcp->mass_gamma_beta_impulse, newImpulse);
        }






        aVelocities[indexA].v = vA;
        aVelocities[indexA].w = wA;
        aVelocities[indexB].v = vB;
        aVelocities[indexB].w = wB;
    }
}


void dmContactSolver::StoreImpulses()
{
    for (dm_int32 i = 0; i < m_constraintCount; ++i)
    {
        dmContactVelocityConstraint * vc = m_aVelocityConstraints + i;
        dmManifold * mp = vc->pManifold;

        for (dm_int32 j = 0; j < vc->pointCount; ++j)
        {
            const dmVelocityConstraintPoint * vcp = m_aVelocityConstraintPoints + vc->firstPointIndex + j;
            mp->m_points[j].normalImpulse = dmGetW(vcp->mass_gamma_beta_impulse);
        }

        mp->m_tangentImpulse = vc->tangent1 * dmSplatX(vc->impulse) + vc->tangent2 * vc->impulse[1];
        mp->m_twistImpulse = dmGetW(vc->impulse);
        mp->m_rollingImpulse = vc->rollingImpulse;
    }
}





bool dmContactSolver::SolvePositionConstraints(const dmSolverData * pData, dm_vec4Arg baumgarte)
{
    dmPosition * aPositions = pData->aPositions;
    const dm_vec4 slop = dmVecSet(dm_linearSlop);

    // We only need positive separation to be considered solved.
    dm_vec4 minSeparation = dmVecSet(0.0f);
    for (dm_int32 i = 0; i < m_constraintCount; ++i)
    {
        dmContactPositionConstraint * pc = m_aPositionConstraints + i;

        dm_int32 indexA = pc->indexA;
        dm_int32 indexB = pc->indexB;

        dm_vec4 massA = dmSplatZ(pc->mass);
        dm_vec4 massB = dmSplatW(pc->mass);
        dmMtx invIA;
        dmMtx invIB;
        invIA.rx = massA * pc->invInertiaA_Local.rx;
        invIA.ry = massA * pc->invInertiaA_Local.ry;
        invIA.rz = massA * pc->invInertiaA_Local.rz;

        invIB.rx = massB * pc->invInertiaB_Local.rx;
        invIB.ry = massB * pc->invInertiaB_Local.ry;
        invIB.rz = massB * pc->invInertiaB_Local.rz;

        dm_vec4 invMassA = dmSplatX(pc->mass) * massA;
        dm_vec4 invMassB = dmSplatY(pc->mass) * massB;

        dm_vec4 normal = pc->normal;

        dm_vec4 xA = aPositions[indexA].x;
        dm_vec4 qA = aPositions[indexA].q;
        dm_vec4 xB = aPositions[indexB].x;
        dm_vec4 qB = aPositions[indexB].q;

        // Solve normal constraints
        for (dm_int32 j = 0; j < pc->pointCount; ++j)
        {
            dmPositionConstraintPoint * pcp = m_aPositionConstraintPoints + pc->firstPointIndex + j;

            dm_vec4 rA = dmRotate(qA, pcp->localAnchorA);
            dm_vec4 rB = dmRotate(qB, pcp->localAnchorB);

            dm_vec4 pA = xA + rA;
            dm_vec4 pB = xB + rB;
            dm_vec4 dp = pB - pA;

            // Current separation
            dm_vec4 separation = dmDot3(normal, dp) + dmSplatX(pcp->sep_emass);

            // Track max constraint error.
            minSeparation = dmMin(minSeparation, separation);

            // Prevent large corrections and allow slop.
            dm_vec4 C = dmMin(baumgarte * (separation + slop), dmVecSet(0.0f));

            // Compute the effective mass.
            dm_vec4 equalizedMass = dmSplatY(pcp->sep_emass);
            dm_vec4 impulse = -equalizedMass * C;

            dm_vec4 P = normal * impulse;

            xA = xA - invMassA * P;
            dm_vec4 wLocalA = dmMul33(invIA, dmInvRotate(qA, dmCross(rA, P)));
            qA = qA - dm_vec4_oneHalf * dmMulQV(qA, wLocalA);
            qA = dmNormalize4(qA);

            xB = xB + invMassB * P;
            dm_vec4 wLocalB = dmMul33(invIB, dmInvRotate(qB, dmCross(rB, P)));
            qB = qB + dm_vec4_oneHalf * dmMulQV(qB, wLocalB);
            qB = dmNormalize4(qB);
        }

        aPositions[indexA].x = xA;
        aPositions[indexA].q = qA;
        aPositions[indexB].x = xB;
        aPositions[indexB].q = qB;
    }

    // We can't expect minSpeparation >= -dm_linearSlop because we don't
    // push the separation above -dm_linearSlop.
    return dmGE(minSeparation, -(dm_vec4_oneHalf * slop + slop));
}
