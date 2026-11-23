// X/Client/Lights.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r6-render/lights/frag.cpp by build/try/r6-render/gen.py.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Lights.h"
#include "CGame.h"
#include "Gfx/GfxCommon.h"
#include "../Sigma/Math/MathGeneral.h"
#include "Definitions/LightDefinition.h"
#include "InterpolationTrack.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/InactiveMarker.h"
#include "../Common/SceneCommon.h"
#include "../Sigma/Math/Intersect.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Memory/SigmaMemory.h"


enum {  // Lights.h:85
    LIGHTID_INVALID = -1,
};


#include "CPlayer.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Sigma/Math/Quaternion.h"
// X/Client/TexAnimation.cpp and X/Client/CWorld.cpp:1433; no adopted header declares them yet.
void TexAnimInitialize(const MaterialTexture * ptMatTex, TexAnimState * ptState, BOOL bStart);
BOOL CWorldIsRetroForLighting();
void TexAnimUpdate(const CHAR * pTexAnimCompParams, uint32 dwTexAnimCompParamsSize, const ObjectTexAnimParam * ptObjectParams, TexAnimState & tTexAnimState, float flDeltaTime, float flClientTime);
static void sEvaluateLightPaths(Light * ptLight, const LightDefinition * pdefLight);
static void sRandomizeLightPaths(Light * ptLight);
#include "RWindow.h"
#include "Gfx/GfxPrimitives.h"
#include "../Common/GlobalSNO.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Math/Cylinder.h"
// X/Client/Gfx/GfxPrimitives.cpp:425 and :541; GfxPrimitives.h does not declare them yet.
void GfxArrow3DDrawDeferred(const Vector3D & wp0, const Vector3D & wp1, const RGBAColor & rgbaColor, const float flHeadLength, GlobalSNO snoShader, const RenderContext eContext);
void GfxConeDrawDeferred(const Vector3D & wpTip, const Vector3D & vDir, const float flLength, const Angle aConeAngle, const RGBAColor & rgbaColor, const int32 nNumDivisions, const int32 nNumSections, GfxFillStyle eFillStyle, GlobalSNO snoShader, const RenderContext eContext);






































struct LightInfo {  // size 0x94
    RenderContext eContext;  // 0x00
    uint32 dwDynamicIgnoreFlags;  // 0x04
    LightState tState;  // 0x08
    int32 nPriority;  // 0x90
};

typedef XPooledList<Light *, Light *> PooledLightList;
typedef XListMemoryPool<Light *> LightListPool;

struct LightRenderData {  // size 0xde08
    LightInfo tDefaultContextLights[256];  // 0x0
    uint32 dwDefaultContextLightCount;  // 0x9400
    LightInfo tNonDefaultContextLights[128];  // 0x9404
    uint32 dwNonDefaultContextLightCount;  // 0xde04
};

struct LightGlobals {  // size 0x1bc98
    LightRenderData tRenderData[2];  // 0x0
    LightListPool tLightListPool;  // 0x1bc10
    PooledLightList listDynamicLights;  // 0x1bc40
    Vector3D wpDirLightPointOfInterest;  // 0x1bc68
    LightID idDebugLightWarn;  // 0x1bc74
    LightID idDebugLightCaution;  // 0x1bc78
    LightID idDebugLightCritical;  // 0x1bc7c
    LightID idDebugLightFullBright;  // 0x1bc80
    SSceneID idSelSScene;  // 0x1bc84
    RActorID idSelRActor;  // 0x1bc88
    SNO snoSelAppearance;  // 0x1bc8c
    int nSelSubObject;  // 0x1bc90
};
static_assert(sizeof(LightGlobals) == 0x1bc98, "LightGlobals");















































void LightsInitializeForGame()
{
    CGame & tCGame = CGameGet();
    LightGlobals *& ptLightGlobals = tCGame.ptLightGlobals;

    ptLightGlobals = (LightGlobals *)SigmaMemoryNew(sizeof(LightGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);

    tCGame.ptLights = TChainedDataArrayCreateTracked<Light, LightID>("Lights", AppGetCurrentMemoryInterface(), 128);

    ptLightGlobals->tLightListPool.Initialize();
    ptLightGlobals->tLightListPool.SetBlockCount_Tracked(128);

    ptLightGlobals->listDynamicLights.Initialize(&ptLightGlobals->tLightListPool);









    ptLightGlobals->idDebugLightWarn = LIGHTID_INVALID;
    ptLightGlobals->idDebugLightCaution = LIGHTID_INVALID;
    ptLightGlobals->idDebugLightCritical = LIGHTID_INVALID;
    ptLightGlobals->idDebugLightFullBright = LIGHTID_INVALID;
    ptLightGlobals->idSelSScene = SSCENEID_INVALID;
    ptLightGlobals->idSelRActor = RACTORID_INVALID;
    ptLightGlobals->snoSelAppearance = SNO_NONE;
    ptLightGlobals->nSelSubObject = -1;
}






void LightsDisposeForGame()
{
    CGame & tCGame = CGameGet();
    LightGlobals *& ptLightGlobals = tCGame.ptLightGlobals;

    TChainedDataArrayDispose(tCGame.ptLights);

    ptLightGlobals->listDynamicLights.Dispose();

    ptLightGlobals->tLightListPool.Dispose();








    SigmaMemoryFree(ptLightGlobals, AppGetCurrentMemoryInterface()); ptLightGlobals = NULL;
}






static void sLightRemoveFromAllLists(Light * ptLight)
{
    if (ptLight->posDynamicList)
    {
        LightGlobals * ptLightGlobals = CGameGet().ptLightGlobals;
        ptLightGlobals->listDynamicLights.RemoveAt(ptLight->posDynamicList);
        ptLight->posDynamicList = NULL;
    }
}




















void LightsSetLastSeenTime(Frustum & tFrustum)
{
    uint32 dwNow = GameGetServerTime();
    CGame & tCGame = CGameGet();
    TDataArrayIterator<Light> tIt;
    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;
        Sphere ws;
        ws.Set(ptLight->tState.wp, ptLight->tState.tInterpState.wdRadius);
        BOOL bInside = Intersect::FrustumAndSphere(tFrustum, ws);
        if (bInside)
        {
            ptLight->dwLastSeen = dwNow;
        }
    }
}







