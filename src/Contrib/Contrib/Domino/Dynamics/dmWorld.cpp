// Original file: Contrib/Contrib/Domino/Dynamics/dmWorld.cpp
#include "dmWorld.h"
#include "dmBody.h"
#include "dmFixture.h"
#include "Joints/dmJoint.h"
#include "Contacts/dmContact.h"
#include "dmPhantom.h"
#include "Contacts/dmConvexContact.h"
#include "Joints/dmDistanceJoint.h"
#include "Joints/dmMouseJoint.h"
#include "Joints/dmPrismaticJoint.h"
#include "Joints/dmRevoluteJoint.h"
#include "Joints/dmShoulderJoint.h"
#include "Joints/dmSphericalJoint.h"
#include "Joints/dmWeldJoint.h"
#include "../Include/dmApi.h"
#include "../Include/dmId.h"
#include "../Include/dmJobDispatcher.h"
#include "../Include/dmSettings.h"
#include <string.h>
#include "../Common/Platform/dmThreading_Linux.h"
// dmMath.cpp and dmCollision.cpp; the DWARF does not name their header.
void dmMathTest();
void dmCollisionTest();
struct dmMutex;
#line 18 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/dmWorld.cpp"
// Job records; the DWARF gives their layout but not their lines.
struct dmTOIJob {
    dmWorld * pWorld; dmMutex * pMutex; bool * aTOIHits; dm_float32 * aTOIs; dm_int32 startIndex; dm_int32 finalIndex;
    static void Fcn(void * pParam);
};
struct dmCollideJob {
    dmWorld * pWorld; dmMutex * pMutex; dm_int32 index; dm_int32 final;
    static void Fcn(void * pParam);
};
#include "Contacts/dmMeshContact.h"
#include "../Include/dmShapes.h"
#include "../Include/dmBuffer.h"
// Declaration from dmFixture.cpp:45; DWARF does not identify its declaration header.
bool dmShouldCollide(const dmFilter & filterA, const dmFilter & filterB);
#include "dmIsland.h"
static dmDefaultJobDispatcher s_defaultJobDispatcher;

const dmWorldId dm_nullWorldId = { 0xFFFF };


bool operator==(dmWorldId id1, dmWorldId id2)
{
    return id1.index == id2.index;
}


bool operator!=(dmWorldId id1, dmWorldId id2)
{
    return id1.index != id2.index;
}


#include "../Common/dmTimer.h"
#include "../Include/dmCallbacks.h"
#include "../Collision/Functions/dmTimeOfImpact.h"
#include <float.h>
// Inferred original macro/value from dmWorld.cpp:938-939 array bounds and :1016 assertion.
#define dm_maxTOIContacts 32
dmWorld * dmWorld::s_aWorlds[65534];

static bool dmShouldCollide(const dmQueryFilter & query, const dmFilter & filter)
{
    dm_uint16 categoryBits = filter.m_categoryBits;
    return (query.m_includeMask & categoryBits) != 0 && (query.m_excludeMask & categoryBits) == 0;
}






// line table: jointCapacity is three terms on 106 and a += of three on 107 (columns 33/57/86/94 and 16/49/57/82)
void dmWorld::Create(const dmWorldDef * pDef)
{
    dmTimer::Initialize();

    m_broadPhase.Create();
    m_stackAllocator.Create(pDef->m_stackCapacity);
    m_poolAllocator.Create();

    m_stepCount = 0;
    m_totalStepTime = 0.0f;

    memset(&m_profile, 0, sizeof(m_profile));
    memset(&m_counters, 0, sizeof(m_counters));

    m_pDestructionListener = NULL;
    m_pContactListener = NULL;
    m_pJobDispatcher = &s_defaultJobDispatcher;




    m_aTOIBodies = (dm_int32 *)dmAlloc(pDef->m_bodyCapacity * sizeof(dm_int32));
    m_toiBodyCount = 0;




    m_bodies.Create(pDef->m_bodyCapacity);
    m_phantoms.Create(pDef->m_phantomCapacity);
    m_fixtures.Create(pDef->m_fixtureCapacity);

    dm_int32 contactCapacity = pDef->m_convexContactCapacity + pDef->m_meshContactCapacity;
    m_contacts.Create(contactCapacity);
    m_convexContacts.Create(pDef->m_convexContactCapacity);
    m_meshContacts.Create(pDef->m_meshContactCapacity);

    dm_int32 jointCapacity = pDef->m_distanceJointCapacity + pDef->m_mouseJointCapacity + pDef->m_revoluteJointCapacity;
    jointCapacity += pDef->m_shoulderJointCapacity + pDef->m_sphericalJointCapacity + pDef->m_weldJointCapacity;
    m_joints.Create(jointCapacity);
    m_distanceJoints.Create(pDef->m_distanceJointCapacity);
    m_mouseJoints.Create(pDef->m_mouseJointCapacity);
    m_prismaticJoints.Create(pDef->m_prismaticJointCapacity);
    m_revoluteJoints.Create(pDef->m_revoluteJointCapacity);
    m_shoulderJoints.Create(pDef->m_shoulderJointCapacity);
    m_sphericalJoints.Create(pDef->m_sphericalJointCapacity);
    m_weldJoints.Create(pDef->m_weldJointCapacity);

    m_gravity = pDef->m_gravity;

    m_counters.bodyCapacity = m_bodies.GetCapacity();
    m_counters.phantomCapacity = m_phantoms.GetCapacity();
    m_counters.fixtureCapacity = m_fixtures.GetCapacity();
    m_counters.distanceJointCapacity = m_distanceJoints.GetCapacity();
    m_counters.mouseJointCapacity = m_mouseJoints.GetCapacity();
    m_counters.prismaticJointCapacity = m_prismaticJoints.GetCapacity();
    m_counters.revoluteJointCapacity = m_revoluteJoints.GetCapacity();
    m_counters.shoulderJointCapacity = m_shoulderJoints.GetCapacity();
    m_counters.sphericalJointCapacity = m_sphericalJoints.GetCapacity();
    m_counters.weldJointCapacity = m_weldJoints.GetCapacity();

    m_aMaterials = NULL;
    m_materialCount = 0;

    m_lock.StoreRelease(0);

    m_newContacts = false;
    m_useSleep = true;
    m_useWarmStarting = true;
    m_useContinuous = true;




    m_useSubStepping = false;
    m_isStepComplete = true;

    m_islandStamp = 0;
    m_inv_dt0 = dmVecSet(0.0f);

    m_aDebugVertices = NULL;
    m_debugVertexCapacity = 0;

    m_aDebugColors = NULL;
    m_debugColorCapacity = 0;

    m_index = -1;
}


void dmWorld::Destroy()
{

    for (dm_int32 i = 0; i < m_meshContacts.GetCeiling(); ++i)
    {
        if (m_meshContacts[i].m_index == i)
        {
            m_meshContacts[i].Destroy(this);
        }
    }


    dmFree(m_aMaterials);
    dmFree(m_aDebugColors);
    dmFree(m_aDebugVertices);


    m_bodies.Destroy();
    m_phantoms.Destroy();
    m_fixtures.Destroy();

    dmFree(m_aTOIBodies);
    m_aTOIBodies = NULL;

    m_joints.Destroy();
    m_distanceJoints.Destroy();
    m_mouseJoints.Destroy();
    m_prismaticJoints.Destroy();
    m_revoluteJoints.Destroy();
    m_shoulderJoints.Destroy();
    m_sphericalJoints.Destroy();
    m_weldJoints.Destroy();

    m_contacts.Destroy();
    m_convexContacts.Destroy();
    m_meshContacts.Destroy();

    m_broadPhase.Destroy();
    m_stackAllocator.Destroy();
    m_poolAllocator.Destroy();
}


