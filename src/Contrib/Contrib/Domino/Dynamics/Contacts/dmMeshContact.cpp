#include "Contrib/Contrib/Domino/Dynamics/Contacts/dmMeshContact.h"
#include "Contrib/Contrib/Domino/Dynamics/dmWorld.h"
#include "Contrib/Contrib/Domino/Include/dmShapes.h"
#include "Contrib/Contrib/Domino/Include/dmMesh.h"
#include "Contrib/Contrib/Domino/Include/dmBuffer.h"
#include "Contrib/Contrib/Domino/Include/dmSettings.h"
#include "Contrib/Contrib/Domino/Include/dmHeightField.h"
#include "Contrib/Contrib/Domino/Include/dmHeightData.h"
#include "Contrib/Contrib/Domino/Include/dmPolytope.h"
#include "Contrib/Contrib/Domino/Dynamics/Contacts/dmContact.h"
#include "Contrib/Contrib/Domino/Dynamics/dmFixture.h"
// dmTransformAABB is defined in Collision/dmCollision.cpp:3; its header declaration is not adopted.
dmAABB dmTransformAABB(const dmTransform & transform, const dmAABB & localAABB);
#include "Contrib/Contrib/Domino/Common/Platform/dmThreading_Linux.h"
// dmTCManifold is defined in Collision/Functions/dmTCManifold.cpp; its header declaration is not in the adopted dmManifold.h.
void dmTPManifold(dmManifold *, dmSATCache *, const dmTriangle *, const dmPolytope *, const dmTransform &);
void dmTCManifold(dmManifold *, dmGJKCache *, const dmTriangle *, const dmCapsule *);
#line 19 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Dynamics/Contacts/dmMeshContact.cpp"
// dm_maxManifoldPoints: name from the assertion text at 1095, value 4 from the compare against 5; no macro DIE survives.
#define dm_maxManifoldPoints 4

// dm_maxTreeManifolds: name from the assertion texts at 516, 551, 627 and 635; value 3 from the compares against 4 and 3 and the three-element local arrays (517, 530, 646).
#define dm_maxTreeManifolds 3
#include <string.h>
// dmManifold_ReducePoints is defined in Collision/Functions/dmManifold.cpp; its header declaration is not adopted.
void dmManifold_ReducePoints(dmManifold *, const dmManifoldPoint *, dm_int32);
#include "Contrib/Contrib/Domino/Collision/Functions/dmTimeOfImpact.h"






























































































































void dmMeshContact::Create()
{
    m_fatAABB.m_lowerBound = dm_vec4{0,0,0,0};
    m_fatAABB.m_upperBound = dm_vec4{0,0,0,0};
    m_triangleCount = 0;
    m_triangleCapacity = 0;
    m_aCache = 0;
}



void dmMeshContact::Destroy(dmWorld * pWorld)
{
    pWorld->m_poolAllocator.Free(m_aCache, m_triangleCapacity * sizeof(dmTriangleCache), true);
    m_aCache = 0;
}





















































