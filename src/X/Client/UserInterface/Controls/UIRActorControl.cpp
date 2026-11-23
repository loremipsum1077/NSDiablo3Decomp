// Original file: X/Client/UserInterface/Controls/UIRActorControl.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIRActorControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIAttributes.h"
#include "../UIMessage.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../CGame.h"
#include "../../Lights.h"
#include "../../Input/Input.h"
#include "../../RActors/RActors.h"
#include "../../RActors/RActorAttachments.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Attributes/FastAttributeStructs.h"
#include "../../Movement/CActorMovement.h"
#include "../../../Common/Game.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Animation.h"
#include "../../RActors/RActorAnimation.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Definitions/AnimationDefinition.h"
#include "../../../Common/Definitions/AnimTreeDefinition.h"
#include <string.h>
// Declared here because their headers are not adopted yet (RActors.cpp, Lights.cpp, Input.cpp, UI.cpp, Gfx.cpp, Sprite.cpp).
void RActorsUpdateFixedUpdateFlag(); LightID LightAdd(const uint32 dwFlags, const SNO snoLight, const PRTransform & transform, const int32 nUnused, const RenderContext eContext); IVector2D InputGetCursorPos(); const UIID & UI_GetCapture(); void UI_SetCapture(const UIID & idCapture); BOOL UI_DrawingFullscreenUI(); BOOL GfxIsLetterboxed(); float GfxGetAspectRatio();
#include "../../../Common/AnimCommon.h"

WeaponClass GetWeaponClassForAnim(const PlayerClass ePlayerClass, const GBID gbidLeft, const GBID gbidRight); BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor); BOOL RActorGetAnimLayerForTag(RActor * ptRActor, uint32 dwAnimationTag, AnimLayer & eAnimLayer); uint32 RActorGetAnimTreeLayerHash(AnimLayer eAnimLayer); BOOL RActorGetAnimTreeManualRampOut(RActor * ptRActor, uint32 dwAnimationTag);
SNO AnimTreeLayerGetSNO(const AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash); void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim, RNG & rng); void AnimationChannelPlay(RActor * ptRActor, const AnimChannelPlayParams & tChannelPlayParams, BOOL bDisableBlending); void AnimTreeLayerPlay(RActor * ptRActor, uint32 dwLayerHash, const AnimChannelPlayParams & tChannelPlayParams, BOOL bArg3, BOOL bManualRampOut);
void SpriteDrawRActorSprite(const int32 nFlags, DisplayList * ptDisplayList, const RActorID idRActor, const Rect2D & wrWindow, const Rect2D & wrClip, const float flDeltaTime, const float flLayerZ, const SNO snoObserver, const SNO snoRenderTarget, const float flAspectRatio, const BOOL fClearZBuffer); void SpriteDrawFullscreenRActorSprite(const int32 nFlags, DisplayList * ptDisplayList, const RActorID idRActor, const float flDeltaTime, const float flLayerZ, const SNO snoRenderTarget, const RenderLayer eRenderLayer);


inline void RActorCreationParams::sInit()
{

    snoActor = SNO_NONE; snoObserver = SNO_NONE;

    snoLight = SNO_NONE;
    eRenderContext = RENDERCONTEXT_DEFAULT;
    fClearZBuffer = FALSE; fRotationEnabled = FALSE; fIsStaticRActor = FALSE;
}







RActorCreationParams::RActorCreationParams(const UIType eType, const UIID & id, const UIID & idParent)



    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}
