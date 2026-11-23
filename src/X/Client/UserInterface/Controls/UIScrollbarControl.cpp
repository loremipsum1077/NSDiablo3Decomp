#include "UIScrollbarControl.h"

#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "UIButtonControl.h"
#include "UIThumbButtonControl.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);
UIThumbButton & UI_GetThumbButtonFromID(const UIID & id);
UIScrollbar & UI_GetScrollbarFromID(const UIID & id);
// X/Client/UserInterface/UIUtils.cpp
enum AlignmentType {
    ALIGN_CENTER_VERTICAL = 0,
    ALIGN_CENTER_HORIZONTAL = 1,
    ALIGN_CENTER_HORIZONTAL_VERTICAL = 2,
};
void AlignRect(const Vector2D & vecSourceTL, const Vector2D & vecSourceDim, const AlignmentType eAlignmentType, const float flStart, const Vector2D & vecToAlignDim, Vector2D & vecTL);
// X/Client/UserInterface/Interface/UIFunctions.cpp
UIScrollbarChangeFunctionPtr UIScrollbarChangeFunction_Lookup(LPCSTR szName);
void UIFunction_GetMouse(Vector2D & vecMouse);
// X/Client/ImageCollection.cpp
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);
// X/Client/Gfx/Texture.cpp
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex);
// X/Client/Input/InputNX64.cpp:50
extern const int32 INPUT_WHEEL_DELTA;
// X/Client/Cinematography.cpp
void CinematographyFixedViewSetWheelPos(int32 nWheelPos);

// Invented name: the "no override" value of eStyleOverride (the binary compares against -1).
#define SVS_NONE ((ScrollbarVisualStyle)-1)

void ScrollbarCreationParams::sInit()
{
    pfnOnChange = NULL;
    eScrollbarStyle = SVS_DEFAULT;
    fIsVertical = FALSE;
    eScrollbarVisibility = SBV_HIDE_WHEN_NOT_NEEDED;
    snoMouseoverSound = SNO_NONE;
}

ScrollbarCreationParams::ScrollbarCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}

ScrollbarCreationParams::ScrollbarCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)
    : WindowCreationParams(eType, szName, idParent)
{
    sInit();
}

void ScrollbarCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    WindowCreationParams::Load(snoUI, pControlData);

    pfnOnChange = UIScrollbarChangeFunction_Lookup(pControlData->GetAttributeValue(ATT_ONCHANGE));

    int32 style = pControlData->GetInt32Value(ATT_STYLE);
    if (style < SVS_DEFAULT || style >= SVS_COUNT)
        style = SVS_DEFAULT;
    eScrollbarStyle = (ScrollbarVisualStyle)style;

    fIsVertical = pControlData->GetBoolValue(ATT_VERTICAL);
    eScrollbarVisibility = (ScrollbarVisibility)pControlData->GetInt32Value(ATT_SCROLLBAR_VISIBILITY);
    snoMouseoverSound = SNOFileLookupGUID(SOUND_SNO, pControlData->GetAttributeValue(ATT_MOUSEOVER_SOUND));
}

static void sSendClickToScrollbarParent(const UIID & id)
{
    UIControl & tControl = UI_GetControlFromID(id);
    UIScrollbar & tScrollbar = UI_GetScrollbarFromID(tControl.GetParent());
    tScrollbar.HandleChildClick(id);
}

BOOL UIScrollbar::sIsSlider() const
{
    return m_eVisualStyle == SVS_POINTER_SLIDER || m_eVisualStyle == SVS_TICK_SLIDER || m_eVisualStyle == SVS_HUE_SLIDER;
}