dm_int32 dmWorld::CreateBody(const dmBodyDef * pDef)
{
    dm_int32 oldCapacity = m_bodies.GetCapacity();

    dm_int32 index = m_bodies.Allocate();
    m_bodies[index].Create(pDef);

    // Grow the TOI body array with the pool.
    if (m_bodies.GetCapacity() > oldCapacity)
    {
        dm_int32 * aTOIBodies = (dm_int32 *)dmAlloc(m_bodies.GetCapacity() * sizeof(dm_int32));
        memcpy(aTOIBodies, m_aTOIBodies, oldCapacity);
        dmFree(m_aTOIBodies);
        m_aTOIBodies = aTOIBodies;
    }

    return index;
}


void dmWorld::DestroyBody(dm_int32 index)
{






    m_bodies[index].Destroy(this);
    m_bodies.Free(index);
}





















dm_int32 dmWorld::CreateJoint(const dmJointDef * pDef)
{
    if (pDef->m_bodyIdA.worldIndex != m_index || pDef->m_bodyIdB.worldIndex != pDef->m_bodyIdA.worldIndex)
    {
        return -1;
    }


    dm_int32 indexA = pDef->m_bodyIdA.index;
    dm_int32 indexB = pDef->m_bodyIdB.index;

    if (m_bodies.IsValid(indexA) == false || m_bodies.IsValid(indexB) == false)
    {
        return -1;
    }

    dm_uint32 revisionA = pDef->m_bodyIdA.revision;
    dm_uint32 revisionB = pDef->m_bodyIdB.revision;

    if (m_bodies[indexA].m_revision != revisionA || m_bodies[indexB].m_revision != revisionB)
    {
        return -1;
    }

    dm_int32 index = m_joints.Allocate();
    if (index != -1)
    {
        bool success = m_joints[index].Create(this, pDef);
        if (success == false)
        {

            m_joints.Free(index);
            return -1;
        }

        return index;
    }

    dmWarn("Domino joint capacity of %d exceeded. Contact ecatto@blizzard.com\n", m_joints.GetCapacity());
    return -1;
}


void dmWorld::DestroyJoint(dm_int32 index)
{






    m_joints[index].Destroy(this);
    m_joints.Free(index);
}

void dmWorld::CreateContact(dm_int32 fixtureIndexA, dm_int32 fixtureIndexB)
{
    dmShape::Type typeA = m_fixtures[fixtureIndexA].m_pShape->m_type;
    dmShape::Type typeB = m_fixtures[fixtureIndexB].m_pShape->m_type;


    if (typeA == dmShape::e_mesh && typeB == dmShape::e_mesh)
    {
        return;
    }



    if (typeA < typeB)
    {
        dmSwap(fixtureIndexA, fixtureIndexB);
    }


    dm_int32 index = m_contacts.Allocate();
    if (index != -1)
    {
        bool success = m_contacts[index].Create(this, fixtureIndexA, fixtureIndexB);
        if (!success)
        {
            m_contacts.Free(index);
        }
    }
}


void dmWorld::DestroyContact(dm_int32 index)
{
    if (index == -1)
    {
        return;
    }


    m_contacts[index].Destroy(this);
    m_contacts.Free(index);
}

