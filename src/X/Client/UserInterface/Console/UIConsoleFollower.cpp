// Original file: X/Client/UserInterface/Console/UIConsoleFollower.cpp
// The console follower screen: the follower's equipment ring, their inventory and skill rows.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleScreenManager.h"
#include "ConsoleUIRActorManager.h"
#include "ControllerActions.h"
#include "ConsoleInputUtilities.h"
#include "Wrappers/RingWrapper.h"
#include "Wrappers/RadioSelectorWrapper.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleNewItemIndicators.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIStoryboard.h"
#include "../Interface/UIConfirmation.h"
#include "../UIID.h"
#include "../../SNOPreload.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Interface/UIHirelingCommon.h"
#include "../../RActors/RActors.h"
#include "../../../Common/Monsters.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/UserInterface/UITextControl.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Hireling.cpp
typedef int32 HirelingTrainReason;
HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling);
HirelingTrainReason HirelingCanTrainSkillReason(const ActorCommonData & tACDHireling, const SNO snoPower);
SNO HirelingGetPower(const ActorCommonData & tACDHireling, const int32 nRow, const int32 nColumn);
BOOL HirelingDoesLevelGrantNewSkillRow(const ActorCommonData & tACDHireling, int32 nLevel);
int32 HirelingGetUnlockLevelBase(const SkillKitDefinition * pdefSkillKit);
// X/Common/VisualInventory.cpp
VisualInventory * VisualInventoryGet(ACDID idACD);
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGame.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/UserInterface/UIText.cpp
CRefString FormatTruncatedNumber(const float flValue, const BOOL bUseMillions, const BOOL bIncludeSpecialChar);
// X/Client/UserInterface/Interface/UIHirelingCommon.cpp
namespace UIHireling { CRefString GetNameForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle); }
// X/Common/Powers/PowersCommon.cpp
void PowerActorGetWeaponDamageAndAttackSpeed(const ActorCommonData & tACD, float & flDamage, float & flAttackSpeed, const BOOL bWantBonuses, const PowerID idPower, const BOOL bUsedForDPS);
BOOL PowerActorMeetsActiveSkillMasteryRequirement(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
// X/Common/Items.cpp
BOOL ItemCanBeMarkedAsJunk(const ActorCommonData & tACDItem);
// X/Common/Networking/GameMessage.h
struct HirelingTrainSkillMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annHireling;  // 0x08
    SNO snoPower;  // 0x0c
};
enum {
    CMSG_HIRELING_TRAIN_SKILL = 117,
    CMSG_HIRELING_DISMISS = 399,
};
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Input/InputNX64.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(InputButton eFrom, InputButton eTo);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Sigma/Unicode.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/UI.cpp
int32 UI_GetPrimaryUIController();
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIInventory {
void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer);
void TryToQuickMoveToLocation(ActorCommonData & tACDOwner, ActorCommonData & tACDItem, const InventorySlotType eSlot);
}  // namespace UIInventory
// X/Client/UserInterface/Interface/UISkillsCommon.cpp
namespace UISkills {
CRefString GetSkillDescriptionText(ActorCommonData & tACD, SNO snoPower, int32 nSkillLevel, BOOL bIsTrait, BOOL bShowDetails);
BOOL GetName(SNO snoPower, CRefString & sName);
}  // namespace UISkills
// X/Client/UserInterface/Console/UISkillsConsole.cpp
namespace UISkills {
namespace Console {
void AnimateSkillHighlight(const UIID & idItem);
void AnimateSkillSelection(const UIID & idItem);
void AnimateEquippedSkillIcon(const UIID & idSlot);
}  // namespace Console
}  // namespace UISkills
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
void SetJunkFlag(ActorCommonData & ptACD, BOOL bJunk, Player * ptPlayer);
void DropItem(ACDID idSelectedItem, Player * ptPlayer);
BOOL PlayerIsInActiveTieredRift(const Player * ptPlayer);
BOOL ShowTieredRiftMessage(const Player * ptPlayer);
}  // namespace Console
}  // namespace UIUtils
namespace UIInventory {
namespace Console {
// X/Client/UserInterface/Console/UIInventoryConsole.cpp
void AnimateEquippedItemIcon(const UIID & idSlot);
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
void UpdateHirelingActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, ActorCommonData & tACDHireling, const SNO snoActor);
// X/Client/UserInterface/Console/UIConsoleInventory.cpp
void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight);
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idRoot, LPCSTR szNavigationContainerID, BOOL bUpdatePartyFollowerTab);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIDLG_FOLLOWER_CONSOLE, UIDLG_FOLLOWER_CONSOLE_LIST, UIDLG_FOLLOWER_CONSOLE_DETAILS,
    UIDLG_FOLLOWER_CONSOLE_EQUIPPED, UIDLG_FOLLOWER_CONSOLE_SKILLS, UISTACK_FOLLOWER_CONSOLE_SKILLS;
extern const UIID UIWND_FOLLOWER_CONSOLE_RING, UIWND_FOLLOWER_CONSOLE_RING_CURSOR, UIWND_FOLLOWER_CONSOLE_SKILLS_DIMMER,
    UIWND_FOLLOWER_CONSOLE_INV_MENU_GLOW, UIWND_FOLLOWER_CONSOLE_SKILLS_MENU_GLOW;
extern const UIID UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_1, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_2,
    UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_3, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_4,
    UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_5, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_6;
// 2.7.7 follower equipment slots ("...FollowerRing.LoadedInventorySlot7".."14"); defined in UIGlobals.cpp
extern const UIID UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_7, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_8,
    UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_9, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_10,
    UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_11, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_12,
    UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_13, UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_14;
extern const UIID UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_1, UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_2,
    UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_3, UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_4;
extern const UIID UITEXT_FOLLOWER_CONSOLE_SKILL_NAME, UITEXT_FOLLOWER_CONSOLE_SKILL_DESCRIPTION,
    UITEXT_FOLLOWER_CONSOLE_SKILL_LEVEL, UITEXT_FOLLOWER_CONSOLE_SKILLS_SUBHEADER, UITXT_FOLLOWER_CONSOLE_HEADER;
extern const UIID UITEXT_FOLLOWER_CONSOLE_NAME, UITEXT_FOLLOWER_CONSOLE_LEVEL, UITEXT_FOLLOWER_CONSOLE_DPS, UITEXT_FOLLOWER_CONSOLE_LIFE,
    UITEXT_FOLLOWER_CONSOLE_ARMOR, UITEXT_FOLLOWER_CONSOLE_STRENGTH, UITEXT_FOLLOWER_CONSOLE_INTELLIGENCE,
    UITEXT_FOLLOWER_CONSOLE_VITALITY, UITEXT_FOLLOWER_CONSOLE_DEXTERITY, UIRACTOR_CONSOLE_RING;

namespace {
BOOL FilterIsFollowerSkill(const UIFilterType eFilterType);
BOOL RingIncludeControlPredicate(const UIID & idControl);
void UpdateRingNewItemIndicators();
LPCSTR GetFollowerSpecialTextureName(ACDID idACDFollower);
}  // namespace

namespace UIFollower {
namespace Console {
enum ButtonActions {
    ACTION_OPEN_SLOT = 1,
    ACTION_CLOSE_SLOT = 2,
    ACTION_SCROLL_LIST = 3,
    ACTION_EQUIP = 4,
    ACTION_UNEQUIP = 5,
    ACTION_LIST = 6,
    ACTION_DETAILS = 7,
    ACTION_MARK_AS_JUNK = 8,
    ACTION_CLOSE = 9,
    ACTION_DISMISS = 10,
    ACTION_DROP = 11,
};

static const float SCROLLBOX_SCROLLDELTA = 4.0f;

void UpdateFollowerInfoUI(const Player & tPlayer);
void SkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
void DoDismiss(const UIID &);
void Refresh();
void OnFollowerScrollUp();
void OnFollowerScrollDown();
SNO GetSkillForSlot(const ActorCommonData & tACDHireling, const int32 nSlot);

class PlayerFollowerScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x16f0
    enum SelectionState {
        SELECTING_RING = 0,
        SELECTING_INVENTORY_ITEM = 1,
        SELECTING_SKILL = 2,
    };
public:
    virtual ~PlayerFollowerScreen() {}