void UIScrollbar::sInit(const ScrollbarCreationParams & tParams, const Vector2D &, const Vector2D & vecDim, const ScrollbarVisualStyle eStyleOverride)
{
    const UIID & id = GetID();

    m_pfnOnChange = tParams.pfnOnChange;

    if (eStyleOverride != SVS_NONE)
    {
        m_eVisualStyle = eStyleOverride;
        m_fIsVertical = !sIsSlider();
    }
    else
    {
        m_eVisualStyle = tParams.eScrollbarStyle;
        m_fIsVertical = tParams.fIsVertical;
    }

    m_eScrollbarVisibility = tParams.eScrollbarVisibility;
    m_nMin = 0;
    m_nMax = 0;
    m_nPosition = 0;
    m_nThumbDistance = 1;
    m_nPageDistance = 1;
    m_nMouseWheelScaleFactor = 4.0f;
    m_idOwner = GetParent();
    m_snoMouseoverSound = tParams.snoMouseoverSound;

    {
        ButtonCreationParams tButtonParams(UIT_BUTTON, "trackUp", id);
        tButtonParams.nRepeatRate = 1;
        if (m_eVisualStyle == SVS_HUE_SLIDER)
            tButtonParams.fDelayRepeat = FALSE;
        tButtonParams.pfnOnClick = sSendClickToScrollbarParent;
        tButtonParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tButtonParams.snoMouseoverSound = m_snoMouseoverSound;
        UIButton & tButton = *new UIButton(tButtonParams, Vector2D(0.0f, 0.0f), vecDim);
        m_idTrackUp = tButton.GetID();
    }

    {
        ButtonCreationParams tButtonParams(UIT_BUTTON, "trackDown", id);
        tButtonParams.nRepeatRate = 1;
        if (m_eVisualStyle == SVS_HUE_SLIDER)
            tButtonParams.fDelayRepeat = FALSE;
        tButtonParams.pfnOnClick = sSendClickToScrollbarParent;
        tButtonParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tButtonParams.snoMouseoverSound = m_snoMouseoverSound;
        UIButton & tButton = *new UIButton(tButtonParams, Vector2D(0.0f, 0.0f), vecDim);
        m_idTrackDown = tButton.GetID();
    }

    m_idButtonUp = GetUIIDNone();
    if (!sIsSlider())
    {
        float flButtonWidth;
        if (sIsVertical())
            flButtonWidth = vecDim.x;
        else
            flButtonWidth = vecDim.y;
        ButtonCreationParams tButtonParams(UIT_BUTTON, "up", id);
        tButtonParams.nRepeatRate = 1;
        tButtonParams.pfnOnClick = sSendClickToScrollbarParent;
        tButtonParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tButtonParams.snoClickSound = GlobalSNOGet(GLOBALSNO_SOUND_SCROLLBAR_BUTTON);
        tButtonParams.snoMouseoverSound = m_snoMouseoverSound;
        UIButton & tButton = *new UIButton(tButtonParams, Vector2D(0.0f, 0.0f), Vector2D(flButtonWidth, flButtonWidth));
        m_idButtonUp = tButton.GetID();
    }

    m_idButtonDown = GetUIIDNone();
    if (!sIsSlider())
    {
        float flButtonWidth;
        if (sIsVertical())
            flButtonWidth = vecDim.x;
        else
            flButtonWidth = vecDim.y;
        ButtonCreationParams tButtonParams(UIT_BUTTON, "down", id);
        tButtonParams.nRepeatRate = 1;
        tButtonParams.pfnOnClick = sSendClickToScrollbarParent;
        tButtonParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tButtonParams.snoClickSound = GlobalSNOGet(GLOBALSNO_SOUND_SCROLLBAR_BUTTON);
        tButtonParams.snoMouseoverSound = m_snoMouseoverSound;
        UIButton & tButton = *new UIButton(tButtonParams, Vector2D(0.0f, 0.0f), Vector2D(flButtonWidth, flButtonWidth));
        m_idButtonDown = tButton.GetID();
    }

    {
        float flButtonWidth;
        if (sIsVertical())
            flButtonWidth = vecDim.x;
        else
            flButtonWidth = vecDim.y;
        ButtonCreationParams tBtnParams(UIT_THUMBBUTTON, "thumb", id);
        tBtnParams.eRenderLayerOverride = tParams.eRenderLayerOverride;
        tBtnParams.snoMouseoverSound = m_snoMouseoverSound;
        m_idThumb = UIThumbButton::Create(tBtnParams, Vector2D(0.0f, 0.0f), Vector2D(flButtonWidth, flButtonWidth), m_fIsVertical);
    }

    sRepositionButtons();
    sRepositionTrack();
    float flControlSpaceDeviation = sGetThumbDeviation();
    sRepositionThumb(flControlSpaceDeviation);
    sClipTrack(flControlSpaceDeviation);

    SetVisualStyle(m_eVisualStyle);
}