void dmWorld::Solve(const dmTimeStep & step)
{

    if (m_islandStamp >= 0x7fffffff)
    {
        ResetIslandStamps();
        m_islandStamp = 0;
    }

    m_toiBodyCount = 0;

    dm_int32 jobCount = 0;
    dm_uint32 baseIslandStamp = m_islandStamp;

    dmIsland * pIslandList = NULL;


    dm_int32 stackCapacity = m_bodies.GetCount();
    dm_int32 * aStack = (dm_int32 *)m_stackAllocator.Allocate(stackCapacity * sizeof(dm_int32));

    dm_int32 bodyCeiling = m_bodies.GetCeiling();
    dm_int32 contactCeiling = m_contacts.GetCeiling();
    dm_int32 jointCeiling = m_joints.GetCeiling();

    dm_int32 * aBodies = (dm_int32 *)m_stackAllocator.Allocate(bodyCeiling * sizeof(dm_int32));
    dm_int32 * aContacts = (dm_int32 *)m_stackAllocator.Allocate(contactCeiling * sizeof(dm_int32));
    dm_int32 * aJoints = (dm_int32 *)m_stackAllocator.Allocate(jointCeiling * sizeof(dm_int32));



    dm_int32 * aBodyMap = (dm_int32 *)m_stackAllocator.Allocate(bodyCeiling * sizeof(dm_int32));








    for (dm_int32 seedIndex = 0; seedIndex < bodyCeiling; ++seedIndex)
    {


        dmBody * pSeed = m_bodies.Get(seedIndex);

        if (pSeed->m_index != seedIndex ||
            pSeed->m_bodyType == dm_staticBody)
            continue;
        // line table: the sleeping/active bit test carries line 401 and is fused with the island stamp compare (and/cmp + ccmp), so they are one condition
        if (pSeed->m_islandStamp > baseIslandStamp || pSeed->m_isSleeping || !pSeed->m_isActive)
        {
            continue;
        }





        dm_int32 bodyCount = 0;
        dm_int32 contactCount = 0;
        dm_int32 jointCount = 0;
        dm_int32 stackCount = 0;
        aStack[stackCount++] = seedIndex;


        aBodies[bodyCount] = seedIndex;
        aBodyMap[seedIndex] = bodyCount;
        ++bodyCount;



        ++m_islandStamp;
        pSeed->m_islandStamp = m_islandStamp;


        while (stackCount > 0)
        {

            dm_int32 bodyIndex = aStack[--stackCount];
            dmBody * b = m_bodies.Get(bodyIndex);




            b->m_isSleeping = false;


            for (dm_int32 j = b->m_fixtureList; j != -1; j = m_fixtures[j].m_ownerNext)
            {
                dmFixture * f = m_fixtures.Get(j);

                dm_int32 contactKey = f->m_contactListKey;
                while (contactKey != -1)
                {
                    dm_int32 contactIndex = contactKey >> 1;
                    dm_int32 edgeIndex = contactKey & 1;
                    dmContact * pContact = m_contacts.Get(contactIndex);
                    contactKey = pContact->m_edges[edgeIndex].m_nextKey;
                    if (pContact->m_islandStamp == m_islandStamp)
                    {
                        continue;
                    }
                    if (!pContact->m_isSolid || !pContact->m_isTouching)
                    {
                        continue;
                    }
                    aContacts[contactCount++] = contactIndex;
                    pContact->m_islandStamp = m_islandStamp;

                    dm_int32 otherFixtureIndex = pContact->m_edges[edgeIndex ^ 1].m_fixtureIndex;
                    dmFixture * pOtherFixture = m_fixtures.Get(otherFixtureIndex);



                    dm_int32 otherIndex = pOtherFixture->m_ownerIndex;


                    dmBody * pOtherBody = m_bodies.Get(otherIndex);
                    if (pOtherBody->m_islandStamp != m_islandStamp)
                    {
                        if (pOtherBody->m_bodyType != dm_staticBody)
                        {






                            dmAssert(stackCount < stackCapacity);
                            aStack[stackCount++] = otherIndex;
                        }
                        aBodies[bodyCount] = otherIndex;
                        aBodyMap[otherIndex] = bodyCount;
                        ++bodyCount;
                        pOtherBody->m_islandStamp = m_islandStamp;
                    }


                    if (edgeIndex == 0)
                    {
                        pContact->m_islandIndexA = aBodyMap[bodyIndex];
                        pContact->m_islandIndexB = aBodyMap[otherIndex];
                    }
                    else
                    {
                        pContact->m_islandIndexA = aBodyMap[otherIndex];
                        pContact->m_islandIndexB = aBodyMap[bodyIndex];
                    }
                }
            }






            dm_int32 jointKey = b->m_jointListKey;
            while (jointKey != -1)
            {
                dm_int32 jointIndex = jointKey >> 1;
                dm_int32 edgeIndex = jointKey & 1;
                dmJoint * pJoint = m_joints.Get(jointIndex);
                jointKey = pJoint->m_edges[edgeIndex].m_nextKey;
                if (pJoint->m_islandStamp == m_islandStamp)
                {
                    continue;
                }

                dm_int32 otherIndex = pJoint->m_edges[edgeIndex ^ 1].m_bodyIndex;
                dmBody * pOtherBody = m_bodies.Get(otherIndex);
                if (!pOtherBody->m_isActive)
                {
                    continue;
                }
                aJoints[jointCount++] = jointIndex;
                pJoint->m_islandStamp = m_islandStamp;
                if (pOtherBody->m_islandStamp != m_islandStamp)
                {
                    if (pOtherBody->m_bodyType != dm_staticBody)
                    {



                        dmAssert(stackCount < stackCapacity);
                        aStack[stackCount++] = otherIndex;
                    }
                    aBodies[bodyCount] = otherIndex;
                    aBodyMap[otherIndex] = bodyCount;
                    ++bodyCount;
                    pOtherBody->m_islandStamp = m_islandStamp;
                }


                if (edgeIndex == 0)
                {
                    pJoint->m_islandIndexA = aBodyMap[bodyIndex];
                    pJoint->m_islandIndexB = aBodyMap[otherIndex];
                }
                else
                {
                    pJoint->m_islandIndexA = aBodyMap[otherIndex];
                    pJoint->m_islandIndexB = aBodyMap[bodyIndex];
                }
            }
        }





        dmIslandDef def;
        def.pWorld = this;
        def.step = step;
        def.aBodies = aBodies;
        def.bodyCount = bodyCount;
        def.aContacts = aContacts;
        def.contactCount = contactCount;
        def.aJoints = aJoints;
        def.jointCount = jointCount;

        dmIsland * pIsland = (dmIsland *)m_stackAllocator.Allocate(sizeof(dmIsland));
        pIsland->Create(&def);
        pIsland->m_pNext = pIslandList;
        pIslandList = pIsland;
        m_pJobDispatcher->Submit(dmIsland::SolveFcn, pIsland);
        ++jobCount;
    }



























    m_pJobDispatcher->FinishJobs();

    m_counters.maxJobCount = dmMax(m_counters.maxJobCount, jobCount);


    dmTimer reportTimer;
    dmIsland * pNode = pIslandList;
    while (pNode != NULL)
    {
        dmIsland * pNext = pNode->m_pNext;
        pNode->Report();
        pNode->Destroy();
        m_stackAllocator.Free(pNode);
        pNode = pNext;
    }
    m_profile.report = reportTimer.GetMilliseconds();

    m_stackAllocator.Free(aBodyMap);
    m_stackAllocator.Free(aJoints);
    m_stackAllocator.Free(aContacts);
    m_stackAllocator.Free(aBodies);
    m_stackAllocator.Free(aStack);

    {
        dmTimer broadPhaseTimer;





        for (dm_int32 i = 0; i < bodyCeiling; ++i)
        {

            dmBody * pBody = m_bodies.Get(i);
            // line table: three tests on 641/642/643 (branch columns of each), none fused with the next
            if (pBody->m_index != i) continue;
            if (pBody->m_bodyType == dm_staticBody) continue;
            if (pBody->m_isSleeping || !pBody->m_isActive) continue;


            pBody->SynchronizeFixtures(this);
            // line table: 648/650 are two tests at loop-body depth (columns 15/26 and 15), and 652-656 sit at the same depth, so both tests continue
            if (pBody->m_bodyType != dm_dynamicBody)
                continue;
            if (pBody->IsFast(step.dt) == false)
                continue;
            pBody->m_toiCount = 0;

            dmAssert(m_toiBodyCount < m_bodies.GetCeiling());
            m_aTOIBodies[m_toiBodyCount] = i;
            m_toiBodyCount += 1;
        }
        m_broadPhase.UpdatePairs(this);
        m_broadPhaseTimer += broadPhaseTimer.GetMilliseconds();
    }
}














void dmTOIJob::Fcn(void * pParam)
{
    dmTOIJob * pJob = (dmTOIJob *)pParam;





    pJob->pWorld->ProcessTOIJob(pJob);
}






