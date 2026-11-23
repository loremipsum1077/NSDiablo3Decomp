// Reconstructed from the original dmIsland.cpp DWARF and instruction evidence.
#include "dmIsland.h"
#include "../Common/dmStackAllocator.h"
#include "../Include/dmCallbacks.h"
#include "../Include/dmStructures.h"
#include "../Include/dmShapes.h"
#include "dmFixture.h"
#include "dmWorld.h"
#include <string.h>
#include "dmBody.h"
#include "../Common/dmTimer.h"
#include "../Common/dmAtomic.h"



























void dmIsland::Create(dmIslandDef * pDef)
{
    m_worldIndex = pDef->pWorld->m_index;
    m_gravity = pDef->pWorld->m_gravity;
    m_bodyArrayPtr = pDef->pWorld->m_bodies.GetPointer();
    m_jointArrayPtr = pDef->pWorld->m_joints.GetPointer();
    m_fixtureArray = pDef->pWorld->m_fixtures.GetPointer();
    m_contactArrayPtr = pDef->pWorld->m_contacts.GetPointer();
    m_bodyCeiling = pDef->pWorld->m_bodies.GetCeiling();
    m_jointCeiling = pDef->pWorld->m_joints.GetCeiling();
    m_pStackAllocator = &pDef->pWorld->m_stackAllocator;
    m_pContactListener = pDef->pWorld->m_pContactListener;
    m_pSetupContraintTimer = &pDef->pWorld->m_setupContraintTimer;
    m_pVelocityConstraintTimer = &pDef->pWorld->m_velocityConstraintTimer;
    m_pPositionConstraintTimer = &pDef->pWorld->m_positionConstraintTimer;

    m_specificJointArrays.m_distanceJointArrayPtr = pDef->pWorld->m_distanceJoints.GetPointer();
    m_specificJointArrays.m_mouseJointArrayPtr = pDef->pWorld->m_mouseJoints.GetPointer();
    m_specificJointArrays.m_prismaticJointArrayPtr = pDef->pWorld->m_prismaticJoints.GetPointer();
    m_specificJointArrays.m_revoluteJointArrayPtr = pDef->pWorld->m_revoluteJoints.GetPointer();
    m_specificJointArrays.m_shoulderJointArrayPtr = pDef->pWorld->m_shoulderJoints.GetPointer();
    m_specificJointArrays.m_sphericalJointArrayPtr = pDef->pWorld->m_sphericalJoints.GetPointer();
    m_specificJointArrays.m_weldJointArrayPtr = pDef->pWorld->m_weldJoints.GetPointer();

    m_step = pDef->step;

    m_bodyCount = pDef->bodyCount;
    m_contactCount = pDef->contactCount;
    m_jointCount = pDef->jointCount;
    // Inference: the allocator local reuses the pointer formed at line 50 (same x21 in original).
    dmStackAllocator * pAllocator = m_pStackAllocator;
















    // Inference: typed allocation converts the DWARF void* result; shifts encode element sizes.
    m_aBodies = (dm_int32 *)pAllocator->Allocate((m_bodyCount + m_contactCount + m_jointCount) * sizeof(dm_int32));
    m_aContacts = m_aBodies + m_bodyCount;
    m_aJoints = m_aContacts + m_contactCount;


    memcpy(m_aBodies, pDef->aBodies, m_bodyCount * sizeof(dm_int32));
    memcpy(m_aContacts, pDef->aContacts, m_contactCount * sizeof(dm_int32));
    memcpy(m_aJoints, pDef->aJoints, m_jointCount * sizeof(dm_int32));
    // Inference: typed allocation conversions follow DWARF result fields and byte counts 32/32/64.
    m_aPositions = (dmPosition *)pAllocator->Allocate(m_bodyCount * sizeof(dmPosition));
    m_aVelocities = (dmVelocity *)pAllocator->Allocate(m_bodyCount * sizeof(dmVelocity));
    m_aInvIs = (dmMtx *)pAllocator->Allocate(m_bodyCount * sizeof(dmMtx));

    dmContactSolverDef solverDef;
    solverDef.pWorld = pDef->pWorld;
    solverDef.aContacts = m_aContacts;
    solverDef.contactCount = m_contactCount;
    m_contactSolver.Create(&solverDef, pAllocator);
}


void dmIsland::Destroy()
{
    dmStackAllocator * pAllocator = m_pStackAllocator;


    m_contactSolver.Destroy(pAllocator);

    pAllocator->Free(m_aInvIs);
    pAllocator->Free(m_aVelocities);
    pAllocator->Free(m_aPositions);

    pAllocator->Free(m_aBodies);
}

















































































































































