UIScrollbar::UIScrollbar(const ScrollbarCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim, const ScrollbarVisualStyle eStyleOverride)
    : UIWindow(tParams, vecTL, vecDim)
{
    sInit(tParams, vecTL, vecDim, eStyleOverride);
}

UIScrollbar::UIScrollbar(const ScrollbarCreationParams & tParams, const Rect2D * pRect, const ScrollbarVisualStyle eStyleOverride)
    : UIWindow(tParams, pRect)
{
    Vector2D vecTL;
    Vector2D vecDim;
    sRelativeCoordsFromCreationParams(vecTL, vecDim, pRect, tParams);
    sInit(tParams, vecTL, vecDim, eStyleOverride);
}

UIScrollbar::~UIScrollbar()
{
}

void UIScrollbar::SetVisualStyle(ScrollbarVisualStyle eStyle)
{
    const LPCSTR pszAnimationNormal[5][4] = {
        { "UI:ScrollSmallThumb_Normal", "UI:ScrollSmallThumbPointer_Normal", "UI:ScrollbarThumbNormal", "UI:ColorPicker_HueSelection" },
        { "UI:ScrollSmallUp_Normal", "", "", "" },
        { "UI:ScrollSmallDown_Normal", "", "", "" },
        { "UI:ScrollSmallMiddle", "UI:SliderBarHorizontalPointerNormal", "UI:SliderBarHorizontalNormal", "UI:ColorPicker_Slider" },
        { "UI:ScrollSmallMiddle", "UI:SliderBarHorizontalPointerNormal", "UI:SliderBarHorizontalNormal", "UI:ColorPicker_Slider" },
    };
    const LPCSTR pszAnimationMouseOver[5][4] = {
        { "UI:ScrollSmallThumb_Mouseover", "UI:ScrollSmallThumbPointer_Mouseover", "UI:ScrollbarThumbMouseover", "UI:ColorPicker_HueSelection" },
        { "UI:ScrollSmallUp_Mouseover", "", "", "" },
        { "UI:ScrollSmallDown_Mouseover", "", "", "" },
        { "UI:ScrollSmallMiddle", "UI:SliderBarHorizontalPointerMouseover", "UI:SliderBarHorizontalMouseover", "UI:ColorPicker_Slider" },
        { "UI:ScrollSmallMiddle", "UI:SliderBarHorizontalPointerMouseover", "UI:SliderBarHorizontalMouseover", "UI:ColorPicker_Slider" },
    };
    const LPCSTR pszAnimationPushed[5][4] = {
        { "UI:ScrollSmallThumb_Pushed", "UI:ScrollSmallThumbPointer_Pushed", "UI:ScrollbarThumbPushed", "UI:ColorPicker_HueSelection" },
        { "UI:ScrollSmallUp_Pushed", "", "", "" },
        { "UI:ScrollSmallDown_Pushed", "", "", "" },
        { "UI:ScrollSmallMiddle", "UI:SliderBarHorizontalPointerMouseover", "UI:SliderBarHorizontalMouseover", "UI:ColorPicker_Slider" },
        { "UI:ScrollSmallMiddle", "UI:SliderBarHorizontalPointerMouseover", "UI:SliderBarHorizontalMouseover", "UI:ColorPicker_Slider" },
    };

    if (eStyle < SVS_DEFAULT || eStyle >= SVS_COUNT)
        eStyle = SVS_DEFAULT;

    if (!(m_idThumb == GetUIIDNone()))
    {
        UIThumbButton & tButton = UI_GetThumbButtonFromID(m_idThumb);
        tButton.SetTextureFrame(BS_NORMAL, pszAnimationNormal[0][eStyle]);
        tButton.SetTextureFrame(BS_MOUSEOVER, pszAnimationMouseOver[0][eStyle]);
        tButton.SetTextureFrame(BS_PUSHED, pszAnimationPushed[0][eStyle]);

        ImageTextureFrame tTexFrame;
        ImageCollectionGetImageInfo(pszAnimationNormal[0][eStyle], tTexFrame);
        IRect2D rectThumbTex;
        TextureGetFrameRectPixels(rectThumbTex, tTexFrame.snoTexture, tTexFrame.nFrame);
        tButton.SetRelativePosition(Vector2D(0.0f, 0.0f), Vector2D((float)rectThumbTex.Width(), (float)rectThumbTex.Height()));
    }

    if (!(m_idButtonUp == GetUIIDNone()))
    {
        UIButton & tButton = UI_GetButtonFromID(m_idButtonUp);
        tButton.SetTextureFrame(BS_NORMAL, pszAnimationNormal[1][eStyle]);
        tButton.SetTextureFrame(BS_MOUSEOVER, pszAnimationMouseOver[1][eStyle]);
        tButton.SetTextureFrame(BS_PUSHED, pszAnimationPushed[1][eStyle]);
    }

    if (!(m_idButtonDown == GetUIIDNone()))
    {
        UIButton & tButton = UI_GetButtonFromID(m_idButtonDown);
        tButton.SetTextureFrame(BS_NORMAL, pszAnimationNormal[2][eStyle]);
        tButton.SetTextureFrame(BS_MOUSEOVER, pszAnimationMouseOver[2][eStyle]);
        tButton.SetTextureFrame(BS_PUSHED, pszAnimationPushed[2][eStyle]);
    }

    if (!(m_idTrackUp == GetUIIDNone()))
    {
        UIButton & tButton = UI_GetButtonFromID(m_idTrackUp);
        tButton.SetTextureFrame(BS_NORMAL, pszAnimationNormal[3][eStyle]);
        tButton.SetTextureFrame(BS_MOUSEOVER, pszAnimationMouseOver[3][eStyle]);
        tButton.SetTextureFrame(BS_PUSHED, pszAnimationPushed[3][eStyle]);
    }

    if (!(m_idTrackDown == GetUIIDNone()))
    {
        UIButton & tButton = UI_GetButtonFromID(m_idTrackDown);
        tButton.SetTextureFrame(BS_NORMAL, pszAnimationNormal[4][eStyle]);
        tButton.SetTextureFrame(BS_MOUSEOVER, pszAnimationMouseOver[4][eStyle]);
        tButton.SetTextureFrame(BS_PUSHED, pszAnimationPushed[4][eStyle]);
    }
}