void dmWorld::ProcessTOIJob(dmTOIJob * pJob)
{






    const dm_int32 startIndex = pJob->startIndex;
    const dm_int32 finalIndex = pJob->finalIndex;
    dm_float32 * aTOIs = pJob->aTOIs;
    bool * aTOIHits = pJob->aTOIHits;
    dmMutex * pMutex = pJob->pMutex;


    for (dm_int32 i = startIndex; i < finalIndex; ++i)
    {
        if (aTOIs[i] == 1.0f)
        {
            aTOIHits[i] = false;
            continue;
        }


        dm_int32 bodyIndex1 = m_aTOIBodies[i];
        dmBody * pBody1 = m_bodies.Get(bodyIndex1);
        dmAssert(pBody1->m_index == bodyIndex1);
        dmAssert(pBody1->m_bodyType == dm_dynamicBody);
        dmAssert(pBody1->m_isSleeping == false);
        dmAssert(pBody1->m_isActive == true);


        if (pBody1->m_toiCount == 12)
        {
            pBody1->m_sweep.m_center0 = pBody1->m_sweep.m_center;
            pBody1->m_sweep.m_quaternion0 = pBody1->m_sweep.m_quaternion;
            pBody1->m_sweep.m_alpha0 = dm_vec4_one;
            pBody1->SynchronizeTransform();
            aTOIs[i] = 1.0f;
            aTOIHits[i] = false;
            continue;
        }

        dmSweep sweep1 = pBody1->m_sweep;

        bool found = false;
        dm_vec4 minEta = dm_vec4_maxFloat;

        dm_int32 aContacts[dm_maxTOIContacts];
        dm_int32 contactCount = 0;


        for (dm_int32 j = pBody1->m_fixtureList; j != -1; j = m_fixtures[j].m_ownerNext)
        {
            dmFixture * pFixture1 = m_fixtures.Get(j);

            dm_int32 contactKey = pFixture1->m_contactListKey;
            while (contactKey != -1 && contactCount < dm_maxTOIContacts)
            {
                dm_int32 contactIndex = contactKey >> 1;
                dm_int32 edgeIndex = contactKey & 1;
                dmContact * c = m_contacts.Get(contactIndex);
                contactKey = c->m_edges[edgeIndex].m_nextKey;



                dmContact::Edge * pEdge2 = c->m_edges + (edgeIndex ^ 1);
                dmFixture * pFixture2 = m_fixtures.Get(pEdge2->m_fixtureIndex);
                if (pFixture2->m_ownerType != dm_bodyOwner)
                {
                    continue;
                }

                dm_int32 bodyIndex2 = pFixture2->m_ownerIndex;
                dmBody * pBody2 = m_bodies.Get(bodyIndex2);
                if (pBody2->m_bodyType != dm_staticBody)
                {
                    continue;
                }





                aContacts[contactCount++] = contactIndex;


                dmSweep sweep2 = pBody2->m_sweep;
                sweep2.m_alpha0 = sweep1.m_alpha0;


                dmTOIOutput output;
                if (edgeIndex == 0)
                {
                    c->TimeOfImpact(&output, this, sweep1, sweep2, pMutex);
                }
                else
                {
                    c->TimeOfImpact(&output, this, sweep2, sweep1, pMutex);
                }
                if (output.exitCode != dm_toiMiss)
                {
                    if (dmLT(output.eta, minEta))
                    {
                        minEta = output.eta;
                        found = true;
                    }
                }
            }
        }
        if (!found)
        {
            pBody1->m_sweep.m_center0 = pBody1->m_sweep.m_center;
            pBody1->m_sweep.m_quaternion0 = pBody1->m_sweep.m_quaternion;
            pBody1->m_sweep.m_alpha0 = dm_vec4_one;
            pBody1->SynchronizeTransform();
            aTOIs[i] = 1.0f;
            aTOIHits[i] = false;
            continue;
        }



        dm_vec4 alpha = sweep1.m_alpha0 + (dm_vec4_one - sweep1.m_alpha0) * minEta;
        if (dmGT(alpha, vdupq_n_f32(1.0f - 100.0f * FLT_EPSILON)))
        {
            alpha = dm_vec4_one;
        }


        aTOIs[i] = dmF32(alpha);
        aTOIHits[i] = true;
        ++pBody1->m_toiCount;


        dmSweep backupSweep = pBody1->m_sweep;
        pBody1->Advance(alpha);



        bool s_once; // DWARF-only local: initialization/use did not survive optimization.
        if (contactCount == dm_maxTOIContacts)
        {
            dmLog("dm_maxTOIContacts reached. Please simplify static collision (ecatto@blizzard.com).\n");
        }




        bool touching = false;
        for (dm_int32 j = 0; j < contactCount; ++j)
        {
            dmContact * c = m_contacts.Get(aContacts[j]);



            c->UpdateManifolds(this, pMutex, true);

            if (c->m_isTouching)
            {
                touching = true;
            }
        }


        if (!touching)
        {
            pBody1->m_sweep = backupSweep;
            pBody1->m_sweep.m_center0 = pBody1->m_sweep.m_center;
            pBody1->m_sweep.m_quaternion0 = pBody1->m_sweep.m_quaternion;
            pBody1->m_sweep.m_alpha0 = dm_vec4_one;
            pBody1->SynchronizeTransform();
            aTOIs[i] = 1.0f;
            aTOIHits[i] = false;
        }
    }
}


void dmWorld::ParallelTOI(dm_float32 * aTOIs, bool * aTOIHits)
{
    if (m_toiBodyCount == 0)
        return;


    const dm_int32 k_targetBatchSize = 8;


    const dm_int32 k_maxJobCount = 512;
    dmTOIJob * aJobs = (dmTOIJob *)m_stackAllocator.Allocate(k_maxJobCount * sizeof(dmTOIJob));


    dm_int32 batchSize = k_targetBatchSize;
    if (m_toiBodyCount > k_targetBatchSize * k_maxJobCount)
    {
        batchSize = (m_toiBodyCount + k_maxJobCount - 1) / k_maxJobCount;
    }
    dmAssert(batchSize * k_maxJobCount >= m_toiBodyCount);

    dmMutex lock;
    lock.Create();

    dm_int32 itemIndex = 0;
    dm_int32 jobIndex = 0;
    for (; jobIndex < k_maxJobCount && itemIndex < m_toiBodyCount; ++jobIndex)
    {
        dm_int32 finalIndex = dmMin(m_toiBodyCount, itemIndex + batchSize);

        dmTOIJob * pJob = aJobs + jobIndex;
        pJob->pWorld = this;
        pJob->pMutex = &lock;
        pJob->aTOIHits = aTOIHits;
        pJob->aTOIs = aTOIs;
        pJob->startIndex = itemIndex;
        pJob->finalIndex = finalIndex;
        m_pJobDispatcher->Submit(dmTOIJob::Fcn, pJob);
        itemIndex = finalIndex;
    }


    m_pJobDispatcher->FinishJobs();
    lock.Destroy();

    m_counters.maxJobCount = dmMax(m_counters.maxJobCount, jobIndex);

    m_stackAllocator.Free(aJobs);
}








