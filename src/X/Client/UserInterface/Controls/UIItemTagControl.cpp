// X/Client/UserInterface/Controls/UIItemTagControl.cpp

#include "UIItemTagControl.h"

#include "../../CPlayer.h"
#include "../../CPlayerInput.h"
#include "../../Camera/Camera.h"
#include "../../Camera/CameraManager.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Gfx/Gfx.h"
#include "../../ImageCollection.h"
#include "../../Input/Input.h"
#include "../../RActors/RActors.h"
#include "../../Sprite.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIUtils.h"
#include "../../../Common/GlobalSNO.h"

inline void UIItemTag::sInit()  // line 30
{
    m_idRActor = RACTORID_INVALID;
    m_fDrawIcon = TRUE;
    m_rgbaQuality = crgbaWhite;
    SetUseDropShadow(TRUE);
    SetFont(GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF));
    m_nPlayerOwnerIndex = -1;
    SetFontSize(16);
    SetJustification(UIJH_CENTER, UIJV_MIDDLE);
    UIButton::SetTextColor(crgbaWhite);
}


UIItemTag::UIItemTag(const ButtonCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)  // line 53
    : UIButton(tParams, vecTL, vecDim)
{
    sInit();
}


UIItemTag::UIItemTag(const ButtonCreationParams & tParams, const Rect2D * pRect)  // line 65
    : UIButton(tParams, pRect)
{
    sInit();
}

void UIItemTag::DrawItemRarity(const uint8, float & flZSlice)  // line 97
{
    Rect2D rectWindowUIC;
    GetWindowRect(rectWindowUIC);
    if (GetWindowColor().a == 0) {
        return;
    }

    RGBAColor rgbaColor = m_rgbaQuality;
    rgbaColor.a = GetWindowColor().a;

    ImageTextureFrame tImageTexFrame;
    ImageCollectionGetImageInfo("UI:Console_EquippedInventoryLootRing", tImageTexFrame);

    if (tImageTexFrame.snoTexture != SNO_NONE) {
        Rect2D rectItemPixels;
        ConvertUICToPixels(rectWindowUIC, rectItemPixels);

        SNO snoShader = SNO_NONE;
        {
            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;

            if (m_bMirrorX || m_bMirrorY) {
                SpriteDrawDeferredMirrored(tImageTexFrame, snoShader, rectItemPixels, rgbaColor, crgbaBlack, flLayerZ, NULL, m_bMirrorX, m_bMirrorY, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID);
            } else {
                SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, snoShader, rectItemPixels, rgbaColor, crgbaBlack, flLayerZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
            }
        }
    }
}

void UIItemTag::Draw(const uint8 bLayerAlpha, float & flZSlice)  // line 164
{
    RActor * ptRActor = RActorTryToGet(m_idRActor);
    if (ptRActor != NULL && GfxStereo3DEnabled()) {
        Vector3D wp = CameraManagerGetCamera(CAMERA_DEFAULT_VIEW).cmState.transform.wp;
        float flDist = Vector3D::Distance(ptRActor->transformClient.wp, wp);
        SetStereoDepth(flDist);
    }

    if (m_fDrawIcon) {
        if (ptRActor != NULL && ((ptRActor->dwFlags >> RACTOR_HIGHLIGHT_BIT) & 1)) {
            return;
        }
        RGBAColor rgbaColor = crgbaWhite;
        rgbaColor.a = GetWindowColor().a;
        SetWindowColor(rgbaColor);
        UIButton::Draw(bLayerAlpha, flZSlice);
        DrawItemRarity(bLayerAlpha, flZSlice);
    } else {
        RGBAColor rgbaColor = crgbaBlack;
        if (!Prefs::GetGameplayPrefs().GetItemTagsAsIcons()) {
            if (CPlayerCanHighlightItem(LocalPlayerGet())) {
                BOOL fHighlight = (GetButtonState() == BS_MOUSEOVER);
                if (!fHighlight && ptRActor != NULL) {
                    fHighlight = (ptRActor->dwFlags >> RACTOR_HIGHLIGHT_BIT) & 1;
                }
                if (fHighlight) {
                    rgbaColor = crgbaOrange;
                }
            }
        }
        rgbaColor.a = GetWindowColor().a;
        SetWindowColor(rgbaColor);
        UIButton::Draw(bLayerAlpha, flZSlice);
    }
}

UIID UIItemTag::HitTest(const IVector2D & pt)  // line 225
{
    if (!IsActive()) {
        return GetUIIDNone();
    }
    if (!CPlayerCanHighlightItem(LocalPlayerGet())) {
        return GetUIIDNone();
    }
    if (IsInside(pt)) {
        return GetID();
    }
    return GetUIIDNone();
}

BOOL UIItemTag::sInput(const BufferedInput & tInput)  // line 248
{
    switch (tInput.eInputMsg) {
    case INPUTMSG_LEFTMOUSEDOWN: {
        if (UI_GetMousedOverControl() == GetUIIDNone()) {
            return FALSE;
        }
        if (EatsClicks() == EC_IGNORE) {
            return FALSE;
        }
        RActor * ptPlayerRActor = RActorTryToGet(LocalPlayerRActorGet());
        if (ptPlayerRActor == NULL) {
            return FALSE;
        }
        return CPlayerInputClickToPickupRActor(ptPlayerRActor, m_idRActor);
    }
    case INPUTMSG_RIGHTMOUSEDOWN:
        return PlayingInput(tInput);
    default:
        break;
    }
    return FALSE;
}

BOOL UIItemTag::HandleMessage(const UIMsg & msg, const uintptr dwParam)  // line 290
{
    if (msg == UI_INPUT) {
        BOOL bRetval = sInput(*(const BufferedInput *)dwParam);
        return bRetval;
    }
    return UIButton::HandleMessage(msg, dwParam);
}
