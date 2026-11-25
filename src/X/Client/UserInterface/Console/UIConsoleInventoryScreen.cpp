// Original file: X/Client/UserInterface/Console/UIConsoleInventoryScreen.cpp
#include "UIConsoleInventoryScreen.h"
#include "../Controls/UITextControl.h"
#include "../UIStoryboard.h"
#include "../../../Common/Items.h"
#include "../../../Sigma/UnicodeText.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleNewItemIndicators.h"
#include "../../Console/ControllerUtils.h"
#include "../Controls/UIButtonControl.h"
#include "../../Gfx/GfxVB.h"
#include "../../ImageCollection.h"
#include "../../../Common/GameSwig.h"

#include "../../../Common/GlobalSNO.h"
#include "UIConsoleUtils.h"
#include "../UI.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/Player.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

// GameCommonData.h; routing declarations avoid adopting unrelated class layouts.
BOOL GameIsOpenWorld();
// UI.h
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
// Items.cpp:10408 (the reference Items.h does not list this entry point).
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// GlobalSNO.h
GBID GlobalGBIDGet(const GlobalGBID eGlobalGBID);
// UIConsoleSeasonStash.cpp
namespace UISeasonStash { namespace Console {
void OnUpdate();
void OnSeasonStashButton(const UIID &, const int32, const InputButton);
} }

extern "C" {
static bool sub_71001f77a0(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon);
}

namespace UIInventory { namespace Console {
// Routing entry points owned by the indicated console implementation files.
// UIInventoryConsole.cpp
void OnInventoryUpdate();
void OnInventoryLeftStick(const UIID &, const int32);
void OnInventoryRightStick(const UIID &, const int32);
void OnInventoryButton(const UIID &, const int32, const InputButton);
// UIConsoleStash.cpp
void OnStashUpdate();
void OnStashLeftStick(const UIID &, const int32);
void OnStashRightStick(const UIID &, const int32);
void OnStashButton(const UIID &, const int32, const InputButton);
// UIShopConsole.cpp
void OnShopUpdate();
void OnShopLeftStick(const UIID &, const int32);
void OnShopRightStick(const UIID &, const int32);
void OnShopButton(const UIID &, const int32, const InputButton);
// UIConsoleJeweler.cpp
void OnJewelerUpdate();
void OnJewelerLeftStick(const UIID &, const int32);
void OnJewelerRightStick(const UIID &, const int32);
void OnJewelerButton(const UIID &, const int32, const InputButton);
// UIConsoleBlacksmith.cpp
void OnBlacksmithUpdate();
void OnBlacksmithLeftStick(const UIID &, const int32);
void OnBlacksmithRightStick(const UIID &, const int32);
void OnBlacksmithButton(const UIID &, const int32, const InputButton);
// UIConsoleMystic.cpp
void OnMysticUpdate();
void OnMysticLeftStick(const UIID &, const int32);
void OnMysticRightStick(const UIID &, const int32);
void OnMysticButton(const UIID &, const int32, const InputButton);
// UIConsoleMail.cpp
void OnMailUpdate();
void OnMailLeftStick(const UIID &, const int32);
void OnMailButton(const UIID &, const int32, const InputButton);
// UIConsoleBloodShardVendor.cpp
void OnBloodShardVendorUpdate();
void OnBloodShardVendorLeftStick(const UIID &, const int32);
void OnBloodShardVendorRightStick(const UIID &, const int32);
void OnBloodShardVendorButton(const UIID &, const int32, const InputButton);
// UIConsoleFollower.cpp
void OnFollowerButton(const UIID &, const int32, const InputButton);
// UIConsolePartyTab.cpp
void OnPartyButton(const UIID &, const int32, const InputButton);

void OnUpdate(const UIID &)
{
    OnInventoryUpdate();
    OnStashUpdate();
    OnShopUpdate();
    OnJewelerUpdate();
    OnBlacksmithUpdate();
    OnMysticUpdate();
    OnMailUpdate();
    UISeasonStash::Console::OnUpdate();
    if (GameIsOpenWorld()) OnBloodShardVendorUpdate();
}

void OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    OnInventoryLeftStick(id, nControllerIndex);
    OnStashLeftStick(id, nControllerIndex);
    OnShopLeftStick(id, nControllerIndex);
    OnJewelerLeftStick(id, nControllerIndex);
    OnBlacksmithLeftStick(id, nControllerIndex);
    OnMysticLeftStick(id, nControllerIndex);
    OnMailLeftStick(id, nControllerIndex);
    if (GameIsOpenWorld()) OnBloodShardVendorLeftStick(id, nControllerIndex);
}

void OnRightStick(const UIID & id, const int32 nControllerIndex)
{
    OnInventoryRightStick(id, nControllerIndex);
    OnStashRightStick(id, nControllerIndex);
    OnShopRightStick(id, nControllerIndex);
    OnJewelerRightStick(id, nControllerIndex);
    OnBlacksmithRightStick(id, nControllerIndex);
    OnMysticRightStick(id, nControllerIndex);
    if (GameIsOpenWorld()) OnBloodShardVendorRightStick(id, nControllerIndex);
}

static void sHandleButton(const UIID & id, const int32 nControllerIndex, const InputButton eButton)
{
    OnInventoryButton(id, nControllerIndex, eButton);
    OnStashButton(id, nControllerIndex, eButton);
    OnFollowerButton(id, nControllerIndex, eButton);
    OnPartyButton(id, nControllerIndex, eButton);
    OnShopButton(id, nControllerIndex, eButton);
    OnJewelerButton(id, nControllerIndex, eButton);
    OnBlacksmithButton(id, nControllerIndex, eButton);
    OnMysticButton(id, nControllerIndex, eButton);
    OnMailButton(id, nControllerIndex, eButton);
    UISeasonStash::Console::OnSeasonStashButton(id, nControllerIndex, eButton);
    if (GameIsOpenWorld()) OnBloodShardVendorButton(id, nControllerIndex, eButton);
}

void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_DPAD_LEFT);
}