#include "../UIUtils.h"
#include "../../Definitions/AppearanceDefinition.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../Gfx/GfxMaterial.h"
#include "../../../Sigma/Math/Ray.h"
#include "../../../Sigma/Math/Intersect.h"
#include "../../../Sigma/Math/Triangle.h"
#include "../../PostFX.h"
#include <float.h>
// Declared here: X/Client/RActors/RActorCollision.h:31 and the RActorCollision.cpp, UIUtils.cpp, AppearanceDefinition.cpp, TextureDefinition.cpp and GfxMaterial.cpp entry points.
struct RActorExactIntersectionResult { Triangle3D wtri; WorldDistance wdT; int32 nSubobject; int32 nSegment; int32 nIndexListIndex; RActorExactIntersectionResult() : wdT(FLT_MAX), nSubobject(-1), nSegment(-1), nIndexListIndex(-1) {} }; BOOL RActorDoesRayIntersectExact(const RActor * ptRActor, const Ray3D & wray, const GeoSetType eGeoSetType, const BOOL fArg3, RActorExactIntersectionResult & tResult); void ConvertPixelsToUIC(const IVector2D & vecPixels, Vector2D & vecUIC); void BarycentricCoordsToTextureUV(const SNO snoAppearance, const int32 nArg1, const int32 nSubobject, const int32 nSegment, const int32 nIndexListIndex, const int32 nArg5, const float flUTriangle, const float flVTriangle, float & flU, float & flV); SubObjectAppearance * AppearanceGetSubObjectAppearance(const AppearanceDefinition * pdefAppearance, const int32 nArg1, const int32 nSubobject, const int32 nArg3); MaterialTextureEntry * MaterialLookup(UberMaterial & tMaterial, ShaderTex eShaderTex); void TextureDefinitionPixelLookup(const SNO snoTexture, const float flU, const float flV, const int32 nMip, RGBAColor & rgbaColor);





static SNO sResolveSNO(SNO snoUI, SNOGroup snoGroup, LPCSTR szSNOName)
{
    if (szSNOName && szSNOName[0] && strcmp(szSNOName, "-1") != 0) {

        return SNOFileLookupGUID(snoGroup, szSNOName);
    }
    return SNO_NONE;
}








void RActorCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{


    WindowCreationParams::Load(snoUI, pControlData);

    snoActor = sResolveSNO(snoUI, ACTOR_SNO, pControlData->GetAttributeValue(ATT_ACTOR));
    snoObserver = sResolveSNO(snoUI, OBSERVER_SNO, pControlData->GetAttributeValue(ATT_OBSERVER));
    snoLight = sResolveSNO(snoUI, LIGHT_SNO, pControlData->GetAttributeValue(ATT_LIGHT));
    eRenderContext = (RenderContext)pControlData->GetInt32Value(ATT_RENDERCONTEXT);
    fClearZBuffer = pControlData->GetBoolValue(ATT_CLEAR_Z_BUFFER);
    fRotationEnabled = pControlData->GetBoolValue(ATT_ROTATION_ENABLED);
    if (pControlData->AttributeExists(ATT_STATIC_ACTOR)) {

        fIsStaticRActor = pControlData->GetBoolValue(ATT_STATIC_ACTOR);
    }
    else {

        fIsStaticRActor = (snoActor != SNO_NONE);
    }

    if (pControlData->AttributeExists(ATT_FULLSCREEN_ELEMENT)) {

        fFullScreenOverride.Set(pControlData->GetBoolValue(ATT_FULLSCREEN_ELEMENT));
    }
}




inline void UIRActor::sInit(const RActorCreationParams & tParams)
{

    m_snoActor = tParams.snoActor;
    m_snoLight = tParams.snoLight;
    SetObserver(tParams.snoObserver);
    m_fClearZBuffer = tParams.fClearZBuffer;
    m_fRotationEnabled = tParams.fRotationEnabled;
    m_fIsStaticRActor = tParams.fIsStaticRActor;
    m_ptAsynchRActorCreator = NULL;









    m_idRActor = RACTORID_INVALID; m_idLight = -1;


    m_vecRotationPixels = IVector2D(0, 0);
    m_eEffectBuffer = EFFECTBUFFER_NONE;
    if (tParams.fFullScreenOverride.Exists()) {

        fFullScreenOverRide.Set(tParams.fFullScreenOverride.Get());
    }

    m_fIsFullscreenUIElement = FALSE;
    m_ePlayerClass = -1; m_dwPlayerFlags = 0;

    reinterpret_cast<VisualEquipment *>(_unadopted1)->Init();



    for (int32 i = 0; i < 2; ++i) {
        tRenderData[i].m_tDisplayList.Initialize(tParams.eRenderContext, TRUE);
        tRenderData[i].m_tDisplayListHighlight.Initialize(tParams.eRenderContext, TRUE);
    }
    SetRenderContext(tParams.eRenderContext);
}
















