#include "Emitter.h"
#include "SubObject.h"
#include "Definitions/AppearanceDefinition.h"
#include "RActors/RActors.h"
#include "RActors/RActorAnimation.h"
#include "Animation.h"
#include "Cloth.h"
#include "Scene.h"
#include "../Common/Definitions/SceneDefinition.h"
#include "../Sigma/Random.h"
#include "../Sigma/Math/Triangle.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
// Original RandomPoint.cpp and Animation.cpp declarations from DWARF.
WorldDistance GetRandomRadius(RNG &, WorldDistance, WorldDistance);
void GetRandomVectorPointOnSphere(RNG &, WorldDistance, Vector3D &);
void GetRandomVectorPointOnCircle(RNG &, WorldDistance, Vector3D &);
void GetRandomVectorPointOnDome(RNG &, WorldDistance, Vector3D &);
void AnimationSkinSingleBone(const Vector3D *, const int32, const VertInfluences *, const ProcessedBoneSkin *, Vector3D *);
static void sRandomActorSubobjectPoint(RNG &, const RActor &, const uint32, const BOOL, const uint32, Vector3D &, Vector3D &, int32 &);
static void sRandomSceneSubobjectPoint(RNG &, const CScene *, const uint32, Vector3D &, Vector3D &);
static void sRandomTrianglePoint(RNG &, const Triangle3D &, Vector3D &, Vector3D &);
static void sSelectRandomTriangleIndex(RNG &, int32 &, int32 &, const SubObject &, const SNO, const BOOL);
static int32 sTriangleFromAnimatingSubobject(const int32, const int32, const RActor *, const SubObject &, Triangle3D &);
static void sTriangleFromCloth(const int32, const SubObject &, const ClothID, Triangle3D &);
static int32 sTriangleFromStaticSubobject(const int32, const int32, const SubObject &, Triangle3D &);
static void sAddTriArea(const SubObject &, const int32, const uint16, const uint16, const uint16, const int32, const BOOL);
static void sComputeTriAreas(const SubObject &, const BOOL);


// Inference at scene helper: unused snoAppearance uses locked tHeader.snoHandle; original emits no argument load.
// Same-line declaration orders for segment/count and vertex indices follow original stack slots.



































































