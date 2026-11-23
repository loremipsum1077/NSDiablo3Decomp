// Original file: Contrib/Contrib/Domino/Dynamics/Joints/dmJoint.cpp

#include "dmJoint.h"
#include "dmDistanceJoint.h"
#include "dmMouseJoint.h"
#include "dmPrismaticJoint.h"
#include "dmRevoluteJoint.h"
#include "dmShoulderJoint.h"
#include "dmSphericalJoint.h"
#include "dmWeldJoint.h"
#include "../dmBody.h"
#include "../dmWorld.h"
#include "../dmSolver.h"
#include "../dmFixture.h"
#include "../Contacts/dmContact.h"
#include "../../Include/dmId.h"
#include "../../Include/dmSettings.h"
#include <string.h>

#line 19 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/Joints/dmJoint.cpp"








const dmJointId dm_nullJointId = { -1, 0xFFFF, 0 };
// dm_nullBodyId is owned by dmBody.cpp.
extern const dmBodyId dm_nullBodyId;
bool operator==(dmJointId id1, dmJointId id2)
{
    return id1.worldIndex == id2.worldIndex && id1.index == id2.index && id1.revision == id2.revision;
}


bool operator!=(dmJointId id1, dmJointId id2)
{
    return id1.worldIndex != id2.worldIndex || id1.index != id2.index || id1.revision != id2.revision;
}






















































