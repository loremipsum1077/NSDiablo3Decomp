// Original file: X/Common/ActorCommonCollision.cpp
// Functions sit on their original lines (see the reference .cpp). The numeric masks in
// sCheckCollParams and the fSearchOnlyInBlockers tests are
// literals: no enumerator for those object-flag bits is known here.
#include <float.h>
#include "ActorCommonCollision.h"
#include "ActorCommonData.h"
#include "Game.h"
#include "GameCommonData.h"
#include "World.h"
#include "GlobalSNO.h"
#include "TeamCommon.h"
#include "ThreadGlobals.h"
#include "WorldPlace.h"
#include "../Sigma/MemoryPools.h"
#include "Definitions/ActorDefinition.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/DataStructures/XSphereArray.h"
#include "../Sigma/Math/Cylinder.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/Math/Ray.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Nav/NavZoneIntersectPrim.h"
struct ACDCollThreadGlobals { ACDCollInfo arCollBroadResults[8192]; };
struct ACDCollGlobals { XChainedFixedBlockAllocator<XSphereArray> tSphereTreeAlloc; };
struct Circle;
BOOL ACDCollBroadCircle(SWorldID idSWorld, const Vector2D & wpCenter, float flRadius, const ACDCollParams & tCollParams, BOOL bAllowNonWorldACDs, ACDCollBroadResult & tResult, ACDID idOwner);  // line 396, not written yet
BOOL ACDCollNarrowCircle(const ACDCollBroadResult & tCollBroadResult, const Circle & wc, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // line 590
BOOL ACDCollNarrowOBB(const ACDCollBroadResult & tCollBroadResult, const OBB & tOBB, ACDID idACDIgnore, ACDCollNarrowResult & tResult);  // line 675



ACDCollParams::ACDCollParams()
{
    uObjectNoOverlapFlags = 0;
    eTeam = 1;
    uSpecialFlags = 0;
}








void ACDCollInitializeForGame()

{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ACDCollGlobals *& ptACDCollGlobals = GameGetActiveGameCommonData().ptACDCollGlobals;
    ptACDCollGlobals = (ACDCollGlobals *)SigmaMemoryNew(sizeof(ACDCollGlobals), 4, ptMemoryInterface, TRUE);

    ptACDCollGlobals->tSphereTreeAlloc.Initialize();
    ptACDCollGlobals->tSphereTreeAlloc.SetMemoryInterface(ptMemoryInterface);
    ptACDCollGlobals->tSphereTreeAlloc.SetBlockCount_Tracked(8);
}


void ACDCollDisposeForGame()

{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ACDCollGlobals *& ptACDCollGlobals = GameGetActiveGameCommonData().ptACDCollGlobals;
    ptACDCollGlobals->tSphereTreeAlloc.Dispose();
    SigmaMemoryFree(ptACDCollGlobals, ptMemoryInterface); ptACDCollGlobals = NULL;
}


static XSphereArray * sAllocSphereTree()
{

    ACDCollGlobals * ptACDCollGlobals = GameGetActiveGameCommonData().ptACDCollGlobals;

    XSphereArray * ptACDCollSphereTree = ptACDCollGlobals->tSphereTreeAlloc.Alloc_Tracked();
    ptACDCollSphereTree->SetMemoryInterface(AppGetCurrentMemoryInterface());
    return ptACDCollSphereTree;
}


static void sFreeSphereTree(XSphereArray * ptSphereTree)

{
    ACDCollGlobals * ptACDCollGlobals = GameGetActiveGameCommonData().ptACDCollGlobals;

    ptACDCollGlobals->tSphereTreeAlloc.Free(ptSphereTree);
}


void ACDCollInitializeForWorld(WorldCommonData & tWCD)

{
    if (!tWCD.ptACDCollSphereTree)
    {
        tWCD.ptACDCollSphereTree = sAllocSphereTree();
        tWCD.ptACDCollSphereTree->Init("ActorCommonCollision", 512, 0.0f);
    }

    if (!tWCD.ptACDBlockerCollSphereTree)
    {
        tWCD.ptACDBlockerCollSphereTree = sAllocSphereTree();
        tWCD.ptACDBlockerCollSphereTree->Init("ActorCommonBlockerCollision", 128, 0.0f);
    }
}


void ACDCollDisposeForWorld(WorldCommonData & tWCD)

{
    if (tWCD.ptACDCollSphereTree)
    {
        tWCD.ptACDCollSphereTree->Uninit();
        sFreeSphereTree(tWCD.ptACDCollSphereTree);
        tWCD.ptACDCollSphereTree = NULL;
    }

    if (tWCD.ptACDBlockerCollSphereTree)
    {
        tWCD.ptACDBlockerCollSphereTree->Uninit();
        sFreeSphereTree(tWCD.ptACDBlockerCollSphereTree);
        tWCD.ptACDBlockerCollSphereTree = NULL;
    }
}


static void sGetSphereForACD(ActorCommonData & tACD, Sphere & ws)
{


    ws.wdRadius = 0.0f;
    ws.wpCenter = cgwpNull;

    switch (tACD.tCollSettings.eCollisionShape)
    {
    case 1:  // cylinder (CollisionShapeType enumerator name not used here)
        {
            const AxialCylinder & tCyl = tACD.GetCollSettings().GetCyl();
            ws.wpCenter = tCyl.wpBase;
            ws.wdRadius = tCyl.wdRadius;
        }
        break;
    case 2:  // OBB
        {
            const OBB & tObb = tACD.GetCollSettings().GetOBB();

            ws.wpCenter = tObb.transform.wp;
            ws.wdRadius = tObb.wvExt.As2D().Magnitude();
        }
        break;
    }

    ws.wpCenter.z = 0.0f;
}


ACDCollThreadGlobals * ACDCollThreadGlobalsGet()
{
    ACDCollThreadGlobals *& ptACDCollThreadGlobals = ThreadGlobals::Get().m_ptACDCollThreadGlobals;


    if (!ptACDCollThreadGlobals)
    {
        ptACDCollThreadGlobals = (ACDCollThreadGlobals *)SigmaMemoryNew(sizeof(ACDCollThreadGlobals), 4, NULL, TRUE);
    }

    return ptACDCollThreadGlobals;
}


void ACDCollDisposeSphereNode(ActorCommonData & tACD)

{
    if (tACD.ptCollSphereNode)
    {
        SWorldID idSWorld = tACD.idCollSWorld;
        WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);

        if (ptWCD)
        {
            ptWCD->ptACDCollSphereTree->RemoveSphereNode(tACD.ptCollSphereNode);
        }
    }

    tACD.ptCollSphereNode = NULL;

    if (tACD.ptCollBlockerSphereNode)
    {
        SWorldID idSWorld = tACD.idCollSWorld;
        WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);

        if (ptWCD)
        {
            ptWCD->ptACDBlockerCollSphereTree->RemoveSphereNode(tACD.ptCollBlockerSphereNode);
        }
    }

    tACD.ptCollBlockerSphereNode = NULL;

    tACD.idCollSWorld = SWORLDID_INVALID;
}









