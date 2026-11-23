// Original file: Contrib/Contrib/Domino/Dynamics/dmBody.cpp
#include "dmBody.h"
#include "dmWorld.h"
#include "dmFixture.h"
#include "Joints/dmJoint.h"
#include "Contacts/dmContact.h"
#include "../Include/dmApi.h"
#include "../Collision/Functions/dmDistance.h"
#include "../Include/dmBuffer.h"
#include "../Include/dmId.h"
#include "../Include/dmSettings.h"
#include "../Include/dmShapes.h"
#include <float.h>
#include <string.h>

extern const dmBodyId dm_nullBodyId;
// Invented declaration (DWARF has only a declaration of class dmDestructionListener): Destroy calls vtable slots 2/3 with dmJointId / dmFixtureId; the name SayGoodbye fits columns 168:47 and 190:47.
class dmDestructionListener
{ public: virtual ~dmDestructionListener();
    virtual void SayGoodbye(dmJointId jointId) = 0; virtual void SayGoodbye(dmFixtureId fixtureId) = 0;
};
const dmBodyId dm_nullBodyId = { -1, 0, 0xFFFF };


bool operator==(dmBodyId id1, dmBodyId id2)
{
    return id1.worldIndex == id2.worldIndex && id1.index == id2.index && id1.revision == id2.revision;
}


bool operator!=(dmBodyId id1, dmBodyId id2)
{
    return id1.worldIndex != id2.worldIndex || id1.index != id2.index || id1.revision != id2.revision;
}
// dmMath.inl:2531 (DWARF _Z15dmDeterminant33RK5dmMtx, param m, inlined only; calls dmScalarTripleProduct at 2533). Declared here to avoid a shared header edit.
inline dm_vec4 dmDeterminant33(const dmMtx & m) { return dmScalarTripleProduct(m.rx, m.ry, m.rz); }
inline bool sValidateInertia(const dmMtx & I)
{
	if (dmIsFinite3(I.rx) == false || dmIsFinite3(I.ry) == false || dmIsFinite3(I.rz) == false)
	{
		return false;
	}


	dm_vec4 det = dmDeterminant33(I);
	if (dmIsFinite4(det) == false || dmGetX(det) < 0.0f)
	{
		return false;
	}


	if (dmGetY(I.rx) != dmGetX(I.ry) || dmGetZ(I.rx) != dmGetX(I.rz) || dmGetZ(I.ry) != dmGetY(I.rz))
	{
		return false;
	}

	return true;
}
// dmMath.inl:4216 (DWARF _Z22dmShiftInertiaToCenterRK5dmMtxRK13__Float32x4_tf, inlined only; calls dmShiftInertiaToOrigin at 4219 col 38 with the mass negated). Declared here to avoid a shared header edit.
inline dmMtx dmShiftInertiaToCenter(const dmMtx & I, dm_vec4Arg c, dm_float32 mass) { return dmShiftInertiaToOrigin(I, c, -mass); }