bool dmJoint::Create(dmWorld * pWorld, const dmJointDef * pDef)
{
    m_type = pDef->m_type;
    dm_int32 indexA = pDef->m_bodyIdA.index;
    dm_int32 indexB = pDef->m_bodyIdB.index;
    m_edges[0].m_bodyIndex = indexA;
    m_edges[1].m_bodyIndex = indexB;




    memset(this, 0, 0x88);




    m_collideConnected = pDef->m_collideConnected;


    m_islandIndexA = -1;
    m_islandIndexB = -1;
    m_islandStamp = 0;


    switch (m_type)
    {
    case dm_distanceJoint:
        {
            m_subIndex = pWorld->m_distanceJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino distance joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_distanceJoints.GetCapacity());
                return false;
            }
            dmDistanceJoint * pJoint = pWorld->m_distanceJoints.Get(m_subIndex);
            pJoint->Create((const dmDistanceJointDef *)pDef);
        }
        break;

    case dm_mouseJoint:
        {
            m_subIndex = pWorld->m_mouseJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino mouse joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_mouseJoints.GetCapacity());
                return false;
            }
            dmMouseJoint * pJoint = pWorld->m_mouseJoints.Get(m_subIndex);
            pJoint->Create((const dmMouseJointDef *)pDef);
        }
        break;

    case dm_prismaticJoint:
        {
            m_subIndex = pWorld->m_prismaticJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino plane joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_prismaticJoints.GetCapacity());
                return false;
            }
            dmPrismaticJoint * pJoint = pWorld->m_prismaticJoints.Get(m_subIndex);
            pJoint->Create((const dmPrismaticJointDef *)pDef);
        }
        break;

    case dm_revoluteJoint:
        {
            m_subIndex = pWorld->m_revoluteJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino revolute joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_revoluteJoints.GetCapacity());
                return false;
            }
            dmRevoluteJoint * pJoint = pWorld->m_revoluteJoints.Get(m_subIndex);
            pJoint->Create((const dmRevoluteJointDef *)pDef);
        }
        break;

    case dm_shoulderJoint:
        {
            m_subIndex = pWorld->m_shoulderJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino shoulder joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_shoulderJoints.GetCapacity());
                return false;
            }
            dmShoulderJoint * pJoint = pWorld->m_shoulderJoints.Get(m_subIndex);
            pJoint->Create((const dmShoulderJointDef *)pDef);
        }
        break;

    case dm_sphericalJoint:
        {
            m_subIndex = pWorld->m_sphericalJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino spherical joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_sphericalJoints.GetCapacity());
                return false;
            }
            dmSphericalJoint * pJoint = pWorld->m_sphericalJoints.Get(m_subIndex);
            pJoint->Create((const dmSphericalJointDef *)pDef);
        }
        break;

    case dm_weldJoint:
        {
            m_subIndex = pWorld->m_weldJoints.Allocate();
            if (m_subIndex == -1)
            {
                dmWarn("Domino weld joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_weldJoints.GetCapacity());
                return false;
            }
            dmWeldJoint * pJoint = pWorld->m_weldJoints.Get(m_subIndex);
            pJoint->Create((const dmWeldJointDef *)pDef);
        }
        break;

    default:
        dmAssert(false);
        return false;
    }





    dmBody * pBodyA = pWorld->m_bodies.Get(indexA);
    dmBody * pBodyB = pWorld->m_bodies.Get(indexB);
    dmJoint::Edge * pEdgeA = m_edges + 0;
    dm_int32 keyA = m_index << 1;
    pEdgeA->m_prevKey = -1;
    pEdgeA->m_nextKey = pBodyA->m_jointListKey;
    pBodyA->m_jointListKey = keyA;
    dm_int32 nA = pEdgeA->m_nextKey;
    if (nA != -1)
    {
        pWorld->m_joints[nA >> 1].m_edges[nA & 1].m_prevKey = keyA;
    }
    dmJoint::Edge * pEdgeB = m_edges + 1;
    dm_int32 keyB = (m_index << 1) | 1;
    pEdgeB->m_prevKey = -1;
    pEdgeB->m_nextKey = pBodyB->m_jointListKey;
    pBodyB->m_jointListKey = keyB;
    dm_int32 nB = pEdgeB->m_nextKey;
    if (nB != -1)
    {
        pWorld->m_joints[nB >> 1].m_edges[nB & 1].m_prevKey = keyB;
    }

    // Destroy contacts between the two bodies if they must not collide.
    if (m_collideConnected == false)
    {

        for (dm_int32 i = pBodyB->m_fixtureList; i != -1; i = pWorld->m_fixtures.Get(i)->m_ownerNext)
        {
            dmFixture * pFixture = pWorld->m_fixtures.Get(i);
            dm_int32 edgeIndex = pFixture->m_contactListKey;
            while (edgeIndex != -1)
            {
                dmContact * pContact = pWorld->m_contacts.Get(edgeIndex >> 1);
                dmContact::Edge * pEdge = pContact->m_edges + (edgeIndex & 1);
                dm_int32 next = pEdge->m_nextKey;

                dmContact::Edge * pOtherEdge = pContact->m_edges + ((edgeIndex & 1) ^ 1);
                dmFixture * pOtherFixture = pWorld->m_fixtures.Get(pOtherEdge->m_fixtureIndex);

                if (pOtherFixture->m_ownerIndex == indexA && pOtherFixture->m_ownerType == dm_bodyOwner)
                {
                    pWorld->DestroyContact(edgeIndex >> 1);
                }

                edgeIndex = next;
            }
        }
    }

    ++m_revision;
    return true;
}


