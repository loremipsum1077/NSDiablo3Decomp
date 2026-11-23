// X/Client/RActors/RActorCollision.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-render/coll/frag.cpp by build/try/r6-render/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "RActors.h"
#include "../CGame.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/DataStructures/XSphereArray.h"
#include "../../Sigma/DataStructures/XSphereNode.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "RActorAnimation.h"
#include "RActorAttachments.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Math/Frustum.h"
#include "RActorCollision.h"
#include "../SubObject.h"
#include "../../Sigma/Math/Intersect.h"
#include "../Gfx/GfxIB.h"
#include "../Animation.h"
#include <float.h>


















typedef XGrowableMap<const XSphereNode *, const XSphereNode *, int, int> XSphereActorMap;

struct RActorCollisionGlobals {
    XSphereArray * ptEnvAppSphereTree;  // 0x00
    XSphereArray * ptRActorSphereTree;  // 0x08
    XSphereActorMap * pmapRActorSphereActor;  // 0x10

    XSphereArray * ptCollisionSphereTree;  // 0x18
};

static CRefString RActorSphereTreeNodeName(const XSphereNode & tSphereNode);
static CRefString CollisionSphereTreeNodeName(const XSphereNode & tSphereNode);






















































void RActorsCollisionInitializeForGame(const int32 nMaxRActors)
{
    int32 nSphereCount = nMaxRActors * 4;

    RActorCollisionGlobals *& ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    ptCollisionGlobals = (RActorCollisionGlobals *)SigmaMemoryNew(sizeof(RActorCollisionGlobals), 4, NULL, TRUE);

    ptCollisionGlobals->ptRActorSphereTree = new XSphereArray(NULL);
    ptCollisionGlobals->ptRActorSphereTree->Init("RActorTree", nSphereCount, 0.5f);
    ptCollisionGlobals->ptRActorSphereTree->RegisterSphereNameFunc(RActorSphereTreeNodeName);
    ptCollisionGlobals->pmapRActorSphereActor = new XSphereActorMap(nSphereCount);

    ptCollisionGlobals->ptEnvAppSphereTree = new XSphereArray(NULL);
    ptCollisionGlobals->ptEnvAppSphereTree->Init("EnvAppTree", 128, 0.5f);
    ptCollisionGlobals->ptEnvAppSphereTree->RegisterSphereNameFunc(RActorSphereTreeNodeName);

    ptCollisionGlobals->ptCollisionSphereTree = new XSphereArray(NULL);
    ptCollisionGlobals->ptCollisionSphereTree->Init("CollisionSphere", nSphereCount, 0.5f);
    ptCollisionGlobals->ptCollisionSphereTree->RegisterSphereNameFunc(CollisionSphereTreeNodeName);
}







void RActorsCollisionDisposeForGame()
{
    RActorCollisionGlobals *& ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    ptCollisionGlobals->ptCollisionSphereTree->Uninit();
    delete ptCollisionGlobals->ptCollisionSphereTree;

    delete ptCollisionGlobals->pmapRActorSphereActor;
    ptCollisionGlobals->ptRActorSphereTree->Uninit();
    delete ptCollisionGlobals->ptRActorSphereTree;

    ptCollisionGlobals->ptEnvAppSphereTree->Uninit();
    delete ptCollisionGlobals->ptEnvAppSphereTree;

    SigmaMemoryFree(ptCollisionGlobals, NULL); ptCollisionGlobals = NULL;
}







void RActorsCollisionResize(const int32 nMaxRActors)
{
    RActorCollisionGlobals *& ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    ptCollisionGlobals->ptRActorSphereTree->Resize(nMaxRActors * 4);
    ptCollisionGlobals->ptCollisionSphereTree->Resize(nMaxRActors * 4);
}








void RActorsCollisionUpdate()
{
    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    ptCollisionGlobals->ptRActorSphereTree->Update();
    ptCollisionGlobals->ptEnvAppSphereTree->Update();
    ptCollisionGlobals->ptCollisionSphereTree->Update();
}