UIRActor::UIRActor(const RActorCreationParams & tParams, const Rect2D * pRect)


    : UIWindow(tParams, pRect)
{
    sInit(tParams);
}




UIRActor::~UIRActor()
{

    CGameAsyncRenderFlush();


    for (int32 i = 0; i < 2; ++i) {

        tRenderData[i].m_tDisplayListHighlight.Dispose();
        tRenderData[i].m_tDisplayList.Dispose();
    }
    ::delete m_ptAsynchRActorCreator; m_ptAsynchRActorCreator = NULL;
}




BOOL UIRActor::ClickIsEaten(const IVector2D & p) const
{

    EatsClicksValue ecValue = EatsClicks();
    switch (ecValue) {

    case EC_EAT:
        return TRUE;

    case EC_EAT_MASKED: {


        if (!IsFullScreenElement()) {

            DisplayInternalError(NULL, "", 230, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }


        RActor * ptRActor = RActorTryToGet(m_idRActor);

        if (!ptRActor) { return FALSE; }



            Vector2D pUIC;
            ConvertPixelsToUIC(p, pUIC);


            Vector3D wpOrigin(
                100.0f,
                (pUIC.x - 480.0f) * 0.01f,
                (360.0f - pUIC.y) * 0.01f);

            Vector3D wvDir = -cgwvXAxis;
            Ray3D wray;
            wray.Set(wpOrigin, wvDir);

            const GeoSetType eGeoSetType = 0;
            RActorExactIntersectionResult tResult;
            BOOL bResult = RActorDoesRayIntersectExact(ptRActor, wray, eGeoSetType, FALSE, tResult);
            if (bResult) {

                float flUTriangle;
                float flVTriangle;
                if (Intersect::Ray3DAndTriangle3D(wray, tResult.wtri, flUTriangle, flVTriangle)) {


                    float flU, flV;
                    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
                    BarycentricCoordsToTextureUV(ptRActor->snoAppearance, 0, tResult.nSubobject, tResult.nSegment, tResult.nIndexListIndex, 0, flUTriangle, flVTriangle, flU, flV);

                    SubObjectAppearance & tSOA = *AppearanceGetSubObjectAppearance(pdefAppearance, 0, tResult.nSubobject, 0);
                    const MaterialTextureEntry * ptMaterial = MaterialLookup(tSOA.tCompositeUberMaterial, SHADERTEX_DIFFUSE);

                    RGBAColor rgbaColor;
                    if (!ptMaterial) { return FALSE; }

                    TextureDefinitionPixelLookup(ptMaterial->tMatTex.snoTex, flU, flV, 0, rgbaColor);
                    if (rgbaColor.a == 255) { return TRUE; }








                }
            }



        break;
    }
    default:
        return FALSE;
    }
    return FALSE;
}


UIID UIRActor::HitTest(const IVector2D & pt)
{

    if (IsActive()) {




        if (IsInside(pt)) {




            if (ClickIsEaten(pt)) {

                return GetID();
            }
        }
    }

    return GetUIIDNone();
}


UIID UIRActor::HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting)
{


    if (IsActive()) {




        if (IsInside(pt)) {












            LPCSTR pszTooltipText = GetTooltipText();
            if (pszTooltipText != NULL && pszTooltipText[0] != '\0') {

                return GetID();
            }




            if (ClickIsEaten(pt)) {

                bContinueTesting = FALSE;
            }
        }
    }

    return GetUIIDNone();
}