void dmWorld::SolveTOI(const dmTimeStep & step)
{
    const dm_int32 toiBodyCount = m_toiBodyCount;

    dm_float32 * aTOIs = (dm_float32 *)m_stackAllocator.Allocate(toiBodyCount * sizeof(dm_float32));
    bool * aTOIHits = (bool *)m_stackAllocator.Allocate(toiBodyCount * sizeof(bool));
    memset(aTOIs, 0, toiBodyCount * sizeof(dm_float32));
    memset(aTOIHits, 0, toiBodyCount * sizeof(dm_float32));

    dmIsland * pIslandList = NULL;

    dm_int32 aIslandBodies[dm_maxTOIContacts + 1];
    dm_int32 aIslandContacts[dm_maxTOIContacts];

    for (;;)
    {

        ParallelTOI(aTOIs, aTOIHits);

        bool found = false;
        for (dm_int32 i = 0; i < toiBodyCount; ++i)
        {

            if (!aTOIHits[i])
            {
                continue;
            }

            dm_int32 bodyIndex = m_aTOIBodies[i];
            dmBody * pBodyB = m_bodies.Get(bodyIndex);
            dmAssert(pBodyB->m_index == bodyIndex);
            dmAssert(pBodyB->m_bodyType == dm_dynamicBody);
            dmAssert(pBodyB->m_isSleeping == false);
            dmAssert(pBodyB->m_isActive == true);

            dm_int32 bodyCount = 0;
            aIslandBodies[bodyCount++] = bodyIndex;

            dm_int32 contactCount = 0;


            for (dm_int32 j = pBodyB->m_fixtureList; j != -1; j = m_fixtures[j].m_ownerNext)
            {
                dmFixture * f = m_fixtures.Get(j);

                dm_int32 contactKey = f->m_contactListKey;
                while (contactKey != -1 && contactCount < dm_maxTOIContacts)
                {
                    dm_int32 contactIndex = contactKey >> 1;
                    dm_int32 edgeIndex = contactKey & 1;
                    dmContact * pContact = m_contacts.Get(contactIndex);


                    contactKey = pContact->m_edges[edgeIndex].m_nextKey;

                    if (pContact->m_manifoldCount == 0)
                    {
                        continue;
                    }

                    dmContact::Edge * pEdgeA = pContact->m_edges + (edgeIndex ^ 1);
                    dmFixture * pFixtureA = m_fixtures.Get(pEdgeA->m_fixtureIndex);
                    if (pFixtureA->m_ownerType != dm_bodyOwner)
                    {
                        continue;
                    }

                    dm_int32 bodyIndexA = pFixtureA->m_ownerIndex;
                    dmBody * pBodyA = m_bodies.Get(bodyIndexA);
                    if (pBodyA->m_bodyType != dm_staticBody)
                    {
                        continue;
                    }



                    if (edgeIndex == 0)
                    {
                        pContact->m_islandIndexA = 0;
                        pContact->m_islandIndexB = bodyCount;
                    }
                    else
                    {
                        pContact->m_islandIndexA = bodyCount;
                        pContact->m_islandIndexB = 0;
                    }



                    dmAssert(bodyCount < dm_maxTOIContacts + 1);
                    aIslandContacts[contactCount++] = contactIndex;
                    aIslandBodies[bodyCount++] = bodyIndexA;
                }
            }


            dmTimeStep subStep;


            subStep.dt = (dm_vec4_one - dmV4(aTOIs[i])) * step.dt;
            subStep.inv_dt = dmSafeReciprocal(subStep.dt);
            subStep.dtRatio = dm_vec4_one;
            subStep.velocityIterations = step.velocityIterations;
            subStep.positionIterations = 20;
            subStep.useWarmStarting = false;
            subStep.useSleep = false; subStep.clearForces = false;
            dmIslandDef islandDef;
            islandDef.aBodies = aIslandBodies;
            islandDef.bodyCount = bodyCount;
            islandDef.aContacts = aIslandContacts;
            islandDef.contactCount = contactCount;
            islandDef.aJoints = NULL;
            islandDef.jointCount = 0;
            islandDef.step = subStep;
            islandDef.pWorld = this;

            dmIsland * pIsland = (dmIsland *)m_poolAllocator.Allocate(sizeof(dmIsland), false);
            pIsland->Create(&islandDef);
            pIsland->m_pNext = pIslandList;
            pIslandList = pIsland;
            found = true;
            m_pJobDispatcher->Submit(dmIsland::SolveTOIFcn, pIsland);
        }



        if (pIslandList != NULL)
        {
            m_pJobDispatcher->FinishJobs();
        }

        if (!found)
        {

            m_isStepComplete = true;
            break;
        }


        dmIsland * pNode = pIslandList;
        while (pNode != NULL)
        {
            dmIsland * pNext = pNode->m_pNext;
            pNode->Report();
            pNode->Destroy();
            m_poolAllocator.Free(pNode, sizeof(dmIsland), false);
            pNode = pNext;
        }
        pIslandList = NULL;


        for (dm_int32 i = 0; i < toiBodyCount; ++i)
        {

            if (!aTOIHits[i])
            {
                continue;
            }

            dm_int32 bodyIndex = m_aTOIBodies[i];
            dmBody * pBody = m_bodies.Get(bodyIndex);

            pBody->SynchronizeFixtures(this);
        }



        m_broadPhase.UpdatePairs(this);


        if (m_useSubStepping)
        {
            m_isStepComplete = false;
            break;
        }
    }

    m_stackAllocator.Free(aTOIHits);
    m_stackAllocator.Free(aTOIs);
}


void dmWorld::Step(dm_vec4Arg dt, dm_int32 velocityIterations, dm_int32 positionIterations, bool clearForces)
{


    dm_int32 dump; // DWARF local has no surviving location or initializer evidence.







    m_broadPhaseTimer = 0.0f;
    m_setupContraintTimer = 0.0f;
    m_velocityConstraintTimer = 0.0f;
    m_positionConstraintTimer = 0.0f;

    dmTimer stepTimer;


    if (m_newContacts)
    {
        dmTimer t;
        m_broadPhase.UpdatePairs(this);
        m_newContacts = false;
        m_broadPhaseTimer += t.GetMilliseconds();
    }

    dmTimeStep step;
    step.dt = dt;
    step.velocityIterations = velocityIterations;
    step.positionIterations = positionIterations;
    step.clearForces = clearForces;
    step.inv_dt = dmSafeReciprocal(dt);
    step.dtRatio = dmMul(dt, m_inv_dt0);
    step.useSleep = m_useSleep;
    step.useWarmStarting = m_useWarmStarting;


    dmTimer collideTimer;








    ParallelCollide();

    m_profile.collide = collideTimer.GetMilliseconds();



    if (m_isStepComplete && dmGTZ(step.dt))
    {
        dmTimer solveTimer;
        Solve(step);
        m_profile.solve = solveTimer.GetMilliseconds();
    }
    else if (m_pContactListener != NULL)
    {


        m_profile.solve = 0.0f;
        dm_int32 contactCapacity = m_contacts.GetCeiling();
        for (dm_int32 i = 0; i < contactCapacity; ++i)
        {
            dmContact * pContact = m_contacts.Get(i);
            if (pContact->m_index != i)
            {
                continue;
            }

            dmFixture * pFixtureA = m_fixtures.Get(pContact->m_edges[0].m_fixtureIndex);
            dmFixture * pFixtureB = m_fixtures.Get(pContact->m_edges[1].m_fixtureIndex);
            if (pFixtureA->m_ownerType != dm_bodyOwner || pFixtureB->m_ownerType != dm_bodyOwner)
            {
                continue;
            }


            dm_int32 manifoldCount = pContact->m_manifoldCount;
            dmManifold * aManifolds = pContact->m_aManifolds;
            for (dm_int32 k = 0; k < manifoldCount; ++k)
            {
                dmManifold * pManifold = aManifolds + k;
                dmListenerManifold m;
                m.normal = pManifold->m_normal;
                m.pointCount = pManifold->m_pointCount;
                m.fixtureIdA.worldIndex = m_index;
                m.fixtureIdA.index = pContact->m_edges[0].m_fixtureIndex;
                m.fixtureIdA.revision = pFixtureA->m_revision;
                m.fixtureIdB.worldIndex = m_index;
                m.fixtureIdB.index = pContact->m_edges[1].m_fixtureIndex;
                m.fixtureIdB.revision = pFixtureB->m_revision;

                for (dm_int32 j = 0; j < m.pointCount; ++j)
                {
                    m.points[j].position = pManifold->m_points[j].point;
                    m.points[j].normalImpulse = 0.0f;
                    m.points[j].normalVelocity = 0.0f;
                    m.points[j].slidingVelocity = 0.0f;
                    m.points[j].isNew = false;
                }
                m_pContactListener->Report(&m);
            }
        }
    }



    if (m_useContinuous && dmGTZ(step.dt))
    {
        dmTimer toiTimer;
        SolveTOI(step);
        m_profile.toi = toiTimer.GetMilliseconds();
    }

    dmAssert(m_stackAllocator.GetAllocation() == 0);

    if (dmGTZ(step.dt))
    {
        m_inv_dt0 = step.inv_dt;
    }


    m_bodies.SortFreeList();
    m_phantoms.SortFreeList();
    m_fixtures.SortFreeList();
    m_contacts.SortFreeList();
    m_meshContacts.SortFreeList();
    m_convexContacts.SortFreeList();
    m_joints.SortFreeList();
    m_distanceJoints.SortFreeList();
    m_mouseJoints.SortFreeList();
    m_prismaticJoints.SortFreeList();
    m_revoluteJoints.SortFreeList();
    m_shoulderJoints.SortFreeList();
    m_sphericalJoints.SortFreeList();
    m_weldJoints.SortFreeList();

    m_counters.maxBodyCount = dmMax(m_counters.maxBodyCount, m_bodies.GetCount());
    m_counters.maxPhantomCount = dmMax(m_counters.maxPhantomCount, m_phantoms.GetCount());
    m_counters.maxFixtureCount = dmMax(m_counters.maxFixtureCount, m_fixtures.GetCount());
    m_counters.maxConvexContactCount = dmMax(m_counters.maxConvexContactCount, m_convexContacts.GetCount());
    m_counters.maxMeshContactCount = dmMax(m_counters.maxMeshContactCount, m_meshContacts.GetCount());
    m_counters.maxDistanceJointCount = dmMax(m_counters.maxDistanceJointCount, m_distanceJoints.GetCount());
    m_counters.maxMouseJointCount = dmMax(m_counters.maxMouseJointCount, m_mouseJoints.GetCount());
    m_counters.maxPrismaticJointCount = dmMax(m_counters.maxPrismaticJointCount, m_prismaticJoints.GetCount());
    m_counters.maxRevoluteJointCount = dmMax(m_counters.maxRevoluteJointCount, m_revoluteJoints.GetCount());
    m_counters.maxShoulderJointCount = dmMax(m_counters.maxShoulderJointCount, m_shoulderJoints.GetCount());
    m_counters.maxSphericalJointCount = dmMax(m_counters.maxSphericalJointCount, m_sphericalJoints.GetCount());
    m_counters.maxWeldJointCount = dmMax(m_counters.maxWeldJointCount, m_weldJoints.GetCount());

    dm_float32 stepTime = stepTimer.GetMilliseconds();
    ++m_stepCount;
    m_totalStepTime += stepTime;

    m_profile.step = stepTime;
    m_profile.aveStep = m_totalStepTime / m_stepCount;

    m_profile.maxStep = dmMax(m_profile.maxStep, m_profile.step);
    m_profile.maxCollide = dmMax(m_profile.maxCollide, m_profile.collide);
    m_profile.maxSolve = dmMax(m_profile.maxSolve, m_profile.solve);
    m_profile.maxTOI = dmMax(m_profile.maxTOI, m_profile.toi);

    m_profile.broadPhase = m_broadPhaseTimer;
    m_profile.setupContraints = m_setupContraintTimer;
    m_profile.velocityConstraints = m_velocityConstraintTimer;
    m_profile.positionConstraints = m_positionConstraintTimer;
}



















