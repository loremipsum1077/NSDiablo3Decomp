#include "../../../Contrib/Contrib/Domino/Include/dmApi.h"
#include "../../../Contrib/Contrib/Domino/Include/dmMath.h"
#include "../../../Contrib/Contrib/Domino/Include/dmStructures.h"
#include "../../Common/Definitions/PhysMeshDefinition.h"
#include "../../Common/Definitions/SceneDefinition.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Shape.h"
#include "../../Sigma/Math/Transform.h"
#include "../SWorld.h"

// Original Domino dmBody.cpp/dmFixture.cpp declarations and public dmApi.h API.
extern const dmFixtureId dm_nullFixtureId;
bool operator==(dmBodyId id1, dmBodyId id2);
bool operator!=(dmBodyId id1, dmBodyId id2);
bool operator==(dmFixtureId id1, dmFixtureId id2);
bool operator!=(dmFixtureId id1, dmFixtureId id2);
void dmWorld_DestroyBody(dmBodyId id);
void DominoRemoveBody(dmBodyId bodyId);
























static dm_vec4 sConvert(const Vector3D & v)
{

    return dmVecSet(v.x, v.y, v.z);
}











static dm_vec4 sConvert(const Quaternion & q)
{

    return dmVecSet(q.wvImag.x, q.wvImag.y, q.wvImag.z, q.wdReal);
}


dmFixtureId sCreateFixture(dmBodyId bodyId, float flScale, const CollisionMesh * pMesh)
{










    dmFilter filter;
    if (pMesh->uFlags & 2)
    {
        if (pMesh->uMaterial == Shape::WATER_MATERIAL)
            filter.m_categoryBits = 0x40;
        else
            filter.m_categoryBits = 0x200;
    }
    else
    {











        filter.m_categoryBits = 1 | (((pMesh->uFlags >> 2) & 3) << 7);
    }
    // Original filter word at 0x8bb29c contains include mask 0xffde and group index zero.
    filter.m_includeMask = 0xffde;
    filter.m_groupIndex = 0;





    dmTreeMeshDef meshDef;




















    // pDupedMesh is null in this original build: the emitted user-data store is zero.
    CollisionMesh * pDupedMesh = NULL;
    meshDef.m_pData = pMesh;


    meshDef.m_pUserData = pDupedMesh;
    meshDef.m_density = 0.0f;
    meshDef.m_scale = dmVecSet(flScale);
    meshDef.m_friction = 0.9f;
    meshDef.m_restitution = 0.0f;
    meshDef.m_filter = filter;

    dmFixtureId fixtureId = dmBody_CreateFixture(bodyId, &meshDef);
    return fixtureId;
}


dmBodyId sCreatePhysMeshBody(dmWorldId worldId,
                           const PhysMeshDefinition * pdefPhysMesh,
                           const PRTransform & tTransform,
                           float flScale,
                           LPCSTR pszName)

{


    if (!pdefPhysMesh->nCollisionMeshCount)
    {
        return dm_nullBodyId;
    }


    dmBodyDef bodyDef;
    bodyDef.m_bodyType = dm_staticBody;
    bodyDef.m_position = sConvert(tTransform.wp);
    bodyDef.m_quaternion = sConvert(tTransform.q);
    bodyDef.m_pName = pszName; bodyDef.m_pUserData = const_cast<PhysMeshDefinition *>(pdefPhysMesh);

    dmBodyId bodyId = dmWorld_CreateBody(worldId, &bodyDef);

    if (bodyId == dm_nullBodyId)
    {
        return dm_nullBodyId;
    }

    // DWARF pdefPhysMesh is const, but dmBodyDef::m_pUserData is void *; the cast at line 167 is type-required.
    bool bFound = false;
    for (int32 i = 0; i < pdefPhysMesh->nCollisionMeshCount; ++i)
    {
        const CollisionMesh * pMesh = &pdefPhysMesh->ptCollisionMeshes[i];
        if (!pMesh->m_triangleCount)
        {
            continue;
        }

        dmFixtureId fixtureId = sCreateFixture(bodyId, flScale, pMesh);
        if (fixtureId == dm_nullFixtureId)
        {
            DominoRemoveBody(bodyId);
            return dm_nullBodyId;
        }
        bFound = true;
    }
    if (!bFound)
    {
        dmWorld_DestroyBody(bodyId);
        return dm_nullBodyId;
    }
    return bodyId;
}