// NON_MATCHING: 2516 bytes; seven FP register differences in even-cylinder angle arithmetic.
void EmitterGetRandomPoint(
    RNG & rng,
    const Vector3D & wpPointOffset,
    Emitter & tEmitter,
    Vector3D & wp,
    Vector3D & wvNormal,
    int32 & nBoneIndex)
{
    wvNormal = cgwvNull;
    nBoneIndex = -1;
    switch (tEmitter.eType) {
    case EMIT_POINT:
        wp = tEmitter.ptPoint + wpPointOffset;
        break;
    case EMIT_CYLINDER_SHELL: {




        WorldDistance wdRadius = GetRandomRadius(rng, tEmitter.wdRadiusMin, tEmitter.wdRadiusDelta);

        Vector3D wvRandOffset;
        GetRandomVectorPointOnCircle(rng, wdRadius, wvRandOffset);
        wvRandOffset.z = RandRangeFloat_Internal(rng, tEmitter.wdHeightMin, tEmitter.wdHeightDelta);
        wp = tEmitter.ptPoint + wpPointOffset + wvRandOffset;
        break;
    }
    case EMIT_EVEN_CYLINDER_SHELL: {




        WorldDistance wdRadius = GetRandomRadius(rng, tEmitter.wdRadiusMin, tEmitter.wdRadiusDelta);

        Angle ang;
        if (tEmitter.nCurCount == 0) {
            ang = RandAngle(rng);
            tEmitter.angleEvenOffset = ang;
        } else {
            ang = Math::NormalizeAngle(tEmitter.angleEvenOffset + tEmitter.nCurCount * FULL_CIRCLE / tEmitter.nSpawnCount);
        }







        Vector3D wvEvenOffset = cgwvXAxis * wdRadius;
        Vector3D::Rotate(cgwvZAxis, ang, wvEvenOffset);
        wvEvenOffset.z = RandRangeFloat_Internal(rng, tEmitter.wdHeightMin, tEmitter.wdHeightDelta);
        wp = tEmitter.ptPoint + wpPointOffset + wvEvenOffset;
        break;
    }
    case EMIT_SPHERE_SHELL: {




        WorldDistance wdRadius = GetRandomRadius(rng, tEmitter.wdRadiusMin, tEmitter.wdRadiusDelta);

        Vector3D wvRandOffset;
        GetRandomVectorPointOnSphere(rng, wdRadius, wvRandOffset);
        wp = tEmitter.ptPoint + wpPointOffset + wvRandOffset;
        break;
    }
    case EMIT_DOME_SHELL: {



        WorldDistance wdRadius = GetRandomRadius(rng, tEmitter.wdRadiusMin, tEmitter.wdRadiusDelta);

        Vector3D wvRandOffset;
        GetRandomVectorPointOnDome(rng, wdRadius, wvRandOffset);
        wp = tEmitter.ptPoint + wpPointOffset + wvRandOffset;
        break;
    }
    case EMIT_ACTOR_SUBOBJECT_SHELL: {



        const RActor * ptRActor = RActorTryToGet((RActorID)tEmitter.id);
        if (ptRActor)
            sRandomActorSubobjectPoint(rng, *ptRActor, tEmitter.dwSubObjectIndex, FALSE, 0, wp, wvNormal, nBoneIndex);
        else
            wp = tEmitter.ptPoint + wpPointOffset;
        break;
    }
    case EMIT_ACTOR_SUBOBJECT_SHELL_SEQUENTIAL: {














        const RActor * ptRActor = RActorTryToGet((RActorID)tEmitter.id);
        if (ptRActor)
            sRandomActorSubobjectPoint(rng, *ptRActor, tEmitter.dwSubObjectIndex, TRUE, tEmitter.dwTriIndex, wp, wvNormal, nBoneIndex);
        else
            wp = tEmitter.ptPoint + wpPointOffset;
        break;
    }
    case EMIT_SCENE_SUBOBJECT_SHELL: {














        const CScene * ptCScene = CSceneTryToGet(tEmitter.id);
        if (ptCScene)
            sRandomSceneSubobjectPoint(rng, ptCScene, tEmitter.dwSubObjectIndex, wp, wvNormal);
        else
            wp = tEmitter.ptPoint + wpPointOffset;
        break;
    }
    case EMIT_BOX_SHELL:
        wp.x = RandRangeFloat_Internal(rng, tEmitter.wvMinBoxExtent.x, tEmitter.wvMaxBoxExtent.x - tEmitter.wvMinBoxExtent.x);
        wp.y = RandRangeFloat_Internal(rng, tEmitter.wvMinBoxExtent.y, tEmitter.wvMaxBoxExtent.y - tEmitter.wvMinBoxExtent.y);
        wp.z = RandRangeFloat_Internal(rng, tEmitter.wvMinBoxExtent.z, tEmitter.wvMaxBoxExtent.z - tEmitter.wvMinBoxExtent.z);
        wp += wpPointOffset;
        break;
    default:














        DisplayInternalError(NULL, "", 253, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}


// NON_MATCHING: 1600 vs 1604 bytes; transform arithmetic scheduling and output-pointer load pairing differ.
static void sRandomActorSubobjectPoint(
    RNG & rng,
    const RActor & tRActor,
    const uint32 dwSubObjectIndex,
    const BOOL fFixedTriangle,
    const uint32 dwFixedTri,
    Vector3D & wp,
    Vector3D & wvNormal,
    int32 & nBoneIndex)
{
    ScopedAppearanceDefinitionLockR pdefAppearance(tRActor.snoAppearance);
    ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

    const Structure & tStructure = pdefAppearance->tStructure;

    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

    BOOL fAnimating = !(tStructure.dwFlags & (1 << STRUCTURE_CANNOT_ANIMATE_BIT)) && tRActor.ptActorAnim;





    int32 nSubObjectIndex;
    if (dwSubObjectIndex == (uint32)-1)
        nSubObjectIndex = Rand_Internal(rng, tGeoSet.nSubObjectCount - 1);
    else
        nSubObjectIndex = dwSubObjectIndex;






    SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
    const RActorGeoSetData & tGeoSetData = tRActor.tGeoSetData[eGeoSetType];

    Triangle3D wtri;
    int32 nBoneInfluences = -1;

    int32 nSegmentIndex;
    int32 nIndexListIndex;
    if (fFixedTriangle) {

        uint32 dwTriNum = dwFixedTri;


        int32 nMaxTriNum = SubObjectGetNumTriangles(tSubObject);
        uint32 dwTriIndex = dwTriNum % nMaxTriNum;
        SubObjectGetTriangleSegment(tSubObject, dwTriIndex, nSegmentIndex, nIndexListIndex);
    } else {
        sSelectRandomTriangleIndex(rng, nSegmentIndex, nIndexListIndex, tSubObject, tRActor.snoAppearance, FALSE);
    }
    if (fAnimating) {






        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];
        if (tSubObjectState.idCloth == -1)
            nBoneInfluences = sTriangleFromAnimatingSubobject(nSegmentIndex, nIndexListIndex, &tRActor, tSubObject, wtri);
        else
            sTriangleFromCloth(nIndexListIndex, tSubObject, tSubObjectState.idCloth, wtri);
    } else {
        nBoneInfluences = sTriangleFromStaticSubobject(nSegmentIndex, nIndexListIndex, tSubObject, wtri);
    }









    const Matrix4x4 & mWorld = tRActor.mWorld;
    wtri.wp0 = wtri.wp0 * mWorld;
    wtri.wp1 = wtri.wp1 * mWorld;
    wtri.wp2 = wtri.wp2 * mWorld;
    sRandomTrianglePoint(rng, wtri, wp, wvNormal);
    nBoneIndex = nBoneInfluences;
}







static void sRandomSceneSubobjectPoint(
    RNG & rng,
    const CScene * ptCScene,
    const uint32 dwSubObjectIndex,
    Vector3D & wp,
    Vector3D & wvNormal)
{
    ScopedSceneDefinitionLockR pdefScene(ptCScene->tSCD.snoScene);
    ScopedAppearanceDefinitionLockR pdefAppearance(pdefScene->GetAppearance());
    ScopedAppearanceBufferContentRetrieval tAppearanceBufferContentRetrieval(pdefAppearance);

    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[GEOSET_HIGHPOLY];

    int32 nSubObjectIndex;
    if (dwSubObjectIndex == (uint32)-1)
        nSubObjectIndex = Rand_Internal(rng, tGeoSet.nSubObjectCount - 1);
    else
        nSubObjectIndex = dwSubObjectIndex;






    SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];

    Triangle3D wtri;
    int32 nSegmentIndex, nIndexListIndex; // DWARF same-line locals; order follows original stack slots.
    sSelectRandomTriangleIndex(rng, nSegmentIndex, nIndexListIndex, tSubObject, pdefAppearance->tHeader.snoHandle, TRUE);
    sTriangleFromStaticSubobject(nSegmentIndex, nIndexListIndex, tSubObject, wtri);
    wtri.wp0 = ptCScene->tSCD.transformWorld * wtri.wp0;
    wtri.wp1 = ptCScene->tSCD.transformWorld * wtri.wp1;
    wtri.wp2 = ptCScene->tSCD.transformWorld * wtri.wp2;
    sRandomTrianglePoint(rng, wtri, wp, wvNormal);
}