void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_DPAD_RIGHT);
}

void OnButton1(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_1);
}

void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_2);
}

void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_3);
}

void OnButton4(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_4);
}

void OnButtonL2(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_LEFT_TRIGGER);
}

void OnButtonL3(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_LEFT_STICK_BUTTON);
}

void OnButtonR2(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_RIGHT_TRIGGER);
}

void OnButtonR3(const UIID & id, const int32 nControllerIndex)
{
    sHandleButton(id, nControllerIndex, BUTTON_RIGHT_STICK_BUTTON);
}


static void sIconSelectionChanged(const ::Console::RingWrapper &, const UIID &, const BOOL);

UIInventoryScreen::UIInventoryScreen()
    : m_fCurrentlyOpening(FALSE), m_nControllerIndex(-1), m_ptPlayer(NULL),
      m_bIsActive(FALSE), m_bUpdateRequested(FALSE), m_ptPrimaryView(NULL),
      m_ptSecondaryView(NULL), m_ptFocusedView(NULL), m_eFilterType(FILTER_NONE), m_eBrowsingMode(BROWSE_LIST), m_tFilterMap(16, NULL)
{
    m_tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);
    SigmaMemoryClear(m_uLastKnownAmount, sizeof(m_uLastKnownAmount));
}

void UIInventoryScreen::OpenForController(int32 nControllerOpening)
{
    if (CanOpenForController(nControllerOpening)) {
        m_fCurrentlyOpening = TRUE;
        m_nControllerIndex = nControllerOpening;
        m_ptPlayer = PlayerGetByControllerIndex(nControllerOpening);
        if (m_ptPlayer) {
            ActorCommonData * ptACDPlayer = ACDTryToGet(m_ptPlayer->idPlayerACD);
            if (ptACDPlayer) {
                for (int32 i = 0; i < NUM_CURRENCY_TYPES; ++i)
                    m_uLastKnownAmount[i] = ptACDPlayer->GetCurrencyAmount(i);
            }
        }
        ResetRing();
        OnOpen();
        SetActive(TRUE);
        m_fCurrentlyOpening = FALSE;
    }
}