dmBodyId DominoCreateBodyFromScene(const SceneCommonData & tSCD)
{

    dmBodyId bodyId = dm_nullBodyId;
    SWorld * ptSWorld = SWorldTryToGet(tSCD.idSWorld);
    if (ptSWorld)
    {
        ScopedSceneDefinitionLockR pdefScene(tSCD.snoScene);
        if (pdefScene->snoPhysMesh != SNO_NONE)
        {
            const PhysMeshDefinition * pdefPhysMesh = static_cast<const PhysMeshDefinition *>(gptGroupDefPhysMesh->LockR(pdefScene->snoPhysMesh, FALSE));
            bodyId = sCreatePhysMeshBody(ptSWorld->worldId,
                                         pdefPhysMesh,
                                         tSCD.transformWorld,
                                         1.0f,
                                         SNOToString(SCENE_SNO, tSCD.snoScene).str());



            if (bodyId == dm_nullBodyId)
            {

                gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
            }
        }

    }

    return bodyId;
}


dmBodyId DominoCreateBodyFromActor(
    SWorldID idSWorld,
    SNO snoPhysMesh,
    const PRTransform & tTransform,
    float flScale)
{
    if (snoPhysMesh == SNO_NONE)
    {
        return dm_nullBodyId;
    }
    dmBodyId bodyId = dm_nullBodyId;
    SWorld * ptSWorld = SWorldTryToGet(idSWorld);
    if (ptSWorld)
    {
        const PhysMeshDefinition * pdefPhysMesh = static_cast<const PhysMeshDefinition *>(gptGroupDefPhysMesh->LockR(snoPhysMesh, FALSE));
        bodyId = sCreatePhysMeshBody(
            ptSWorld->worldId,
            pdefPhysMesh,
            tTransform,
            flScale,
            CRefString().Format("PhysMesh:%d", snoPhysMesh));






        if (bodyId == dm_nullBodyId)
        {

            gptGroupDefPhysMesh->UnlockR((void *&)pdefPhysMesh);
        }
    }


    return bodyId;
}


void DominoRemoveBody(dmBodyId bodyId)

{
    if (bodyId != dm_nullBodyId)
    {
        dmFixtureId fixtureId = dmBody_GetFirstFixture(bodyId);
        while (fixtureId != dm_nullFixtureId)
        {
            CollisionMesh * pMesh = static_cast<CollisionMesh *>(dmFixture_GetUserData(fixtureId));
            if (pMesh)
            {
                SigmaMemoryFree(pMesh->m_pRoot, NULL); pMesh->m_pRoot = NULL;
                SigmaMemoryFree(pMesh->m_aTriangles, NULL); pMesh->m_aTriangles = NULL;
                SigmaMemoryFree(pMesh->m_aVertices, NULL); pMesh->m_aVertices = NULL;
                SigmaMemoryFree(pMesh, NULL);
            }

            fixtureId = dmBody_GetNextFixture(fixtureId);
        }


        PhysMeshDefinition * pdefPhysMesh = static_cast<PhysMeshDefinition *>(dmBody_GetUserData(bodyId));


        dmWorld_DestroyBody(bodyId);


        if (pdefPhysMesh)
        {
            gptGroupDefPhysMesh->UnlockR(reinterpret_cast<void *&>(pdefPhysMesh));
        }
    }
}



BOOL DominoRaycastAgainstWorld(const WorldPlace & placeStart, const WorldPlace & placeEnd,
                               Vector3D & vCollidePoint, Vector3D & vCollideNormal,
                               float & flUnitTravel, uint16 uExcludeMask)
{




    if (placeStart.idSWorld == placeEnd.idSWorld)
    {
        SWorld * ptSWorld = SWorldTryToGet(placeStart.idSWorld);
        if (ptSWorld)
        {
            dmWorldRayCastInput input;
            input.p1 = sConvert(placeStart.wp);
            input.p2 = sConvert(placeEnd.wp);
            input.filter.m_includeMask = 0xffff;
            input.filter.m_excludeMask = uExcludeMask;
            input.pUserData = NULL;

            dmWorldRayCastOutput output;
            bool hit = dmWorld_RayCastClosest(ptSWorld->worldId, &output, &input);
            if (hit)
            {
                vCollidePoint = Vector3D(dmGetX(output.p), dmGetY(output.p), dmGetZ(output.p));
                vCollideNormal = Vector3D(dmGetX(output.normal), dmGetY(output.normal), dmGetZ(output.normal));
                flUnitTravel = output.fraction;
                return TRUE;
            }
            else
            {
                vCollidePoint = placeEnd.wp;
                flUnitTravel = 1.0f;
                vCollideNormal = cgwvNull;
                return FALSE;
            }
        }
    }


    vCollidePoint = placeStart.wp;
    flUnitTravel = 0.0f;
    vCollideNormal = cgwvNull;
    return TRUE;
}

