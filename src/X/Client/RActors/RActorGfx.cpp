// X/Client/RActors/RActorGfx.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/p2-ractor/gfx/frag.cpp by build/try/p2-ractor/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "RActors.h"
#include "../CGame.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RGBAColor.h"
#include "../Definitions/TextureDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Player.h"
#include "../CPlayer.h"
#include "../../Sigma/Random.h"

// X/Client/Definitions/TextureDefinition.cpp
void TextureDefinitionPixelLookup(const SNO snoTexture, const float flU, const float flV, const int32 nMipLevel, RGBAColor & rgbaColor);

void RActorGfxPerformLOSCheck(const RActor ** pptRActorSources, int32 nSourceCount, RActor & tRActorTarget);

struct UpdateRActorsLOSData {  // size 0x204
    static const uint32 MAX_ACTORS = 128;
    RActorID aidActors[128];  // 0x00
    uint32 uActorsCount;  // 0x200
    static void Func(const void * pDataIn);
};
static_assert(sizeof(UpdateRActorsLOSData) == 0x204, "UpdateRActorsLOSData");

struct RActorGfxGlobals {  // size 0x18
    RActorIDListPooled * plistFrustumActors;  // 0x00
    RActorIDListPooled * plistShadowFrustumActors;  // 0x08
    BOOL bNecroCorpseInFrustum;  // 0x10
};
static_assert(sizeof(RActorGfxGlobals) == 0x18, "RActorGfxGlobals");



// Declarations the original gets from headers not yet adopted (DWARF decl files in comments).
#include "../Definitions/AppearanceDefinition.h"
#include "../../Sigma/Math/Intersect.h"
#include "../SubObjectGfx.h"
#include "RActorAnimation.h"
#include "../RWindow.h"
#include "../Camera/CameraManager.h"
#include "../Cloth.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../UserInterface/Interface/UIGame.h"
#include "../Attachments.h"
#include "../Particle.h"
#include "../Rope.h"
#include "../Camera/Camera.h"
#include "../Definitions/ShaderDefinition.h"
#include "../../Common/JobManager/JobManager.h"
// X/Client/Definitions/AppearanceDefinition.cpp
SubObjectAppearance & AppearanceGetSubObjectAppearance(const AppearanceDefinition * ptDefinition, int32 nSubObjectLook, const int32 nSubObject, const GeoSetType eGeoSetType);
// X/Client/SubObjectGfx.cpp
void SubObjectStateUpdate(const SubObjectAppearance & tSOA, SubObjectState & tSubObjectState, const ObjectTexAnimParam * ptObjTexAnimParam, const float flDeltaTime, const float flClientTime);
// X/Client/Lights.cpp
int32 LightsGetDynamic(const RenderContext eContext, const Sphere & ws, const uint32 dwFilterFlags, const int32 nMaxLightCount, const LightState ** pArrayLights, const AABB * ptAABB, SNO * snoArrayLights);
// X/Client/Attachments.cpp:106
Attachment * AttachmentTryToGet(const AttachmentID idAttachment);
float s_flResolutionFactor = 100.0f;
void RopeRenderInDisplayList(Rope * ptRope, const Camera & cam, DisplayList & tDisplayList, const float flDeltaTime);  // X/Client/Rope.cpp:2022
// X/Client/Particle.cpp
void ParticlesMultiRenderUpdateKick();
// X/Client/Gfx/GfxMaterial.cpp
ShaderTex OverlayTypeToShaderTex(const OverlayType eOverlayType);
// X/Client/TexAnimation.cpp
TexAnimState * TexAnimRenderAsyncCopy(const int32 nTexAnimStateCount, const TexAnimState * arTexAnimStates, const UberMaterial * ptUberMaterial, const ShaderTex eOverlayShaderTex);
// X/Client/SubObjectGfx.cpp
BOOL SubObjectGfxOcclusionHighlightingIsEnabled();
static const float TIME_BETWEEN_LOS_UPDATES = 1.0f;
#include "../Definitions/ShaderMapDefinition.h"
#include "../Prefetch.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/VertexShaderTags.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../PostFX.h"

static const float FADE_RATE_NEAR = 1.0f / 15.0f;

// X/Common/ActorCommonData.cpp:9215
BOOL ACDCanBeHighlighted(ActorCommonData * ptACD);
// X/Client/RActors/RActors.cpp:11815
BOOL RActorIsHostile(const RActor * ptRActor);

typedef float Matrix3x4[12];  // DWARF RActorGfx.cpp:89 (typedef float[12]; restrict pointers to it in sDisplayListSubmitRActorSubObject)
// X/Client/SubObjectGfx.cpp:3838
void SubObjectGfxOcclusionHighlightingSetEstimatedPixelCount(OcclusionHighlighting * ptManager, uint32 nPixelCount);
// X/Client/Camera/Camera.cpp:135
BOOL CameraComputeSphereVisibility(const Camera & cam, const Sphere & ws);
// X/Client/RActors/RActors.cpp:5981
RGBAColorValue RActorGetMeshLighting(const RActorID idRActor);













struct SubObjectSkinInfos
{
    int32 nSkinInfoCount;
    float * pmMatrixPalette[16];
};
// Forward declarations; definitions belong at RActorGfx.cpp:1474 and :1485 (DWARF).

static float sRenderUpdateActorVisibility(RActor * ptRActor, DisplayList & dispList, const float flDeltaTime);
#include "../../Common/WorldPlace.h"
#include "../Physics/DominoClient.h"
#include "../../Common/ActorCommonCollision.h"
#include "../../Common/WorldCommonCollision.h"
// X/Common/Nav/NavigationMesh.cpp:3573
BOOL NavMeshShouldCheckLOS(const NavMeshHandle & tNavMeshHandle);
// X/Common/ActorCommonCollision.cpp:486
BOOL ACDCollBroadLine(SWorldID idSWorld, const Vector2D & wp0, const Vector2D & wp1, const ACDCollParams & tCollParams, ACDCollBroadResult & tResult);
// X/Common/ActorCommonCollision.cpp:781
BOOL ACDCollNarrowSweptPoint(const ACDCollBroadResult & tCollBroadResult, const Vector2D & wp0, const Vector2D & wp1, uint32 uFlags, ACDID idACDIgnore, ACDCollNarrowResult & tResult);

// X/Client/RActors/RActorCollision.cpp:932
void RActorsIntersectFrustum(const Frustum & tFrustum, RActorIDListPooled & listVisibleActors);
// X/Client/RActors/RActors.cpp:12137
BOOL RActorIsHidden(const RActor * ptRActor);
// X/Client/RActors/RActors.cpp:11869
BOOL RActorIsNecroCorpse(const RActor * ptRActor);
// X/Client/RActors/RActors.cpp:2253
void RActorAttachSpawnedActor(RActor * ptRActor);
// X/Client/RActors/RActorAttachments.cpp:811
BOOL RActorIsAttachment(const RActor & tRActor);
// X/Client/RWindow.cpp:40
RWindowID RWindowIDFromContext(const RenderContext eContext);
#include "../../Common/TagMaps/ActorTags.h"














void RActorGfxInitializeForGame()
{

    RActorGfxGlobals *& ptRActorGfxGlobals = CGameGet().ptRActorGfxGlobals;
    ptRActorGfxGlobals = (RActorGfxGlobals *)SigmaMemoryNew(sizeof(RActorGfxGlobals), 4, NULL, TRUE);

    ptRActorGfxGlobals->plistFrustumActors = new RActorIDListPooled(MemoryPoolsGetDATAIDListPool());
    ptRActorGfxGlobals->plistShadowFrustumActors = new RActorIDListPooled(MemoryPoolsGetDATAIDListPool());





    ptRActorGfxGlobals->bNecroCorpseInFrustum = FALSE;
}




void RActorGfxDisposeForGame()
{

    RActorGfxGlobals *& ptRActorGfxGlobals = CGameGet().ptRActorGfxGlobals;

    delete ptRActorGfxGlobals->plistFrustumActors; ptRActorGfxGlobals->plistFrustumActors = NULL;
    delete ptRActorGfxGlobals->plistShadowFrustumActors; ptRActorGfxGlobals->plistShadowFrustumActors = NULL;





    SigmaMemoryFree(ptRActorGfxGlobals, NULL); ptRActorGfxGlobals = NULL;

}




static BOOL sShaderIsHardwareSkinned(
    const SNO snoShader,
    const ShaderDefinition * pdefShader)
{

    BOOL fLocalLock = FALSE;
    if (!pdefShader)
    {
        pdefShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShader);
        // Inference: fLocalLock (DWARF 198) has no rows; it is read only by code that leaves none (lines 222-230).
        fLocalLock = TRUE;
    }
















    BOOL fHardwareSkinned = pdefShader->ptPasses[0].bPostProcessed && (pdefShader->ptPasses[0].tPostprocessed.ptVSD->dwFlags & (1 << VSPP_DOES_HARDWARE_SKINNING_BIT));








    return fHardwareSkinned;
}
static BOOL sShaderDoesHardwareSkinning(
    const ShaderDefinition * pdefShader)
{
    if (pdefShader->nPassCount > 0)
    {
        RenderPass * ptPass = pdefShader->ptPasses;
// Lines 238-248 leave no code; the DWARF block of ptPass runs 237-249.










        return ptPass->tPostprocessed.ptVSD->dwFlags & (1 << VSPP_DOES_HARDWARE_SKINNING_BIT);
    }

    return FALSE;
}




static SNO sSelectShader(
    const float flAlpha,
    const SubObjectAppearance & tSOA,
    const RActor * ptRActor,
    const int32 nSubObjectIndex,
    const SubObjectState & tSubObjectState,
    const AppearanceDefinition * pdefAppearance,
    const ShaderMapDefinition * pdefShaderMap)
{



    SNO snoShader;
// Lines 271-276 and 286-461 leave no code (the original has only the two rows 277 and 285).





    if (flAlpha < 1.0f)
    {
        snoShader = tSubObjectState.snoFadeShader;
    }
    else
    {
        snoShader = tSubObjectState.snoDefaultShader;
    }
    return snoShader;
}

















































































































































































static SNO sSelectPrepassShader(
    const ShaderDefinition * pdefShader,
    const ShaderMapDefinition * pdefShaderMap)
{
    SNO snoPrepassShader = SNO_NONE;

    BOOL fSkinned = sShaderDoesHardwareSkinning(pdefShader);
    snoPrepassShader = ShaderMapGetPrepassShader(pdefShaderMap, fSkinned);

    if (snoPrepassShader == SNO_NONE)
    {
        if (pdefShader->nPassCount == 1)
        {
            RenderPass * ptPass = pdefShader->ptPasses;
            if ((ptPass->tRenderParams.fAlphaTestEnable == FALSE) &&
                (ptPass->tRenderParams.fAlphaBlendEnable == FALSE) &&
                (ptPass->tRenderParams.fZBufferWriteEnable == TRUE) &&
                (ptPass->tRenderParams.eCullMode == CULLMODE_CW) &&
                (
                ((ptPass->eRenderLayer == RENDERLAYER_FLOOR) ||
                 (ptPass->eRenderLayer == RENDERLAYER_OPAQUE) ||
                 (ptPass->eRenderLayer == RENDERLAYER_DEPTHPASS_DEPRECATED))))
            {

                BOOL fVSIsHardwareSkinned = ptPass->tPostprocessed.ptVSD->dwFlags & (1 << VSPP_DOES_HARDWARE_SKINNING_BIT);

                if (fVSIsHardwareSkinned)
                {
                    snoPrepassShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_PREPASS_SKIN);
                }
                else
                {
                    snoPrepassShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_PREPASS);
                }
            }
        }
    }

    return snoPrepassShader;
}





