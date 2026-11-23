// X/Client/Physics/Physics.cpp
// Functions sit on their original line numbers (2.6.2 DWARF, CU 0x1c08d42).
#include <float.h>
#include "../CGame.h"
#include "Physics.h"
#include "../Wind.h"
#include "../Definitions/PhysicsDefinition.h"
#include "../RActors/RActors.h"
#include "../../Common/Game.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XSphereArray.h"
#include "../../Sigma/DataStructures/XSphereNode.h"
#include "../../Sigma/Math/Cylinder.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"




enum {
    MAX_POINTMASSES = 2048,
};

const float ITERATION_METRIC_THRESHOLD = 0.1f;
const float SLEEP_METRIC_THRESHOLD = 0.0001f;

const float EPSILON_DISTANCE = 0.0005f;


// X/Client/Physics/Physics.h:67 (declared here: the src Physics.h is a subset without Cylinder.h).
struct PhysicsGlobals {  // size 0x398
    PointMassListPool * ptPointMassListPool;  // 0x00
    FixedBlockAllocator * ptAllocPointMass;  // 0x08
    Capsule aCapsules[PHYSICSSYSTEM_MAX_COLLISION_CAPSULES];  // 0x10
    int32 nCapsuleCount;  // 0x390
};
static_assert(sizeof(PhysicsGlobals) == 0x398, "PhysicsGlobals");

extern const float cgflGravity;  // X/Common/Definitions/WorldDefinition.cpp:44
XSphereArray * RActorGetCollisionSphereTree();  // X/Client/RActors/RActorCollision.cpp:1297
// Declared in X/Client/Physics/Physics.h in the original.
void PhysicsSystemSetPoint(PhysicsSystem & tPhysics, SNO snoPhysics, RActorID idLocalRActor);
void PhysicsSystemClear(PhysicsSystem & tPhysics);
void PhysicsSystemUpdate(PhysicsSystem & tPhysics, float flDeltaTime);
// File statics defined below PhysicsSystemUpdate (lines 304-626).
static void sComputeForcesAndIntegrate(PhysicsSystem & tPhysics, float flDeltaT);
static void sApplyConstraints(PhysicsSystem & tPhysics);
static void sApplyMaximumSpeed(const PhysicsSystem & tPhysics, const PhysicsDefinition * pdefPhysics);
static float sApplyLengthConstraint(PhysicsSystem & tPhysics);





void PhysicsInitialize(
    void)
{
    PhysicsGlobals *& ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;

	ptPhysicsGlobals = (PhysicsGlobals *)SigmaMemoryNew(sizeof(PhysicsGlobals), 4, AppGetCurrentMemoryInterface(), FALSE);
    ptPhysicsGlobals->ptAllocPointMass = (FixedBlockAllocator *)SigmaMemoryNew(sizeof(FixedBlockAllocator), 4, AppGetCurrentMemoryInterface(), TRUE);
    ptPhysicsGlobals->ptAllocPointMass->InitializeDynamic(sizeof(PointMass), MAX_POINTMASSES, FALSE, AppGetCurrentMemoryInterface());
    ptPhysicsGlobals->ptPointMassListPool = (PointMassListPool *)SigmaMemoryNew(sizeof(PointMassListPool), 4, AppGetCurrentMemoryInterface(), TRUE);
    ptPhysicsGlobals->ptPointMassListPool->Initialize();
    ptPhysicsGlobals->ptPointMassListPool->SetBlockCount_Tracked(MAX_POINTMASSES);
    ptPhysicsGlobals->nCapsuleCount = 0;
}




void PhysicsDispose(
    void)
{
    PhysicsGlobals *& ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;

    ptPhysicsGlobals->ptAllocPointMass->Dispose();
    SigmaMemoryFree(ptPhysicsGlobals->ptAllocPointMass, NULL); ptPhysicsGlobals->ptAllocPointMass = NULL;

    ptPhysicsGlobals->ptPointMassListPool->Dispose();
    SigmaMemoryFree(ptPhysicsGlobals->ptPointMassListPool, NULL); ptPhysicsGlobals->ptPointMassListPool = NULL;

	SigmaMemoryFree(ptPhysicsGlobals, AppGetCurrentMemoryInterface()); ptPhysicsGlobals = NULL;
}