void RActorCollisionInitialize(RActor * ptRActor)
{
    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    if (loc.ptSphereNode == NULL)
    {
        if (ptCollisionGlobals->ptRActorSphereTree->IsSphereValid(loc.wsBounds))
        {





            loc.ptSphereNode = ptCollisionGlobals->ptRActorSphereTree->AddSphereNode(loc.wsBounds, NULL);

            loc.ptSphereNode->SetUserData((void *)(intptr_t)ptRActor->id);

            ptCollisionGlobals->pmapRActorSphereActor->SetAt_Tracked(loc.ptSphereNode, ptRActor->id);
        }
    }


    if (ptRActor->snoPhysMesh != SNO_NONE && ptRActor->ptEnvAppSphereNode == NULL)
    {
        if (ptCollisionGlobals->ptEnvAppSphereTree->IsSphereValid(loc.wsBounds))
        {





            ptRActor->ptEnvAppSphereNode = ptCollisionGlobals->ptEnvAppSphereTree->AddSphereNode(loc.wsBounds, NULL);

            ptRActor->ptEnvAppSphereNode->SetUserData((void *)(intptr_t)ptRActor->id);

            ptCollisionGlobals->pmapRActorSphereActor->SetAt_Tracked(ptRActor->ptEnvAppSphereNode, ptRActor->id);
        }
    }
}




void RActorCollisionDispose(RActor * ptRActor)
{
    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    if (ptRActor->tLocation.eActorLocation == 0 && ptRActor->tLocation.tWorldLocation.ptSphereNode)
    {
        WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

        ptCollisionGlobals->pmapRActorSphereActor->RemoveAt(loc.ptSphereNode);
        ptCollisionGlobals->ptRActorSphereTree->RemoveSphereNode(loc.ptSphereNode);
        loc.ptSphereNode = NULL;
    }

    if (ptRActor->ptEnvAppSphereNode)
    {
        ptCollisionGlobals->pmapRActorSphereActor->RemoveAt(ptRActor->ptEnvAppSphereNode);
        ptCollisionGlobals->ptEnvAppSphereTree->RemoveSphereNode(ptRActor->ptEnvAppSphereNode);
        ptRActor->ptEnvAppSphereNode = NULL;
    }
}






void RActorComputeBoneBoundingSphere(RActor * ptRActor)
{
    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

    PRSTransform transformWorld(loc.transform.q, loc.transform.wp, loc.flScale);

    const Vector3D & wpMax = ptRActor->ptActorAnim->wpMax;
    const Vector3D & wpMin = ptRActor->ptActorAnim->wpMin;
    Sphere wsLocal;
    wsLocal.wpCenter = 0.5f * (wpMin + wpMax);
    wsLocal.wdRadius = 0.5f * (wpMax - wpMin).Magnitude();

    loc.wsBounds.wpCenter = transformWorld * wsLocal.wpCenter;
    loc.wsBounds.wdRadius = wsLocal.wdRadius * loc.flScale;
}




void RActorUpdateSphereTreeNode(RActor * ptRActor, const Sphere & wsOld)
{
    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    if (loc.ptSphereNode == NULL)
        return;

    // only move the node when the bounds actually changed


    if (Math::Abs(wsOld.wpCenter.x - loc.wsBounds.wpCenter.x) < EPSILON &&
        Math::Abs(wsOld.wpCenter.y - loc.wsBounds.wpCenter.y) < EPSILON &&
        Math::Abs(wsOld.wpCenter.z - loc.wsBounds.wpCenter.z) < EPSILON &&
        Math::Abs(wsOld.wdRadius - loc.wsBounds.wdRadius) < EPSILON)
        return;







    ptCollisionGlobals->ptRActorSphereTree->ModifySphereNode(loc.ptSphereNode, loc.wsBounds);

    if (ptRActor->ptEnvAppSphereNode)
    {
        ptCollisionGlobals->ptEnvAppSphereTree->ModifySphereNode(ptRActor->ptEnvAppSphereNode, loc.wsBounds);
    }
}




















































































































































































































