void ACDCollUpdateSphereNode(ActorCommonData & tACD, const WorldLocation & tWorldLocation)


{
    Sphere ws;
    sGetSphereForACD(tACD, ws);

    SWorldID idSWorld = tWorldLocation.idSWorld;
    if (idSWorld != tACD.idCollSWorld)
    {
        ACDCollDisposeSphereNode(tACD);
    }

    WorldCommonData * ptWCD = WorldTryGetCommonData(idSWorld);
    if (ptWCD)
    {
        XSphereArray * ptSphereTree = ptWCD->ptACDCollSphereTree;
        if (!tACD.ptCollSphereNode)
        {
            if (ptSphereTree->IsSphereValid(ws))
            {














                tACD.ptCollSphereNode = ptSphereTree->AddSphereNode(ws, (void *)(intptr_t)tACD.id);
            }

            tACD.idCollSWorld = idSWorld;
        }
        else
        {
            ptSphereTree->ModifySphereNode(tACD.ptCollSphereNode, ws);
        }

        if (tACD.tCollSettings.uDynamicMyFlags & 0xc20)
        {
            ptSphereTree = ptWCD->ptACDBlockerCollSphereTree;
            if (!tACD.ptCollBlockerSphereNode)
            {
                if (ptSphereTree->IsSphereValid(ws))
                {














                    tACD.ptCollBlockerSphereNode = ptSphereTree->AddSphereNode(ws, (void *)(intptr_t)tACD.id);
                }

                tACD.idCollSWorld = idSWorld;
            }
            else
            {
                ptSphereTree->ModifySphereNode(tACD.ptCollBlockerSphereNode, ws);
            }
        }
    }
}


void ACDCollGetSettings(SNO snoActor, ACDCollSettings & tCollSettings)


{
    ScopedActorDefinitionLockR ptActorDef(snoActor);

    tCollSettings.uStaticOneRequiredFlags = ptActorDef->tCollData.tCollFlags.uStaticOneRequiredFlags;
    tCollSettings.uStaticPreventFlags = ptActorDef->tCollData.tCollFlags.uStaticPreventFlags;
    tCollSettings.uDynamicFlagsNoOverlap = ptActorDef->tCollData.tCollFlags.uDynamicFlagsNoOverlap;
    tCollSettings.uDynamicMyFlags = ptActorDef->tCollData.tCollFlags.uDynamicMyFlags;
    tCollSettings.eCollisionShape = ptActorDef->tCollData.eCollisionShape;
    tCollSettings.eTeam = 1;
}


static void sACDsIntersectSphere(XSphereArray * ptSphereTree, const WorldPlace tPlace, float flRadius, XPooledDataIDList & listACDs)
{




    Sphere tSphere;
    tSphere.wdRadius = flRadius;
    tSphere.wpCenter = tPlace.wp;

    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());
    ptSphereTree->SphereCull(tSphere, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        ACDID idACD = (ACDID)(size_t)ptSphereNode->GetUserData();
        listACDs.AddTail_Tracked(idACD);
    }
}