BOOL UIScrollbar::IsEnabled() const
{
    if (UI_TestID(m_idThumb))
        return UI_GetThumbButtonFromID(m_idThumb).IsEnabled();
    return TRUE;
}

void UIScrollbar::SetEnabled(BOOL fEnabled)
{
    UIThumbButton & tButtonThumb = UI_GetThumbButtonFromID(m_idThumb);

    if (!fEnabled)
    {
        tButtonThumb.SetActive(FALSE);
        tButtonThumb.SetEnabled(FALSE);
        UI_GetButtonFromID(m_idTrackUp).SetEnabled(FALSE);
        UI_GetButtonFromID(m_idTrackDown).SetEnabled(FALSE);
        if (UI_TestID(m_idButtonUp))
            UI_GetButtonFromID(m_idButtonUp).SetEnabled(FALSE);
        if (UI_TestID(m_idButtonDown))
            UI_GetButtonFromID(m_idButtonDown).SetEnabled(FALSE);
    }
    else if (!tButtonThumb.IsEnabled())
    {
        tButtonThumb.SetActive(TRUE);
        tButtonThumb.SetEnabled(TRUE);
        UI_GetButtonFromID(m_idTrackUp).SetEnabled(TRUE);
        UI_GetButtonFromID(m_idTrackDown).SetEnabled(TRUE);
        if (UI_TestID(m_idButtonUp))
            UI_GetButtonFromID(m_idButtonUp).SetEnabled(TRUE);
        if (UI_TestID(m_idButtonDown))
            UI_GetButtonFromID(m_idButtonDown).SetEnabled(TRUE);
    }
}

void UIScrollbar::SetPosition(int32 nPosition, const BOOL bSendUpdate)
{
    nPosition = Math::Pin(nPosition, m_nMin, m_nMax);

    int32 nOldPosition = m_nPosition;
    m_nPosition = nPosition;

    sReposition();

    if (bSendUpdate && m_pfnOnChange != NULL)
        m_pfnOnChange(GetID(), nOldPosition, m_nPosition);
}