void dmMeshContact::TimeOfImpact(dmTOIOutput * pOutput, dmWorld * pWorld, const dmContact * pBase, const dmSweep & sweepA, const dmSweep & sweepB, dmMutex * pMutex)
{
    const dmFixture * pFixtureA = pWorld->m_fixtures.Get(pBase->m_edges[0].m_fixtureIndex);
    const dmFixture * pFixtureB = pWorld->m_fixtures.Get(pBase->m_edges[1].m_fixtureIndex);


    const dmMesh * pMeshA = static_cast<const dmMesh *>(pFixtureA->m_pShape);


    UpdateTriangleBuffer(pWorld, pBase, pMutex);
    dmTriangleCache * aCache = m_aCache;


    dmTOIInput input;
    input.proxyB.Set(pFixtureB->m_pShape);
    input.sweepA = sweepA;
    input.sweepB = sweepB;

    dmTransform transformA1, transformA2;
    dmTransform transformB1, transformB2;

    transformA1.q = sweepA.m_quaternion0;
    transformA1.p = sweepA.m_center0 - dmRotate(transformA1.q, sweepA.m_localCenter);
    transformA2.q = sweepA.m_quaternion;
    transformA2.p = sweepA.m_center - dmRotate(transformA2.q, sweepA.m_localCenter);

    transformB1.q = sweepB.m_quaternion0;
    transformB1.p = sweepB.m_center0 - dmRotate(transformB1.q, sweepB.m_localCenter);
    transformB2.q = sweepB.m_quaternion;
    transformB2.p = sweepB.m_center - dmRotate(transformB2.q, sweepB.m_localCenter);


    dm_vec4 centroidB = pFixtureB->m_pShape->ComputeCentroid();
    dm_vec4 centroidBinA = dmMulT(transformA1, dmMul(transformB1, centroidB));

    pOutput->exitCode = dm_toiMiss;
    pOutput->eta = dm_vec4_maxFloat;

    dm_int32 triangleCount = m_triangleCount;
    for (dm_int32 i = 0; i < triangleCount; ++i)
    {
        dmTriangleCache * pCache = &aCache[i];
        dm_int32 triangleIndex = pCache->triangleIndex;
        dmTriangle triangleA;
        pMeshA->GetTriangle(&triangleA, triangleIndex);


        input.cache = pCache->gjkCache;
        input.proxyA.Set(&triangleA);


        dm_float32 descale = 1.0f / 32767.0f;
        dm_vec4 cachedAxis = dmVecSet(pCache->ax * descale, pCache->ay * descale, pCache->az * descale);

        if (dmGTZ(dmLengthSqr3(cachedAxis)))
        {
            dm_vec4 axisA1 = dmInvRotate(transformA1.q, cachedAxis);
            dm_vec4 axisA2 = dmInvRotate(transformA2.q, cachedAxis);
            dm_vec4 vA1, vA2;
            input.proxyA.GetSupport(&vA1, axisA1);
            input.proxyA.GetSupport(&vA2, axisA2);

            vA1 = dmMul(transformA1, vA1);
            vA2 = dmMul(transformA2, vA2);

            dm_vec4 axisB1 = -dmInvRotate(transformB1.q, cachedAxis);  // The original negates the rotated axis (fneg at 288:21 / 289:21, the unary minus after "= "), not cachedAxis.
            dm_vec4 axisB2 = -dmInvRotate(transformB2.q, cachedAxis);
            dm_vec4 vB1, vB2;
            input.proxyB.GetSupport(&vB1, axisB1);
            input.proxyB.GetSupport(&vB2, axisB2);

            vB1 = dmMul(transformB1, vB1);
            vB2 = dmMul(transformB2, vB2);

            dm_float32 s1 = dmDot3_f32(cachedAxis, vB1 - vA1);
            dm_float32 s2 = dmDot3_f32(cachedAxis, vB2 - vA2);
            // Inferred: the original tests s1 > 0 and s2 > 0 first (two fcmp #0.0 / b.le), and column 37 is the "-" of s2 - s1 after them.
            if (s1 > 0.0f && s2 > 0.0f && s2 - s1 >= -0.0164042f)
            {
                continue;
            }
        }


        dm_vec4 normalA = triangleA.GetNormal();
        dm_float32 offset = dmDot3_f32(normalA, centroidBinA - triangleA.m_p[0]);
        if (offset <= 0.0f)
        {
            continue;
        }

        dmTOIOutput output;
        dmTimeOfImpact(&output, &input);

        pCache->gjkCache = output.cache;


        const dm_vec4 shortMax = vdupq_n_f32(32767.0f);
        if (output.exitCode == dm_toiMiss)
        {

            dm_vec4 axis = output.pointB - output.pointA;
            if (!dmIsZeroSafe3(axis))
            {
                axis = dmNormalize3(axis);
                axis = dmClamp(axis * shortMax, -shortMax, shortMax);
                pCache->ax = (dm_int16)dmGetX(axis);
                pCache->ay = (dm_int16)dmGetY(axis);
                pCache->az = (dm_int16)dmGetZ(axis);
            }
            else
            {
                pCache->ax = 0;
                pCache->ay = 0;
                pCache->az = 0;
            }
        }
        else
        {
            pCache->ax = 0;
            pCache->ay = 0;
            pCache->az = 0;
        }

        if (output.exitCode == dm_toiHit && dmLT(output.eta, pOutput->eta))
        {
            *pOutput = output;
        }
        else if (output.exitCode == dm_toiOverlap)
        {

            *pOutput = output;
            return;
        }
    }
}

