static void sACDsIntersectLine(XSphereArray * ptSphereTree, const WorldPlace tPlace, const Vector3D & wpDest, XPooledDataIDList & listACDs)
{




    LineSeg3D wline;

    wline.Set(tPlace.wp, wpDest);

    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());
    ptSphereTree->LineSegCull(wline, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        ACDID idACD = (ACDID)(size_t)ptSphereNode->GetUserData();
        listACDs.AddTail_Tracked(idACD);
    }
}



static BOOL sCheckCollParams(const ACDCollParams & tCollParams, const ACDCollSettings & tACDCollSettings)
{


    if (tACDCollSettings.eCollisionShape != 0)
    {
        uint32 uFlagsOverlap = tCollParams.uObjectNoOverlapFlags & tACDCollSettings.uDynamicMyFlags;

        if (uFlagsOverlap)
        {
            uint32 uTeamFlags = 0x108;




            if (tCollParams.uSpecialFlags & (1 << COLL_PARAMS_FROM_POWER))
            {
                uTeamFlags |= 0x2;
            }

            if (uFlagsOverlap & ~uTeamFlags)
            {
                return TRUE;
            }

            if (!TeamsAreFriendly(tCollParams.eTeam, tACDCollSettings.eTeam))
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}








BOOL ACDCollBroadCircle(SWorldID idSWorld, const Vector2D & wpCenter, float flRadius, const ACDCollParams & tCollParams, BOOL bAllowNonWorldACDs, ACDCollBroadResult & tResult, ACDID idOwner)
{







    ACDCollThreadGlobals * ptACDCollThreadGlobals = ACDCollThreadGlobalsGet();
    BOOL fSearchOnlyInBlockers = (tCollParams.uObjectNoOverlapFlags & ~0xc20) == 0;
    XSphereArray * ptSphereTree = fSearchOnlyInBlockers ? WorldGetCommonData(idSWorld).ptACDBlockerCollSphereTree : WorldGetCommonData(idSWorld).ptACDCollSphereTree;

    tResult.uCount = 0;
    tResult.arACDISect = NULL;

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    WorldPlace placeStart;

    placeStart.idSWorld = idSWorld;
    placeStart.wp.Set(wpCenter.x, wpCenter.y, 0.0f);

    sACDsIntersectSphere(ptSphereTree, placeStart, flRadius, listACDs);

    int32 nACDCollCount = 0;
    if (listACDs.Count() > 0)
    {
        tResult.arACDISect = ptACDCollThreadGlobals->arCollBroadResults;
    }
    POS pos = listACDs.GetHeadPosition();
    int nIndex = 0;

    while (pos)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {


            if (!bAllowNonWorldACDs && ptACD->GetLocationType() != 0)
            {
                continue;
            }


            if ((tCollParams.uSpecialFlags & (1 << COLL_PARAMS_IGNORE_WRECKABLES)) && ptACD->IsWreckableObject())
            {
                continue;
            }

            if ((tCollParams.uSpecialFlags & (1 << COLL_PARAMS_IGNORE_WALLER_WALLS)) && ptACD->snoActor == GlobalSNOGet(GLOBALSNO_MONSTER_WALLER_MODEL))
            {
                continue;
            }

            if (idOwner != ACDID_INVALID)
            {


                ActorCommonData * ptACDOwner = ACDTryToGet(idOwner);



                if (ptACDOwner && ptACD->GetAssignedHeroID() != OnlineService::INVALID_HERO_ID &&
                    ptACDOwner->GetAssignedHeroID() != ptACD->GetAssignedHeroID())
                {
                    continue;
                }
            }


            const ACDCollSettings & tACDCollSettings = ptACD->GetCollSettings();
            if (tResult.arACDISect && sCheckCollParams(tCollParams, tACDCollSettings))
            {
                tResult.arACDISect[nACDCollCount].idACD = idACD;
                tResult.arACDISect[nACDCollCount].tCollSettings = tACDCollSettings;
                nACDCollCount++;
            }
        }
    }

    tResult.uCount = nACDCollCount;



    return nACDCollCount != 0;
}


BOOL ACDCollBroadLine(SWorldID idSWorld,
                      const Vector2D & wp0,
                      const Vector2D & wp1,
                      const ACDCollParams & tCollParams,
                      ACDCollBroadResult & tResult)

{
    ACDCollThreadGlobals * ptACDCollThreadGlobals = ACDCollThreadGlobalsGet();
    tResult.uCount = 0;
    tResult.arACDISect = NULL;

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    WorldPlace placeStart;
    Vector3D wpDest;

    placeStart.idSWorld = idSWorld;
    placeStart.wp.Set(wp0.x, wp0.y, 0.0f);

    wpDest.Set(wp1.x, wp1.y, 0.0f);

    BOOL fSearchOnlyInBlockers = (tCollParams.uObjectNoOverlapFlags & ~0xc20) == 0;
    XSphereArray * ptSphereTree = fSearchOnlyInBlockers ? WorldGetCommonData(idSWorld).ptACDBlockerCollSphereTree : WorldGetCommonData(idSWorld).ptACDCollSphereTree;
    sACDsIntersectLine(ptSphereTree, placeStart, wpDest, listACDs);

    int32 nACDCollCount = 0;
    if (listACDs.Count() > 0)
    {
        tResult.arACDISect = ptACDCollThreadGlobals->arCollBroadResults;
    }
    POS pos = listACDs.GetHeadPosition();
    int nIndex = 0;
    float flT;
    LineSeg2D wlineseg;

    wlineseg.Set(wp0, wp1);

    while (pos)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);
        ActorCommonData & tACD = ACDGet(idACD);
        const ACDCollSettings & tACDCollSettings = tACD.GetCollSettings();

        if ((tCollParams.uSpecialFlags & (1 << COLL_PARAMS_IGNORE_WRECKABLES)) && tACD.IsWreckableObject())
        {
            continue;
        }

        if ((tCollParams.uSpecialFlags & (1 << COLL_PARAMS_IGNORE_WALLER_WALLS)) && tACD.snoActor == GlobalSNOGet(GLOBALSNO_MONSTER_WALLER_MODEL))
        {
            continue;
        }

        if (sCheckCollParams(tCollParams, tACDCollSettings))
        {

            BOOL bLineCheck = FALSE;

            switch (tACDCollSettings.eCollisionShape)
            {
            case 1:  // cylinder
                {
                    Circle wc;
                    const AxialCylinder & tCyl = tACDCollSettings.GetCyl();
                    wc.wdRadius = tCyl.wdRadius;
                    wc.wpCenter = tCyl.wpBase.As2D();


                    bLineCheck = Intersect::LineSeg2DAndCircle(wlineseg, wc, flT, NULL);
                }
                break;

            case 2:  // OBB
                {


                    bLineCheck = Intersect::LineSeg2DAndOBB(wlineseg, tACDCollSettings.GetOBB(), flT, NULL);
                }
                break;
            }




            if (tResult.arACDISect && bLineCheck)
            {
                tResult.arACDISect[nACDCollCount].idACD = idACD;
                tResult.arACDISect[nACDCollCount].tCollSettings = tACDCollSettings;
                nACDCollCount++;
            }
        }
    }

    tResult.uCount = nACDCollCount;



    return nACDCollCount != 0;
}