static SNO sSelectMaskShader(
    const ShaderDefinition * pdefShader,
    const ShaderMapDefinition * pdefShaderMap,
    const BOOL fHasAnimation,
    const BOOL fTinted,
    const BOOL fIsCloth)
{
    SNO snoShader = SNO_NONE;

    BOOL fSkinned = sShaderDoesHardwareSkinning(pdefShader);
    snoShader = ShaderMapGetMaskShader(pdefShaderMap, fSkinned);

    if (snoShader == SNO_NONE)
    {
        if (fHasAnimation && !fIsCloth)
        {
            if (fTinted)
            {
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_MASK_SKIN_TINT);
            }
            else
            {
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_MASK_SKIN);
            }
        }
        else
        {
            if (fTinted)
            {
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_MASK_TINT);
            }
            else
            {
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_MASK);
            }
        }
    }

    return snoShader;
}





static BOOL s_fHighlightColorsInitialized = FALSE;
RGBAColor s_tRgbaColorHostileA;
RGBAColor s_tRgbaColorHostileB;
RGBAColor s_tRgbaColorFriendlyA;
RGBAColor s_tRgbaColorFriendlyB;




void sFetchHighlightColorsFromTexture(
    BOOL fIsHostile,
    RGBAColor & rgbaColorA,
    RGBAColor & rgbaColorB)
{
    // fetch the highlight colors from the texture once
    if (!s_fHighlightColorsInitialized)
    {
        // remember that we did it
        s_fHighlightColorsInitialized = TRUE;
        SNO snoTexture = GlobalSNOGet(GLOBALSNO_HIGHLIGHT_TEXTURE);


        ScopedTextureDefinitionLockR pdefTexture(snoTexture);

        float fTexU = 0.09375f;

        TextureDefinitionPixelLookup(snoTexture, fTexU, 0.25f, 0, s_tRgbaColorHostileA);
        TextureDefinitionPixelLookup(snoTexture, fTexU, 0.75f, 0, s_tRgbaColorHostileB);

        fTexU = 0.15625f;

        TextureDefinitionPixelLookup(snoTexture, fTexU, 0.25f, 0, s_tRgbaColorFriendlyA);
        TextureDefinitionPixelLookup(snoTexture, fTexU, 0.75f, 0, s_tRgbaColorFriendlyB);
    }

    if (fIsHostile)
    {
        rgbaColorA = s_tRgbaColorHostileA;
        rgbaColorB = s_tRgbaColorHostileB;
    }
    else
    {
        rgbaColorA = s_tRgbaColorFriendlyA;
        rgbaColorB = s_tRgbaColorFriendlyB;
    }







}



void sGetHightlightColor(const RActor * ptRActor, RGBAColor * rgbaColor)
{
    BOOL fIsHostile = RActorIsHostile(ptRActor);
    RGBAColor rgbaColorA;
    RGBAColor rgbaColorB;
    float meshLightingContribution;

    sFetchHighlightColorsFromTexture(fIsHostile, rgbaColorA, rgbaColorB);

    meshLightingContribution = RActorGetMeshLighting(ptRActor->id).AsGrayScale();

    *rgbaColor = Math::Lerp(rgbaColorA, rgbaColorB, meshLightingContribution);
    rgbaColor->a = 0xff;
}




static void sSelectHighlightShader(
    const RActor * ptRActor,
    const BOOL fIsCloth,
    const ShaderDefinition * pdefShader,
    SNO * snoShader,
    RGBAColor * rgbaColor,
    const ShaderMapDefinition * pdefShaderMap)
{
    sGetHightlightColor(ptRActor, rgbaColor);

    BOOL fVSIsHardwareSkinned = (ptRActor->ptActorAnim != NULL) && !fIsCloth;
    if (pdefShader->nPassCount > 0)
    {
        fVSIsHardwareSkinned = sShaderDoesHardwareSkinning(pdefShader);
    }

    *snoShader = ShaderMapGetHighlightShader(pdefShaderMap, fVSIsHardwareSkinned);
}






static void sSelectOccludedShader(
    const RActor * ptRActor,
    const BOOL fIsCloth,
    const ShaderDefinition * pdefShader,
    SNO * snoShader,
    RGBAColor * rgbaColor,
    BOOL bSelected,
    const ShaderMapDefinition * pdefShaderMap)
{
    BOOL fVSIsHardwareSkinned = (ptRActor->ptActorAnim != NULL) && !fIsCloth;
    if (pdefShader->nPassCount > 0)
    {
        fVSIsHardwareSkinned = sShaderDoesHardwareSkinning(pdefShader);
    }

    if (!bSelected)
    {
        rgbaColor->r = 0x80;
        rgbaColor->g = 0x80;
        rgbaColor->b = 0x80;
    }

    *snoShader = ShaderMapGetOccludedShader(pdefShaderMap, fVSIsHardwareSkinned);
}




static SNO sSelectOcclusionQueryShader(
    const ShaderDefinition * pdefShader)
{
    SNO snoPrepassShader = SNO_NONE;

    if (pdefShader->nPassCount > 0)
    {
        BOOL fVSIsHardwareSkinned = sShaderDoesHardwareSkinning(pdefShader);
        if (fVSIsHardwareSkinned)
        {
            snoPrepassShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_OCCLUSION_QUERY_SKIN);
        }
        else
        {
            snoPrepassShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_OCCLUSION_QUERY);
        }
    }

    return snoPrepassShader;
}






static void sSelectHighlightClearShader(
    const RActor * ptRActor,
    const BOOL fIsCloth,
    SNO * snoShader)
{
    if (ptRActor->ptActorAnim && !fIsCloth)
    {
        *snoShader = GlobalSNOGet(GLOBALSNO_SHADER_RACTOR_HIGHLIGHT_CLEAR_SKINNED);
    }
    else
    {
        *snoShader = GlobalSNOGet(GLOBALSNO_SHADER_RACTOR_HIGHLIGHT_CLEAR);
    }
}




static void sRActorGfxFadeUpdate(
    RActor * ptRActor,
    const float flDeltaTime)
{
    if (flDeltaTime == 0.0f) { return; }




    if (ptRActor->flHitFlashAddTimeRemaining > 0.0f)
    {
        ptRActor->flHitFlashAddTimeRemaining = Math::Max(0.0f, ptRActor->flHitFlashAddTimeRemaining - flDeltaTime);
    }
    if (ptRActor->flHitFlashMulTimeRemaining > 0.0f)
    {
        ptRActor->flHitFlashMulTimeRemaining = Math::Max(0.0f, ptRActor->flHitFlashMulTimeRemaining - flDeltaTime);
    }


    BOOL bFadingAway;
    float flAlphaChange;
    if (ptRActor->dwFlagsEx & (1 << RACTOREX_FADE_WHEN_NEAR_BIT))
    {


        bFadingAway = CameraManagerShouldNearPlaneFade(ptRActor->tLocation.tWorldLocation.transform.wp);










        flAlphaChange = flDeltaTime * FADE_RATE_NEAR;
        // Inference (761/769): fadd and the flAlpha load carry line 761, the Pin call is at 769, and the load precedes the select, so flAlphaChange holds the new alpha.
        flAlphaChange = ptRActor->flAlpha + (bFadingAway ? -flAlphaChange : flAlphaChange);






        ptRActor->flAlpha = Math::Pin(flAlphaChange, 0.0f, 1.0f);
    }
}





static void sDisplayListSubmitRActorSubObject(
    DisplayList & dispList,
    const RActor * ptRActor,
    SubObject & tSubObject,
    BOOL fIsCloth,
    Renderable & tRenderable,
    int32 * anRenderableDetailsIndices,
    int32 nRenderableDetailsCapacity,
    SubObjectSkinInfos * ptSkinInfosCache)
{
    RenderableDetails& tDetails = tRenderable.details;
    RenderableHeader& tHeader = tRenderable.header;
    const ShaderDefinition* pdefShader = tHeader.pdefShader;


    // Inference (793): the original passes no SNO (w1 is never set before LockRFast); the shader's own handle is the only SNO in reach.
    if (ptRActor->ptActorAnim && !fIsCloth && sShaderIsHardwareSkinned(pdefShader->tHeader.snoHandle, pdefShader))
    {

        ProcessedBoneSkin* __restrict ptProcessedBoneSkinList = ptRActor->ptActorAnim->ptProcessedBoneSkinList;



        int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);
        for (int32 nInfo = 0; nInfo < nSkinningInfoCount; ++nInfo)
        {
            tHeader.nGroup = nInfo;
            BOOL fIndexValid = nInfo + 1 < nRenderableDetailsCapacity;
            int32 nRenderableDetails = fIndexValid ? anRenderableDetailsIndices[nInfo + 1] : -1;
            if (nRenderableDetails == -1)
            {
                tDetails.tAppearance.ptBoneMapping = &tSubObject.ptSkinningInfo[nInfo];

                const SkinningInfo& tMapping = tSubObject.ptSkinningInfo[nInfo];
                tDetails.tAppearance.nNumMatrices = tMapping.nNumBones;

                if (ptSkinInfosCache && nInfo < ptSkinInfosCache->nSkinInfoCount)
                {
                    tDetails.tAppearance.pmMatrixPalette = ptSkinInfosCache->pmMatrixPalette[nInfo];
                }
                else
                {
                    Matrix3x4* __restrict pm = (Matrix3x4*)CGameAsyncRenderGetMemory(tMapping.nNumBones * sizeof(Matrix3x4));
                    tDetails.tAppearance.pmMatrixPalette = (float *)pm;

                    // Inference (823): pm advances once per entry (add #0x30 on the loop rows) and the stores at 828-841 sit at column 35.
                    for (int32 nEntryIndex = 0; nEntryIndex < tMapping.nNumBones; nEntryIndex++, pm++)
                    {
                        int32 nBoneIndex = tMapping.pBoneIDs[nEntryIndex];
                        ProcessedBoneSkin* __restrict ptBone = &ptProcessedBoneSkinList[nBoneIndex];

                        (*pm)[0]  = ptBone->mSkin._11;
                        (*pm)[1]  = ptBone->mSkin._21;
                        (*pm)[2]  = ptBone->mSkin._31;
                        (*pm)[3]  = ptBone->mSkin._41;

                        (*pm)[4]  = ptBone->mSkin._12;
                        (*pm)[5]  = ptBone->mSkin._22;
                        (*pm)[6]  = ptBone->mSkin._32;
                        (*pm)[7]  = ptBone->mSkin._42;

                        (*pm)[8]  = ptBone->mSkin._13;
                        (*pm)[9]  = ptBone->mSkin._23;
                        (*pm)[10] = ptBone->mSkin._33;
                        (*pm)[11] = ptBone->mSkin._43;
                    }

                    if (ptSkinInfosCache)
                    {
                        ptSkinInfosCache->nSkinInfoCount = nInfo + 1;
                        ptSkinInfosCache->pmMatrixPalette[nInfo] = tDetails.tAppearance.pmMatrixPalette;
                    }
                }

                nRenderableDetails = dispList.SubmitRenderableDetails(tDetails);
                if (fIndexValid)
                {
                    anRenderableDetailsIndices[nInfo + 1] = nRenderableDetails;
                }
            }
            dispList.SubmitRenderableHeader(tHeader, nRenderableDetails);
        }
    }
    else
    {





















































        tHeader.nGroup = (uintptr)-1;
        BOOL fIndexValid = 0 < nRenderableDetailsCapacity;
        int32 nRenderableDetails = fIndexValid ? anRenderableDetailsIndices[0] : -1;
        if (nRenderableDetails == -1)
        {
            tDetails.tAppearance.ptBoneMapping = NULL;
            nRenderableDetails = dispList.SubmitRenderableDetails(tDetails);
            if (fIndexValid)
            {
                anRenderableDetailsIndices[0] = nRenderableDetails;
            }
        }
        dispList.SubmitRenderableHeader(tHeader, nRenderableDetails);
    }


}