void UIScrollbar::SetIncrements(const int32 nButtonIncrement, const int32 nPageIncrement)
{
    m_nThumbDistance = nButtonIncrement;
    m_nPageDistance = nPageIncrement;
}

void UIScrollbar::SetExtentsAndPosition(const int32 nMin, const int32 nMax, const int32 nPos, const BOOL bSendUpdate)
{
    int32 nOldPosition = m_nPosition;

    m_nMax = nMax;
    m_nMin = nMin;
    m_nPosition = Math::Pin(nPos, nMin, nMax);
    m_nThumbDistance = 1;
    m_nPageDistance = 1;

    BOOL fNotNeeded = nMax <= nMin;
    SetEnabled(!fNotNeeded);

    sReposition();

    if (bSendUpdate == TRUE && m_pfnOnChange != NULL)
        m_pfnOnChange(GetID(), nOldPosition, m_nPosition);

    if (m_eScrollbarVisibility == SBV_ALWAYS_SHOW)
        SetActive(TRUE);
    else if (m_eScrollbarVisibility == SBV_HIDE_WHEN_NOT_NEEDED)
        SetActive(!fNotNeeded);
    else
        SetActive(FALSE);
}

void UIScrollbar::sClickUp()
{
    sMove(-m_nThumbDistance);
}

void UIScrollbar::sClickDown()
{
    sMove(m_nThumbDistance);
}

void UIScrollbar::sPageUp()
{
    sMove(-m_nPageDistance);
}

void UIScrollbar::sPageDown()
{
    sMove(m_nPageDistance);
}

void UIScrollbar::HandleChildClick(const UIID & idChild)
{
    if (idChild == m_idButtonUp)
    {
        sClickUp();
        return;
    }

    if (idChild == m_idButtonDown)
    {
        sClickDown();
        return;
    }

    Vector2D ptMouse;
    UIFunction_GetMouse(ptMouse);

    Rect2D rectThumb;
    UI_GetThumbButtonFromID(m_idThumb).GetWindowRect(rectThumb);
    Rect2D rectWin;
    GetWindowRect(rectWin);

    if (idChild == m_idTrackUp)
    {
        float flCurrent;
        float flMouse;
        float flMin;
        float flMax;
        if (sIsVertical())
        {
            flCurrent = rectThumb.top;
            flMouse = ptMouse.y;
            flMin = rectWin.top;
            flMax = rectWin.bottom;
        }
        else
        {
            flCurrent = rectThumb.left;
            flMouse = ptMouse.x;
            flMin = rectWin.left;
            flMax = rectWin.right;
        }

        if (m_eVisualStyle == SVS_HUE_SLIDER)
        {
            float flRange = flMax - flMin;
            flRange = flRange != 0.0f ? flRange : 1.0f;
            int32 nRange = m_nMax - m_nMin;
            int32 nNewPosition = m_nMin + (int32)((nRange == 0 ? 1.0f : (float)nRange) * ((flMouse - flMin) / flRange));
            SetPosition(nNewPosition, TRUE);
        }
        else if (flCurrent - flMouse > EPSILON)
        {
            sPageUp();
        }
    }
    else if (idChild == m_idTrackDown)
    {
        float flCurrent;
        float flMouse;
        float flMin;
        float flMax;
        if (sIsVertical())
        {
            flCurrent = rectThumb.bottom;
            flMouse = ptMouse.y;
            flMin = rectWin.top;
            flMax = rectWin.bottom;
        }
        else
        {
            flCurrent = rectThumb.right;
            flMouse = ptMouse.x;
            flMin = rectWin.left;
            flMax = rectWin.right;
        }

        if (m_eVisualStyle == SVS_HUE_SLIDER)
        {
            float flRange = flMax - flMin;
            flRange = flRange != 0.0f ? flRange : 1.0f;
            int32 nRange = m_nMax - m_nMin;
            int32 nNewPosition = m_nMin + (int32)((nRange == 0 ? 1.0f : (float)nRange) * ((flMouse - flMin) / flRange));
            SetPosition(nNewPosition, TRUE);
        }
        else if (flMouse - flCurrent > EPSILON)
        {
            sPageDown();
        }
    }
}