// Forward declarations (inference: both helpers are defined below their only caller at 571/576).
static BOOL sRayIntersectStaticRActorExact(const Ray3D & wrRay, const RActor * ptRActor, const BOOL fFindClosest, const GeoSetType eGeoSetType, RActorExactIntersectionResult & tResult);
static BOOL sRayIntersectAnimatingRActorExact(const Ray3D & wrRay, const RActor * ptRActor, const BOOL fFindClosest, const GeoSetType eGeoSetType, RActorExactIntersectionResult & tResult);


BOOL RActorDoesRayIntersectExact(
    const RActor * ptRActor,
    const Ray3D & wrRay,
    const BOOL fFindClosest,
    const GeoSetType eGeoSetType,
    RActorExactIntersectionResult & tResult)
{
	BOOL fHit;

	if (!ptRActor->ptActorAnim)
	{
		fHit = sRayIntersectStaticRActorExact(wrRay, ptRActor, fFindClosest, eGeoSetType, tResult);
	}
	else
	{

		fHit = sRayIntersectAnimatingRActorExact(wrRay, ptRActor, fFindClosest, eGeoSetType, tResult);
	}
	return fHit;
}





static BOOL sRayIntersectStaticRActorExact(
    const Ray3D & wrRay,
    const RActor * ptRActor,
    const BOOL fFindClosest,
    const GeoSetType eGeoSetType,
    RActorExactIntersectionResult & tResult)
{
	ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
	ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

	const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];

	PRSTransform transformRActorInv = RActorGetInvClientTransform(ptRActor);
	Ray3D wrRayActor(wrRay);
	wrRayActor.wpOrigin = transformRActorInv * wrRayActor.wpOrigin;
	wrRayActor.wvDirection = transformRActorInv.Rotate(wrRayActor.wvDirection);

	WorldDistance wdTMin = FLT_MAX;
	// Inference: ++i from the increment column (604:49).
	for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
	{

        SubObject & tSubObject = tGeoSet.ptSubObjects[i];

        int32 nNumSegments = SubObjectGetNumSegments(tSubObject);

        for (int32 nSegment = 0; nSegment < nNumSegments; nSegment++)
        {
            int32 nIndexListCount, nIndexListIncr;
            SubObjectGetSegmentIndexListWalkInfo(tSubObject, nSegment, nIndexListCount, nIndexListIncr);

            for (int32 nIndexListIndex = 0; nIndexListIndex < nIndexListCount; nIndexListIndex += nIndexListIncr)
            {
                uint16 wIndex0, wIndex1, wIndex2;
                SubObjectGetSegmentTriangle(tSubObject, nSegment, nIndexListIndex, wIndex0, wIndex1, wIndex2);
                // skip strip restarts
                if ((wIndex0 == TRISTRIP_RESTART_INDEX) ||
                    (wIndex1 == TRISTRIP_RESTART_INDEX) ||
                    (wIndex2 == TRISTRIP_RESTART_INDEX))
                    continue;


                // skip degenerate triangles

                if (wIndex0 == wIndex2 || wIndex0 == wIndex1 || wIndex1 == wIndex2)
                    continue;






















                SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex0, tResult.wtri.wp0);
                SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex1, tResult.wtri.wp1);
                SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex2, tResult.wtri.wp2);

                BOOL fHit;


                fHit = Intersect::Ray3DAndTriangle3D(wrRayActor, tResult.wtri, tResult.wdT);






                if (fHit)
                {
                    if (tResult.wdT < wdTMin)
                    {
                        wdTMin = tResult.wdT;
                        tResult.nSubobject = i;
                        tResult.nSegment = nSegment;
                        tResult.nIndexListIndex = nIndexListIndex;
                    }

                    // Inference: the !fFindClosest exit jumps straight to the hit result at 695 (no row of its own); label name invented.
                    if (!fFindClosest)
                        goto HitFound;



                }
            }
        }
	}
	// Inference: the miss test is written negated; the original branches on (wdTMin < FLT_MAX) (b.mi / b.pl).
	if (!(wdTMin < FLT_MAX))
		return FALSE;




