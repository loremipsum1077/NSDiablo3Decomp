#include "dmConvexContact.h"
#include "dmContact.h"
#include "../../Include/dmPolytope.h"
#include "../dmWorld.h"
#include "../dmFixture.h"
#include "../../Common/Platform/dmThreading_Linux.h"
#include "../../Collision/Functions/dmTimeOfImpact.h"

void dmConvexContact::Create(dmShape::Type typeA, dmShape::Type typeB)
{
    if (typeA == dmShape::e_polytope && typeB == dmShape::e_polytope) m_satCache.Clear();
    else m_gjkCache.Clear();
}

void dmConvexContact::TimeOfImpact(dmTOIOutput * pOutput, dmWorld * pWorld,
    const dmContact * pBase, const dmSweep & sweepA, const dmSweep & sweepB)
{
    const dmFixture * pFixtureA = pWorld->m_fixtures.Get(pBase->m_edges[0].m_fixtureIndex);
    const dmFixture * pFixtureB = pWorld->m_fixtures.Get(pBase->m_edges[1].m_fixtureIndex);
    dmTOIInput input;
    input.cache.Clear();
    input.proxyA.Set(pFixtureA->m_pShape);
    input.proxyB.Set(pFixtureB->m_pShape);
    input.sweepA = sweepA;
    input.sweepB = sweepB;
    dmTimeOfImpact(pOutput, &input);
}

inline void dmConvexContact::ClearBaseManifold(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex)
{
    if (pBase->m_aManifolds) {
        if (pMutex) pMutex->Lock();
        pWorld->m_poolAllocator.Free(pBase->m_aManifolds, sizeof(dmManifold), false);
        if (pMutex) pMutex->Unlock();
        pBase->m_aManifolds = NULL;
    }
    pBase->m_manifoldCount = 0;
}

bool dmConvexContact::UpdateManifold(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex, bool isTOIEvent)
{
    const dmFixture * pFixtureA = pWorld->m_fixtures.Get(pBase->m_edges[0].m_fixtureIndex);
    const dmFixture * pFixtureB = pWorld->m_fixtures.Get(pBase->m_edges[1].m_fixtureIndex);
    dm_int32 count0 = pBase->m_manifoldCount;
    dmManifold m0;
    dmManifold_Create(&m0);
    if (count0 == 1) {
        m0 = pBase->m_aManifolds[0];
        dmManifold_ClearNewFlags(&m0);
    }
    dmManifold m1;
    dmManifold_Create(&m1);
    if (!dmTestOverlap(pFixtureA->m_aabb, pFixtureB->m_aabb)) {
        ClearBaseManifold(pWorld, pBase, pMutex);
        return false;
    }
    dmShape::Type typeA = pFixtureA->m_pShape->m_type;
    dmShape::Type typeB = pFixtureB->m_pShape->m_type;
    const dmTransform xfA = pFixtureA->GetTransform(pWorld);
    const dmTransform xfB = pFixtureB->GetTransform(pWorld);
    if (ComputeManifold(m0, &m1, &m_gjkCache, &m_satCache,
        pFixtureA->m_pShape, pFixtureB->m_pShape, xfA, xfB, typeA, typeB, isTOIEvent)) {
        if (pBase->m_aManifolds == NULL) {
            if (pMutex) pMutex->Lock();
            pBase->m_aManifolds = static_cast<dmManifold *>(pWorld->m_poolAllocator.Allocate(sizeof(dmManifold), false));
            if (pMutex) pMutex->Unlock();
        }
        pBase->m_aManifolds[0] = m1;
        pBase->m_manifoldCount = 1;
        return true;
    } else {
        ClearBaseManifold(pWorld, pBase, pMutex);
        return false;
    }
}




