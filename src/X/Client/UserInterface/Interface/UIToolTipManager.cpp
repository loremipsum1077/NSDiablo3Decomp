#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
// blz/tuple.h for blz::ignore, which sits at the front of the merged statics block.
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of gtTooltipMgr.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIToolTipManager.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIToolTipControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../CGame.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
BOOL GameIsStartUp();  // X/Common/Game.cpp
ACDID LocalPlayerACDGet();  // X/Client/CPlayer.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
void UI_ConstrainRectToViewport(Rect2D & rect, float flIndent);  // X/Client/UserInterface/UIUtils.cpp
void CHyperlinkPopulateItemTooltip(LPCSTR szHyperlink, ActorCommonData * ptACDPlayer, UIToolTip & tToolTip);  // X/Common/Hyperlink.cpp

UIToolTipManager gtTooltipMgr;

UIToolTipManager::UIToolTipManager()
{
    m_bInitialized = FALSE;
    m_nTipsInUse = 0;
    m_bAllowTooltipDelays = TRUE;
    SigmaMemoryClear(m_ptToolTips, sizeof(m_ptToolTips));
    m_ptHyperlinkTip = NULL;
    m_ptListTip = NULL;
}

UIToolTip & UIToolTipManager::sGetUnusedTip(BOOL bPrimary)
{
    if (bPrimary)
    {
        for (int32 i = 1; i < m_nTipsInUse; i++)
        {
            m_ptToolTips[i]->SetActive(FALSE);
        }

        m_nTipsInUse = 1;
        return *m_ptToolTips[PRIMARY_TIP];
    }

    UIToolTip * ptTooltip = m_ptToolTips[m_nTipsInUse];
    if (m_bAllowTooltipDelays)
    {
        ptTooltip->StartFadeInTimer();
    }
    m_nTipsInUse++;
    return *ptTooltip;
}

UIToolTip & UIToolTipManager::GetHyperlinkTip()
{
    return *m_ptHyperlinkTip;
}

void UIToolTipManager::HideTips()
{
    if (m_bInitialized)
    {
        for (int32 i = 0; i < m_nTipsInUse; i++)
        {
            m_ptToolTips[i]->SetActive(FALSE);
        }

        m_nTipsInUse = 0;
        m_bAllowTooltipDelays = TRUE;
    }
}

BOOL UIToolTipManager::sToolTipCanBeShown()
{
    GameCommonData & tGCD = CGameGetGameCommonData();
    return m_bInitialized && (tGCD.GameModeGet() == GAMEMODE_PLAYING || tGCD.GameModeGet() == GAMEMODE_MENU);
}

static Rect2D sToolTipMoveWithinBounds(Rect2D & rectTip)
{
    UI_ConstrainRectToViewport(rectTip, 10.0f);

    return rectTip;
}

static void sMovePtWithinBounds(Vector2D & vecPos, const Vector2D & vecDim)
{
    Rect2D rectNew;
    rectNew.Set(vecPos.x, vecPos.y, vecPos.x + vecDim.x, vecPos.y + vecDim.y);
    rectNew = sToolTipMoveWithinBounds(rectNew);
    vecPos.Set(rectNew.left, Math::Max(rectNew.top, 0.0f));
}

void UIToolTipManager::sPositionItemTooltips()
{
    if (m_nTipsInUse == 0)
    {
        return;
    }

    BOOL fGoRight = FALSE;
    UIToolTip & tPrimaryToolTip = *m_ptToolTips[PRIMARY_TIP];
    const Rect2D & rectParent = tPrimaryToolTip.m_rectParent;
    if (rectParent.left < 480.0f)
    {
        fGoRight = TRUE;
    }

    // Constant FALSE in the DWARF for the whole function; nothing reads it here.
    BOOL fIsBackpackTooltip = FALSE;

    {
        Rect2D rectTip;
        tPrimaryToolTip.GetWindowRect(rectTip);
        Vector2D vecTipDim(rectTip.Width(), rectTip.Height());

        Vector2D vecPos;
        if (fGoRight)
        {
            vecPos.x = rectParent.right;
        }
        else
        {
            vecPos.x = rectParent.left - vecTipDim.x;
        }

        vecPos.y = rectParent.top - vecTipDim.y;

        sMovePtWithinBounds(vecPos, vecTipDim);

        tPrimaryToolTip.MoveTo(vecPos);
    }

    if (m_nTipsInUse < 2)
    {
        return;
    }

    Rect2D rectPrimaryTip;
    tPrimaryToolTip.GetWindowRect(rectPrimaryTip);

    float flDesiredTop = rectPrimaryTip.top - 60.0f;
    const float OVERLAP_AMOUNT = 15.0f;

    for (int32 i = 1; i < m_nTipsInUse; i++)
    {
        UIToolTip & tToolTipPrev = *m_ptToolTips[i - 1];
        UIToolTip & tToolTip = *m_ptToolTips[i];

        Rect2D rectPrevTip;
        tToolTipPrev.GetWindowRect(rectPrevTip);

        Rect2D rectTip;
        tToolTip.GetWindowRect(rectTip);
        Vector2D vecTipDim(rectTip.Width(), rectTip.Height());

        Vector2D vecPos;
        if (fGoRight)
        {
            vecPos.x = rectPrevTip.right - OVERLAP_AMOUNT;
        }
        else
        {
            vecPos.x = rectPrevTip.left - vecTipDim.x + OVERLAP_AMOUNT;
        }
        vecPos.y = flDesiredTop;

        sMovePtWithinBounds(vecPos, vecTipDim);

        tToolTip.MoveTo(vecPos);
    }
}