void dmBody::Create(const dmBodyDef * pDef)
{

	dm_vec4 q = dmNormalize4(pDef->m_quaternion);
	m_transform.q = q;
	m_transform.p = pDef->m_position;
	// Inference: line 70 has no row of its own; its -1 store is merged into the 8-byte store at 69:16.
	m_fixtureList = -1;
	m_jointListKey = -1;

	m_isActive = pDef->m_isActive;
	m_pUserData = pDef->m_pUserData;


	if (pDef->m_pName)
	{
		for (dm_int32 i = 0; i < 15; ++i)
		{
			m_name[i] = pDef->m_pName[i];
			if (m_name[i] == 0)
			{
				break;
			}
		}
		m_name[15] = 0;
	}
	else
	{
		m_name[0] = 0;
	}

	m_bodyType = pDef->m_bodyType;

	m_isSleeping = pDef->m_isSleeping;
	m_enableSleeping = pDef->m_enableSleeping;


	m_isBullet = pDef->m_isBullet;

	m_isActive = pDef->m_isActive;
	m_isStabilized = false;
	// Inference: 105-108 order chosen so the paired sweep stores schedule as in the original (106 and 108 leave no rows).
	m_sweep.m_quaternion0 = q;
	m_sweep.m_localCenter = dmVecSet(0.0f);
	m_sweep.m_quaternion = q;
	m_sweep.m_center0 = m_sweep.m_center = m_transform.p;
	m_sweep.m_alpha0 = dmVecSet(0.0f);
	m_linearDamping = pDef->m_linearDamping;
	m_angularDamping = pDef->m_angularDamping;
	m_inertiaScale = pDef->m_inertiaScale;
	m_gravityScale = pDef->m_gravityScale;




	m_linearVelocity = dmVecSet(0.0f);
	m_angularVelocity = dmVecSet(0.0f);

	m_islandStamp = 0;

	m_sleepTime = 0.0f;
	// Inference: the original keeps a second zero store to m_sweep.m_localCenter here (124:24) after the one at 105.
	m_sweep.m_localCenter = dmVecSet(0.0f);
	memset(&m_I, 0, sizeof(dmMtx));
	memset(&m_invI, 0, sizeof(dmMtx));
	m_force = dmVecSet(0.0f);
	m_torque = dmVecSet(0.0f);
	if (m_bodyType == dm_dynamicBody)
	{
		m_mass = 1.0f;
		m_invMass = 1.0f;
	}
	else
	{
		m_mass = 0.0f;
		m_invMass = 0.0f;
	}

	m_minRadius = 0.0f;
	m_maxRadius = 0.0f;

	++m_revision;

	m_toiCount = 0;
}
// dmMath.inl:1685 (DWARF _ZpLR5dmMtxRKS_, inlined only; dmAdd at 1687-1690). Declared here to avoid a shared header edit.
inline dmMtx & operator+=(dmMtx & a, const dmMtx & b) { a.rx = dmAdd(a.rx, b.rx); a.ry = dmAdd(a.ry, b.ry); a.rz = dmAdd(a.rz, b.rz); a.p = dmAdd(a.p, b.p); return a; }
void dmBody::Destroy(dmWorld * pWorld)
{

	dm_int32 jointKey = m_jointListKey;
	while (jointKey != -1)
	{
		dm_int32 jointIndex = jointKey >> 1;
		dm_int32 edgeIndex = jointKey & 1;


		dmJoint * pJoint = pWorld->m_joints.Get(jointIndex);
		jointKey = pJoint->m_edges[edgeIndex].m_nextKey;

		if (pWorld->m_pDestructionListener)
		{
			dmJointId jointId;
			jointId.index = jointIndex;
			jointId.worldIndex = pWorld->m_index;
			jointId.revision = pJoint->m_revision;
			pWorld->m_pDestructionListener->SayGoodbye(jointId);
		}


		pJoint->Destroy(pWorld);


		pWorld->m_joints.Free(jointIndex);
	}
	m_jointListKey = -1;


	dm_int32 fixtureIndex = m_fixtureList;
	while (fixtureIndex != -1)
	{
		dm_int32 next = pWorld->m_fixtures[fixtureIndex].m_ownerNext;
		if (pWorld->m_pDestructionListener)
		{
			dmFixtureId fixtureId;
			fixtureId.index = fixtureIndex;
			fixtureId.worldIndex = pWorld->m_index;
			fixtureId.revision = pWorld->m_fixtures[fixtureIndex].m_revision;
			pWorld->m_pDestructionListener->SayGoodbye(fixtureId);
		}

		DestroyFixture(pWorld, fixtureIndex);
		fixtureIndex = next;
	}
	m_fixtureList = -1;
}


dm_int32 dmBody::CreateFixture(dmWorld * pWorld, const dmFixtureDef * pDef)
{
    dm_int32 fixtureIndex = pWorld->m_fixtures.Allocate();
    if (fixtureIndex == -1)
    {
        return -1;
    }

    dmFixture * pFixture = pWorld->m_fixtures.Get(fixtureIndex);
    pFixture->m_index = fixtureIndex;


    pFixture->Create(pWorld, pDef);
    pFixture->m_ownerType = dm_bodyOwner;
    pFixture->m_ownerIndex = m_index;


    pFixture->m_ownerNext = m_fixtureList;
    m_fixtureList = fixtureIndex;

    if (m_isActive)
    {
        pFixture->CreateProxy(pWorld, dmMtxFromTransform(m_transform));
    }




    pWorld->m_newContacts = true;

    if (m_bodyType == dm_dynamicBody)
    {
        SetMassFromFixtures(pWorld);
    }
    // Inferred return spelling: the original returns the allocated index on both successful paths.
    return fixtureIndex;
}


void dmBody::DestroyFixture(dmWorld * pWorld, dm_int32 fixtureIndex)
{

	dm_int32 prev = -1;
	dm_int32 node = m_fixtureList;
	bool found = false;
	while (node != -1)
	{
		dm_int32 next = pWorld->m_fixtures[node].m_ownerNext;

		if (node == fixtureIndex)
		{
			if (prev == -1)
			{
				m_fixtureList = next;
			}
			else
			{
				pWorld->m_fixtures[prev].m_ownerNext = next;
			}

			found = true;
			break;
		}

		prev = node;
		node = next;
	}

	// Inference: DestroyFixture is linker-dropped (concrete DIE low_pc 0); 270-274 from its inlined copy in Destroy, closing line not recorded.

	dmFixture * pFixture = pWorld->m_fixtures.Get(fixtureIndex);
	pFixture->Destroy(pWorld);
	pFixture->m_ownerIndex = -1;
	pFixture->m_ownerNext = -1;
	pWorld->m_fixtures.Free(fixtureIndex);
}



