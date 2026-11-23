#include "dmContact.h"
#include "dmConvexContact.h"
#include "dmMeshContact.h"
#include "../dmWorld.h"
#include "../../Collision/Functions/dmTimeOfImpact.h"

void dmContact::UpdateManifolds(dmWorld* pWorld, dmMutex* pMutex, bool isTOIEvent)
{
    switch (m_type) {
    case dm_convexContact:
        m_isTouching = pWorld->m_convexContacts[m_subIndex].UpdateManifold(pWorld, this, pMutex, isTOIEvent);
        break;
    case dm_meshContact:
        m_isTouching = pWorld->m_meshContacts[m_subIndex].UpdateManifolds(pWorld, this, pMutex, isTOIEvent);
        break;
    default:
        m_isTouching = false;
        break;
    }
}

void dmContact::TimeOfImpact(dmTOIOutput* pOutput, dmWorld* pWorld, const dmSweep& sweepA, const dmSweep& sweepB, dmMutex* pMutex)
{
    pOutput->exitCode = dm_toiMiss;
    switch (m_type) {
    case dm_convexContact:
        pWorld->m_convexContacts[m_subIndex].TimeOfImpact(pOutput, pWorld, this, sweepA, sweepB);
        break;
    case dm_meshContact:
        pWorld->m_meshContacts[m_subIndex].TimeOfImpact(pOutput, pWorld, this, sweepA, sweepB, pMutex);
        break;
    default:
        break;
    }
}

#include "../dmFixture.h"
#include <math.h>

bool dmContact::Create(dmWorld* pWorld, dm_int32 fixtureIndexA, dm_int32 fixtureIndexB)
{
    m_edges[0].m_fixtureIndex = fixtureIndexA;
    m_edges[1].m_fixtureIndex = fixtureIndexB;
    dmFixture* pFixtureA = pWorld->m_fixtures.Get(fixtureIndexA);
    dmFixture* pFixtureB = pWorld->m_fixtures.Get(fixtureIndexB);
    m_edges[0].m_ownerIndex = pFixtureA->m_ownerIndex;
    m_edges[1].m_ownerIndex = pFixtureB->m_ownerIndex;
    dmShape::Type typeA = pFixtureA->m_pShape->m_type;
    dmShape::Type typeB = pFixtureB->m_pShape->m_type;
#line 48 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/Contacts/dmContact.cpp"
    dmAssert(typeA >= typeB);
    m_isTouching = false;
    m_toiValid = false;
    m_isSolid = true;
    if (pFixtureA->m_isSensor || pFixtureB->m_isSensor) m_isSolid = false;
    m_aManifolds = 0;
    m_manifoldCount = 0;
    m_friction = sqrtf(pFixtureA->m_friction * pFixtureB->m_friction);
    m_restitution = dmMax(pFixtureA->m_restitution, pFixtureB->m_restitution);
    dm_float32 rollingResistanceA = pFixtureA->m_rollingResistance * pFixtureA->m_pShape->m_radius;
    dm_float32 rollingResistanceB = pFixtureB->m_rollingResistance * pFixtureB->m_pShape->m_radius;
    m_rollingResistance = dmMax(rollingResistanceA, rollingResistanceB);
    dmOwnerType ownerTypeA = pFixtureA->m_ownerType;
    dmOwnerType ownerTypeB = pFixtureB->m_ownerType;
    if (ownerTypeA == dm_phantomOwner || ownerTypeB == dm_phantomOwner) m_isSolid = false;
    m_islandStamp = 0;
    m_islandIndexA = -1;
    m_islandIndexB = -1;
    if (typeA == dmShape::e_mesh)
    {
        m_type = dm_meshContact;
        m_subIndex = pWorld->m_meshContacts.Allocate();
        if (m_subIndex == -1) return false;
        pWorld->m_meshContacts[m_subIndex].Create();
    }
    else
    {
        m_type = dm_convexContact;
        m_subIndex = pWorld->m_convexContacts.Allocate();
        if (m_subIndex == -1) return false;
        pWorld->m_convexContacts[m_subIndex].Create(typeA, typeB);
    }
    Edge* pEdgeA = m_edges;
    dm_int32 keyA = m_index << 1;
    pEdgeA->m_prevKey = -1;
    pEdgeA->m_nextKey = pFixtureA->m_contactListKey;
    pFixtureA->m_contactListKey = keyA;
    dm_int32 nA = pEdgeA->m_nextKey;
    if (nA != -1) pWorld->m_contacts[nA >> 1].m_edges[nA & 1].m_prevKey = keyA;
    ++pFixtureA->m_contactCount;
    Edge* pEdgeB = m_edges + 1;
    dm_int32 keyB = (m_index << 1) | 1;
    pEdgeB->m_prevKey = -1;
    pEdgeB->m_nextKey = pFixtureB->m_contactListKey;
    pFixtureB->m_contactListKey = keyB;
    dm_int32 nB = pEdgeB->m_nextKey;
    if (nB != -1) pWorld->m_contacts[nB >> 1].m_edges[nB & 1].m_prevKey = keyB;
    ++pFixtureB->m_contactCount;
    ++m_revision;
    return true;
}
#line 137
void dmContact::Destroy(dmWorld* pWorld)
{
	dmFixture* pFixtureA = pWorld->m_fixtures.Get(m_edges[0].m_fixtureIndex);
	dmFixture* pFixtureB = pWorld->m_fixtures.Get(m_edges[1].m_fixtureIndex);

	Edge* pEdgeA = m_edges;
	dm_int32 pA = pEdgeA->m_prevKey;
	dm_int32 nA = pEdgeA->m_nextKey;


	if (pA != -1)
	{
		pWorld->m_contacts.Get(pA >> 1)->m_edges[pA & 1].m_nextKey = nA;
	}

	if (nA != -1)
	{
		pWorld->m_contacts.Get(nA >> 1)->m_edges[nA & 1].m_prevKey = pA;
	}

	// Columns 158:19/27 and 159:25 (member after "keyA == pFixtureA->") give the parenthesised key and the operand order.
	dm_int32 keyA = (m_index << 1);
	if (keyA == pFixtureA->m_contactListKey)
	{
		pFixtureA->m_contactListKey = nA;
	}

	--pFixtureA->m_contactCount;

	Edge* pEdgeB = m_edges + 1;
	dm_int32 pB = pEdgeB->m_prevKey;
	dm_int32 nB = pEdgeB->m_nextKey;


	if (pB != -1)
	{
		pWorld->m_contacts.Get(pB >> 1)->m_edges[pB & 1].m_nextKey = nB;
	}

	if (nB != -1)
	{
		pWorld->m_contacts.Get(nB >> 1)->m_edges[nB & 1].m_prevKey = pB;
	}


	dm_int32 keyB = (m_index << 1) | 1;
	if (keyB == pFixtureB->m_contactListKey)
	{
		pFixtureB->m_contactListKey = nB;
	}

	--pFixtureB->m_contactCount;

	if (m_manifoldCount > 0)
	{
		pWorld->m_poolAllocator.Free(m_aManifolds, m_manifoldCount * sizeof(dmManifold), false);
		m_aManifolds = 0;
		m_manifoldCount = 0;
	}



	if (m_type == dm_convexContact)
	{
		pWorld->m_convexContacts.Free(m_subIndex);
	}
	else
	{
		pWorld->m_meshContacts[m_subIndex].Destroy(pWorld);
		pWorld->m_meshContacts.Free(m_subIndex);
	}
}