void dmWorld::ResetIslandStamps()
{
    for (dm_int32 i = 0; i < m_bodies.GetCeiling(); ++i)
    {
        m_bodies[i].m_islandStamp = 0;
    }

    for (dm_int32 i = 0; i < m_contacts.GetCeiling(); ++i)
    {
        m_contacts[i].m_islandStamp = 0;
    }

    for (dm_int32 i = 0; i < m_joints.GetCeiling(); ++i)
    {
        m_joints[i].m_islandStamp = 0;
    }
}





























































































































































































































































































































































































































































































































































































































struct dmBroadPhaseCallback
{
    dmBroadPhaseCallback()
    {
        m_pWorld = NULL;
        m_pCollector = NULL;
        m_pUserData = NULL;
        m_pFixtures = NULL;
    }


    void QueryCallback(dm_int32 userIndex)
    {
        dmFixture * pFixture = m_pWorld->m_fixtures.Get(userIndex);
        if (dmShouldCollide(m_filter, pFixture->m_filter) == false)
        {
            return;
        }

        dmFixtureId id;
        id.worldIndex = m_pWorld->m_index;
        id.index = userIndex;
        id.revision = pFixture->m_revision;
        m_pFixtures->PushBack(id);
    }


    dmCollisionCode RayCastCallback(dm_vec4 * pLambda, const dmRayCastInput & input, dm_int32 userIndex);



























    dmWorld * m_pWorld;
    dmQueryFilter m_filter;
    dmRayCastPointCollector * m_pCollector;
    void * m_pUserData;
    dmBuffer<dmFixtureId, 32> * m_pFixtures;
};


struct dmRayCastClosestCallback
{
    dmRayCastClosestCallback()
    {
        m_pWorld = NULL;
        m_pOutput = NULL;
        m_hit = false;
    }


    dmCollisionCode RayCastCallback(dm_vec4 * pLambda, const dmRayCastInput & input, dm_int32 userIndex)
    {
        dmFixture * pFixture = m_pWorld->m_fixtures.Get(userIndex);

        if (dmShouldCollide(m_filter, pFixture->m_filter) == false)
        {
            return dm_collideContinue;
        }

        dmRayCastOutput output;
        output.triangleIndex = -1;
        output.materialID = 0;
        dmTransform xf = pFixture->GetTransform(m_pWorld);
        bool hit = pFixture->m_pShape->RayCast(&output, &input, xf);

        if (hit)
        {
            m_hit = true;
            m_pOutput->fraction = dmGetX(output.lambda);
            m_pOutput->triangleIndex = output.triangleIndex;
            m_pOutput->materialID = output.materialID;
            m_pOutput->normal = output.normal;
            m_pOutput->p = input.p1 + output.lambda * (input.p2 - input.p1);
            m_pOutput->fixtureId.worldIndex = m_pWorld->m_index;
            m_pOutput->fixtureId.index = pFixture->m_index;
            m_pOutput->fixtureId.revision = pFixture->m_revision;
            *pLambda = output.lambda;
            return dm_collideClip;
        }

        return dm_collideContinue;
    }

    dmWorld * m_pWorld;
    dmQueryFilter m_filter;
    dmWorldRayCastOutput * m_pOutput;
    bool m_hit;
};











































void dmWorld::QueryAABB(dmBuffer<dmFixtureId, 32> & fixtures, const dmAABB & aabb, const dmQueryFilter & filter)
{
    fixtures.SetCount(0);

    dmBroadPhaseCallback callback;
    callback.m_pWorld = this;
    callback.m_filter = filter;
    callback.m_pFixtures = &fixtures;

    m_broadPhase.QueryAABB(&callback, aabb);
}

// NON_MATCHING: dmDynamicTree::RayCast<dmBroadPhaseRayCastWrapper<dmRayCastClosestCallback>> (dmDynamicTree.h:228) is 1480 vs 1492 bytes; same instructions but point2 stays in a register across the loop and the loop test is cmp #0/b.gt instead of cmp #1/b.ge.
bool dmWorld::RayCastClosest(dmWorldRayCastOutput * pOutput, const dmWorldRayCastInput * pInput)
{
    dmRayCastClosestCallback callback;
    callback.m_pWorld = this;
    callback.m_filter = pInput->filter;
    callback.m_pOutput = pOutput;

    dmRayCastInput input;
    input.p1 = pInput->p1;
    input.p2 = pInput->p2;
    input.maxLambda = dm_vec4_one;
    input.triangleMask = pInput->triangleMask;
    input.slop = dmVecSet(0.0f);

    m_broadPhase.RayCast(&callback, &input);
    return callback.m_hit;
}




























































































































































































































































































































































































































































