PointMass * PointMassCreate(
    PhysicsSystem & tPhysics)
{
	PhysicsGlobals * ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;
	PointMass * ptPointMass = (PointMass *)ptPhysicsGlobals->ptAllocPointMass->Alloc_Tracked();
	if (ptPointMass)
	{
		SigmaMemoryClear(ptPointMass, sizeof(PointMass));

		tPhysics.listContacts.AddTail_Tracked(ptPointMass);
	}

    return ptPointMass;
}




void PointMassDelete(
    PhysicsSystem & tPhysics,
    PointMass * ptPointMass)
{
	tPhysics.listContacts.RemoveAt(
		tPhysics.listContacts.Find(ptPointMass));

	PhysicsGlobals * ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;
	ptPhysicsGlobals->ptAllocPointMass->Free(ptPointMass);
}




void PointMassDeleteAll(
    PhysicsSystem & tPhysics)
{
	PhysicsGlobals * ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;
	while (!tPhysics.listContacts.IsEmpty())
	{
		PointMass * ptPointMass = tPhysics.listContacts.GetHead();
		tPhysics.listContacts.RemoveHead();
		ptPhysicsGlobals->ptAllocPointMass->Free(ptPointMass);
	}
}














void PhysicsSystemInitialize(
    PhysicsSystem & tPhysics,
    SNO snoPhysics,
    RActorID idLocalRActor)
{
    PhysicsGlobals * ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;
    tPhysics.listContacts.SetPool(ptPhysicsGlobals->ptPointMassListPool);
    PhysicsSystemSetPoint(tPhysics, snoPhysics, idLocalRActor);
}




void PhysicsSystemDispose(
    PhysicsSystem & tPhysics)
{
	PhysicsSystemClear(tPhysics);
}




void PhysicsSystemSetPoint(
    PhysicsSystem & tPhysics,
    SNO snoPhysics,
    RActorID idLocalRActor)
{
	// Inference: store order from the original's stores (flags/sno, constraints, count/id);
	// the line rows (185, 182, 180) do not settle the source lines.
	tPhysics.dwFlags = (1 << PHYSICSSYSTEM_INITIALIZED);
	tPhysics.snoPhysics = snoPhysics;
	tPhysics.ptConstraints = NULL;
	tPhysics.nConstraintCount = 0;
	tPhysics.idLocalRActor = idLocalRActor;
}




void PhysicsSystemClear(
    PhysicsSystem & tPhysics)
{
	if (!(tPhysics.dwFlags & (1 << PHYSICSSYSTEM_INITIALIZED)))
	{
		return;
	}

	tPhysics.dwFlags &= ~(1 << PHYSICSSYSTEM_INITIALIZED);

	PointMassDeleteAll(tPhysics);
	tPhysics.listContacts.RemoveAll();
	SigmaMemoryFree(tPhysics.ptConstraints, NULL); tPhysics.ptConstraints = NULL;
}




void PhysicsSystemAwaken(
    PhysicsSystem & tPhysics,
    const int32 nInitialRelaxationSteps)
{
	if ((tPhysics.dwFlags & (1 << PHYSICSSYSTEM_AWAKE_BIT)) ||
		tPhysics.snoPhysics == SNO_NONE)
	{
		return;
	}

	tPhysics.dwFlags |= (1 << PHYSICSSYSTEM_AWAKE_BIT);

	// relax the system: one 60 Hz step at a time (literal: no named constant in the CU)
	for (int32 i = 0; i < nInitialRelaxationSteps; i++)
	{
		PhysicsSystemUpdate(tPhysics, 1.0f / 60.0f);
	}

}