    PlayerFollowerScreen();
    virtual void HandleLeftStickInput(const int32 nControllerIndex);  // vtable slot 7
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void HandleDPadInput(const ::Console::AnalogStickInputFilter::Rule::ID idRule);
    void PlaySelectionSound(const UIID & idParent, const BOOL bSelected);
    void OnSkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput);
    void Refresh();

    virtual void OpenSelectedSlot()
    {
        UIInventoryScreen::OpenSelectedSlot();
        SetSelectionState(SELECTING_INVENTORY_ITEM);
    }

    virtual void CloseSelectedSlot()
    {
        UIInventoryScreen::CloseSelectedSlot();
        SetSelectionState(SELECTING_RING);
    }

    virtual BOOL RingHasFocus() const
    {
        return m_eState == SELECTING_RING && UIInventoryScreen::RingHasFocus();
    }

    void HideBagSpace();
    // Read directly by the anonymous-namespace UpdateRingNewItemIndicators; the original access
    // path is unknown.
    using UIInventoryScreen::m_tFilterMap;
    VisualInventory m_tVisualInventory;  // 0x200
    BOOL m_bNewSkillFlag;  // 0x320
protected:
    virtual BOOL CanOpenForController(int32 nControllerOpening) const;  // vtable slot 15
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);  // vtable slot 14
    virtual void OnSetActive(BOOL bActive);  // vtable slot 16
    virtual void OnUpdate();  // vtable slot 13
    virtual void OnRequestedUpdate();  // vtable slot 12
    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper);  // vtable slot 17
    virtual UIFilterType GetDefaultRingSelection() const;  // vtable slot 18
    virtual void UpdateRingIconState(const UIID & idIcon, const BOOL bSelected);  // vtable slot 19
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventory::Console::UIInventoryView * GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventory::Console::UIInventoryView * GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
    int32 GetSelectedSlot();
    void UpdateSkillRing();
    void UpdateSkillSlotIcon(const int32 nSlot);
private:
    UIInventory::Console::InventoryItems m_tRingItemsProvider;  // 0x328
    UIInventory::Console::InventoryItems m_tItemsProvider;  // 0xc10
    UIInventory::Console::EquippedItem m_tEquippedItemProvider;  // 0x14f8
    UIInventory::Console::UIInventoryListView m_tListView;  // 0x1530
    UIInventory::Console::UIInventoryDetailsView m_tDetailsView;  // 0x15b8
    UIInventory::Console::UIInventoryDetailsView m_tEquippedView;  // 0x1630
    ::Console::RadioSelectorWrapper m_tSkillSelector;  // 0x16a8
    SNO m_snoSkillToLearn;  // 0x16e0
    BOOL m_bSkillsDirtyFlag;  // 0x16e4
    SelectionState m_eState;  // 0x16e8
    ACDID m_idLastKnownEquippedItem;  // 0x16ec
    ACDID GetSelectedItem();
    void EquipSkill();
    void OnConfirmEquipSkill(ACDID idACDFollower, SNO snoPower, const UIID &);
    void EquipItem();
    void OnCloseButton();
    void OnSwitchBrowsingMode();
    void OnUnequipButton();
    void OnDismiss();
    void OnInventoryFilterChanged();
    void OnSkillCategoryChanged();
    void ShowSkillsUI(const BOOL bShow);
    void UpdateSkillsForFilter();
    void UpdateSkillInfoUI();
    void ShowSkillDetails(const SNO snoPower);
    void AnimateHighlight();
    void AnimateSkillSelection();
    void AnimateEquippedSkillIcon();
    void AnimateEquippedItemIcon();
    void UpdateSkillsForSelectionState();
    void SetSelectionState(const SelectionState eState);
    BOOL CanLearnSelectedSkill();
    void ToggleSelectedItemAsJunk();
    BOOL HirelingHasLearnedNewSkill();

    friend void OnDPadLeft(const UIID & id, const int32 nControllerIndex);
    friend void OnDPadRight(const UIID & id, const int32 nControllerIndex);
};
static_assert(sizeof(PlayerFollowerScreen) == 0x16f0, "PlayerFollowerScreen");

inline void PlayerFollowerScreen::SetSelectionState(const SelectionState eState)
{
    m_eState = eState;
    EnableRing(eState == SELECTING_RING);
    UpdateSkillsForSelectionState();
    UpdateSkillInfoUI();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

PlayerFollowerScreen * sgptFollowerGlobals;

PlayerFollowerScreen::PlayerFollowerScreen()
    : m_bNewSkillFlag(FALSE)
    , m_tRingItemsProvider(UIInventory::Console::InventoryItems::PROVIDERMODE_FOLLOWER)
    , m_tItemsProvider(UIInventory::Console::InventoryItems::PROVIDERMODE_FOLLOWER)
    , m_tListView(m_tItemsProvider, UIDLG_FOLLOWER_CONSOLE_LIST, FALSE)
    , m_tDetailsView(m_tItemsProvider, UIDLG_FOLLOWER_CONSOLE_DETAILS, 1)
    , m_tEquippedView(m_tEquippedItemProvider, UIDLG_FOLLOWER_CONSOLE_EQUIPPED, 0)
    , m_snoSkillToLearn(-1)
    , m_bSkillsDirtyFlag(FALSE)
    , m_eState(SELECTING_RING)
    , m_idLastKnownEquippedItem(ACDID_INVALID)
{
    m_tVisualInventory.Init();
    m_tListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tEquippedView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_tSkillSelector.WrapUsingContainerControl(UISTACK_FOLLOWER_CONSOLE_SKILLS);
    m_tSkillSelector.SetCallbacks(NULL, SkillSelectionChanged);
}

void PlayerFollowerScreen::HandleLeftStickInput(const int32 nControllerIndex)
{
    if (m_eState != SELECTING_SKILL)
    {
        UIInventoryScreen::HandleLeftStickInput(nControllerIndex);
        return;
    }

    if (m_tSkillSelector.GetSelectedIndex() != -1)
    {
        Vector2D vecStickPos = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
        ::Console::AnalogStickInputFilter::Rule::ID idRule = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL).HandleInput(vecStickPos, FALSE);
        if (idRule == ::Console::RULE_STICK_LEFT)
            m_tSkillSelector.SelectPrevious(TRUE);
        else if (idRule == ::Console::RULE_STICK_RIGHT)
            m_tSkillSelector.SelectNext(TRUE);
        else
            return;
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

inline void PlayerFollowerScreen::HandleDPadInput(const ::Console::AnalogStickInputFilter::Rule::ID idRule)
{
    if (m_eState == SELECTING_SKILL && m_tSkillSelector.GetSelectedIndex() != -1)
    {
        if (idRule == ::Console::RULE_STICK_RIGHT)
            m_tSkillSelector.SelectNext(TRUE);
        else if (idRule == ::Console::RULE_STICK_LEFT)
            m_tSkillSelector.SelectPrevious(TRUE);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}
void PlayerFollowerScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    if (GetControllerIndex() != nControllerIndex)
        return;

    switch (eInputButton)
    {
    case BUTTON_1:
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
        BOOL fIsSkill = FilterIsFollowerSkill(GetFilter());

        if (GameIsWeeklyChallenge())
        {
            if (idAction != ACTION_OPEN_SLOT || fIsSkill)
                break;
        }
        else
        {
            // DWARF line 474 holds both action compares (OPEN_SLOT, then EQUIP): a switch
            switch (idAction)
            {
            case ACTION_OPEN_SLOT:
                break;
            case ACTION_EQUIP:
                if (!UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
                {
                    if (fIsSkill)
                        EquipSkill();
                    else
                        EquipItem();
                }
                return;
            default:
                return;
            }
        }

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        if (fIsSkill)
            SetSelectionState(SELECTING_SKILL);
        else
            OpenSelectedSlot();
        break;
    }
    case BUTTON_2:
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
        // DWARF line 508 holds both action compares (CLOSE_SLOT tested first): a switch
        switch (idAction)
        {
        case ACTION_CLOSE:
            OnCloseButton();
            break;
        case ACTION_CLOSE_SLOT:
            m_snoSkillToLearn = -1;
            if (GetPrimaryView() && GetPlayer() && GetPlayer()->ptNewItemManager)
            {
                ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
                if (idSelectedItem != ACDID_INVALID)
                {
                    GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
                    GetPlayer()->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
                }
            }
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
            CloseSelectedSlot();
            break;
        }
        break;
    }
    case BUTTON_3:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        OnSwitchBrowsingMode();
        break;
    case BUTTON_4:
    {
        ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
        if (idAction != ::Console::ACTION_NONE && !UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
        {
            // DWARF line 547 holds both action compares (DROP tested first): a switch
            switch (idAction)
            {
            case ACTION_DISMISS:
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
                OnDismiss();
                break;
            case ACTION_DROP:
            {
                ACDID idSelectedItem = GetSelectedItem();
                UIUtils::Console::DropItem(idSelectedItem, GetPlayer());
                break;
            }
            }
        }
        break;
    }
    case BUTTON_LEFT_STICK_BUTTON:
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON) == ACTION_UNEQUIP)
        {
            if (!UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
            {
                OnUnequipButton();
            }
        }
        break;
    case BUTTON_LEFT_TRIGGER:
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST)
            OnFollowerScrollUp();
        break;
    case BUTTON_RIGHT_TRIGGER:
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST)
            OnFollowerScrollDown();
        break;
    case BUTTON_RIGHT_STICK_BUTTON:
        if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON) == ACTION_MARK_AS_JUNK)
        {
            if (!UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
            {
                ToggleSelectedItemAsJunk();
            }
        }
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

void PlayerFollowerScreen::EquipSkill()
{
    if (CanLearnSelectedSkill())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SKILL), cgtPSODefaults);
        OnConfirmEquipSkill(PlayerGetHireling(GetPlayer()->idPlayerACD), m_snoSkillToLearn, GetUIIDNone());
        m_bSkillsDirtyFlag = TRUE;
        CloseSelectedSlot();
        AnimateSkillSelection();
        AnimateEquippedSkillIcon();
    }
}