void UIInventoryScreen::Close()
{
    if (m_ptPrimaryView) m_ptPrimaryView->Show(FALSE);
    if (m_ptSecondaryView) m_ptSecondaryView->Show(FALSE);
    m_eFilterType = FILTER_NONE;
    OnClose();
    SetActive(FALSE);
}

void UIInventoryScreen::SetActive(const BOOL bActive)
{
    m_bIsActive = bActive;
    OnSetActive(bActive);
}

void UIInventoryScreen::ResetRing()
{
    InitializeRingWrapper(m_tRingUI);
    POS pos = m_tFilterMap.GetStartAssoc();
    while (pos) {
        UIID idIcon;
        UIFilterType eDummyFilter;
        m_tFilterMap.GetNextAssoc(pos, idIcon, eDummyFilter);
        if (idIcon != GetUIIDNone()) UpdateRingIconState(idIcon, FALSE);
    }
    m_tFilterMap.RemoveAll();
    InitRingFilters(m_tFilterMap);
    const UIID idRingContainer = m_tRingUI.GetContainerControlID();
    if (idRingContainer != GetUIIDNone()) UI_GetControlFromID(idRingContainer).SetUserData(this);
}

void UIInventoryScreen::EnableRing(const BOOL bEnable)
{
    if (m_tRingUI.IsEnabled() != bEnable) {
        m_tRingUI.SetEnabled(bEnable);
        UpdateRingIcons();
    }
}

Player * UIInventoryScreen::GetPlayer() const { return m_ptPlayer; }

void UIInventoryScreen::SetPlayer(Player * ptPlayer) { m_ptPlayer = ptPlayer; }

::Console::RingWrapper & UIInventoryScreen::GetRingUI() { return m_tRingUI; }

UIInventoryView * UIInventoryScreen::GetPrimaryView() { return m_ptPrimaryView; }

void UIInventoryScreen::SetPrimaryView(UIInventoryView * ptView)
{
    if (m_ptPrimaryView != ptView) SetView(m_ptPrimaryView, ptView);
}

UIInventoryView * UIInventoryScreen::GetSecondaryView() { return m_ptSecondaryView; }

void UIInventoryScreen::SetSecondaryView(UIInventoryView * ptView)
{
    if (m_ptSecondaryView != ptView) SetView(m_ptSecondaryView, ptView);
}

UIInventoryView * UIInventoryScreen::GetFocusedView() const { return m_ptFocusedView; }