// NON_MATCHING: 1132/1132 bytes, 39 of 283 instructions differ, register names only: v20/v21 swapped for center vs I.rz through the loop and the closing scaled center. The m_center0 store at 348 (missing before) fixed the end-of-function stores. Tried: m_invI = m_I (memset, worse), dmMtx I = dmMtx(), DWARF-shaped dmRotate/dmCross (no change).
void dmBody::SetMassFromFixtures(dmWorld * pWorld)
{
	dmAssert(m_bodyType == dm_dynamicBody);
	// Inferences: I zeroed by memset (no row; follows SetType), valid at 297 includes density >= 0.0f (ccmp ge); 340 (no row) zero vectors sunk into the shared m_invI store need a vector-typed zero, so the chained member form is used, rx assigned first (original materialises rx's zero first and copies it to ry/rz/p); 339 m_I zero stores are xzr (dmMtx()).
	dmMtx I;
	dm_vec4 center = dmVecSet(0.0f);
	dm_float32 mass = 0.0f;
	memset(&I, 0, sizeof(dmMtx));
	m_minRadius = FLT_MAX;
	m_maxRadius = 0.0f;


	for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
	{
		dmFixture * pFixture = pWorld->m_fixtures.Get(i);

		dm_float32 density = pFixture->m_density;
		bool valid = dmIsFinite(density) && density >= 0.0f;


		if (valid && density > 0.0f)
		{
			dmMassData massData;
			pFixture->m_pShape->ComputeMass(&massData, density);
			I += massData.I;
			center += dmVecSet(massData.mass) * massData.center;
			mass += massData.mass;
		}

		m_minRadius = dmMin(m_minRadius, pFixture->m_minRadius);
		m_maxRadius = dmMax(m_maxRadius, pFixture->m_maxRadius);
	}


	if (dmIsFinite(mass) && mass > 1000.0f * FLT_MIN)
	{
		m_mass = mass;
		m_invMass = 1.0f / mass;

		center = dmVecSet(m_invMass) * center;
		I = dmShiftInertiaToCenter(I, center, mass);
	}
	else
	{

		m_mass = 0.0f;
		m_invMass = 0.0f;
		center = dmVecSet(0.0f);
	}


	dm_vec4 det = dmDeterminant33(I);
	if (dmIsFinite4(det) && dmGetX(det) > 1000.0f * FLT_MIN)
	{
		m_I = m_inertiaScale * I;
		m_invI = dmInvertSym33(m_I);
	}
	else
	{
		m_I = dmMtx();
		m_invI.p = m_invI.rz = m_invI.ry = m_invI.rx = dmVecSet(0.0f);
	}


	m_sweep.m_localCenter = center;


	m_sweep.m_center = m_transform.p + dmRotate(m_sweep.m_quaternion, center);
	m_sweep.m_center0 = m_sweep.m_center;  // No row: the original writes center0 (0x80) in the stp with m_localCenter (344:24), after the 347 store.
}





































void dmBody::SetType(dmWorld * pWorld, dmBodyType type)
{
    if (m_bodyType == type)
    {
        return;
    }

    m_bodyType = type;

    switch (type)
    {
    case dm_staticBody:
        m_mass = 0.0f;
        m_invMass = 0.0f;
        memset(&m_I, 0, sizeof(dmMtx));
        memset(&m_invI, 0, sizeof(dmMtx));
        m_linearVelocity = dmVecSet(0.0f);
        m_angularVelocity = dmVecSet(0.0f);
        break;

    case dm_dynamicBody:
        SetMassFromFixtures(pWorld);
        break;

    case dm_kinematicBody:
        m_mass = 0.0f;
        m_invMass = 0.0f;
        memset(&m_I, 0, sizeof(dmMtx));
        memset(&m_invI, 0, sizeof(dmMtx));
        break;

    default:
        break;
    }

    m_force = dmVecSet(0.0f);
    m_torque = dmVecSet(0.0f);

    // Since the body type changed, we need to flag contacts for filtering.
    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        pWorld->m_fixtures[i].DestroyContacts(pWorld);


        pWorld->m_fixtures[i].TouchProxy(pWorld);
    }



    pWorld->m_newContacts = true;
}





























void dmBody::SetTransform(dmWorld * pWorld, const dmTransform & xf)
{

    m_transform.q = dmNormalize4(xf.q);
    m_transform.p = xf.p;
    // Inferred placement: the quaternion stores are merged with the center stores at lines 476-477.
    m_sweep.m_quaternion = m_transform.q;
    // Inferred placement: the previous quaternion store is merged into the center0 pair at line 477.
    m_sweep.m_quaternion0 = m_sweep.m_quaternion;
    m_sweep.m_center = dmMul(m_transform, m_sweep.m_localCenter);
    m_sweep.m_center0 = m_sweep.m_center;

    dmMtx T = dmMtxFromTransform(m_transform); // Inferred loop spelling below follows the ownerNext chain and -1 sentinel.
    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        dmFixture * f = pWorld->m_fixtures.Get(i);
        f->Synchronize(pWorld, T);
    }
}