dm_vec4 sSolveGyroscopic(dm_vec4Arg q1, const dmMtx & I, dm_vec4Arg omega, dm_vec4Arg dt)
{
    dm_vec4 w1 = dmInvRotate(q1, omega);
    dm_vec4 w = w1;
    // Inference: i has constant value zero; exactly one Newton iteration survives in the original.
    for (dm_int32 i = 0; i < 1; ++i) {
        // Inference: Newton residual/Jacobian follow the original inline tree and arithmetic order.
        dm_vec4 f = dmMul33(I, w - w1) + dt * dmCross(w, dmMul33(I, w));
        dmMtx J = I + dt * (dmMul33(dmMtxSetSkew(w), I) - dmMtxSetSkew(dmMul33(I, w)));
        dm_vec4 d = dmSolve33(J, f);
        w -= d;
    }
    // DWARF declares w2 without a location; no initializer is recoverable.
    dm_vec4 w2;





    dm_vec4 q2 = q1;








    return dmRotate(q2, w);
}

// NON_MATCHING: 3028/3028 bytes, 18/757 insns differ: loop IVs x25/x26 swapped (0x20 vs 0x40 strides) and one v21/v22 swap inside inlined dmMulVQ; tried else-if chain vs continue.
void dmIsland::Solve()
{




    dmTimer setupTimer;

    const dm_vec4 gravity = m_gravity;
    const dm_vec4 dt = m_step.dt;
    const dm_vec4 inv_dt = m_step.inv_dt;


    for (dm_int32 i = 0; i < m_bodyCount; ++i)
    {







        dm_int32 index = m_aBodies[i];

        dmBody * b = m_bodyArrayPtr + index;


        dm_vec4 x = b->m_sweep.m_center;
        dm_vec4 q = b->m_sweep.m_quaternion;
        m_aPositions[i].x = x;
        m_aPositions[i].q = q;



        // Inference: the static and kinematic tests share line 366 in the original (compare chain folded); kinematic test written on 374.
        if (b->m_bodyType == dm_staticBody)
        {
            m_aVelocities[i].v = dmVecSet(0.0f);
            m_aVelocities[i].w = dmVecSet(0.0f);
            m_aInvIs[i] = dmMtx();
            continue;
        }

        if (b->m_bodyType == dm_kinematicBody)
        {
            m_aVelocities[i].v = b->m_linearVelocity;
            m_aVelocities[i].w = b->m_angularVelocity;
            m_aInvIs[i] = dmMtx();

            // Inference: line 381 stp stores both m_center0 and m_quaternion0.
            b->m_sweep.m_center0 = x;
            b->m_sweep.m_quaternion0 = q;
            continue;
        }


        dmMtx R = dmMtxFromQuat(q);
        m_aInvIs[i] = dmMulRART(R, b->m_invI);

        dm_vec4 v = b->m_linearVelocity;
        dm_vec4 w = b->m_angularVelocity;


        v += dt * dmVecSet(b->m_invMass) * (dmVecSet(b->m_gravityScale * b->m_mass) * gravity + b->m_force);


        w += dt * dmMul33(m_aInvIs[i], b->m_torque);
        w = sSolveGyroscopic(q, b->m_I, w, dt);


        if (m_step.clearForces)
        {
            b->m_force = dmVecSet(0.0f);
            b->m_torque = dmVecSet(0.0f);
        }







        // Inference: damping factor clamped to [0, 1] (fmin with dm_vec4_one, fmax with zero).
        v = v * dmClamp(dm_vec4_one - dt * dmVecSet(b->m_linearDamping), dmVecSet(0.0f), dm_vec4_one);
        w = w * dmClamp(dm_vec4_one - dt * dmVecSet(b->m_angularDamping), dmVecSet(0.0f), dm_vec4_one);

        m_aVelocities[i].v = v;
        m_aVelocities[i].w = w;


        b->m_sweep.m_alpha0 = dmVecSet(0.0f);
        b->m_sweep.m_center0 = b->m_sweep.m_center;
        b->m_sweep.m_quaternion0 = b->m_sweep.m_quaternion;
    }

    dmSolverData solverData;
    solverData.step = m_step;
    solverData.aPositions = m_aPositions;
    solverData.aVelocities = m_aVelocities;
    solverData.aInvIs = m_aInvIs;
    solverData.pBodyArrayPtr = m_bodyArrayPtr;
    solverData.pContactArrayPtr = m_contactArrayPtr;





    for (dm_int32 i = 0; i < m_jointCount; ++i)
    {








        dm_int32 index = m_aJoints[i];

        dmJoint * pJoint = m_jointArrayPtr + index;
        dm_int32 indexA = pJoint->m_edges[0].m_bodyIndex;
        dm_int32 indexB = pJoint->m_edges[1].m_bodyIndex;

        pJoint->InitVelocityConstraints(m_specificJointArrays, m_bodyArrayPtr + indexA, m_bodyArrayPtr + indexB, &solverData);
    }










    m_contactSolver.Setup(&solverData);

    *m_pSetupContraintTimer += setupTimer.GetMilliseconds();


    dmTimer velocityTimer;
    for (dm_int32 i = 0; i < m_step.velocityIterations; ++i)
    {
        for (dm_int32 j = 0; j < m_jointCount; ++j)
        {
            dm_int32 index = m_aJoints[j];

            dmJoint * pJoint = m_jointArrayPtr + index;
            pJoint->SolveVelocityConstraints(m_specificJointArrays, &solverData);
        }

        m_contactSolver.SolveVelocityConstraints(&solverData);
    }


    m_contactSolver.StoreImpulses();

    // Inference: literal limits recover original float bits 40d1f948/3f490fdb (no named constant recorded).
    const dm_vec4 maxTranslation = dmVecSet(6.561679840087891f);
    const dm_vec4 maxTranslationSquared = maxTranslation * maxTranslation;
    const dm_vec4 maxRotation = dmVecSet(0.7853981852531433f);
    const dm_vec4 maxRotationSquared = maxRotation * maxRotation;

    for (dm_int32 i = 0; i < m_bodyCount; ++i)
    {
        dm_int32 index = m_aBodies[i];

        dmBody * b = m_bodyArrayPtr + index;
        if (b->m_bodyType == dm_staticBody)
        {
            continue;
        }

        dm_vec4 v = m_aVelocities[i].v;
        dm_vec4 w = m_aVelocities[i].w;



        if (dmIsFinite3(v) == false || dmIsFinite3(w) == false)
        {
            v = dmVecSet(0.0f);
            w = dmVecSet(0.0f);
        }




        if (dmGT(dmLengthSqr3(dt * v), maxTranslationSquared))
        {
            v = dmNormalize3(v);
            v = maxTranslation * inv_dt * v;
        }

        if (dmGT(dmLengthSqr3(dt * w), maxRotationSquared))
        {
            w = dmNormalize3(w);
            w = maxRotation * inv_dt * w;
        }


        m_aPositions[i].x += dt * v;


        dm_vec4 q = m_aPositions[i].q;



        q += dt * dm_vec4_oneHalf * dmMulVQ(w, q);
        if (dmIsZeroSafe4(q))
        {
            m_aPositions[i].q = dmNormalize4(q);
        }


        m_aVelocities[i].v = v;
        m_aVelocities[i].w = w;
    }

    *m_pVelocityConstraintTimer += velocityTimer.GetMilliseconds();


    // Inference: k_baumgarte literal 0.2f from original bits 3e4ccccd.
    dmTimer positionTimer;
    const dm_vec4 k_baumgarte = dmVecSet(0.2f);
    for (dm_int32 i = 0; i < m_step.positionIterations; ++i)
    {
        for (dm_int32 j = 0; j < m_jointCount; ++j)
        {
            dm_int32 index = m_aJoints[j];

            dmJoint * pJoint = m_jointArrayPtr + index;
            pJoint->SolvePositionConstraints(m_specificJointArrays, &solverData);
        }

        m_contactSolver.SolvePositionConstraints(&solverData, k_baumgarte);
    }


    for (dm_int32 i = 0; i < m_bodyCount; ++i)
    {
        dmBody * b = m_bodyArrayPtr + m_aBodies[i];
        if (b->m_bodyType == dm_staticBody)
        {
            continue;
        }

        dm_vec4 x = m_aPositions[i].x;
        dm_vec4 q = m_aPositions[i].q;

        if (dmIsFinite3(x) && dmIsFinite4(q))
        {
            b->m_sweep.m_center = x;
            b->m_sweep.m_quaternion = q;
            b->SynchronizeTransform();
        }






        dm_vec4 v = m_aVelocities[i].v;
        dm_vec4 w = m_aVelocities[i].w;

        if (dmIsFinite3(v) && dmIsFinite3(w))
        {
            b->m_linearVelocity = v;
            b->m_angularVelocity = w;
        }
    }







    if (m_step.useSleep)
    {
        dm_float32 minSleepTime = 3.402823466e+38f; // Inference: FLT_MAX value (original bits 7f7fffff); spelling unknown.
        // Inference: tolerance literals from original bits 3cdc71e4 (linear) and 3bf98ae9 (angular).
        const dm_vec4 linTolSqr = dmVecSet(0.026909776031970978f);
        const dm_vec4 angTolSqr = dmVecSet(0.0076154363341629505f);

        for (dm_int32 i = 0; i < m_bodyCount; ++i)
        {
            dmBody * b = m_bodyArrayPtr + m_aBodies[i];
            if (b->m_bodyType == dm_staticBody)
            {
                continue;
            }

            if (b->m_enableSleeping == false ||
                dmGT(dmLengthSqr3(b->m_angularVelocity), angTolSqr) ||
                dmGT(dmLengthSqr3(b->m_linearVelocity), linTolSqr))
            {
                b->m_sleepTime = 0.0f;
                minSleepTime = 0.0f;
            }
            else
            {
                b->m_sleepTime += dmGetX(dt);
                minSleepTime = dmMin(minSleepTime, b->m_sleepTime);
            }
        }
        // Inference: 0.5f sleep threshold literal (fmov immediate); no named constant recorded.
        if (minSleepTime >= 0.5f)
        {
            for (dm_int32 i = 0; i < m_bodyCount; ++i)
            {
                dmBody * b = m_bodyArrayPtr + m_aBodies[i];
                if (b->m_bodyType != dm_staticBody)
                {
                    b->SetSleeping(true);
                }
            }
        }
    }

    *m_pPositionConstraintTimer += positionTimer.GetMilliseconds();


}