void UIInventoryScreen::SetFocusedView(UIInventoryView * ptView)
{
    if (m_ptFocusedView && m_ptFocusedView != ptView) m_ptFocusedView->OnLostFocus();
    m_ptFocusedView = ptView;
    if (m_ptFocusedView) m_ptFocusedView->OnGotFocus();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void UIInventoryScreen::SetView(UIInventoryView *& ptTargetView, UIInventoryView * ptNewView)
{
    if (ptTargetView) {
        ptTargetView->Show(FALSE);
        ptTargetView->SetParentScreen(NULL);
    }
    BOOL bViewHadFocus = GetFocusedView() && GetFocusedView() == ptTargetView;
    ptTargetView = ptNewView;
    if (ptTargetView) {
        ptTargetView->SetParentScreen(this);
        ptTargetView->Show(TRUE);
        if (bViewHadFocus) SetFocusedView(ptTargetView);
    }
}

UIFilterType UIInventoryScreen::GetFilter() const { return m_eFilterType; }

UIInventoryScreen::BrowsingMode UIInventoryScreen::GetBrowsingMode() { return m_eBrowsingMode; }

void UIInventoryScreen::SetBrowsingMode(const BrowsingMode eBrowsingMode)
{
    m_eBrowsingMode = eBrowsingMode;
    UIInventoryView * ptView = GetPrimaryView();
    if (ptView) {
        UIInventoryView * ptNewView = GetPrimaryViewForMode(eBrowsingMode);
        if (ptNewView && ptNewView != ptView) {
            ACDID idSelectedItem = ptView->GetSelectedItem();
            SetPrimaryView(ptNewView);
            ptNewView->SelectItem(idSelectedItem);
        } else ptView->RequestUpdate(KEEP_ITEM_SELECTED);
    }
    ptView = GetSecondaryView();
    if (ptView) {
        UIInventoryView * ptNewView = GetSecondaryViewForMode(eBrowsingMode);
        if (ptNewView && ptNewView != ptView) {
            ACDID idSelectedItem = ptView->GetSelectedItem();
            SetSecondaryView(ptNewView);
            ptNewView->SelectItem(idSelectedItem);
        } else ptView->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}

void UIInventoryScreen::OpenSelectedSlot()
{
    EnableRing(FALSE);
    UIInventoryView * ptPrimaryView = GetPrimaryView();
    UIInventoryView * ptSecondaryView = GetSecondaryView();
    UIInventoryView * ptDefaultView = NULL;
    if (ptPrimaryView) {
        ptPrimaryView->Enable(TRUE);
        if (ptPrimaryView->CanAcceptFocus()) ptDefaultView = ptPrimaryView;
    }
    if (ptSecondaryView) {
        ptSecondaryView->Enable(TRUE);
        if (!ptDefaultView && ptSecondaryView->CanAcceptFocus()) ptDefaultView = ptSecondaryView;
    }
    if (ptDefaultView && ptDefaultView->CanAcceptFocus()) SetFocusedView(ptDefaultView);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void UIInventoryScreen::CloseSelectedSlot()
{
    SetBrowsingMode(BROWSE_LIST);
    EnableRing(TRUE);
    SetFocusedView(NULL);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    if (GetPrimaryView()) GetPrimaryView()->Enable(FALSE);
    if (GetSecondaryView()) GetSecondaryView()->Enable(FALSE);
}

BOOL UIInventoryScreen::RingHasFocus() const { return GetFocusedView() == NULL; }

void UIInventoryScreen::RequestUpdate(const MaintainSelectionOptions eSelectionType)
{
    m_bUpdateRequested = TRUE;
    if (GetPrimaryView()) GetPrimaryView()->RequestUpdate(eSelectionType);
    if (GetSecondaryView()) GetSecondaryView()->RequestUpdate(eSelectionType);
}

void UIInventoryScreen::Update()
{
    if (GetPlayer()) m_tRingUI.UpdateInput(m_nControllerIndex, sIconSelectionChanged, sub_71001f77a0);
    if (m_bUpdateRequested) { OnRequestedUpdate(); m_bUpdateRequested = FALSE; }
    OnUpdate();
    if (GetPrimaryView()) GetPrimaryView()->Update();
    if (GetSecondaryView()) GetSecondaryView()->Update();
    UIID idRing = m_tRingUI.GetContainerControlID();
    if (idRing != GetUIIDNone()) {
        UIID idRingGlow = UIIDCreateFromParent(idRing, ".RingGlow");
        UI_GetWindowFromID(idRingGlow).SetActive(RingHasFocus());
    }
    if (::Console::Screens::GetActionManager().AreActionsDirty()) {
        UpdateControllerActions(::Console::Screens::GetActionManager());
        ::Console::Screens::UpdateButtonLegendText();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }
}

}  // namespace Console
}  // namespace UIInventory

// 2.7.7 Console::Screens functions on the int vector at +0x1008/+0x1010 of their global (unnamed):
// sub_710016aa70 returns whether it is non-empty, sub_710016aa90 whether it contains the value.
extern "C" bool sub_710016aa70();
extern "C" bool sub_710016aa90(int32 nValue);

// 2.7.7 ring-icon selection filter passed to RingWrapper::UpdateInput (unnamed in the binary).
extern "C" {
static bool sub_71001f77a0(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon)
{
    const UIID idContainer = tRingWrapper.GetContainerControlID();
    UIInventory::Console::UIInventoryScreen * ptInventoryScreen = static_cast<UIInventory::Console::UIInventoryScreen *>(UI_GetControlFromID(idContainer).GetUserData());
    if (ptInventoryScreen && idIcon != GetUIIDNone() && sub_710016aa70()) {
        UIFilterType eFilterType = FILTER_NONE;
        ptInventoryScreen->GetFilterMap().Lookup(idIcon, eFilterType);
        if (!sub_710016aa90(eFilterType)) {
            if (eFilterType == FILTER_BAG) {
                return sub_710016aa90(FILTER_UPGRADEABLE_JEWEL);
            }
            return false;
        }
    }
    return true;
}
}

namespace UIInventory {
namespace Console {

void UIInventoryScreen::HandleLeftStickInput(const int32 nControllerIndex) {}

void UIInventoryScreen::HandleRightStickInput(const int32 nControllerIndex) {}

void UIInventoryScreen::HandleButtonInput(const int32, const InputButton) {}

void UIInventoryScreen::UpdateRingIcons()
{
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIFilterType eFilterType;
    UIID idIcon;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos) {
        tFilterMap.GetNextAssoc(pos, idIcon, eFilterType);
        BOOL fEnabled = m_tRingUI.IsEnabled() ? ShouldEnableRingIcon(eFilterType) : m_tRingUI.GetSelectedControlID() == idIcon;
        UIID idButton = UIIDCreateFromParent(idIcon, ".LayoutRoot.Button");
        UI_GetButtonFromID(idButton).SetShader(fEnabled ? SNO_NONE : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    }
}

void sUpdateMainhandRingIcon(const UIID & id, const Player * ptPlayer)
{
    ImageTextureFrame tImageTexFrame;
    ImageTextureFrame tImageTexFrameSel;
    if (ptPlayer && ptPlayer->ePlayerClassPortrait == PLAYER_CLASS_DEMONHUNTER) {
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponMainHandLarge_DH", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponMainHandLarge_DHSelected", tImageTexFrameSel);
    } else {
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponMainHandLarge", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponMainHandLargeSelected", tImageTexFrameSel);
    }
    UIButton & tButton = UI_GetButtonFromID(id);
    tButton.SetTextureFrameDisabled(tImageTexFrame);
    tButton.SetTextureFrame(BS_NORMAL, tImageTexFrame);
    tButton.SetTextureFrame(BS_MOUSEOVER, tImageTexFrameSel);
    tButton.SetTextureFrame(BS_PUSHED, tImageTexFrameSel);
}

void sUpdateOffhandRingIcon(const UIID & id, const Player * ptPlayer)
{
    ImageTextureFrame tImageTexFrame;
    ImageTextureFrame tImageTexFrameSel;
    switch (ptPlayer ? ptPlayer->ePlayerClassPortrait : PLAYER_CLASS_CRUSADER) {
    case PLAYER_CLASS_BARBARIAN:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_Barbarian", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_BarbarianSelecte", tImageTexFrameSel);
        break;
    case PLAYER_CLASS_DEMONHUNTER:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_DemonHunter", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_DemonHunterSelec", tImageTexFrameSel);
        break;
    case PLAYER_CLASS_WIZARD:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_Wizard", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_WizardSelected", tImageTexFrameSel);
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_WitchDoctor", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_WitchDoctorSelec", tImageTexFrameSel);
        break;
    case PLAYER_CLASS_MONK:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_Monk", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_MonkSelected", tImageTexFrameSel);
        break;
    case PLAYER_CLASS_CRUSADER:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_Generic", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_GenericSelected", tImageTexFrameSel);
        break;
    case PLAYER_CLASS_NECROMANCER:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlot_NecromancerOffhand", tImageTexFrame);
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlot_NecromancerOffhandSelected", tImageTexFrameSel);
        break;
    }
    UIButton & tButton = UI_GetButtonFromID(id);
    tButton.SetTextureFrameDisabled(tImageTexFrame);
    tButton.SetTextureFrame(BS_NORMAL, tImageTexFrame);
    tButton.SetTextureFrame(BS_MOUSEOVER, tImageTexFrameSel);
    tButton.SetTextureFrame(BS_PUSHED, tImageTexFrameSel);
}