HitFound:
	tResult.wdT = wdTMin * ptRActor->transformClient.flScale;
	return TRUE;
























































}

static BOOL sRayIntersectAnimatingRActorExact(
    const Ray3D & wrRay,
    const RActor * ptRActor,
    const BOOL fFindClosest,
    const GeoSetType eGeoSetType,
    RActorExactIntersectionResult & tResult)
{

	const ActorAnim & tActorAnim = *ptRActor->ptActorAnim;

	ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
	ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

	const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];

	PRSTransform transformRActorInv = RActorGetInvClientTransform(ptRActor);
	Ray3D wrRayActor(wrRay);
	wrRayActor.wpOrigin = transformRActorInv * wrRayActor.wpOrigin;
	wrRayActor.wvDirection = transformRActorInv.Rotate(wrRayActor.wvDirection);

	WorldDistance wdTMin = FLT_MAX;
	// Inference: ++i from the increment column (777:49).
	for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
	{

        SubObject & tSubObject = tGeoSet.ptSubObjects[i];

        int32 nNumSegments = SubObjectGetNumSegments(tSubObject);
        for (int32 nSegment = 0; nSegment < nNumSegments; nSegment++)
        {
            int32 nIndexListCount, nIndexListIncr;
            SubObjectGetSegmentIndexListWalkInfo(tSubObject, nSegment, nIndexListCount, nIndexListIncr);
            for (int32 nIndexListIndex = 0; nIndexListIndex < nIndexListCount; nIndexListIndex += nIndexListIncr)
            {
                uint16 wIndex0, wIndex1, wIndex2;
                SubObjectGetSegmentTriangle(tSubObject, nSegment, nIndexListIndex, wIndex0, wIndex1, wIndex2);
                // skip strip restarts
                if ((wIndex0 == TRISTRIP_RESTART_INDEX) ||
                    (wIndex1 == TRISTRIP_RESTART_INDEX) ||
                    (wIndex2 == TRISTRIP_RESTART_INDEX))
                    continue;


                // skip degenerate triangles

                if (wIndex0 == wIndex2 || wIndex0 == wIndex1 || wIndex1 == wIndex2)
                    continue;






















                SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex0, tResult.wtri.wp0);
                SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex1, tResult.wtri.wp1);
                SubObjectGetSegmentVertexPos(tSubObject, nSegment, wIndex2, tResult.wtri.wp2);

                if (SubObjectHasVertexInfluences(tSubObject))
                {
                    VertInfluences tInfluences;

                    SubObjectGetSegmentVertexInfluences(tSubObject, nSegment, wIndex0, tInfluences);
                    const Matrix4x4 &m0 = tActorAnim.ptProcessedBoneSkinList[tInfluences.tBoneInfluence[0].nBoneIndex].mSkin;
                    tResult.wtri.wp0 = tResult.wtri.wp0 * m0;

                    SubObjectGetSegmentVertexInfluences(tSubObject, nSegment, wIndex1, tInfluences);
                    const Matrix4x4 &m1 = tActorAnim.ptProcessedBoneSkinList[tInfluences.tBoneInfluence[0].nBoneIndex].mSkin;
                    tResult.wtri.wp1 = tResult.wtri.wp1 * m1;

                    SubObjectGetSegmentVertexInfluences(tSubObject, nSegment, wIndex2, tInfluences);
                    const Matrix4x4 &m2 = tActorAnim.ptProcessedBoneSkinList[tInfluences.tBoneInfluence[0].nBoneIndex].mSkin;
                    tResult.wtri.wp2 = tResult.wtri.wp2 * m2;
                }


                BOOL fHit;


                fHit = Intersect::Ray3DAndTriangle3D(wrRayActor, tResult.wtri, tResult.wdT);






                if (fHit)
                {
                    if (tResult.wdT < wdTMin)
                    {
                        wdTMin = tResult.wdT;
                        tResult.nSubobject = i;
                        tResult.nSegment = nSegment;
                        tResult.nIndexListIndex = nIndexListIndex;
                    }
                        // Inference: the original jumps from here straight to the hit result at 884; label name invented.
                    if (!fFindClosest)


                        goto HitFound;


                }
            }
        }
	}
	// Inference: the miss test is written negated; the original branches on (wdTMin < FLT_MAX) (b.pl).
	if (!(wdTMin < FLT_MAX))
		return FALSE;