void UIRActor::Cleanup()
{

    RActorDelete(m_idRActor);
    m_idRActor = RACTORID_INVALID;
    LightDelete(m_idLight);
    m_idLight = -1;
    ::delete m_ptAsynchRActorCreator; m_ptAsynchRActorCreator = NULL;
}




inline void UIRActor::SetRenderContext(const RenderContext eRenderContext)
{

    m_eRenderContext = eRenderContext;
    for (int32 i = 0; i < 2; ++i) {

        tRenderData[i].m_tDisplayList.SetRenderContext(eRenderContext);
        tRenderData[i].m_tDisplayListHighlight.SetRenderContext(eRenderContext);
    }
}




RenderContext UIRActor::GetRenderContext()
{

    return m_eRenderContext;
}




void UIRActor::Draw(const uint8 bLayerAlpha, float & flZSlice)
{


    const uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
    if (bAlphaCascaded == 0) {
        return;
    }


    if (AppRenderLayerIsHidden(RENDERLAYER_UI)) {
        return;
    }












    if (UI_DrawingFullscreenUI()) {
        return;
    }


    UIWindow::Draw(bLayerAlpha, flZSlice);

    const float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
    flZSlice += 1.0f;

    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (!ptRActor) {
        return;
    }


    ptRActor->flAlpha = bAlphaCascaded / 255.0f;
    const float flDeltaTime = CGameGetRealDeltaTime();
    UIRActor::RenderData & tData = tRenderData[CGameAsyncGetRenderDataIndex(0)];

    const SNO snoRenderTarget = PostFXGetBuffer((EffectBuffer)m_eEffectBuffer);
    if (IsFullScreenElement()) {

        SpriteDrawFullscreenRActorSprite(-1, &tData.m_tDisplayList, m_idRActor, flDeltaTime, flLayerZ, snoRenderTarget, RENDERLAYER_UI);



        if (!AppDrawFlagIsSet(APP_DRAW_NO_POSTFX_OUTLINE_BIT) &&
            sgtAppGlobals.eDebugRenderMode != DRM_DEPTH_COMPLEXITY &&
            AppDrawFlagIsSet(APP_DRAW_HWCLASS_UI_OUTLINES_BIT))
        {
            SpriteDrawFullscreenRActorSprite(-1, &tData.m_tDisplayListHighlight, m_idRActor, flDeltaTime, flLayerZ, snoRenderTarget, RENDERLAYER_HIGHLIGHT);
        }
    }
    else {



        Rect2D wrWindow; GetWindowRect(wrWindow);
        const Rect2D & wrCascadingRect = GetCascadedClippingRect(); Rect2D wrFinalRect;
        if (Rect2D::Intersect(wrWindow, wrCascadingRect, &wrFinalRect)) {












            SpriteDrawRActorSprite(4, &tData.m_tDisplayList, m_idRActor, wrWindow, wrFinalRect, flDeltaTime, flLayerZ, m_snoObserver,
                snoRenderTarget, GfxIsLetterboxed() ? (16.0f / 9.0f) : GfxGetAspectRatio(), m_fClearZBuffer);
        }
    }
}













void UIRActor::SetRActor(const RActorID idRActor)
{

    RActor * ptRActor = RActorTryToGet(idRActor);

    if (ptRActor) {


        m_idRActor = idRActor;

        ptRActor->dwFlagsEx |= 1;

        RActorsUpdateFixedUpdateFlag();
    }
}



void UIRActor::DeleteRActor()
{

    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor) {
        RActorDelete(m_idRActor);
    }
    m_idRActor = RACTORID_INVALID;

    RActorsUpdateFixedUpdateFlag();
}

























void UIRActor::CreateRActor(const RenderContext eContext, const SNO snoActor)
{



    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor) {

        RActorDelete(m_idRActor);
    }
    m_idRActor = RACTORID_INVALID;

    if (snoActor != SNO_NONE) {


        PRTransform transform(cgqIdentity, cgwpNull);
        SetRenderContext(eContext);

        m_fIsFullscreenUIElement = TRUE;

        RActorID idRActorNew = RActorCreate(snoActor, 1.0f, -1.0f, transform, 1, eContext, -1);
        SetRActor(idRActorNew);
    }
}