void UIInventoryScreen::UpdateMainhandAndOffhandRingIcons(const Player * ptPlayer)
{
    const UIID & idMainHand = GetIconUIIDForFilterType(FILTER_MAIN_HAND);
    if (idMainHand != GetUIIDNone()) {
        UIID idMainHandIcon = UIIDCreateFromParent(idMainHand, ".LayoutRoot.Button");
        sUpdateMainhandRingIcon(idMainHandIcon, ptPlayer);
    }
    const UIID & idOffHand = GetIconUIIDForFilterType(FILTER_OFF_HAND);
    if (idOffHand != GetUIIDNone()) {
        UIID idOffHandIcon = UIIDCreateFromParent(idOffHand, ".LayoutRoot.Button");
        sUpdateOffhandRingIcon(idOffHandIcon, ptPlayer);
    }
}

BOOL UIInventoryScreen::CanSwitchFocusToView(const UIInventoryView * ptTargetView)
{
    return ptTargetView && GetFocusedView() != ptTargetView && ptTargetView->CanAcceptFocus();
}

UIFilterType UIInventoryScreen::GetFilterFromUIID(const UIID & id)
{
    UIFilterType eFilterType = FILTER_NONE;
    m_tFilterMap.Lookup(id, eFilterType);
    return eFilterType;
}