BOOL ACDCollNarrowCircle(const ACDCollBroadResult & tCollBroadResult, const Circle & wc, ACDID idACDIgnore, ACDCollNarrowResult & tResult)




{
    BOOL bFoundCollision = FALSE;

    Sphere ws;

    ws.wdRadius = wc.wdRadius;
    tResult.nIndexHit = -1;
    tResult.flT = 0.0f;


    for (uint32 i = 0; i < tCollBroadResult.uCount; i++)
    {
        if (tCollBroadResult.arACDISect[i].idACD == idACDIgnore)
        {
            continue;
        }

        const ACDCollSettings & tACDCollSettings = tCollBroadResult.arACDISect[i].tCollSettings;

        switch (tACDCollSettings.eCollisionShape)
        {
        case 1:
            {
                const AxialCylinder & tCyl = tACDCollSettings.GetCyl();
                float flACDRadius = tCyl.wdRadius;
                float flRadius = flACDRadius + wc.wdRadius;
                float flRadiusSqr = flRadius * flRadius;

                Vector2D wp(tCyl.wpBase.x, tCyl.wpBase.y);
                float flDistSqr = Vector2D::DistanceSquared(wp, wc.wpCenter);

                if (flDistSqr < flRadiusSqr)
                {
                    bFoundCollision = TRUE;
                    tResult.nIndexHit = i;
                    tResult.wpPosition = tCyl.wpBase;
                    tResult.flDistance = Math::Sqrt(flDistSqr);
                    tResult.flACDRadius = flACDRadius;
                    tResult.wpContact.Set(wc.wpCenter.x, wc.wpCenter.y, tCyl.wpBase.z);
                    tResult.wvNormal = tResult.wpContact - tCyl.wpBase;
                    tResult.wvNormal.Normalize();
                }
            }
            break;
        case 2:
            {

                const OBB & tOBB = tACDCollSettings.GetOBB();

                Vector3D wpClosest;
                float flZ = tOBB.transform.wp.z - tOBB.wvExt.z;

                ws.wpCenter.Set(wc.wpCenter.x, wc.wpCenter.y, flZ);


                if (Intersect::SphereAndOBB(ws, tOBB, &wpClosest))
                {
                    bFoundCollision = TRUE;
                    tResult.nIndexHit = i;
                    tResult.wpPosition = tOBB.transform.wp;
                    tResult.flDistance = Vector3D::Distance2D(ws.wpCenter, tOBB.transform.wp);
                    tResult.flACDRadius = tOBB.wvExt.Magnitude();
                    tResult.wpContact.Set(wc.wpCenter.x, wc.wpCenter.y, flZ);
                    tResult.wvNormal = tResult.wpContact - wpClosest;
                    tResult.wvNormal.Normalize();
                }
            }
            break;
        }

        if (bFoundCollision)
        {
            break;
        }
    }

    return bFoundCollision;
}