void PlayerFollowerScreen::EquipItem()
{
    if (!GetPrimaryView())
        return;

    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
        return;

    Player * ptPlayer = GetPlayer();
    ActorCommonData * ptACDHireling = ACDTryToGet(PlayerGetHireling(ptPlayer->idPlayerACD));
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDHireling && ptACDItem)
    {
        InventorySlotType arValidSlots[4];
        arValidSlots[0] = UIInventory::Console::GetSlotForFilterType(GetFilter());
        if (arValidSlots[0] != SLOT_NONE)
        {
            ptPlayer->ptNewItemManager->UnMarkAsNew(idSelectedItem);
            ptPlayer->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
            UpdateRingNewItemIndicators();
            int32 nNumSlots = 1;
            UIInventory::QuickMoveToEmptySlot(*ptACDHireling, *ptACDItem, nNumSlots, arValidSlots, FALSE, ptPlayer);
            AnimateEquippedItemIcon();
            CloseSelectedSlot();
        }
    }
}

void PlayerFollowerScreen::OnSwitchBrowsingMode()
{
    ACDID idSelectedItem = GetSelectedItem();
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    if (idAction == ACTION_DETAILS)
        SetBrowsingMode(BROWSE_DETAILS);
    else if (idAction == ACTION_LIST)
        SetBrowsingMode(BROWSE_LIST);
    else
        return;

    if (GetPrimaryView())
        GetPrimaryView()->SelectItem(idSelectedItem);
}

void PlayerFollowerScreen::OnUnequipButton()
{
    ACDID idEquippedItem = UIInventory::Console::GetEquippedItem(GetPlayer(), GetFilter(), FALSE);
    if (idEquippedItem != ACDID_INVALID)
    {
        ActorCommonData * ptACDOwner = ACDTryToGet(GetPlayer()->idPlayerACD);
        ActorCommonData * ptACDItem = ACDTryToGet(idEquippedItem);
        UIInventory::TryToQuickMoveToLocation(*ptACDOwner, *ptACDItem, SLOT_PLAYER_BACKPACK);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}
void PlayerFollowerScreen::ToggleSelectedItemAsJunk()
{
    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem != ACDID_INVALID)
    {
        ActorCommonData * ptSelectedItemACD = ACDTryToGet(idSelectedItem);
        if (ptSelectedItemACD &&
            ItemCanBeMarkedAsJunk(*ptSelectedItemACD))
        {
            BOOL bNewJunkFlag = !ptSelectedItemACD->AttributesGetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK);
            if (GetPlayer())
            {
                UIUtils::Console::SetJunkFlag(*ptSelectedItemACD, bNewJunkFlag, GetPlayer());
                GetPlayer()->ptNewItemManager->UnMarkAsNew(idSelectedItem);
                GetPlayer()->ptNewEliteItemManager->UnMarkAsNew(idSelectedItem);
                UpdateRingNewItemIndicators();
            }

            SoundManager::PlaySound(GlobalSNOGet(bNewJunkFlag ? GLOBALSNO_SOUND_MARK_AS_JUNK : GLOBALSNO_SOUND_UNMARK_AS_JUNK), cgtPSODefaults);

            // inferred: the option select branches at DWARF line 1697, before the Refresh (1700) / ForceUpdate (1701)
            // rows, so both arms hold the same two calls and LLVM hoisted them
            if (bNewJunkFlag)
            {
                m_tItemsProvider.Refresh();
                GetPrimaryView()->ForceUpdate(UIInventory::Console::KEEP_INDEX_SELECTED);
            }
            else
            {
                m_tItemsProvider.Refresh();
                GetPrimaryView()->ForceUpdate(UIInventory::Console::KEEP_ITEM_SELECTED);
            }

            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }
}

BOOL PlayerFollowerScreen::CanOpenForController(int32 nControllerOpening) const
{
    return UIInventoryScreen::CanOpenForController(nControllerOpening);
}

void PlayerFollowerScreen::OnOpen()
{
    Player * ptPlayer = GetPlayer();
    ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);

    m_tRingItemsProvider.SetPlayer(ptPlayer);
    m_tRingItemsProvider.SetOwner(idACDHireling);
    m_tRingItemsProvider.SetItemOptions(1);
    m_tItemsProvider.SetPlayer(ptPlayer);
    m_tItemsProvider.SetOwner(idACDHireling);
    m_tEquippedItemProvider.SetPlayer(ptPlayer);
    m_tEquippedItemProvider.SetOwner(idACDHireling);

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));

    UpdateFollowerInfoUI(*ptPlayer);
    UpdateRingIcons();
    UpdateRingNewItemIndicators();
    UpdateMainhandAndOffhandRingIcons(NULL);

    UIFilterType eFilterType = GetInitialRingSelection(FILTER_FOLLOWER_START, FILTER_FOLLOWER_END);
    SetFilterType(eFilterType != FILTER_NONE ? eFilterType : FILTER_FOLLOWER_SKILL_START);
    CloseSelectedSlot();
    m_bNewSkillFlag = FALSE;
}