static void sDeactivate(Light * ptLight)
{
    Sphere wsBounds;
    wsBounds.Set(ptLight->tState.wp, ptLight->tState.tInterpState.wdRadius);

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptLight->idSScene);
    if (ptSCD)
    {
        InactiveMarkerCreateFromMarkerSet(*ptSCD, ptLight->tMarkerOwner, PRTransform(ptLight->tState.transform.q, ptLight->tState.wp), wsBounds);
    }

    LightDelete(ptLight->id);
}
































void LightsCheckSeenRecently()
{
    uint32 dwNow = GameGetServerTime();
    CGame & tCGame = CGameGet();
    TDataArrayIterator<Light> tIt;
    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;
        if (ptLight->tMarkerOwner.snoMarkerSet != SNO_NONE && ptLight->idSScene != SSCENEID_INVALID && ptLight->dwLastSeen < dwNow)
        {
            sDeactivate(ptLight);
        }
    }
}




















void LightsDeactivateInScene(SSceneID idSScene)
{
    CGame & tCGame = CGameGet();
    if (!tCGame.ptLights)
        return;

    TDataArrayIterator<Light> tIt;
    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;
        if (ptLight->idSScene == idSScene && ptLight->tMarkerOwner.snoMarkerSet != SNO_NONE)
        {
            sDeactivate(ptLight);
        }
    }
}



















void LightsDeleteAllForRenderContext(const RenderContext eContext)
{
    CGame & tCGame = CGameGet();
    if (!tCGame.ptLights)
        return;

    TDataArrayIterator<Light> tIt;
    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;
        if (ptLight->eContext == eContext)
        {
            LightDelete(ptLight->id);
        }
    }

    LightGlobals * ptLightGlobals = tCGame.ptLightGlobals;

    if (!LightTryToGet(ptLightGlobals->idDebugLightWarn))
        ptLightGlobals->idDebugLightWarn = LIGHTID_INVALID;

    if (!LightTryToGet(ptLightGlobals->idDebugLightCaution))
        ptLightGlobals->idDebugLightCaution = LIGHTID_INVALID;

    if (!LightTryToGet(ptLightGlobals->idDebugLightCritical))
        ptLightGlobals->idDebugLightCritical = LIGHTID_INVALID;

    if (!LightTryToGet(ptLightGlobals->idDebugLightFullBright))
        ptLightGlobals->idDebugLightFullBright = LIGHTID_INVALID;
}











void LightsDeleteAll(RenderContext eContext)
{
    // delete all the lights in this context

    LightsDeleteAllForRenderContext(eContext);
}

struct LightSort {  // size 0x18
    const LightState * ptLightState;  // 0x00
    SNO snoLight;  // 0x08
    int32 nPriority;  // 0x0c
    float flIntensity;  // 0x10
    int32 nIntensity;  // 0x14
};
static const LightState * sFindAndRemoveBestLight(int32 & nSortedLightCount, LightSort * ptSort, SNO & snoLightBest)
{


    int32 nBestIndex = 0;

    for (int32 i = 1; i < nSortedLightCount; i++)
    {
        const LightSort & tSort = ptSort[i];
        const LightSort & tBest = ptSort[nBestIndex];

        if (tSort.nPriority > tBest.nPriority)
        {
            nBestIndex = i;
        }
        else if (tSort.nPriority == tBest.nPriority)
        {

            if (tSort.nIntensity > tBest.nIntensity)
            {
                nBestIndex = i;
            }
        }
    }




    const LightState * ptLightStateBest = ptSort[nBestIndex].ptLightState;
    snoLightBest = ptSort[nBestIndex].snoLight;

    ptSort[nBestIndex] = ptSort[nSortedLightCount - 1];
    nSortedLightCount--;

    return ptLightStateBest;
}






static void sModifyAttenuationForPlusToLightRadius(float * pflAttenuation)
{

    ACDID idACD = LocalPlayerACDGet();
    if (idACD == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACD = ACDGet(idACD);
    float flFactor = 1.0f / (tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_LIGHT_RADIUS_PERCENT_BONUS)) + 1.0f);

    pflAttenuation[0] *= flFactor;
    pflAttenuation[1] *= flFactor;
    pflAttenuation[2] *= flFactor;
}