BOOL ACDCollNarrowOBB(const ACDCollBroadResult & tCollBroadResult, const OBB & tOBB, ACDID idACDIgnore, ACDCollNarrowResult & tResult)




{
    BOOL bFoundCollision = FALSE;

    Sphere ws;

    ws.wpCenter.z = tOBB.transform.wp.z;

    tResult.nIndexHit = -1;
    tResult.flT = 0.0f;


    for (uint32 i = 0; i < tCollBroadResult.uCount; i++)
    {
        if (tCollBroadResult.arACDISect[i].idACD == idACDIgnore)
        {
            continue;
        }

        const ACDCollSettings & tACDCollSettings = tCollBroadResult.arACDISect[i].tCollSettings;

        Vector2D wp;

        switch (tACDCollSettings.eCollisionShape)
        {
        case 1:
            {
                const AxialCylinder & tCyl = tACDCollSettings.GetCyl();
                float flACDRadius = tCyl.wdRadius;

                ws.wdRadius = flACDRadius;
                ws.wpCenter.x = tCyl.wpBase.x;
                ws.wpCenter.y = tCyl.wpBase.y;

                if (Intersect::SphereAndOBB(ws, tOBB, NULL))
                {
                    bFoundCollision = TRUE;
                    tResult.nIndexHit = i;
                    tResult.wpPosition = tCyl.wpBase;
                    tResult.flDistance = Vector3D::Distance2D(tOBB.transform.wp, tCyl.wpBase);
                    tResult.flACDRadius = flACDRadius;
                    tResult.wpContact = tCyl.wpBase;
                    tResult.wvNormal = tOBB.transform.wp - tCyl.wpBase;
                    tResult.wvNormal.Normalize();
                }
            }
            break;
        case 2:
            {







                const OBB & tOBBAcd = tACDCollSettings.GetOBB();

                Vector3D wpA, wpB, wpC, wpD;
                LineSeg2D wlineA, wlineB, wlineC, wlineD;
                float flTA, flTB, flTC, flTD;

                wpA = tOBB.transform.wp + tOBB.wvExt;
                wpB = wpA;
                wpB.y = -wpB.y;
                wpC = tOBB.transform.wp - tOBB.wvExt;
                wpD = wpC;
                wpD.x = -wpD.x;

                wlineA.Set(wpA, wpB);
                wlineB.Set(wpB, wpC);
                wlineC.Set(wpC, wpD);
                wlineD.Set(wpD, wpA);

                if (Intersect::LineSeg2DAndOBB(wlineA, tOBBAcd, flTA, NULL) ||
                    Intersect::LineSeg2DAndOBB(wlineB, tOBBAcd, flTB, NULL) ||
                    Intersect::LineSeg2DAndOBB(wlineC, tOBBAcd, flTC, NULL) ||
                    Intersect::LineSeg2DAndOBB(wlineD, tOBBAcd, flTD, NULL))
                {
                    bFoundCollision = TRUE;
                    tResult.nIndexHit = i;
                    tResult.wpPosition = tOBBAcd.transform.wp;
                    tResult.flDistance = Vector3D::Distance2D(tOBB.transform.wp, tOBBAcd.transform.wp);
                    tResult.flACDRadius = tOBBAcd.wvExt.Magnitude();
                    tResult.wpContact = tOBBAcd.transform.wp;
                    tResult.wvNormal = tOBB.transform.wp - tOBBAcd.transform.wp;
                    tResult.wvNormal.Normalize();
                }
            }
            break;
        }

        if (bFoundCollision)
        {
            break;
        }
    }

    return bFoundCollision;
}
const Vector2D & AABBGetNormal(AABBCollideFeature eCollFeature);  // X/Common/Nav/NavZoneIntersectPrim.cpp:73
AABBCollideFeature Ray2DAndAABB2D(const Vector2D wpStart, const Vector2D wvDir, const Vector2D wpAABBMin, const Vector2D wpAABBMax, float & flT, Vector2D & wpIsect);  // NavZoneIntersectPrim.cpp:176
BOOL ACDCollNarrowSweptPoint(const ACDCollBroadResult & tCollBroadResult,
                             const Vector2D & wp0,
                             const Vector2D & wp1,
                             uint32 uFlags,
                             ACDID idACDIgnore,
                             ACDCollNarrowResult & tResult)
{
    BOOL bFoundCollision = FALSE;
    Vector2D wvDirection = wp1 - wp0;
    Vector2D wpEnd;
    float flT;
    LineSeg2D wlineseg;

    wlineseg.Set(wp0, wp1);

    tResult.nIndexHit = -1;
    tResult.flT = FLT_MAX;

    BOOL bIgnoreCollisionsIfMovingAway = (uFlags & 1);



    for (uint32 i = 0; i < tCollBroadResult.uCount; i++)
    {
        if (tCollBroadResult.arACDISect[i].idACD == idACDIgnore)
        {
            continue;
        }

        const ACDCollSettings & tACDCollSettings = tCollBroadResult.arACDISect[i].tCollSettings;

        Vector2D wp;

        switch (tACDCollSettings.eCollisionShape)
        {
        case 1:
            {
                const AxialCylinder & tCyl = tACDCollSettings.GetCyl();
                float flACDRadius = tCyl.wdRadius;

                Circle wc;

                wc.wpCenter = tCyl.wpBase.As2D();
                wc.wdRadius = flACDRadius;

                flT = FLT_MAX;
                BOOL bCollided = FALSE;
                if (wvDirection.MagnitudeSquared() >= EPSILON)
                {
                    if (Intersect::LineSeg2DAndCircle(wlineseg, wc, flT, &wpEnd))
                    {
                        bCollided = TRUE;
                        flT /= wlineseg.wdLength;
                    }
                }
                else
                {

                    if (wc.ContainsPoint(wp0))
                    {
                        flT = 0.0f;
                        wpEnd = wp0;
                        bCollided = TRUE;
                    }
                }

                if (bCollided)
                {
                    BOOL bIgnoreCollision = FALSE;

                    if (bIgnoreCollisionsIfMovingAway && flT < EPSILON)
                    {


                        Vector2D vStartToAcd = wc.wpCenter - wp0;
                        float flDot = Vector2D::DotProduct(wvDirection, vStartToAcd);
                        if (flDot < EPSILON)
                        {
                            bIgnoreCollision = TRUE;
                        }
                    }


                    if (!bIgnoreCollision && flT < tResult.flT)
                    {
                        bFoundCollision = TRUE;
                        tResult.flT = flT;
                        tResult.wpPosition.Set(wpEnd.x, wpEnd.y, tCyl.wpBase.z);
                        tResult.flDistance = 0.0f;
                        tResult.nIndexHit = i;
                        tResult.flACDRadius = flACDRadius;
                        tResult.wvNormal = tResult.wpPosition - tCyl.wpBase;
                        tResult.wvNormal.Normalize();
                        tResult.wpContact = tResult.wpPosition;
                    }
                }
            }
            break;

        case 2:
            {
                const OBB & tOBB = tACDCollSettings.GetOBB();

                PRTransform transOBB;
                transOBB = tOBB.transform;
                transOBB.wp.x = 0.0f;
                transOBB.wp.y = 0.0f;
                PRTransform transformInv = transOBB.Inverse();
                Vector2D wpStart, wvDir, wpAABBMin, wpAABBMax;

                wpStart = wp0 - tOBB.transform.wp.As2D();
                wpStart = transformInv * wpStart;


                wvDir = transformInv.Rotate(wvDirection);
                wpAABBMin.Set(-tOBB.wvExt.x, -tOBB.wvExt.y);
                wpAABBMax.Set(tOBB.wvExt.x, tOBB.wvExt.y);

                if (wvDirection.MagnitudeSquared() < EPSILON)
                {
                    if (!bIgnoreCollisionsIfMovingAway && tResult.flT > 0.0f && wpStart.x >= wpAABBMin.x && wpStart.x <= wpAABBMax.x && wpStart.y >= wpAABBMin.y && wpStart.y <= wpAABBMax.y)


                    {
                        bFoundCollision = TRUE;
                        tResult.flT = 0.0f;
                        tResult.wpPosition.Set(wp0.x, wp0.y, tOBB.transform.wp.z - tOBB.wvExt.z);
                        tResult.flDistance = 0.0f;
                        tResult.nIndexHit = i;
                        tResult.flACDRadius = tOBB.wvExt.Magnitude();
                        tResult.wvNormal = tResult.wpPosition - tOBB.transform.wp;
                        if (tResult.wvNormal.MagnitudeSquared() > EPSILON)
                        {
                            tResult.wvNormal.Normalize();
                        }
                        else
                        {
                            tResult.wvNormal = cgwvXAxis;
                        }
                        tResult.wpContact = tResult.wpPosition;
                    }
                }
                else
                {

                    AABBCollideFeature eCollFeature = Ray2DAndAABB2D(wpStart, wvDir, wpAABBMin, wpAABBMax, flT, wpEnd);

                    if (eCollFeature != AABB_COLLIDE_NONE && flT < 1.000001f)
                    {
                        BOOL bIgnoreCollision = FALSE;

                        Vector2D wvNormal = AABBGetNormal(eCollFeature);
                        if (eCollFeature == AABB_COLLIDE_VERTEX)
                        {
                            wvNormal = wpEnd;
                            wvNormal.Normalize();
                        }


                        wvNormal = tOBB.transform.Rotate(wvNormal);

                        if (bIgnoreCollisionsIfMovingAway && flT < EPSILON)
                        {


                            Vector2D vStartToAcd;  // line 946 (DWARF); nothing of it survives
                            float flDot = Vector2D::DotProduct(wvDirection, wvNormal);

                            if (flDot >= -EPSILON)
                            {
                                bIgnoreCollision = TRUE;
                            }
                        }

                        if (!bIgnoreCollision && flT < tResult.flT)
                        {
                            float flZ = tOBB.transform.wp.z - tOBB.wvExt.z;

                            Vector2D wpEndWorld = transOBB * wpEnd;
                            wpEndWorld += tOBB.transform.wp.As2D();

                            bFoundCollision = TRUE;
                            tResult.flT = flT;
                            tResult.wpPosition.Set(wpEndWorld.x, wpEndWorld.y, flZ);
                            tResult.flDistance = 0.0f;
                            tResult.nIndexHit = i;
                            tResult.flACDRadius = tOBB.wvExt.Magnitude();
                            tResult.wpContact = tResult.wpPosition;
                            tResult.wvNormal.Set(wvNormal.x, wvNormal.y, 0.0f);
                        }
                    }
                }
            }
            break;
        }
    }
    return bFoundCollision;
}