static int32 sDisplayListSubmitRActorStencilBoundsDetails(
    DisplayList & dispList,
    const RActor * ptRActor,
    const RenderableDetails & tDetails)
{
    // Inference (942/966/974): columns put the initialisers one to the left of `Type & name`, so these declarations are spelled `Type& name` / `Type* name`.
    const Structure& tStructure = tDetails.tAppearance.pdefAppearance->tStructure;
    AABB aabbBounds;
    Vector3D wpMin;
    Vector3D wpMax;

    if (ptRActor->ptActorAnim)
    {
        wpMin = ptRActor->ptActorAnim->wpMin;
        wpMax = ptRActor->ptActorAnim->wpMax;
    }
    else
    {

        wpMin = tStructure.aabbBounds.wp - tStructure.aabbBounds.wvExt;
        wpMax = tStructure.aabbBounds.wp + tStructure.aabbBounds.wvExt;
    }


    const GeoSet& tGeoSet = tStructure.tGeoSets[GEOSET_HIGHPOLY];
    const RActorGeoSetData& tGeoSetData = ptRActor->tGeoSetData[GEOSET_HIGHPOLY];

    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {

        SubObjectState& tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        BOOL fIsCloth = tSubObjectState.idCloth != CLOTHID_INVALID;
        if (fIsCloth)
        {



            Cloth* ptCloth = ClothManager::Get(tSubObjectState.idCloth); // Inference: the next line uses Cloth.h inline GetBoundingSphere; original has no call.
            Sphere wsBounds = ptCloth->GetBoundingSphere();
            float flRadius = wsBounds.wdRadius;
            Matrix4x4 matInverse = tDetails.tCommon.mWorld.Inverse();
            Vector3D wp = wsBounds.wpCenter * matInverse;

            wpMin.Set(Math::Min(wpMin.x, wp.x - flRadius), Math::Min(wpMin.y, wp.y - flRadius), Math::Min(wpMin.z, wp.z - flRadius));
            wpMax.Set(Math::Max(wpMax.x, wp.x + flRadius), Math::Max(wpMax.y, wp.y + flRadius), Math::Max(wpMax.z, wp.z + flRadius));
        }
    }


    const float flInflate = 2.0f;
    aabbBounds.Set(wpMin, wpMax);
    aabbBounds.Inflate(flInflate); // Inference: the next two writes require a cast dropping tDetails constness (DWARF signature and original stores).
    (Vector3D &)tDetails.tStencilBound.wp = aabbBounds.wp;
    (Vector3D &)tDetails.tStencilBound.wvExt = aabbBounds.wvExt;

    return dispList.SubmitRenderableDetails(tDetails);
}

































static BOOL sRActorCanBeHighlighted(
    RActor * ptOwnerRActor)
{
    if (GameGetActiveGameCommonData().GameModeGet() == GAMEMODE_PLAYING)
    {



        ActorCommonData * ptACD = ACDTryToGet(ptOwnerRActor->idACD);
        if (!ptACD)
        {
            return FALSE;
        }

        return ACDCanBeHighlighted(ptACD);
    }

    return TRUE;
}

static void sSetupSubobjectRenderableDetails(
    RenderableDetails & tDetails,
    const AppearanceDefinition * pdefAppearance,
    RActor * ptRActor,
    RActor * ptOwnerRActor,
    SubObjectState & tSubObjectState,
    SubObject & tSubObject,
    const Matrix4x4 & mWorld,
    const int32 nLightCount,
    const LightState ** pArrayLights,
    const int32 nSubObjectIndex,
    const SubObjectAppearance & tSOA)
{

    BOOL fIsCloth = tSubObjectState.idCloth != CLOTHID_INVALID;

    if (!fIsCloth)
    {
        tDetails.tCommon.mWorld = mWorld;
    }
    tDetails.tAppearance.nSubObjectIndex = nSubObjectIndex;

    tDetails.tAppearance.flLookSwitchTime = tSubObjectState.flLookSwitchTime;
    tDetails.tAppearance.idCloth = tSubObjectState.idCloth;
    tDetails.tAppearance.eDyeType = tSubObjectState.eDyeType;
    tDetails.tAppearance.nTexAnimStateCount = tSubObjectState.nTexAnimStateCount;

    ShaderTex eOverlayShaderTex = SHADERTEX_NONE;
    if (ptRActor->eOverlayType != 0)
    {
        eOverlayShaderTex = OverlayTypeToShaderTex(ptRActor->eOverlayType);
    }
    tDetails.tAppearance.arTexAnimStates = TexAnimRenderAsyncCopy(tSubObjectState.nTexAnimStateCount, tSubObjectState.arTexAnimStates, &tSOA.tCompositeUberMaterial, eOverlayShaderTex);




    tDetails.tAppearance.ptSOA = &tSOA;
    tDetails.tAppearance.ptSubObject = &tSubObject;
    tDetails.tAppearance.snoAppearance = ptRActor->snoAppearance;
    tDetails.tAppearance.snoIrradianceTex = ptRActor->snoIrradianceTexHighlight != SNO_NONE ? ptRActor->snoIrradianceTexHighlight :
        ptRActor->snoIrradianceTex;
    tDetails.tAppearance.snoAmbientOcclusionOverrideTex = ptRActor->snoAmbientOcclusionOverrideTex;
    tDetails.tAppearance.flRand = ptRActor->flRand;

    tDetails.tCommon.nLightCount = nLightCount;
    if (nLightCount)
    {

        SigmaMemoryCopy(tDetails.tCommon.pArrayLights, pArrayLights, nLightCount * sizeof(pArrayLights[0]));
    }

    if ((tSubObject.dwFlags & ((1 << SUBOBJECT_EDGE_GEOMETRY_BIT) | (1 << SUBOBJECT_PACK_GEOMETRY_BIT))) == 0)
    {

        tDetails.tAppearance.ptEntry = &tSubObject.ptPostprocessed->tVBEntry;



        tDetails.tAppearance.ptEntryIB = &tSubObject.ptPostprocessed->tIBEntry;
    }
    else
    if ((tSubObject.dwFlags & ((1 << SUBOBJECT_PACK_GEOMETRY_BIT) | (1 << SUBOBJECT_PACK_SKIN_BIT))) == (1 << SUBOBJECT_PACK_GEOMETRY_BIT))
    {

        tDetails.tAppearance.ptEntry = &tSubObject.ptPostprocessed->tPackVBEntry;
        tDetails.tAppearance.ptEntryIB = &tSubObject.ptPostprocessed->tIBEntry;
    }
    else if (tSubObject.dwFlags & (1 << SUBOBJECT_PACK_SKIN_BIT))
    {
        tDetails.tAppearance.ptEntry = &tSubObject.ptPostprocessed->tVBEntry;
        tDetails.tAppearance.ptEntryIB = &tSubObject.ptPostprocessed->tIBEntry;
        tDetails.tAppearance.ptBoneMapping = tSubObject.ptSkinningInfo;
    }


    tDetails.tAppearance.eOverlayType = ptRActor->dwOverlayFadeTimeCurrent ? ptRActor->eOverlayType : OVERLAYTYPE_NONE;
    tDetails.tAppearance.eOverlayProjectionAxis = pdefAppearance->eOverlayProjectionAxis;
    tDetails.tAppearance.dwOverlayFadeTimeCurrent = ptRActor->dwOverlayFadeTimeCurrent;
    tDetails.tAppearance.dwOverlayFadeTimeOriginal = ptRActor->dwOverlayFadeTimeOriginal;

    if (ptRActor->flHitFlashAddTimeRemaining > 0.0f)
    {
        float flFrac = ptRActor->flHitFlashAddTimeRemaining / ptRActor->flHitFlashAddDuration;
        flFrac = Math::SmoothStep(flFrac);

        tDetails.tAppearance.rgbavalHitFlashAdd[0] = ptRActor->rgbavalHitFlashAdd.r * flFrac;
        tDetails.tAppearance.rgbavalHitFlashAdd[1] = ptRActor->rgbavalHitFlashAdd.g * flFrac;
        tDetails.tAppearance.rgbavalHitFlashAdd[2] = ptRActor->rgbavalHitFlashAdd.b * flFrac;
    }
    if (ptRActor->flHitFlashMulTimeRemaining != 0.0f)
    {
        float flFrac = ptRActor->flHitFlashMulTimeRemaining / ptRActor->flHitFlashMulDuration;
        flFrac = Math::SmoothStep(flFrac);
        float flAlpha = 1.0f - flFrac;

        tDetails.tAppearance.rgbavalHitFlashMul[0] = ptRActor->rgbavalHitFlashMul.r * flFrac + flAlpha;
        tDetails.tAppearance.rgbavalHitFlashMul[1] = ptRActor->rgbavalHitFlashMul.g * flFrac + flAlpha;
        tDetails.tAppearance.rgbavalHitFlashMul[2] = ptRActor->rgbavalHitFlashMul.b * flFrac + flAlpha;
    }

    tDetails.tAppearance.pdefAppearance = pdefAppearance;


    if (ptOwnerRActor->ptOcclusionHighlighting && SubObjectGfxOcclusionHighlightingIsEnabled())
    {
        tDetails.tAppearance.ptOcclusionHighlighting = ptOwnerRActor->ptOcclusionHighlighting;
    }
}





static BOOL sIsAllSubObjectTexturesLoaded(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance)
{
    if (ptRActor->dwFlags & (1 << RACTOR_ALL_TEXTURE_LOADED_BIT))
    {
        if (ParentIsRActor(ptRActor->tParentData.m_eParentType))
        {
            const RActor * ptRActorParent = RActorTryToGet((RActorID)ptRActor->tParentData.m_idParent);


            if (ptRActorParent && (ptRActorParent->dwFlags & ((1 << RACTOR_ALL_TEXTURE_PREFETCHED_BIT) | (1 << RACTOR_ALL_TEXTURE_LOADED_BIT))) == (1 << RACTOR_ALL_TEXTURE_PREFETCHED_BIT))
            {
                return FALSE;
            }
        }
        return TRUE;
    }


    Prefetch::BeginAdd();

    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
    RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];

    ptRActor->dwFlags |= (1 << RACTOR_ALL_TEXTURE_PREFETCHED_BIT);

    BOOL fAllLoaded = TRUE;
    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        if (!(tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT)))
        {
            continue;
        }

        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
        int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
        MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
        for (int32 i = 0; i < nCount; i++)
        {
            SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
            if (snoTex != SNO_NONE && !Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoTex), 2)) { fAllLoaded = FALSE; }
        }
    }




    Prefetch::EndAdd();

    if (ParentIsRActor(ptRActor->tParentData.m_eParentType))
    {
        const RActor * ptRActorParent = RActorTryToGet((RActorID)ptRActor->tParentData.m_idParent);


        if (ptRActorParent && (ptRActorParent->dwFlags & ((1 << RACTOR_ALL_TEXTURE_PREFETCHED_BIT) | (1 << RACTOR_ALL_TEXTURE_LOADED_BIT))) == (1 << RACTOR_ALL_TEXTURE_PREFETCHED_BIT))
        {
            fAllLoaded = FALSE;
        }
    }


    if (fAllLoaded)
    {
        ptRActor->dwFlags |= (1 << RACTOR_ALL_TEXTURE_LOADED_BIT);
    }

    return (ptRActor->dwFlags & (1 << RACTOR_ALL_TEXTURE_LOADED_BIT)) ? TRUE : FALSE;
}