static void sResetRingAnimations()
{
    UIIDArray aridTargets;
    sgptFollowerGlobals->GetRingUI().GetTargetControlIDs(aridTargets);
    for (uint32 i = 0; i < aridTargets.size(); ++i)
    {
        UIID idStoryboard = UIIDCreateFromParent(aridTargets[i], ".LoadoutEquipSkill");
        if (!UI_TestID(idStoryboard))
            idStoryboard = UIIDCreateFromParent(aridTargets[i], ".LoadoutEquipItem");

        if (UI_TestID(idStoryboard))
        {
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
            tStoryboard.Stop();
            tStoryboard.Reset();
        }
    }
}

void PlayerFollowerScreen::OnClose()
{
    InputClearRemappings();
    sResetRingAnimations();
    SetFilterType(FILTER_NONE);
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
    CloseSelectedSlot();
}

void PlayerFollowerScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    InputClearRemappings();
    tActionManager.ClearAllActions();

    if (m_eState == SELECTING_RING)
    {
        if (ShouldEnableRingIcon(GetFilter()) && (!GameIsWeeklyChallenge() || !FilterIsFollowerSkill(GetFilter())))
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        if (!GameIsWeeklyChallenge())
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DISMISS, StringListFetch("Pets:PetDismiss").str());
        return;
    }

    if (CanScrollDetails())
    {
        CRefString sScrollDetails = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollDetails);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollDetails);
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

    if (m_eState == SELECTING_SKILL)
    {
        if (CanLearnSelectedSkill() || UIUtils::Console::PlayerIsInActiveTieredRift(GetPlayer()))
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_EQUIP, StringListFetch("ConsoleInventory:Equip").str());
        return;
    }

    UIInventory::Console::UIInventoryView * ptPrimaryView = GetPrimaryView();
    if (ptPrimaryView && ptPrimaryView->GetSelectedItem() != ACDID_INVALID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_EQUIP, StringListFetch("ConsoleInventory:Equip").str());

        ACDID idSelectedItem = ptPrimaryView->GetSelectedItem();
        if (idSelectedItem != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
            if (ptACDItem)
            {
                if (ItemIsJunk(*ptACDItem))
                {
                    InputRemapSingleJoyconButton(BUTTON_DPAD_RIGHT, BUTTON_RIGHT_STICK_BUTTON);
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_MARK_AS_JUNK, StringListFetch("ConsoleInventory:NotJunk").str());
                }
                else if (ItemCanBeMarkedAsJunk(*ptACDItem))
                {
                    InputRemapSingleJoyconButton(BUTTON_DPAD_RIGHT, BUTTON_RIGHT_STICK_BUTTON);
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_MARK_AS_JUNK, StringListFetch("ConsoleInventory:Junk").str());
                }
            }
        }

        if (GetBrowsingMode() == BROWSE_LIST)
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
        else
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_DROP, StringListFetch("ConsoleInventory:Drop").str());
    }

    UIInventory::Console::UIInventoryView * ptSecondaryView = GetSecondaryView();
    if (ptSecondaryView && ptSecondaryView->GetSelectedItem() != ACDID_INVALID && !GameIsWeeklyChallenge())
    {
        InputRemapSingleJoyconButton(BUTTON_DPAD_UP, BUTTON_LEFT_STICK_BUTTON);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, ACTION_UNEQUIP, StringListFetch("ConsoleInventory:Unequip").str());
    }
}

void PlayerFollowerScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        UpdateRingIcons();
        LPCSTR szTextureName = GetFollowerSpecialTextureName(PlayerGetHireling(GetPlayer()->idPlayerACD));
        ImageTextureFrame tTexFrame(CRefString().Format("UI:%s", szTextureName));
        ImageTextureFrame tTexFrameSelected(CRefString().Format("UI:%sSelected", szTextureName));
        UIButton & tButton = UI_GetButtonFromID(UIIDCreateFromParent(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_5, ".LayoutRoot.Button"));
        tButton.SetAllTextureFrames(tTexFrame);
        tButton.SetTextureFrameDisabled(tTexFrame);
        tButton.SetTextureFrame(BS_MOUSEOVER, tTexFrameSelected);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_FOLLOWER_CONSOLE, FALSE, GetUIIDNone());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_FOLLOWER_CONSOLE);
    }
}