void UIInventoryScreen::OnRingSelectionChanged(const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon != GetUIIDNone()) {
        UpdateRingIconState(idIcon, bSelected);
        if (bSelected) {
            UIFilterType eFilterType = GetFilterFromUIID(idIcon);
            DoSetFilterType(eFilterType);
        }
    }
}

BOOL UIInventoryScreen::CanOpenForController(int32 nControllerOpening) const
{
    return nControllerOpening != -1 && PlayerGetByControllerIndex(nControllerOpening) && !IsActive();
}

void UIInventoryScreen::OnOpen() {}

void UIInventoryScreen::OnClose() {}

void UIInventoryScreen::UpdateRingIconState(const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon != GetUIIDNone()) {
        UIID idButton = UIIDCreateFromParent(idIcon, ".LayoutRoot.Button");
        UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);
    }
}

UIID UIInventoryScreen::GetIconUIIDForFilterType(const UIFilterType eFilterType)
{
    UIFilterType eValue;
    UIID idKey;
    POS pos = m_tFilterMap.GetStartAssoc();
    while (pos) {
        m_tFilterMap.GetNextAssoc(pos, idKey, eValue);
        if (eValue == eFilterType) return idKey;
    }
    return GetUIIDNone();
}

UIFilterType UIInventoryScreen::GetInitialRingSelection(const UIFilterType eFilterMin, const UIFilterType eFilterMax)
{
    UIFilterType eFilterType = GetDefaultRingSelection();
    if (eFilterType == FILTER_NONE || !ShouldEnableRingIcon(eFilterType)) {
        eFilterType = FILTER_NONE;
        for (UIFilterType eFilterTypeToUse = eFilterMin; eFilterTypeToUse <= eFilterMax; ++eFilterTypeToUse) {
            if (ShouldEnableRingIcon(eFilterTypeToUse)) return eFilterTypeToUse;
        }
    }
    return eFilterType;
}

void UIInventoryScreen::SetFilterType(const UIFilterType eFilterType)
{
    const UIID & idIcon = GetIconUIIDForFilterType(eFilterType);
    m_tRingUI.SelectControl(idIcon, sIconSelectionChanged);
    DoSetFilterType(eFilterType);
}

void UIInventoryScreen::DoSetFilterType(const UIFilterType eFilterType)
{
    if (m_eFilterType != eFilterType) {
        UIFilterType eOldFilterType = m_eFilterType;
        m_eFilterType = eFilterType;
        SetBrowsingMode(BROWSE_LIST);
        OnFilterTypeChanged(eOldFilterType, m_eFilterType);
    }
}

void UIInventoryScreen::ShowNavBar(const UIID & idParent, const LPCSTR szNavBarName)
{
    UIID idNavBarContainer = GetUIIDNone();
    idNavBarContainer.SetFromParent(idParent, "LayoutRoot.Navigation");
    UIID idNavBarName = GetUIIDNone();
    idNavBarName.SetFromParent(idNavBarContainer, szNavBarName);
    const UIControlItemArray & arChildIDs = UI_GetWindowFromID(idNavBarContainer).GetChildList();
    for (uint32 i = 0; i < arChildIDs.size(); ++i)
        UI_GetWindowFromID(arChildIDs[i].id).SetActive(arChildIDs[i].id == idNavBarName);
}