void dmIsland::SolveFcn(void * pParam)
{


    // Inference: DWARF gives void* pParam and dmIsland* pThis in the same incoming register.
    dmIsland * pThis = static_cast<dmIsland *>(pParam);
    pThis->Solve();
}






void dmIsland::SolveTOIFcn(void * pParam) {
    // Inference: DWARF gives void* pParam and dmIsland* pThis in the same incoming register.
    dmIsland * pThis = static_cast<dmIsland *>(pParam);
    pThis->SolveTOI();
}

// NON_MATCHING: 1964/1964 bytes, 9/491 insns differ: first-loop IVs x8/x9 swapped (0x20 vs 0x40 strides), same pattern as Solve.
void dmIsland::SolveTOI()
{
    for (dm_int32 i = 0; i < m_bodyCount; ++i)
    {
        dm_int32 index = m_aBodies[i];

        dmBody * b = m_bodyArrayPtr + index;


        m_aPositions[i].x = b->m_sweep.m_center;
        m_aPositions[i].q = b->m_sweep.m_quaternion;

        if (b->m_bodyType == dm_staticBody)
        {
            m_aInvIs[i] = {};
            m_aVelocities[i].v = dmVecSet(0.0f);
            // Inference: static path merges this zero store into the loop tail at original +0x1f8.
            m_aVelocities[i].w = dmVecSet(0.0f);
            continue;
        }

        dmMtx R = dmMtxFromQuat(b->m_transform.q);
        m_aInvIs[i] = dmMulRART(R, b->m_invI);

        m_aVelocities[i].v = b->m_linearVelocity;
        m_aVelocities[i].w = b->m_angularVelocity;
    }

    dmSolverData solverData;
    solverData.step = m_step;
    solverData.aPositions = m_aPositions;
    solverData.aVelocities = m_aVelocities;
    solverData.aInvIs = m_aInvIs;
    solverData.pBodyArrayPtr = m_bodyArrayPtr;
    solverData.pContactArrayPtr = m_contactArrayPtr;

    m_contactSolver.Setup(&solverData);


    for (dm_int32 i = 0; i < m_step.velocityIterations; ++i)
    {
        m_contactSolver.SolveVelocityConstraints(&solverData);
    }

    // Inference: original fmov immediate is 0.75; no named constant is recorded.
    const dm_vec4 k_baumgarte = dmVecSet(0.75f);
    for (dm_int32 i = 0; i < m_step.positionIterations; ++i)
    {
        bool done = m_contactSolver.SolvePositionConstraints(&solverData, k_baumgarte);
        if (done)
        {
            break;
        }
    }

    // Inference: only index zero executes here: no bound test/backedge, and DWARF i is constant zero.
    for (dm_int32 i = 0; i < 1; ++i)
    {
        dmBody * b = m_bodyArrayPtr + m_aBodies[i];
        dm_vec4 x = m_aPositions[i].x;
        dm_vec4 q = m_aPositions[i].q;

        if (dmIsFinite3(x) && dmIsFinite4(q))
        {
            b->m_sweep.m_center0 = x;
            b->m_sweep.m_center = x;
            b->m_sweep.m_quaternion0 = q;
            b->m_sweep.m_quaternion = q;
            b->SynchronizeTransform();
        }
    }












































    // Inference: literal limits recover original float bits 40d1f948/422c38fa/3f490fdb/3f1de9e7; symbolic constant spellings were not retained (local names are DWARF).
    const dm_vec4 dt = m_step.dt;
    const dm_vec4 inv_dt = m_step.inv_dt;
    const dm_vec4 maxTranslation = dmVecSet(6.561679840087891f);
    const dm_vec4 maxTranslationSquared = maxTranslation * maxTranslation;
    const dm_vec4 maxRotation = dmVecSet(0.7853981852531433f);
    const dm_vec4 maxRotationSquared = maxRotation * maxRotation;

    // Inference: this second final block likewise visits index zero only, without a bound test/backedge.
    for (dm_int32 i = 0; i < 1; ++i)
    {
        dmBody * b = m_bodyArrayPtr + m_aBodies[i];
        if (b->m_bodyType == dm_staticBody)
        {
            continue;
        }

        dm_vec4 v = m_aVelocities[i].v;
        dm_vec4 w = m_aVelocities[i].w;


        if (!dmIsFinite3(v) || !dmIsFinite3(w))
        {
            v = dmVecSet(0.0f);
            w = dmVecSet(0.0f);
        }





        if (dmGT(dmLengthSqr3(dt * v), maxTranslationSquared))
        {
            v = dmNormalize3(v);
            v = maxTranslation * inv_dt * v;
        }

        if (dmGT(dmLengthSqr3(dt * w), maxRotationSquared))
        {
            w = dmNormalize3(w);
            w = maxRotation * inv_dt * w;
        }


        dm_vec4 x = m_aPositions[i].x + dt * v;
        m_aPositions[i].x = x;


        dm_vec4 q = m_aPositions[i].q;
        q += dt * dm_vec4_oneHalf * dmMulVQ(w, q);
        q = dmNormalize4(q);
        m_aPositions[i].q = q;


        m_aVelocities[i].v = v;
        m_aVelocities[i].w = w;


        if (dmIsFinite3(x) && dmIsFinite4(q))
        {
            b->m_sweep.m_center = x;
            b->m_sweep.m_quaternion = q;
            b->SynchronizeTransform();
        }






        if (dmIsFinite3(v) && dmIsFinite3(w))
        {
            b->m_linearVelocity = m_aVelocities[i].v;
            b->m_angularVelocity = m_aVelocities[i].w;
        }
    }





}