void PlayerFollowerScreen::OnUpdate()
{
    UIFilterType eFilterType = GetFilter();
    if (eFilterType == FILTER_NONE)
        UI_GetTextFromID(UITXT_FOLLOWER_CONSOLE_HEADER).SetText(StringListFetch("ConsoleInventory:Empty"), FALSE);
    else if (FilterIsFollowerSkill(eFilterType))
        UI_GetTextFromID(UITXT_FOLLOWER_CONSOLE_HEADER).SetText(StringListFetch("ConsoleSkills:Skills"), FALSE);
    else
        UIInventory::Console::ShowFilterName(UITXT_FOLLOWER_CONSOLE_HEADER, GetFilter(), !RingHasFocus());

    ACDID idEquippedItem = UIInventory::Console::GetEquippedItem(GetPlayer(), GetFilter(), FALSE);
    if (idEquippedItem != m_idLastKnownEquippedItem)
    {
        m_tEquippedView.SelectItem(idEquippedItem);
        m_idLastKnownEquippedItem = idEquippedItem;
    }

    if (HirelingHasLearnedNewSkill())
        m_snoSkillToLearn = -1;

    UIID idStatusLine = GetSecondaryView() ? GetSecondaryView()->GetStatusLineUIID() : GetUIIDNone();
    if (idStatusLine != GetUIIDNone())
    {
        CRefString sSellValue;
        ActorCommonData * ptACDItem = ACDTryToGet(GetSecondaryView()->GetSelectedItem());
        if (ptACDItem)
        {
            CRefString sCost = UIText_ValueToGameCurrency(ItemGetCost(*ptACDItem, 0, 0, NULL, FALSE, FALSE), 0);
            Set_Text_Color(sCost, 0);
            sSellValue = LocalizedSprintf(StringListFetch("ConsoleShop:SellValueFormat").str(), sCost.str(), NULL);
        }
        UI_GetTextFromID(idStatusLine).SetText(sSellValue, FALSE);
    }

    idStatusLine = GetPrimaryView() ? GetPrimaryView()->GetStatusLineUIID() : GetUIIDNone();
    if (idStatusLine != GetUIIDNone())
    {
        int32 nUsedBagSpace = 0;
        int32 nTotalBagSpace = 0;
        m_tItemsProvider.GetBagSpace(nUsedBagSpace, nTotalBagSpace);
        CRefString sBagSpace = LocalizedSprintf(StringListFetch("ConsoleInventory:BagSpace").str(), StringListFetch("ConsoleInventory:Inventory").str(),
            UnicodeText::FormatNumber(nUsedBagSpace, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
            UnicodeText::FormatNumber(nTotalBagSpace, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
        UI_GetTextFromID(idStatusLine).SetText(sBagSpace, FALSE);
    }

    if (m_bSkillsDirtyFlag)
    {
        UpdateSkillsForFilter();
        UpdateSkillInfoUI();
        m_bSkillsDirtyFlag = FALSE;
    }
}

BOOL PlayerFollowerScreen::CanLearnSelectedSkill()
{
    if (m_snoSkillToLearn != -1)
    {
        ActorCommonData * ptACDHireling = ACDTryToGet(PlayerGetHireling(GetPlayer()->idPlayerACD));
        if (ptACDHireling)
        {
            HirelingTrainReason eReason = HirelingCanTrainSkillReason(*ptACDHireling, m_snoSkillToLearn);
            if (eReason == 0 || eReason == 2)
                return TRUE;
        }
    }
    return FALSE;
}

void PlayerFollowerScreen::OnRequestedUpdate()
{
    m_tRingItemsProvider.Refresh();
    m_tItemsProvider.Refresh();
    m_tEquippedView.SelectItem(UIInventory::Console::GetEquippedItem(GetPlayer(), GetFilter(), FALSE));
    UpdateFollowerInfoUI(*GetPlayer());
    if (sgptFollowerGlobals)
        sgptFollowerGlobals->UpdateRingIcons();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerFollowerScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_FOLLOWER_CONSOLE_RING, UIWND_FOLLOWER_CONSOLE_RING_CURSOR, RingIncludeControlPredicate, FALSE);
}

UIFilterType PlayerFollowerScreen::GetDefaultRingSelection() const
{
    return FILTER_FOLLOWER_START;
}

void PlayerFollowerScreen::UpdateRingIconState(const UIID & idIcon, const BOOL bSelected)
{
    UIID idLocked = UIIDCreateFromParent(idIcon, ".LayoutRoot.Locked");
    if (UI_TestID(idLocked))
    {
        UIID idLevel = UIIDCreateFromParent(idLocked, ".LockedFade.LevelRequirementNumber");
        UI_GetTextFromID(idLevel).SetTextColor(bSelected ? crgbaWhite : crgbaGray);
    }

    UIID idButton = UIIDCreateFromParent(idIcon, ".LayoutRoot.Button");
    UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);
}

void PlayerFollowerScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    if (!FilterIsFollowerSkill(eNewFilterType))
        OnInventoryFilterChanged();
    else
        OnSkillCategoryChanged();

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerFollowerScreen::OnInventoryFilterChanged()
{
    ShowSkillsUI(FALSE);
    m_tEquippedItemProvider.SetFilter(GetFilter());
    m_tItemsProvider.SetFilter(GetFilter());
    m_tItemsProvider.Refresh();

    if (GetPrimaryView())
        GetPrimaryView()->ForceUpdate(UIInventory::Console::RESET_SELECTION);
    if (GetSecondaryView())
        GetSecondaryView()->ForceUpdate(UIInventory::Console::RESET_SELECTION);
}

inline void PlayerFollowerScreen::OnSkillCategoryChanged()
{
    ShowSkillsUI(TRUE);
    UpdateSkillsForFilter();
    UpdateSkillInfoUI();
}

inline void PlayerFollowerScreen::ShowSkillsUI(const BOOL bShow)
{
    UI_GetWindowFromID(UIDLG_FOLLOWER_CONSOLE_SKILLS).SetActive(bShow);
    GetPrimaryViewForMode(GetBrowsingMode())->Show(!bShow);
    GetSecondaryViewForMode(GetBrowsingMode())->Show(!bShow);
}

static int32 sGetUnlockLevelForSkillSlot(ActorCommonData & tACDHireling, int32 nSlot)
{
    SNO snoSkillKit = tACDHireling.GetSkillKit();
    // DWARF lists no lock local (ScopedDefinitionLockR ctor inlined at line 1067): a temporary
    int32 nUnlockLevelBase = HirelingGetUnlockLevelBase(ScopedSkillKitDefinitionLockR(snoSkillKit));
    return nUnlockLevelBase + nSlot * 5;
}
void PlayerFollowerScreen::UpdateSkillsForFilter()
{
    ACDID idACDHireling = PlayerGetHireling(GetPlayer()->idPlayerACD);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (!ptACDHireling)
        return;

    int32 nRow = GetFilter() - FILTER_FOLLOWER_SKILL_START;
    int32 nUnlockLevel = sGetUnlockLevelForSkillSlot(*ptACDHireling, nRow);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_SKILL_LEVEL).SetText(UnicodeText::FormatNumber(nUnlockLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), FALSE);

    for (int32 nChoice = 0; nChoice < 2; ++nChoice)
    {
        UIID idWndSkill = UIIDCreateFromParent(UISTACK_FOLLOWER_CONSOLE_SKILLS, CRefString().Format(".Skill%d", nChoice + 1));
        BOOL bActive = FALSE;
        SNO snoPower = HirelingGetPower(*ptACDHireling, nRow, nChoice);
        if (snoPower != -1)
        {
            UIID idBtn = UIIDCreateFromParent(idWndSkill, ".LayoutRoot.Icon");
            UIHotbarButton & tButton = UI_GetHotbarButtonFromID(idBtn);
            HirelingTrainReason eReason = HirelingCanTrainSkillReason(*ptACDHireling, snoPower);
            HotbarButtonData tHotbarButtonData;
            tHotbarButtonData.SetSkill(snoPower, -1);
            tButton.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

            // inferred: the shader SNO is held in snoPower (GlobalSNOGet result lands in snoPower's register, DWARF line 1111)
            if (eReason == 3)
                snoPower = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE);
            else
                snoPower = -1;
            tButton.SetShader(snoPower);

            UIID idEquippedFrame = UIIDCreateFromParent(idWndSkill, ".LayoutRoot.EquippedFrame");
            UI_GetWindowFromID(idEquippedFrame).SetActive(eReason == 1);

            UIID idEquippedIcon = UIIDCreateFromParent(idWndSkill, ".LayoutRoot.EquippedIcon");
            UI_GetWindowFromID(idEquippedIcon).SetActive(eReason == 1);
            bActive = TRUE;
        }

        UI_GetWindowFromID(idWndSkill).SetActive(bActive);
    }

    m_tSkillSelector.SetSelectedIndex(-1, TRUE);
}

void PlayerFollowerScreen::UpdateSkillInfoUI()
{
    SNO snoSelectedPower = -1;
    int32 nRow = GetFilter() - FILTER_FOLLOWER_SKILL_START;

    ACDID idACDFollower = PlayerGetHireling(GetPlayer()->idPlayerACD);
    ActorCommonData * ptACDFollower = ACDTryToGet(idACDFollower);
    if (ptACDFollower)
    {
        switch (m_eState)
        {
        case SELECTING_RING:
            if (m_snoSkillToLearn != -1)
                return;

            for (int32 nChoice = 0; nChoice < 2; ++nChoice)
            {
                SNO snoPower = HirelingGetPower(*ptACDFollower, nRow, nChoice);
                if (snoPower == -1)
                    continue;

                if (HirelingCanTrainSkillReason(*ptACDFollower, snoPower) == 1)
                {
                    snoSelectedPower = snoPower;
                    break;
                }
            }
            break;

        case SELECTING_SKILL:
            // inferred: DWARF has no local for the selected index (lines 1170-1173), so it is held in snoSelectedPower
            snoSelectedPower = m_tSkillSelector.GetSelectedIndex();
            if (snoSelectedPower != -1)
                snoSelectedPower = HirelingGetPower(*ptACDFollower, nRow, snoSelectedPower);
            break;
        }
    }

    ShowSkillDetails(snoSelectedPower);
}

void PlayerFollowerScreen::UpdateSkillsForSelectionState()
{
    if (m_eState == SELECTING_RING)
        m_tSkillSelector.SetSelectedIndex(-1, TRUE);
    else
        m_tSkillSelector.SetSelectedIndex(0, TRUE);
    UI_GetWindowFromID(UIWND_FOLLOWER_CONSOLE_SKILLS_DIMMER).SetActive(m_eState != SELECTING_SKILL);
    UI_GetWindowFromID(UIWND_FOLLOWER_CONSOLE_INV_MENU_GLOW).SetActive(m_eState == SELECTING_INVENTORY_ITEM);
    UI_GetWindowFromID(UIWND_FOLLOWER_CONSOLE_SKILLS_MENU_GLOW).SetActive(m_eState == SELECTING_SKILL);
}