BOOL UIInventoryScreen::CanScrollDetails()
{
    BOOL bResult = FALSE;
    UIInventoryView * ptView = GetPrimaryView();
    if (ptView) bResult = ptView->CanScrollWithTriggers();
    ptView = GetSecondaryView();
    if (!bResult && ptView) bResult = ptView->CanScrollWithTriggers();
    return bResult;
}

void UIInventoryScreen::ScrollDetails(const float flDelta)
{
    UIInventoryView * ptView = GetPrimaryView();
    if (ptView && ptView->CanScrollWithTriggers()) {
        UIID idItemDetails = ptView->GetScrollBoxUIID();
        if (UI_TestID(idItemDetails)) UIUtils::Console::ScrollContentInScrollBox(idItemDetails, flDelta);
    }
    ptView = GetSecondaryView();
    if (ptView && ptView->CanScrollWithTriggers()) {
        UIID idItemDetails = ptView->GetScrollBoxUIID();
        if (UI_TestID(idItemDetails)) UIUtils::Console::ScrollContentInScrollBox(idItemDetails, flDelta);
    }
}

void UIInventoryScreen::ResetDetailsScroll()
{
    UIInventoryView * ptView = GetPrimaryView();
    if (ptView) {
        UIID idItemDetails;
        idItemDetails.SetFromParent(ptView->GetRootUIID(), "LayoutRoot.ItemDetails.Item.ScrollBox");
        if (UI_TestID(idItemDetails)) UIUtils::Console::ResetScrollBox(idItemDetails);
    }
    ptView = GetSecondaryView();
    if (ptView) {
        UIID idItemDetails;
        idItemDetails.SetFromParent(ptView->GetRootUIID(), "LayoutRoot.ItemDetails.Item.ScrollBox");
        if (UI_TestID(idItemDetails)) UIUtils::Console::ResetScrollBox(idItemDetails);
    }
}

static void sIconSelectionChanged(const ::Console::RingWrapper & tRingWrapper, const UIID & idIcon, const BOOL bSelected)
{
    const UIID idContainer = tRingWrapper.GetContainerControlID();
    UIInventoryScreen * ptInventoryScreen = static_cast<UIInventoryScreen *>(UI_GetControlFromID(idContainer).GetUserData());
    if (ptInventoryScreen) ptInventoryScreen->OnRingSelectionChanged(idIcon, bSelected);
}

BOOL UIInventoryScreen::HandleLeftStickViewSwitching(const int32 nControllerIndex)
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    ::Console::AnalogStickInputFilter::Rule::ID idRule = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL).HandleInput(vecStick, FALSE);
    UIInventoryView * ptNextView = NULL;
    if (idRule == ::Console::RULE_STICK_RIGHT) ptNextView = GetSecondaryView();
    else if (idRule == ::Console::RULE_STICK_LEFT) ptNextView = GetPrimaryView();
    return SwitchFocusToView(ptNextView);
}

BOOL UIInventoryScreen::SwitchFocusToView(UIInventoryView * ptView)
{
    if (!CanSwitchFocusToView(ptView)) return FALSE;
    if (GetFocusedView() && GetPlayer()) {
        ACDID idSelectedItem = GetFocusedView()->GetSelectedItem();
        if (idSelectedItem != ACDID_INVALID) {
            if (GetPlayer()->ptNewItemManager) GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
            if (GetPlayer()->ptNewEliteItemManager) GetPlayer()->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
        }
    }
    SetFocusedView(ptView);
    return TRUE;
}


static void sAnimateCurrencyChange(const UIID & idSpentText, const UIID & idSpentStoryboard,
                                  const UIID & idReturnedText, const UIID & idReturnedStoryboard,
                                  const int64 nDiff, const CurrencyType eCurrencyType)
{
    CRefString sDiff = UIText_ValueToGameCurrency(nDiff, eCurrencyType);
    if (nDiff < 0) {
        UI_GetTextFromID(idSpentText).SetText(sDiff.str(), FALSE);
        UI_GetStoryboardFromID(idSpentStoryboard).Play(FALSE, 0.0f);
    } else if (nDiff > 0) {
        UI_GetTextFromID(idReturnedText).SetText(sDiff.str(), FALSE);
        UI_GetStoryboardFromID(idReturnedStoryboard).Play(FALSE, 0.0f);
    }
}