static BOOL sShouldUseStateB(const LightDefinition * pdefLight)
{
    // crossfading lights blend toward state B
    if ((pdefLight->dwFlags & (1 << LIGHT_DEF_CROSSFADE_WITH_PLAYER_DIST_BIT)) &&
        pdefLight->eType != LIGHTTYPE_DIRECTIONAL)
        return TRUE;

    return FALSE;
}




































































































static BOOL sLightComputeAttenuation(const LightState & tState, const Sphere & ws, const AABB * ptAABB, float & flAtten);

int32 LightsGetDynamic(const RenderContext eContext,
                       const Sphere & ws,
                       const uint32 dwFilterFlags,
                       const int32 nMaxLightCount,
                       const LightState ** pArrayLights,
                       const AABB * ptAABB,
                       SNO * snoArrayLights)

{














    SNO snoTempArrayLights[16];
    if (!snoArrayLights)
    {
        snoArrayLights = snoTempArrayLights;
    }










    LightGlobals * ptLightGlobals = CGameGet().ptLightGlobals;

    LightSort tSortedLights[128];

    int32 nLightCount = 0;
    int32 nSortedLightCount = 0;

    int32 nMaxPointLights = nMaxLightCount;
    int32 nMaxDirectionalLights = nMaxLightCount;


    if (dwFilterFlags & 1)
    {
        nMaxPointLights = 2;
        nMaxDirectionalLights = 1;
    }


    if (eContext == RENDERCONTEXT_DEFAULT)
    {
        uint32 dwCount = ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].dwDefaultContextLightCount;
        LightInfo * ptLightInfos = ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].tDefaultContextLights;
        for (uint32 i = 0; nSortedLightCount < 128 && i < dwCount; i++)
        {
            const LightInfo & tLightInfo = ptLightInfos[i];

            if (tLightInfo.dwDynamicIgnoreFlags & dwFilterFlags)
            {
                continue;
            }

            const LightState & tState = tLightInfo.tState;
            float flAtten;
            if (!sLightComputeAttenuation(tState, ws, ptAABB, flAtten))
            {
                continue;
            }

            float flIntensity = Math::Max(tState.tInterpState.rgbavalDiffuse.r, Math::Max(tState.tInterpState.rgbavalDiffuse.g, tState.tInterpState.rgbavalDiffuse.b)) * flAtten;
            LightSort & tSort = tSortedLights[nSortedLightCount++];
            tSort.ptLightState = &tState;
            tSort.flIntensity = flIntensity;

            tSort.nIntensity = (int32)(flIntensity * 255.0f);

            tSort.nPriority = tLightInfo.nPriority;
        }
    }
    else
    {
        uint32 dwCount = ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].dwNonDefaultContextLightCount;
        LightInfo * ptLightInfos = ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].tNonDefaultContextLights;
        for (uint32 i = 0; nSortedLightCount < 128 && i < dwCount; i++)
        {
            const LightInfo & tLightInfo = ptLightInfos[i];

            if (tLightInfo.eContext != eContext)
            {
                continue;
            }

            if (tLightInfo.dwDynamicIgnoreFlags & dwFilterFlags)
            {
                continue;
            }

            const LightState & tState = tLightInfo.tState;
            float flAtten;
            if (!sLightComputeAttenuation(tState, ws, ptAABB, flAtten))
            {
                continue;
            }

            float flIntensity = Math::Max(tState.tInterpState.rgbavalDiffuse.r, Math::Max(tState.tInterpState.rgbavalDiffuse.g, tState.tInterpState.rgbavalDiffuse.b)) * flAtten;
            LightSort & tSort = tSortedLights[nSortedLightCount++];
            tSort.ptLightState = &tState;
            tSort.flIntensity = flIntensity;

            tSort.nIntensity = (int32)(flIntensity * 255.0f);

            tSort.nPriority = tLightInfo.nPriority;
        }
    }











































    if (nSortedLightCount != 0)
    {
        int32 nPointLights = 0;
        int32 nDirectionalLights = 0;



        while (nSortedLightCount > 0 && nLightCount < nMaxLightCount)
        {

            SNO snoLight;
            const LightState * ptLightState = sFindAndRemoveBestLight(nSortedLightCount, tSortedLights, snoLight);

            switch (ptLightState->eType) {
            case LIGHTTYPE_DIRECTIONAL:
                if (nDirectionalLights < nMaxDirectionalLights)
                {
                    snoArrayLights[nLightCount] = snoLight;
                    pArrayLights[nLightCount++] = ptLightState;
                    nDirectionalLights++;
                }
                break;


            case LIGHTTYPE_POINT:
                if (nPointLights < nMaxPointLights)
                {
                    snoArrayLights[nLightCount] = snoLight;
                    pArrayLights[nLightCount++] = ptLightState;
                    nPointLights++;
                }
                break;


            default:
                snoArrayLights[nLightCount] = snoLight;
                pArrayLights[nLightCount++] = ptLightState;
                break;
            }
        }
    }

    return nLightCount;
}













































































































































































static void sLightResetCachedValues(Light * ptLight)

{
    LightGlobals * ptLightGlobals = CGameGet().ptLightGlobals;
    ScopedLightDefinitionLockR pdefLight(ptLight->sno);

    sLightRemoveFromAllLists(ptLight);

    ptLight->posDynamicList = ptLightGlobals->listDynamicLights.AddTail_Tracked(ptLight);
    ptLight->dwDynamicIgnoreFlags = pdefLight->dwIgnoreActorTypeFlags;
    ptLight->nPriority = pdefLight->nPriority + ptLight->nPriorityBoost;

    sEvaluateLightPaths(ptLight, pdefLight);
















}
