HitFound:
	tResult.wdT = wdTMin * ptRActor->transformClient.flScale;
	return TRUE;
}






static void sRActorsIntersectFrustumATree(const Frustum & tFrustum, RActorIDListPooled & listVisibleActors, XSphereArray * ptSphereTree, XSphereActorMap * pmapSphereActor)
{



    XSphereNodeList listVisibleSpheres(MemoryPoolsGetSphereNodeListPool());

    ptSphereTree->FrustumCull(tFrustum, listVisibleSpheres);

    POS pos = listVisibleSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listVisibleSpheres.GetNext(pos);
        int idActor;
        pmapSphereActor->Lookup(ptSphereNode, idActor);
        listVisibleActors.AddTail_Tracked(idActor);
    }
}





















void RActorsIntersectFrustum(const Frustum & tFrustum, RActorIDListPooled & listVisibleActors)
{
    // find all the ractors whose spheres intersect the frustum

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    sRActorsIntersectFrustumATree(tFrustum, listVisibleActors, ptCollisionGlobals->ptRActorSphereTree, ptCollisionGlobals->pmapRActorSphereActor);
}










































































static void sRActorsIntersectSphereATree(const Sphere & tSphere, const RActorID idIgnore, RActorIDListPooled & listActors, XSphereArray * ptSphereTree, XSphereActorMap * pmapSphereActor)
{



    XSphereNodeList listSpheres(MemoryPoolsGetSphereNodeListPool());
    ptSphereTree->SphereCull(tSphere, listSpheres);

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        RActorID idActor;
        pmapSphereActor->Lookup(ptSphereNode, (int &)idActor);

        // skip the ignored actor
        if (idActor != idIgnore)
            listActors.AddTail_Tracked(idActor);
    }
}








void RActorsIntersectSphere(const Sphere & tSphere, const RActorID idIgnore, RActorIDListPooled & listActors)
{
    // find all the ractors whose spheres intersect the sphere

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    sRActorsIntersectSphereATree(tSphere, idIgnore, listActors, ptCollisionGlobals->ptRActorSphereTree, ptCollisionGlobals->pmapRActorSphereActor);
}










void RActorsPlayerTargetCull(const Sphere & tSphereClose, RActorIDListPooled & listActorsClose, const Sphere & tSphereCone, const Vector2D & wpConeStart, const Vector2D & wpConeDir, Angle aRequiredTheta, float flSearchDist, RActorIDListPooled & listActorsCone, const RActorID idIgnore)









