// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleInventoryScreen.h
// Original screen layout and virtual interface.

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleInputUtilities.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryView.h"
#include "Wrappers/RingWrapper.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { class ControllerActionManager; }
struct Player;
struct UIID;

typedef int32 InputButton;  // X/Client/Input/Input.h

typedef XGrowableMap<UIID, const UIID &, int, int> UIIDToFilterMap;

namespace UIInventory {
namespace Console {
typedef int32 MaintainSelectionOptions;

class UIInventoryScreen {  // size 0x200
public:
    enum BrowsingMode {
        BROWSE_LIST = 0,
        BROWSE_DETAILS = 1,
    };

    UIInventoryScreen();
    virtual ~UIInventoryScreen() {}  // vtable slot 0; line 46
    void OpenForController(int32 nControllerOpening);
    void Close();
    BOOL IsActive() const { return m_bIsActive; }  // line 57
    void SetActive(const BOOL bActive);
    void ResetRing();
    void EnableRing(const BOOL bEnable);
    Player * GetPlayer() const;
    void SetPlayer(Player * ptPlayer);
    int32 GetControllerIndex() const { return m_nControllerIndex; }  // line 85
    ::Console::RingWrapper & GetRingUI();
    UIInventoryView * GetPrimaryView();
    void SetPrimaryView(UIInventoryView * ptView);
    UIInventoryView * GetSecondaryView();
    void SetSecondaryView(UIInventoryView * ptView);
    UIInventoryView * GetFocusedView() const;
    void SetFocusedView(UIInventoryView * ptView);
    UIFilterType GetFilter() const;
    BrowsingMode GetBrowsingMode();
    void SetBrowsingMode(const BrowsingMode eBrowsingMode);
    virtual void OpenSelectedSlot();  // vtable slot 2
    virtual void CloseSelectedSlot();  // vtable slot 3
    virtual BOOL RingHasFocus() const;  // vtable slot 4
    const UIIDToFilterMap & GetFilterMap() { return m_tFilterMap; }
    void RequestUpdate(const MaintainSelectionOptions eSelectionType);
    virtual void Update();  // vtable slot 5
    void UpdateRingIcons();
    void UpdateMainhandAndOffhandRingIcons(const Player * ptPlayer);
    BOOL CanScrollDetails();
    void ScrollDetails(const float flDelta);
    void ResetDetailsScroll();
    void UpdateCurrencyDisplay(const UIID & idAmountText, const UIID & idSpentText, const UIID & idSpentStoryboard, const UIID & idReturnedText, const UIID & idReturnedStoryboard, const int32 eDisplayedCurrency);
    void ResetCurrencyAnimations(const UIID & idSpentStoryboard, const UIID & idReturnedStoryboard);
    void OnRingSelectionChanged(const UIID & idIcon, const BOOL bSelected);
    BOOL HandleLeftStickViewSwitching(const int32 nControllerIndex);
    virtual void OnSelectedItemChanged() { ResetDetailsScroll(); }  // vtable slot 6
    virtual void HandleLeftStickInput(const int32 nControllerIndex);  // vtable slot 7
    virtual void HandleRightStickInput(const int32 nControllerIndex);  // vtable slot 8
    virtual void HandleButtonInput(const int32, const InputButton);  // vtable slot 9
protected:
    BOOL m_fCurrentlyOpening;  // 0x08
    int32 m_nControllerIndex;  // 0x0c
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void OnRequestedUpdate() {}  // vtable slot 12
    virtual void OnUpdate() {}  // vtable slot 13
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager) {}  // vtable slot 14
    virtual BOOL CanOpenForController(int32 nControllerOpening) const;  // vtable slot 15
    virtual void OnSetActive(BOOL) = 0;  // vtable slot 16
    virtual void InitializeRingWrapper(::Console::RingWrapper &) = 0;  // vtable slot 17
    virtual UIFilterType GetDefaultRingSelection() const = 0;  // vtable slot 18
    virtual void UpdateRingIconState(const UIID & idIcon, const BOOL bSelected);  // vtable slot 19
    virtual BOOL ShouldEnableRingIcon(const UIFilterType) { return TRUE; }  // vtable slot 20
    virtual void InitRingFilters(UIIDToFilterMap &) = 0;  // vtable slot 21
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType) {}  // vtable slot 22
    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode) = 0;  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode) = 0;  // vtable slot 24
    UIID GetIconUIIDForFilterType(const UIFilterType eFilterType);
    BOOL CanSwitchFocusToView(const UIInventoryView * ptTargetView);
    BOOL SwitchFocusToView(UIInventoryView * ptView);
    void ShowNavBar(const UIID & idParent, const LPCSTR szNavBarName);
    UIFilterType GetInitialRingSelection(const UIFilterType eFilterMin, const UIFilterType eFilterMax);
    void SetFilterType(const UIFilterType eFilterType);
    virtual void DoSetFilterType(const UIFilterType eFilterType);  // vtable slot 25
private:
    UIFilterType GetFilterFromUIID(const UIID & id);
    void SetView(UIInventoryView *& ptTargetView, UIInventoryView * ptNewView);
    Player * m_ptPlayer;  // 0x10
    BOOL m_bIsActive;  // 0x18
    BOOL m_bUpdateRequested;  // 0x1c
    uint64 m_uLastKnownAmount[NUM_CURRENCY_TYPES];  // 0x20; 22 entries in 2.7.7 (constructor clears 0xb0 bytes)
    ::Console::RingWrapper m_tRingUI;  // 0xd0
    UIInventoryView * m_ptPrimaryView;  // 0x138
    UIInventoryView * m_ptSecondaryView;  // 0x140
    UIInventoryView * m_ptFocusedView;  // 0x148
    UIFilterType m_eFilterType;  // 0x150
    BrowsingMode m_eBrowsingMode;  // 0x154
protected:
    // The reference header lists this as private. DetailsInvScreen::UpdateWarningIcons reads it
    // directly (no GetFilterMap() call or inline record in its DWARF), so it is protected here
    // until the original access path is known.
    UIIDToFilterMap m_tFilterMap;  // 0x158
private:
    ::Console::AnalogStickInputFilter m_tRingStickInputFilter;  // 0x1d8
};
static_assert(sizeof(UIInventoryScreen) == 0x200, "UIInventoryScreen");

// UIInventoryView inlines from UIConsoleInventoryView.h. They need UIInventoryScreen complete,
// which that header only forward-declares, so their bodies are here.
inline BOOL UIInventoryView::IsFocused() const  // UIConsoleInventoryView.h:268
{
    return GetParentScreen() != NULL && GetParentScreen()->GetFocusedView() == this;
}

inline void UIInventoryView::SendIdentifyingRequest()  // UIConsoleInventoryView.h:487
{
    if (SendIdentifyRequest(GetParentScreen()->GetPlayer(), GetIdentifyingItemACD()))
    {
        m_bIdentifyingRequestSent = TRUE;
    }
}

}  // namespace Console
}  // namespace UIInventory