void PlayerFollowerScreen::ShowSkillDetails(const SNO snoPower)
{
    CRefString sName;
    CRefString sDescription;
    if (snoPower == -1)
    {
        sName = StringListFetch("ConsoleSkills:Skills");
        sDescription = StringListFetch("ConsoleSkills:NoneAssigned");
    }
    else
    {
        ActorCommonData * ptACDHireling = ACDTryToGet(PlayerGetHireling(GetPlayer()->idPlayerACD));
        if (ptACDHireling)
        {
            UISkills::GetName(snoPower, sName);
            sDescription = UISkills::GetSkillDescriptionText(*ptACDHireling, snoPower, ptACDHireling->AttributesGetInt(ATTRIBUTE_LEVEL), TRUE, FALSE);
        }
    }

    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_SKILL_NAME).SetText(sName.str(), FALSE);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_SKILL_DESCRIPTION).SetText(sDescription.str(), FALSE);
}

// DWARF (UIConsoleFollower.cpp:1233): not declared inline; its out-of-line copy (0x4c bytes) was removed by the linker
void PlayerFollowerScreen::AnimateHighlight()
{
    const UIID * ptidItem = NULL;
    int32 nSelectedIndex = m_tSkillSelector.GetSelectedIndex();
    if (nSelectedIndex != -1)
    {
        ptidItem = &m_tSkillSelector.GetUIIDAtIndex(nSelectedIndex);
    }

    // inferred: the NULL test leaves no instruction (GetUIIDAtIndex returns a reference); call is on DWARF line 1248
    if (ptidItem != NULL)
    {
        UISkills::Console::AnimateSkillHighlight(*ptidItem);
    }
}

inline void PlayerFollowerScreen::AnimateSkillSelection()
{
    int32 nIndex = m_tSkillSelector.GetSelectedIndex();
    if (nIndex != -1)
        UISkills::Console::AnimateSkillSelection(m_tSkillSelector.GetUIIDAtIndex(nIndex));
}

inline void PlayerFollowerScreen::AnimateEquippedSkillIcon()
{
    UISkills::Console::AnimateEquippedSkillIcon(GetRingUI().GetSelectedControlID());
}

inline void PlayerFollowerScreen::AnimateEquippedItemIcon()
{
    UIInventory::Console::AnimateEquippedItemIcon(GetRingUI().GetSelectedControlID());
}
// DWARF (UIConsoleFollower.cpp:1293): not declared inline; its out-of-line copy (0x64 bytes) was removed by the linker
void PlayerFollowerScreen::PlaySelectionSound(const UIID & idParent, const BOOL bSelected)
{
    UIID idButton = UIIDCreateFromParent(idParent, ".LayoutRoot.Icon");
    UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);
}

void PlayerFollowerScreen::OnSkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)
{
    if (idSkill == GetUIIDNone())
        return;

    UIID idSelected = UIIDCreateFromParent(idSkill, ".LayoutRoot.SelectedGlow");
    UI_GetWindowFromID(idSelected).SetActive(bSelected);

    UIID idFX = UIIDCreateFromParent(idSkill, ".LayoutRoot.SelectFX");
    UI_GetWindowFromID(idFX).SetActive(bSelected);

    if (bSelected)
    {
        ACDID idACDFollower = PlayerGetHireling(GetPlayer()->idPlayerACD);
        ActorCommonData * ptACDFollower = ACDTryToGet(idACDFollower);
        if (ptACDFollower)
        {
            int32 nRow = GetFilter() - FILTER_FOLLOWER_SKILL_START;
            m_snoSkillToLearn = HirelingGetPower(*ptACDFollower, nRow, nIndex);
            ShowSkillDetails(m_snoSkillToLearn);
        }

        AnimateHighlight();
    }

    PlaySelectionSound(idSkill, bSelected);
}

inline int32 PlayerFollowerScreen::GetSelectedSlot()
{
    const UIID & idSelectedIcon = GetRingUI().GetSelectedControlID();
    if (idSelectedIcon == GetUIIDNone())
        return -1;
    return (int32)(intptr)UI_GetWindowFromID(idSelectedIcon).GetUserData();
}

void PlayerFollowerScreen::UpdateSkillSlotIcon(const int32 nSlot)
{
    UIID idSkillIcon = UIIDCreateFromParent(UIWND_FOLLOWER_CONSOLE_RING, CRefString().Format(".LoadedSkillSlot%d", nSlot + 1));
    if (!UI_TestID(idSkillIcon))
        return;

    UI_GetWindowFromID(idSkillIcon).SetUserData((void *)(intptr)nSlot);

    SNO snoShader = (GetRingUI().IsEnabled() || GetSelectedSlot() == nSlot) ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE);

    Player * ptPlayer = GetPlayer();

    ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (!ptACDHireling)
        return;

    SNO snoPower = GetSkillForSlot(*ptACDHireling, nSlot);
    BOOL fHasSkill = snoPower != -1;
    BOOL fLocked = FALSE;
    if (!fHasSkill)
    {
        SNO snoRepresentativePower = HirelingGetPower(*ptACDHireling, nSlot, 0);
        BOOL bMeetsMasteryRequirement = PowerActorMeetsActiveSkillMasteryRequirement(*ptACDHireling, AutoPowerDefinitionR(snoRepresentativePower));
        fLocked = !bMeetsMasteryRequirement;
    }

    UIID idFrame;
    idFrame.SetFromParent(idSkillIcon, "LayoutRoot.Frame");
    UI_GetWindowFromID(idFrame).SetShader(snoShader);

    UIID idUnlock;
    idUnlock.SetFromParent(idSkillIcon, "LayoutRoot.UnLocked");
    UIWindow & tWndUnlock = UI_GetWindowFromID(idUnlock);
    tWndUnlock.SetActive(!fLocked && !fHasSkill);
    tWndUnlock.SetShader(snoShader);

    UIID idLock;
    idLock.SetFromParent(idSkillIcon, "LayoutRoot.Locked");
    UIWindow & tWndLock = UI_GetWindowFromID(idLock);
    tWndLock.SetActive(fLocked);
    tWndLock.SetShader(snoShader);

    if (fLocked)
    {
        int32 nUnlockLevel = sGetUnlockLevelForSkillSlot(*ptACDHireling, nSlot);
        CRefString sLevel = UnicodeText::FormatNumber(nUnlockLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str();

        UIID idUnlockLevel = UIIDCreateFromParent(idSkillIcon, ".LayoutRoot.Locked.LockedFade.LevelRequirementNumber");
        UIText & tUnlockLevelText = UI_GetTextFromID(idUnlockLevel);
        tUnlockLevelText.SetText(sLevel, FALSE);
        tUnlockLevelText.SetTextColor(crgbaGray);
    }

    UIID idIcon;
    idIcon.SetFromParent(idSkillIcon, "LayoutRoot.Icon");
    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(idIcon);
    tIcon.SetActive(fHasSkill && !fLocked);

    // inferred: original compares the flags with cmp #1 after an and (DWARF line 1442): explicit TRUE/FALSE compares
    if (fHasSkill == TRUE && fLocked == FALSE)
    {
        HotbarButtonData tHotbarButtonData;
        tHotbarButtonData.SetSkill(snoPower, -1);
        tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);
    }

    tIcon.SetShader(snoShader);

    UIID idSparkle;
    idSparkle.SetFromParent(idSkillIcon, "LayoutRoot.FX");
    UI_GetWindowFromID(idSparkle).SetActive(GetRingUI().IsEnabled() && GetSelectedSlot() == nSlot);
}

void PlayerFollowerScreen::Refresh()
{
    UpdateSkillSlotIcon(0);
    UpdateSkillSlotIcon(1);
    UpdateSkillSlotIcon(2);
    UpdateSkillSlotIcon(3);
    UpdateSkillSlotIcon(4);
    if (FilterIsFollowerSkill(GetFilter()))
    {
        UpdateSkillsForFilter();
        UpdateSkillInfoUI();
    }
    UpdateRingNewItemIndicators();
}