// Needed by dmBody_SetTransformNoSync; DWARF07d22005 low_pc0, so the discarded standalone body is not checkable.
void dmBody::SetTransformNoSync(const dmTransform & xf)
{

    m_transform.q = dmNormalize4(xf.q);
    m_transform.p = xf.p;
    // Inference: quaternion assignment494 is merged into the center/quaternion pair497, as in SetTransform.
    m_sweep.m_quaternion = m_transform.q;
    // Inference: previous quaternion assignment is merged into the second pair498.
    m_sweep.m_quaternion0 = m_sweep.m_quaternion;
    m_sweep.m_center = dmMul(m_transform, m_sweep.m_localCenter);
    m_sweep.m_center0 = m_sweep.m_center;
}




void dmBody::SynchronizeFixtures(dmWorld * pWorld)
{

    dmTransform transform0;
    transform0.q = m_sweep.m_quaternion0;
    transform0.p = m_sweep.m_center0 - dmRotate(m_sweep.m_quaternion0, m_sweep.m_localCenter);

    dmMtx T0 = dmMtxFromTransform(transform0);
    dmMtx T = dmMtxFromTransform(m_transform);

    // Inferred for-loop spelling: the original walks ownerNext until the -1 fixture sentinel.
    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        dmFixture * f = pWorld->m_fixtures.Get(i);
        f->Synchronize(pWorld, T0, T);
    }
}


















dm_vec4 dmBody::ComputeSurfaceArea(dmWorld * pWorld) const
{
    dm_vec4 area = dmVecSet(0.0f);

    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        area += pWorld->m_fixtures.Get(i)->
            m_pShape->ComputeSurfaceArea();
    }

    return area;
}


dm_vec4 dmBody::ComputeProjectedSurfaceArea(dmWorld * pWorld, dm_vec4Arg dir) const
{
    dm_vec4 area = dmVecSet(0.0f);

    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        area += pWorld->m_fixtures.Get(i)->
            m_pShape->ComputeProjectedSurfaceArea(dir);
    }

    return area;
}


dm_vec4 dmBody::ComputeVolume(dmWorld * pWorld) const
{
    dm_vec4 volume = dmVecSet(0.0f);

    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        volume += pWorld->m_fixtures.Get(i)->
            m_pShape->ComputeVolume();
    }

    return volume;
}


dm_vec4 dmBody::ComputeSubmergedVolume(dmWorld * pWorld, dm_vec4 * pCentroid, dm_vec4Arg normal, dm_vec4Arg offset) const
{
    dm_vec4 volume = dmVecSet(0.0f);
    dm_vec4 centroid = dmVecSet(0.0f);
    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        const dmShape * pShape = pWorld->m_fixtures.Get(i)->m_pShape;
        dm_vec4 c;
        dm_vec4 v = pShape->ComputeSubmergedVolume(&c, m_transform, normal, offset);
        volume += v;
        centroid += v * c;
    }


    if (dmGTZ(volume))
    {
        centroid /= volume;
    }

    *pCentroid = centroid;
    return volume;
}















































// dmMath.inl:147 (DWARF _Z5dmMaxIfET_S0_S0_S0_, inlined only; calls dmMax(dm_float32, dm_float32) twice from its line 149). Declared here, not in dmMath.inl, to avoid a shared header edit; nesting order from StabilizeTreeRecursive (y/z compared first).
template <typename T> inline T dmMax(T a, T b, T c)
{
    return dmMax(a, dmMax(b, c));
}






void dmBody::StabilizeTree(dmWorld * pWorld, dm_float32 inertiaScale, dm_float32 massRatio)
{


	dmBuffer<dmBody *, 64> bodies;

	BuildIsland(pWorld, bodies);


	dm_float32 totalMass1 = 0.0f;
	for (dm_int32 i = 0; i < bodies.GetCount(); ++i)
	{
		bodies[i]->m_isStabilized = false;
		totalMass1 += bodies[i]->m_mass;
	}


	StabilizeTreeRecursive(pWorld, inertiaScale, massRatio, NULL);


	dm_float32 totalMass2 = 0.0f;
	for (dm_int32 i = 0; i < bodies.GetCount(); ++i)
	{
		bodies[i]->m_isStabilized = false;
		totalMass2 += bodies[i]->m_mass;
	}

	if (totalMass1 == 0.0f || totalMass2 == 0.0f)
	{
		return;
	}
	// Inference: the failure exits at 690/707/713/719 are spelled return (all branch to the buffer destructor).


	dm_float32 ratio = totalMass1 / totalMass2;
	dm_float32 invRatio = totalMass2 / totalMass1;
	for (dm_int32 i = 0; i < bodies.GetCount(); ++i)
	{
		dmBody* pBody = bodies[i];
		dm_float32 mass = ratio * pBody->m_mass;
		dm_float32 invMass = invRatio * pBody->m_invMass;
		dmMtx I = ratio * pBody->m_I;
		dmMtx invI = invRatio * pBody->m_invI;

		if (dmIsFinite(mass) == false || mass < 0.0f)
		{
			return;
		}


		if (dmIsFinite(invMass) == false || invMass < 0.0f)
		{
			return;
		}


		if (sValidateInertia(I) == false || sValidateInertia(invI) == false)
		{
			return;
		}

		pBody->m_mass = mass;
		pBody->m_invMass = invMass;
		pBody->m_I = I;
		pBody->m_invI = invI;
	}
}


