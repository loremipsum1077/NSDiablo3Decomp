// Original file: X/Client/UserInterface/Controls/UIHotbarButtonControl.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIHotbarButtonControl.h"
#include "UITextControl.h"
#include "UITimerControl.h"
#include "UIConsoleSkillsTimerControl.h"
#include "UIRechargeTimerControl.h"
#include "UIItemButtonControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIAttributes.h"
#include "../UICreation.h"
#include "../UIUtils.h"
#include "../../Sprite.h"
#include "../../Prefetch.h"
#include "../../ImageCollection.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../CPlayer.h"
// Declared here because their headers are not adopted yet (UI.cpp, UIInventory.cpp, UISkills.cpp, UIBuffs.cpp, CActorInventory.cpp).
UITimer & UI_GetTimerFromID(const UIID & id); void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot); void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType);
namespace UIInventory { void UpdateTimerVisibility(UITimer & tTimer, const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem); void DrawItemOverlay(const Rect2D & rectItemUIC, int32 eOverlay, float flAlpha, float & flZ, const Rect2D * prectClippingUIC); }
namespace UISkills { void GetSkillIconImages(SNO snoPower, const BOOL bInactive, ImageTextureFrame & tTexFrameNormal, ImageTextureFrame & tTexFrameMouseover, ImageTextureFrame & tTexFramePushed, ImageTextureFrame & tTexFrameRound); } namespace UIBuffs { int32 GetStackCountForSkillButton(SNO snoPower, const Player * ptPlayer); }
extern "C" int32 sub_71004b8900(const ActorCommonData * ptACD, int32 nLevel);














inline void HotbarButtonCreationParams::sInit()
{
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, "GlowSquare", tTexFrameFlash);
    bHideStackCount = FALSE;
}






HotbarButtonCreationParams::HotbarButtonCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : ButtonCreationParams(eType, id, idParent)
{
    sInit();
}


















void HotbarButtonCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{

    if (snoUI == SNO_NONE)
    {
        return;
    }

    pControlData->GetImageAttribute(ATT_IMAGEFRAME_FLASH, tTexFrameFlash);
    bHideStackCount = pControlData->GetBoolValue(ATT_HIDE_STACK_COUNT);

    ButtonCreationParams::Load(snoUI, pControlData);
}




void UIHotbarButton::sInit(const HotbarButtonCreationParams & tParams, const Vector2D &, const Vector2D & vecDim)
{



    const UIID & id = GetID();

    m_tData.Empty();
    m_bUnusable = FALSE;

    const float INSET = 8.0f;
    {
        TextCreationParams tTextParams(UIT_TEXT, "charges", id);
        tTextParams.eEatsClicks = EC_IGNORE;
        tTextParams.pfnTooltip = NULL;

        tTextParams.eJustifyHorizontal = 3; tTextParams.eJustifyVertical = 3;
        tTextParams.snoFont = GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF);
        tTextParams.nFontSize = 11;
        tTextParams.rgbaDropShadow = crgbaBlack;
        tTextParams.fUseDropShadow = TRUE;
        m_idChargeText = CreateControl<UIText>(tTextParams, Vector2D(INSET, INSET), Vector2D(vecDim.x - INSET * 2.0f, vecDim.y - INSET * 2.0f));
    }
    {

        TextCreationParams tTextParams(UIT_TEXT, "stackcount", id);
        tTextParams.eEatsClicks = EC_IGNORE;
        tTextParams.pfnTooltip = NULL;

        tTextParams.eJustifyHorizontal = 1; tTextParams.eJustifyVertical = 3;
        tTextParams.snoFont = GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF);
        tTextParams.nFontSize = 11;
        tTextParams.rgbaText = crgbaGreen;
        tTextParams.rgbaDropShadow = crgbaBlack;
        tTextParams.fUseDropShadow = TRUE;
        m_idBuffStackCountText = CreateControl<UIText>(tTextParams, Vector2D(INSET, INSET), Vector2D(vecDim.x - INSET * 2.0f, vecDim.y - INSET * 2.0f));
    }

    TimerCreationParams tTimerParams(UIT_TIMER, "timer", id);
    tTimerParams.eEatsClicks = EC_IGNORE;
    tTimerParams.tImageTexFlash = tParams.tTexFrameFlash;

    m_idTimer = CreateControl<UIConsoleSkillsTimer>(tTimerParams, Vector2D(0.0f, 0.0f), vecDim);


    const UIID & idRecharge = id;


    TimerCreationParams tRechargeTimerParams(UIT_TIMER, "rechargetimer", idRecharge);
    tRechargeTimerParams.eEatsClicks = EC_IGNORE;
    tRechargeTimerParams.tImageTexFlash = tParams.tTexFrameFlash;

    m_idRechargeTimer = CreateControl<UIRechargeTimer>(tRechargeTimerParams, Vector2D(0.0f, 0.0f), vecDim);

    m_ptPlayer = NULL;

    m_bHideStackCount = tParams.bHideStackCount;
}
















