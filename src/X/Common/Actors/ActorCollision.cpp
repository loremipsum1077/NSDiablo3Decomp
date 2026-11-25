// Original file: X/Common/Actors/ActorCollision.cpp

#include "ActorCollision.h"
#include "../ActorCommonData.h"
#include "../Game.h"
#include "../GameCommonData.h"
#include "../World.h"
#include "../WorldPlace.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XBitArray.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XSphereArray.h"
#include "../../Sigma/DataStructures/XSphereNode.h"
#include "../../Sigma/Math/Cylinder.h"
#include "../../Sigma/Math/Frustum.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include <math.h>









struct ActorSphereTree
{
    ActorSphereTree() : tSphereTree(AppGetCurrentMemoryInterface()) {}

    XSphereArray tSphereTree;  // 0x00
};

typedef XChainedFixedBlockAllocator<ActorSphereTree> TActorCollisionPool;

struct ActorCollisionGlobals {  // size 0x30
    TActorCollisionPool tActorCollisionPool;  // 0x00
};






void ActorCollisionInitializeForGame()
{

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    tGCD.ptActorCollisionGlobals = (ActorCollisionGlobals *)SigmaMemoryNew(sizeof(ActorCollisionGlobals), 4, ptMemoryInterface, TRUE);

    tGCD.ptActorCollisionGlobals->tActorCollisionPool.Initialize();
    tGCD.ptActorCollisionGlobals->tActorCollisionPool.SetMemoryInterface(ptMemoryInterface);
    tGCD.ptActorCollisionGlobals->tActorCollisionPool.SetBlockCount_Tracked(1000);
}




void ActorCollisionDisposeForGame()
{

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    tGCD.ptActorCollisionGlobals->tActorCollisionPool.Dispose();
    SigmaMemoryFree(tGCD.ptActorCollisionGlobals, ptMemoryInterface); tGCD.ptActorCollisionGlobals = NULL;
}




void ActorCollisionInitializeForWorld(WorldCommonData & tWCD)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ActorCollisionGlobals * ptActorCollisionGlobals = tGCD.ptActorCollisionGlobals;
    TActorCollisionPool & tActorCollisionPool = ptActorCollisionGlobals->tActorCollisionPool;

    if (tWCD.ptActorSphereTreeNormal == NULL)
    {
        tWCD.ptActorSphereTreeNormal = tActorCollisionPool.Alloc_Tracked();
        tWCD.ptActorSphereTreeNormal->tSphereTree.Init("ActorCollisionNormal", 512, 0.0f);
    }

    if (tWCD.ptActorSphereTreeFastMover == NULL)
    {
        tWCD.ptActorSphereTreeFastMover = tActorCollisionPool.Alloc_Tracked();
        tWCD.ptActorSphereTreeFastMover->tSphereTree.Init("ActorCollisionFastMover", 512, 0.0f);
    }

    if (tWCD.ptActorSphereTreeItemsInWorld == NULL)
    {
        tWCD.ptActorSphereTreeItemsInWorld = tActorCollisionPool.Alloc_Tracked();
        tWCD.ptActorSphereTreeItemsInWorld->tSphereTree.Init("ActorCollisionFlippy", 512, 0.0f);
    }
}




void ActorCollisionDisposeForWorld(WorldCommonData & tWCD)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ActorCollisionGlobals * ptActorCollisionGlobals = tGCD.ptActorCollisionGlobals;
    TActorCollisionPool & tActorCollisionPool = ptActorCollisionGlobals->tActorCollisionPool;

    if (tWCD.ptActorSphereTreeNormal)
    {
        tWCD.ptActorSphereTreeNormal->tSphereTree.Uninit();
        tActorCollisionPool.Free(tWCD.ptActorSphereTreeNormal);
        tWCD.ptActorSphereTreeNormal = NULL;
    }

    if (tWCD.ptActorSphereTreeFastMover)
    {
        tWCD.ptActorSphereTreeFastMover->tSphereTree.Uninit();
        tActorCollisionPool.Free(tWCD.ptActorSphereTreeFastMover);
        tWCD.ptActorSphereTreeFastMover = NULL;
    }

    if (tWCD.ptActorSphereTreeItemsInWorld)
    {
        tWCD.ptActorSphereTreeItemsInWorld->tSphereTree.Uninit();
        tActorCollisionPool.Free(tWCD.ptActorSphereTreeItemsInWorld);
        tWCD.ptActorSphereTreeItemsInWorld = NULL;
    }
}