void UIToolTipManager::sAlignTipRectToRect(UIToolTip & tToolTip, const Rect2D & rectParent, const Vector2D & vecOffset, const AlignPosition eTipHorizontal, const AlignPosition eTipVertical) const
{
    if (eTipHorizontal != AP_NONE || eTipVertical != AP_NONE)
    {
        Rect2D rectTip;
        tToolTip.GetWindowRect(rectTip);

        rectTip.AlignToRect(rectParent, eTipHorizontal, eTipVertical, 10.0f);
        rectTip.left += vecOffset.x;
        rectTip.right += vecOffset.x;
        rectTip.top += vecOffset.y;
        rectTip.bottom += vecOffset.y;

        rectTip = sToolTipMoveWithinBounds(rectTip);

        Vector2D vecTL(rectTip.left, rectTip.top);
        vecTL.SnapToIntegerCoords();
        tToolTip.MoveTo(vecTL);
    }
    else
    {
        Vector2D vecTL(rectParent.left, rectParent.top);
        vecTL += vecOffset;
        tToolTip.MoveTo(vecTL);
    }
}

UIWindow & UIToolTipManager::sGetTooltipParent(const UIToolTip & tToolTip) const
{
    UIID idParent(tToolTip.m_idActiveOwner);
    if (idParent == GetUIIDNone())
    {
        idParent = cgUIIDTopLayer;
    }

    return UI_GetWindowFromID(idParent);
}

void UIToolTipManager::sPositionNewTooltip(UIToolTip & tToolTip) const
{
    tToolTip.SetActive(TRUE);

    Rect2D rectParent;
    UIWindow & tParent = sGetTooltipParent(tToolTip);
    tParent.GetWindowRect(rectParent);

    tToolTip.SetParentRect(rectParent);

    AlignPosition eTipHorizontal, eTipVertical;
    tParent.GetTooltipAlignment(eTipHorizontal, eTipVertical);
    Vector2D vecOffset = tParent.GetTooltipOffset();
    sAlignTipRectToRect(tToolTip, rectParent, vecOffset, eTipHorizontal, eTipVertical);
}

UIToolTip * UIToolTipManager::SetTextToolTip(const UIID & idParent, LPCSTR uszTip, const UIJustifyHorizontal eJustifyHorizontal, const BOOL fForceRefresh, const BOOL fPrimaryTip)
{
    if (!sToolTipCanBeShown())
    {
        return NULL;
    }

    UIToolTip & tToolTip = sGetUnusedTip(fPrimaryTip);
    tToolTip.SetTextTooltipData(idParent, uszTip, eJustifyHorizontal, fForceRefresh);

    sPositionNewTooltip(tToolTip);

    return &tToolTip;
}

UIToolTip * UIToolTipManager::SetItemToolTip(const UIID & idParent, const ItemTooltipData & tData, const BOOL bPrimaryTip, const BOOL bResetScrollPos, const Vector2D * pvecDesiredUIC, const BOOL fUseDefaultPositioning)
{
    if (!sToolTipCanBeShown())
    {
        return NULL;
    }

    UIToolTip & tToolTip = sGetUnusedTip(bPrimaryTip);
    tToolTip.SetItemTooltipData(idParent, tData, bResetScrollPos);

    if (pvecDesiredUIC != NULL)
    {
        tToolTip.SetActive(TRUE);

        Rect2D rectParent;
        rectParent.Set(pvecDesiredUIC->x, pvecDesiredUIC->y, pvecDesiredUIC->x, pvecDesiredUIC->y);
        tToolTip.SetParentRect(rectParent);
        sAlignTipRectToRect(tToolTip, rectParent, Vector2D(0.0f, 0.0f), AP_HORIZONTAL_LEFT, AP_VERTICAL_ABOVE);
    }
    else if (!fUseDefaultPositioning)
    {
        tToolTip.SetActive(TRUE);

        UIWindow & tParent = sGetTooltipParent(tToolTip);
        Rect2D rectParent;
        tParent.GetWindowRect(rectParent);

        AlignItemTooltip(rectParent, tToolTip);
    }
    else
    {
        sPositionNewTooltip(tToolTip);
    }

    return &tToolTip;
}

UIToolTip * UIToolTipManager::SetHyperlinkTooltip(const UITextHyperlinkRecord & tRecord)
{
    UIToolTip & tTip = GetHyperlinkTip();
    tTip.SetActive(TRUE);

    ActorCommonData * ptACDPlayer = NULL;
    if (!GameIsStartUp())
    {
        if (LocalPlayerACDGet() == ACDID_INVALID)
        {
            return NULL;
        }

        ptACDPlayer = &ACDGet(LocalPlayerACDGet());
    }

    CHyperlinkPopulateItemTooltip(tRecord.sRawHyperlinkText, ptACDPlayer, tTip);

    return &tTip;
}

void UIToolTipManager::AlignItemTooltip(const Rect2D & rectParent, UIToolTip & tToolTip)
{
    tToolTip.SetParentRect(rectParent);
    sPositionItemTooltips();
}

BOOL UIToolTipManager::ControlIsShowingTooltip(const UIID & idParent)
{
    if (!m_bInitialized)
    {
        return FALSE;
    }

    for (int32 i = 0; i < m_nTipsInUse; i++)
    {
        UIToolTip & tToolTip = *m_ptToolTips[i];
        if (idParent == tToolTip.m_idActiveOwner)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL UIToolTipManager::IsShowingItemTooltipWithAltInstructions()
{
    for (int32 i = 0; i < m_nTipsInUse; i++)
    {
        UIToolTip & tToolTip = *m_ptToolTips[i];
        if (tToolTip.IsActive())
        {
            if (tToolTip.m_dwTooltipFlags & (1 << TOOLTIP_SHOW_ALT_INSTRUCTIONS_BIT))
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}