void UIScrollbar::sMove(int32 nDistance)
{
    int32 nNewPosition = Math::Pin(m_nPosition + nDistance, m_nMin, m_nMax);

    SetPosition(nNewPosition, TRUE);
}

void UIScrollbar::OnDrag()
{
    Vector2D vecThumbTL;
    Vector2D vecThumbDim;
    UI_GetThumbButtonFromID(m_idThumb).GetRelativePosition(vecThumbTL, vecThumbDim);

    Vector2D vecTrackTL;
    Vector2D vecTrackDim;
    UI_GetButtonFromID(m_idTrackUp).GetRelativePosition(vecTrackTL, vecTrackDim);

    BOOL fIsVertical = sIsVertical();
    int32 nNewPosition;
    if (fIsVertical)
    {
        if (vecTrackTL.y - vecThumbTL.y > EPSILON)
            nNewPosition = m_nMin;
        else if ((vecThumbTL.y + vecThumbDim.y) - (vecTrackDim.y + vecTrackTL.y) > EPSILON)
            nNewPosition = m_nMax;
        else
            nNewPosition = Math::FloatRoundToInt(((vecThumbTL.y - vecTrackTL.y) / (vecTrackDim.y - vecThumbDim.y)) * (float)(m_nMax - m_nMin)) + m_nMin;
    }
    else
    {
        nNewPosition = Math::FloatRoundToInt(((vecThumbTL.x - vecTrackTL.x) / (vecTrackDim.x - vecThumbDim.x)) * (float)(m_nMax - m_nMin)) + m_nMin;
    }

    SetPosition(nNewPosition, TRUE);
}

void UIScrollbar::sRepositionButtons()
{
    Vector2D vecTL;
    Vector2D vecDim;
    GetRelativePosition(vecTL, vecDim);

    if (m_idButtonUp != GetUIIDNone())
    {
        UIButton & tButton = UI_GetButtonFromID(m_idButtonUp);
        Vector2D vecTLButton;
        Vector2D vecDimButton;
        tButton.GetRelativePosition(vecTLButton, vecDimButton);

        Vector2D vecNewPos;
        if (sIsVertical())
            AlignRect(Vector2D(0.0f, 0.0f), vecDim, ALIGN_CENTER_HORIZONTAL, 0.0f, vecDimButton, vecNewPos);
        else
            AlignRect(Vector2D(0.0f, 0.0f), vecDim, ALIGN_CENTER_VERTICAL, 0.0f, vecDimButton, vecNewPos);

        tButton.Move(vecNewPos - vecTLButton);
    }

    if (m_idButtonDown != GetUIIDNone())
    {
        UIButton & tButton = UI_GetButtonFromID(m_idButtonDown);
        Vector2D vecTLButton;
        Vector2D vecDimButton;
        tButton.GetRelativePosition(vecTLButton, vecDimButton);

        Vector2D vecNewPos;
        if (sIsVertical())
            AlignRect(Vector2D(0.0f, 0.0f), vecDim, ALIGN_CENTER_HORIZONTAL, vecDim.y - vecDimButton.y, vecDimButton, vecNewPos);
        else
            AlignRect(Vector2D(0.0f, 0.0f), vecDim, ALIGN_CENTER_VERTICAL, vecDim.x - vecDimButton.x, vecDimButton, vecNewPos);

        tButton.Move(vecNewPos - vecTLButton);
    }
}