void dmBody::BuildIsland(dmWorld * pWorld, dmBuffer<dmBody *, 64> & buffer)
{
    buffer.PushBack(this);
    m_isStabilized = true;

    dm_int32 jointKey = m_jointListKey;
    while (jointKey != -1)
    {
        dm_int32 jointIndex = jointKey >> 1;
        dm_int32 edgeIndex = jointKey & 1;


        dmJoint * pJoint = pWorld->m_joints.Get(jointIndex);
        jointKey = pJoint->m_edges[edgeIndex].m_nextKey;

        dm_int32 otherIndex = pJoint->m_edges[edgeIndex ^ 1].m_bodyIndex;

        dmBody * pOther = pWorld->m_bodies.Get(otherIndex);
        if (pOther->m_isStabilized)
        {
            continue;
        }


        pOther->BuildIsland(pWorld, buffer);
    }
}

void dmBody::StabilizeTreeRecursive(dmWorld * pWorld, dm_float32 inertiaScale, dm_float32 massRatio, const dmBody * pParent)
{

	m_isStabilized = true;


	if (m_bodyType == dm_dynamicBody)
	{



		dm_float32 maxJointLeverArm = 0.0f;
		dm_int32 jointKey = m_jointListKey;
		while (jointKey != -1)
		{
			dm_int32 jointIndex = jointKey >> 1;
			dm_int32 edgeIndex = jointKey & 1;


			dmJoint * pJoint = pWorld->m_joints.Get(jointIndex);
			jointKey = pJoint->m_edges[edgeIndex].m_nextKey;

			dm_float32 jointLeverArm;
			if (edgeIndex == 0)
			{
				jointLeverArm = pJoint->GetLeverArmA(pWorld);
			}
			else
			{
				jointLeverArm = pJoint->GetLeverArmB(pWorld);
			}

			maxJointLeverArm = dmMax(maxJointLeverArm, jointLeverArm);
		}


		dm_float32 maxInertiaRadius = sqrtf(m_invMass * dmMax(dmGetX(m_I.rx), dmGetY(m_I.ry), dmGetZ(m_I.rz)));


		dm_float32 localInertiaScale = inertiaScale;
		if (maxJointLeverArm > maxInertiaRadius && maxInertiaRadius > 0.0f)
		{
			dm_float32 ratio = maxJointLeverArm / maxInertiaRadius;
			localInertiaScale = ratio * ratio * inertiaScale;
		}

		if (pParent && pParent->m_mass > 0.0f)
		{
			dm_float32 newMass = dmMin( m_mass, massRatio * pParent->m_mass );

			m_I *= localInertiaScale * newMass / m_mass;
			m_invI *= m_mass / (localInertiaScale * newMass);

			m_mass = newMass;
			m_invMass = 1.0f / newMass;
		}
		else
		{
			m_I *= localInertiaScale;
			m_invI *= 1.0f / localInertiaScale;
		}
	}

	dm_int32 jointKey = m_jointListKey;
	while (jointKey != -1)
	{
		dm_int32 jointIndex = jointKey >> 1;
		dm_int32 edgeIndex = jointKey & 1;


		dmJoint * pJoint = pWorld->m_joints.Get(jointIndex);
		jointKey = pJoint->m_edges[edgeIndex].m_nextKey;

		dm_int32 otherIndex = pJoint->m_edges[edgeIndex ^ 1].m_bodyIndex;
		dmBody * pOther = pWorld->m_bodies.Get(otherIndex);
		if (pOther == pParent || pOther->m_isStabilized)
		{
			continue;
		}

		pOther->StabilizeTreeRecursive(pWorld, inertiaScale, massRatio, this);
	}
}


















































































































bool dmBody::ShouldCollide(dmWorld * pWorld, const dmBody * pBodyA, const dmBody * pBodyB)
{
    // At least one body should be dynamic.
    if (pBodyA->m_bodyType != dm_dynamicBody && pBodyB->m_bodyType != dm_dynamicBody)
    {
        return false;
    }

    // Does a joint prevent collision?
    dm_int32 jointKey = pBodyA->m_jointListKey;
    while (jointKey != -1)
    {
        dm_int32 jointIndex = jointKey >> 1;
        dmJoint * pJoint = pWorld->m_joints.Get(jointIndex);

        dm_int32 edgeIndex = jointKey & 1;
        jointKey = pJoint->m_edges[edgeIndex].m_nextKey;

        dm_int32 indexB = pJoint->m_edges[edgeIndex ^ 1].m_bodyIndex;

        if (indexB == pBodyB->m_index && pJoint->m_collideConnected == false)
        {
            return false;
        }
    }

    return true;
}