void dmJoint::Destroy(dmWorld * pWorld)
{
    switch (m_type)
    {
    case dm_distanceJoint:
        pWorld->m_distanceJoints.Free(m_subIndex);
        break;

    case dm_mouseJoint:
        pWorld->m_mouseJoints.Free(m_subIndex);
        break;

    case dm_prismaticJoint:
        pWorld->m_prismaticJoints.Free(m_subIndex);
        break;

    case dm_revoluteJoint:
        pWorld->m_revoluteJoints.Free(m_subIndex);
        break;

    case dm_shoulderJoint:
        pWorld->m_shoulderJoints.Free(m_subIndex);
        break;

    case dm_sphericalJoint:
        pWorld->m_sphericalJoints.Free(m_subIndex);
        break;

    case dm_weldJoint:
        pWorld->m_weldJoints.Free(m_subIndex);
        break;

    default:
        break;
    }




    dmBody * pBodyA = pWorld->m_bodies.Get(m_edges[0].m_bodyIndex);
    dmBody * pBodyB = pWorld->m_bodies.Get(m_edges[1].m_bodyIndex);

    // Wake up connected bodies.
    pBodyA->SetSleeping(false);
    pBodyB->SetSleeping(false);

    // Remove from body A
    {
        dmJoint::Edge * pEdgeA = m_edges + 0;

        dm_int32 nA = pEdgeA->m_nextKey;
        dm_int32 pA = pEdgeA->m_prevKey;
        if (pA != -1)
        {
            pWorld->m_joints.Get(pA >> 1)->m_edges[pA & 1].m_nextKey = nA;
        }

        if (nA != -1)
        {
            pWorld->m_joints.Get(nA >> 1)->m_edges[nA & 1].m_prevKey = pA;
        }

        dm_int32 keyA = m_index << 1;
        if (keyA == pBodyA->m_jointListKey)
        {
            pBodyA->m_jointListKey = nA;
        }
    }

    // Remove from body B
    {
        dmJoint::Edge * pEdgeB = m_edges + 1;

        dm_int32 nB = pEdgeB->m_nextKey;
        dm_int32 pB = pEdgeB->m_prevKey;
        if (pB != -1)
        {
            pWorld->m_joints.Get(pB >> 1)->m_edges[pB & 1].m_nextKey = nB;
        }

        if (nB != -1)
        {
            pWorld->m_joints.Get(nB >> 1)->m_edges[nB & 1].m_prevKey = pB;
        }

        dm_int32 keyB = (m_index << 1) | 1;
        if (keyB == pBodyB->m_jointListKey)
        {
            pBodyB->m_jointListKey = nB;
        }
    }

    // If the joint prevents collisions, then flag any contacts for filtering.
    if (m_collideConnected == false)
    {
        for (dm_int32 i = pBodyB->m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
        {
            dmFixture * pFixture = pWorld->m_fixtures.Get(i);


            pWorld->m_broadPhase.TouchProxy(pFixture->m_proxyId);
        }

        pWorld->m_newContacts = true;
    }
}









































































dm_float32 dmJoint::GetLeverArmA(dmWorld * pWorld) const
{
    dm_int32 indexA = m_edges[0].m_bodyIndex;
    dm_vec4 localCenter = pWorld->m_bodies[indexA].m_sweep.m_localCenter;
    dm_vec4 localAnchor;

    switch (m_type)
    {
    case dm_distanceJoint:
        localAnchor = pWorld->m_distanceJoints[m_subIndex].m_localAnchorA;
        break;

    case dm_mouseJoint:
        return 0.0f;

    case dm_prismaticJoint:
        localAnchor = pWorld->m_prismaticJoints[m_subIndex].m_localFrameA.p;
        break;

    case dm_revoluteJoint:
        localAnchor = pWorld->m_revoluteJoints[m_subIndex].m_localFrameA.p;
        break;

    case dm_shoulderJoint:
        localAnchor = pWorld->m_shoulderJoints[m_subIndex].m_localFrameA.p;
        break;

    case dm_sphericalJoint:
        localAnchor = pWorld->m_sphericalJoints[m_subIndex].m_localAnchorA;
        break;

    case dm_weldJoint:
        localAnchor = pWorld->m_weldJoints[m_subIndex].m_localFrameA.p;
        break;

    default:
        return 0.0f;
    }

    dm_float32 leverArm = dmLength3_f32(dmSub(localAnchor, localCenter));
    return leverArm;
}



dm_float32 dmJoint::GetLeverArmB(dmWorld * pWorld) const
{
    dm_int32 indexB = m_edges[1].m_bodyIndex;
    dm_vec4 localCenter = pWorld->m_bodies[indexB].m_sweep.m_localCenter;
    dm_vec4 localAnchor;

    switch (m_type)
    {
    case dm_distanceJoint:
        localAnchor = pWorld->m_distanceJoints[m_subIndex].m_localAnchorB;
        break;

    case dm_mouseJoint:
        localAnchor = pWorld->m_mouseJoints[m_subIndex].m_localAnchor;
        break;

    case dm_prismaticJoint:
        localAnchor = pWorld->m_prismaticJoints[m_subIndex].m_localFrameB.p;
        break;

    case dm_revoluteJoint:
        localAnchor = pWorld->m_revoluteJoints[m_subIndex].m_localFrameB.p;
        break;

    case dm_shoulderJoint:
        localAnchor = pWorld->m_shoulderJoints[m_subIndex].m_localFrameB.p;
        break;

    case dm_sphericalJoint:
        localAnchor = pWorld->m_sphericalJoints[m_subIndex].m_localAnchorB;
        break;

    case dm_weldJoint:
        localAnchor = pWorld->m_weldJoints[m_subIndex].m_localFrameB.p;
        break;

    default:
        return 0.0f;
    }

    dm_float32 leverArm = dmLength3_f32(dmSub(localAnchor, localCenter));
    return leverArm;
}



void dmJoint::InitVelocityConstraints(dmSpecificJointArrays & specificJointArrays, const dmBody * pBodyA, const dmBody * pBodyB, const dmSolverData * pData)
{
    m_invMassA = pBodyA->m_invMass;
    m_invMassB = pBodyB->m_invMass;
    m_localInvInertiaA = pBodyA->m_invI;
    m_localInvInertiaB = pBodyB->m_invI;

    switch (m_type)
    {
    case dm_distanceJoint:
        specificJointArrays.m_distanceJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyA, pBodyB, this, pData);
        break;

    case dm_mouseJoint:
        specificJointArrays.m_mouseJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyB, this, pData);
        break;

    case dm_prismaticJoint:
        specificJointArrays.m_prismaticJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyA, pBodyB, this, pData);
        break;

    case dm_revoluteJoint:
        specificJointArrays.m_revoluteJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyA, pBodyB, this, pData);
        break;

    case dm_shoulderJoint:
        specificJointArrays.m_shoulderJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyA, pBodyB, this, pData);
        break;

    case dm_sphericalJoint:
        specificJointArrays.m_sphericalJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyA, pBodyB, this, pData);
        break;

    case dm_weldJoint:
        specificJointArrays.m_weldJointArrayPtr[m_subIndex].InitVelocityConstraints(pBodyA, pBodyB, this, pData);
        break;

    default:
        dmAssert(false);
    }
}