void dmMeshContact::ClearBaseManifolds(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex)
{
    if (pBase->m_aManifolds)
    {
        if (pMutex) pMutex->Lock();
        pWorld->m_poolAllocator.Free(pBase->m_aManifolds, pBase->m_manifoldCount * sizeof(dmManifold), false);
        if (pMutex) pMutex->Unlock();
        pBase->m_aManifolds = NULL;
    }
    pBase->m_manifoldCount = 0;
}


void dmMeshContact::UpdateTriangleBuffer(dmWorld * pWorld, const dmContact * pBase, dmMutex * pMutex)
{
    const dmFixture * pFixtureA = pWorld->m_fixtures.Get(pBase->m_edges[0].m_fixtureIndex);
    const dmFixture * pFixtureB = pWorld->m_fixtures.Get(pBase->m_edges[1].m_fixtureIndex);

    const dmTransform transformA = pFixtureA->GetTransform(pWorld);


    const dmTransform invTransformA = dmInvert(transformA);





    const dmAABB aabb = dmTransformAABB(invTransformA, pFixtureB->m_aabb);

    if (m_fatAABB.Contains(aabb)) return;




    dm_vec4 center = aabb.GetCenter();
    dm_vec4 extents = aabb.GetExtents();
    const dm_vec4 k_scale = dm_vec4_one + dm_vec4_oneHalf;
    m_fatAABB.m_lowerBound = center - extents * k_scale;
    m_fatAABB.m_upperBound = center + extents * k_scale;

    const dmMesh * pMeshA = static_cast<const dmMesh *>(pFixtureA->m_pShape);








    dmBuffer<dm_int32, 128> newIndices;
    pMeshA->Query(&newIndices, m_fatAABB);
    dm_int32 newCount = newIndices.GetCount();


    for (dm_int32 i = 1; i < newCount; ++i)
    {
        dmAssert(newIndices[i-1] < newIndices[i]);
    }


    dmTriangleCache * aCache = m_aCache;
    dmBuffer<dmTriangleCache, 32> newCache;
    newCache.SetCount(newCount);
    dm_int32 oldCount = m_triangleCount;
    dm_int32 i1 = 0;
    for (dm_int32 i2 = 0; i2 < newCount; ++i2)
    {
        dm_int32 newIndex = newIndices[i2];
        newCache[i2].triangleIndex = newIndex;

        while (i1 < oldCount && aCache[i1].triangleIndex < newIndex)
        {
            ++i1;
        }

        if (i1 < oldCount && aCache[i1].triangleIndex == newIndex)
        {
            newCache[i2] = aCache[i1];
        }










        else
        {
            newCache[i2].ax = 0;
            newCache[i2].ay = 0;
            newCache[i2].az = 0;
            newCache[i2].gjkCache.Clear();
            newCache[i2].satCache.Clear();
        }
    }

    if (m_triangleCapacity < newCount)
    {
        if (pMutex) pMutex->Lock();
        pWorld->m_poolAllocator.Free(m_aCache, m_triangleCapacity * sizeof(dmTriangleCache), true);
        m_triangleCapacity = dmMax(newCount * 2, 4);
        m_aCache = static_cast<dmTriangleCache *>(pWorld->m_poolAllocator.Allocate(m_triangleCapacity * sizeof(dmTriangleCache), true));
        if (pMutex) pMutex->Unlock();
        aCache = m_aCache;
    }

    for (dm_int32 i = 0; i < newCount; ++i)
    {
        aCache[i] = newCache[i];
    }

    m_triangleCount = newCount;
}