void dmWorld::Collide(dm_int32 index, dm_int32 final, dmMutex * pMutex)
{



    for (dm_int32 i = index; i < final; ++i)
    {

        dmContact * pContact = m_contacts.Get(i);
        if (pContact->m_index != i)
        {
            continue;
        }


        dmFixture * pFixtureA = m_fixtures.Get(pContact->m_edges[0].m_fixtureIndex);
        dmFixture * pFixtureB = m_fixtures.Get(pContact->m_edges[1].m_fixtureIndex);

        dm_int32 proxyIdA = pFixtureA->m_proxyId;
        dm_int32 proxyIdB = pFixtureB->m_proxyId;
        bool overlap = m_broadPhase.TestOverlap(proxyIdA, proxyIdB);
        if (!overlap)
        {


            if (pMutex) pMutex->Lock();
            DestroyContact(i);
            if (pMutex) pMutex->Unlock();
            continue;
        }

        dmOwnerType ownerTypeA = pFixtureA->m_ownerType;
        dmOwnerType ownerTypeB = pFixtureB->m_ownerType;
        if (ownerTypeA == dm_phantomOwner || ownerTypeB == dm_phantomOwner)
        {
            continue;
        }



        dmBody * pBodyA = m_bodies.Get(pFixtureA->m_ownerIndex);
        dmBody * pBodyB = m_bodies.Get(pFixtureB->m_ownerIndex);

        dmBodyType bodyTypeA = pBodyA->m_bodyType;
        dmBodyType bodyTypeB = pBodyB->m_bodyType;

        if ((pBodyA->m_isSleeping || bodyTypeA == dm_staticBody) &&
            (pBodyB->m_isSleeping || bodyTypeB == dm_staticBody))
        {
            continue;
        }


        pContact->UpdateManifolds(this, pMutex, false);
    }
}














void dmCollideJob::Fcn(void * pParam)
{
    dmCollideJob * pJob = (dmCollideJob *)pParam;






    pJob->pWorld->Collide(pJob->index, pJob->final, pJob->pMutex);
}





void dmWorld::ParallelCollide()
{
    const dm_int32 itemCount = m_contacts.GetCeiling();
    if (itemCount == 0)
        return;


    const dm_int32 k_targetBatchSize = 32;


    const dm_int32 k_maxJobCount = 512;
    dmCollideJob * aJobs = (dmCollideJob *)m_stackAllocator.Allocate(k_maxJobCount * sizeof(dmCollideJob));


    dm_int32 batchSize = k_targetBatchSize;
    if (itemCount > k_targetBatchSize * k_maxJobCount)
    {
        batchSize = (itemCount + k_maxJobCount - 1) / k_maxJobCount;
    }
    dmAssert(batchSize * k_maxJobCount >= itemCount);

    dmMutex lock;
    lock.Create();

    dm_int32 itemIndex = 0;
    dm_int32 jobIndex = 0;
    for (; itemIndex < itemCount && jobIndex < k_maxJobCount; ++jobIndex)
    {
        dm_int32 finalIndex = dmMin(itemCount, itemIndex + batchSize);

        dmCollideJob * pJob = aJobs + jobIndex;
        pJob->pWorld = this;
        pJob->pMutex = &lock;
        pJob->index = itemIndex;
        pJob->final = finalIndex;
        m_pJobDispatcher->Submit(dmCollideJob::Fcn, pJob);
        itemIndex = finalIndex;
    }


    m_pJobDispatcher->FinishJobs();
    lock.Destroy();

    m_counters.maxJobCount = dmMax(m_counters.maxJobCount, jobIndex);

    m_stackAllocator.Free(aJobs);
}


void dmWorld::AddPair(dm_uint64 userIndexA, dm_uint64 userIndexB)
{
    dm_int32 fixtureIndexA = (dm_int32)userIndexA;
    dm_int32 fixtureIndexB = (dm_int32)userIndexB;

    if (fixtureIndexA < 0 || fixtureIndexA >= m_fixtures.GetCapacity() ||
        fixtureIndexB < 0 || fixtureIndexB >= m_fixtures.GetCapacity())
    {
        return;
    }







    dmFixture * pFixtureA = m_fixtures.Get(fixtureIndexA);
    dmFixture * pFixtureB = m_fixtures.Get(fixtureIndexB);

    dmOwnerType typeA = pFixtureA->m_ownerType;
    dmOwnerType typeB = pFixtureB->m_ownerType;

    dm_int32 ownerIndexA = pFixtureA->m_ownerIndex;
    dm_int32 ownerIndexB = pFixtureB->m_ownerIndex;

    if (typeA == typeB && ownerIndexA == ownerIndexB)
    {
        return;
    }



    if (!dmShouldCollide(pFixtureA->m_filter, pFixtureB->m_filter))
    {
        return;
    }




    if (pFixtureB->m_contactCount > pFixtureA->m_contactCount)
    {
        dm_int32 key = pFixtureA->m_contactListKey;
        while (key != -1)
        {
            dm_int32 index = key >> 1;
            dm_int32 edgeIndex = key & 1;
            const dmContact * pContact = m_contacts.Get(index);
            const dmContact::Edge * pEdge = pContact->m_edges + edgeIndex;
            const dmContact::Edge * pOtherEdge = pContact->m_edges + (edgeIndex ^ 1);
            if (pOtherEdge->m_fixtureIndex == fixtureIndexB)
            {
                return;
            }
            key = pEdge->m_nextKey;
        }
    }
    else
    {

        dm_int32 key = pFixtureB->m_contactListKey;
        while (key != -1)
        {
            dm_int32 index = key >> 1;
            dm_int32 edgeIndex = key & 1;
            const dmContact * pContact = m_contacts.Get(index);
            const dmContact::Edge * pEdge = pContact->m_edges + edgeIndex;
            const dmContact::Edge * pOtherEdge = pContact->m_edges + (edgeIndex ^ 1);
            if (pOtherEdge->m_fixtureIndex == fixtureIndexA)
            {
                return;
            }
            key = pEdge->m_nextKey;
        }
    }



    if (typeA == dm_bodyOwner && typeB == dm_bodyOwner)
    {
        dmBody * pBodyA = m_bodies.Get(ownerIndexA);
        dmBody * pBodyB = m_bodies.Get(ownerIndexB);
        if (!dmBody::ShouldCollide(this, pBodyA, pBodyB))
        {
            return;
        }
    }


    CreateContact(fixtureIndexA, fixtureIndexB);
}















































































dmWorldId dmWorld_Create(const dmWorldDef * pDef)
{
    dmWorldId id = dm_nullWorldId;


    for (dm_int32 i = 0; i < 65534; ++i)
    {
        if (dmWorld::s_aWorlds[i] == NULL)
        {
            dmWorld * pWorld = (dmWorld *)dmAlloc(sizeof(dmWorld));
            pWorld->Create(pDef);
            dmWorld::s_aWorlds[i] = pWorld;
            id.index = (dm_uint16)i;
            pWorld->m_index = id.index;
            break;
        }
    }


    return id;
}