UIHotbarButton::UIHotbarButton(const HotbarButtonCreationParams & tParams, const Rect2D * pRect)
    : UIButton(tParams, pRect)
{


    Vector2D vecTL, vecDim;
    sRelativeCoordsFromCreationParams(vecTL, vecDim, pRect, tParams);

    sInit(tParams, vecTL, vecDim);
}





UIHotbarButton::~UIHotbarButton()
{
}




void UIHotbarButton::DisableTimer()
{

    UI_DeleteControl(m_idTimer, TRUE);
    UI_DeleteControl(m_idRechargeTimer, TRUE);
}














int32 UIHotbarButton::GetDurationRemaining() const
{


    if (UI_TestID(m_idTimer))
    {
        return UI_GetTimerFromID(m_idTimer).GetTimeRemaining();
    }


    return -1;
}
// NON_MATCHING: same size; this and ptTimer swap x19/x20, and the charges test at line 318 becomes a ccmp chain where the original keeps three branches.














void UIHotbarButton::UpdateTimer(
    const BOOL fSkillChanged,
    const BOOL bDisplayCharges)
{
    UITimer * ptTimer = NULL;
    if (UI_TestID(m_idTimer))
    {
        ptTimer = &UI_GetTimerFromID(m_idTimer);
        ptTimer->SetActive(TRUE);

        if (fSkillChanged)
        {

            ptTimer->SetTimer(0, 0);
            m_bOnCooldown = FALSE;
        }
    }
    UIText * ptText = NULL;
    if (UI_TestID(m_idChargeText))
    {
        ptText = &UI_GetTextFromID(m_idChargeText);
        ptText->SetActive(FALSE);
    }

    const Player * ptPlayer = m_ptPlayer ? m_ptPlayer : LocalPlayerGet();
    if (!ptPlayer) { return; }



    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer) { return; }




    if (m_tData.m_snoPower != SNO_NONE)
    {
        BOOL bShowCooldown = FALSE;
        if (ptTimer)
        {
            const uint32 dwCooldownEnd = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, m_tData.m_snoPower));
            if (dwCooldownEnd != (uint32)-1)
            {
                const uint32 dwCooldownStart = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, m_tData.m_snoPower));
                ptTimer->SetTimer(dwCooldownStart, dwCooldownEnd);
                bShowCooldown = TRUE;
                m_bOnCooldown = TRUE;
            }
            else if (!ptTimer->IsFinished())
            {


                ptTimer->SetTimer(1, 0);
            }

            ptText->SetActive(FALSE);
        }















        const int32 nMaxCharges = PowerActorGetMaxCharges(*ptACDPlayer, m_tData.m_snoPower);

        if (bDisplayCharges && !bShowCooldown && nMaxCharges > 0)
        {


            if (UI_TestID(m_idRechargeTimer))
            {
                UIRechargeTimer & tRechargeTimer = (UIRechargeTimer &)UI_GetTimerFromID(m_idRechargeTimer);
                tRechargeTimer.SetActive(TRUE);

                const uint32 dwRechargeStart = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_RECHARGE_START_TIME, m_tData.m_snoPower));
                const uint32 dwRechargeEnd = ptACDPlayer->AttributesGetInt(MakeAttribKey(ATTRIBUTE_NEXT_CHARGE_GAINED_TIME, m_tData.m_snoPower));

                tRechargeTimer.SetTimer(dwRechargeStart, dwRechargeEnd);
            }
        }


        if (!bDisplayCharges || nMaxCharges <= 0)
        {
            if (UI_TestID(m_idRechargeTimer))
            {
                UIRechargeTimer & tRechargeTimer = (UIRechargeTimer &)UI_GetTimerFromID(m_idRechargeTimer);
                tRechargeTimer.SetActive(FALSE);
            }
        }
    }
    else if (m_tData.m_annItem != ANN_INVALID)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(m_tData.m_annItem));

        if (ptTimer && ptACDItem)
        {

            ptTimer->SetActive(TRUE);
            UIInventory::UpdateTimerVisibility(*ptTimer, *ptACDPlayer, *ptACDItem);
        }
    }
    else if (m_tData.m_gbidItem != GBID_NONE)
    {
        ACDID idACDItem;
        const int32 nCount = ActorInventoryCountItemsOfTypeOnPlayer(ptACDPlayer->id, m_tData.m_gbidItem, &idACDItem);

        if (ptText && !m_bHideStackCount)
        {
            ptText->SetActive(TRUE);
            ptText->SetText(CRefString().Format("%d", nCount), FALSE);
            if (nCount == 0)
            {
                ptText->SetTextColor(RGBAColor(0x88, 0x88, 0x88));
            }
            else
            {
                ptText->SetTextColor(crgbaWhite);
            }
        }

        if (ptTimer && nCount != 0)
        {

            ActorCommonData & tACDItem = ACDGet(idACDItem);
            ptTimer->SetActive(TRUE);
            UIInventory::UpdateTimerVisibility(*ptTimer, *ptACDPlayer, tACDItem);
        }
    }



    else if (ptTimer)
    {
        ptTimer->SetActive(FALSE);

        if (UI_TestID(m_idRechargeTimer))
        {
            UIRechargeTimer & tRechargeTimer = (UIRechargeTimer &)UI_GetTimerFromID(m_idRechargeTimer);
            tRechargeTimer.SetActive(FALSE);
            tRechargeTimer.SetTimer(0, 0);
        }
    }
}