void dmMeshContact::ReduceManifolds(dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmManifold * manifolds, dm_int32 manifoldCount, dm_vec4 quatRel, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex)
{
    if (manifoldCount == 0) return;




    dm_int32 oldManifoldCount = baseManifoldCount;
    dmAssert(oldManifoldCount <= dm_maxTreeManifolds);
    dmManifold oldManifolds[dm_maxTreeManifolds];
    memcpy(oldManifolds, aBaseManifolds, oldManifoldCount * sizeof(dmManifold));



    for (dm_int32 i = 0; i < dm_maxTreeManifolds; ++i)
    {
        dmManifold_ClearNewFlags(&oldManifolds[i]);
    }




    dm_vec4 normals[dm_maxTreeManifolds];
    dm_int32 clusterCount = 1;
    normals[0] = manifolds[0].m_normal;
    const dm_float32 k_cos5Degrees = 0.9961947f;

    for (dm_int32 i = 1; i < manifoldCount && clusterCount < dm_maxTreeManifolds; ++i)
    {
        dm_float32 maxDot = -3.40282347e+38f;
        for (dm_int32 j = 0; j < clusterCount; ++j)
        {
            dm_float32 dot = dmDot3_f32(manifolds[i].m_normal, normals[j]);
            maxDot = dmMax(maxDot, dot);
        }

        if (maxDot < k_cos5Degrees)
        {
            normals[clusterCount] = manifolds[i].m_normal;
            ++clusterCount;
        }
    }

    dmAssert(clusterCount <= dm_maxTreeManifolds);


    dmBuffer<dm_int32, 32> clusterMemberships;
    clusterMemberships.SetCount(manifoldCount);
    for (dm_int32 i = 0; i < manifoldCount; ++i)
    {
        clusterMemberships[i] = -1;
    }





    for (dm_int32 iter = 0; iter < 4; ++iter)
    {
        dm_vec4 normalSums[dm_maxTreeManifolds];
        for (dm_int32 i = 0; i < clusterCount; ++i)
        {
            normalSums[i] = dmVecSet(0.0f);
        }

        bool changedMembership = false;
        for (dm_int32 i = 0; i < manifoldCount; ++i)
        {
            dm_float32 bestDot = -3.40282347e+38f;
            dm_int32 bestIndex = 0;
            for (dm_int32 j = 0; j < clusterCount; ++j)
            {
                dm_vec4 nj = normals[j];
                if (dmLengthSqr3_f32(nj) == 0.0f)
                {
                    continue;
                }


                dm_float32 dot = dmDot3_f32(manifolds[i].m_normal, nj);
                if (dot > bestDot)
                {
                    bestDot = dot;
                    bestIndex = j;
                }
            }

            normalSums[bestIndex] = dmAdd(manifolds[i].m_normal, normalSums[bestIndex]);

            if (bestIndex != clusterMemberships[i])
            {
                clusterMemberships[i] = bestIndex;
                changedMembership = true;
            }
        }


        if (!changedMembership)
        {
            break;
        }



        for (dm_int32 i = 0; i < clusterCount; ++i)
        {
            normals[i] = dmIsZeroSafe3(normalSums[i])
                ? dmVecSet(0.0f)
                : dmNormalize3(normalSums[i]);

        }
    }







    dmAssert(clusterCount <= dm_maxTreeManifolds);
    dmBuffer<dmManifoldPoint, 64> aClusterPoints[dm_maxTreeManifolds];


    for (dm_int32 i = 0; i < manifoldCount; ++i)
    {
        const dmManifold * pManifold = &manifolds[i];
        dm_int32 cluster = clusterMemberships[i];
        dmAssert(cluster < dm_maxTreeManifolds);

        for (dm_int32 j = 0; j < pManifold->m_pointCount; ++j)
        {
            dmManifoldPoint point = pManifold->m_points[j];
            aClusterPoints[cluster].PushBack(point);
        }
    }



    dmManifold aTempManifolds[dm_maxTreeManifolds];
    dm_int32 newManifoldCount = 0;
    for (dm_int32 i = 0; i < clusterCount; ++i)
    {
        if (aClusterPoints[i].GetCount() == 0)
        {
            continue;
        }


        dm_int32 pointCount = aClusterPoints[i].GetCount();
        const dmManifoldPoint * points = aClusterPoints[i].GetPointer();
        dmManifold * pManifold = &aTempManifolds[newManifoldCount];
        dmManifold_Create(pManifold);
        pManifold->m_normal = normals[i];
        ++newManifoldCount;


        dmManifold_ReducePoints(pManifold, points, pointCount);

        dmAssert(pManifold->m_pointCount <= pointCount);
        dmAssert(0 < pManifold->m_pointCount && pManifold->m_pointCount <= dm_maxManifoldPoints);
    }
    baseManifoldCount = newManifoldCount;
    if (pPoolAllocator == NULL)
    {

        memcpy(aBaseManifolds, aTempManifolds, baseManifoldCount * sizeof(dmManifold));
    }
    else
    {


        if (oldManifoldCount != baseManifoldCount)
        {
            if (pMutex) pMutex->Lock();
            pPoolAllocator->Free(aBaseManifolds, oldManifoldCount * sizeof(dmManifold), false);
            aBaseManifolds = static_cast<dmManifold *>(pPoolAllocator->Allocate(baseManifoldCount * sizeof(dmManifold), false));
            if (pMutex) pMutex->Unlock();
        }
        memcpy(aBaseManifolds, aTempManifolds, baseManifoldCount * sizeof(dmManifold));
    }




    bool manifoldMatched[dm_maxTreeManifolds] = { false, false, false };

    for (dm_int32 i = 0; i < baseManifoldCount; ++i)
    {
        dm_int32 index = -1;
        for (dm_int32 j = 0; j < oldManifoldCount; ++j)
        {
            if (manifoldMatched[j])
            {
                continue;
            }

            dm_float32 dot = dmDot3_f32(dmRotate(quatRel, aBaseManifolds[i].m_normal), oldManifolds[j].m_normal);
            if (dot > k_cos5Degrees)
            {
                manifoldMatched[j] = true;
                index = j;
                break;
            }
        }

        if (index == -1)
        {
            dmManifold_ClearCache(&aBaseManifolds[i]);
            continue;
        }


        bool persisted[4] = { false, false, false, false };

        dmManifold * pNew = &aBaseManifolds[i];
        const dmManifold * pOld = &oldManifolds[index];

        pNew->m_tangentImpulse = pOld->m_tangentImpulse;
        pNew->m_rollingImpulse = pOld->m_rollingImpulse;
        pNew->m_twistImpulse = pOld->m_twistImpulse;



        for (dm_int32 j = 0; j < pNew->m_pointCount; ++j)
        {
            dmManifoldPoint * mp = &pNew->m_points[j];

            bool found = false;
            dm_uint64 key = mp->id.key;
            for (dm_int32 k = 0; k < pOld->m_pointCount; ++k)
            {
                if (persisted[k])
                {
                    continue;
                }

                const dmManifoldPoint * mp0 = &pOld->m_points[k];

                if (mp0->id.key == key)
                {
                    persisted[k] = true;
                    mp->normalImpulse = mp0->normalImpulse;
                    mp->isNew = false;
                    found = true;
                    break;
                }
            }






            if (!found)
            {
                mp->isNew = true;
                mp->normalImpulse = 0.0f;
            }
        }
    }
}