void dmJoint::SolveVelocityConstraints(dmSpecificJointArrays & specificJointArrays, dmSolverData * pData)
{
    switch (m_type)
    {
    case dm_distanceJoint:
        specificJointArrays.m_distanceJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    case dm_mouseJoint:
        specificJointArrays.m_mouseJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    case dm_prismaticJoint:
        specificJointArrays.m_prismaticJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    case dm_revoluteJoint:
        specificJointArrays.m_revoluteJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    case dm_shoulderJoint:
        specificJointArrays.m_shoulderJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    case dm_sphericalJoint:
        specificJointArrays.m_sphericalJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    case dm_weldJoint:
        specificJointArrays.m_weldJointArrayPtr[m_subIndex].SolveVelocityConstraints(pData, this);
        break;

    default:
        dmAssert(false);
    }
}


void dmJoint::SolvePositionConstraints(dmSpecificJointArrays & specificJointArrays, dmSolverData * pData)
{
    switch (m_type)
    {
    case dm_distanceJoint:
        specificJointArrays.m_distanceJointArrayPtr[m_subIndex].SolvePositionConstraints(pData, this);
        break;

    case dm_mouseJoint:
        // Mouse joints have no position constraint.
        break;

    case dm_prismaticJoint:
        specificJointArrays.m_prismaticJointArrayPtr[m_subIndex].SolvePositionConstraints(pData, this);
        break;

    case dm_revoluteJoint:
        specificJointArrays.m_revoluteJointArrayPtr[m_subIndex].SolvePositionConstraints(pData, this);
        break;

    case dm_shoulderJoint:
        specificJointArrays.m_shoulderJointArrayPtr[m_subIndex].SolvePositionConstraints(pData, this);
        break;

    case dm_sphericalJoint:
        specificJointArrays.m_sphericalJointArrayPtr[m_subIndex].SolvePositionConstraints(pData, this);
        break;

    case dm_weldJoint:
        specificJointArrays.m_weldJointArrayPtr[m_subIndex].SolvePositionConstraints(pData, this);
        break;

    default:
        dmAssert(false);
    }
}































































































