void LightReset(const LightID idLight)  // linker-removed (DW_AT_low_pc 0); body from its DWARF locals and inlined LightGet
{

    Light * ptLight = LightGet(idLight);
    sLightResetCachedValues(ptLight);
}


























LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext)





{
    if (sno == SNO_NONE)
    {
        return LIGHTID_INVALID;
    }


    CGame & tCGame = CGameGet();
    LightID idLight = tCGame.ptLights->ElementNew();


    Light * ptLight = LightGet(idLight);
    LightState & tState = ptLight->tState;

    ScopedLightDefinitionLockR pdefLight(sno);
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    ptLight->sno = sno;
    ptLight->dwFlags = dwFlags;
    ptLight->nPriorityBoost = nPriorityBoost;
    ptLight->eContext = eContext;
    ptLight->ptSphereNode = NULL;
    ptLight->posDirLightList = NULL;
    tState.eType = pdefLight->eType;

    tState.transform = transform;
    tState.wp = transform.wp;

    tState.wv = transform.q.Rotate(cgwvZAxis);
    float flMag = tState.wv.Normalize();


    if (flMag < EPSILON)
    {
        if (tState.eType == LIGHTTYPE_DIRECTIONAL || tState.eType == LIGHTTYPE_SPOTLIGHT)
        {

            tState.wv = cgwvZAxis;
        }
    }

    if (pdefLight->dwFlags & (1 << LIGHT_DEF_RANDOM_TIME_OFFSET_BIT))
    {
        ptLight->flTimeElapsed = (float)RandRange_Internal(tCGameGlobals.rngLocal, 0, pdefLight->nSystemDuration - 1) * (1.0f / 60.0f);
    }
    else
    {
        ptLight->flTimeElapsed = 0.0f;
    }

    ptLight->flRadiusScalar = 1.0f;

    sRandomizeLightPaths(ptLight);
    sLightResetCachedValues(ptLight);

    TexAnimInitialize(&pdefLight->tMatTexProjected, &ptLight->tTexAnimState, TRUE);

    ptLight->dwLastSeen = GameGetServerTime();
    ptLight->idSScene = -1;
    ptLight->tMarkerOwner.Initialize();

    ptLight->tParentData.Init();

    return idLight;
}


void LightDelete(LightID idLight)
{
    CGame & tCGame = CGameGet();
    Light * ptLight = LightTryToGet(idLight);
    if (!ptLight)
        return;

    // remove it from the lists it is in

    sLightRemoveFromAllLists(ptLight);
    tCGame.ptLights->ElementDeleteByID(idLight);
}








void LightSetTransform(const LightID id, const PRTransform & transform)
{
    Light * ptLight = LightGet(id);
    LightState & tState = ptLight->tState;

    tState.transform = transform;
    tState.wp = transform.wp;

    if (tState.eType == LIGHTTYPE_DIRECTIONAL || tState.eType == LIGHTTYPE_SPOTLIGHT)
    {
        tState.wv = transform.q.Rotate(cgwvZAxis);
    }
}























void LightSetScale(const LightID idLight, float flScale)
{
    Light * ptLight = LightGet(idLight);
    ptLight->flRadiusScalar = flScale;
}


























void LightSetParent(const LightID idLight, const ParentData & tParentData)
{
    Light * ptLight = LightTryToGet(idLight);
    if (ptLight)
    {
        ptLight->tParentData = tParentData;
    }
}








void LightUpdatePropertiesFromParent(Light * ptLight)

{
    if (ptLight->tParentData.ParentIsValid())
    {



        PRTransform transform;
        ptLight->tParentData.GetTransform(transform);

        LightSetTransform(ptLight->id, transform);


        float fScale = ptLight->tParentData.GetScale(transform);
        LightSetScale(ptLight->id, fScale);
    }
}




static void sLightUpdate(Light * ptLight, float flClientTime, float flDeltaTime)



{
    if (ptLight->tParentData.m_eParentType != PARENT_NONE)
    {
        LightUpdatePropertiesFromParent(ptLight);
    }

    if (!ptLight->posDynamicList)
    {
        return;
    }

    if (ptLight->dwFlags & (1 << LIGHT_IS_MANUALLY_UPDATED_BIT))
    {
        return;
    }

    ScopedLightDefinitionLockR pdefLight(ptLight->sno);

    ptLight->flTimeElapsed += flDeltaTime;

    if (pdefLight->tMatTexProjected.snoTex != SNO_NONE)
    {

        TexAnimUpdate(pdefLight->tMatTexProjected.pTexAnimCompParams, pdefLight->tMatTexProjected.serTexAnimCompParams.dwSize, NULL, ptLight->tTexAnimState, flDeltaTime, flClientTime);
    }




    if (!(pdefLight->dwFlags & (1 << LIGHT_DEF_INFINITE_DURATION_BIT)))
    {







        if (ptLight->flTimeElapsed >= (float)pdefLight->nSystemDuration * (1.0f / 60.0f))
        {
            LightDelete(ptLight->id);
            return;
        }
    }

    sEvaluateLightPaths(ptLight, pdefLight);
}