bool dmMeshContact::UpdateManifolds(dmWorld * pWorld, dmContact * pBase, dmMutex * pMutex, bool isTOIEvent)

{
    const dmFixture * pFixtureA = pWorld->m_fixtures.Get(pBase->m_edges[0].m_fixtureIndex);
    const dmFixture * pFixtureB = pWorld->m_fixtures.Get(pBase->m_edges[1].m_fixtureIndex);

    dmShape::Type typeA = pFixtureA->m_pShape->m_type;
    dmShape::Type typeB = pFixtureB->m_pShape->m_type;

    const dmTransform xfA = pFixtureA->GetTransform(pWorld);
    const dmTransform xfB = pFixtureB->GetTransform(pWorld);

    if (typeA != dmShape::e_mesh)
    {
        pBase->m_manifoldCount = 0;
        return false;
    }


    const dmMesh * pMeshA = static_cast<const dmMesh *>(pFixtureA->m_pShape);






    switch (typeB) {
    case dmShape::e_sphere: {
            const dmSphere * pSphereB = static_cast<const dmSphere *>(pFixtureB->m_pShape);


            dmSphere sphereB;
            {
                dm_vec4 p = dmMulT(xfA, dmMul(xfB, pSphereB->m_p));
                dm_float32 radius = pSphereB->m_radius;
                sphereB.Create(p, radius);
            }

            UpdateTriangleBuffer(pWorld, pBase, pMutex);


            if (m_triangleCount == 0)
            {
                ClearBaseManifolds(pWorld, pBase, pMutex);
                return false;
            }









            if (!ComputeManifoldsTrianglesSphere(pMeshA, m_aCache, m_triangleCount, pBase->m_aManifolds, pBase->m_manifoldCount, sphereB, xfA, &pWorld->m_poolAllocator, pMutex))
            {
                ClearBaseManifolds(pWorld, pBase, pMutex);
                return false;
            }
            break;
        }
    case dmShape::e_capsule: {
            const dmCapsule * pCapsuleB = static_cast<const dmCapsule *>(pFixtureB->m_pShape);


            dmCapsule capsuleB;
            {
                dm_vec4 p1 = dmMulT(xfA, dmMul(xfB, pCapsuleB->m_p1));
                dm_vec4 p2 = dmMulT(xfA, dmMul(xfB, pCapsuleB->m_p2));
                dm_float32 radius = pCapsuleB->m_radius;
                capsuleB.Create(p1, p2, radius);
            }

            UpdateTriangleBuffer(pWorld, pBase, pMutex);


            if (m_triangleCount == 0)
            {
                ClearBaseManifolds(pWorld, pBase, pMutex);
                return false;
            }









            if (!ComputeManifoldsTrianglesCapsule(pMeshA, m_aCache, m_triangleCount, pBase->m_aManifolds, pBase->m_manifoldCount, capsuleB, xfA, &pWorld->m_poolAllocator, pMutex))
            {
                ClearBaseManifolds(pWorld, pBase, pMutex);
                return false;
            }
            break;
        }
    case dmShape::e_polytope: {

            const dmTransform transform = dmMulT(xfA, xfB);

            const dmPolytope * pPolytopeB = static_cast<const dmPolytope *>(pFixtureB->m_pShape);

            UpdateTriangleBuffer(pWorld, pBase, pMutex);


            if (m_triangleCount == 0)
            {
                ClearBaseManifolds(pWorld, pBase, pMutex);
                return false;
            }









            if (!ComputeManifoldsTrianglesPolytope(pMeshA, m_aCache, m_triangleCount, pBase->m_aManifolds, pBase->m_manifoldCount, pPolytopeB, transform, xfA, &pWorld->m_poolAllocator, pMutex, isTOIEvent))
            {
                ClearBaseManifolds(pWorld, pBase, pMutex);
                return false;
            }
            break;
        }
    default:
        pBase->m_manifoldCount = 0;
        return false;
    }



    if (pMeshA->m_meshType == dmMesh::e_heightField && pWorld->m_materialCount > 0)
    {
        const dmHeightField * pHeightField = static_cast<const dmHeightField *>(pMeshA);


        dm_float32 friction = 0.0f;
        dm_float32 restitution = 0.0f;
        dm_int32 foundCount = 0;
        for (dm_int32 i = 0; i < pBase->m_manifoldCount; ++i)
        {
            const dmManifold * pManifold = &pBase->m_aManifolds[i];
            for (dm_int32 j = 0; j < pManifold->m_pointCount; ++j)
            {
                dm_int32 triangleIndex = pManifold->m_points[j].id.fp.triangleIndex;
                dm_int32 materialIndex = pHeightField->GetMaterial(triangleIndex);
                if (materialIndex < pWorld->m_materialCount)
                {
                    friction += pWorld->m_aMaterials[materialIndex].friction;
                    restitution += pWorld->m_aMaterials[materialIndex].restitution;
                    ++foundCount;
                }
            }
        }

        if (foundCount > 0)
        {
            dm_float32 invCount = 1.0f / foundCount;
            pBase->m_friction = invCount * friction;
            pBase->m_restitution = invCount * restitution;
        }
    }

    return pBase->m_manifoldCount > 0;
}