static void sRandomTrianglePoint(
    RNG & rng,
    const Triangle3D & wtri,
    Vector3D & wp,
    Vector3D & wvNormal)
{
    float flU = RandFloat_Internal(rng);
    float flV = RandFloat_Internal(rng);
    if (flU + flV > 1.0f) {
        flU = 1.0f - flU;
        flV = 1.0f - flV;
    }
    wtri.PointFromBarycentric(flU, flV, wp);
    wtri.GetFaceNormal(wvNormal);
}













static void sSelectRandomTriangleIndex(
    RNG & rng,
    int32 & nSegmentIndex,
    int32 & nIndexListIndex,
    const SubObject & tSubObject,
    const SNO snoAppearance,
    const BOOL bIsScene)
{
    sComputeTriAreas(tSubObject, bIsScene);


    blz::vector<int, blz::allocator<int> > & tTriAreas = tSubObject.ptPostprocessed->tTriAreas;
    int32 & nTotalTriArea = tSubObject.ptPostprocessed->nTotalTriArea;




    int32 nRandValue = Rand_Internal(rng, nTotalTriArea - 1);
    nRandValue = Math::Min(nRandValue, tTriAreas.back());












    int32 nChosen = 0;
    for (; nChosen != (int32)tSubObject.ptPostprocessed->tTriAreas.size(); ++nChosen) {
        if (nRandValue <= tSubObject.ptPostprocessed->tTriAreas[nChosen])
            break;
    }
    nChosen = Math::Min(nChosen, (int32)tSubObject.ptPostprocessed->tTriAreas.size() - 1);
    SubObjectGetTriangleSegment(tSubObject, nChosen, nSegmentIndex, nIndexListIndex);
}