{
    // gather the actors close to the player, and the actors in the cone in front of the player
    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    XSphereNodeList listSpheresClose(MemoryPoolsGetSphereNodeListPool());
    XSphereNodeList listSpheresCone(MemoryPoolsGetSphereNodeListPool());

    ptCollisionGlobals->ptRActorSphereTree->PlayerTargetCull(tSphereClose, listSpheresClose, tSphereCone, wpConeStart, wpConeDir, aRequiredTheta, flSearchDist, listSpheresCone);

    // close actors

    POS posClose = listSpheresClose.GetHeadPosition();
    while (posClose)
    {
        const XSphereNode * ptSphereNode = listSpheresClose.GetNext(posClose);

        // the user data is the actor id

        RActorID idActor = (RActorID)(intptr_t)ptSphereNode->GetUserData();





        if (idActor == idIgnore)
            continue;



        listActorsClose.AddTail_Tracked(idActor);
    }

    POS posCone = listSpheresCone.GetHeadPosition();
    while (posCone)
    {
        const XSphereNode * ptSphereNode = listSpheresCone.GetNext(posCone);

        // the user data is the actor id

        RActorID idActor = (RActorID)(intptr_t)ptSphereNode->GetUserData();





        if (idActor == idIgnore)
            continue;



        listActorsCone.AddTail_Tracked(idActor);
    }
}



void RActorsIntersectSegment(const LineSeg3D & tLineSeg, const RActorID idIgnore, SphereNodeMemoryPool * ptSphereNodePool, RActorIDListPooled & listActors, BOOL fEnvAppOnly)
{
    // find all the ractors whose spheres intersect the line segment

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    XSphereNodeList listSpheres(ptSphereNodePool);
    if (fEnvAppOnly)
    {
        ptCollisionGlobals->ptEnvAppSphereTree->LineSegCull(tLineSeg, listSpheres);
    }
    else
    {
        ptCollisionGlobals->ptRActorSphereTree->LineSegCull(tLineSeg, listSpheres);
    }
    XSphereActorMap * pmapSphereActor = ptCollisionGlobals->pmapRActorSphereActor;

    POS pos = listSpheres.GetHeadPosition();
    while (pos)
    {
        const XSphereNode * ptSphereNode = listSpheres.GetNext(pos);
        RActorID idActor;
        BOOL bRetval = pmapSphereActor->Lookup(ptSphereNode, (int &)idActor);
        (void)bRetval;

        // skip the ignored actor
        if (idActor != idIgnore)
        {






            listActors.AddTail_Tracked(idActor);
        }
    }
}




void RActorCollisionCapsulesAddInitial(
    RActor * ptRActor)
{

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    const PRSTransform & transformRActor = ptRActor->transformClient;
    const float flScale = transformRActor.flScale;

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;

    for (int32 i = 0; i < tStructure.nCollisionCapsuleCount; i++)
    {
        const CollisionCapsule & tCC = tStructure.ptCollisionCapsules[i];

        PRTransform transform;
        RActorGetHardpointTransform(*ptRActor, tCC.tHardpoint, transform);


        Sphere ws;
        ws.Set(transform.wp, flScale * (tCC.flRadius + tCC.flLength * 0.5f));

        if (ptCollisionGlobals->ptCollisionSphereTree->IsSphereValid(ws))
        {





            XSphereNode * ptSphereNode = ptCollisionGlobals->ptCollisionSphereTree->AddSphereNode(ws, NULL);

            CollisionCapsuleInstance tCCInstance;
            tCCInstance.ptSphereNode = ptSphereNode;
            tCCInstance.idRActor = ptRActor->id;
            tCCInstance.nCollisionCapsuleIndex = i;


            Vector3D wp1(0.0f, 0.0f, flScale * -0.5f * tCC.flLength);
            Vector3D wp2(0.0f, 0.0f, flScale * 0.5f * tCC.flLength);
            tCCInstance.tCapsule.wp1 = transform * wp1;
            tCCInstance.tCapsule.wp2 = transform * wp2;
            tCCInstance.tCapsule.flRadius = flScale * tCC.flRadius;

            POS tPos = ptRActor->tCollisionCapsuleList.AddTail_Tracked(tCCInstance);
            ptSphereNode->SetUserData(&ptRActor->tCollisionCapsuleList.GetAt(tPos));
        }
    }
}