bool dmMeshContact::ComputeManifoldsTrianglesSphere(const dmMesh * pMeshInput, dmTriangleCache * aCache, dm_int32 triangleCount, dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmSphere & sphereB, const dmTransform & xfA, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex)
{

    dmBuffer<dmManifold, 16> tempManifolds;

    for (dm_int32 i = 0; i < triangleCount; ++i)
    {
        dm_int32 triangleIndex = aCache[i].triangleIndex;
        dmTriangle triangleA;
        pMeshInput->GetTriangleFull(&triangleA, triangleIndex);


        dmManifold manifold;
        dmManifold_Create(&manifold);
        dmTSManifold(&manifold, &aCache[i].gjkCache, &triangleA, &sphereB);

        if (manifold.m_pointCount > 0)
        {
            tempManifolds.PushBack(manifold);
        }
    }









    if (tempManifolds.GetCount() == 0) return false;





    dmMeshContact::ReduceManifolds(aBaseManifolds, baseManifoldCount, tempManifolds.GetPointer(), tempManifolds.GetCount(), xfA.q, pPoolAllocator, pMutex);


    for (dm_int32 i = 0; i < baseManifoldCount; ++i)
    {
        dmManifold * pManifold = &aBaseManifolds[i];
        pManifold->m_normal = dmRotate(xfA.q, pManifold->m_normal);
        for (dm_int32 j = 0; j < pManifold->m_pointCount; ++j)
        {
            pManifold->m_points[j].point = dmMul(xfA, pManifold->m_points[j].point);
        }
    }

    return true;
}