dm_vec4 dmBody::GetClosestPoint(dmWorld * pWorld, dm_vec4Arg point) const
{
    dm_vec4 distance = dm_vec4_maxFloat;
    dm_vec4 c = m_sweep.m_center;
    for (dm_int32 i = m_fixtureList; i != -1; i = pWorld->m_fixtures[i].m_ownerNext)
    {
        dmFixture * f = pWorld->m_fixtures.Get(i);

        dmDistanceOutput output;
        f->GetClosestPoint(&output, m_transform, point);
        if (dmLT(output.distance, distance))
        {
            distance = output.distance;
            c = output.pointA;
        }
    }

    return c;
}








































bool dmBody::IsFast(dm_vec4Arg dt) const
{
	dm_vec4 minRadius = dmV4(m_minRadius);
	dm_vec4 maxRadius = dmV4(m_maxRadius);

    dm_vec4 origin = m_transform.p;
    dm_vec4 v = GetLinearVelocityFromWorldPoint(origin);
    dm_vec4 w = m_angularVelocity;

    // Bound the displacement of any point on the body.
    dm_vec4 velocityBound = dmLength3(v) + maxRadius * dmLength3(w);
    dm_vec4 displacementBound = dt * velocityBound;

	bool isFast = dmGT(displacementBound, dm_vec4_oneHalf * dm_vec4_oneHalf * minRadius);
    return isFast;
}


































// Every dmBody_* entry point validates its id the same way on one source line;
// the original used a macro, this name is invented.
#define dmGetBodyWorld(id, failValue) \
    if (id.worldIndex >= 65534) return failValue; \
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex]; \
    if (pWorld == NULL || !pWorld->m_bodies.IsValid(id.index) || pWorld->m_bodies[id.index].m_revision != id.revision) return failValue





dmFixtureId dmBody_CreateFixture(dmBodyId id, const dmFixtureDef * pDef)
{
    if (id.worldIndex >= 65534) return dm_nullFixtureId; if (id.index == -1) return dm_nullFixtureId;
    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld == NULL || !pWorld->m_bodies.IsValid(id.index) || pWorld->m_bodies[id.index].m_revision != id.revision) return dm_nullFixtureId;

    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return dm_nullFixtureId;
    }

    dmBody * pBody = pWorld->m_bodies.Get(id.index);
    dm_int32 index = pBody->CreateFixture(pWorld, pDef);
    if (index == -1)
    {
        dmWarn("Domino fixture capacity of %d exceeded. Contact ecatto@blizzard.com\n", pWorld->m_fixtures.GetCapacity());
        return dm_nullFixtureId;
    }

    dmFixtureId fixtureId;
    fixtureId.index = index;
    fixtureId.revision = pWorld->m_fixtures[index].m_revision;
    fixtureId.worldIndex = id.worldIndex;
    return fixtureId;
}


















// NON_MATCHING: 188/188 bytes, same instructions; block layout only: the original places the shared dm_nullFixtureId return right after the first test. Re-laid on DWARF lines (cols match); combined/separate guard forms tried.
dmFixtureId dmBody_GetFirstFixture(dmBodyId id)
{ // Inference: direct ID-validation expansion keeps pWorld on its DWARF line 1153.
    if (id.worldIndex >= 65534) return dm_nullFixtureId; if (id.index == -1) return dm_nullFixtureId; if (dmWorld::s_aWorlds[id.worldIndex] == NULL || !dmWorld::s_aWorlds[id.worldIndex]->m_bodies.IsValid(id.index) || dmWorld::s_aWorlds[id.worldIndex]->m_bodies[id.index].m_revision != id.revision) return dm_nullFixtureId;

    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    dmBody * pBody = &pWorld->m_bodies[id.index];
    dm_int32 index = pBody->m_fixtureList;
    if (index == -1)
    {
        return dm_nullFixtureId;
    }

    dmFixtureId fixtureId;
    fixtureId.index = index;
    fixtureId.worldIndex = id.worldIndex;
    fixtureId.revision = pWorld->m_fixtures[index].m_revision;
    return fixtureId;
}

// NON_MATCHING: 180/180 bytes, same instructions; block layout only (shared dm_nullFixtureId return placed after the first test in the original). Re-laid on DWARF lines; guard forms tried.
dmFixtureId dmBody_GetNextFixture(dmFixtureId id)
{ // Inference: direct ID-validation expansion keeps pWorld on its DWARF line 1173.
    if (id.worldIndex >= 65534) return dm_nullFixtureId; if (id.index == -1) return dm_nullFixtureId; if (dmWorld::s_aWorlds[id.worldIndex] == NULL || !dmWorld::s_aWorlds[id.worldIndex]->m_fixtures.IsValid(id.index) || dmWorld::s_aWorlds[id.worldIndex]->m_fixtures[id.index].m_revision != id.revision) return dm_nullFixtureId;

    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    dmFixture * pFixture = &pWorld->m_fixtures[id.index];

    if (pFixture->m_ownerType != dm_bodyOwner)
    {
        return dm_nullFixtureId;
    }


    if (pFixture->m_ownerNext == -1)
    {
        return dm_nullFixtureId;
    }

    dmFixture * pNext = &pWorld->m_fixtures[pFixture->m_ownerNext];

    dmFixtureId nextId;
    nextId.index = pFixture->m_ownerNext;
    nextId.worldIndex = id.worldIndex;
    nextId.revision = pNext->m_revision;
    return nextId;
}