static void sRenderLowPolyShadows(
    DisplayList & dispList,
    const Camera & cam,
    const AppearanceDefinition * pdefAppearance,
    RActor * ptRActor,
    const Matrix4x4 & mWorld,
    const float flAlpha,
    const float flDeltaTime,
    const float flClientTime,
    const PerfBin ePerfBin,
    const ShaderDefinition * pdefShaderShadowStencilBounds)
{


    BOOL bHasShadows = FALSE;


    BOOL fIsBanner = pdefAppearance->tHeader.snoHandle == GlobalSNOGet(GLOBALSNO_PLAYER_BANNER_APPEARANCE);

    const ShaderDefinition * pdefLastCookieShader = NULL;

    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[GEOSET_LOWPOLY];
    RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[GEOSET_LOWPOLY];

    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        if (!(tSubObject.dwFlags & (1 << SUBOBJECT_CASTS_SHADOW)))
        {
            continue;
        }

        if (!(tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT)))
        {
            continue;
        }




















        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
        if (tSOA.tCompositeUberMaterial.snoShaderMap == SNO_NONE)
        {
            continue;
        }

        ScopedShaderMapDefinitionLockR pdefShaderMap(tSOA.tCompositeUberMaterial.snoShaderMap);
        SNO snoShader = sSelectShader(flAlpha, tSOA, ptRActor, nSubObjectIndex, tSubObjectState, pdefAppearance, pdefShaderMap);
        if (snoShader == SNO_NONE)
        {
            continue;
        }


        SNO snoZPass = ShaderMapGetGhostZPassShader(pdefShaderMap);
        if (snoZPass != SNO_NONE)
        {
            continue;
        }

        ScopedShaderDefinitionLockR pdefShader(snoShader);

        SubObjectStateUpdate(tSOA, tSubObjectState, &ptRActor->tObjTexAnimParam, flDeltaTime, flClientTime);

        Renderable tRenderable(
            RENDERABLETYPE_RACTOR,
            ptRActor->id,
            ePerfBin,
            pdefShader,
            tSOA.tCompositeUberMaterial.mat);
        RenderableHeader & tHeader = tRenderable.header;
        RenderableDetails & tDetails = tRenderable.details;


        ptRActor->dwLastFrameSubmitted = CGameAsyncGetUpdateFrameCount();



        if (ptRActor->ptBannerRenderData)
        {
            tDetails.tAppearance.ptBannerRenderData = (BannerRenderData *)CGameAsyncRenderGetMemory(sizeof(BannerRenderData));
            memcpy(tDetails.tAppearance.ptBannerRenderData, ptRActor->ptBannerRenderData, sizeof(BannerRenderData));
        }
        else
        {
            tDetails.tAppearance.ptBannerRenderData = NULL;
        }



        tDetails.tAppearance.nSubObjectCount = tGeoSet.nSubObjectCount;


        Vector3D wpSort = tSubObject.aabbBounds.wp * mWorld;
        tHeader.SetDist(Vector3D::DistanceSquared(cam.cmState.transform.wp, wpSort));
        tHeader.SetRectFromPoint(wpSort);

        tHeader.rgbaColor = crgbaWhite;
        if (flAlpha < 0.999999f)
        {
            tHeader.rgbaColor.a = (uint8)Math::FloatRoundToInt(flAlpha * 255.0f);
        }
        else
        {
            tHeader.rgbaColor.a = 0xff;
        }

        sSetupSubobjectRenderableDetails(tDetails, pdefAppearance, ptRActor, ptRActor, tSubObjectState, tSubObject, mWorld, 0, NULL, nSubObjectIndex, tSOA);

        BOOL fIsCloth = tSubObjectState.idCloth != CLOTHID_INVALID;

        BOOL fTinted = TagMapGetDWORD(pdefShader->ptPasses->hTagMap, TAG_VS_TINT);

        tHeader.fSetShadowLevel = TRUE;
        SNO snoShaderCookie = ShaderMapGetCookieShader(
            pdefShaderMap,
            ptRActor->ptActorAnim != NULL,
            fTinted, fIsCloth, fIsBanner);

        if (pdefLastCookieShader == NULL || snoShaderCookie != pdefLastCookieShader->tHeader.snoHandle)
        {




            pdefLastCookieShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderCookie);
        }

        tHeader.pdefShader = pdefLastCookieShader;

        sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, NULL, 0, NULL);
        bHasShadows = TRUE;
    }












    if (bHasShadows)
    {
        Material mat;
        SigmaMemoryClear(&mat, sizeof(mat));

        Renderable tRenderable(
            RENDERABLETYPE_STENCIL_BOUNDS,
            ptRActor->id,
            ePerfBin,
            pdefShaderShadowStencilBounds,
            mat);

        tRenderable.details.tCommon.mWorld = mWorld;

        dispList.SubmitRenderableHeader(tRenderable.header, sDisplayListSubmitRActorStencilBoundsDetails(dispList, ptRActor, tRenderable.details));
    }
}




static void sRActorDrawTempSuffocationInfo(
    RActor & tRActor)
{
    if (PlayerIsLocal(tRActor.id))
    {
        ActorCommonData * ptACD = ACDTryToGet(tRActor.idACD);
        if (ptACD)
        {
            float flUnitSuffocation = ptACD->AttributesGetFloat(FastAttribKey(ATTRIBUTE_SUFFOCATION_UNIT_VALUE));
            UIGame::SetBreathMeter(flUnitSuffocation);
        }
    }
}



static void sRActorDrawLoopingAnimationBar(
    const RActor & tRActor)
{
    if (PlayerIsLocal(tRActor.id))
    {
        ActorCommonData * ptACD = ACDTryToGet(tRActor.idACD);
        if (ptACD)
        {
            float flUnitTime = 0.0f;
            uint32 uEndTime = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_LOOPING_ANIMATION_END_TIME));
            if (uEndTime != 0)
            {
                uint32 uStartTime = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_LOOPING_ANIMATION_START_TIME));
                if (uStartTime <= uEndTime)
                {
                    uint32 uCurrTime = CGameGetPredictedServerTime();
                    if (uCurrTime >= uStartTime)
                    {

                        uint32 uTotalTime = uEndTime - uStartTime;
                        flUnitTime = Math::Min(1.0f, (float)(uCurrTime - uStartTime) / (float)uTotalTime);
                    }
                }
            }


            UIGame::SetLoopingAnimMeter(tRActor, flUnitTime);
        }
    }
}




static void sRenderUpdateActorDebugDraw(
    RActor * ptRActor)
{




    sRActorDrawTempSuffocationInfo(*ptRActor);
    sRActorDrawLoopingAnimationBar(*ptRActor);
}

static float sRenderUpdateActorVisibility(
    RActor * ptRActor,
    DisplayList & dispList,
    const float flDeltaTime)
{
    RActorID idRActorTopmost = RActorGetTopmostOwner(ptRActor->id, FALSE, FALSE);
    if (idRActorTopmost == RACTORID_INVALID)
    {
        idRActorTopmost = ptRActor->id;
    }

    if ((sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT)) && PlayerIsLocal(idRActorTopmost))
    {
        return 0.0f;
    }


    RActor * ptOwnerRActor = RActorGet(idRActorTopmost);

    ActorCommonData * ptACDTopmost = ACDTryToGet(ptOwnerRActor->idACD);

    if (dispList.GetRenderContext() != ptOwnerRActor->eContext)
    {
        return 0.0f;
    }

    sRActorGfxFadeUpdate(ptRActor, flDeltaTime);


    float flAlpha = RActorGetAlpha(ptRActor) * ptRActor->flFadeAlpha * ptRActor->flTriggerFadeAlpha;

    if (AppDrawFlagIsSet(APP_DRAW_NO_PLAYER_ACTOR_BIT) && ptACDTopmost && ACDIsPlayer(ptACDTopmost->id))
    {
        return 0.0f;
    }


    if (AppDrawFlagIsSet(APP_DRAW_NO_MONSTER_ACTORS_BIT) && ptACDTopmost && ptACDTopmost->IsMonster())
    {
        return 0.0f;
    }


    if (AppDrawFlagIsSet(APP_DRAW_NO_GIZMO_ACTORS_BIT))
    {
        if (ptOwnerRActor->snoActor == GlobalSNOGet(GLOBALSNO_MARKER_ACTOR_GIZMOLOC)) { return 0.0f; }




        ScopedActorDefinitionLockR pdefOwnerActor(ptOwnerRActor->snoActor);
        if (pdefOwnerActor->eType == ACTORTYPE_GIZMO) { return 0.0f; }



    }


    if ((ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)) != 0 || flAlpha < 0.000001f)
    {
        flAlpha = 0.0f;
    }


    return flAlpha;
}