void PlayerFollowerScreen::InitRingFilters(UIIDToFilterMap & tFilterMap)
{
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_1, FILTER_FOLLOWER_MAIN_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_2, FILTER_FOLLOWER_RING_LEFT);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_3, FILTER_FOLLOWER_RING_RIGHT);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_4, FILTER_FOLLOWER_NECK);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_5, FILTER_FOLLOWER_SPECIAL);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_6, FILTER_FOLLOWER_OFF_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_7, FILTER_FOLLOWER_BELT);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_8, FILTER_FOLLOWER_LEGS);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_9, FILTER_FOLLOWER_FEET);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_10, FILTER_FOLLOWER_HANDS);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_11, FILTER_FOLLOWER_SHOULDERS);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_12, FILTER_FOLLOWER_CHEST);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_13, FILTER_FOLLOWER_HEAD);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_INVENTORY_SLOT_14, FILTER_FOLLOWER_WRIST);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_1, FILTER_FOLLOWER_SKILL_1);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_2, FILTER_FOLLOWER_SKILL_2);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_3, FILTER_FOLLOWER_SKILL_3);
    tFilterMap.SetAt_Tracked(UIBTN_FOLLOWER_CONSOLE_SKILL_SLOT_4, FILTER_FOLLOWER_SKILL_4);
}

UIInventory::Console::UIInventoryView * PlayerFollowerScreen::GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)
{
    if (eBrowsingMode == BROWSE_LIST)
        return &m_tListView;
    return &m_tDetailsView;
}

UIInventory::Console::UIInventoryView * PlayerFollowerScreen::GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)
{
    return &m_tEquippedView;
}

BOOL PlayerFollowerScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    if (FilterIsFollowerSkill(eFilterType))
        return TRUE;
    return m_tRingItemsProvider.GetItemsWithFilter(eFilterType).size() != 0;
}

inline ACDID PlayerFollowerScreen::GetSelectedItem()
{
    if (GetPrimaryView())
        return GetPrimaryView()->GetSelectedItem();
    return ACDID_INVALID;
}

inline void PlayerFollowerScreen::OnConfirmEquipSkill(ACDID idACDFollower, SNO snoPower, const UIID &)
{
    HirelingTrainSkillMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.annHireling = ANNFromACD(idACDFollower);
    tMessage.snoPower = snoPower;
    MessageSendToServer(CMSG_HIRELING_TRAIN_SKILL, &tMessage, sizeof(tMessage), GetPlayer());
}

inline void PlayerFollowerScreen::OnCloseButton()
{
    ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}

inline void PlayerFollowerScreen::OnDismiss()
{
    CRefString sMsg = StringListFetch("ConsoleFollower:DismissWarning");
    UIConfirmation::Show(sMsg.str(), DoDismiss, NULL, FALSE, FALSE, GetControllerIndex(), -1);
}

BOOL PlayerFollowerScreen::HirelingHasLearnedNewSkill()
{
    if (m_snoSkillToLearn != -1)
    {
        ActorCommonData * ptACDHireling = ACDTryToGet(PlayerGetHireling(GetPlayer()->idPlayerACD));
        if (ptACDHireling)
        {
            for (int32 nRow = 0; nRow < 5; ++nRow)
            {
                for (int32 nColumn = 0; nColumn < 2; ++nColumn)
                {
                    SNO snoPower = HirelingGetPower(*ptACDHireling, nRow, nColumn);
                    if (snoPower == m_snoSkillToLearn && HirelingCanTrainSkillReason(*ptACDHireling, snoPower) == 1)
                        return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void InitializeForGame()
{
    if (GameIsStartUp())
        return;

    sgptFollowerGlobals = new PlayerFollowerScreen;
}

void DisposeForGame()
{
    if (GameIsStartUp())
        return;

    ::delete sgptFollowerGlobals;
    sgptFollowerGlobals = NULL;
}

void OpenForController(int32 nControllerIndex)
{
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        sgptFollowerGlobals->UIInventoryScreen::OpenForController(nControllerIndex);
        Refresh();
        UIInventory::Console::UpdateNavBarTabState(ptPlayer, UIDLG_FOLLOWER_CONSOLE, "LayoutRoot.Navigation", TRUE);
    }
}

void Close()
{
    ::Console::UIRActorManager::SetActive(FALSE);
    if (sgptFollowerGlobals->IsActive())
    {
        sgptFollowerGlobals->UIInventoryScreen::Close();
        SNOPreload::UpdateHireling();
        SNOPreload::UpdateSkills();
        SNOPreload::UpdateEquipped();
    }
}

void Refresh()
{
    if (sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->Refresh();
}

void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive() &&
        (bForce || tOldLoc.eActorLocation != tNewLoc.eActorLocation || tNewLoc.tInvLocation.eSlot != tOldLoc.tInvLocation.eSlot))
    {
        Player * ptPlayer = sgptFollowerGlobals->GetPlayer();
        if (!ptPlayer || tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD || tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
        {
            ::Console::UIRActorManager::ForceRActorRecreate();
            sgptFollowerGlobals->RequestUpdate(UIInventory::Console::KEEP_ITEM_SELECTED);
        }
    }
}

void OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleLeftStickInput(nControllerIndex);
}

void OnRightStick(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleRightStickInput(nControllerIndex);
}

void OnDPadLeft(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleDPadInput(::Console::RULE_STICK_LEFT);
}

void OnDPadRight(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleDPadInput(::Console::RULE_STICK_RIGHT);
}

void OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_1);
}

void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_2);
}

void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_3);
}

void OnButton4(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_4);
}

void OnButtonL2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_LEFT_TRIGGER);
}

void OnButtonL3(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_LEFT_STICK_BUTTON);
}

void OnButtonR3(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_RIGHT_STICK_BUTTON);
}

void OnButtonR2(const UIID & id, const int32 nControllerIndex)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
        sgptFollowerGlobals->HandleButtonInput(nControllerIndex, BUTTON_RIGHT_TRIGGER);
}

inline void PlayerFollowerScreen::HideBagSpace()
{
    if (GetPrimaryView())
    {
        UIID idStatusLine = GetPrimaryView()->GetStatusLineUIID();
        if (idStatusLine != GetUIIDNone())
            UI_GetTextFromID(idStatusLine).SetText("", FALSE);
    }
}

void OnUpdate()
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
    {
        sgptFollowerGlobals->Update();
        UpdateRingNewItemIndicators();
        sgptFollowerGlobals->HideBagSpace();
    }
}

void OnFollowerLeveled(const ActorCommonData & tACDHireling, const int32 nLevel)
{
    if (HirelingDoesLevelGrantNewSkillRow(tACDHireling, nLevel))
        sgptFollowerGlobals->m_bNewSkillFlag = TRUE;
}