ActorSphereTree * ActorCollisionGetSphereTree(const WorldCommonData & tWCD, ActorSphereTreeTypes eSphereTreeType)
{


    switch (eSphereTreeType)
    {
    case ACTOR_SPHERE_TREE_FAST_MOVER: return tWCD.ptActorSphereTreeFastMover;



    case ACTOR_SPHERE_TREE_ITEMS_IN_WORLD: return tWCD.ptActorSphereTreeItemsInWorld;



    default: return tWCD.ptActorSphereTreeNormal;
    }
}



ActorSphereTree * ActorCollisionGetSphereTree(const SWorldID idSWorld, const ActorCommonData & tACD)
{

    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);

    if (ptWCD == NULL)
        return NULL;
    if (tACD.dwFlags & (1 << ACD_FASTMOVER_BIT))
    {
        return ActorCollisionGetSphereTree(*ptWCD, ACTOR_SPHERE_TREE_FAST_MOVER);
    }
    if (tACD.IsItem() && tACD.IsInWorld())
    {
        return ActorCollisionGetSphereTree(*ptWCD, ACTOR_SPHERE_TREE_ITEMS_IN_WORLD);
    }

    return ActorCollisionGetSphereTree(*ptWCD, ACTOR_SPHERE_TREE_NORMAL);
}










void ActorCollisionInitialize(ActorCommonData & tACD)
{

    SWorldID idSWorld = tACD.GetSWorldID();
    ActorSphereTree * ptActorSphereTree = ActorCollisionGetSphereTree(idSWorld, tACD);


    WorldLocation & loc = tACD.GetWorldLocation();
    if (ptActorSphereTree->tSphereTree.IsSphereValid(loc.wsBounds))
    {




        loc.ptSphereNode = ptActorSphereTree->tSphereTree.AddSphereNode(loc.wsBounds, (void *)(intptr_t)tACD.id);
    }
}



void ActorCollisionDispose(ActorCommonData & tACD)
{

    SWorldID idSWorld = tACD.GetSWorldID();
    ActorSphereTree * ptActorSphereTree = ActorCollisionGetSphereTree(idSWorld, tACD);
    if (tACD.IsInWorld())
    {




        WorldLocation & loc = tACD.GetWorldLocation();
        if (ptActorSphereTree)
        {
            ptActorSphereTree->tSphereTree.RemoveSphereNode(loc.ptSphereNode);
        }
        loc.ptSphereNode = NULL;
    }
}



static void sSetBoundingSphere(XSphereArray * ptSphereTree, const Sphere & wsOld, const Sphere & wsActor, XSphereNode * ptSphereNode)
{




    if (Math::Abs(wsOld.wpCenter.x - wsActor.wpCenter.x) < EPSILON &&
        Math::Abs(wsOld.wpCenter.y - wsActor.wpCenter.y) < EPSILON &&
        Math::Abs(wsOld.wpCenter.z - wsActor.wpCenter.z) < EPSILON &&
        Math::Abs(wsOld.wdRadius - wsActor.wdRadius) < EPSILON)
    {
        return;
    }

    ptSphereTree->ModifySphereNode(ptSphereNode, wsActor);
}