// frame 0x430 vs 0x440 and spill slots follow. Tried: sno-first cache compares, DistanceSquared(cam, wpSort) (fixed sRenderLowPolyShadows math), prefix loop increments; layout unchanged. sRenderLowPolyShadows likewise calls GhostZPass before the main path where the original places it last.
// NON_MATCHING: 6348 vs 6388 bytes. Same constants; ordered call structure and block placement differ: the original rotates the subobject loop so the mask block (2038-2086) sits first and shares one ACDIsPlayer call between the parent and self paths, ours puts the 1843 GhostZPass branch first and keeps two ACDIsPlayer calls; Cookie condition re-split per rows 1989:17/1989:70/1992:30/1992:42 (flag test first); original branches on (snoZPass == SNO_NONE & fCast), then context, then flag, ours and-merges differently (6348 either way).
static void sRenderUpdateActorSubmit(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance,
    const Camera & cam,
    DisplayList & dispList,
    DisplayList * ptDisplayListReflection,
    DisplayList * ptDisplayListProjected,
    const float flDeltaTime,
    const float flClientTime,
    const BOOL fVisibilityCull,
    const BOOL fClearHighlight,
    int32 nLightCount,
    const LightState ** pArrayLights,
    float flAlpha,
    const ShaderDefinition * pdefShaderShadowStencilBounds,
    const ShaderDefinition * pdefShaderHighlightStencilBounds)
{


    if (!sIsAllSubObjectTexturesLoaded(ptRActor, pdefAppearance))
    {
        return;
    }

    BOOL bHasShadows = FALSE;
    BOOL bHasHighlight = FALSE;


    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
    if (tGeoSet.nSubObjectCount == 0)
    {
        return;
    }
    const BOOL bAppPrepassFlagEnabled = AppDrawFlagIsSet(APP_DRAW_DEPTH_PREPASS_BIT);

    RActorID idRActorTopmost = RActorGetTopmostOwner(ptRActor->id, FALSE, FALSE);
    if (idRActorTopmost == RACTORID_INVALID)
    {
        idRActorTopmost = ptRActor->id;
    }

    RActor * ptOwnerRActor = RActorGet(idRActorTopmost);

    int32 nReflectionQuality = Prefs::GetVideoPrefs().GetReflectionQuality();

    const Matrix4x4 & mWorld = RActorGetWorldMatrix(ptRActor);

    enum ShadowMethod
    {
        SHADOWMETHOD_NO_SHADOWS,
        SHADOWMETHOD_COOKIE_SHADOWS,
    };

    ShadowMethod eShadowMethod = SHADOWMETHOD_NO_SHADOWS;
    if (!AppDrawFlagIsSet(APP_DRAW_NO_ACTOR_SHADOWS_BIT))
    {
        if (ptRActor->dwFlags & (1 << RACTOR_SHADOWS_ENABLED_BIT))
        {
            eShadowMethod = SHADOWMETHOD_COOKIE_SHADOWS;
        }
    }

    PerfBin ePerfBin = PERFBIN_ACTORS;
    RenderContext eContext = dispList.GetRenderContext();

    BOOL fDefaultOrHolodeckContext = (eContext == RENDERCONTEXT_DEFAULT) || (eContext == RENDERCONTEXT_HOLODECK);

    if (eContext == RENDERCONTEXT_HUD_0)
    {
        ePerfBin = PERFBIN_UI;
    }














    BOOL fRActorCanBeHighlighted = sRActorCanBeHighlighted(ptOwnerRActor);
    BOOL fRActorDrawsHighlighted = fRActorCanBeHighlighted && (ptOwnerRActor->dwFlags & (1 << RACTOR_HIGHLIGHT_BIT));

    RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];







    if (ptOwnerRActor->ptOcclusionHighlighting)
    {
        const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

        float flDist = (loc.transform.wp - cam.cmState.wvLookFrom).Magnitude();
        float flTransformedRadius = ((loc.wcylBounds.wdRadius * loc.wcylBounds.wdHeight) / flDist) * s_flResolutionFactor;

        SubObjectGfxOcclusionHighlightingSetEstimatedPixelCount(ptOwnerRActor->ptOcclusionHighlighting, (uint32)(flTransformedRadius * flTransformedRadius * PI));
    }



    BOOL fVisible = !fVisibilityCull || (ptRActor->dwVisibilityFlags & (1 << RACTOR_VISIBLE_BIT));
    BOOL fIsBanner = pdefAppearance->tHeader.snoHandle == GlobalSNOGet(GLOBALSNO_PLAYER_BANNER_APPEARANCE);
    BOOL fCastHighPolyShadow = (eShadowMethod == SHADOWMETHOD_COOKIE_SHADOWS) && (ptRActor->dwVisibilityFlags & (1 << RACTOR_CASTS_SHADOW_BIT)) && (!AppDrawFlagIsSet(APP_DRAW_LOW_POLY_SHADOWS_BIT) || (tStructure.tGeoSets[GEOSET_LOWPOLY].nSubObjectCount == 0));

    const ShaderDefinition * pdefLastClearShader = NULL;
    const ShaderDefinition * pdefLastZPassShader = NULL;
    const ShaderDefinition * pdefLastOccQueryShader = NULL;
    const ShaderDefinition * pdefLastHighlightShader = NULL;
    const ShaderDefinition * pdefLastOccludedShader = NULL;
    const ShaderDefinition * pdefLastPrepassShader = NULL;
    const ShaderDefinition * pdefLastMaskShader = NULL;
    const ShaderDefinition * pdefLastCookieShader = NULL;





    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        BOOL fIsCloth = tSubObjectState.idCloth != CLOTHID_INVALID;

        if (!(tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT)))
        {
            continue;
        }






























        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
        if (tSOA.tCompositeUberMaterial.snoShaderMap == SNO_NONE)
        {
            continue;
        }

        ScopedShaderMapDefinitionLockR pdefShaderMap(tSOA.tCompositeUberMaterial.snoShaderMap);
        SNO snoShader = sSelectShader(flAlpha, tSOA, ptRActor, nSubObjectIndex, tSubObjectState, pdefAppearance, pdefShaderMap);
        if (snoShader == SNO_NONE)
        {
            continue;
        }


        ScopedShaderDefinitionLockR pdefShader(snoShader);
        uint32 dwRenderLayers = pdefShader->dwRenderLayers;
        if (dwRenderLayers & (1 << RENDERLAYER_POSTFX_DISTORTION))
        {
            if (!AppDrawFlagIsSet(APP_DRAW_HWCLASS_DISTORTIONS_BIT))
            {
                continue;
            }
            PostFXSetFrameFlag(POSTFX_FRAME_HAS_SOMETHING_DISTORTING, TRUE);
        }

        Renderable tRenderable(
            RENDERABLETYPE_RACTOR,
            ptRActor->id,
            ePerfBin,
            pdefShader,
            tSOA.tCompositeUberMaterial.mat);
        RenderableDetails & tDetails = tRenderable.details;
        RenderableHeader & tHeader = tRenderable.header;


        ptRActor->dwLastFrameSubmitted = CGameAsyncGetUpdateFrameCount();

        tDetails.tAppearance.nSubObjectCount = tGeoSet.nSubObjectCount;


        Vector3D wpSort = tSubObject.aabbBounds.wp * mWorld;
        float flDist = Vector3D::DistanceSquared(cam.cmState.transform.wp, wpSort);
        tHeader.SetDist(flDist);
        tHeader.SetRectFromPoint(wpSort);

        tHeader.rgbaColor = crgbaWhite;
        if (flAlpha < 0.999999f)
        {
            tHeader.rgbaColor.a = (uint8)Math::FloatRoundToInt(flAlpha * 255.0f);
        }
        else
        {
            tHeader.rgbaColor.a = 0xff;
        }

        sSetupSubobjectRenderableDetails(tDetails, pdefAppearance, ptRActor, ptOwnerRActor, tSubObjectState, tSubObject, mWorld, nLightCount, pArrayLights, nSubObjectIndex, tSOA);

        SNO snoShaderPrepass = SNO_NONE;

        if ((flAlpha >= 0.999999f) && fDefaultOrHolodeckContext && fVisible)
        {
            const float flActorPrepassClipDist = 400.0f;
            const float flActorPrepassClipDistSqrd = flActorPrepassClipDist * flActorPrepassClipDist;

            if (flDist < flActorPrepassClipDistSqrd)
            {
                snoShaderPrepass = sSelectPrepassShader(pdefShader, pdefShaderMap);
                if (snoShaderPrepass != SNO_NONE)
                {
                    tHeader.fInPrepass = TRUE;
                }
            }
        }


        if (ptRActor->ptBannerRenderData)
        {
            tDetails.tAppearance.ptBannerRenderData = (BannerRenderData *)CGameAsyncRenderGetMemory(sizeof(BannerRenderData));
            memcpy(tDetails.tAppearance.ptBannerRenderData, ptRActor->ptBannerRenderData, sizeof(BannerRenderData));
        }
        else
        {
            tDetails.tAppearance.ptBannerRenderData = NULL;
        }





        if (!fDefaultOrHolodeckContext && tDetails.tAppearance.snoIrradianceTex == SNO_NONE)
        {
            tDetails.tAppearance.snoIrradianceTex = GlobalSNOGet(GLOBALSNO_SUBOBJECT_IRRADIANCE_TEXTURE);
        }

        SubObjectSkinInfos tSkinInfoCache;
        tSkinInfoCache.nSkinInfoCount = 0;

        int32 anRenderableDetailsIndices[16];
        for (int32 i = 0; i < 16; i++)
        {
            anRenderableDetailsIndices[i] = -1;
        }

        if (fClearHighlight)
        {
            SNO snoShaderScoped;
            sSelectHighlightClearShader(ptRActor, fIsCloth, &snoShaderScoped);
            if (pdefLastClearShader == NULL || snoShaderScoped != pdefLastClearShader->tHeader.snoHandle)
            {




                pdefLastClearShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderScoped);
            }
            tHeader.pdefShader = pdefLastClearShader;
            sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
        }
        else
        {
            SNO snoZPass = ShaderMapGetGhostZPassShader(pdefShaderMap);
            if (fVisible)
            {
                if (tSubObject.dwFlags & (1 << SUBOBJECT_RECEIVES_SHADOW))
                {
                    tHeader.fSetShadowLevel = TRUE;
                }












                sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                tHeader.fSetShadowLevel = FALSE;
                tHeader.fUseHighlight = FALSE;

                if (fDefaultOrHolodeckContext)
                {

                    if (ptDisplayListReflection && nReflectionQuality > 0)
                    {
                        WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

                        for (int i = 0; i < PostFXGetNumReflectionPlanes(); i++)
                        {
                            const Camera & tCamera = CameraManagerGetCamera(CAMERA_REFLECTION_0 + i);
                            const Plane & tPlane = PostFXGetReflectionPlane(i);

                            if (CameraComputeSphereVisibility(tCamera, loc.wsBounds))
                            {
                                float flDistScoped = tPlane.ContainsPoint(loc.wsBounds.wpCenter);
                                if ((flDistScoped <= loc.wsBounds.wdRadius + 6.0) &&
                                    (flDistScoped >= -loc.wsBounds.wdRadius))
                                {
                                    sDisplayListSubmitRActorSubObject(ptDisplayListReflection[i], ptRActor, tSubObject, fIsCloth, tRenderable, NULL, 0, &tSkinInfoCache);
                                }
                            }
                        }
                    }


                    if (ptDisplayListProjected && (tSubObject.dwFlags & (1 << SUBOBJECT_RECEIVES_FX_BIT)))
                    {
                        sDisplayListSubmitRActorSubObject(*ptDisplayListProjected, ptRActor, tSubObject, fIsCloth, tRenderable, NULL, 0, &tSkinInfoCache);
                    }
                }

                if (snoZPass != SNO_NONE)
                {
                    if (pdefLastZPassShader == NULL || snoZPass != pdefLastZPassShader->tHeader.snoHandle)
                    {




                        pdefLastZPassShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoZPass);
                    }
                    tHeader.pdefShader = pdefLastZPassShader;
                    sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                }


                if (ptOwnerRActor == ptRActor && ptOwnerRActor->ptOcclusionHighlighting && SubObjectGfxOcclusionHighlightingIsEnabled())
                {
                    SNO snoOcclusionQueryShader = sSelectOcclusionQueryShader(pdefShader);
                    if (snoOcclusionQueryShader != SNO_NONE)
                    {
                        if (pdefLastOccQueryShader == NULL || snoOcclusionQueryShader != pdefLastOccQueryShader->tHeader.snoHandle)
                        {




                            pdefLastOccQueryShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoOcclusionQueryShader);
                        }
                        tHeader.pdefShader = pdefLastOccQueryShader;
                        tHeader.fUseHighlight = TRUE;
                        sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                        tHeader.fUseHighlight = FALSE;
                    }
                }



                if (!(tSOA.dwFlags & (1 << SUBOBJECTAPPEARANCE_NO_HIGHLIGHT_BIT)))
                {




                    if (fRActorDrawsHighlighted)
                    {
                        SNO snoShaderScoped;
                        sSelectHighlightShader(ptRActor, fIsCloth, pdefShader, &snoShaderScoped, &tHeader.rgbaColor, pdefShaderMap);
                        if (snoShaderScoped != SNO_NONE)
                        {
                            if (pdefLastHighlightShader == NULL || snoShaderScoped != pdefLastHighlightShader->tHeader.snoHandle)
                            {




                                pdefLastHighlightShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderScoped);
                            }
                            tHeader.pdefShader = pdefLastHighlightShader;
                            sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                            bHasHighlight = TRUE;
                        }
                    }



                    if (ptOwnerRActor->ptOcclusionHighlighting && SubObjectGfxOcclusionHighlightingIsEnabled())
                    {
                        SNO snoShaderScoped;
                        sSelectOccludedShader(ptRActor, fIsCloth, pdefShader, &snoShaderScoped, &tHeader.rgbaColor, fRActorDrawsHighlighted, pdefShaderMap);
                        if (snoShaderScoped != SNO_NONE)
                        {
                            if (pdefLastOccludedShader == NULL || snoShaderScoped != pdefLastOccludedShader->tHeader.snoHandle)
                            {




                                pdefLastOccludedShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderScoped);
                            }
                            tHeader.pdefShader = pdefLastOccludedShader;
                            tHeader.fUseHighlight = TRUE;
                            sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                            tHeader.fUseHighlight = FALSE;
                            bHasHighlight = TRUE;
                        }
                    }
                }
            }




            if ((tSubObject.dwFlags & (1 << SUBOBJECT_CASTS_SHADOW)) &&

                // Inference: operand split and order from rows 1989:17 (flag test), 1989:70 (&&), 1992:30/42 (== and &&).
                    snoZPass == SNO_NONE && fCastHighPolyShadow && fDefaultOrHolodeckContext)
            {

                BOOL fTinted = TagMapGetDWORD(pdefShader->ptPasses->hTagMap, TAG_VS_TINT);

                tHeader.fSetShadowLevel = TRUE;
                SNO snoShaderScoped = ShaderMapGetCookieShader(pdefShaderMap, ptRActor->ptActorAnim != NULL, fTinted, fIsCloth, fIsBanner);

                if (pdefLastCookieShader == NULL || snoShaderScoped != pdefLastCookieShader->tHeader.snoHandle)
                {




                    pdefLastCookieShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderScoped);
                }
                tHeader.pdefShader = pdefLastCookieShader;
                sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, NULL);
                tHeader.fSetShadowLevel = FALSE;
                bHasShadows = TRUE;
            }
        }





        if (fDefaultOrHolodeckContext)
        {

            if (bAppPrepassFlagEnabled && tHeader.fInPrepass)
            {
                if (pdefLastPrepassShader == NULL || snoShaderPrepass != pdefLastPrepassShader->tHeader.snoHandle)
                {




                    pdefLastPrepassShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderPrepass);
                }
                tHeader.pdefShader = pdefLastPrepassShader;
                sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsCloth, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
            }



            if (tSubObject.dwFlags & (1 << SUBOBJECT_CASTS_SHADOW))
            {

                if (ParentIsRActor(ptRActor->tParentData.m_eParentType))
                {
                    const RActor * ptRActorParent = RActorTryToGet((RActorID)ptRActor->tParentData.m_idParent);
                    if (ptRActorParent && ptRActorParent->idACD != ACDID_INVALID && ACDIsPlayer(ptRActorParent->idACD))
                    {
                        BOOL fTinted = TagMapGetDWORD(pdefShader->ptPasses->hTagMap, TAG_VS_TINT);

                        BOOL fIsClothScoped = tSubObjectState.idCloth != CLOTHID_INVALID;

                        SNO snoShaderMask = sSelectMaskShader(pdefShader, pdefShaderMap, ptRActor->ptActorAnim != NULL, fTinted, fIsClothScoped);
                        if (snoShaderMask != SNO_NONE)
                        {
                            if (pdefLastMaskShader == NULL || snoShaderMask != pdefLastMaskShader->tHeader.snoHandle)
                            {
                                pdefLastMaskShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderMask);
                            }
                            tHeader.pdefShader = pdefLastMaskShader;
                            sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsClothScoped, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                        }
                    }
                }
                else





                    if (ptRActor->idACD != ACDID_INVALID && ACDIsPlayer(ptRActor->idACD))
                    {
                        BOOL fTinted = TagMapGetDWORD(pdefShader->ptPasses->hTagMap, TAG_VS_TINT);

                        BOOL fIsClothScoped = tSubObjectState.idCloth != CLOTHID_INVALID;

                        SNO snoShaderMask = sSelectMaskShader(pdefShader, pdefShaderMap, ptRActor->ptActorAnim != NULL, fTinted, fIsClothScoped);
                        if (snoShaderMask != SNO_NONE)
                        {
                            if (pdefLastMaskShader == NULL || snoShaderMask != pdefLastMaskShader->tHeader.snoHandle)
                            {
                                pdefLastMaskShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast(snoShaderMask);
                            }
                            tHeader.pdefShader = pdefLastMaskShader;
                            sDisplayListSubmitRActorSubObject(dispList, ptRActor, tSubObject, fIsClothScoped, tRenderable, anRenderableDetailsIndices, 16, &tSkinInfoCache);
                        }
                    }

            }
        }
    }





    if (!fClearHighlight && fDefaultOrHolodeckContext &&


        (eShadowMethod == SHADOWMETHOD_COOKIE_SHADOWS) && (ptRActor->dwVisibilityFlags & (1 << RACTOR_CASTS_SHADOW_BIT)) &&


        AppDrawFlagIsSet(APP_DRAW_LOW_POLY_SHADOWS_BIT) &&

        tStructure.tGeoSets[GEOSET_LOWPOLY].nSubObjectCount)
    {

        sRenderLowPolyShadows(dispList, cam, pdefAppearance, ptRActor, mWorld, flAlpha, flDeltaTime, flClientTime, ePerfBin, pdefShaderShadowStencilBounds);
    }














































    if (fDefaultOrHolodeckContext && (bHasShadows || bHasHighlight))
    {
        Material mat;
        SigmaMemoryClear(&mat, sizeof(mat));

        Renderable tRenderable(
            RENDERABLETYPE_STENCIL_BOUNDS,
            ptRActor->id,
            ePerfBin,
            NULL,
            mat);

        tRenderable.details.tCommon.mWorld = mWorld;
        tRenderable.details.tAppearance.pdefAppearance = pdefAppearance;

        int32 nRenderableDetails = sDisplayListSubmitRActorStencilBoundsDetails(dispList, ptRActor, tRenderable.details);

        if (bHasShadows)
        {
            tRenderable.header.pdefShader = pdefShaderShadowStencilBounds;
            dispList.SubmitRenderableHeader(tRenderable.header, nRenderableDetails);
        }

        if (bHasHighlight)
        {
            tRenderable.header.pdefShader = pdefShaderHighlightStencilBounds;
            dispList.SubmitRenderableHeader(tRenderable.header, nRenderableDetails);
        }
    }

}