// Inference: the line-879 null branch skips the complete report, reconstructed as an early return.
void dmIsland::Report()
{
    if (m_pContactListener == NULL)
    {
        return;
    }

    dm_int32 count = m_contactSolver.m_constraintCount;
    // Inference: the line-886 induction starts at zero and advances through every constraint.
    for (dm_int32 i = 0; i < count; ++i)
    {
        dmContactVelocityConstraint * c = m_contactSolver.m_aVelocityConstraints + i;

        dmListenerManifold manifold;
        manifold.normal = c->normal;
        manifold.frictionPoint = c->tangentAnchor;
        manifold.frictionImpulse = c->pManifold->m_tangentImpulse;
        manifold.pointCount = c->pointCount;
        manifold.fixtureIdA.worldIndex = m_worldIndex;
        manifold.fixtureIdA.index = c->fixtureIndexA;
        manifold.fixtureIdA.revision = m_fixtureArray[c->fixtureIndexA].m_revision;
        manifold.fixtureIdB.worldIndex = m_worldIndex;
        manifold.fixtureIdB.index = c->fixtureIndexB;
        manifold.fixtureIdB.revision = m_fixtureArray[c->fixtureIndexB].m_revision;
        // Inference: the point loop advances both arrays from zero through c->pointCount.
        for (dm_int32 j = 0; j < c->pointCount; ++j)
        {
            const dmVelocityConstraintPoint * pPoint = m_contactSolver.m_aVelocityConstraintPoints + c->firstPointIndex + j;
            const dmManifoldPoint * mp = c->pManifold->m_points + j;

            manifold.points[j].position = mp->point;
            manifold.points[j].normalImpulse = dmGetW(pPoint->mass_gamma_beta_impulse);
            manifold.points[j].isNew = dmGTZ(dmSplatX(pPoint->results));
            manifold.points[j].normalVelocity = dmGetY(pPoint->results);
            manifold.points[j].slidingVelocity = dmGetZ(pPoint->results);
            manifold.points[j].triangleIndex = mp->id.fp.triangleIndex;
            manifold.points[j].isEdge = mp->id.fp.typeA == dmFeature::e_edge;
        }

        m_pContactListener->Report(&manifold);
    }
}