bool dmConvexContact::ComputeManifold(const dmManifold & originalManifold, dmManifold * pManifold,
    dmGJKCache * pGJKCache, dmSATCache * pSATCache, dmShape * pShapeA, dmShape * pShapeB,
    const dmTransform & xfA, const dmTransform & xfB, dmShape::Type typeA, dmShape::Type typeB, bool isTOIEvent)
{
    pManifold->m_pointCount = 0;
    pManifold->m_normal = dmVecSet(0.0f);
    if (typeA == dmShape::e_sphere && typeB == dmShape::e_sphere) {
        const dmSphere * pSphereA = static_cast<const dmSphere *>(pShapeA);
        const dmSphere * pSphereB = static_cast<const dmSphere *>(pShapeB);
        dmSphere sphereA, sphereB;
        sphereA.Create(dmMul(xfA, pSphereA->m_p), pSphereA->m_radius);
        sphereB.Create(dmMul(xfB, pSphereB->m_p), pSphereB->m_radius);
        dmSSManifold(pManifold, &sphereA, &sphereB);
    } else if (typeA == dmShape::e_capsule) {
        const dmCapsule * pCapsuleA = static_cast<const dmCapsule *>(pShapeA);
        dmCapsule capsuleA;
        capsuleA.Create(dmMul(xfA, pCapsuleA->m_p1), dmMul(xfA, pCapsuleA->m_p2), pCapsuleA->m_radius);
        if (typeB == dmShape::e_sphere) {
            const dmSphere * pSphereB = static_cast<const dmSphere *>(pShapeB);
            dmSphere sphereB;
            sphereB.Create(dmMul(xfB, pSphereB->m_p), pSphereB->m_radius);
            dmCSManifold(pManifold, &capsuleA, &sphereB);
        } else if (typeB == dmShape::e_capsule) {
            const dmCapsule * pCapsuleB = static_cast<const dmCapsule *>(pShapeB);
            dmCapsule capsuleB;
            capsuleB.Create(dmMul(xfB, pCapsuleB->m_p1), dmMul(xfB, pCapsuleB->m_p2), pCapsuleB->m_radius);
            dmCCManifold(pManifold, &capsuleA, &capsuleB);
        }
    } else if (typeA == dmShape::e_polytope) {
        const dmPolytope * pPolytopeA = static_cast<const dmPolytope *>(pShapeA);
        if (typeB == dmShape::e_sphere) {
            const dmSphere * pSphereB = static_cast<const dmSphere *>(pShapeB);
            dmSphere sphereB;
            sphereB.Create(dmMulT(xfA, dmMul(xfB, pSphereB->m_p)), pSphereB->m_radius);
            dmPSManifold(pManifold, pGJKCache, pPolytopeA, &sphereB);
        } else if (typeB == dmShape::e_capsule) {
            const dmCapsule * pCapsuleB = static_cast<const dmCapsule *>(pShapeB);
            dmCapsule capsuleB;
            capsuleB.Create(dmMulT(xfA, dmMul(xfB, pCapsuleB->m_p1)), dmMulT(xfA, dmMul(xfB, pCapsuleB->m_p2)), pCapsuleB->m_radius);
            dmPCManifold(pManifold, pGJKCache, pPolytopeA, &capsuleB);
        } else if (typeB == dmShape::e_polytope) {
            const dmPolytope * pPolytopeB = static_cast<const dmPolytope *>(pShapeB);
            if (isTOIEvent) pSATCache->Clear();
            dmTransform xf = dmMulT(xfA, xfB);
            dmPPManifold(pManifold, pSATCache, pPolytopeA, pPolytopeB, xf);
        }
        for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i)
            pManifold->m_points[i].point = dmMul(xfA, pManifold->m_points[i].point);
        pManifold->m_normal = dmRotate(xfA.q, pManifold->m_normal);
    }
    bool persisted[4] = {false, false, false, false};
    if (pManifold->m_pointCount < 1) return false;
    pManifold->m_tangentImpulse = originalManifold.m_tangentImpulse;
    pManifold->m_rollingImpulse = originalManifold.m_rollingImpulse;
    pManifold->m_twistImpulse = originalManifold.m_twistImpulse;
    for (dm_int32 i = 0; i < pManifold->m_pointCount; ++i) {
        dmManifoldPoint * mp = pManifold->m_points + i;
        dm_uint64 key = mp->id.key;
        bool found = false;
        for (dm_int32 j = 0; j < originalManifold.m_pointCount; ++j) {
            const dmManifoldPoint * mp0 = originalManifold.m_points + j;
            if (!persisted[j] && mp0->id.key == key) {
                persisted[j] = true;
                mp->normalImpulse = mp0->normalImpulse;
                mp->isNew = false;
                found = true;
                break;
            }
        }
        if (!found) {
            mp->isNew = true;
            mp->normalImpulse = 0.0f;
        }
    }
    return true;
}