static void sAdjustVisibilityFlags(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance,
    const Frustum * ptFrustum)
{
    if ((ptRActor->dwVisibilityFlags & (1 << RACTOR_VISIBLE_BIT)) == 0) { return; }




    Sphere wsBound;
    if (ptRActor->ptActorAnim)
    {
        AABB aabb;
        aabb.Set(ptRActor->ptActorAnim->wpMin, ptRActor->ptActorAnim->wpMax);
        wsBound.wpCenter = aabb.wp;
        wsBound.wdRadius = aabb.wvExt.Magnitude();
    }
    else
    {
        const Structure & tStructure = pdefAppearance->tStructure;
        wsBound = tStructure.wsBounds;
    }

    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    const Matrix4x4 & mWorld = ptRActor->mWorld;
    wsBound.wpCenter = wsBound.wpCenter * mWorld;
    wsBound.wdRadius *= loc.flScale;

    BOOL fReallyVisible = Intersect::FrustumAndSphere(*ptFrustum, wsBound);
    ptRActor->dwVisibilityFlags = !fReallyVisible ? (ptRActor->dwVisibilityFlags & ~(1 << RACTOR_VISIBLE_BIT)) : (ptRActor->dwVisibilityFlags | (1 << RACTOR_VISIBLE_BIT));
}




static int32 sRenderUpdateActorLightingAndSubObjectStates(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance,
    const Frustum * ptFrustum,
    const int32 nMaxLightCount,
    const LightState ** pArrayLights,
    const float flDeltaTime,
    const float flClientTime)
{
    if (ptFrustum != NULL)
    {
        sAdjustVisibilityFlags(ptRActor, pdefAppearance, ptFrustum);
    }

    RActorID idRActorTopmost = RActorGetTopmostOwner(ptRActor->id, FALSE, FALSE);
    if (idRActorTopmost == RACTORID_INVALID)
    {
        idRActorTopmost = ptRActor->id;
    }

    RActor * ptOwnerRActor = RActorGet(idRActorTopmost);
    RActor * ptLightingRActor = ptOwnerRActor;

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);


    if (ptACD && ACDIsPlayer(ptACD->id)) { ptLightingRActor = ptRActor; }




    const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
    RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];

    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; nSubObjectIndex++)
    {
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];
        if (tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT))
        {



            const SubObjectAppearance & tSOA = AppearanceGetSubObjectAppearance(pdefAppearance, tSubObjectState.nSubObjectLook, nSubObjectIndex, eGeoSetType);
            if (tSOA.tCompositeUberMaterial.snoShaderMap != SNO_NONE)




                SubObjectStateUpdate(tSOA, tSubObjectState, &ptRActor->tObjTexAnimParam, flDeltaTime, flClientTime);
        }
    }
    return LightsGetDynamic(ptOwnerRActor->eContext, loc.wsBounds, ptLightingRActor->dwLightFilterFlags, nMaxLightCount, pArrayLights, NULL, NULL);
}




static void sUpdateAndSubmitRActor(
    RActor * ptRActor,
    const Frustum * ptFrustum,
    const Camera & cam,
    DisplayList & dispList,
    DisplayList * ptDisplayListReflection,
    DisplayList * ptDisplayListProjected,
    const float flDeltaTime,
    const float flClientTime,
    const BOOL fVisibilityCull,
    const BOOL fClearHighlight,
    const ShaderDefinition * pdefShaderShadowStencilBounds,
    const ShaderDefinition * pdefShaderHighlightStencilBounds)
{

    if (dispList.GetRenderContext() == ptRActor->eContext)
    {
        sRenderUpdateActorDebugDraw(ptRActor);
    }

    float flAlpha = sRenderUpdateActorVisibility(ptRActor, dispList, flDeltaTime);
    if (flAlpha == 0.0f)
    {
        return;
    }

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);



    const LightState * pArrayLights[16];
    int32 nLightCount = sRenderUpdateActorLightingAndSubObjectStates(ptRActor, pdefAppearance, ptFrustum, 15, pArrayLights, flDeltaTime, flClientTime);

    sRenderUpdateActorSubmit(ptRActor, pdefAppearance, cam, dispList, ptDisplayListReflection, ptDisplayListProjected, flDeltaTime, flClientTime, fVisibilityCull, fClearHighlight, nLightCount, pArrayLights, flAlpha, pdefShaderShadowStencilBounds, pdefShaderHighlightStencilBounds);
}



static BOOL sPassedLOSCheck(
    const RActor & tRActorSource,
    const RActor & tRActorTarget)
{
    if (tRActorSource.id == tRActorTarget.id)
    {
        return TRUE;
    }

    ScopedActorDefinitionLockR pdefActor(tRActorTarget.snoActor);
    if (!TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_FORCE_LOS_CULLING))
    {
        ACDID idACDTarget = tRActorTarget.idACD;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        // Inference: the line-0 null test after ACDTryToGet exits through the TRUE return.
        if (!ptACDTarget) { return TRUE; }

        if (PlayerIsLocal(idACDTarget))
        {
            return TRUE;
        }

        Team eTeamLocalPlayer = PlayerGetLocalTeam();
        ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet());
        if (ptACDLocalPlayer && ptACDLocalPlayer->IsDueling())
        {
            eTeamLocalPlayer = ptACDLocalPlayer->GetTeam();
        }

        if ((ptACDTarget->IsPlayerTeam() && TeamsAreFriendly(ptACDTarget->GetTeam(), eTeamLocalPlayer)) || !ptACDTarget->IsAlive())
        {
            return TRUE;
        }






        const CActorMovement * ptMoveSource = tRActorSource.ptCActorMovement;
        const CActorMovement * ptMoveTarget = tRActorTarget.ptCActorMovement;
        if (!ptMoveSource || !ptMoveTarget)
        {
            return TRUE;
        }

        if (pdefActor->eType != ACTORTYPE_PLAYER && pdefActor->eType != ACTORTYPE_MONSTER)
        {
            return TRUE;
        }

        if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_IGNORE_LOS_CULLING))
        {
            return TRUE;
        }
    }

    if (tRActorSource.tLocation.eActorLocation != AL_WORLD)
    {
        return FALSE;
    }
    WorldPlace placeSource;
    placeSource.idSWorld = tRActorSource.tLocation.tWorldLocation.idSWorld;
    placeSource.wp = tRActorSource.wpWalkingPos;

    if (tRActorTarget.tLocation.eActorLocation != AL_WORLD)
    {
        return FALSE;
    }
    WorldPlace placeTarget;
    placeTarget.idSWorld = tRActorTarget.tLocation.tWorldLocation.idSWorld;
    placeTarget.wp = tRActorTarget.wpWalkingPos;

    if (placeSource.idSWorld == SWORLDID_INVALID || placeSource.idSWorld != placeTarget.idSWorld)
    {
        return FALSE;
    }


    RayCastInput tInput;
    RayCastOutput tOutput;
    tInput.wpFrom = placeSource.wp;
    tInput.wpTo = placeTarget.wp;
    tInput.uIncludeMask = (1 << COLLISION_CATEGORY_SCENE) | (1 << COLLISION_CATEGORY_SMOKE);
    tInput.uExcludeMask = (1 << COLLISION_CATEGORY_FENCE) | (1 << COLLISION_CATEGORY_SEE_THRU);



    // Inference: original fadd uses 10.0f; no named height constant is recorded in the CU.
    tInput.wpFrom.z += 10.0f;
    tInput.wpTo.z = Math::Max(tInput.wpFrom.z, tInput.wpTo.z + 10.0f);

    BOOL bHit = gptPhysicsAPIImpl->RayCast(&tOutput, &tInput);
    if (!bHit)
    {

        ACDCollParams tACDCollParams;
        tACDCollParams.uObjectNoOverlapFlags = (1 << COLL_DYN_LOS_BLOCKER);
        ACDCollBroadResult tBroadResult;
        ACDCollNarrowResult tNarrowResult;
        if (ACDCollBroadLine(placeSource.idSWorld, tInput.wpFrom.As2D(), tInput.wpTo.As2D(), tACDCollParams, tBroadResult))
        {
            if (ACDCollNarrowSweptPoint(tBroadResult, tInput.wpFrom.As2D(), tInput.wpTo.As2D(), 0, tRActorSource.idACD, tNarrowResult))
            {
                bHit = TRUE;
            }
        }
    }

    return !bHit;

}