void RActorCollisionCapsulesDispose(RActor * ptRActor)
{
    // remove all the capsules from the collision sphere tree

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    POS pos = ptRActor->tCollisionCapsuleList.GetHeadPosition();
    while (pos)
    {
        CollisionCapsuleInstance & tCCInstance = ptRActor->tCollisionCapsuleList.GetNext(pos);

        ptCollisionGlobals->ptCollisionSphereTree->RemoveSphereNode(tCCInstance.ptSphereNode);
    }

    ptRActor->tCollisionCapsuleList.RemoveAll();
}





void RActorCollisionCapsulesUpdate(
    RActor * ptRActor)
{

    if (ptRActor->tCollisionCapsuleList.Count() == 0)
    {
        return;
    }

    const PRSTransform & transformRActor = ptRActor->transformClient;
    const float flScale = transformRActor.flScale;

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;
    if (!tStructure.ptCollisionCapsules)
    {
        return;
    }

    const float cflMoveToleranceSqr = 0.0001f;

    POS pos = ptRActor->tCollisionCapsuleList.GetHeadPosition();
    while (pos)
    {
        CollisionCapsuleInstance & tCCInstance = ptRActor->tCollisionCapsuleList.GetNext(pos);
        CollisionCapsule & tCC = tStructure.ptCollisionCapsules[tCCInstance.nCollisionCapsuleIndex];

        PRTransform transform;
        RActorGetHardpointTransform(*ptRActor, tCC.tHardpoint, transform);


        Vector3D wp1(0.0f, 0.0f, flScale * -0.5f * tCC.flLength);
        Vector3D wp2(0.0f, 0.0f, flScale * 0.5f * tCC.flLength);

        wp1 = transform * wp1;
        wp2 = transform * wp2;


        if (Vector3D::DistanceSquared(wp1, tCCInstance.tCapsule.wp1) < cflMoveToleranceSqr &&
              Vector3D::DistanceSquared(wp2, tCCInstance.tCapsule.wp2) < cflMoveToleranceSqr)
        {
            continue;
        }

        tCCInstance.tCapsule.wp1 = wp1;
        tCCInstance.tCapsule.wp2 = wp2;
        tCCInstance.tCapsule.flRadius = flScale * tCC.flRadius;


        Sphere ws;
        ws.Set(transform.wp, flScale * (tCC.flRadius + tCC.flLength * 0.5f));

        ptCollisionGlobals->ptCollisionSphereTree->ModifySphereNode(tCCInstance.ptSphereNode, ws);
    }
}



XSphereArray * RActorGetCollisionSphereTree()
{
    // the sphere tree used for collision capsules

    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    return ptCollisionGlobals->ptCollisionSphereTree;
}











































static CRefString RActorSphereTreeNodeName(const XSphereNode & tSphereNode)
{
    CRefString strName("INVALID RACTOR");
    RActorCollisionGlobals * ptCollisionGlobals = CGameGet().ptRActorCollisionGlobals;

    XSphereActorMap * pmapSphereActor = ptCollisionGlobals->pmapRActorSphereActor;
    RActorID idRActor;
    BOOL bRetval = pmapSphereActor->Lookup(&tSphereNode, (int &)idRActor);
    if (bRetval)
    {
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            strName = MarkerNameGetString(ptRActor->tName);
        }
    }

    return strName;
}


static CRefString CollisionSphereTreeNodeName(const XSphereNode & tSphereNode)
{
    CRefString strName("INVALID COLLSPHERE");
    CollisionCapsuleInstance * ptCCInstance = (CollisionCapsuleInstance *)tSphereNode.GetUserData();
    if (ptCCInstance)
    {
        RActor * ptRActor = RActorTryToGet(ptCCInstance->idRActor);
        if (ptRActor)
        {
            strName = MarkerNameGetString(ptRActor->tName);
        }
    }

    return strName;
}