void UIInventoryScreen::UpdateCurrencyDisplay(const UIID & idAmountText, const UIID & idSpentText,
                                             const UIID & idSpentStoryboard, const UIID & idReturnedText,
                                             const UIID & idReturnedStoryboard, const CurrencyType eDisplayedCurrency)
{
    if (!GetPlayer()) return;
    ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
    if (!ptACDPlayer) return;
    uint64 uCurrent[NUM_CURRENCY_TYPES];
    int64 nDiff[NUM_CURRENCY_TYPES];
    for (int32 i = 0; i < NUM_CURRENCY_TYPES; ++i) {
        uCurrent[i] = ptACDPlayer->GetCurrencyAmount(i);
        nDiff[i] = uCurrent[i] - m_uLastKnownAmount[i];
        m_uLastKnownAmount[i] = uCurrent[i];
    }
    CRefString sCurrency;
    if (eDisplayedCurrency != CURRENCY_INVALID) {
        sAnimateCurrencyChange(idSpentText, idSpentStoryboard, idReturnedText, idReturnedStoryboard,
                               nDiff[eDisplayedCurrency], eDisplayedCurrency);
        if (eDisplayedCurrency == CURRENCY_BLOODSHARDS) {
            // GB_ITEMS = 2, from Definitions/GameBalanceDefinition.h:49.
            GBHandle hGB(2, GlobalGBIDGet(GLOBALGBID_CURRENCY_BLOODSHARDS));
            uint64 uBloodShardCap = ItemGetMaxStackSize(hGB, ptACDPlayer, NULL);
            sCurrency = UnicodeText::FormatNumber(uCurrent[CURRENCY_BLOODSHARDS], TRUE, FNS_NORMAL, XLOCALE_INVALID);
            sCurrency += CRefString("/") + UIText_ValueToGameCurrency(uBloodShardCap, CURRENCY_BLOODSHARDS);
        } else {
            sCurrency = UIText_ValueToGameCurrency(uCurrent[eDisplayedCurrency], eDisplayedCurrency);
        }
    } else {
        for (int32 i = 0; i < NUM_CURRENCY_TYPES; ++i) {
            if (nDiff[i]) {
                sAnimateCurrencyChange(idSpentText, idSpentStoryboard, idReturnedText, idReturnedStoryboard, nDiff[i], i);
                break;
            }
        }
        // GB_ITEMS = 2, from Definitions/GameBalanceDefinition.h:49.
        GBHandle gbHBloodShards(2, GlobalGBIDGet(GLOBALGBID_CURRENCY_BLOODSHARDS));
        uint64 uBloodShardCap = ItemGetMaxStackSize(gbHBloodShards, ptACDPlayer, NULL);
        sCurrency = UIText_ValueToGameCurrency(uCurrent[CURRENCY_GOLD], CURRENCY_GOLD);
        sCurrency += CRefString("  ") + UnicodeText::FormatNumber(uCurrent[CURRENCY_BLOODSHARDS], TRUE, FNS_NORMAL, XLOCALE_INVALID);
        sCurrency += CRefString("/") + UIText_ValueToGameCurrency(uBloodShardCap, CURRENCY_BLOODSHARDS);
        if (GameGetDailyPlatinumLimit())
            sCurrency += CRefString("  ") + UIText_ValueToGameCurrency(uCurrent[CURRENCY_PLATINUM], CURRENCY_PLATINUM);
    }
    UI_GetTextFromID(idAmountText).SetText(sCurrency.str(), FALSE);
}

void UIInventoryScreen::ResetCurrencyAnimations(const UIID & idSpentStoryboard, const UIID & idReturnedStoryboard)
{
    UIStoryboard & tSpentStoryboard = UI_GetStoryboardFromID(idSpentStoryboard);
    tSpentStoryboard.Stop();
    tSpentStoryboard.Reset();
    UIStoryboard & tReturnedStoryboard = UI_GetStoryboardFromID(idReturnedStoryboard);
    tReturnedStoryboard.Stop();
    tReturnedStoryboard.Reset();
}
} }