void UIScrollbar::sRepositionTrack()
{
    Vector2D vecTL;
    Vector2D vecDim;
    GetRelativePosition(vecTL, vecDim);

    Vector2D vecBtnDownTL(0.0f, 0.0f);
    Vector2D vecBtnDownDim(0.0f, 0.0f);
    Vector2D vecBtnUpTL(0.0f, 0.0f);
    Vector2D vecBtnUpDim(0.0f, 0.0f);
    if (UI_TestID(m_idButtonUp) && UI_TestID(m_idButtonDown))
    {
        UI_GetButtonFromID(m_idButtonUp).GetRelativePosition(vecBtnUpTL, vecBtnUpDim);
        UI_GetButtonFromID(m_idButtonDown).GetRelativePosition(vecBtnDownTL, vecBtnDownDim);
    }

    UIButton & tButtonTrackUp = UI_GetButtonFromID(m_idTrackUp);
    Vector2D vecTopTrackTL;
    Vector2D vecTopTrackDim;
    tButtonTrackUp.GetRelativePosition(vecTopTrackTL, vecTopTrackDim);

    if (sIsVertical())
    {
        vecTL.Set(0.0f, vecBtnUpDim.y);
        vecDim.y = vecDim.y - (vecBtnDownDim.y + vecBtnUpDim.y);
        vecTopTrackTL.y = vecBtnUpDim.y;
        vecTopTrackDim.y = vecDim.y;
    }
    else
    {
        vecTL.Set(vecBtnDownDim.x, 0.0f);
        vecDim.x = vecDim.x - (vecBtnDownDim.x + vecBtnUpDim.x);
        vecTopTrackTL.x = vecBtnDownDim.x;
        vecTopTrackDim.x = vecDim.x;
    }

    tButtonTrackUp.SetRelativePosition(vecTopTrackTL, vecTopTrackDim);

    Vector2D vecNewPos;
    AlignRect(vecTL, vecDim, ALIGN_CENTER_HORIZONTAL_VERTICAL, 0.0f, vecTopTrackDim, vecNewPos);
    tButtonTrackUp.Move(vecNewPos - vecTopTrackTL);

    {
        UIButton & tButtonTrackDown = UI_GetButtonFromID(m_idTrackDown);
        if (!sIsSlider())
        {
            tButtonTrackUp.GetRelativePosition(vecTopTrackTL, vecTopTrackDim);
            tButtonTrackDown.SetRelativePosition(vecTopTrackTL, vecTopTrackDim);
        }
        else
        {
            Vector2D vecBottomTrackTL;
            Vector2D vecBottomTrackDim;
            tButtonTrackDown.GetRelativePosition(vecBottomTrackTL, vecBottomTrackDim);
            AlignRect(vecTL, vecDim, ALIGN_CENTER_HORIZONTAL_VERTICAL, 0.0f, vecBottomTrackDim, vecNewPos);
            tButtonTrackDown.Move(vecNewPos - vecBottomTrackTL);
        }
    }
}

void UIScrollbar::sRepositionThumb(float flDeviation)
{
    Vector2D vecTopTrackTL;
    Vector2D vecTopTrackDim;
    UI_GetButtonFromID(m_idTrackUp).GetRelativePosition(vecTopTrackTL, vecTopTrackDim);

    UIThumbButton & tButtonThumb = UI_GetThumbButtonFromID(m_idThumb);
    Vector2D vecThumbTL;
    Vector2D vecThumbDim;
    tButtonThumb.GetRelativePosition(vecThumbTL, vecThumbDim);

    Vector2D vecNewPos;
    if (sIsVertical())
        AlignRect(vecTopTrackTL, vecTopTrackDim, ALIGN_CENTER_HORIZONTAL, vecTopTrackTL.y + flDeviation, vecThumbDim, vecNewPos);
    else
        AlignRect(vecTopTrackTL, vecTopTrackDim, ALIGN_CENTER_VERTICAL, vecTopTrackTL.x + flDeviation, vecThumbDim, vecNewPos);

    tButtonThumb.Move(vecNewPos - vecThumbTL);
}

void UIScrollbar::sClipTrack(float flDeviation)
{
    if (m_eVisualStyle == SVS_HUE_SLIDER)
        return;

    Vector2D vecTL;
    Vector2D vecDim;
    GetRelativePosition(vecTL, vecDim);

    UIButton & tButtonUp = UI_GetButtonFromID(m_idTrackUp);
    Vector2D vecTopTrackTL;
    Vector2D vecTopTrackDim;
    tButtonUp.GetRelativePosition(vecTopTrackTL, vecTopTrackDim);
    (sIsVertical() ? vecTopTrackDim.y : vecTopTrackDim.x) = flDeviation;
    tButtonUp.SetRelativeClipRect(vecTopTrackTL, vecTopTrackDim);

    UIButton & tButtonDown = UI_GetButtonFromID(m_idTrackDown);
    Vector2D vecBottomTrackTL;
    Vector2D vecBottomTrackDim;
    tButtonDown.GetRelativePosition(vecBottomTrackTL, vecBottomTrackDim);
    if (sIsVertical())
    {
        vecBottomTrackTL.y = vecTopTrackTL.y + flDeviation;
        vecBottomTrackDim.y = vecDim.y - flDeviation;
    }
    else
    {
        vecBottomTrackTL.x = vecTopTrackTL.x + flDeviation;
        vecBottomTrackDim.x = vecDim.x - flDeviation;
    }
    tButtonDown.SetRelativeClipRect(vecBottomTrackTL, vecBottomTrackDim);
}