void UIHotbarButton::UpdateSkillButtonState(const HotbarButtonData & tButtonData, const BOOL bUnusable, const BOOL bNoMouseOver, const BOOL bRoundIcon, const BOOL bDisplayCharges, const BOOL bDisplayBuffStackCount)
{






    const BOOL fSkillChanged = tButtonData.m_snoPower != m_tData.m_snoPower;

    m_tData = tButtonData;
    m_bUnusable = bUnusable;

    UpdateTimer(fSkillChanged, bDisplayCharges);

    SetWindowColor(crgbaWhite);
    SetText("", FALSE);

    if (UI_TestID(m_idBuffStackCountText))
    {
        UIText & tText = UI_GetTextFromID(m_idBuffStackCountText);
        tText.SetText("", FALSE);
        tText.SetTextureFrame("");
    }

    if (m_tData.m_snoPower != SNO_NONE)
    {

        ImageTextureFrame tTexFrameNormal;
        ImageTextureFrame tTexFrameMouseover;
        ImageTextureFrame tTexFramePushed;
        ImageTextureFrame tTexFrameRound;
        UISkills::GetSkillIconImages(m_tData.m_snoPower, bUnusable, tTexFrameNormal, tTexFrameMouseover, tTexFramePushed, tTexFrameRound);

        if (bRoundIcon)
        {
            SetAllTextureFrames(tTexFrameRound);
            if (bUnusable)
            {
                SetShader(GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
            }
            else
            {
                SetShader(SNO_NONE);
            }
        }
        else if (bNoMouseOver)
        {
            SetAllTextureFrames(tTexFrameNormal);
        }
        else
        {
            SetTextureFrame(BS_NORMAL, tTexFrameNormal);
            SetTextureFrame(BS_MOUSEOVER, tTexFrameMouseover);
            SetTextureFrame(BS_PUSHED, tTexFramePushed);
        }

        if (UI_TestID(m_idBuffStackCountText))
        {

            UIText & tStackCount = UI_GetTextFromID(m_idBuffStackCountText);

            int32 nStackCount = 0;
            if (bDisplayBuffStackCount && m_ptPlayer)
            {
                nStackCount = UIBuffs::GetStackCountForSkillButton(m_tData.m_snoPower, m_ptPlayer);
            }




            if (nStackCount > 1)
            {
                tStackCount.SetText(UnicodeText::FormatNumber(nStackCount, TRUE, FNS_NORMAL, 0), FALSE);
                tStackCount.SetTextureFrame("UI:BuffBarDotFlipped");
            }
            else
            {
            tStackCount.SetText("", FALSE);
            tStackCount.SetTextureFrame("");
            }
        }
        return;
    }

    else if (m_tData.m_annItem != ANN_INVALID)
    {
        Player * ptPlayer = LocalPlayerGet();
        if (!ptPlayer) { return; }

        ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(m_tData.m_annItem));
        if (!ptACDItem) { return; }

        PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
        BOOL fIsMale = PlayerIsMale(*ptPlayer);

        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfo(*ptACDItem, tImageTexFrame, 0, ePlayerClass, fIsMale);
        SetAllTextureFrames(tImageTexFrame);
        return;
    }



    else if (m_tData.m_gbidItem != GBID_NONE)
    {
        Player * ptPlayer = LocalPlayerGet();
        if (ptPlayer)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (!ptACDPlayer) { return; }

            ImageTextureFrame tImageTexFrame;
            ACDInventoryGetItemDrawInfoWithGBID(m_tData.m_gbidItem, tImageTexFrame, 0); SetAllTextureFrames(tImageTexFrame); return;
        }
    }





    SetAllTextureFrames(cgtImageTextureFrameNull);
}