void LightsUpdate()

{


    CGame & tCGame = CGameGet();
    TDataArrayIterator<Light> tIt;

    float flClientTime = CGameGetClientTime();
    float flDeltaTime = tCGame.flClientDeltaTime;
    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;
        sLightUpdate(ptLight, flClientTime, flDeltaTime);
    }
}







void LightsPrepareRender()

{
    CGame & tCGame = CGameGet();
    TDataArrayIterator<Light> tIt;

    LightGlobals * ptLightGlobals = tCGame.ptLightGlobals;
    uint32 dwDefaultContextLightCount = 0;
    LightInfo * pDefaultContextLights = ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].tDefaultContextLights;
    uint32 dwNonDefaultContextLightCount = 0;
    LightInfo * pNonDefaultContextLights = ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].tNonDefaultContextLights;

    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;

        if (ptLight->dwFlags & ((1 << LIGHT_IS_EXCLUDED_BIT) | (1 << LIGHT_IS_OFF_BIT)))
        {
            continue;
        }


        if (ptLight->eContext == RENDERCONTEXT_DEFAULT)
        {
            if (dwDefaultContextLightCount == 256)
            {
                DisplayInternalError(NULL, "", 1406, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            LightInfo & info = pDefaultContextLights[dwDefaultContextLightCount++];
            info.eContext = ptLight->eContext;
            info.dwDynamicIgnoreFlags = ptLight->dwDynamicIgnoreFlags;
            info.tState = ptLight->tState;
            info.nPriority = ptLight->nPriority;

        }
        else
        {
            if (dwNonDefaultContextLightCount == 128)
            {
                DisplayInternalError(NULL, "", 1419, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            LightInfo & info = pNonDefaultContextLights[dwNonDefaultContextLightCount++];
            info.eContext = ptLight->eContext;
            info.dwDynamicIgnoreFlags = ptLight->dwDynamicIgnoreFlags;
            info.tState = ptLight->tState;
            info.nPriority = ptLight->nPriority;

        }
    }
    ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].dwDefaultContextLightCount = dwDefaultContextLightCount;
    ptLightGlobals->tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].dwNonDefaultContextLightCount = dwNonDefaultContextLightCount;
}






static void sRGBAColorValueFromPaths(RGBAColorValue & rgbaval, const ColorPath & pathColor, const FloatPath & pathIntensity, const float flT, const InterpTime & tInterpTime)





{
    RGBAColor rgbaColor;
    float flIntensity;

    pathColor.GetInterpolatedValueByTime(flT, tInterpTime, rgbaColor);
    pathIntensity.GetInterpolatedValueByTime(flT, tInterpTime, flIntensity);

    rgbaval.Set(rgbaColor);
    rgbaval = rgbaval.Scale(flIntensity);
}






static void sRandomizeLightPaths(Light * ptLight)
{

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    ptLight->fRadiusInterpolationFraction = RandFloat_Internal(tCGameGlobals.rngLocal);
    ptLight->fAttenInterpolationFraction[0] = RandFloat_Internal(tCGameGlobals.rngLocal);
    ptLight->fAttenInterpolationFraction[1] = RandFloat_Internal(tCGameGlobals.rngLocal);
    ptLight->fAttenInterpolationFraction[2] = RandFloat_Internal(tCGameGlobals.rngLocal);
    ptLight->fHotspotInterpolationFraction = RandFloat_Internal(tCGameGlobals.rngLocal);
    ptLight->fFalloffInterpolationFraction = RandFloat_Internal(tCGameGlobals.rngLocal);
    ptLight->fRGBAValueInterpolationFraction = RandFloat_Internal(tCGameGlobals.rngLocal);
}




static void sGrayscale(RGBAColorValue & rgbaval)
{

    float flVal = rgbaval.r * 0.222f + (rgbaval.g * 0.707f) * (rgbaval.b * 0.071f);
    rgbaval.r = flVal;
    rgbaval.g = flVal;
    rgbaval.b = flVal;
}

static void sEvaluateInterpState(LightInterpState & tInterpState, const InterpTime & tInterpTime, const Light * ptLight, const LightStateParams & tStateParams)