void dmWorld_Destroy(dmWorldId worldId)
{
    dm_int32 index = worldId.index;
    if (index >= 65534 || dmWorld::s_aWorlds[worldId.index] == NULL)
    {
        return;
    }
    dmWorld::s_aWorlds[index]->Destroy();
    dmFree(dmWorld::s_aWorlds[index]);
    dmWorld::s_aWorlds[index] = NULL;
}

dmBodyId dmWorld_CreateBody(dmWorldId id, const dmBodyDef * pDef)
{
    if (id.index >= 65534) return dm_nullBodyId; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return dm_nullBodyId;

    if (dmIsFinite3(pDef->m_position) == false)
    {
        dmLog("Invalid position used to create body %s.\n", pDef->m_pName);
        return dm_nullBodyId;
    }

    if (dmIsFinite4(pDef->m_quaternion) == false || dmIsZeroSafe4(pDef->m_quaternion) == false)
    {

        dmLog("Invalid quaternion used to create body %s.\n", pDef->m_pName);
        return dm_nullBodyId;
    }



    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return dm_nullBodyId;
    }


    dm_int32 index = pWorld->CreateBody(pDef);

    if (index == -1)
    {
        dmWarn("Domino body capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_bodies.GetCapacity());
        return dm_nullBodyId;
    }

    dmBodyId bodyId;
    bodyId.index = index;
    bodyId.worldIndex = id.index;
    bodyId.revision = pWorld->m_bodies[index].m_revision;
    return bodyId;
}

void dmWorld_DestroyBody(dmBodyId id)
{
    if (id.worldIndex >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex]; if (pWorld == NULL) return;



    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }


    if (pWorld->m_bodies.IsValid(id.index) == false)
    {
        return;
    }


    if (pWorld->m_bodies[id.index].m_revision != id.revision)
    {
        return;
    }


    pWorld->DestroyBody(id.index);
}






































































void dmWorld_SetListener(dmWorldId id, dmContactListener * pListener)
{
    if (id.index >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return;


    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }



    pWorld->m_pContactListener = pListener;
}
















void dmWorld_SetJobDispatcher(dmWorldId id, dmJobDispatcher * pDispatcher)
{
    if (id.index >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return;


    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }



    pWorld->m_pJobDispatcher = pDispatcher;
}










































dmJointId dmWorld_CreateJoint(dmWorldId id, const dmJointDef * pDef)
{
    if (id.index >= 65534) return dm_nullJointId; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return dm_nullJointId;



    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return dm_nullJointId;
    }



    dm_int32 index = pWorld->CreateJoint(pDef);
    if (index == -1)
    {
        return dm_nullJointId;
    }

    dmJointId jointId;
    jointId.index = index;
    jointId.worldIndex = id.index;
    jointId.revision = pWorld->m_joints[index].m_revision;
    return jointId;
}

void dmWorld_DestroyJoint(dmJointId id)
{
    if (id.worldIndex >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex]; if (pWorld == NULL) return;



    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }


    if (pWorld->m_joints.IsValid(id.index) == false)
    {
        return;
    }


    if (pWorld->m_joints[id.index].m_revision != id.revision)
    {
        return;
    }


    pWorld->DestroyJoint(id.index);
}


dmBodyId dmWorld_GetFirstBody(dmWorldId id)
{
    if (id.index >= 65534) return dm_nullBodyId; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return dm_nullBodyId;



    const dm_int32 count = pWorld->m_bodies.GetCount();
    for (dm_int32 i = 0; i < count; ++i)
    {
        if (pWorld->m_bodies[i].m_index == i)
        {
            dmBodyId bodyId;
            bodyId.index = i;
            bodyId.revision = pWorld->m_bodies[i].m_revision;
            bodyId.worldIndex = id.index;
            return bodyId;
        }
    }

    return dm_nullBodyId;
}


dmBodyId dmWorld_GetNextBody(dmBodyId id)
{
    if (id.worldIndex >= 65534) return dm_nullBodyId; dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex]; if (pWorld == NULL) return dm_nullBodyId;



    if (pWorld->m_bodies.IsValid(id.index) == false)
    {
        return dm_nullBodyId;
    }



    dmBody * pBody = &pWorld->m_bodies[id.index];
    if (pBody->m_revision != id.revision)
    {
        return dm_nullBodyId;
    }


    dm_int32 ceiling = pWorld->m_bodies.GetCeiling();
    for (dm_int32 i = id.index + 1; i < ceiling; ++i)
    {

        if (pWorld->m_bodies[i].m_index == i)
        {
            dmBodyId bodyId;
            bodyId.index = i;
            bodyId.revision = pWorld->m_bodies[i].m_revision;
            bodyId.worldIndex = id.worldIndex;
            return bodyId;
        }
    }

    return dm_nullBodyId;
}
























// dmBroadPhase::QueryAABB and dmDynamicTree::QueryAABB inline here (dmBroadPhase.h:268, dmDynamicTree.h:184).
void dmWorld_QueryAABB(dmWorldId id, dmBuffer<dmFixtureId, 32> & fixtures, dm_vec4Arg lowerBound, dm_vec4Arg upperBound, const dmQueryFilter & filter)
{
    if (id.index >= 65534 || dmWorld::s_aWorlds[id.index] == NULL) return;

    dmWorld * pWorld = dmWorld::s_aWorlds[id.index];
    dmAABB aabb;
    aabb.m_lowerBound = lowerBound;
    aabb.m_upperBound = upperBound;
    pWorld->QueryAABB(fixtures, aabb, filter);
}

// dmBroadPhase::RayCast inlines here (dmBroadPhase.h:292); the tree walk stays a call as in the original.
bool dmWorld_RayCastClosest(dmWorldId id, dmWorldRayCastOutput * pOutput, const dmWorldRayCastInput * pInput)
{
    if (id.index >= 65534 || dmWorld::s_aWorlds[id.index] == NULL) return false;

    dmWorld * pWorld = dmWorld::s_aWorlds[id.index];
    return pWorld->RayCastClosest(pOutput, pInput);
}



























































































































































void dmWorld_EnableContinuous(dmWorldId id, bool flag)
{
    if (id.index >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return;



    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }


    pWorld->m_useContinuous = flag;
}




























































void dmWorld_Step(dmWorldId id, dm_float32 timeStep, dm_int32 velocityIterations, dm_int32 positionIterations, bool clearForces)
{

    dmMathTest();
    dmCollisionTest();

    if (id.index >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return;



    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }



    pWorld->m_lock.StoreRelease(1);
    pWorld->Step(dmV4(timeStep), velocityIterations, positionIterations, clearForces);
    pWorld->m_lock.StoreRelease(0);
}

void dmWorld_Lock(dmWorldId id)
{
    if (id.index >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return;

    pWorld->m_lock.StoreRelease(1);
}


void dmWorld_Unlock(dmWorldId id)
{
    if (id.index >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.index]; if (pWorld == NULL) return;

    pWorld->m_lock.StoreRelease(0);
}