bool dmJoint_IsValid(dmJointId id)
{
    if (id.worldIndex >= 65534) return false;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL)
    {
        return false;
    }

    if (pWorld->m_joints.IsValid(id.index) == false)
    {
        return false;
    }

    return pWorld->m_joints[id.index].m_revision == id.revision;
}







dmJointType dmJoint_GetType(dmJointId id)
{
    if (id.worldIndex >= 65534) return dm_unknownJoint;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return dm_unknownJoint;

    return pWorld->m_joints[id.index].m_type;
}
dmBodyId dmJoint_GetBodyA(dmJointId id)
{
    if (id.worldIndex >= 65534) return dm_nullBodyId;
    if (id.index == -1) return dm_nullBodyId;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return dm_nullBodyId;

    dm_int32 bodyIndex = pWorld->m_joints[id.index].m_edges[0].m_bodyIndex;
    dmBodyId bodyId;
    bodyId.index = bodyIndex;
    bodyId.revision = pWorld->m_bodies[bodyIndex].m_revision;
    bodyId.worldIndex = id.worldIndex;
    return bodyId;
}
dmBodyId dmJoint_GetBodyB(dmJointId id)
{
    if (id.worldIndex >= 65534) return dm_nullBodyId;
    if (id.index == -1) return dm_nullBodyId;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return dm_nullBodyId;

    dm_int32 bodyIndex = pWorld->m_joints[id.index].m_edges[1].m_bodyIndex;
    dmBodyId bodyId;
    bodyId.index = bodyIndex;
    bodyId.revision = pWorld->m_bodies[bodyIndex].m_revision;
    bodyId.worldIndex = id.worldIndex;
    return bodyId;
}
































void dmJoint_SetMotorMode(dmJointId id, dmMotorMode mode, dmWake state)
{
    if (id.worldIndex >= 65534) return;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return;

    dmJoint * pJoint = &pWorld->m_joints[id.index];
    dm_int32 indexA = pJoint->m_edges[0].m_bodyIndex;
    dm_int32 indexB = pJoint->m_edges[1].m_bodyIndex;

    bool changed = false;
    if (pJoint->m_type == dm_revoluteJoint)
    {
        dmRevoluteJoint * pSub = pWorld->m_revoluteJoints.Get(pJoint->m_subIndex);
        if (pSub->m_motorMode != mode)
        {
            pSub->m_motorMode = mode;
            changed = true;
        }
    }
    else if (pJoint->m_type == dm_shoulderJoint)
    {
        dmShoulderJoint * pSub = pWorld->m_shoulderJoints.Get(pJoint->m_subIndex);
        if (pSub->m_motorMode != mode)
        {
            pSub->m_motorMode = mode;
            changed = true;
        }
    }
    else if (pJoint->m_type == dm_prismaticJoint)
    {
        dmPrismaticJoint * pSub = pWorld->m_prismaticJoints.Get(pJoint->m_subIndex);
        if (pSub->m_motorMode != mode)
        {
            pSub->m_motorMode = mode;
            changed = true;
        }
    }

    if (changed && state == dm_wakeIfChanged)
    {
        pWorld->m_bodies[indexA].SetSleeping(false);
        pWorld->m_bodies[indexB].SetSleeping(false);
    }
}
