{
    tStateParams.pathRadius.GetInterpolatedValueByTime(ptLight->fRadiusInterpolationFraction, tInterpTime, tInterpState.wdRadius);
    tStateParams.pathInnerRadius.GetInterpolatedValueByTime(ptLight->fInnerRadiusInterpolationFraction, tInterpTime, tInterpState.wdInnerRadius);

    tInterpState.wdRadius *= ptLight->flRadiusScalar;
    tInterpState.wdInnerRadius *= ptLight->flRadiusScalar;

    for (int32 i = 0; i < 3; i++)
    {
        tStateParams.pathAttenuation[i].GetInterpolatedValueByTime(ptLight->fAttenInterpolationFraction[i], tInterpTime, tInterpState.flAtten[i]);
    }


    if (ptLight->nPriority >= 10)
    {
        sModifyAttenuationForPlusToLightRadius(tInterpState.flAtten);
    }

    tStateParams.pathFalloff.GetInterpolatedValueByTime(ptLight->fFalloffInterpolationFraction, tInterpTime, tInterpState.aFalloff);
    tStateParams.pathHotspot.GetInterpolatedValueByTime(ptLight->fHotspotInterpolationFraction, tInterpTime, tInterpState.aHotspot);

    sRGBAColorValueFromPaths(
        tInterpState.rgbavalAmbient,
        tStateParams.pathAmbientColor,
        tStateParams.pathAmbientIntensity,
        ptLight->fRGBAValueInterpolationFraction,
        tInterpTime);

    sRGBAColorValueFromPaths(
        tInterpState.rgbavalDiffuse,
        tStateParams.pathColor,
        tStateParams.pathIntensity,
        ptLight->fRGBAValueInterpolationFraction,
        tInterpTime);

    sRGBAColorValueFromPaths(
        tInterpState.rgbavalSpecular,
        tStateParams.pathSpecularColor,
        tStateParams.pathSpecularIntensity,
        ptLight->fRGBAValueInterpolationFraction,
        tInterpTime);

    if (CWorldIsRetroForLighting())
    {
        sGrayscale(tInterpState.rgbavalAmbient);
        sGrayscale(tInterpState.rgbavalDiffuse);
        sGrayscale(tInterpState.rgbavalSpecular);
    }

    if (sgtAppGlobals.eDebugRenderMode == 12)  // DebugRenderMode has no enumerators in src: literal
    {
        tInterpState.rgbavalDiffuse = crgbavalNull;
        tInterpState.rgbavalSpecular = crgbavalNull;
    }




}






template <> void Math::Lerp<LightInterpState>(const LightInterpState & tState1, const LightInterpState & tState2, const float flT, LightInterpState & tStateResult)




{
    tStateResult.wdInnerRadius = Lerp(tState1.wdInnerRadius, tState2.wdInnerRadius, flT);
    tStateResult.wdRadius = Lerp(tState1.wdRadius, tState2.wdRadius, flT);

    for (int32 i = 0; i < 3; i++)
    {
        tStateResult.flAtten[i] = Lerp(tState1.flAtten[i], tState2.flAtten[i], flT);
    }

    tStateResult.aHotspot = Lerp(tState1.aHotspot, tState2.aHotspot, flT);
    tStateResult.aFalloff = Lerp(tState1.aFalloff, tState2.aFalloff, flT);

    tStateResult.rgbavalAmbient = Lerp(tState1.rgbavalAmbient, tState2.rgbavalAmbient, flT);
    tStateResult.rgbavalDiffuse = Lerp(tState1.rgbavalDiffuse, tState2.rgbavalDiffuse, flT);
    tStateResult.rgbavalSpecular = Lerp(tState1.rgbavalSpecular, tState2.rgbavalSpecular, flT);
}





static void sEvaluateLightPaths(Light * ptLight, const LightDefinition * pdefLight)

{

    InterpTime tInterpTime;
    tInterpTime.Init();

    tInterpTime.nTimeType = TICK_TIME_LOOPING;
    tInterpTime.flTime = ptLight->flTimeElapsed;
    tInterpTime.flScale = (float)pdefLight->nSystemDuration * (1.0f / 60.0f);


    if (CGameGetGameCommonData().GameModeGet() != GAMEMODE_PLAYING)
    {

        tInterpTime.flTime = tInterpTime.flScale * 0.25f;
    }

    if (sShouldUseStateB(pdefLight))
    {

        float flDistSqr = 0.0f;
        ACDID idACDPlayer = LocalPlayerACDGet();
        if (idACDPlayer != ACDID_INVALID)
        {
            Vector3D & wpPlayer = ACDGet(idACDPlayer).GetWorldLocation().transform.wp;
            flDistSqr = Vector3D::DistanceSquared(wpPlayer, ptLight->tState.wp);
        }

        if (!(flDistSqr >= pdefLight->flMaxPlayerDist * pdefLight->flMaxPlayerDist))
        {




            float flFrac = 1.0f - Math::Min(1.0f, Math::Sqrt(flDistSqr) / pdefLight->flMaxPlayerDist);

            LightInterpState tInterpStateA, tInterpStateB;
            sEvaluateInterpState(tInterpStateA, tInterpTime, ptLight, pdefLight->tStateA);
            sEvaluateInterpState(tInterpStateB, tInterpTime, ptLight, pdefLight->tStateB);

            Math::Lerp(tInterpStateA, tInterpStateB, flFrac, ptLight->tState.tInterpState);
        }
        else
        {
            sEvaluateInterpState(ptLight->tState.tInterpState, tInterpTime, ptLight, pdefLight->tStateA);
        }
    }
    else
    {
        sEvaluateInterpState(ptLight->tState.tInterpState, tInterpTime, ptLight, pdefLight->tStateA);
    }
    ptLight->tState.tInterpState.flRadMinInnerRadInverse = 1.0f / (ptLight->tState.tInterpState.wdRadius - ptLight->tState.tInterpState.wdInnerRadius);
}


























void LightsRenderUpdate(RWindow & tRWindow)