void ACDSetBoundingSphere(ActorCommonData & tACD, const Sphere & wsOld)
{


    SWorldID idSWorld = tACD.GetSWorldID();
    ActorSphereTree * ptActorSphereTree = ActorCollisionGetSphereTree(idSWorld, tACD);


    WorldLocation & loc = tACD.GetWorldLocation();
    if (loc.ptSphereNode == NULL)
    {
        return;
    }


    if (Math::Abs(loc.wsBounds.wpCenter.x) >= 100000.0f ||
        Math::Abs(loc.wsBounds.wdRadius) >= 100000.0f ||
        !isfinite(loc.wsBounds.wpCenter.x) ||
        !isfinite(loc.wsBounds.wdRadius))
    {
        return;
    }








    sSetBoundingSphere(&ptActorSphereTree->tSphereTree, wsOld, loc.wsBounds, loc.ptSphereNode);
}








static void sACDsIntersectSphereATree(const Sphere & tSphere, const ACDID idACDIgnore, XPooledDataIDList & listACDs, ActorSphereTree * ptActorSphereTree)
{




    XSphereArray * ptSphereTree = &ptActorSphereTree->tSphereTree;

    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());
    ptSphereTree->SphereCull(tSphere, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        ACDID idACD = (ACDID)(intptr_t)ptSphereNode->GetUserData();
        if (idACD != idACDIgnore)
        {
            listACDs.AddTail_Tracked(idACD);
        }
    }
}






void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags)
{





    WorldCommonData * ptWCD = WorldTryGetCommonData(tPlace.idSWorld);
    if (!ptWCD) return;

    Sphere tSphere;
    tSphere.Set(tPlace.wp, wdRadius);

    for (int32 t = 0; t < ACTOR_SPHERE_TREE_NUM_TYPES; t++)
    {
        if (dwSphereTreeFlags & (1 << t))
        {
            sACDsIntersectSphereATree(tSphere, idACDIgnore, listACDs, ActorCollisionGetSphereTree(*ptWCD, (ActorSphereTreeTypes)t));
        }
    }
}











void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags)
{





    XPooledDataIDList listACDsSpheres(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(placeCenter, wdRadius, idACDIgnore, listACDsSpheres, dwSphereTreeFlags);






    Sphere tSphere;
    tSphere.wpCenter = placeCenter.wp;
    tSphere.wdRadius = wdRadius;

    POS pos = listACDsSpheres.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listACDsSpheres.GetNext(pos);

        ActorCommonData & tACD = ACDGet(idACD);
        if (Intersect::AxialCylinderAndSphere(tACD.GetWorldLocation().wcylBounds, tSphere))
        {
            listACDs.AddTail_Tracked(idACD);
        }
    }
}



void ACDsCylinderIntersectCylinder(const WorldPlace & wpCenter, const WorldDistance wdRadius, const WorldDistance wdHeight, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags)
{






    XPooledDataIDList listACDsSpheres(MemoryPoolsGetDATAIDListPool());


    float flSphereCheckRadius = Math::Sqrt(wdRadius * wdRadius + wdHeight * wdHeight);
    ACDsIntersectSphere(wpCenter, flSphereCheckRadius, idACDIgnore, listACDsSpheres, dwSphereTreeFlags);






    AxialCylinder tCylinder;
    tCylinder.wdRadius = wdRadius; tCylinder.wdHeight = wdHeight;
    tCylinder.wpBase = wpCenter.wp;


    POS pos = listACDsSpheres.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listACDsSpheres.GetNext(pos);

        ActorCommonData & tACD = ACDGet(idACD);
        if (Intersect::AxialCylinderAndAxialCylinder(tACD.GetWorldLocation().wcylBounds, tCylinder))
        {
            listACDs.AddTail_Tracked(idACD);
        }
    }
}



static void sACDsIntersectLineSeg(ActorSphereTree * ptActorSphereTree, const LineSeg3D & wline, XPooledDataIDList & listACDs)
{



    if (!ptActorSphereTree) return;

    XSphereArray * ptSphereTree = &ptActorSphereTree->tSphereTree;

    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());

    ptSphereTree->LineSegCull(wline, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        ACDID idACD = (ACDID)(intptr_t)ptSphereNode->GetUserData();

        ActorCommonData & tACD = ACDGet(idACD);
        const WorldLocation & tLocation = tACD.GetWorldLocation();

        WorldDistance wdT;
        BOOL bIntersect = Intersect::LineSeg3DAndAxialCylinder(wline, tLocation.wcylBounds, wdT);

        if (bIntersect)
        {
            listACDs.AddTail_Tracked(idACD);
        }
    }
}