// NON_MATCHING: 3268 vs 3328 bytes; only inlined sApplyMaximumSpeed differs (the rest is identical):
// our unused wdSpeed = Magnitude() loses its sqrtf call (LibCallsShrinkWrap), the original keeps it,
// so the original used wdSpeed in code that was removed later; not reconstructed (no evidence).
void PhysicsSystemUpdate(
    PhysicsSystem & tPhysics,
    float flDeltaTime)
{


	if (!(tPhysics.dwFlags & (1 << PHYSICSSYSTEM_AWAKE_BIT)))
	{
		return;
	}

	if (tPhysics.listContacts.IsEmpty())
	{
		return;
	}

	const PhysicsDefinition * pdefPhysics = (const PhysicsDefinition *)gptGroupDefPhysics->LockRFast(tPhysics.snoPhysics);

	// remember where every point mass started
	POS pos = tPhysics.listContacts.GetHeadPosition();
    while (pos)
	{
        PointMass & pm = *tPhysics.listContacts.GetNext(pos);
		pm.wp0 = pm.wp;
	}

    float flInvDeltaT = 1.0f / flDeltaTime;

	sComputeForcesAndIntegrate(tPhysics, flDeltaTime);
	sApplyConstraints(tPhysics);

	// derive the velocities from the corrected positions
	pos = tPhysics.listContacts.GetHeadPosition();
    while (pos)
	{
        PointMass & pm = *tPhysics.listContacts.GetNext(pos);
		pm.wv = flInvDeltaT * (pm.wp - pm.wp0);
	}

	if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_HAS_MAXIMUM_SPEED_BIT))
	{
		sApplyMaximumSpeed(tPhysics, pdefPhysics);
	}

	if (!(pdefPhysics->dwFlags & (1 << PHYSICSDEF_DO_NOT_AUTOSLEEP_BIT)))
	{
		// go to sleep when nothing is moving
		WorldDistance wdKineticEnergyMetric = 0.0f;
		// Inference: the 0-row load of the list head at +0xc7c is this reset.
		pos = tPhysics.listContacts.GetHeadPosition();
		while (pos)
		{
			PointMass & pm = *tPhysics.listContacts.GetNext(pos);
			wdKineticEnergyMetric += pm.wv.MagnitudeSquared();
		}


		if (wdKineticEnergyMetric <= SLEEP_METRIC_THRESHOLD)
		{
			tPhysics.dwFlags &= ~(1 << PHYSICSSYSTEM_AWAKE_BIT);
		}
	}




}




static void sComputeForcesAndIntegrate(
    PhysicsSystem & tPhysics,
    float flDeltaT)
{
	const PhysicsDefinition * pdefPhysics = (const PhysicsDefinition *)gptGroupDefPhysics->LockRFast(tPhysics.snoPhysics);

	Vector3D wvBaseAcc = cgwvNull;
    if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_AFFECTED_BY_GRAVITY_BIT))
	{
        wvBaseAcc.z += cgflGravity + pdefPhysics->flAirBuoyancy * 60.0f * 60.0f;
	}

	CGameGlobals & tCGameGlobals = CGameGlobalsGet();
	Vector3D wvGlobalWind = Wind::GetGlobalWindState().m_wvWind;

	// integrate every point mass
	POS pos = tPhysics.listContacts.GetHeadPosition();
	while (pos)
	{
		PointMass * pm = tPhysics.listContacts.GetNext(pos);

		if (pm->flInvMass <= EPSILON)
		{
			continue;
		}

		Vector3D wvAcc = wvBaseAcc;

        if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_AFFECTED_BY_RANDOM_ACCEL))
		{
			Vector3D wvRandomAcceleration;


                wvRandomAcceleration.x = RandRangeFloat_Internal(tCGameGlobals.rngLocal, pdefPhysics->flRandomAccelMin, pdefPhysics->flRandomAccelDelta);
                wvRandomAcceleration.y = RandRangeFloat_Internal(tCGameGlobals.rngLocal, pdefPhysics->flRandomAccelMin, pdefPhysics->flRandomAccelDelta);
                wvRandomAcceleration.z = RandRangeFloat_Internal(tCGameGlobals.rngLocal, pdefPhysics->flRandomAccelMin, pdefPhysics->flRandomAccelDelta);

			wvAcc += wvRandomAcceleration * 60.0f * 60.0f;
		}

        if (pdefPhysics->flDragCoeff > 0.0f)
		{
			Vector3D wvAirVelocity = pm->wv;

            if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_AFFECTED_BY_WIND_BIT))
			{
				wvAirVelocity -= wvGlobalWind;
			}

			WorldDistance wdMagnitude = wvAirVelocity.Normalize();
            if (wdMagnitude > EPSILON_DISTANCE)
			{
                WorldDistance wdDragMagnitude = wdMagnitude * wdMagnitude * (pdefPhysics->flDragCoeff * (1.0f / 60.0f));
				wdDragMagnitude = Math::Min(wdDragMagnitude, wdMagnitude);
				wvAcc -= wvAirVelocity * (wdDragMagnitude / flDeltaT);
			}
		}

        if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_HAS_MAXIMUM_ACCELERATION_BIT))
		{
			WorldDistance wdAcceleration = wvAcc.Magnitude();

            if (wdAcceleration > pdefPhysics->flAccelerationMax * 60.0f * 60.0f)
			{
				wvAcc.Normalize();
				wvAcc *= pdefPhysics->flAccelerationMax * 60.0f * 60.0f;
			}
		}




		pm->wv += flDeltaT * wvAcc;
		pm->wp += flDeltaT * pm->wv;




		pm->wp.x = Math::Pin(pm->wp.x, -FLT_MAX, FLT_MAX);
		pm->wp.y = Math::Pin(pm->wp.y, -FLT_MAX, FLT_MAX);
		pm->wp.z = Math::Pin(pm->wp.z, -FLT_MAX, FLT_MAX);
	}
}