RActorID UIRActor::CreateRActorForCurrentContext(const SNO snoActor, const float flScale, const float flLife, const PRTransform & transform, const uint32 dwRActorAddFlags)
{






    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor) {

        RActorDelete(m_idRActor);
    }
    m_idRActor = RACTORID_INVALID;

    if (snoActor != SNO_NONE) {


        RActorID idRActorNew = RActorCreate(snoActor, flScale, flLife, transform, dwRActorAddFlags, GetRenderContext(), -1);
        SetRActor(idRActorNew);
    }

    return m_idRActor;
}




void UIRActor::PlayAnimForEquipment(RActor * ptRActor, const VisualEquipment & tVisualEquipment, const PlayerClass ePlayerClass, const uint32 dwAnimTag, BOOL bDisableBlending)
{




    const VisualItem & tLeftItem = tVisualEquipment.tVisualItem[4];
    const VisualItem & tRightItem = tVisualEquipment.tVisualItem[5];

    WeaponClass eWeaponClass = GetWeaponClassForAnim(ePlayerClass, tLeftItem.gbidItem, tRightItem.gbidItem);

    SNO snoAnim = ActorGetAnim(ptRActor->snoActor, eWeaponClass, dwAnimTag);

    if (!ptRActor->ptActorAnim) {

        return;
    }

    if (!RActorAnimationIsAnimTreeEnabled(*ptRActor)) {

        AnimChannel & tAnimChannel = ptRActor->ptActorAnim->tAnimChannel;
        if (tAnimChannel.tAnimPermID.snoAnim == snoAnim) {

            return;
        }
    }
    else {

        AnimLayer eAnimLayer = ANIM_LAYER_NONE;
        if (RActorGetAnimLayerForTag(ptRActor, dwAnimTag, eAnimLayer)) {

            uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
            ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);
            if (AnimTreeLayerGetSNO(*ptRActor->ptActorAnim->ptAnimTree, *pdefAnimTree, dwAnimLayerHash) == snoAnim) {

                return;
            }
        }
    }

    RNG & rng = CGameGlobalsGet().rngLocal;
    AnimChannelPlayParams tChannelPlayParams;

    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
    BOOL bAnimTreeEnabled = RActorAnimationIsAnimTreeEnabled(*ptRActor);
    if (!bAnimTreeEnabled) {

        AnimationChannelPlay(ptRActor, tChannelPlayParams, bDisableBlending);
    }
    else {

        AnimLayer eAnimLayer = ANIM_LAYER_NONE;
        if (RActorGetAnimLayerForTag(ptRActor, dwAnimTag, eAnimLayer)) {

            uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
            BOOL bManualRampOut = RActorGetAnimTreeManualRampOut(ptRActor, dwAnimTag);
            AnimTreeLayerPlay(ptRActor, dwAnimLayerHash, tChannelPlayParams, TRUE, bManualRampOut);
        }
    }
}



inline void UIRActor::sRecreateActor()
{

    if (m_snoActor != SNO_NONE) {

        CreateRActor(m_eRenderContext, m_snoActor);
    }
    if (m_snoLight != SNO_NONE) {

        PRTransform transform(cgqIdentity, cgwpNull);
        SetLight(LightAdd(0, m_snoLight, transform, 0, m_eRenderContext));
    }
}



BOOL UIRActor::UpdateCachedActiveState()
{

    BOOL fWasActive = UIControlGroup::UpdateCachedActiveState();
    if (m_fIsStaticRActor) {

        BOOL fActive = IsActive();
        if (fWasActive != fActive) {

            if (fActive && CGameGlobalsTryToGet() && CGameGlobalsGet().idCurrentCWorld != -1) {


                sRecreateActor();
            }
            else {

                Cleanup();
            }
        }
    }
    return fWasActive;
}