AABBCollideFeature SweptCircleAndAABB2D(const Circle & wc, const Vector2D & wvDir, const float flDirMag, const Vector2D & wvDirNormalized, const Vector2D & wpMin, const Vector2D & wpMax, float & flT, Vector2D & wpCollision);  // NavZoneIntersectPrim.cpp:271
AABBCollideFeature CircleAndAABB2D(const Circle & wc, const Vector2D & wpMin, const Vector2D & wpMax, float & flSqrDistance, Vector2D & wpCollision);  // NavZoneIntersectPrim.cpp:359
BOOL ACDCollNarrowSweptCircle(const ACDCollBroadResult & tCollBroadResult,
                              const Circle & wcStart,
                              const Vector2D & wpDest,
                              uint32 uFlags,
                              ACDID idACDIgnore,
                              ACDCollNarrowResult & tResult)
{
    BOOL bFoundCollision = FALSE;
    float flT, flDirMagnitude;
    Vector2D wpEnd, wvDirection, wvDirectionNormalized;
    LineSeg2D wlineseg;

    wlineseg.Set(wcStart.wpCenter, wpDest);

    wvDirection = wpDest - wcStart.wpCenter;
    wvDirectionNormalized = wvDirection;
    flDirMagnitude = wvDirectionNormalized.Normalize();

    tResult.nIndexHit = -1;
    tResult.flT = FLT_MAX;

    BOOL bIgnoreCollisionsIfMovingAway = (uFlags & 1);



    for (uint32 i = 0; i < tCollBroadResult.uCount; i++)
    {
        if (tCollBroadResult.arACDISect[i].idACD == idACDIgnore)
        {
            continue;
        }

        const ACDCollSettings & tACDCollSettings = tCollBroadResult.arACDISect[i].tCollSettings;

        Vector2D wp;

        switch (tACDCollSettings.eCollisionShape)
        {
        case 1:
            {
                const AxialCylinder & tCyl = tACDCollSettings.GetCyl();
                float flACDRadius = tCyl.wdRadius;

                Circle wc;

                wc.wpCenter = tCyl.wpBase.As2D();
                wc.wdRadius = flACDRadius + wcStart.wdRadius;

                flT = FLT_MAX;
                BOOL bCollided = FALSE;
                if (wvDirection.MagnitudeSquared() >= EPSILON)
                {
                    if (Intersect::LineSeg2DAndCircle(wlineseg, wc, flT, &wpEnd))
                    {
                        bCollided = TRUE;
                        flT /= wlineseg.wdLength;
                    }
                }
                else
                {

                    if (wc.ContainsPoint(wcStart.wpCenter))
                    {
                        flT = 0.0f;
                        wpEnd = wcStart.wpCenter;
                        bCollided = TRUE;
                    }
                }

                if (bCollided)
                {
                    BOOL bIgnoreCollision = FALSE;

                    if (bIgnoreCollisionsIfMovingAway)
                    {


                        Vector2D vStartToAcd = wc.wpCenter - wcStart.wpCenter;
                        float flDot = Vector2D::DotProduct(wvDirection, vStartToAcd);
                        if (flDot < EPSILON)
                        {
                            bIgnoreCollision = TRUE;
                        }
                    }


                    if (!bIgnoreCollision && flT < tResult.flT)
                    {
                        bFoundCollision = TRUE;
                        tResult.flT = flT;
                        tResult.wpPosition.Set(wpEnd.x, wpEnd.y, tCyl.wpBase.z);
                        tResult.flDistance = 0.0f;
                        tResult.nIndexHit = i;
                        tResult.flACDRadius = flACDRadius;
                        tResult.wvNormal = tResult.wpPosition - tCyl.wpBase;
                        tResult.wvNormal.Normalize();
                        tResult.wpContact = tResult.wpPosition - tResult.wvNormal * wcStart.wdRadius;
                    }
                }
            }
            break;

        case 2:
            {
                const OBB & tOBB = tACDCollSettings.GetOBB();





                PRTransform transOBB;
                transOBB = tOBB.transform;
                transOBB.wp.x = 0.0f;
                transOBB.wp.y = 0.0f;
                PRTransform invTrans = transOBB.Inverse();
                Vector2D wpCollision, wpMin, wpMax;
                Vector2D wvDir, wvDirNrm;
                Circle wc;
                AABBCollideFeature eCollFeature;

                wc.wdRadius = wcStart.wdRadius;
                wc.wpCenter = wcStart.wpCenter - tOBB.transform.wp.As2D();
                wc.wpCenter = invTrans * wc.wpCenter;

                wvDir = invTrans.Rotate(wvDirection);
                wvDirNrm = invTrans.Rotate(wvDirectionNormalized);

                wpMin.Set(-tOBB.wvExt.x, -tOBB.wvExt.y);
                wpMax.Set(tOBB.wvExt.x, tOBB.wvExt.y);

                if (wvDirection.MagnitudeSquared() < EPSILON)
                {
                    float flSqrDistance = 0.0f;
                    if (!bIgnoreCollisionsIfMovingAway && tResult.flT > 0.0f &&

                        (eCollFeature = CircleAndAABB2D(wc, wpMin, wpMax, flSqrDistance, wpCollision)) != AABB_COLLIDE_NONE)
                    {
                        bFoundCollision = TRUE;
                        tResult.flT = 0.0f;
                        tResult.wpPosition.Set(wpCollision.x, wpCollision.y, tOBB.transform.wp.z - tOBB.wvExt.z);
                        tResult.flDistance = 0.0f;
                        tResult.nIndexHit = i;
                        tResult.flACDRadius = tOBB.wvExt.Magnitude();
                        tResult.wvNormal = tResult.wpPosition - tOBB.transform.wp;
                        if (tResult.wvNormal.MagnitudeSquared() > EPSILON)
                        {
                            tResult.wvNormal.Normalize();
                        }
                        else
                        {
                            tResult.wvNormal = cgwvXAxis;
                        }
                        tResult.wpContact = tResult.wpPosition;
                    }
                }
                else
                {

                    eCollFeature = SweptCircleAndAABB2D(wc, wvDir, flDirMagnitude, wvDirNrm, wpMin, wpMax, flT, wpCollision);

                    if (eCollFeature != AABB_COLLIDE_NONE)
                    {
                        BOOL bIgnoreCollision = FALSE;

                        Vector2D wvNormal;

                        if (eCollFeature == AABB_COLLIDE_VERTEX)
                        {
                            Vector2D wpFinish = wc.wpCenter + wvDir * flT;
                            wvNormal = wpFinish - wpCollision;
                            wvNormal.Normalize();
                        }
                        else
                        {
                            wvNormal = AABBGetNormal(eCollFeature);
                        }


                        wvNormal = tOBB.transform.Rotate(wvNormal);

                        if (bIgnoreCollisionsIfMovingAway && flT < EPSILON)
                        {


                            Vector2D vStartToAcd;  // line 1166 (DWARF); nothing of it survives
                            float flDot = Vector2D::DotProduct(wvDirection, wvNormal);

                            if (flDot >= -EPSILON)
                            {
                                bIgnoreCollision = TRUE;
                            }
                        }

                        if (!bIgnoreCollision && flT < tResult.flT)
                        {
                            float flZ = tOBB.transform.wp.z - tOBB.wvExt.z;
                            wpEnd = wcStart.wpCenter + wvDirection * flT;

                            bFoundCollision = TRUE;
                            tResult.flT = flT;
                            tResult.wpPosition.Set(wpEnd.x, wpEnd.y, flZ);
                            tResult.flDistance = 0.0f;
                            tResult.nIndexHit = i;
                            tResult.flACDRadius = tOBB.wvExt.Magnitude();
                            tResult.wvNormal.Set(wvNormal.x, wvNormal.y, 0.0f);

                            Vector2D wpContact = transOBB * wpCollision;
                            wpContact += tOBB.transform.wp.As2D();
                            tResult.wpContact.Set(wpContact.x, wpContact.y, flZ);
                        }
                    }
                }
            }
            break;
        }
    }

    return bFoundCollision;
}