void RActorGfxPerformLOSCheckInit(
    RActor & tRActorTarget)
{
    uint32 nLocalPlayerCount = 0;
    const RActor * pptLocalPlayerActors[4];

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer))
        {
            RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);
            if (ptRActorPlayer)
            {
                pptLocalPlayerActors[nLocalPlayerCount++] = ptRActorPlayer;
            }
        }
    }

    if (nLocalPlayerCount > 0)
    {


        RActorGfxPerformLOSCheck(pptLocalPlayerActors, nLocalPlayerCount, tRActorTarget);
    }

    float flCurrentTime = CGameGetClientTime();
    float flTimeOffset = 1.0f + RandFloat_Internal(CGameGlobalsGet().rngLocal);
    tRActorTarget.flLastLOSCheckTime = (flCurrentTime > flTimeOffset) ?

        flCurrentTime - flTimeOffset : 0.0f;





    tRActorTarget.fLastLOSCheckResult = (tRActorTarget.dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)) != 0;
}



void RActorGfxPerformLOSCheck(
    const RActor ** pptRActorSources,
    int32 nSourceCount,
    RActor & tRActorTarget)
{
    if (!AppDrawFlagIsSet(APP_DRAW_DISABLE_LOS_BIT))
    {
        for (int32 i = 0; i < nSourceCount; i++)
        {
            NavMeshHandle tNavMeshHandle;
            ACDGetNavMeshHandle(ACDGet(pptRActorSources[i]->idACD), tNavMeshHandle);
            if (NavMeshShouldCheckLOS(tNavMeshHandle))
            {
                RActorID idRActorTopmost = RActorGetTopmostOwner(tRActorTarget.id, FALSE, FALSE);
                RActor * ptRActorTopmost = RActorTryToGet(idRActorTopmost);
                if (!ptRActorTopmost)
                {
                    ptRActorTopmost = &tRActorTarget;
                }

                if (sPassedLOSCheck(*pptRActorSources[i], *ptRActorTopmost))
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }




        tRActorTarget.dwVisibilityFlags |= (1 << RACTOR_NOT_IN_LOS_BIT);
    }
}











void UpdateRActorsLOSData::Func(const void * pDataIn)
{
    const UpdateRActorsLOSData * pData = (const UpdateRActorsLOSData *)pDataIn;


    uint32 nLocalPlayerCount = 0;
    const RActor * pptLocalPlayerActors[4];

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer))
        {
            RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);
            if (ptRActorPlayer)
            {
                pptLocalPlayerActors[nLocalPlayerCount++] = ptRActorPlayer;
            }
        }
    }


    for (uint32 i = 0; i < pData->uActorsCount; i++)
    {
        RActor * ptRActor = RActorTryToGet(pData->aidActors[i]);
        if (ptRActor)
        {



            ptRActor->dwVisibilityFlags &= ~(1 << RACTOR_NOT_IN_LOS_BIT);
            RActorGfxPerformLOSCheck(pptLocalPlayerActors, nLocalPlayerCount, *ptRActor);
            ptRActor->fLastLOSCheckResult = (ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)) != 0;
        } }
}



static void sRActorsUpdateVisibility(
    RWindow & tRWindow)
{
    CGame & tCGame = CGameGet();
    RActorGfxGlobals * ptRActorGfxGlobals = tCGame.ptRActorGfxGlobals;

    uint32 nLocalPlayerCount = 0;
    const RActor * pptLocalPlayerActors[4];

    for (const Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer))
        {
            RActor * ptRActorPlayer = RActorTryToGet(ptPlayer->idPlayerRActor);
            if (ptRActorPlayer)
            {
                // Inference: array store is eliminated; DWARF array/local count mirror PerformLOSCheckInit.
                pptLocalPlayerActors[nLocalPlayerCount++] = ptRActorPlayer;
            }
        }
    }
    float flCurrTime = CGameGetClientTime();


















    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    RenderContext eContext = tAsyncRVData.dispList.GetRenderContext();


    UpdateRActorsLOSData * pUpdateRActorsLOSData = NULL;



    TDataArrayIterator<RActor> tIt;
    while (tCGame.ptRActors->Iterate(tIt))
    {
        RActor * ptRActor = tIt.ptElement;

        if (ptRActor->eContext != eContext)
        {
            continue;
        }


        ptRActor->dwVisibilityFlags = 0;
    }


    {
        ptRActorGfxGlobals->plistFrustumActors->RemoveAll();
        ptRActorGfxGlobals->bNecroCorpseInFrustum = FALSE;

        const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
        RActorsIntersectFrustum(cam.tFrustum, *ptRActorGfxGlobals->plistFrustumActors);

        POS pos = ptRActorGfxGlobals->plistFrustumActors->GetHeadPosition();
        while (pos)
        {
            RActorID idRActor = (RActorID)ptRActorGfxGlobals->plistFrustumActors->GetNext(pos);



            RActor * ptRActor = RActorTryToGet(idRActor);
            // Inference: line-0 null branch after RActorTryToGet continues the list loop.
            if (!ptRActor) { continue; }



            if (ptRActor->eContext != eContext)
            {
                continue;
            }


            if (!ptRActorGfxGlobals->bNecroCorpseInFrustum && RActorIsNecroCorpse(ptRActor))
            {
                if (cam.tFrustum.ContainsPoint(ptRActor->transformClient.wp))
                {
                    ptRActorGfxGlobals->bNecroCorpseInFrustum = TRUE;
                }
            }
            if (nLocalPlayerCount) {
                // Inference: the cached-bit update has no line row (2663:50 row covers it): identical first statements of both branches hoisted.

                if (ptRActor->flLastLOSCheckTime + TIME_BETWEEN_LOS_UPDATES > flCurrTime)
                {
                    if (ptRActor->fLastLOSCheckResult) ptRActor->dwVisibilityFlags |= (1 << RACTOR_NOT_IN_LOS_BIT); else ptRActor->dwVisibilityFlags &= ~(1 << RACTOR_NOT_IN_LOS_BIT);
                }
                else
                {
                    if (ptRActor->fLastLOSCheckResult) ptRActor->dwVisibilityFlags |= (1 << RACTOR_NOT_IN_LOS_BIT); else ptRActor->dwVisibilityFlags &= ~(1 << RACTOR_NOT_IN_LOS_BIT);


                    float flTrunc = Math::FloatTrunc((flCurrTime - ptRActor->flLastLOSCheckTime) / TIME_BETWEEN_LOS_UPDATES);
                    ptRActor->flLastLOSCheckTime = flCurrTime - flTrunc * TIME_BETWEEN_LOS_UPDATES;

                    if (!pUpdateRActorsLOSData || pUpdateRActorsLOSData->uActorsCount >= UpdateRActorsLOSData::MAX_ACTORS)
                    {
                        pUpdateRActorsLOSData = (UpdateRActorsLOSData *)gptPhysicsAPIImpl->AsyncDefer(UpdateRActorsLOSData::Func, sizeof(UpdateRActorsLOSData));
                        pUpdateRActorsLOSData->uActorsCount = 0;
                    }

                    pUpdateRActorsLOSData->aidActors[pUpdateRActorsLOSData->uActorsCount++] = idRActor;
                }






            }







































            ptRActor->dwVisibilityFlags |= (1 << RACTOR_VISIBLE_BIT);
        }
    }

    {
        ptRActorGfxGlobals->plistShadowFrustumActors->RemoveAll();
        const Camera & cam = CameraManagerGetCamera(CAMERA_SHADOW);

        RActorsIntersectFrustum(cam.tFrustum, *ptRActorGfxGlobals->plistShadowFrustumActors);

        POS pos = ptRActorGfxGlobals->plistShadowFrustumActors->GetHeadPosition();
        while (pos)
        {
            RActorID idRActor = (RActorID)ptRActorGfxGlobals->plistShadowFrustumActors->GetNext(pos);



            RActor * ptRActor = RActorTryToGet(idRActor);
            if (!ptRActor) { continue; }




            if (RActorIsHidden(ptRActor))
            {
                continue;
            }

            if (ptRActor->eContext != eContext)
            {
                continue;
            }


            ptRActor->dwVisibilityFlags |= (1 << RACTOR_CASTS_SHADOW_BIT);


            if (!(ptRActor->dwVisibilityFlags & (1 << RACTOR_VISIBLE_BIT)))
            {
                ptRActorGfxGlobals->plistFrustumActors->AddTail_Tracked(idRActor);
            }
        }
    }
}




void RActorGfxRenderRActorInDisplayList(
    const Camera & cam,
    DisplayList & tDisplayList,
    DisplayList * ptDisplayListReflection,
    DisplayList * ptDisplayListProjected,
    RActorID idRActor,
    const BOOL fVisibilityCull,
    const float flDeltaTime,
    const BOOL fClearHighlight)
{
    RActor * ptRActor = RActorTryToGet(idRActor);






    if (ptRActor && !AppDrawFlagIsSet(APP_DRAW_NO_ACTORS_BIT))
    {



        POS pos = ptRActor->tAttachmentList.GetHeadPosition();
        while (pos)
        {
            AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
            Attachment * ptAttachment = AttachmentTryToGet(idAttachment);
            if (ptAttachment)
            {
                switch (ptAttachment->snoname.snoGroup)
                {
                case ACTOR_SNO:
                    {



                        RActorID idRActorAttached = (RActorID)ptAttachment->idAttachedObject;
                        RActorGfxRenderRActorInDisplayList(cam, tDisplayList, ptDisplayListReflection, ptDisplayListProjected, idRActorAttached, fVisibilityCull, flDeltaTime, fClearHighlight);
                    }
                    break;
                case PARTICLE_SNO:
                    {

                        ParticleSystemID idParticleSystemAttached = ptAttachment->idAttachedObject;
                        ParticleSystem * ptParticleSystemAttached = ParticleSystemTryToGet(idParticleSystemAttached);
                        if (ptParticleSystemAttached)
                        {
                            ptParticleSystemAttached->RenderInDisplayList(cam, tDisplayList, flDeltaTime);
                        }
                    }
                    break;
                case ROPE_SNO:
                    {

                        RopeID idRopeAttached = ptAttachment->idAttachedObject;
                        Rope * ptRope = RopeTryToGet(idRopeAttached);
                        if (ptRope)
                        {
                            RopeRenderInDisplayList(ptRope, cam, tDisplayList, flDeltaTime);
                        }
                    }
                    break;
                }
            }
        }

        if (tDisplayList.GetRenderContext() == RENDERCONTEXT_DEFAULT)
        {
            ScopedShaderDefinitionLockR pdefShaderShadowStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_SHADOW_STENCIL_BOUNDS));
            ScopedShaderDefinitionLockR pdefShaderHighlightStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_HIGHLIGHT_STENCIL_BOUNDS));
            sUpdateAndSubmitRActor(ptRActor, NULL, cam, tDisplayList, ptDisplayListReflection, ptDisplayListProjected, flDeltaTime, CGameGetClientTime(), fVisibilityCull, fClearHighlight, pdefShaderShadowStencilBounds, pdefShaderHighlightStencilBounds);
        }
        else
        {
            sUpdateAndSubmitRActor(ptRActor, NULL, cam, tDisplayList, ptDisplayListReflection, ptDisplayListProjected, flDeltaTime, CGameGetClientTime(), fVisibilityCull, fClearHighlight, NULL, NULL);
        }
    }
}






