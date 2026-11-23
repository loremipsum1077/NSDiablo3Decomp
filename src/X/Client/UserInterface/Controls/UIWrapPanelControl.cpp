#include "UIWrapPanelControl.h"

#include "../../../Sigma/Math/MathGeneral.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../UI.h"
#include "../UIAttributes.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);
// X/Client/UserInterface/UIUtils.cpp
void UIParseCsvElement(float & dest, LPCSTR & szCsv);

struct CountStrips {  // size 0x8
    CountStrips(int32 & nStripCount) : m_nStripCount(nStripCount) {}
    BOOL operator()(const int32, UIWindow &, const BOOL bIsOverflowing);
private:
    int32 & m_nStripCount;  // 0x00
};

struct CountChildrenInStrip {  // size 0x10
    CountChildrenInStrip(int32 & nInitialCount, const int32 nInitialStrip, const int32 nTargetStrip);
    BOOL operator()(const int32, UIWindow &, BOOL bIsOverflowing);
private:
    int32 & m_nChildCount;  // 0x00
    int32 m_nCurrStrip;  // 0x08
    int32 m_nTargetStrip;  // 0x0c
};

template <typename T>
void sForEachChild(const UIWrapPanel & tWrapPanel, const int32 nStartIndex, const int32 nEndIndex, T operation)
{
    Vector2D vecTL;
    Vector2D vecDim;
    tWrapPanel.GetRelativePosition(vecTL, vecDim);
    const float flMySize = tWrapPanel.m_fStackVertically ? vecDim.y : vecDim.x;
    float flChildOffset = tWrapPanel.GetFrontMargin();
    const float flSpacing = tWrapPanel.m_fStackVertically ? tWrapPanel.GetVerticalSpacing() : tWrapPanel.GetHorizontalSpacing();

    for (int32 i = nStartIndex; i <= nEndIndex; ++i)
    {
        const UIID & idChild = tWrapPanel.GetChildList()[i].id;
        UIControl & tControl = UI_GetControlFromID(idChild);
        if (tControl.IsType(UIT_WINDOW) && tControl.IsItselfActive())
        {
            UIWindow & tChild = UI_GetWindowFromID(idChild);
            Vector2D vecChildTL;
            Vector2D vecChildDim;
            tChild.GetRelativePosition(vecChildTL, vecChildDim);
            const float flChildSize = tWrapPanel.IsVertical() ? vecChildDim.y : vecChildDim.x;
            if (flChildOffset + flChildSize > flMySize - (tWrapPanel.IsVertical() ? tWrapPanel.m_vecMarginBottomRight.y : tWrapPanel.m_vecMarginBottomRight.x))
            {
                if (!operation(i, tChild, TRUE))
                    return;
                flChildOffset = tWrapPanel.GetFrontMargin() + flChildSize + flSpacing;
            }
            else
            {
                if (!operation(i, tChild, FALSE))
                    return;
                flChildOffset += flChildSize + flSpacing;
            }
        }
    }
}

inline void WrapPanelCreationParams::sInit()
{
    fStackVertically = FALSE;
    fReverseOrder = FALSE;
    flHSpacing = 0.0f;
    flVSpacing = 0.0f;
    flMarginTop = 0.0f;
    flMarginRight = 0.0f;
    flMarginBottom = 0.0f;
    flMarginLeft = 0.0f;
}

void WrapPanelCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    WindowCreationParams::Load(snoUI, pControlData);

    fStackVertically = pControlData->GetBoolValue(ATT_VERTICAL);
    fReverseOrder = pControlData->GetBoolValue(ATT_REVERSE_ORDER);
    flHSpacing = (float)pControlData->GetInt32Value(ATT_HSPACING);
    flVSpacing = (float)pControlData->GetInt32Value(ATT_VSPACING);

    LPCSTR szMargins = pControlData->GetAttributeValue(ATT_MARGINS_TRBL);
    UIParseCsvElement(flMarginTop, szMargins);
    UIParseCsvElement(flMarginRight, szMargins);
    UIParseCsvElement(flMarginBottom, szMargins);
    UIParseCsvElement(flMarginLeft, szMargins);
}

WrapPanelCreationParams::WrapPanelCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}

