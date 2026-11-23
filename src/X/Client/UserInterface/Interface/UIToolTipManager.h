// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIToolTipManager.h
// Trimmed to the members UIToolTipManager.cpp defines or calls.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Controls/UITextControl.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

struct ItemTooltipData;
struct UIID;
class UIToolTip;
class UIWindow;
struct Vector2D;

class UIToolTipManager {  // size 0x38
public:
    UIToolTipManager();
    UIToolTip & GetHyperlinkTip();
    void HideTips();
    void SetAllowTooltipDelays(const BOOL fVal) { m_bAllowTooltipDelays = fVal; }  // line 198
    UIToolTip * SetTextToolTip(const UIID & idParent, LPCSTR uszTip, const UIJustifyHorizontal eJustifyHorizontal, const BOOL fForceRefresh, const BOOL fPrimaryTip);
    UIToolTip * SetItemToolTip(const UIID & idParent, const ItemTooltipData & tData, const BOOL bPrimaryTip, const BOOL bResetScrollPos, const Vector2D * pvecDesiredUIC, const BOOL fUseDefaultPositioning);
    UIToolTip * SetHyperlinkTooltip(const UITextHyperlinkRecord & tRecord);
    void AlignItemTooltip(const Rect2D & rectParent, UIToolTip & tToolTip);
    BOOL ControlIsShowingTooltip(const UIID & idParent);
    BOOL IsShowingItemTooltipWithAltInstructions();
private:
    UIToolTip & sGetUnusedTip(BOOL bPrimary);
    BOOL sToolTipCanBeShown();
    void sPositionItemTooltips();
    void sAlignTipRectToRect(UIToolTip & tToolTip, const Rect2D & rectParent, const Vector2D & vecOffset, const AlignPosition eTipHorizontal, const AlignPosition eTipVertical) const;
    UIWindow & sGetTooltipParent(const UIToolTip & tToolTip) const;
    void sPositionNewTooltip(UIToolTip & tToolTip) const;
    static const uint32 MAX_TIPS = 3;
    static const uint32 PRIMARY_TIP = 0;
    BOOL m_bInitialized;  // 0x00
    int32 m_nTipsInUse;  // 0x04
    UIToolTip * m_ptToolTips[3];  // 0x08
    BOOL m_bAllowTooltipDelays;  // 0x20
    UIToolTip * m_ptHyperlinkTip;  // 0x28
    UIToolTip * m_ptListTip;  // 0x30
};
static_assert(sizeof(UIToolTipManager) == 0x38, "UIToolTipManager");

extern UIToolTipManager gtTooltipMgr;  // UIToolTipManager.cpp:24