void ACDsIntersectLineSeg(const WorldPlace & placeStart, const WorldPlace & placeEnd, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags)
{






    WorldCommonData * ptWCD = WorldTryGetCommonData(placeStart.idSWorld);
    if (!ptWCD) return;

    LineSeg3D wline;
    wline.Set(placeStart.wp, placeEnd.wp);

    for (int32 t = 0; t < ACTOR_SPHERE_TREE_NUM_TYPES; t++)
    {
        if (dwSphereTreeFlags & (1 << t))
        {
            sACDsIntersectLineSeg(ActorCollisionGetSphereTree(*ptWCD, (ActorSphereTreeTypes)t), wline, listACDs);
        }
    }
}






static void sACDsIntersectCapsule(ActorSphereTree * ptActorSphereTree, const Capsule & capsule, XPooledDataIDList & listACDs)
{



    if (!ptActorSphereTree) return;

    XSphereArray * ptSphereTree = &ptActorSphereTree->tSphereTree;
    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());

    ptSphereTree->CapsuleCull(capsule, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        ACDID idACD = (ACDID)(intptr_t)ptSphereNode->GetUserData();
        ActorCommonData & tACD = ACDGet(idACD);
        const WorldLocation & tLocation = tACD.GetWorldLocation();

        WorldDistance wdT;
        BOOL bIntersect = Intersect::CapsuleAndAxialCylinder(capsule, tLocation.wcylBounds, wdT);

        if (bIntersect)
        {
            listACDs.AddTail_Tracked(idACD);
        }
    }

}



void ACDsIntersectCapsule(SWorldID idSWorld, const Capsule & capsule, XPooledDataIDList & listACDs, BOOL bForPower)
{




    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);
    if (ptWCD)
    {
        sACDsIntersectCapsule(ptWCD->ptActorSphereTreeNormal, capsule, listACDs);
        sACDsIntersectCapsule(ptWCD->ptActorSphereTreeFastMover, capsule, listACDs);

        if (!bForPower)
        {
            sACDsIntersectCapsule(ptWCD->ptActorSphereTreeItemsInWorld, capsule, listACDs);
        }
    }
}



static void sActorsIntersectFrustum(ActorSphereTree * ptActorSphereTree, const Frustum & tFrustum, XPooledDataIDList & listACDs, XBitArray<11776> * ptActorsInListBitArray)
{




    if (!ptActorSphereTree) return;

    XSphereArray * ptSphereTree = &ptActorSphereTree->tSphereTree;
    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());

    ptSphereTree->FrustumCull(tFrustum, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        ACDID idACD = (ACDID)(intptr_t)ptSphereNode->GetUserData();

        if (ptActorsInListBitArray)
        {

            uint16 uIndex = (uint16)idACD;
            if (ptActorsInListBitArray->Get(uIndex))
            {
                continue;
            }
            ptActorsInListBitArray->Set(uIndex, TRUE);
        }

        listACDs.AddTail_Tracked(idACD);
    }
}




void ActorsIntersectFrustum(SWorldID idSWorld, const Frustum & tFrustum, XPooledDataIDList & listACDs, XBitArray<11776> * ptActorsInListBitArray, uint32 dwSphereTreeFlags)
{





    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);
    if (!ptWCD) return;

    for (int32 t = 0; t < ACTOR_SPHERE_TREE_NUM_TYPES; t++)
    {
        if (dwSphereTreeFlags & (1 << t))
        {



            sActorsIntersectFrustum(ActorCollisionGetSphereTree(*ptWCD, (ActorSphereTreeTypes)t), tFrustum, listACDs, ptActorsInListBitArray);
        }
    }
}




void GetACDsInRadius(const ActorCommonData & tACD, const float flRadius, XPooledDataIDList & listACDs)
{

    ACDsIntersectSphere(tACD.GetPlace(), flRadius, tACD.id, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
}