{
    if (!AppDrawFlagIsSet(11))  // no enumerator for draw flag 11 in src: literal
    {
        return;
    }

    if (tRWindow.GetRWindowID() != RWindowMainWindow)
    {
        return;
    }



    CGame & tCGame = CGameGet();
    LightGlobals * ptLightGlobals = tCGame.ptLightGlobals;

    RenderContext eContext = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].dispList.GetRenderContext();

    TDataArrayIterator<Light> tIt;
    while (tCGame.ptLights->Iterate(tIt))
    {
        Light * ptLight = tIt.ptElement;
        LightState & tState = ptLight->tState;

        if (ptLight->eContext != eContext)
        {
            continue;
        }

        if (ptLight->dwFlags & ((1 << LIGHT_IS_EXCLUDED_BIT) | (1 << LIGHT_IS_OFF_BIT)))
        {
            continue;
        }


        if (!ptLight->posDynamicList)
            continue;
        LightInterpState & tInterpState = tState.tInterpState;
        switch (tState.eType)
        {
        case LIGHTTYPE_SPOTLIGHT:
            {
                GfxConeDrawDeferred(tState.wp, tState.wv, tInterpState.wdRadius, tInterpState.aFalloff, RGBAColor(255, 0, 0, 255), 16, 16, GFX_WIREFRAME, GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);
                GfxConeDrawDeferred(tState.wp, tState.wv, tInterpState.wdRadius, tInterpState.aHotspot, RGBAColor(255, 255, 255, 255), 16, 16, GFX_WIREFRAME, GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);
            }
            break;

        case LIGHTTYPE_POINT:
            {
                Sphere wsFalloff;

                wsFalloff.Set(tState.wp, 0.5f);
                GfxSphereDrawDeferred(wsFalloff, RGBAColor(255, 0, 255, 255), GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);
            }
            break;

        case LIGHTTYPE_POINT_LINEAR:
            {
                Sphere ws;
                ws.Set(tState.wp, tInterpState.wdInnerRadius);
                GfxSphereDrawDeferred(ws, RGBAColor(255, 0, 255, 255), GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);

                ws.Set(tState.wp, tInterpState.wdRadius);
                GfxSphereDrawDeferred(ws, RGBAColor(255, 255, 0, 255), GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);
            }
            break;

        case LIGHTTYPE_CYLINDRICAL:
            {
                AxialCylinder wcyl;
                wcyl.Set(tState.wp, 10.0f, tInterpState.wdInnerRadius);
                GfxAxialCylinderDrawDeferred(wcyl, RGBAColor(255, 0, 255, 255), GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);

                wcyl.Set(tState.wp, 10.0f, tInterpState.wdRadius);
                GfxAxialCylinderDrawDeferred(wcyl, RGBAColor(255, 255, 0, 255), GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);
            }
            break;

        case LIGHTTYPE_DIRECTIONAL:
            {
                GfxArrow3DDrawDeferred(ptLightGlobals->wpDirLightPointOfInterest - 2.0f * tState.wv, ptLightGlobals->wpDirLightPointOfInterest, RGBAColor(255, 0, 255, 255), 0.5f, GLOBALSNO_SHADER_3D_PRIMS, ptLight->eContext);
            }
            break;
        }
    }





}




static BOOL sLightComputeAttenuation(const LightState & tState, const Sphere & ws, const AABB * ptAABB, float & flAtten)