float UIScrollbar::sGetThumbDeviation()
{
    float flDeviation;
    if (m_nMax - m_nMin == 0)
    {
        flDeviation = 0.5f;
    }
    else
    {
        m_nPosition = Math::Pin(m_nPosition, m_nMin, m_nMax);
        flDeviation = (float)(m_nPosition - m_nMin) / (float)(m_nMax - m_nMin);
    }

    Vector2D vecTopTrackTL;
    Vector2D vecTopTrackDim;
    UI_GetButtonFromID(m_idTrackUp).GetRelativePosition(vecTopTrackTL, vecTopTrackDim);
    Vector2D vecThumbTL;
    Vector2D vecThumbDim;
    UI_GetThumbButtonFromID(m_idThumb).GetRelativePosition(vecThumbTL, vecThumbDim);

    float flDelta;
    if (sIsVertical())
        flDelta = vecTopTrackDim.y - vecThumbDim.y;
    else
        flDelta = vecTopTrackDim.x - vecThumbDim.x;
    flDelta = Math::Max(0.0f, flDelta);

    return flDeviation * flDelta;
}

void UIScrollbar::sReposition()
{
    float flControlSpaceDeviation = sGetThumbDeviation();
    sRepositionThumb(flControlSpaceDeviation);
    sClipTrack(flControlSpaceDeviation);

    UIWindow & tWnd = UI_GetWindowFromID(m_idOwner);
    tWnd.OnScrollbarPositionChanged(GetID());
}

void UIScrollbar::GetScrollbarRect(Vector2D & vecScrollbarTL, Vector2D & vecScrollbarDim, const ScrollbarVisualStyle, const Vector2D & vecParentDim)
{
    vecScrollbarDim.Set(32.0f, vecParentDim.y);
    vecScrollbarTL.Set(0.0f, 0.0f);
    vecScrollbarTL.x = vecParentDim.x - vecScrollbarDim.x;
}

BOOL UIScrollbar::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    if (msg == UI_INPUT)
    {
        const UIID & idParent = GetParent();
        UIWindow * ptParentWin = NULL;
        if (UI_TestID(idParent) && UI_GetControlFromID(idParent).IsType(UIT_WINDOW))
            ptParentWin = &UI_GetWindowFromID(idParent);

        const BufferedInput & tInput = *(const BufferedInput *)dwParam;
        if (sIsVertical() && tInput.eInputMsg == INPUTMSG_MOUSEWHEEL && IsEnabled() &&
            (ptParentWin == NULL || ptParentWin->IsInside(tInput.tBufMouseData.p)))
        {
            const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
            int32 nClicks = -(tInputState.tMouseState.nMouseWheelDeltaSinceLastUpdate / INPUT_WHEEL_DELTA);
            sMove((int32)(m_nMouseWheelScaleFactor * (float)nClicks * (float)m_nThumbDistance));
            CinematographyFixedViewSetWheelPos(tInputState.tMouseState.nMouseWheelPos);
            return TRUE;
        }
    }

    return UIWindow::HandleMessage(msg, dwParam);
}

void UIScrollbar::OnScrollChangeDefault(const UIID & id, const int32 nOldPos, const int32 nNewPos)
{
    UIScrollbar & tScrollbar = UI_GetScrollbarFromID(id);
    Vector2D vecScroll(0.0f, (float)(nOldPos - nNewPos));
    UI_GetWindowFromID(tScrollbar.GetOwner()).Scroll(vecScroll);
}

void UIScrollbar::SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim)
{
    UIWindow::SetRelativePosition(vecTL, vecDim);

    sRepositionButtons();
    sRepositionTrack();
    float flControlSpaceDeviation = sGetThumbDeviation();
    sRepositionThumb(flControlSpaceDeviation);
    sClipTrack(flControlSpaceDeviation);
}