static int32 sTriangleFromAnimatingSubobject(
    const int32 nSegmentIndex,
    const int32 nIndexListIndex,
    const RActor * ptRActor,
    const SubObject & tSubObject,
    Triangle3D & wtri)
{

    const ActorAnim & tActorAnim = *ptRActor->ptActorAnim;

    uint16 wIndices[3];
    SubObjectGetSegmentTriangle(tSubObject, nSegmentIndex, nIndexListIndex, wIndices[0], wIndices[1], wIndices[2]);

    Vector3D pv[3];
    int32 nMostInfluentialBoneIndex = -1;
    float flMostInfluence = 0.0f;

    for (int32 i = 0; i < 3; ++i) {

        Vector3D wpPos;
        VertInfluences tInfluence;
        SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndices[i], wpPos);
        SubObjectGetSegmentVertexInfluences(tSubObject, nSegmentIndex, wIndices[i], tInfluence);
        AnimationSkinSingleBone(&wpPos, 1, &tInfluence, tActorAnim.ptProcessedBoneSkinList, &pv[i]);
        if (tInfluence.tBoneInfluence[0].flWeight >= flMostInfluence) {
            flMostInfluence = tInfluence.tBoneInfluence[0].flWeight;
            nMostInfluentialBoneIndex = tInfluence.tBoneInfluence[0].nBoneIndex;
        }
    }
    wtri.Set(pv[0], pv[1], pv[2]);
    return nMostInfluentialBoneIndex;
}










static void sTriangleFromCloth(
    const int32 nIndexListIndex,
    const SubObject & tSubObject,
    const ClothID idCloth,
    Triangle3D & wtri)
{


    const Cloth * ptCloth = ClothManager::Get(idCloth);
    Vector3D wps[3];

    for (int32 i = 0; i < 3; ++i) {

        uint16 wIndex = tSubObject.pwIndexList[nIndexListIndex + i];



        uint16 wClothVertexIndex = tSubObject.ptClothStructure->ptClothVertexMapping[wIndex];
        wps[i] = ptCloth->GetVertexPosition(wClothVertexIndex);
    }
    wtri.Set(wps[0], wps[1], wps[2]);
}