BOOL ACDCollCollideCircle(SWorldID idSWorld, const Circle & wc, const ACDCollParams & tCollParams, ACDID idACDIgnore, ACDCollNarrowResult & tResult)





{
    ACDCollBroadResult tBroadRes;

    if (ACDCollBroadCircle(idSWorld, wc.wpCenter, wc.wdRadius, tCollParams, FALSE, tBroadRes, ACDID_INVALID))
    {
        if (ACDCollNarrowCircle(tBroadRes, wc, idACDIgnore, tResult))
        {
            return TRUE;
        }
    }

    return FALSE;
}


BOOL ACDCollCollideOBB(SWorldID idSWorld, const OBB & tOBB, const ACDCollParams & tCollParams, ACDID idACDIgnore, ACDCollNarrowResult & tResult)





{
    ACDCollBroadResult tBroadRes;

    Circle wc;
    wc.wpCenter.Set(tOBB.transform.wp.x, tOBB.transform.wp.y);
    wc.wdRadius = tOBB.wvExt.As2D().Magnitude();

    if (ACDCollBroadCircle(idSWorld, wc.wpCenter, wc.wdRadius, tCollParams, FALSE, tBroadRes, ACDID_INVALID))
    {
        if (ACDCollNarrowOBB(tBroadRes, tOBB, idACDIgnore, tResult))
        {
            return TRUE;
        }
    }

    return FALSE;
}