static void sRActorsAttachSpawnedActorsForEditMode(
    DisplayList & dispList)
{
    if (RWindowIDFromContext(dispList.GetRenderContext()) != RWindowMainWindow)
    {
        return;
    }









    CGame & tCGame = CGameGet();
    RActorGfxGlobals * ptRActorGfxGlobals = tCGame.ptRActorGfxGlobals;
    POS pos = ptRActorGfxGlobals->plistFrustumActors->GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)ptRActorGfxGlobals->plistFrustumActors->GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor) { continue; }
        if (
            !ptRActor->ptParticleParams &&

            !RActorIsAttachment(*ptRActor))
        {




        RActorAttachSpawnedActor(ptRActor);
        }
    }
}




























// Reconstructed (linker-removed in the original, DW_AT_low_pc 0): locals and lines from the abstract DIE; body inferred.

void sRActorsSequentialUpdate(
    RWindow & tRWindow,
    const float flDeltaTime,
    const float flClientTime)
{
    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());

    CGame & tCGame = CGameGet();


    ScopedShaderDefinitionLockR pdefShaderShadowStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_SHADOW_STENCIL_BOUNDS));
    ScopedShaderDefinitionLockR pdefShaderHighlightStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_HIGHLIGHT_STENCIL_BOUNDS));





    RActorGfxGlobals * ptRActorGfxGlobals = tCGame.ptRActorGfxGlobals;
    POS pos = ptRActorGfxGlobals->plistFrustumActors->GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)ptRActorGfxGlobals->plistFrustumActors->GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            sUpdateAndSubmitRActor(ptRActor, &cam.tFrustum, cam, tAsyncRVData.dispList, tAsyncRVData.dispListReflections, &tAsyncRVData.dispListProjected, flDeltaTime, flClientTime, TRUE, FALSE, pdefShaderShadowStencilBounds, pdefShaderHighlightStencilBounds);
        }
    }
}










struct ThreadedRActorInfo {  // size 0x98

    RActor * ptRActor;
    const AppearanceDefinition * pdefAppearance;
    float flAlpha;
    int32 nLightCount;
    const LightState * pArrayLights[16];
};

class ActorLightingAndSubObjectStatesUpdateJob {
public:

    static float sflDeltaTime;
    static float sflClientTime;
    static const Frustum * sptFrustum;

    ActorLightingAndSubObjectStatesUpdateJob(
        ThreadedRActorInfo * ptList,
        int32 nStartingIndex,
        int32 nCount)
        : m_ptList(ptList)
        , m_nStartingIndex(nStartingIndex)
        , m_nCount(nCount)
    {
    }


    void Execute()
    {
        GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);

        int32 nStartingIndex = m_nStartingIndex;
        int32 nCount = m_nCount;

        float flDeltaTime = sflDeltaTime;
        float flClientTime = sflClientTime;
        for (int32 i = 0; i < nCount; i++)
        {
            ThreadedRActorInfo & info = m_ptList[nStartingIndex + i];



            info.nLightCount = sRenderUpdateActorLightingAndSubObjectStates(info.ptRActor, info.pdefAppearance, sptFrustum, 15, info.pArrayLights, flDeltaTime, flClientTime);
        }
    }

    static void JobFcn(void * pParam)
    {
        ActorLightingAndSubObjectStatesUpdateJob * pThis = (ActorLightingAndSubObjectStatesUpdateJob *)pParam;
        pThis->Execute();
    }

private:
    ThreadedRActorInfo * m_ptList;
    int32 m_nStartingIndex;
    int32 m_nCount;
};
float ActorLightingAndSubObjectStatesUpdateJob::sflDeltaTime;
float ActorLightingAndSubObjectStatesUpdateJob::sflClientTime;
const Frustum * ActorLightingAndSubObjectStatesUpdateJob::sptFrustum;



class ActorRenderSubmitJob {
public:

    ActorRenderSubmitJob(
        ThreadedRActorInfo * ptList,
        int32 nStartingIndex,
        int32 nCount)
        : m_ptList(ptList)
        , m_nStartingIndex(nStartingIndex)
        , m_nCount(nCount)
    {
    }


    void Execute()
    {
        GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);

        int32 nStartingIndex = m_nStartingIndex;
        int32 nCount = m_nCount;

        for (int32 i = 0; i < nCount; i++)
        {
            ThreadedRActorInfo & info = m_ptList[nStartingIndex + i];

            sRenderUpdateActorSubmit(info.ptRActor, info.pdefAppearance, *stCamera, stAsyncRVData->dispList, stAsyncRVData->dispListReflections, &stAsyncRVData->dispListProjected, sflDeltaTime, sflClientTime, TRUE, FALSE, info.nLightCount, info.pArrayLights, info.flAlpha, spdefShaderShadowStencilBounds, spdefShaderHighlightStencilBounds);
        }
    }


    static void JobFcn(void * pParam)
    {
        ActorRenderSubmitJob * pThis = (ActorRenderSubmitJob *)pParam;
        pThis->Execute();
    }

    static const Camera * stCamera;
    static AsyncRWindowData * stAsyncRVData;
    static float sflDeltaTime;
    static float sflClientTime;
    static const ShaderDefinition * spdefShaderShadowStencilBounds;
    static const ShaderDefinition * spdefShaderHighlightStencilBounds;

private:
    ThreadedRActorInfo * m_ptList;
    int32 m_nStartingIndex;
    int32 m_nCount;
};
const Camera * ActorRenderSubmitJob::stCamera;
AsyncRWindowData * ActorRenderSubmitJob::stAsyncRVData;
float ActorRenderSubmitJob::sflDeltaTime;
float ActorRenderSubmitJob::sflClientTime;
const ShaderDefinition * ActorRenderSubmitJob::spdefShaderShadowStencilBounds;
const ShaderDefinition * ActorRenderSubmitJob::spdefShaderHighlightStencilBounds;






// NON_MATCHING: 1104 vs 908 bytes; guard form (continue on flAlpha == 0, continue on full array) follows the DWARF (flAlpha and info share one block, bActorLimitHit const 0) and rows 3124:17/3135:18, but ours jump-threads a second copy of the loop for the full-array state; nested if/else (916) and nested continue (924) contradict the block structure. Tried >= and sizeof-count forms.
void sRActorsThreadedUpdate(
    RWindow & tRWindow,
    const float flDeltaTime,
    const float flClientTime)
{
    ThreadedRActorInfo arThreadedRActorInfo[1024];
    int32 nVisibleRActorCount = 0;
    BOOL bActorLimitHit = FALSE;

    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    const Camera & cam = CameraManagerGetCamera(tRWindow.GetCameraIndex());
    CGame & tCGame = CGameGet();
    RActorGfxGlobals * ptRActorGfxGlobals = tCGame.ptRActorGfxGlobals;
    POS pos = ptRActorGfxGlobals->plistFrustumActors->GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)ptRActorGfxGlobals->plistFrustumActors->GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);

        if (ptRActor &&
            ptRActor->ptParticleParams == NULL)
        {
            sRenderUpdateActorDebugDraw(ptRActor);

            float flAlpha = sRenderUpdateActorVisibility(ptRActor, tAsyncRVData.dispList, flDeltaTime);
            if (flAlpha == 0.0f)
            {
                continue;
            }

            if (nVisibleRActorCount == 1024)
            {
                bActorLimitHit = TRUE;
                continue;
            }





            ThreadedRActorInfo & info = arThreadedRActorInfo[nVisibleRActorCount++];
            info.ptRActor = ptRActor;
            info.flAlpha = flAlpha;
            info.pdefAppearance = (const AppearanceDefinition *)gptGroupDefAppearance->LockR(ptRActor->snoAppearance, FALSE);


        }
    }
    if (nVisibleRActorCount > 0)
    {
        char arJobBuffer[4096];

        ActorLightingAndSubObjectStatesUpdateJob::sflDeltaTime = flDeltaTime;
        ActorLightingAndSubObjectStatesUpdateJob::sflClientTime = flClientTime;
        ActorLightingAndSubObjectStatesUpdateJob::sptFrustum = &cam.tFrustum;

        int32 nIncrement = nVisibleRActorCount / 256 + 1;
        int32 nJobIndex = 0;

        for (int32 i = 0; i < nVisibleRActorCount; i += nIncrement, nJobIndex++)
        {
            void * pBuffer = &arJobBuffer[nJobIndex * sizeof(ActorLightingAndSubObjectStatesUpdateJob)];
            ActorLightingAndSubObjectStatesUpdateJob * ptJob = new (pBuffer) ActorLightingAndSubObjectStatesUpdateJob(arThreadedRActorInfo, i, Math::Min(nIncrement, nVisibleRActorCount - i));








            JobManager::Get()->PushJob(ActorLightingAndSubObjectStatesUpdateJob::JobFcn, ptJob);
        }


        JobManager::Get()->WaitForIdle();
    }


    ScopedShaderDefinitionLockR pdefShaderShadowStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_SHADOW_STENCIL_BOUNDS));
    ScopedShaderDefinitionLockR pdefShaderHighlightStencilBounds(GlobalSNOGet(GLOBALSNO_SHADER_HIGHLIGHT_STENCIL_BOUNDS));






    if (nVisibleRActorCount > 0)
    {
        char arJobBuffer[4096];

        ActorRenderSubmitJob::stCamera = &cam;
        ActorRenderSubmitJob::stAsyncRVData = &tAsyncRVData;
        ActorRenderSubmitJob::sflDeltaTime = flDeltaTime;
        ActorRenderSubmitJob::sflClientTime = flClientTime;
        ActorRenderSubmitJob::spdefShaderShadowStencilBounds = pdefShaderShadowStencilBounds;
        ActorRenderSubmitJob::spdefShaderHighlightStencilBounds = pdefShaderHighlightStencilBounds;

        int32 nIncrement = nVisibleRActorCount / 256 + 1;
        int32 nJobIndex = 0;

        for (int32 i = 0; i < nVisibleRActorCount; i += nIncrement, nJobIndex++)
        {
            void * pBuffer = &arJobBuffer[nJobIndex * sizeof(ActorRenderSubmitJob)];
            ActorRenderSubmitJob * ptJob = new (pBuffer) ActorRenderSubmitJob(arThreadedRActorInfo, i, Math::Min(nIncrement, nVisibleRActorCount - i));
            JobManager::Get()->PushJob(ActorRenderSubmitJob::JobFcn, ptJob);
        }

        JobManager::Get()->WaitForIdle();

        for (int32 i = 0; i < nVisibleRActorCount; i++)
        {
            ThreadedRActorInfo & info = arThreadedRActorInfo[i];
            gptGroupDefAppearance->UnlockR((void *&)info.pdefAppearance);
        }
    }


    ParticlesMultiRenderUpdateKick();
}


















void RActorGfxRenderUpdate(
    RWindow & tRWindow,
    const float flDeltaTime,
    const float flClientTime)
{
    if (AppDrawFlagIsSet(APP_DRAW_NO_ACTORS_BIT))
    {
        ParticlesMultiRenderUpdateKick();
        return;
    }



    AsyncRWindowData & tAsyncRVData = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    sRActorsUpdateVisibility(tRWindow);

    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode == GAMEMODE_EDITUNITLAYOUT)
    {
        sRActorsAttachSpawnedActorsForEditMode(tAsyncRVData.dispList);
    }









    sRActorsThreadedUpdate(tRWindow, flDeltaTime, flClientTime);
}