static void sComputeBoundingSphere(
    Sphere * ws,
    const PhysicsSystem & tPhysics)
{
	// bounding box of the point masses

	Vector3D wpMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3D wpMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	POS pos = tPhysics.listContacts.GetHeadPosition();
    while (pos)
	{
        PointMass * ptPointMass = tPhysics.listContacts.GetNext(pos);

		wpMin = Vector3D::Min(wpMin, ptPointMass->wp);
		wpMax = Vector3D::Max(wpMax, ptPointMass->wp);
	}

	// sphere around its center
	ws->wpCenter = 0.5f * (wpMin + wpMax);


	float flRadiusSquared = 0.0f;

	pos = tPhysics.listContacts.GetHeadPosition();
    while (pos)
	{
        PointMass * ptPointMass = tPhysics.listContacts.GetNext(pos);

		float flDistanceSquared = Vector3D::DistanceSquared(ptPointMass->wp, ws->wpCenter);
		flRadiusSquared = Math::Max(flRadiusSquared, flDistanceSquared);
	}

    ws->wdRadius = Math::Sqrt(flRadiusSquared);
}




static void sFindCollisionSpheres(
    const PhysicsSystem & tPhysics,
    RActorID idLocalRActor)
{
	Sphere wsBoundingSphere;
	sComputeBoundingSphere(&wsBoundingSphere, tPhysics);

	PhysicsGlobals * ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;
    ptPhysicsGlobals->nCapsuleCount = 0;

	XSphereArray * ptSphereTree = RActorGetCollisionSphereTree();

	XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());
	ptSphereTree->SphereCull(wsBoundingSphere, listSpheres);

	int32 nCapsuleCount = 0;
	POS pos = listSpheres.GetHeadPosition();
    while (pos && nCapsuleCount < PHYSICSSYSTEM_MAX_COLLISION_CAPSULES)
	{
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
		CollisionCapsuleInstance * ptCCInstance = (CollisionCapsuleInstance *)ptSphereNode->GetUserData();

        if (idLocalRActor != RACTORID_INVALID && ptCCInstance->idRActor != idLocalRActor)
		{
			continue;
		}


        ptPhysicsGlobals->aCapsules[nCapsuleCount] = ptCCInstance->tCapsule;
        ++nCapsuleCount;
	}

    ptPhysicsGlobals->nCapsuleCount = nCapsuleCount;
}



static float sApplyCollisionCapsuleConstraint(
    PointMass & pm,
    const PhysicsGlobals * ptPhysicsGlobals)
{
	float flChangeMetric = 0.0f;

    int32 nCapsuleCount = ptPhysicsGlobals->nCapsuleCount;
    for (int32 i = 0; i < nCapsuleCount; i++)
	{
		Vector3D wpBefore = pm.wp;
		pm.wp = ptPhysicsGlobals->aCapsules[i].PushToSurface(pm.wp);
		flChangeMetric += Vector3D::DistanceSquared(pm.wp, wpBefore);
	}

	return flChangeMetric;
}




