// Decompilation of X/Client/UserInterface/Controls/UIConsoleScrollIndicatorControl.cpp
// Diablo III 2.6.2 (Switch), original DWARF/source order.

#include "UIConsoleScrollIndicatorControl.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../UITypes.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../../Sigma/Math/Vector.h"

// Declared here as the other consumers of UI.cpp do (UI.h's DWARF skeleton
// does not carry it): X/Client/UserInterface/UI.cpp defines it.
UIWindow & UI_GetWindowFromID(const UIID & id);

// 0x000000710029e100  size 56
ConsoleScrollIndicatorCreationParams::ConsoleScrollIndicatorCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)
    : WindowCreationParams(eType, szName, idParent)
{
    sInit();
}

// 0x000000710029e140  size 52
void ConsoleScrollIndicatorCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    WindowCreationParams::Load(snoUI, pControlData);
    eScrollbarVisibility = (ScrollbarVisibility)pControlData->GetInt32Value(ATT_SCROLLBAR_VISIBILITY);
}

// 0x000000710029e100 inlined sInit, original line 56
inline void ConsoleScrollIndicatorCreationParams::sInit()
{
    nMax = 100;
    eScrollbarVisibility = SBV_HIDE_WHEN_NOT_NEEDED;
}

// 0x000000710029e180  size 60
UIConsoleScrollIndicator::UIConsoleScrollIndicator(const ConsoleScrollIndicatorCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)
    : UIWindow(tParams, vecTL, vecDim)
{
    sInit(tParams, vecTL, vecDim);
}

// 0x000000710029e290 / 0x000000710029e2a0  size 4 / 36
UIConsoleScrollIndicator::~UIConsoleScrollIndicator()
{
}

// 0x000000710029e1c0  size 204
void UIConsoleScrollIndicator::sInit(const ConsoleScrollIndicatorCreationParams & tParams, const Vector2D &, const Vector2D &)
{
    m_nMax = tParams.nMax;
    m_nPosition = 0;
    m_eScrollbarVisibility = tParams.eScrollbarVisibility;

    WindowCreationParams tWindowParams(UIT_WINDOW, "thumb", GetID());
    UIWindow & tThumb = *new UIWindow(tWindowParams, Vector2D(0.0f, 0.0f), Vector2D(0.0f, 0.0f));
    tThumb.SetTextureFrame("UI:Console_ScrollbarThumb");
    m_idThumb = tThumb.GetID();
}

// 0x000000710029e2d0  size 28
void UIConsoleScrollIndicator::SetExtentsAndPosition(int32 nMax, int32 nPosition)
{
    m_nMax = nMax;
    m_nPosition = Math::Pin(nPosition, 0, nMax);
    PositionAndSizeThumb();
}

// 0x000000710029e3e0  size 36
void UIConsoleScrollIndicator::SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim)
{
    UIWindow::SetRelativePosition(vecTL, vecDim);
    PositionAndSizeThumb();
}

// 0x000000710029e2f0  size 240
// m_tImageTexBkgrnd.snoTexture (UIWindow+0xa0) and m_vecDim (UIWindow+0xe8) are
// read directly, matching the original bytes (no accessor call).
void UIConsoleScrollIndicator::PositionAndSizeThumb()
{
    int32 nRange = m_nMax;
    UIWindow & tThumb = UI_GetWindowFromID(m_idThumb);
    Vector2D vecThumbSize;

    if (tThumb.m_tImageTexBkgrnd.snoTexture != SNO_NONE) {
        tThumb.GetTextureFrameSize(vecThumbSize);

        float flMaxThumbPos = m_vecDim.y - vecThumbSize.y;
        float flThumbPos = (nRange > 0) ? flMaxThumbPos * m_nPosition / nRange : 0.0f;

        flThumbPos = Math::Min(flMaxThumbPos, flThumbPos);
        Vector2D vecTL(0.0f, flThumbPos);

        tThumb.SetRelativePosition(vecTL, vecThumbSize);
    }

    if (m_eScrollbarVisibility == SBV_ALWAYS_SHOW) {
        tThumb.SetActive(TRUE);
    } else if (m_eScrollbarVisibility == SBV_HIDE_WHEN_NOT_NEEDED) {
        tThumb.SetActive(nRange > 0);
    } else {
        tThumb.SetActive(FALSE);
    }
}

// 0x000000710029e410  size 68
void UIConsoleScrollIndicator::GetThumbSize(Vector2D & vecDim) const
{
    UIWindow & tThumb = UI_GetWindowFromID(m_idThumb);

    if (tThumb.m_tImageTexBkgrnd.snoTexture != SNO_NONE) {
        tThumb.GetTextureFrameSize(vecDim);
    } else {
        vecDim = Vector2D(0.0f, 0.0f);
    }
}