dmBodyType dmBody_GetType(dmBodyId id)
{
    dmGetBodyWorld(id, dm_unknownBody);

    dmBody * pBody = &pWorld->m_bodies[id.index];
    return pBody->m_bodyType;
}


void dmBody_SetType(dmBodyId id, dmBodyType type)
{
    if (id.worldIndex >= 65534) return; dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex]; if (pWorld == NULL || !pWorld->m_bodies.IsValid(id.index) || pWorld->m_bodies[id.index].m_revision != id.revision || type < dm_dynamicBody || type > dm_staticBody) return;







    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }



    pWorld->m_bodies[id.index].SetType(pWorld, type);
}

void dmBody_SetTransform(dmBodyId id, const dmTransform & xf)
{ // Inference: expand original ID validation without an early local; pWorld is declared at DWARF1237.
    if (id.worldIndex >= 65534 || dmWorld::s_aWorlds[id.worldIndex] == NULL || !dmWorld::s_aWorlds[id.worldIndex]->m_bodies.IsValid(id.index) || dmWorld::s_aWorlds[id.worldIndex]->m_bodies[id.index].m_revision != id.revision) return;
    if (dmIsFinite4(xf.q) == false || dmIsZeroSafe4(xf.q) == false || dmIsFinite3(xf.p) == false)
    {
        dmLog("Bad arguments passed to dmBody_SetTransform: q=[%g, %g, %g, %g], p=[%g, %g, %g]\n",
              dmGetX(xf.q), dmGetY(xf.q), dmGetZ(xf.q), dmGetW(xf.q), dmGetX(xf.p), dmGetY(xf.p), dmGetZ(xf.p));
        return;
    }


    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }


    pWorld->m_bodies[id.index].SetTransform(pWorld, xf);
}


void dmBody_SetTransformNoSync(dmBodyId id, const dmTransform & xf)
{ // Inference: direct ID-validation expansion preserves the sole pWorld local at DWARF1259.
    if (id.worldIndex >= 65534 || dmWorld::s_aWorlds[id.worldIndex] == NULL || !dmWorld::s_aWorlds[id.worldIndex]->m_bodies.IsValid(id.index) || dmWorld::s_aWorlds[id.worldIndex]->m_bodies[id.index].m_revision != id.revision) return;
    if (dmIsFinite4(xf.q) == false || dmIsZeroSafe4(xf.q) == false || dmIsFinite3(xf.p) == false)
    {
        dmLog("Bad arguments passed to dmBody_SetTransformNoSync: q=[%g, %g, %g, %g], p=[%g, %g, %g]\n",
              dmGetX(xf.q), dmGetY(xf.q), dmGetZ(xf.q), dmGetW(xf.q), dmGetX(xf.p), dmGetY(xf.p), dmGetZ(xf.p));
        return;
    }


    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];

    pWorld->m_bodies[id.index].SetTransformNoSync(xf);
}


void dmBody_SetPosition(dmBodyId id, dm_vec4Arg p)
{ // Inference: direct ID-validation expansion preserves pWorld at DWARF1275.
    if (id.worldIndex >= 65534 || dmWorld::s_aWorlds[id.worldIndex] == NULL || !dmWorld::s_aWorlds[id.worldIndex]->m_bodies.IsValid(id.index) || dmWorld::s_aWorlds[id.worldIndex]->m_bodies[id.index].m_revision != id.revision) return;
    if (dmIsFinite3(p) == false)
    {
        dmLog("Bad argument passed to dmBody_SetPosition: p=[%g, %g, %g]\n", dmGetX(p), dmGetY(p), dmGetZ(p));
        return;
    }


    dmWorld * pWorld = dmWorld::s_aWorlds[id.worldIndex];
    if (pWorld->m_lock.LoadAcquire() != 0)
    {
        return;
    }


    dmTransform xf; // Inference: original load at1283 takes the current sweep quaternion.
    xf.q = pWorld->m_bodies[id.index].m_sweep.m_quaternion;
    xf.p = p;
    pWorld->m_bodies[id.index].SetTransform(pWorld, xf);
}


dm_vec4 dmBody_GetQuaternion(dmBodyId id)
{
    dmGetBodyWorld(id, dm_quat_identity);

    return pWorld->m_bodies[id.index].m_sweep.m_quaternion;
}