bool dmMeshContact::ComputeManifoldsTrianglesCapsule(const dmMesh * pMeshInput, dmTriangleCache * aCache, dm_int32 triangleCount, dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmCapsule & capsuleB, const dmTransform & xfA, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex)
{

    dmBuffer<dmManifold, 16> tempManifolds;
    for (dm_int32 i = 0; i < triangleCount; ++i)
    {
        dm_int32 triangleIndex = aCache[i].triangleIndex;
        dmTriangle triangleA;
        pMeshInput->GetTriangleFull(&triangleA, triangleIndex);


        dmManifold manifold;
        dmManifold_Create(&manifold);
        dmTCManifold(&manifold, &aCache[i].gjkCache, &triangleA, &capsuleB);

        if (manifold.m_pointCount > 0)
        {
            tempManifolds.PushBack(manifold);
        }
    }









    if (tempManifolds.GetCount() == 0) return false;





    dmMeshContact::ReduceManifolds(aBaseManifolds, baseManifoldCount, tempManifolds.GetPointer(), tempManifolds.GetCount(), xfA.q, pPoolAllocator, pMutex);


    for (dm_int32 i = 0; i < baseManifoldCount; ++i)
    {
        dmManifold * pManifold = &aBaseManifolds[i];
        pManifold->m_normal = dmRotate(xfA.q, pManifold->m_normal);
        for (dm_int32 j = 0; j < pManifold->m_pointCount; ++j)
        {
            pManifold->m_points[j].point = dmMul(xfA, pManifold->m_points[j].point);
        }
    }

    return true;
}

bool dmMeshContact::ComputeManifoldsTrianglesPolytope(const dmMesh * pMeshInput, dmTriangleCache * aCache, dm_int32 triangleCount, dmManifold *& aBaseManifolds, dm_int32 & baseManifoldCount, const dmPolytope * pPolytopeB, const dmTransform & transform, const dmTransform & xfA, dmPoolAllocator * pPoolAllocator, dmMutex * pMutex, bool isTOIEvent)
{


    dmBuffer<dmManifold, 16> tempManifolds;
    for (dm_int32 i = 0; i < triangleCount; ++i)
    {
        dm_int32 triangleIndex = aCache[i].triangleIndex;
        dmTriangle triangleA;
        pMeshInput->GetTriangleFull(&triangleA, triangleIndex);




        if (isTOIEvent)
            aCache[i].satCache.Clear();


        dmManifold manifold;
        dmManifold_Create(&manifold);
        dmTPManifold(&manifold, &aCache[i].satCache, &triangleA, pPolytopeB, transform);

        if (manifold.m_pointCount > 0)
        {
            tempManifolds.PushBack(manifold);
        }
    }









    if (tempManifolds.GetCount() == 0) return false;





    dmMeshContact::ReduceManifolds(aBaseManifolds, baseManifoldCount, tempManifolds.GetPointer(), tempManifolds.GetCount(), xfA.q, pPoolAllocator, pMutex);


    for (dm_int32 i = 0; i < baseManifoldCount; ++i)
    {
        dmManifold * pManifold = &aBaseManifolds[i];
        dmAssert(pManifold->m_pointCount <= dm_maxManifoldPoints);
        pManifold->m_normal = dmRotate(xfA.q, pManifold->m_normal);

        for (dm_int32 j = 0; j < pManifold->m_pointCount; ++j)
        {
            pManifold->m_points[j].point = dmMul(xfA, pManifold->m_points[j].point);
        }
    }

    return true;
}