static void sApplyConstraints(
    PhysicsSystem & tPhysics)
{
	const PhysicsDefinition * pdefPhysics = (const PhysicsDefinition *)gptGroupDefPhysics->LockRFast(tPhysics.snoPhysics);
	PhysicsGlobals * ptPhysicsGlobals = CGameGet().ptPhysicsGlobals;


    if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_CONSTRAINED_TO_COLLISION_SPHERES_BIT))
	{
        sFindCollisionSpheres(tPhysics, tPhysics.idLocalRActor);
	}

	// relax the constraints
	int32 nIterationsLeft = pdefPhysics->nMaxIterations;
	float flChangeMetric;
	do {
		nIterationsLeft--;

		flChangeMetric = 0.0f;

		flChangeMetric += sApplyLengthConstraint(tPhysics);


		POS pos = tPhysics.listContacts.GetHeadPosition();
		while (pos)
		{
			PointMass & pm = *tPhysics.listContacts.GetNext(pos);

			if (pdefPhysics->dwFlags & (1 << PHYSICSDEF_CONSTRAINED_TO_COLLISION_SPHERES_BIT))
			{
				flChangeMetric += sApplyCollisionCapsuleConstraint(pm, ptPhysicsGlobals);
			}

			// keep the point masses inside the world (literals: no named constant in the CU)
			pm.wp.x = Math::Pin(pm.wp.x, -100000.0f, 100000.0f);
			pm.wp.y = Math::Pin(pm.wp.y, -100000.0f, 100000.0f);
			pm.wp.z = Math::Pin(pm.wp.z, -100000.0f, 100000.0f);
		}

	} while (nIterationsLeft > 0 && Math::Abs(flChangeMetric) > ITERATION_METRIC_THRESHOLD);
}








static void sApplyMaximumSpeed(
    const PhysicsSystem & tPhysics,
    const PhysicsDefinition * pdefPhysics)
{
	POS pos = tPhysics.listContacts.GetHeadPosition();
	while (pos)
	{
		PointMass * ptPointMass = tPhysics.listContacts.GetNext(pos);

		Vector3D wvVelocity = ptPointMass->wv;

		WorldDistance wdSpeed = wvVelocity.Magnitude();
		// Inference: nothing else of this loop survives in the original (no load of
		// pdefPhysics, no store), so no clamp against flSpeedMax is written here.
		// The original keeps the sqrtf call, so wdSpeed had a use there; see NON_MATCHING above
		// PhysicsSystemUpdate.
	}
}






static float sApplyLengthConstraint(
    PhysicsSystem & tPhysics)
{
	float flChangeMetric = 0.0f;
	for (int32 i = 0; i < tPhysics.nConstraintCount; i++)
	{
		const DistanceConstraint & tConstraint = tPhysics.ptConstraints[i];
		PointMass & pm0 = *tConstraint.ptPointMassA;
		PointMass & pm1 = *tConstraint.ptPointMassB;

		if (pm0.flInvMass < EPSILON &&
			pm1.flInvMass < EPSILON)
		{
			continue;
		}


		Vector3D wvDelta = pm1.wp - pm0.wp;

		WorldDistance wdDiff = 0.0f;
		WorldDistance wdDeltaLength = wvDelta.Magnitude();
		if (wdDeltaLength < EPSILON)
		{
			// the two point masses coincide: push them apart along pm0's velocity




			wdDiff = tConstraint.wdRestLength * 0.5f;
			if (tConstraint.wdRestLength > EPSILON)
			{
				Vector3D wvVel = pm0.wv;
				WorldDistance wdVelMag = wvVel.Normalize();
				if (wdVelMag < EPSILON)
				{
					// Inference: x and y are zeroed in the original; written as three stores.
					wvVel.x = 0.0f; wvVel.y = 0.0f; wvVel.z = 0.001f;
				}

				wvVel *= wdDiff;
				pm0.wp += wvVel;
				pm1.wp -= wvVel;
			}
		}
		else
		{
			float divisor = wdDeltaLength * (pm0.flInvMass + pm1.flInvMass);
			if (divisor > EPSILON)
			{
				wdDiff = (wdDeltaLength - tConstraint.wdRestLength) / divisor;
				wvDelta *= wdDiff;
				pm0.wp += wvDelta * pm0.flInvMass;
				pm1.wp -= wvDelta * pm1.flInvMass;
			}
		}




		flChangeMetric += wdDiff;
	}

	return flChangeMetric;
}