dm_vec4 dmBody_GetPosition(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].m_transform.p;
}


void dmBody_SetLinearVelocity(dmBodyId id, dm_vec4Arg v)
{
    dmGetBodyWorld(id, );

    if (dmIsFinite3(v) == false)
    {
        return;
    }

    dmBody * pBody = &pWorld->m_bodies[id.index];


    if (pBody->m_bodyType == dm_staticBody)
    {
        return;
    }

    pBody->m_linearVelocity = v;
}


dm_vec4 dmBody_GetLinearVelocity(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].m_linearVelocity;
}


void dmBody_SetAngularVelocity(dmBodyId id, dm_vec4Arg omega)
{
    dmGetBodyWorld(id, );

    if (dmIsFinite3(omega) == false)
    {
        return;
    }

    dmBody * pBody = &pWorld->m_bodies[id.index];


    if (pBody->m_bodyType == dm_staticBody)
    {
        return;
    }

    pBody->m_angularVelocity = omega;
}


dm_vec4 dmBody_GetAngularVelocity(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].m_angularVelocity;
}






















dm_vec4 dmBody_GetWorldPointLinearVelocity(dmBodyId id, dm_vec4Arg point)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].GetLinearVelocityFromWorldPoint(point);
}


void dmBody_ApplyImpulse(dmBodyId id, dm_vec4Arg impulse, dm_vec4Arg point, dmWake state)
{
    dmGetBodyWorld(id, );
    if (dmIsFinite3(impulse) == false || dmIsFinite3(point) == false) return;






    pWorld->m_bodies[id.index].ApplyImpulse(impulse, point, state == dm_wakeIfChanged);
}


void dmBody_ApplyForce(dmBodyId id, dm_vec4Arg force, dm_vec4Arg point, dmWake state)
{
    dmGetBodyWorld(id, );
    if (dmIsFinite3(force) == false || dmIsFinite3(point) == false) return;






    pWorld->m_bodies[id.index].ApplyForce(force, point, state == dm_wakeIfChanged);
}


void dmBody_ApplyTorque(dmBodyId id, dm_vec4Arg torque, dmWake state)
{
    dmGetBodyWorld(id, );
    if (dmIsFinite3(torque) == false) return;






    pWorld->m_bodies[id.index].ApplyTorque(torque, state == dm_wakeIfChanged);
}


dm_float32 dmBody_GetMass(dmBodyId id)
{
    dmGetBodyWorld(id, 0.0f);

    return pWorld->m_bodies[id.index].m_mass;
}










dm_vec4 dmBody_GetCenter(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].m_sweep.m_center;
}


dm_vec4 dmBody_GetLocalCenter(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].m_sweep.m_localCenter;
}































































































bool dmBody_IsSleeping(dmBodyId id)
{
    dmGetBodyWorld(id, false);

    return pWorld->m_bodies[id.index].m_isSleeping;
}


dm_vec4 dmBody_ComputeSurfaceArea(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].ComputeSurfaceArea(pWorld);
}


dm_vec4 dmBody_ComputeProjectedSurfaceArea(dmBodyId id, dm_vec4Arg dir)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].ComputeProjectedSurfaceArea(pWorld, dir);
}


dm_vec4 dmBody_ComputeVolume(dmBodyId id)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].ComputeVolume(pWorld);
}


dm_vec4 dmBody_ComputeSubmergedVolume(dmBodyId id, dm_vec4 * pCentroid, dm_vec4Arg normal, dm_vec4Arg offset)
{
    *pCentroid = dmVecSet(0.0f);
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].ComputeSubmergedVolume(pWorld, pCentroid, normal, offset);
}

// Inference: || operand order from line-table columns 1602:40/56 and 1608:37; lines 1604/1610 have no rows (blank).
void dmBody_StabilizeTree(dmBodyId id, dm_float32 inertiaScale, dm_float32 massRatio)
{
    dmGetBodyWorld(id, );
	if (dmIsFinite(inertiaScale) == false || inertiaScale < 1000.0f * FLT_MIN)
	{

		return;
	}

	if (dmIsFinite(massRatio) == false || massRatio < 1000.0f * FLT_MIN)
	{

		return;
	}


    if (dmIsFinite(massRatio) == false) return;




    pWorld->m_bodies[id.index].StabilizeTree(pWorld, inertiaScale, massRatio);
}






































void * dmBody_GetUserData(dmBodyId id)
{
    dmGetBodyWorld(id, NULL);

    return pWorld->m_bodies[id.index].m_pUserData;
}










void dmBody_SetSleeping(dmBodyId id, bool flag)
{
    dmGetBodyWorld(id, );

    pWorld->m_bodies[id.index].SetSleeping(flag);
}


dm_vec4 dmBody_GetClosestPoint(dmBodyId id, dm_vec4Arg point)
{
    dmGetBodyWorld(id, dmVecSet(0.0f));

    return pWorld->m_bodies[id.index].GetClosestPoint(pWorld, point);
}