static int32 sTriangleFromStaticSubobject(
    const int32 nSegmentIndex,
    const int32 nIndexListIndex,
    const SubObject & tSubObject,
    Triangle3D & wtri)
{
    uint16 wIndices[3];
    SubObjectGetSegmentTriangle(tSubObject, nSegmentIndex, nIndexListIndex, wIndices[0], wIndices[1], wIndices[2]);

    Vector3D wpPos0, wpPos1, wpPos2;
    SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndices[0], wpPos0);
    SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndices[1], wpPos1);
    SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndices[2], wpPos2);
    wtri.Set(wpPos0, wpPos1, wpPos2);



    int32 nMostInfluentialBoneIndex = -1;
    if (SubObjectHasVertexInfluences(tSubObject)) {


        float flMostInfluence = 0.0f;
        for (int32 i = 0; i < 3; ++i) {

            VertInfluences tInfluence;
            SubObjectGetSegmentVertexInfluences(tSubObject, nSegmentIndex, wIndices[i], tInfluence);
            if (tInfluence.tBoneInfluence[0].flWeight >= flMostInfluence) {
                flMostInfluence = tInfluence.tBoneInfluence[0].flWeight;
                nMostInfluentialBoneIndex = tInfluence.tBoneInfluence[0].nBoneIndex;
            }
        }
    }
    return nMostInfluentialBoneIndex;
}















static void sAddTriArea(
    const SubObject & tSubObject,
    const int32 nSegmentIndex,
    const uint16 wIndex0,
    const uint16 wIndex1,
    const uint16 wIndex2,
    const int32 nFace,
    const BOOL bIsScene)
{
    float fTriArea = 0.0f;
    if (wIndex0 != (uint16)-1 && wIndex1 != (uint16)-1 && wIndex2 != (uint16)-1) {




        Vector3D wpPos0, wpPos1, wpPos2;
        SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndex0, wpPos0);
        SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndex1, wpPos1);
        SubObjectGetSegmentVertexPos(tSubObject, nSegmentIndex, wIndex2, wpPos2);

        Triangle3D wtri;
        wtri.Set(wpPos0, wpPos1, wpPos2);
        fTriArea = wtri.Area();
        if (bIsScene) {


            RGBAColor rgbaColor0, rgbaColor1, rgbaColor2;
            SubObjectGetSegmentVertexColor(tSubObject, nSegmentIndex, wIndex0, 0, rgbaColor0);
            SubObjectGetSegmentVertexColor(tSubObject, nSegmentIndex, wIndex1, 0, rgbaColor1);
            SubObjectGetSegmentVertexColor(tSubObject, nSegmentIndex, wIndex2, 0, rgbaColor2);
            if (rgbaColor0.a == 0)
                fTriArea *= (rgbaColor0.r / 255.0f + rgbaColor1.r / 255.0f + rgbaColor2.r / 255.0f) / 3.0f;
        }
    }











    int32 & nTotalTriArea = tSubObject.ptPostprocessed->nTotalTriArea;
    nTotalTriArea += (int32)(fTriArea * 16384.0f);
    tSubObject.ptPostprocessed->tTriAreas[nFace] = nTotalTriArea;
}










static void sComputeTriAreas(
    const SubObject & tSubObject,
    const BOOL bIsScene)
{
    if (!tSubObject.ptPostprocessed->tTriAreas.empty())
        return;



    int32 nNumSegments = SubObjectGetNumSegments(tSubObject);
    int32 nNumTriangles = SubObjectGetNumTriangles(tSubObject);



    blz::vector<int, blz::allocator<int> > & tTriAreas = tSubObject.ptPostprocessed->tTriAreas;
    tSubObject.ptPostprocessed->nTotalTriArea = 0;
    tTriAreas.resize(nNumTriangles);






    for (int32 nSegment = 0, t = 0; nSegment < nNumSegments; ++nSegment) {

        int32 nIndexListCount, nIndexListIncr;
        SubObjectGetSegmentIndexListWalkInfo(tSubObject, nSegment, nIndexListCount, nIndexListIncr);

        for (int32 nIndexListIndex = 0; nIndexListIndex < nIndexListCount; nIndexListIndex += nIndexListIncr) {

            uint16 wIndex0, wIndex1, wIndex2;
            SubObjectGetSegmentTriangle(tSubObject, nSegment, nIndexListIndex, wIndex0, wIndex1, wIndex2);
            sAddTriArea(tSubObject, nSegment, wIndex0, wIndex1, wIndex2, t++, bIsScene);
        }
    }
}