BOOL UIHotbarButton::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{

    if (msg == UI_UPDATE)
    {
        sUpdate();
    }






    return UIButton::HandleMessage(msg, dwParam);
}














void UIHotbarButton::SetPlayer(const Player * ptPlayer)
{
    m_ptPlayer = ptPlayer;
}














void UIHotbarButton::HideStackCount(const BOOL bHide)
{
    m_bHideStackCount = bHide;
}




void UIHotbarButton::DrawItem(
    const uint8,
    float & flZSlice)
{







    Rect2D rectWindowUIC;
    GetWindowRect(rectWindowUIC);


    if (GetWindowColor().a == 0) { return; }




    RGBAColor rgbaColor(crgbaWhite);
    rgbaColor.a = GetWindowColor().a;


    ImageTextureFrame & tImageTexFrame = m_tTextureFrame[BS_NORMAL];
    if (m_tTextureFrame[BS_NORMAL].snoTexture != SNO_NONE &&


        Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
    {




        Rect2D rectItemUIC;

        UIItemButton::GetItemRect(tImageTexFrame, rectWindowUIC, rectItemUIC);
        Rect2D rectItemPixels;
        ConvertUICToPixels(rectItemUIC, rectItemPixels);

        const SNO snoShader = GlobalSNOGet(m_bUnusable ? GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE : GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        {

            float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
            flZSlice += 1.0f;

            if (m_bMirrorX || m_bMirrorY)
            {

                SpriteDrawDeferredMirrored(tImageTexFrame, snoShader, rectItemPixels, rgbaColor, crgbaBlack, flLayerZ, NULL, m_bMirrorX, m_bMirrorY, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID);
            }
            else
            {









                SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, snoShader, rectItemPixels, rgbaColor, crgbaBlack, flLayerZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
            }
        }



        Player * ptPlayer = LocalPlayerGet();
        if (ptPlayer)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                UIItemOverlay eOverlay = (UIItemOverlay)1;
                int32 nRequiredLevel = 0;

                if (m_tData.m_annItem != ANN_INVALID)
                {
                    ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(m_tData.m_annItem));
                    if (ptACDItem)
                    {
                        nRequiredLevel = sub_71004b8900(ptACDPlayer, ItemGetLevelRequirement(ptACDItem->hGB.gbid, ptACDItem, FALSE));
                    }
                }
                else
                {
                    nRequiredLevel = sub_71004b8900(ptACDPlayer, ItemGetLevelRequirement(m_tData.m_gbidItem, NULL, FALSE));
                }

                if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) < nRequiredLevel)
                {





                    float flLayerZ = UIDC_DIALOG_Z_START - (UIDC_DIALOG_Z_INCREMENT * flZSlice);
                    flZSlice += 1.0f;

                    UIInventory::DrawItemOverlay(rectItemUIC, eOverlay, rgbaColor.a / 255.0f, flLayerZ, NULL);
                }
            }
        }
    }
}





void UIHotbarButton::Draw(const uint8 bLayerAlpha, float & flZSlice)
{

    if (m_tData.m_gbidItem != GBID_NONE || m_tData.m_annItem != ANN_INVALID)
    {

        DrawItem(bLayerAlpha, flZSlice);
        DrawChildren(bLayerAlpha, flZSlice);
    }
    else
    {
        UIButton::Draw(bLayerAlpha, flZSlice);
    }
}





inline void UIHotbarButton::sUpdate()
{


    if (UI_TestID(m_idTimer))
    {
        UITimer & tTimer = UI_GetTimerFromID(m_idTimer);

        const Rect2D rectWindow = GetWindowRect();
        tTimer.SetWindowRect(rectWindow);
    }
}