void dmJoint_SetMaxMotorTorque(dmJointId id, dm_float32 maxTorque, dmWake state)
{
    if (id.worldIndex >= 65534) return;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return;

    dmJoint * pJoint = &pWorld->m_joints[id.index];
    dm_int32 indexA = pJoint->m_edges[0].m_bodyIndex;
    dm_int32 indexB = pJoint->m_edges[1].m_bodyIndex;

    bool changed = false;
    if (pJoint->m_type == dm_revoluteJoint)
    {
        dmRevoluteJoint * pSub = pWorld->m_revoluteJoints.Get(pJoint->m_subIndex);
        if (pSub->m_maxMotorTorque != maxTorque)
        {
            pSub->m_maxMotorTorque = maxTorque;
            changed = true;
        }
    }
    else if (pJoint->m_type == dm_shoulderJoint)
    {
        dmShoulderJoint * pSub = pWorld->m_shoulderJoints.Get(pJoint->m_subIndex);
        if (pSub->m_maxMotorTorque != maxTorque)
        {
            pSub->m_maxMotorTorque = maxTorque;
            changed = true;
        }
    }

    if (changed && state == dm_wakeIfChanged)
    {
        pWorld->m_bodies[indexA].SetSleeping(false);
        pWorld->m_bodies[indexB].SetSleeping(false);
    }
}




















































































































void dmRevoluteJoint_SetDesiredMotorAngle(dmJointId id, dm_vec4Arg qRel, dmWake state)
{
    if (id.worldIndex >= 65534) return;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return;
    dmJoint * pJoint = &pWorld->m_joints[id.index];
    dm_int32 indexA = pJoint->m_edges[0].m_bodyIndex;
    dm_int32 indexB = pJoint->m_edges[1].m_bodyIndex;
    if (pJoint->m_type != dm_revoluteJoint)
    {
        return;
    }

    dmRevoluteJoint * pSub = pWorld->m_revoluteJoints.Get(pJoint->m_subIndex);

    // The relative rotation about the hinge axis (local z of frame A).






    dm_float32 sinHalf = dmDot3_f32(qRel, dmRotate(pSub->m_localFrameA.q, dm_vec4_unitZ));
    dm_float32 cosHalf = dmGetW(qRel);
    dm_float32 angle = 2.0f * dmEstAtan2(sinHalf, cosHalf);

    if (pSub->m_desiredMotorAngle == angle)
    {
        return;
    }

    if (state == dm_wakeIfChanged)
    {
        pWorld->m_bodies[indexA].SetSleeping(false);
        pWorld->m_bodies[indexB].SetSleeping(false);
    }

    pSub->m_desiredMotorAngle = angle;
}



















































void dmShoulderJoint_SetDesiredMotorOrientation(dmJointId id, dm_vec4Arg relativeQuaternion, dmWake state)
{
    if (id.worldIndex >= 65534) return;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_joints.IsValid(id.index) || pWorld->m_joints[id.index].m_revision != id.revision) return;
    dmJoint * pJoint = &pWorld->m_joints[id.index];
    dm_int32 indexA = pJoint->m_edges[0].m_bodyIndex;
    dm_int32 indexB = pJoint->m_edges[1].m_bodyIndex;
    if (pJoint->m_type == dm_shoulderJoint)
    {
        dmShoulderJoint * pSub = pWorld->m_shoulderJoints.Get(pJoint->m_subIndex);
        if (dmGT(dmLength3(dmSub(pSub->m_motorQuaternion, relativeQuaternion)), dm_vec4_epsilon))
        {
            if (state == dm_wakeIfChanged)
            {
                pWorld->m_bodies[indexA].SetSleeping(false);
                pWorld->m_bodies[indexB].SetSleeping(false);
            }

            pSub->m_motorQuaternion = relativeQuaternion;
        }
    }
}