BOOL UIRActor::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{


    switch (msg)
    {


    case UI_INIT_FOR_WORLD:
        if (IsActive()) {
            sRecreateActor();
        }
        break;


    case UI_DISPOSE_FOR_WORLD:

        Cleanup();
        break;


    case UI_INPUT:
    {
        BufferedInput & tInput = *(BufferedInput *)dwParam;
        if (tInput.eInputMsg == INPUTMSG_LEFTMOUSEDOWN) {



            if (m_fRotationEnabled && IsInside(tInput.tBufMouseData.p)) {


                m_vecRotationPixels = tInput.tBufMouseData.p;
                UI_SetCapture(GetID());
                return TRUE;
            }
        }


        else if (tInput.eInputMsg == INPUTMSG_LEFTMOUSEUP) {

            if (UI_GetCapture() == GetID()) {

                UI_SetCapture(GetUIIDNone()); return TRUE;
            }
        }
        break;
    }





    case UI_UPDATE:
    {
        if (m_fRotationEnabled && UI_GetCapture() == GetID()) {


            RActor * ptRActor = RActorTryToGet(m_idRActor);
            if (ptRActor) {


                Rect2D rectWindow; GetWindowRect(rectWindow);
                float flDiff = (float)InputGetCursorPos().x - (float)m_vecRotationPixels.x;
                float flAbsDiff = Math::Abs(flDiff);
                float xNormalDiff = (Math::Min(flAbsDiff, rectWindow.Width()) / rectWindow.Width()) * (flDiff != 0.0f ? flDiff / flAbsDiff : 1.0f);
                xNormalDiff *= PI / 16.0f;
                if (ptRActor->ptCActorMovement) {

                    ptRActor->ptCActorMovement->SetNewFacing(ptRActor->aYaw + xNormalDiff, TRUE, TRUE);
                }
                else {

                    ptRActor->aYaw += xNormalDiff;
                }
                return TRUE;
            }
        }
        if (m_ptAsynchRActorCreator && m_ptAsynchRActorCreator->NeedsUpdate()) {

            RActorID idRActor = m_ptAsynchRActorCreator->TryToCreateRActor(m_eRenderContext);
            if (idRActor != RACTORID_INVALID) {

                SetRActor(idRActor);
                ::delete m_ptAsynchRActorCreator; m_ptAsynchRActorCreator = NULL;
            }
        }

        break;
    }
    default:
        break;
    }

    return UIWindow::HandleMessage(msg, dwParam);
}






void UIRActor::AttachParticleSystem(const SNO snoParticle, const Vector2D & vecUIC, const FastAttribKey * ptAttribKey)
{

    const float UIC_TO_RACTOR_SCALE_FACTOR = 0.01f;

    RActor * ptRActor = RActorGet(m_idRActor);




    Vector3D vecPos(0.1f, (vecUIC.x - 480.0f) * UIC_TO_RACTOR_SCALE_FACTOR,
        (360.0f - vecUIC.y) * UIC_TO_RACTOR_SCALE_FACTOR);
    Hardpoint tDynamicHardpoint;
    tDynamicHardpoint.tHash = 0u;
    tDynamicHardpoint.nBoneIndex = -1;
    tDynamicHardpoint.transform.Set(cgqIdentity, vecPos);

    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_PARTICLESYSTEM;
    tTriggerEvent.snoname.Set(PARTICLE_SNO, snoParticle);

    if (ptAttribKey) {

        tTriggerEvent.dwID = ptAttribKey->nValue + INTERNAL_ATTACHMENTID_ATTRIB_PARTICLES;
    }

    AttachmentID idAttachment = RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, &tDynamicHardpoint, 1, -1.0f, FALSE, 0.0f, 0);

    if (ptAttribKey) {

        if (!ptRActor->ptEffectAttachments) {

            ptRActor->ptEffectAttachments = new AppearanceAttributeAttachmentIDMap(16, NULL);
        }

        ptRActor->ptEffectAttachments->SetAt_Tracked(*ptAttribKey, idAttachment);
    }
}