inline void UIWrapPanel::sInit(const WrapPanelCreationParams & tParams)
{
    m_fStackVertically = tParams.fStackVertically;
    m_fReverseOrder = tParams.fReverseOrder;
    m_flHSpacing = tParams.flHSpacing;
    m_flVSpacing = tParams.flVSpacing;
    m_vecMarginTopLeft.x = tParams.flMarginLeft;
    m_vecMarginTopLeft.y = tParams.flMarginTop;
    m_vecMarginBottomRight.x = tParams.flMarginRight;
    m_vecMarginBottomRight.y = tParams.flMarginBottom;
    m_flContentStripSize = 0.0f;
}

UIWrapPanel::UIWrapPanel(const WrapPanelCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{
    sInit(tParams);
}

UIWrapPanel::~UIWrapPanel()
{
}

inline void UIWrapPanel::FixupContentStripSize(UIWindow * ptWindow)
{
    Vector2D vecChildOffset;
    Vector2D vecChildDim;
    ptWindow->GetRelativePosition(vecChildOffset, vecChildDim);

    if (m_fStackVertically)
    {
        m_flContentStripSize = Math::Max(m_flContentStripSize, vecChildDim.x);
        if (Math::Abs(vecChildOffset.y - m_vecMarginTopLeft.y) < EPSILON)
            m_flContentStripSize = vecChildDim.x;
    }
    else
    {
        m_flContentStripSize = Math::Max(m_flContentStripSize, vecChildDim.y);
        if (Math::Abs(vecChildOffset.x - m_vecMarginTopLeft.x) < EPSILON)
            m_flContentStripSize = vecChildDim.y;
    }
}

inline void UIWrapPanel::RelayoutChild(UIWindow * ptWindow, Vector2D & vecOffset)
{
    Vector2D vecChildOffset;
    Vector2D vecChildDim;
    ptWindow->GetRelativePosition(vecChildOffset, vecChildDim);

    Vector2D vecNewOffset = vecOffset;
    if (m_fStackVertically)
    {
        if (vecChildDim.y + vecOffset.y > m_vecDim.y - m_vecMarginBottomRight.y)
        {
            vecNewOffset.x = vecOffset.x + (m_flContentStripSize + m_flHSpacing);
            vecNewOffset.y = m_vecMarginTopLeft.y;
            m_flContentStripSize = vecChildDim.x;
            vecOffset = vecNewOffset;
        }
        else
        {
            m_flContentStripSize = Math::Max(m_flContentStripSize, vecChildDim.x);
        }
    }
    else
    {
        if (vecChildDim.x + vecOffset.x > m_vecDim.x - m_vecMarginBottomRight.x)
        {
            vecNewOffset.x = m_vecMarginTopLeft.x;
            vecNewOffset.y = vecOffset.y + (m_flContentStripSize + m_flVSpacing);
            m_flContentStripSize = vecChildDim.y;
            vecOffset = vecNewOffset;
        }
        else
        {
            m_flContentStripSize = Math::Max(m_flContentStripSize, vecChildDim.y);
        }
    }

    ptWindow->Move(vecNewOffset - vecChildOffset);

    if (m_fStackVertically)
        vecOffset.y += vecChildDim.y + m_flVSpacing;
    else
        vecOffset.x += vecChildDim.x + m_flHSpacing;
}

inline int32 UIWrapPanel::FindPrevWindowChild(int32 nStartingIndex)
{
    int32 nStart = nStartingIndex - 1;
    int32 nEnd = -1;
    int8 nIncrement = -1;
    if (m_fReverseOrder)
    {
        nEnd = GetChildList().size();
        nIncrement = 1;
        nStart = nStartingIndex + 1;
    }

    for (int32 i = nStart; i != nEnd; i += nIncrement)
    {
        UIControl & tControl = UI_GetControlFromID(GetChildList()[i].id);
        if (tControl.IsType(UIT_WINDOW) && tControl.IsItselfActive())
            return i;
    }
    return -1;
}

inline void UIWrapPanel::GetStartingLayout(int32 nStartingChildIndex, Vector2D & vecOffset)
{
    const int32 nNumChildren = GetChildList().size();
    if (nStartingChildIndex == nNumChildren)
    {
        if (nStartingChildIndex < 0 || m_fReverseOrder)
            return;
    }
    else if (nStartingChildIndex < 0)
    {
        return;
    }

    const int32 nPrev = FindPrevWindowChild(nStartingChildIndex);
    if (nPrev < 0)
        return;

    Vector2D vecPrevTL;
    Vector2D vecPrevDim;
    UI_GetWindowFromID(GetChildList()[nPrev].id).GetRelativePosition(vecPrevTL, vecPrevDim);
    if (m_fStackVertically)
    {
        vecOffset.x = vecPrevTL.x;
        vecOffset.y = vecPrevTL.y + vecPrevDim.y + m_flVSpacing;
    }
    else
    {
        vecOffset.x = vecPrevTL.x + vecPrevDim.x + m_flHSpacing;
        vecOffset.y = vecPrevTL.y;
    }
}

void UIWrapPanel::RecalculateLayout(int32 nStartingChildIndex)
{
    if (!UI_TestID(GetID()) || !UI_TestID(GetParent()))
        return;
    if (nStartingChildIndex < -1 || nStartingChildIndex > (int32)GetChildList().size())
        return;

    if (m_fReverseOrder)
        RecalculateReverseLayout(nStartingChildIndex);
    else
        RecalculateForwardLayout(nStartingChildIndex);
}

void UIWrapPanel::DeleteAllChildren()
{
    const UIControlItemArray & arChildren = GetChildList();
    while (arChildren.size() != 0)
    {
        const UIID & idChild = arChildren[arChildren.size() - 1].id;
        if (UI_TestID(idChild))
            UI_DeleteControl(idChild, TRUE);
    }
}

void UIWrapPanel::RecalculateForwardLayout(int32 nStartingChildIndex)
{
    const int32 nNumChildren = GetChildList().size();
    Vector2D vecOffset = m_vecMarginTopLeft;
    GetStartingLayout(nStartingChildIndex, vecOffset);

    m_flContentStripSize = 0.0f;
    Vector2D vecEncapsulatingDim(0.0f, 0.0f);
    const int32 nIndex = nStartingChildIndex == -1 ? 0 : nStartingChildIndex;
    for (int32 i = 0; i < nNumChildren; ++i)
    {
        UIControl & tControl = UI_GetControlFromID(GetChildList()[i].id);
        if (tControl.IsType(UIT_WINDOW) && tControl.IsItselfActive())
        {
            UIWindow * ptWindow = static_cast<UIWindow *>(&tControl);
            Vector2D vecChildOffset;
            Vector2D vecChildDim;
            if (i >= nIndex)
                RelayoutChild(ptWindow, vecOffset);
            else
                FixupContentStripSize(ptWindow);

            ptWindow->GetRelativePosition(vecChildOffset, vecChildDim);
            vecEncapsulatingDim.x = Math::Max(vecEncapsulatingDim.x, vecChildOffset.x + vecChildDim.x);
            vecEncapsulatingDim.y = Math::Max(vecEncapsulatingDim.y, vecChildOffset.y + vecChildDim.y);
        }
    }

    vecEncapsulatingDim.x += m_vecMarginBottomRight.x;
    vecEncapsulatingDim.y += m_vecMarginBottomRight.y;

    if (m_fStackVertically)
        vecEncapsulatingDim.y = m_vecDim.y;
    else
        vecEncapsulatingDim.x = m_vecDim.x;

    SetRelativePosition(m_vecTL, vecEncapsulatingDim);
}

void UIWrapPanel::RecalculateReverseLayout(int32 nStartingChildIndex)
{
    int32 nIndex = nStartingChildIndex;
    const int32 nNumChildren = GetChildList().size();
    Vector2D vecOffset = m_vecMarginTopLeft;
    GetStartingLayout(nStartingChildIndex, vecOffset);

    if (nStartingChildIndex == -1 || nStartingChildIndex >= nNumChildren)
        nIndex = nNumChildren - 1;

    m_flContentStripSize = 0.0f;
    Vector2D vecEncapsulatingDim(0.0f, 0.0f);
    for (int32 i = nNumChildren - 1; i >= 0; --i)
    {
        UIControl & tControl = UI_GetControlFromID(GetChildList()[i].id);
        if (tControl.IsType(UIT_WINDOW) && tControl.IsItselfActive())
        {
            UIWindow * ptWindow = static_cast<UIWindow *>(&tControl);
            Vector2D vecChildOffset;
            Vector2D vecChildDim;
            if (i <= nIndex)
                RelayoutChild(ptWindow, vecOffset);
            else
                FixupContentStripSize(ptWindow);

            ptWindow->GetRelativePosition(vecChildOffset, vecChildDim);
            vecEncapsulatingDim.x = Math::Max(vecEncapsulatingDim.x, vecChildOffset.x + vecChildDim.x);
            vecEncapsulatingDim.y = Math::Max(vecEncapsulatingDim.y, vecChildOffset.y + vecChildDim.y);
        }
    }

    vecEncapsulatingDim.x += m_vecMarginBottomRight.x;
    vecEncapsulatingDim.y += m_vecMarginBottomRight.y;

    if (m_fStackVertically)
        vecEncapsulatingDim.y = m_vecDim.y;
    else
        vecEncapsulatingDim.x = m_vecDim.x;

    SetRelativePosition(m_vecTL, vecEncapsulatingDim);
}

inline float UIWrapPanel::GetHorizontalSpacing() const
{
    return m_flHSpacing;
}

inline float UIWrapPanel::GetVerticalSpacing() const
{
    return m_flVSpacing;
}

inline float UIWrapPanel::GetFrontMargin() const
{
    if (m_fStackVertically)
        return m_vecMarginTopLeft.y;
    return m_vecMarginTopLeft.x;
}

inline BOOL UIWrapPanel::IsVertical() const
{
    return m_fStackVertically;
}

inline BOOL CountStrips::operator()(const int32, UIWindow &, const BOOL bIsOverflowing)
{
    if (bIsOverflowing)
        ++m_nStripCount;
    return TRUE;
}

int32 UIWrapPanel::GetNumStrips() const
{
    const int32 nNumChildren = GetChildList().size();
    if (nNumChildren == 0)
        return 0;

    int32 nStripCount = 1;
    sForEachChild(*this, 0, nNumChildren - 1, CountStrips(nStripCount));
    return nStripCount;
}

int32 UIWrapPanel::GetStripContainingChild(const int32 nChildIndex) const
{
    if (nChildIndex == -1)
        return -1;

    int32 nStripCount = 0;
    sForEachChild(*this, 0, nChildIndex, CountStrips(nStripCount));
    return nStripCount;
}

inline CountChildrenInStrip::CountChildrenInStrip(int32 & nInitialCount, const int32 nInitialStrip, const int32 nTargetStrip)
    : m_nChildCount(nInitialCount), m_nCurrStrip(nInitialStrip), m_nTargetStrip(nTargetStrip)
{
}

inline BOOL CountChildrenInStrip::operator()(const int32, UIWindow &, BOOL bIsOverflowing)
{
    if (bIsOverflowing)
    {
        if (m_nCurrStrip == m_nTargetStrip)
            return FALSE;
        ++m_nCurrStrip;
    }
    if (m_nCurrStrip == m_nTargetStrip)
        ++m_nChildCount;
    return TRUE;
}

int32 UIWrapPanel::GetNumChildrenInStrip(const int32 nStrip) const
{
    const int32 nNumChildren = GetChildList().size();
    int32 nChildCount = 0;
    sForEachChild(*this, 0, nNumChildren - 1, CountChildrenInStrip(nChildCount, 0, nStrip));
    return nChildCount;
}

BOOL UIWrapPanel::GetGridPosOfChild(const int32 nChildIndex, IVector2D & vecGridPos) const
{
    const int32 nNumChildren = GetChildList().size();
    if (nChildIndex < 0 || nChildIndex >= nNumChildren)
        return FALSE;

    const int32 nStrip = GetStripContainingChild(nChildIndex);
    if (m_fStackVertically)
        vecGridPos.x = nStrip;
    else
        vecGridPos.y = nStrip;

    int32 nNumChildrenInPrevStrips = 0;
    for (int32 i = 0; i < nStrip; ++i)
        sForEachChild(*this, nNumChildrenInPrevStrips, nChildIndex, CountChildrenInStrip(nNumChildrenInPrevStrips, i, i));

    if (m_fStackVertically)
        vecGridPos.y = nChildIndex - nNumChildrenInPrevStrips;
    else
        vecGridPos.x = nChildIndex - nNumChildrenInPrevStrips;
    return TRUE;
}

int32 UIWrapPanel::GetChildIndexAtGridPos(const IVector2D & vecGridPos) const
{
    if (vecGridPos.x < 0 || vecGridPos.y < 0)
        return -1;

    const int32 nStrip = m_fStackVertically ? vecGridPos.x : vecGridPos.y;
    const int32 nNumChildren = GetChildList().size();
    int32 nChildCount = 0;
    for (int32 i = 0; i < nStrip; ++i)
        sForEachChild(*this, nChildCount, nNumChildren - 1, CountChildrenInStrip(nChildCount, i, i));

    const int32 nResult = nChildCount + (m_fStackVertically ? vecGridPos.y : vecGridPos.x);
    if (nResult >= nNumChildren)
        return -1;
    if (!UI_GetControlFromID(GetChildList()[nResult].id).IsItselfActive())
        return -1;
    return nResult;
}