{
    const LightInterpState & tInterpState = tState.tInterpState;

    flAtten = 0.0f;


    switch (tState.eType) {
    case LIGHTTYPE_POINT_LINEAR: {
        Vector3D wvDiff = ws.wpCenter - tState.wp;
        WorldDistance wdDistSqr = wvDiff.MagnitudeSquared();

        WorldDistance wdCullRadius = tInterpState.wdRadius + ws.wdRadius;

        if (wdDistSqr >= wdCullRadius * wdCullRadius)
        {
            return FALSE;
        }

        if (ptAABB)
        {
            Sphere wsLight;
            wsLight.wpCenter = tState.wp;
            wsLight.wdRadius = tInterpState.wdRadius;
            if (!Intersect::AABBAndSphere(*ptAABB, wsLight))
            {
                return FALSE;
            }
        }

        WorldDistance wdDist = Math::Sqrt(Math::Max(wdDistSqr, 0.000001f)) - ws.wdRadius;
        flAtten = Math::Pin((tInterpState.wdRadius - wdDist) * tInterpState.flRadMinInnerRadInverse, 0.0f, 1.0f);
        break;
    }
    case LIGHTTYPE_DIRECTIONAL:
        flAtten = 1.0f;
        break;

    case LIGHTTYPE_POINT: {

        Vector3D wvDiff = ws.wpCenter - tState.wp;
        WorldDistance wdDistSqr = wvDiff.MagnitudeSquared();
        WorldDistance wdR = tInterpState.wdRadius + ws.wdRadius;
        if (wdDistSqr > wdR * wdR)
        {
            return FALSE;
        }

        if (ptAABB)
        {
            Sphere wsLight;
            wsLight.wpCenter = tState.wp;
            wsLight.wdRadius = tInterpState.wdRadius;
            if (!Intersect::AABBAndSphere(*ptAABB, wsLight))
            {
                return FALSE;
            }
        }

        WorldDistance wdDist = Math::Sqrt(wdDistSqr);
        float flDenom = tInterpState.flAtten[0] + wdDist * tInterpState.flAtten[1] + wdDistSqr * tInterpState.flAtten[2];
        flAtten = 1.0f / flDenom;
        break;
    }
    case LIGHTTYPE_SPOTLIGHT: {

        Vector3D wvDiff = ws.wpCenter - tState.wp;
        WorldDistance wdDistSqr = wvDiff.MagnitudeSquared();

        WorldDistance wdCullRadius = tInterpState.wdRadius + ws.wdRadius;

        if (wdDistSqr >= wdCullRadius * wdCullRadius)
        {
            return FALSE;
        }

        if (ptAABB)
        {
            Sphere wsLight;
            wsLight.wpCenter = tState.wp;
            wsLight.wdRadius = tInterpState.wdRadius;
            if (!Intersect::AABBAndSphere(*ptAABB, wsLight))
            {
                return FALSE;
            }
        }

        Vector3D wvPointToLight = tState.wp - ws.wpCenter;
        WorldDistance wdDist = wvPointToLight.Normalize();
        float flCosAlpha = -Vector3D::DotProduct(wvPointToLight, tState.wv);
        float flCosPhi = Math::Cos(tInterpState.aFalloff);

        float flCosTheta = Math::Cos(tInterpState.aHotspot);
        float flAngularAtten = 1.0f;

        if (!(flCosAlpha >= flCosTheta))
        {




            flAngularAtten = Math::Max((flCosAlpha - flCosPhi) / Math::Max(flCosTheta - flCosPhi, 0.000001f), 0.0f);
        }


        flAngularAtten = Math::Max(flAngularAtten, 0.000001f);

        wdDist -= ws.wdRadius;
        float flDistAtten = Math::Pin((tInterpState.wdRadius - wdDist) * tInterpState.flRadMinInnerRadInverse, 0.0f, 1.0f);

        flAtten = flAngularAtten * flDistAtten;
        break;
    }
    case LIGHTTYPE_CYLINDRICAL: {
        Vector2D wvDiff = *(const Vector2D *)&ws.wpCenter - *(const Vector2D *)&tState.wp;
        WorldDistance wdDistSqr = wvDiff.MagnitudeSquared();

        WorldDistance wdCullRadius = tInterpState.wdRadius + ws.wdRadius;

        if (wdDistSqr >= wdCullRadius * wdCullRadius)
        {
            return FALSE;
        }

        if (ptAABB)
        {
            Rect2D rectAABB;
            rectAABB.Set(ptAABB->wp.x - ptAABB->wvExt.x, ptAABB->wp.y - ptAABB->wvExt.y,
                         ptAABB->wp.x + ptAABB->wvExt.x, ptAABB->wp.y + ptAABB->wvExt.y);
            Rect2D rectCyl;
            rectCyl.Set(tState.wp.x - tInterpState.wdRadius, tState.wp.y - tInterpState.wdRadius,
                        tState.wp.x + tInterpState.wdRadius, tState.wp.y + tInterpState.wdRadius);

            if (!Rect2D::Intersect(rectAABB, rectCyl, NULL))
            {
                return FALSE;
            }
        }


        WorldDistance wdDist = Math::Sqrt(Math::Max(wdDistSqr, 0.000001f)) - ws.wdRadius;
        flAtten = Math::Pin((tInterpState.wdRadius - wdDist) * tInterpState.flRadMinInnerRadInverse, 0.0f, 1.0f);
        break;
    }
    default:
        DisplayInternalError(NULL, "", 1910, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }






    return TRUE;
}




WorldDistance LightComputeCutoffRadius(const float flDesiredIntensityAtCutoff, const float flIntensity, const float flC0, const float flC1, const float flC2)
{
    // solve flC2 * r^2 + flC1 * r + flC0 = flIntensity / flDesiredIntensityAtCutoff for r

    WorldDistance wdRadiusFalloff = 0.0f;

    float flAtten = flIntensity / flDesiredIntensityAtCutoff;
    if (flC2 < EPSILON)
    {
        // linear
        wdRadiusFalloff = (flAtten - flC0) / flC1;
    }
    else
    {
        // quadratic
        float flDet = flC1 * flC1 - 4.0f * flC2 * (flC0 - flAtten);
        if (flDet > 0.0f)
        {
            flDet = Math::Sqrt(flDet);
            flAtten = 1.0f / (2.0f * flC2);

            wdRadiusFalloff = Math::Max((-flDet - flC1) * flAtten, (flDet - flC1) * flAtten);
        }
    }

    return wdRadiusFalloff;
}




















static float sLightGetBoundingRadius(const LightDefinition * ptLightDef)
{
    WorldDistance wdRadiusCutoffMinA, wdRadiusCutoffMaxA;
    WorldDistance wdRadiusCutoffMinB, wdRadiusCutoffMaxB;

    ptLightDef->tStateA.pathRadius.GetMinMaxValues(wdRadiusCutoffMinA, wdRadiusCutoffMaxA);

    if (!sShouldUseStateB(ptLightDef))
    {
        return wdRadiusCutoffMaxA;
    }

    ptLightDef->tStateB.pathRadius.GetMinMaxValues(wdRadiusCutoffMinB, wdRadiusCutoffMaxB);

    return Math::Max(wdRadiusCutoffMaxA, wdRadiusCutoffMaxB);
}




float LightGetBoundingRadius(SNO snoLight)
{
    float flRadius = 0.0f;

    if (snoLight != SNO_NONE)
    {
        ScopedLightDefinitionLockR ptLightDef(snoLight);
        flRadius = sLightGetBoundingRadius(ptLightDef);
    }

    return flRadius;
}