BOOL HasNewSkillBlinker()
{
    return sgptFollowerGlobals->m_bNewSkillFlag;
}
void UpdateFollowerInfoUI(const Player & tPlayer)
{
    ACDID idFollowerACD = PlayerGetHireling(tPlayer.idPlayerACD);
    ActorCommonData * ptFollowerACD = ACDTryToGet(idFollowerACD);
    if (!ptFollowerACD)
        return;

    ::Console::UIRActorManager::SetActive(TRUE);
    BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_NPC, FALSE, TRUE);
    if (bShouldCreateRActor)
    {
        RActorID idRActor = (RActorID)ptFollowerACD->idOwner;
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            VisualInventory * ptVisualInventory = VisualInventoryGet(ptFollowerACD->id);
            sgptFollowerGlobals->m_tVisualInventory.tEquipmentLast = sgptFollowerGlobals->m_tVisualInventory.tEquipmentCurrent;
            sgptFollowerGlobals->m_tVisualInventory.tEquipmentCurrent = ptVisualInventory->tEquipmentCurrent;
            UIInventory::Console::UpdateHirelingActorUI(UIRACTOR_CONSOLE_RING, sgptFollowerGlobals->m_tVisualInventory, *ptFollowerACD, ptRActor->snoActor);
        }

        UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
        LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
        tModel.SetLight(idLight);
        tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_FOLLOWER));
    }

    CRefString sName = MonsterGetName(ptFollowerACD, FALSE);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_NAME).SetText(sName.str(), FALSE);

    int32 nLevel = ptFollowerACD->AttributesGetInt(ATTRIBUTE_LEVEL);
    CRefString sLevel = LocalizedSprintf(StringListFetch("ConsoleInventory:Level").str(), UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_LEVEL).SetText(sLevel.str(), FALSE);

    float flDamage, flAttackSpeed;
    PowerActorGetWeaponDamageAndAttackSpeed(*ptFollowerACD, flDamage, flAttackSpeed, TRUE, -1, TRUE);
    float flBaseDamageScalar = UIHireling::GetBaseDamageScalarForClass(*ptFollowerACD);
    float flDPS = flBaseDamageScalar * (flDamage * flAttackSpeed);

    CRefString sDamage = FormatTruncatedNumber(flDPS, TRUE, TRUE);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_DPS).SetText(sDamage.str(), FALSE);

    float flLifeMax = ptFollowerACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_LIFE).SetText(FormatTruncatedNumber(flLifeMax, TRUE, TRUE), FALSE);

    static UIID arStatControls[] = {
        UITEXT_FOLLOWER_CONSOLE_ARMOR,
        UITEXT_FOLLOWER_CONSOLE_STRENGTH,
        UITEXT_FOLLOWER_CONSOLE_INTELLIGENCE,
        UITEXT_FOLLOWER_CONSOLE_VITALITY,
        UITEXT_FOLLOWER_CONSOLE_DEXTERITY,
    };
    static Attrib arStatAttribs[] = {
        ATTRIBUTE_ARMOR_TOTAL,
        ATTRIBUTE_STRENGTH_TOTAL,
        ATTRIBUTE_INTELLIGENCE_TOTAL,
        ATTRIBUTE_VITALITY_TOTAL,
        ATTRIBUTE_DEXTERITY_TOTAL,
    };
    for (int32 i = 0; i < 5; ++i)
    {
        float flStat = ptFollowerACD->AttributesGetFloat(arStatAttribs[i]);
        UI_GetTextFromID(arStatControls[i]).SetText(FormatTruncatedNumber(flStat, TRUE, TRUE), FALSE);
    }

    float flStat = ptFollowerACD->GetEHPValue();
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_ARMOR).SetText(FormatTruncatedNumber(flStat, TRUE, TRUE), FALSE);

    HirelingClass eClass = HirelingGetClass(*ptFollowerACD);
    CRefString sClass = UIHireling::GetNameForHirelingClass(eClass, FALSE);
    UI_GetTextFromID(UITEXT_FOLLOWER_CONSOLE_SKILLS_SUBHEADER).SetText(sClass.str(), FALSE);
}

inline void OnFollowerScrollUp()
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive() &&
        ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(sgptFollowerGlobals->GetControllerIndex(), BUTTON_RIGHT_TRIGGER))
        sgptFollowerGlobals->ScrollDetails(-SCROLLBOX_SCROLLDELTA);
}

inline void OnFollowerScrollDown()
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive() &&
        ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(sgptFollowerGlobals->GetControllerIndex(), BUTTON_LEFT_TRIGGER))
        sgptFollowerGlobals->ScrollDetails(SCROLLBOX_SCROLLDELTA);
}

inline SNO GetSkillForSlot(const ActorCommonData & tACDHireling, const int32 nSlot)
{
    for (int32 nColumn = 0; nColumn < 2; ++nColumn)
    {
        SNO snoPower = HirelingGetPower(tACDHireling, nSlot, nColumn);
        if (snoPower != -1 && HirelingCanTrainSkillReason(tACDHireling, snoPower) == 1)
            return snoPower;
    }
    return -1;
}

void SkillSelectionChanged(const UIID & idSkill, const int32 nIndex, const BOOL bSelected, const BOOL bTriggeredByInput)
{
    sgptFollowerGlobals->OnSkillSelectionChanged(idSkill, nIndex, bSelected, bTriggeredByInput);
}

void DoDismiss(const UIID &)
{
    if (sgptFollowerGlobals && sgptFollowerGlobals->IsActive())
    {
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(PlayerGetHireling(sgptFollowerGlobals->GetPlayer()->idPlayerACD));
        if (tMessage.ann != ANN_INVALID)
        {
            MessageSendToServer(CMSG_HIRELING_DISMISS, &tMessage, sizeof(tMessage), sgptFollowerGlobals->GetPlayer());
            ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        }
    }
}

}  // namespace Console
}  // namespace UIFollower

namespace {

inline BOOL FilterIsFollowerSkill(const UIFilterType eFilterType)
{
    return eFilterType - FILTER_FOLLOWER_SKILL_START < 4U;
}

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_FOLLOWER_CONSOLE_RING_CURSOR;
}

void UpdateRingNewItemIndicators()
{
    using namespace UIInventory::Console;
    Player * ptPlayer = UIFollower::Console::sgptFollowerGlobals->GetPlayer();
    NewItemIndicatorManager<ACDID> * ptNewItemManager = ptPlayer->ptNewItemManager;
    NewItemIndicatorManager<ACDID> * ptNewEliteItemManager = ptPlayer->ptNewEliteItemManager;
    RemovedItemPredicate tPredicate(SLOT_HIRELING_START, SLOT_HIRELING_END);

    ptNewItemManager->RemoveItemsIf(tPredicate);
    ptNewEliteItemManager->RemoveItemsIf(tPredicate);

    UIFilterType eFilterType;
    UIID idKey;

    const UIIDToFilterMap & tFilterMap = UIFollower::Console::sgptFollowerGlobals->m_tFilterMap;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        if (eFilterType >= FILTER_FOLLOWER_START && eFilterType <= FILTER_FOLLOWER_END)
        {
            UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
            UIID idNewEliteIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStarElite");
            BOOL bRingEnabled = UIFollower::Console::sgptFollowerGlobals->GetRingUI().IsEnabled();
            BOOL bNewElite = FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewEliteItemManager, TRUE);
            BOOL bNewItem = !bNewElite && FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewItemManager, TRUE);
            UI_GetWindowFromID(idNewIndicator).SetActive(bRingEnabled && bNewItem);
            UI_GetWindowFromID(idNewEliteIndicator).SetActive(bRingEnabled && bNewElite);
        }
    }
}

inline LPCSTR GetFollowerSpecialTextureName(ACDID idACDFollower)
{
    ActorCommonData * ptACDFollower = ACDTryToGet(idACDFollower);
    if (ptACDFollower)
    {
        switch (HirelingGetClass(*ptACDFollower))
        {
        case 1:
            return "Console_EquippedInventoryTemplar";
        case 2:
            return "Console_EquippedInventoryScoundrel";
        case 3:
            return "Console_EquippedInventoryEnchantress";
        }
    }
    return NULL;
}

}  // namespace

namespace UIInventory {
namespace Console {

void OnFollowerButton(const UIID & id, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (UIFollower::Console::sgptFollowerGlobals && UIFollower::Console::sgptFollowerGlobals->IsActive())
        UIFollower::Console::sgptFollowerGlobals->HandleButtonInput(nControllerIndex, eInputButton);
}

}  // namespace Console
}  // namespace UIInventory
