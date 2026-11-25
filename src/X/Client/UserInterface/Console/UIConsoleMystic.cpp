// Original file: X/Client/UserInterface/Console/UIConsoleMystic.cpp
// The console mystic screen: enchant, transmogrify, dye and training tabs.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "ConsoleModalNotificationManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UIBlinkerControl.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "UIConsoleDyeItemView.h"
#include "UIConsoleEnchantView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleTraining.h"
#include "UIConsoleTransmogView.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIStoryboard.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../CGame.h"
#include "../../Input/Input.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../ImageCollection.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Networking/GameMessage.h
struct IntDataMessage;
struct EnchantAffixChooseMessage;
struct TransmogItemMessage;
struct DyeItemMessage;




enum {
    CMSG_CLOSE_INTERACTION = 22,

};
// X/Common/Crafting/ItemCraftingSwig.h
enum {
    CRAFTER_INVALID = -1,
    CRAFTER_MYSTIC = 2,
};
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Input/Input.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(int32 eButton, int32 eRemappedButton);  // X/Client/Input/Input.h
VisualInventorySlot VisualInventorySlotFromInventorySlot(const InventorySlotType eInventorySlot);  // X/Common/VisualInventory.h (VisualInventory.cpp:268)
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);  // X/Common/Items.h
void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eEffectType, int32 & nEffectLevel, BOOL bA, BOOL bB);  // X/Common/Items.h (parameter names invented)
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.h
GBID ItemGetGBIDForDyeType(DyeType eDyeType);  // X/Common/Items.h
void VisualInventoryClearAttachments(RActor * ptRActor);  // X/Client/VisualInventoryClient.h
void VisualInventoryApply(RActor * ptRActor, VisualInventory & tVisualInventory, int32 nA, PlayerClass eClass, BOOL bIsFemale);  // X/Client/VisualInventoryClient.h (parameter names invented)
namespace UIDyeItem { bool ItemHasDye(const ActorCommonData & tACDItem, GBID gbidDye); DyeType GetDyeTypeForItemGBID(GBID gbidItem); }  // X/Client/UserInterface/Interface/UIDyeItemCommon.cpp
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/Console/UIConsoleFastEquip.cpp
namespace UIFastEquip { namespace Console { void CloseForPlayer(const int32 nPlayerIndex); } }
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
enum InfoPanelType {
    NO_INFO_PANEL = -1,
    CRAFTING_SUMMARY_INFO_PANEL = 3,
    MYSTIC_ENCHANT_LOCKED_PANEL = 11,
};
CRefString GetFilterName(const UIFilterType eFilter);
int32 sGetRequiredCrafterLevelForFilterType(UIFilterType eFilterType);
void ClearStatusLine(UIInventoryView * ptView);
void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight);
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
void UpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl, VisualInventory & tVisualInventory);
}  // namespace Console
}  // namespace UIInventory

// X/Common/Items.h






// X/Common/Items.cpp
ItemEnchantAffixError ItemCanEnchantAffix(const ActorCommonData & tACDItem);
BOOL ItemPlayerHasEnchantAffixReagents(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem);
// X/Client/UserInterface/Console/ConsoleScreenManager.h
namespace Console {
namespace Screens {
enum {
    MYSTIC_ENCHANT_SCREEN = 0,
    MYSTIC_TRANSMOGRIFY_SCREEN = 1,
    MYSTIC_DYE_ITEM_SCREEN = 2,
    MYSTIC_TRAIN_SCREEN = 3,
};
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Interface/UITutorials.h
struct UITutorial;
#include "../Interface/UITutorials.h"




extern UITutorialManager gtTutorialMgr;
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame { void ShowTransmogRewardNotification(ACDID idACDPlayer, GBID gbidTransmog); }
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIInventory { void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD); }
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
// X/Client/Render/Light.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGameAsync.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
void UpdateRecipeRequirementsUI(const UIID & idParent, const GBID gbidRecipe, const Player & tPlayer);
void UpdateTrainingUI(const CrafterType eCrafterType, const Player & tPlayer);
void UpdateRecipeRequirementsUI(const UIID & idParent, const Player & tPlayer, uint64 nCost, const RecipeIngredient * arRecipeIngredients, uint32 nNumIngredients);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS,
    UIDLG_INVENTORY_CONSOLE_TRAINING, UIDLG_INVENTORY_CONSOLE_ENCHANT, UIDLG_INVENTORY_CONSOLE_TRANSMOGRIFY,
    UIDLG_INVENTORY_CONSOLE_DYE_ITEM, UIDLG_INVENTORY_CONSOLE_MYSTIC_REWARDS_LIST, UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL;
extern const UIID UIRACTOR_CONSOLE_RING, UITXT_CONSOLE_PLAYER_LABEL_RING, UITXT_MYSTIC_ENCHANT_LOCKED_DETAILS; extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;
extern const UIID UIWND_MYSTIC_ENCHANT_REWARD_SLOT, UITXT_MYSTIC_ENCHANT_REWARD_COUNT, UITXT_MYSTIC_TRANSMOG_REWARD_COUNT;
extern const UIID UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, UIWND_INVENTORY_CONSOLE_RING_GLOW,
    UIWND_INVENTORY_CONSOLE_ROOT, UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER, UITXT_INVENTORY_CONSOLE_GOLD; extern const UIID UITXT_INVENTORY_CONSOLE_GOLD_SPENT,  // DWARF: UIGlobals.cpp:3144 (HEADER), :3139 (GOLD); no header declarations recorded.
    UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED;

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_DETAILS = 4,
    ACTION_LIST = 5,
    ACTION_SCROLL_LIST = 6,
    ACTION_BACK = 7,
    ACTION_CANCEL = 8,
    ACTION_CLOSE = 9,
    ACTION_SELECT_ITEM_TO_ENCHANT = 10,
    ACTION_SELECT_AFFIX_TO_RECAST = 11,
    ACTION_ENCHANT = 12,
    ACTION_VIEW_REPLACEMENTS = 13,
    ACTION_VIEW_AFFIXES = 14,
    ACTION_ACCEPT_ENCHANT = 15,
    ACTION_SELECT_ITEM_TO_TRANSMOG = 16,
    ACTION_TRANSMOGRIFY = 17,
    ACTION_SELECT_ITEM_TO_DYE = 18,
    ACTION_DYE_ITEM = 19,
    ACTION_REMOVE_DYE = 20,
    ACTION_TRAIN = 21,
    ACTION_TOGGLE_STAT_RANGES = 22,
};

class PlayerMysticScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x23f0
public:
    enum MysticMode {
        MYSTIC_MODE_INVALID = -1,
        MYSTIC_MODE_ENCHANT = 0,
        MYSTIC_MODE_TRANSMOGRIFY = 1,
        MYSTIC_MODE_DYE_ITEM = 2,
        MYSTIC_MODE_TRAIN = 3,
    };

    virtual ~PlayerMysticScreen() {}  // line 120

    MysticMode GetMysticMode() const  // line 136
    {
        return m_eMysticMode;
    }

    VisualEquipment & LastDisplayedVisualEquipment()  // line 176
    {
        return m_tLastDisplayedVisualEquipment;
    }

    PlayerMysticScreen();
    void SetMysticMode(const MysticMode eMysticMode);
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void OnTrainingResults(const IntDataMessage * pMsg);
    void Train();
    void SelectAffixToRecast();
    void CancelEnchant();
    void OnEnchantAffixChoose(const EnchantAffixChooseMessage * ptMessage);
    void OnEnchantAffixResults(const ANNDataMessage * ptMessage);
    void OnTransmogResults(const TransmogItemMessage & tTransmogResults);
    void OnUnlockTransmogResults(const GBIDDataMessage * ptMessage);
    void OnDyeItemResults(const DyeItemMessage & tDyeItemResults);
    BOOL CanCloseSafely();
protected:
    virtual BOOL CanOpenForController(int32 nControllerIndex) const;  // vtable slot 15
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);  // vtable slot 14
    virtual void OnSetActive(BOOL bActive);  // vtable slot 16
    virtual void OnUpdate();  // vtable slot 13
    virtual void OnRequestedUpdate();  // vtable slot 12
    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper);  // vtable slot 17
    virtual UIFilterType GetDefaultRingSelection() const;  // vtable slot 18
    void UpdateRingNewItemIndicators();
    void UpdateLockTextColoring();
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
private:
    MysticMode m_eMysticMode;  // 0x200
    InventoryItems m_tPlayerItemsProvider;  // 0x208
    TransmogableItems m_tPlayerTransmogItemsProvider;  // 0xaf0
    DyeableItems m_tPlayerDyeableItemsProvider;  // 0x13c8
    TrainingItems m_tTrainingItemsProvider;  // 0x1ca0
    UIInventoryListView m_tPlayerListView;  // 0x1cf0
    UIInventoryDetailsView m_tPlayerDetailsView;  // 0x1d78
    UIInventoryListView m_tPlayerTransmogListView;  // 0x1df0
    UIInventoryDetailsView m_tPlayerTransmogDetailsView;  // 0x1e78
    UIInventoryListView m_tPlayerDyeableItemsListView;  // 0x1ef0
    UIInventoryDetailsView m_tPlayerDyeableItemsDetailsView;  // 0x1f78
    UIMystic::Console::UIEnchantView m_tEnchantView;  // 0x1ff0
    UIMystic::Console::UITransmogView m_tTransmogView;  // 0x2128
    UIMystic::Console::UIDyeItemView m_tDyeItemView;  // 0x21c8
    UIInventoryTrainingView m_tTrainingView;  // 0x2268
    UIInventoryListView m_tTrainingListView;  // 0x22c8
    ACDID m_idLastSelectedItem;  // 0x2350
    GBID m_gbidLastViewedTransmog;  // 0x2354
    GBID m_gbidLastViewedDye;  // 0x2358
    VisualEquipment m_tLastDisplayedVisualEquipment;  // 0x235c
    void OnButton1();
    void OnButton2();
    void OnButton3();
    void OnLeftTrigger();
    void OnRightTrigger();
    void OnDPadLeft();
    void OnLeftStickButton();
    BOOL CanClose();
    void RefreshProviders(const MaintainSelectionOptions eSelectionType);
    void UpdateNavBar();
    void UpdateLockInfo();
    void HideLockInfo();
    void UpdateEnchantActions(::Console::ControllerActionManager & tActionManager);
    void UpdateTransmogActions(::Console::ControllerActionManager & tActionManager);
    void UpdateDyeItemActions(::Console::ControllerActionManager & tActionManager);
    void UpdateTrainingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateEnchantUI();
    void UpdateTransmogUI();
    void UpdateDyeItemUI();
    void ShowTrainingUI();
    void UpdateTrainingUI();
    void SyncViews(BOOL bSelectionChanged);
    void UpdateRequiredMaterialsUI(BOOL bSelectionChanged);
    void OnNotificationDismissed(const UIID &);
    void SelectItemToEnchant();
    void TrySelectAffixToRecast();
    void TryCancelEnchant();
    void AcceptEnchant();
    BOOL CanAffordEnchant(const ACDID idItemACD);
    void SelectItemToTransmog();
    void SelectItemToDye();
    BOOL CanAffordTransmog(const GBID gbidTransmog);
    BOOL CanAffordDye(const GBID gbidDye);
    void AcceptTransmog();
    void AcceptDye();
    void ShowMysticModel();
    void ShowPlayerModel();
    void UpdateTransmogPreviewVisualInventory(const Player & tPlayer, const ACDID idACDItem, const GBID gbidSelectedTransmog);
    void UpdateDyeItemPreviewVisualInventory(const Player & tPlayer, const ACDID idACDItem, const GBID gbidSelectedDye);
    void ToggleStatRangeDisplay();
};
static_assert(sizeof(PlayerMysticScreen) == 0x23f0, "PlayerMysticScreen");

PlayerMysticScreen * sgptMysticGlobals;  // line 392
static const float SCROLLBOX_SCROLLDELTA = 4.0f;

}  // namespace Console
}  // namespace UIInventory

namespace {
BOOL RingIncludeControlPredicate(const UIID & idControl);
void EnableTabNavigation(const BOOL bEnable);
void HideRingNewItemIndicators();
#line 3069 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"
void GetIconForFilterType(
    const UIFilterType eFilterType,
    const PlayerClass ePlayerClass,
    ImageTextureFrame & tTextureFrame)
{
    switch (eFilterType) {  // Slot and class case targets come from the two tables at 0xe3e340 and 0xe3e374.
    case FILTER_MAIN_HAND:
        if (ePlayerClass == PLAYER_CLASS_DEMONHUNTER)
        {
            ImageCollectionGetImageInfo("UI:Console_GroundIcon_Crossbow", tTextureFrame);
        }
        else
        {
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponMainHand", tTextureFrame);
        }
        break;

    case FILTER_OFF_HAND:
        switch (ePlayerClass)
        {
        case PLAYER_CLASS_BARBARIAN:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_BarbarianSmall", tTextureFrame);
            break;

        case PLAYER_CLASS_CRUSADER:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_GenericSmall", tTextureFrame);
            break;

        case PLAYER_CLASS_DEMONHUNTER:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_DemonHunterSmall", tTextureFrame);
            break;

        case PLAYER_CLASS_MONK:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_MonkSmall", tTextureFrame);
            break;

        case PLAYER_CLASS_NECROMANCER:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_NecromancerSmall", tTextureFrame);
            break;

        case PLAYER_CLASS_WITCHDOCTOR:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_WitchDoctorSmall", tTextureFrame);
            break;

        case PLAYER_CLASS_WIZARD:
            ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWeaponOffHand_WizardSmall", tTextureFrame);
            break;

        }
        break;



    case FILTER_BELT:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotBelt", tTextureFrame);
        break;

    case FILTER_CHEST:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotChest", tTextureFrame);
        break;

    case FILTER_FEET:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotFeet", tTextureFrame);
        break;

    case FILTER_HANDS:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotHands", tTextureFrame);
        break;

    case FILTER_HEAD:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotHelm", tTextureFrame);
        break;

    case FILTER_LEGS:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotLegs", tTextureFrame);
        break;

    case FILTER_NECK:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotNeck", tTextureFrame);
        break;


    case FILTER_RING_LEFT:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotRing", tTextureFrame);
        break;

    case FILTER_SHOULDERS:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotShoulder", tTextureFrame);
        break;

    case FILTER_WRIST:
        ImageCollectionGetImageInfo("UI:Console_EquippedInventoryRingSlotWrist", tTextureFrame);
        break;

    }
}
#line 320 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"
}  // namespace

namespace UIInventory {
namespace Console {

void PlayerMysticScreen::ShowPlayerModel()
{
    ::Console::UIRActorManager::SetActive(TRUE);
    VisualInventory tVisualInventory;
    tVisualInventory.Init();
    UpdatePlayerInfoUI(*GetPlayer(), UIWND_INVENTORY_CONSOLE_ROOT, tVisualInventory);
    switch (m_eMysticMode)
    {
    case MYSTIC_MODE_TRANSMOGRIFY:
        UpdateTransmogPreviewVisualInventory(*GetPlayer(), GetPrimaryView()->GetSelectedItem(), m_tTransmogView.GetSelectedTransmog());
        break;
    case MYSTIC_MODE_DYE_ITEM:
        UpdateDyeItemPreviewVisualInventory(*GetPlayer(), GetPrimaryView()->GetSelectedItem(), m_tDyeItemView.GetSelectedDye());
        break;
    }
}

void PlayerMysticScreen::SyncViews(BOOL bSelectionChanged)
{
    MysticMode eMode = GetMysticMode();
    if (eMode == MYSTIC_MODE_ENCHANT)
    {
        if (m_tEnchantView.IsChoosingItem() && bSelectionChanged)
        {
            m_tEnchantView.SelectItem(GetPrimaryView()->GetSelectedItem());
            m_tEnchantView.RequestUpdate(RESET_SELECTION);
        }
    }
    else if (eMode == MYSTIC_MODE_TRANSMOGRIFY)
    {
        if (bSelectionChanged)
        {
            m_tTransmogView.SelectItem(GetPrimaryView()->GetSelectedItem());
        }
    }
    else if (eMode == MYSTIC_MODE_DYE_ITEM)
    {
        if (bSelectionChanged)
        {
            m_tDyeItemView.SelectItem(GetPrimaryView()->GetSelectedItem());
        }
    }
}

void PlayerMysticScreen::UpdateEnchantUI()
{
    static BOOL bIsRollingReplacements = FALSE;
    static BOOL bIsWaitingToFinish = FALSE;

    if (RingHasFocus())
    {
        UIID idSelectedIcon = GetRingUI().GetSelectedControlID();
        if (UI_GetWindowFromID(UIIDCreateFromParent(idSelectedIcon, ".LayoutRoot.LockedWrapper")).IsActive())
        {
            SwitchToInfoPanel(MYSTIC_ENCHANT_LOCKED_PANEL);
            UIFilterType eFilterType = GetFilter();
            int32 nLevel = sGetRequiredCrafterLevelForFilterType(eFilterType);
            CRefString sFilterName = GetFilterName(eFilterType);
            CRefString sLockDetails = LocalizedSprintf(StringListFetch("X1_ConsoleMystic:EnchantSlotLockedDetails").str(), sFilterName.str(),
                                                       UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, 0).str(), NULL);
            UI_GetTextFromID(UITXT_MYSTIC_ENCHANT_LOCKED_DETAILS).SetText(sLockDetails, FALSE);
        }
        else
        {
            SwitchToInfoPanel(NO_INFO_PANEL);
        }
    }
    else if (m_tEnchantView.IsChoosingItem())
    {
        ACDID idACDItem = RingHasFocus() ? ACDID_INVALID : GetPrimaryView()->GetSelectedItem();
        BOOL bSelectedItemChanged = m_idLastSelectedItem != idACDItem;
        SwitchToInfoPanel(CRAFTING_SUMMARY_INFO_PANEL);
        UpdateRequiredMaterialsUI(bSelectedItemChanged);
        SyncViews(bSelectedItemChanged);
        m_idLastSelectedItem = idACDItem;
    }
    else if (m_tEnchantView.IsRollingReplacementAffixes())
    {
        bIsRollingReplacements = TRUE;
    }
    else if (m_tEnchantView.IsChoosingReplacementAffix())
    {
        if (bIsRollingReplacements)
        {
            UpdateRequiredMaterialsUI(TRUE);
            bIsRollingReplacements = FALSE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }
    else if (m_tEnchantView.IsWaitingToFinish())
    {
        bIsWaitingToFinish = TRUE;
    }
    else if (m_tEnchantView.IsFinishedEnchanting() && bIsWaitingToFinish)
    {
        bIsWaitingToFinish = FALSE;
        UIInventoryView * ptPrimaryView = GetPrimaryView();
        ACDID idEnchantedItem = m_tEnchantView.GetSelectedItem();
        if (ptPrimaryView->GetSelectedItem() != idEnchantedItem)
        {
            m_tPlayerItemsProvider.Refresh();
            ptPrimaryView->SelectItem(idEnchantedItem);
            ptPrimaryView->ForceUpdate(KEEP_ITEM_SELECTED);
            m_idLastSelectedItem = idEnchantedItem;
        }
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

#line 686 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"
static UIID sGetUIIDForSlot(
    int32 nSlotNum)
{
    CHAR szNum[2];
    sprintf(szNum, "%d", nSlotNum);
    return UIIDCreateFromParent(UIWND_MYSTIC_ENCHANT_REWARD_SLOT, szNum);
}




void PlayerMysticScreen::UpdateTrainingUI()

{
    if (GetPlayer() != NULL)
    {
        UIUtils::Console::UpdateTrainingUI(CRAFTER_MYSTIC, *GetPlayer());
    }


    CRefString sTitle = StringListFetch("ConsoleCrafting:Train");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, FALSE);


    int32 nNextLevel = GetCraftLevel(CRAFTER_MYSTIC, *GetPlayer()) + 1;
    int32 nSlotCount = 0;

    PlayerClass ePlayerClass = GetPlayer()->ePlayerClassPortrait;

    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIID idKey;
    UIFilterType eFilterType;

    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);


        if (eFilterType == FILTER_RING_RIGHT)
        {
            continue;
        }

        int32 nReqCrafterLevel = sGetRequiredCrafterLevelForFilterType(eFilterType);
        if (nReqCrafterLevel == nNextLevel)
        {
            ++nSlotCount;

            ImageTextureFrame tTextureFrame;
            GetIconForFilterType(eFilterType, ePlayerClass, tTextureFrame);

            UIID idSlot = sGetUIIDForSlot(nSlotCount);
            UIWindow & tSlot = UI_GetWindowFromID(idSlot);
            tSlot.SetActive(TRUE);
            tSlot.SetTextureFrame(tTextureFrame);
        }
    }


    for (int32 i = nSlotCount; i < 2; ++i)
    {
        UIID idSlot = sGetUIIDForSlot(i + 1);
        UI_GetWindowFromID(idSlot).SetActive(FALSE);
    }


    CRefString sSlotCount = LocalizedSprintf(
        StringListFetch("X1_ConsoleMystic:EnchantReward").str(),
        UnicodeText::FormatNumber(nSlotCount, TRUE, FNS_NORMAL, 0).str(),
        NULL);
    UI_GetTextFromID(UITXT_MYSTIC_ENCHANT_REWARD_COUNT).SetText(sSlotCount, FALSE);


    int32 nCount = (int32)m_tTrainingItemsProvider.GetItems().size();
    CRefString sCount = LocalizedSprintf(
        StringListFetch("X1_ConsoleMystic:TransmogRewardCount").str(),
        UnicodeText::FormatNumber(nCount, TRUE, FNS_NORMAL, 0).str(),
        NULL);
    UI_GetTextFromID(UITXT_MYSTIC_TRANSMOG_REWARD_COUNT).SetText(sCount, FALSE);
}
#line 524 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"

void PlayerMysticScreen::UpdateTransmogUI()
{
    ACDID idACDItem = RingHasFocus() ? ACDID_INVALID : GetPrimaryView()->GetSelectedItem();
    GBID gbidTransmog = m_tTransmogView.GetSelectedTransmog();
    BOOL bSelectedItemChanged = m_idLastSelectedItem != idACDItem;
    BOOL bSelectedTransmogChanged = m_gbidLastViewedTransmog != gbidTransmog;
    UpdateRequiredMaterialsUI(bSelectedItemChanged);
    SyncViews(bSelectedItemChanged);
    if (bSelectedItemChanged || bSelectedTransmogChanged)
    {
        UpdateTransmogPreviewVisualInventory(*GetPlayer(), GetPrimaryView()->GetSelectedItem(), m_tTransmogView.GetSelectedTransmog());
    }
    m_idLastSelectedItem = idACDItem;
    m_gbidLastViewedTransmog = gbidTransmog;
    if (m_tTransmogView.IsFinishedTransmogrifying())
    {
        AcceptTransmog();
    }
}

void PlayerMysticScreen::AcceptTransmog()
{
    ACDID idTransmogItem = m_tTransmogView.GetSelectedItem();
    m_tTransmogView.Reset();
    SetBrowsingMode(BROWSE_LIST);
    SetFocusedView(&m_tPlayerTransmogListView);
    m_tPlayerTransmogListView.SelectItem(idTransmogItem);
    m_tTransmogView.SelectItem(idTransmogItem);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

void PlayerMysticScreen::UpdateDyeItemUI()
{
    ACDID idACDItem = RingHasFocus() ? ACDID_INVALID : GetPrimaryView()->GetSelectedItem();
    GBID gbidDye = m_tDyeItemView.GetSelectedDye();
    BOOL bSelectedItemChanged = m_idLastSelectedItem != idACDItem;
    BOOL bSelectedDyeChanged = m_gbidLastViewedDye != gbidDye;
    UpdateRequiredMaterialsUI(bSelectedItemChanged);
    SyncViews(bSelectedItemChanged);
    if (bSelectedItemChanged || bSelectedDyeChanged)
    {
        UpdateDyeItemPreviewVisualInventory(*GetPlayer(), GetPrimaryView()->GetSelectedItem(), m_tDyeItemView.GetSelectedDye());
    }
    m_idLastSelectedItem = idACDItem;
    m_gbidLastViewedTransmog = gbidDye;
    if (m_tDyeItemView.IsFinishedDyeing())
    {
        AcceptDye();
    }
}

void PlayerMysticScreen::AcceptDye()
{
    ACDID idItemToDye = m_tDyeItemView.GetSelectedItem();
    m_tDyeItemView.Reset();
    SetBrowsingMode(BROWSE_LIST);
    SetFocusedView(&m_tPlayerDyeableItemsListView);
    m_tPlayerDyeableItemsListView.SelectItem(idItemToDye);
    m_tDyeItemView.SelectItem(idItemToDye);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

void PlayerMysticScreen::HideLockInfo()
{
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIID idKey;
    UIFilterType eFilterType;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idLockWrapper = UIIDCreateFromParent(idKey, ".LayoutRoot.LockedWrapper");
        UI_GetWindowFromID(idLockWrapper).SetActive(FALSE);
    }
}

void PlayerMysticScreen::AcceptEnchant()
{
    EnableTabNavigation(TRUE);
    ACDID idEnchantedItem = m_tEnchantView.GetSelectedItem();
    m_tEnchantView.Reset();
    SetBrowsingMode(BROWSE_LIST);
    SetFocusedView(&m_tPlayerListView);
    m_tPlayerListView.SelectItem(idEnchantedItem);
    m_tEnchantView.SelectItem(idEnchantedItem);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

BOOL PlayerMysticScreen::CanAffordEnchant(const ACDID idItemACD)
{
    ActorCommonData * ptItemACD = ACDTryToGet(idItemACD);
    if (ptItemACD != NULL)
    {
        ActorCommonData * ptPlayerACD = ACDTryToGet(GetPlayer()->idPlayerACD);
        if (ptPlayerACD != NULL)
        {
            return ItemPlayerHasEnchantAffixReagents(*ptPlayerACD, *ptItemACD);
        }
    }
    return FALSE;
}

BOOL PlayerMysticScreen::CanAffordDye(const GBID gbidDye)
{
    if (gbidDye != GBID_NONE)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            ScopedItemDefinitionLockR pdefDye(gbidDye);
            CurrencyType eCurrencyType = CURRENCY_GOLD;
            int32 nDyeCost = pdefDye->nBaseCostGold;
            BOOL bPlayerCanAfford = ptACDPlayer->CanAffordCurrencyCost(nDyeCost, eCurrencyType);
            return bPlayerCanAfford;
        }
    }
    return FALSE;
}

void PlayerMysticScreen::UpdateLockTextColoring()
{
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIFilterType eFilterType;
    UIID idKey;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        RGBAColor rgbaColor = GetFilter() == eFilterType ? crgbaWhite : crgbaGray;
        UIID idLockWrapper = UIIDCreateFromParent(idKey, ".LayoutRoot.LockedWrapper.LevelRequirementNumber");
        UI_GetTextFromID(idLockWrapper).m_rgbaText = rgbaColor;
    }
}

void PlayerMysticScreen::OnUpdate()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    BOOL bShouldHighlight = !RingHasFocus() || m_eMysticMode == MYSTIC_MODE_TRAIN;
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(bShouldHighlight);
    if (m_eMysticMode != MYSTIC_MODE_TRAIN)
    {
        UIFilterType eFilterType = GetFilter();
        if (eFilterType == FILTER_NONE)
        {
            UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(StringListFetch("ConsoleInventory:Empty"), 0);
        }
        else
        {
            ShowFilterName(UITXT_INVENTORY_CONSOLE_HEADER, eFilterType, !RingHasFocus());
        }
    }
    UpdateCurrencyDisplay(UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED, CURRENCY_GOLD);
    switch (m_eMysticMode)
    {
    case MYSTIC_MODE_ENCHANT:
        UpdateEnchantUI();
        break;
    case MYSTIC_MODE_TRANSMOGRIFY:
        UpdateTransmogUI();
        break;
    case MYSTIC_MODE_DYE_ITEM:
        UpdateDyeItemUI();
        break;
    case MYSTIC_MODE_TRAIN:
        UpdateTrainingUI();
        break;
    }

    UIInventoryView * ptBrowsingView = GetPrimaryView();
    if (ptBrowsingView != NULL)
    {
        ClearStatusLine(ptBrowsingView);
    }
    UIInventoryView * ptDetailsView = GetSecondaryView();
    if (ptDetailsView != NULL)
    {
        ClearStatusLine(ptDetailsView);
    }
}

void PlayerMysticScreen::ShowMysticModel()
{
    BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_NPC, FALSE, TRUE);
    if (bShouldCreateRActor)
    {
        UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
        SNO snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_MYSTIC);
        tModel.CreateRActor(RENDERCONTEXT_HUD_0, snoActor);
        LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
        tModel.SetLight(idLight);
        tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_FOLLOWER));
    }
    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", 0);
    UIID idGold;
    idGold.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "GoldBalance");
    UpdateGoldAmount(idGold, GetPlayer()->idPlayerACD);
}

void PlayerMysticScreen::UpdateLockInfo()
{
    if (m_eMysticMode != MYSTIC_MODE_ENCHANT)
    {
        HideLockInfo();
        return;
    }

    int32 nCrafterLevel = GetCraftLevel(CRAFTER_MYSTIC, *GetPlayer());
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    UIFilterType eFilterType;
    UIID idKey;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        int32 nReqCrafterLevel = sGetRequiredCrafterLevelForFilterType(eFilterType);
        BOOL bShowLock = nReqCrafterLevel > nCrafterLevel;
        UIID idLockWrapper = UIIDCreateFromParent(idKey, ".LayoutRoot.LockedWrapper");
        UI_GetWindowFromID(idLockWrapper).SetActive(bShowLock);
        if (bShowLock)
        {
            UI_GetTextFromID(UIIDCreateFromParent(idLockWrapper, ".LevelRequirementNumber")).SetText(UnicodeText::FormatNumber(nReqCrafterLevel, 1, FNS_NORMAL, 0), 0);
        }
    }
}
#line 848 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"
void PlayerMysticScreen::UpdateNavBar()

{
    static LPCSTR arTabs[] = {
        "EnchantTab",
        "TransmogrifyTab",
        "DyeItemTab",
        "TrainTab"
    };

    UIID idNavBar = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE, ".LayoutRoot.Navigation.Mystic");
    int32 nTabIndex = GetMysticMode();



    for (int32 i = 0; i < (int32)(sizeof(arTabs) / sizeof(arTabs[0])); ++i)
    {
        UIID idTab = GetUIIDNone();
        idTab.SetFromParent(idNavBar, arTabs[i]);
        UI_GetWindowFromID(idTab).SetActive(i == nTabIndex);
    }


    if (nTabIndex >= 0)
    {
        UIID idBlinker = GetUIIDNone();
        idBlinker.SetFromParent(idNavBar, CRefString().Format("%s.New3", arTabs[nTabIndex]));
        Player * ptPlayer = GetPlayer();
        BOOL bActive = nTabIndex != MYSTIC_MODE_TRAIN &&
            ptPlayer != NULL &&
            GetCraftLevel(CRAFTER_MYSTIC, *ptPlayer) <= 1 &&
            ItemCrafting_CanTrain(*ptPlayer);

        UIBlinker & tBlinker = UI_GetBlinkerFromID(idBlinker);
        tBlinker.SetActive(bActive);


        if (nTabIndex != MYSTIC_MODE_TRAIN && !gtTutorialMgr.IsAnyShowing(0))
        {
            UIID idTrainTab;
            idTrainTab.SetFromParent(idNavBar, CRefString().Format("%s.Text3", arTabs[nTabIndex]));
            gtTutorialMgr.ShowTutorial(idTrainTab, GlobalSNOGet(GLOBALSNO_TUTORIAL_X1_ENCHANT_HELP_CONSOLE), ptPlayer, TRUE);
        }
    }
}
#line 787 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"

static GBID sGetItemTypeGBIDForFilter(UIFilterType eFilterType)
{
    static XGrowableMap<int, int, int, int> tItemTypeGBIDMap;

    if (tItemTypeGBIDMap.Count() == 0)
    {
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_BELT, GBIDExists("Belt", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_CHEST, GBIDExists("ChestArmor", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_FEET, GBIDExists("Boots", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_HANDS, GBIDExists("Gloves", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_HEAD, GBIDExists("Helm", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_LEGS, GBIDExists("Legs", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_MAIN_HAND, GBIDExists("Weapon", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_NECK, GBIDExists("Amulet", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_OFF_HAND, GBIDExists("Offhand", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_RING_LEFT, GBIDExists("Ring", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_RING_RIGHT, GBIDExists("Ring", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_SHOULDERS, GBIDExists("Shoulders", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_WRIST, GBIDExists("Bracers", (GameBalanceType)1));
        tItemTypeGBIDMap.SetAt_Tracked(FILTER_FOLLOWER_SPECIAL, GBIDExists("FollowerSpecial", (GameBalanceType)1));
    }

    GBID gbidItemType = GBID_NONE;
    tItemTypeGBIDMap.Lookup(eFilterType, gbidItemType);
    return gbidItemType;
}

int32 sGetRequiredCrafterLevelForFilterType(UIFilterType eFilterType)
{
    int32 nResult = -1;

    GBID gbidItemType = sGetItemTypeGBIDForFilter(eFilterType);
    if (gbidItemType != GBID_NONE)
    {
        ScopedItemTypeDefinitonLockR pdefItemType(gbidItemType);
        if (pdefItemType != NULL)
        {
            nResult = pdefItemType->nReqCrafterLevelForEnchant;
        }
    }

    return nResult;
}
// NON_MATCHING: 380/380 bytes; four lookup and free-enchant branches skip the redundant item-ID check at line 835.
// The panel guard, mode/selection order, material flag assignment, and combined versus nested cleanup were checked.
#line 806 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"
void PlayerMysticScreen::UpdateRequiredMaterialsUI(
    BOOL bSelectionChanged)
{
    if (UI_GetWindowFromID(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL).IsItselfActive())
    {
        ACDID idACDItem = RingHasFocus() ? ACDID_INVALID : GetPrimaryView()->GetSelectedItem();
        BOOL bShowIngredients = FALSE;
        if (bSelectionChanged)
        {
            if (m_eMysticMode == MYSTIC_MODE_ENCHANT && idACDItem != ACDID_INVALID)
            {
                ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                bShowIngredients = ptACDItem && !(GetPlayer()->tHeroStateData.uPlayerFlags & (1u << PLAYERFLAG_FREE_ENCHANT));
                if (bShowIngredients)
                {
                    int32 nEnchantCost = ItemCrafting_GetEnchantAffixCost(*ptACDItem);

                    RecipeIngredient arIngredients[3];
                    int32 nIngredientsCount = ItemCrafting_GetEnchantAffixReagents(*ptACDItem, arIngredients, 3);
                    UIUtils::Console::UpdateRecipeRequirementsUI(
                        UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL,
                        *GetPlayer(),
                        nEnchantCost,
                        arIngredients,
                        nIngredientsCount);
                }
            }
        }

        if (idACDItem == ACDID_INVALID && !bShowIngredients)
        {
            UIUtils::Console::UpdateRecipeRequirementsUI(
                UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL,
                GBID_NONE,
                *GetPlayer());
        }
    }
}
#line 857 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"

void PlayerMysticScreen::OnUnlockTransmogResults(const GBIDDataMessage * ptMessage)
{
    if (!ItemCrafting_CanUnlockTransmog(ptMessage->gbidData, NULL, FALSE))
    {
        return;
    }
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }
    Player * ptPlayer = PlayerGetByControllerIndex(ptMessage->tHeader.nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    ACDID idACDPlayer = ptPlayer->idPlayerACD;
    if (ACDTryToGet(idACDPlayer) != NULL)
    {
        UIGame::ShowTransmogRewardNotification(idACDPlayer, ptMessage->gbidData);
    }
    if (GetPlayer() == ptPlayer && m_eMysticMode == MYSTIC_MODE_TRANSMOGRIFY)
    {
        m_tTransmogView.OnUnlockTransmogResults(ptMessage);
    }
    gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_X1_LEGENDARY_TRANSMOG), ptPlayer, TRUE);
}

inline void PlayerMysticScreen::Train()  // line 1025
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRAIN), cgtPSODefaults);
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_VENDOR_TRY_TRAIN, &tMessage, sizeof(tMessage), GetPlayer());
}

inline void PlayerMysticScreen::SelectItemToEnchant()  // line 1131
{
    ACDID idItemACD = GetPrimaryView()->GetSelectedItem();
    m_tEnchantView.ChooseAffixesForItem(idItemACD);
    SetPrimaryView(&m_tPlayerDetailsView);
    m_tPlayerDetailsView.SelectItem(idItemACD);
    SetFocusedView(&m_tEnchantView);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

inline void sSelectAffixToRecast(const UIID &)  // line 1147
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->SelectAffixToRecast();
    }
}

void PlayerMysticScreen::TrySelectAffixToRecast()
{
    if (m_tEnchantView.SelectedItemHasEnchantedAffix())
    {
        SelectAffixToRecast();
        return;
    }

    NotificationCreationParams tParams;
    tParams.uszMessage = StringListFetch("X1_ConsoleMystic:EnchantAffixWarning");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sSelectAffixToRecast>();
    tParams.nControllerIndex = GetPlayer()->nControllerIndex;
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}

inline void PlayerMysticScreen::SelectAffixToRecast()  // line 1180
{
    EnableTabNavigation(FALSE);
    m_tEnchantView.SelectCurrentAffix();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

BOOL PlayerMysticScreen::CanAffordTransmog(const GBID gbidTransmog)
{
    if (gbidTransmog != GBID_NONE)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(m_tTransmogView.GetSelectedItem());
            if (ptACDItem != NULL && ItemCrafting_CanTransmogItem(*ptACDItem, gbidTransmog, GetPlayer(), TRUE, TRUE))
            {
                ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
                CurrencyType eCurrencyType = CURRENCY_GOLD;
                int32 nTransmogCost = pdefTransmog->nTransmogCost;
                if (ItemCrafting_ItemTransmogsMatch(ptACDItem->hGB.gbid, gbidTransmog))
                {
                    nTransmogCost = 0;
                }
                BOOL bPlayerCanAfford = ptACDPlayer->CanAffordCurrencyCost(nTransmogCost, eCurrencyType);
                return bPlayerCanAfford;
            }
        }
    }
    return FALSE;
}

inline void PlayerMysticScreen::ToggleStatRangeDisplay()  // line 1577
{
    m_tPlayerDetailsView.ToggleStatRangeDisplay();
    m_tPlayerTransmogDetailsView.ToggleStatRangeDisplay();
    m_tPlayerDyeableItemsDetailsView.ToggleStatRangeDisplay();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_TOGGLE_STATS), cgtPSODefaults);
}

inline void PlayerMysticScreen::SelectItemToTransmog()  // line 1589
{
    m_tTransmogView.ChooseTransmogForSelectedItem();
    SetFocusedView(&m_tTransmogView);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

inline void PlayerMysticScreen::SelectItemToDye()  // line 1601
{
    m_tDyeItemView.ChooseDyeForSelectedItem();
    SetFocusedView(&m_tDyeItemView);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

void PlayerMysticScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    if (GetControllerIndex() != nControllerIndex)
    {
        return;
    }

    switch (eInputButton)
    {
    case BUTTON_1:
    case BUTTON_TOUCH_PAD:
        OnButton1();
        break;
    case BUTTON_2:
        OnButton2();
        break;
    case BUTTON_3:
        OnButton3();
        break;
    case BUTTON_LEFT_TRIGGER:
        OnLeftTrigger();
        break;
    case BUTTON_RIGHT_TRIGGER:
        OnRightTrigger();
        break;
    case BUTTON_DPAD_LEFT:
        OnDPadLeft();
        break;
    case BUTTON_LEFT_STICK_BUTTON:
        OnLeftStickButton();
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

void PlayerMysticScreen::OnButton1()
{
    switch (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1))
    {
    case ACTION_OPEN_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        OpenSelectedSlot();
        break;
    case ACTION_SELECT_ITEM_TO_ENCHANT:
        SelectItemToEnchant();
        break;
    case ACTION_SELECT_AFFIX_TO_RECAST:
        TrySelectAffixToRecast();
        break;
    case ACTION_ENCHANT:
        m_tEnchantView.EnchantItem();
        break;
    case ACTION_ACCEPT_ENCHANT:
        AcceptEnchant();
        break;
    case ACTION_SELECT_ITEM_TO_TRANSMOG:
        SelectItemToTransmog();
        break;
    case ACTION_TRANSMOGRIFY:
        m_tTransmogView.Transmogrify();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    case ACTION_SELECT_ITEM_TO_DYE:
        SelectItemToDye();
        break;
    case ACTION_DYE_ITEM:
        m_tDyeItemView.ApplyDye();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    case ACTION_TRAIN:
        Train();
        break;
    }
}

void PlayerMysticScreen::OnButton2()
{
    switch (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2))
    {
    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        break;
    case ACTION_CLOSE_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        m_tEnchantView.Reset();
        m_tTransmogView.Reset();
        m_tDyeItemView.Reset();
        GetPrimaryView()->SelectItem(ACDID_INVALID);
        SetBrowsingMode(BROWSE_LIST);
        m_idLastSelectedItem = ACDID_INVALID;
        CloseSelectedSlot();
        break;
    case ACTION_BACK:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        switch (m_eMysticMode)
        {
        case MYSTIC_MODE_ENCHANT:
        {
            ACDID idSelectedItem = m_tEnchantView.GetSelectedItem();
            m_tEnchantView.GoBack();
            if (m_tEnchantView.IsChoosingItem())
            {
                SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
                SetFocusedView(GetPrimaryView());
                GetPrimaryView()->SelectItem(idSelectedItem);
            }
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;
        }
        case MYSTIC_MODE_TRANSMOGRIFY:
            m_tTransmogView.GoBack();
            SetFocusedView(GetPrimaryView());
            break;
        case MYSTIC_MODE_DYE_ITEM:
            m_tDyeItemView.GoBack();
            SetFocusedView(GetPrimaryView());
            UpdateDyeItemPreviewVisualInventory(*GetPlayer(), GetPrimaryView()->GetSelectedItem(), GBID_NONE);
            break;
        }
        break;
    case ACTION_CANCEL:
        TryCancelEnchant();
        break;
    }
}

void sCancelEnchant(const UIID &)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->CancelEnchant();
    }
}

void PlayerMysticScreen::TryCancelEnchant()
{
    if (m_tEnchantView.IsRollingReplacementAffixes())
    {
        CancelEnchant();
        return;
    }

    NotificationCreationParams tParams;
    tParams.uszMessage = StringListFetch("X1_ConsoleMystic:CancelEnchantWarning");
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sCancelEnchant>();
    tParams.nControllerIndex = GetPlayer()->nControllerIndex;
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
}

inline void PlayerMysticScreen::CancelEnchant()  // line 1814
{
    EnableTabNavigation(TRUE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
    m_tEnchantView.Reset();
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetFocusedView(GetPrimaryView());
    SyncViews(TRUE);
}

inline void PlayerMysticScreen::ShowTrainingUI()  // line 672
{
    SwitchToInfoPanel(CRAFTING_SUMMARY_INFO_PANEL);
    m_tTrainingView.SetHeaderText(StringListFetch("ConsoleCrafting:ArtisanRank"));
    m_tTrainingListView.SetHeaderText(StringListFetch("ConsoleCrafting:Rewards"));
    OpenSelectedSlot();
    UpdateTrainingUI();
}

PlayerMysticScreen::PlayerMysticScreen()
    : m_eMysticMode(MYSTIC_MODE_INVALID)
    , m_tPlayerItemsProvider(InventoryItems::PROVIDERMODE_MYSTIC_ENCHANT)
    , m_tPlayerListView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPlayerDetailsView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, TRUE)
    , m_tPlayerTransmogListView(m_tPlayerTransmogItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPlayerTransmogDetailsView(m_tPlayerTransmogItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, TRUE)
    , m_tPlayerDyeableItemsListView(m_tPlayerDyeableItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPlayerDyeableItemsDetailsView(m_tPlayerDyeableItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, TRUE)
    , m_tEnchantView(UIDLG_INVENTORY_CONSOLE_ENCHANT)
    , m_tTransmogView(UIDLG_INVENTORY_CONSOLE_TRANSMOGRIFY)
    , m_tDyeItemView(UIDLG_INVENTORY_CONSOLE_DYE_ITEM)
    , m_tTrainingView(UIDLG_INVENTORY_CONSOLE_TRAINING)
    , m_tTrainingListView(m_tTrainingItemsProvider, UIDLG_INVENTORY_CONSOLE_MYSTIC_REWARDS_LIST, TRUE)
{
    m_idLastSelectedItem = ACDID_INVALID;
    m_gbidLastViewedTransmog = GBID_NONE;
    m_gbidLastViewedDye = GBID_NONE;
    m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerTransmogListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerTransmogDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerDyeableItemsListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerDyeableItemsDetailsView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tEnchantView.SetHeaderText(StringListFetch("X1_ConsoleMystic:ItemToEnchant"));
    m_tTransmogView.SetHeaderText(StringListFetch("X1_ConsoleMystic:ItemToTransmog"));
    m_tDyeItemView.SetHeaderText(StringListFetch("X1_ConsoleMystic:AvailableDyes"));
}

void PlayerMysticScreen::SetMysticMode(const MysticMode eMysticMode)
{
    if (m_eMysticMode == eMysticMode)
    {
        return;
    }

    if (GetPrimaryView() != NULL)
    {
        GetPrimaryView()->SelectItem(ACDID_INVALID);
    }
    if (GetSecondaryView() != NULL)
    {
        GetSecondaryView()->SelectItem(ACDID_INVALID);
    }

    m_idLastSelectedItem = ACDID_INVALID;
    m_gbidLastViewedTransmog = GBID_NONE;
    m_gbidLastViewedDye = GBID_NONE;

    if (m_eMysticMode == MYSTIC_MODE_ENCHANT)
    {
        CancelEnchant();
    }

    m_tEnchantView.Reset();
    m_tTransmogView.Reset();
    m_tDyeItemView.Reset();

    m_eMysticMode = eMysticMode;
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    CloseSelectedSlot();

    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(m_eMysticMode != MYSTIC_MODE_TRAIN);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(m_eMysticMode != MYSTIC_MODE_TRAIN);

    ResetRing();
    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(GetPlayer());

    ::Console::UIRActorManager::SetActive(TRUE);

    switch (m_eMysticMode)
    {
    case MYSTIC_MODE_ENCHANT:
    case MYSTIC_MODE_TRANSMOGRIFY:
    case MYSTIC_MODE_DYE_ITEM:
        if (m_eMysticMode == MYSTIC_MODE_ENCHANT)
        {
            ShowMysticModel();
        }
        else
        {
            ShowPlayerModel();
        }
        if (GetDefaultRingSelection() == GetFilter())
        {
            SetFilterType(GetFilter());
            OnFilterTypeChanged(GetFilter(), GetFilter());
        }
        else
        {
            SetFilterType(GetDefaultRingSelection());
        }
        SwitchToInfoPanel(CRAFTING_SUMMARY_INFO_PANEL);
        break;
    case MYSTIC_MODE_TRAIN:
        ShowMysticModel();
        SetFilterType(FILTER_NONE);
        ShowTrainingUI();
        break;
    default:
        ShowPlayerModel();
        SetFilterType(FILTER_NONE);
        break;
    }

    UpdateLockInfo();
    UpdateNavBar();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerMysticScreen::OnButton3()
{
    switch (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3))
    {
    case ACTION_DETAILS:
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        SetBrowsingMode(BROWSE_DETAILS);
        GetPrimaryView()->SelectItem(idSelectedItem);
        break;
    }
    case ACTION_LIST:
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        SetBrowsingMode(BROWSE_LIST);
        GetPrimaryView()->SelectItem(idSelectedItem);
        break;
    }
    case ACTION_VIEW_REPLACEMENTS:
        m_tEnchantView.ShowPossibleReplacements(TRUE);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    case ACTION_VIEW_AFFIXES:
        m_tEnchantView.ShowPossibleReplacements(FALSE);
        ResetDetailsScroll();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    }
}

inline void PlayerMysticScreen::OnLeftTrigger()  // line 1864
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        !InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[GetControllerIndex()].tCurrentFrame.IsButtonDown(BUTTON_RIGHT_TRIGGER))
    {
        ScrollDetails(-SCROLLBOX_SCROLLDELTA);
    }
}

inline void PlayerMysticScreen::OnRightTrigger()  // line 1881
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        !InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[GetControllerIndex()].tCurrentFrame.IsButtonDown(BUTTON_LEFT_TRIGGER))
    {
        ScrollDetails(SCROLLBOX_SCROLLDELTA);
    }
}

inline void PlayerMysticScreen::OnDPadLeft()  // line 1898
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_DPAD_LEFT) == ACTION_TOGGLE_STAT_RANGES)
    {
        ToggleStatRangeDisplay();
    }
}

inline void PlayerMysticScreen::OnLeftStickButton()  // line 1910
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON) == ACTION_REMOVE_DYE)
    {
        m_tDyeItemView.RemoveDye();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}
#line 2649 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"
BOOL PlayerMysticScreen::ShouldEnableRingIcon(
    const UIFilterType eFilterType)
{
    switch (m_eMysticMode)
    {
    case MYSTIC_MODE_ENCHANT:
        {
            size_t nNumItems = m_tPlayerItemsProvider.GetItemsWithFilter(eFilterType).size();
            ACDID idACDEquipped = GetEquippedItem(GetPlayer(), eFilterType, FALSE);
            ActorCommonData * ptACDEquipped = ACDTryToGet(idACDEquipped);

            return nNumItems != 0 || (ptACDEquipped && ItemCanEnchantAffix(*ptACDEquipped) == IEAE_NONE);
        }

    case MYSTIC_MODE_TRANSMOGRIFY:
        return m_tPlayerTransmogItemsProvider.GetItemsWithFilter(eFilterType).size() != 0;


    case MYSTIC_MODE_DYE_ITEM:
        return m_tPlayerDyeableItemsProvider.GetItemsWithFilter(eFilterType).size() != 0;

    default:
        break;
    }
    return FALSE;
}
#line 1348 "X/Client/UserInterface/Console/UIConsoleMystic.cpp"

void PlayerMysticScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    switch (m_eMysticMode)
    {
    case MYSTIC_MODE_ENCHANT:
        UpdateEnchantActions(tActionManager);
        break;
    case MYSTIC_MODE_TRANSMOGRIFY:
        UpdateTransmogActions(tActionManager);
        break;
    case MYSTIC_MODE_DYE_ITEM:
        UpdateDyeItemActions(tActionManager);
        break;
    case MYSTIC_MODE_TRAIN:
        UpdateTrainingActions(tActionManager);
        break;
    }
}

BOOL PlayerMysticScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerMysticScreen::OnOpen()
{
    Player * ptPlayer = GetPlayer();
    ptPlayer->eActiveCrafterType = CRAFTER_MYSTIC;

    m_tLastDisplayedVisualEquipment.Init();

    m_tPlayerItemsProvider.SetPlayer(ptPlayer);
    m_tPlayerTransmogItemsProvider.SetPlayer(ptPlayer);
    m_tPlayerDyeableItemsProvider.SetPlayer(ptPlayer);
    m_tTrainingItemsProvider.SetPlayer(ptPlayer);

    SetMysticMode(MYSTIC_MODE_ENCHANT);

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));


    SetFocusedView(NULL);

    m_idLastSelectedItem = ACDID_INVALID;

    HideRingNewItemIndicators();

    ShowMysticModel();
}

void PlayerMysticScreen::OnClose()
{
    InputClearRemappings();
    ResetCurrencyAnimations(UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED);
    SetMysticMode(MYSTIC_MODE_INVALID);
    m_tPlayerItemsProvider.SetPlayer(NULL);
    m_tTrainingItemsProvider.SetPlayer(NULL);
    Player * ptPlayer = GetPlayer();
    ptPlayer->idACDActiveVendor = ACDID_INVALID;
    ptPlayer->eActiveCrafterType = CRAFTER_INVALID;
    HideLockInfo();
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_CLOSE_INTERACTION, &tMessage, sizeof(tMessage), ptPlayer);
}

void PlayerMysticScreen::UpdateTransmogPreviewVisualInventory(const Player & tPlayer, const ACDID idACDItem, const GBID gbidSelectedTransmog)
{
    PlayerClass eClass = PlayerGetPlayerClass(&tPlayer);
    ActorCommonData * ptACDTransmogItem = ACDTryToGet(idACDItem);
    if (ptACDTransmogItem == NULL)
    {
        return;
    }

    VisualInventorySlot eVisSlot = VisualInventorySlotFromInventorySlot(ptACDTransmogItem->GetLocationSlot());
    BOOL b2Handed = FALSE;
    if (eVisSlot == -1)
    {
        if (ItemMeetsOwnerClassRestrictions(ptACDTransmogItem->hGB.gbid, eClass, FALSE))
        {
            InventorySlotType eTransmogSlot = ItemCrafting_GetTransmogSlot(ptACDTransmogItem->hGB.gbid);
            b2Handed = eTransmogSlot == 18;
            eVisSlot = VisualInventorySlotFromInventorySlot(b2Handed ? 4 : eTransmogSlot);
        }
    }

    GBID gbidTransmog = gbidSelectedTransmog;
    BOOL bShowOriginalLook = !m_tTransmogView.IsChoosingTransmog() || gbidTransmog == GBID_NONE ||
                             !ItemCrafting_CanTransmogItem(*ptACDTransmogItem, gbidTransmog, &tPlayer, FALSE, TRUE);
    if (bShowOriginalLook)
    {
        gbidTransmog = ptACDTransmogItem->GetVisualGBID();
    }

    if (eVisSlot != -1)
    {
        VisualEquipment tEquipment = VisualInventoryGet(tPlayer.idPlayerACD)->tEquipmentCurrent;
        for (int32 i = 0; i < 4; i++)
        {
            tEquipment.tCosmeticItems[i].gbidItem = GBID_NONE;
        }
        tEquipment.tVisualItem[eVisSlot].gbidItem = gbidTransmog;
        if (b2Handed)
        {
            tEquipment.tVisualItem[5].gbidItem = GBID_NONE;
        }
        if (tEquipment.tVisualItem[eVisSlot].eDyeType == 1)
        {
            tEquipment.tVisualItem[eVisSlot].eDyeType = 0;
        }
        ACDGetItemEffectTypeAndLevel(*ptACDTransmogItem, tEquipment.tVisualItem[eVisSlot].eItemEffectType, tEquipment.tVisualItem[eVisSlot].nEffectLevel, TRUE, FALSE);

        VisualInventory tVisualInventory;
        tVisualInventory.Init();
        tVisualInventory.tEquipmentCurrent = tEquipment;
        if (tVisualInventory.tEquipmentCurrent != sgptMysticGlobals->LastDisplayedVisualEquipment())
        {
            UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
            RActor * ptRActor = RActorTryToGet(tModel.GetRActor());
            if (ptRActor != NULL)
            {
                VisualInventoryClearAttachments(ptRActor);
                VisualInventoryApply(ptRActor, tVisualInventory, 0, eClass, !PlayerIsMale(tPlayer));
                UIRActor::PlayAnimForEquipment(ptRActor, tVisualInventory.tEquipmentCurrent, eClass, 0x68700, FALSE);
                sgptMysticGlobals->LastDisplayedVisualEquipment() = tVisualInventory.tEquipmentCurrent;
            }
        }
    }
}

void PlayerMysticScreen::UpdateDyeItemPreviewVisualInventory(const Player & tPlayer, const ACDID idACDItem, const GBID gbidSelectedDye)
{
    PlayerClass eClass = PlayerGetPlayerClass(&tPlayer);
    ActorCommonData * ptACDItemToDye = ACDTryToGet(idACDItem);
    if (ptACDItemToDye == NULL)
    {
        return;
    }

    VisualInventorySlot eVisSlot = VisualInventorySlotFromInventorySlot(ptACDItemToDye->GetLocationSlot());
    if (eVisSlot == -1)
    {
        if (ItemMeetsOwnerClassRestrictions(ptACDItemToDye->hGB.gbid, eClass, FALSE))
        {
            InventorySlotType eItemSlot = GetSlotForFilterType(sgptMysticGlobals->GetFilter());
            eVisSlot = VisualInventorySlotFromInventorySlot(eItemSlot);
        }
        else
        {
            eVisSlot = -1;
        }
    }

    GBID gbidDye = gbidSelectedDye;
    BOOL bShowOriginalLook = !m_tDyeItemView.IsChoosingDye() || gbidDye == GBID_NONE;
    if (bShowOriginalLook)
    {
        gbidDye = ItemGetGBIDForDyeType(ptACDItemToDye->GetDyeType());
    }

    if (eVisSlot != -1)
    {
        VisualEquipment tEquipment = VisualInventoryGet(tPlayer.idPlayerACD)->tEquipmentCurrent;
        for (int32 i = 0; i < 4; i++)
        {
            tEquipment.tCosmeticItems[i].gbidItem = GBID_NONE;
        }
        GBID gbidTransmog = ptACDItemToDye->AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
        if (gbidTransmog == GBID_NONE)
        {
            gbidTransmog = ptACDItemToDye->hGB.gbid;
        }
        tEquipment.tVisualItem[eVisSlot].gbidItem = gbidTransmog;
        tEquipment.tVisualItem[eVisSlot].eDyeType = UIDyeItem::GetDyeTypeForItemGBID(gbidDye);
        if (tEquipment.tVisualItem[eVisSlot].eDyeType == 1)
        {
            tEquipment.tVisualItem[eVisSlot].gbidItem = GBID_NONE;
        }
        ACDGetItemEffectTypeAndLevel(*ptACDItemToDye, tEquipment.tVisualItem[eVisSlot].eItemEffectType, tEquipment.tVisualItem[eVisSlot].nEffectLevel, TRUE, FALSE);

        VisualInventory tVisualInventory;
        tVisualInventory.Init();
        tVisualInventory.tEquipmentCurrent = tEquipment;
        if (tVisualInventory.tEquipmentCurrent != sgptMysticGlobals->LastDisplayedVisualEquipment())
        {
            UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
            RActor * ptRActor = RActorTryToGet(tModel.GetRActor());
            if (ptRActor != NULL)
            {
                VisualInventoryClearAttachments(ptRActor);
                VisualInventoryApply(ptRActor, tVisualInventory, 0, eClass, !PlayerIsMale(tPlayer));
                UIRActor::PlayAnimForEquipment(ptRActor, tVisualInventory.tEquipmentCurrent, eClass, 0x68700, FALSE);
                sgptMysticGlobals->LastDisplayedVisualEquipment() = tVisualInventory.tEquipmentCurrent;
            }
        }
    }
}

void PlayerMysticScreen::UpdateEnchantActions(::Console::ControllerActionManager & tActionManager)
{
    if (RingHasFocus())
    {
        if (GetRingUI().IsEnabled())
        {
            if (ShouldEnableRingIcon(GetFilter()))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
            }
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        }
        return;
    }

    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (GetFocusedView() == GetPrimaryView())
    {
        if (idSelectedItem != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
            if (ptACDItem != NULL && m_tEnchantView.GetNumAffixes() > 0)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_ITEM_TO_ENCHANT, StringListFetch("ConsoleUI:Select").str());
            }
            if (GetBrowsingMode() == BROWSE_LIST)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            }
            else
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
            }
            if (GetBrowsingMode() == BROWSE_DETAILS)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
            }
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
    }
    else if (GetFocusedView() == GetSecondaryView())
    {
        if (m_tEnchantView.IsChoosingAffix())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
            if (m_tEnchantView.GetSelectedItem() != ACDID_INVALID)
            {
                ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
                if (ptACDItem != NULL && ItemCanEnchantAffix(*ptACDItem) == IEAE_NONE && CanAffordEnchant(idSelectedItem) && m_tEnchantView.GetNumAffixes() > 0)
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_AFFIX_TO_RECAST, StringListFetch("ConsoleUI:Select").str());
                }
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_VIEW_REPLACEMENTS, StringListFetch("X1_ConsoleMystic:ViewReplacements").str());
            }
        }
        else if (m_tEnchantView.IsRollingReplacementAffixes())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL, StringListFetch("Console:Cancel").str());
        }
        else if (m_tEnchantView.IsChoosingReplacementAffix())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ENCHANT, StringListFetch("X1_ConsoleMystic:Enchant").str());
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL, StringListFetch("Console:Cancel").str());
        }
        else if (m_tEnchantView.IsFinishedEnchanting())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_ENCHANT, StringListFetch("Console:Accept").str());
        }
        else if (m_tEnchantView.IsShowingPossibleReplacements())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_VIEW_AFFIXES, StringListFetch("X1_ConsoleMystic:ViewAffixes").str());
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        }
    }

    if (idSelectedItem != ACDID_INVALID && CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}

void PlayerMysticScreen::UpdateTransmogActions(::Console::ControllerActionManager & tActionManager)
{
    if (RingHasFocus())
    {
        if (GetRingUI().IsEnabled())
        {
            if (ShouldEnableRingIcon(GetFilter()))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
            }
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        }
        return;
    }

    BOOL bHasSelectedItem = GetPrimaryView()->GetSelectedItem() != ACDID_INVALID;
    if (GetFocusedView() == GetPrimaryView())
    {
        if (bHasSelectedItem)
        {
            if (m_tTransmogView.HasTransmogs())
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_ITEM_TO_TRANSMOG, StringListFetch("ConsoleUI:Select").str());
            }
            if (GetBrowsingMode() == BROWSE_LIST)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            }
            else
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
            }
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
    }
    else if (GetFocusedView() == &m_tTransmogView && m_tTransmogView.IsChoosingTransmog())
    {
        GBID gbidTransmog = m_tTransmogView.GetSelectedTransmog();
        if (m_tTransmogView.GetSelectedItem() != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(m_tTransmogView.GetSelectedItem());
            if (ptACDItem != NULL && !ItemCrafting_ItemTransmogsMatch(*ptACDItem, gbidTransmog) && CanAffordTransmog(gbidTransmog))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_TRANSMOGRIFY, StringListFetch("X1_ConsoleMystic:Transmogrify").str());
            }
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
    }

    if (!bHasSelectedItem)
    {
        return;
    }

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
    if (GetBrowsingMode() == BROWSE_DETAILS)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
    }
}

void PlayerMysticScreen::UpdateDyeItemActions(::Console::ControllerActionManager & tActionManager)
{
    if (RingHasFocus())
    {
        if (GetRingUI().IsEnabled())
        {
            if (ShouldEnableRingIcon(GetFilter()))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
            }
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        }
        return;
    }

    InputClearRemappings();

    BOOL bHasSelectedItem = GetPrimaryView()->GetSelectedItem() != ACDID_INVALID;
    if (GetFocusedView() == GetPrimaryView())
    {
        if (bHasSelectedItem)
        {
            if (m_tDyeItemView.HasDyes())
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_ITEM_TO_DYE, StringListFetch("ConsoleUI:Select").str());
            }
            if (GetBrowsingMode() == BROWSE_LIST)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            }
            else
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
            }
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        if (m_tDyeItemView.IsSelectedItemDyed())
        {
            InputRemapSingleJoyconButton(8, 14);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, ACTION_REMOVE_DYE, StringListFetch("X1_ConsoleMystic:RemoveDye").str());
        }
    }
    else if (GetFocusedView() == &m_tDyeItemView && m_tDyeItemView.IsChoosingDye())
    {
        GBID gbidDye = m_tDyeItemView.GetSelectedDye();
        if (m_tDyeItemView.GetSelectedItem() != ACDID_INVALID)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(m_tDyeItemView.GetSelectedItem());
            if (ptACDItem != NULL && !UIDyeItem::ItemHasDye(*ptACDItem, gbidDye) && CanAffordDye(gbidDye))
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_DYE_ITEM, StringListFetch("X1_ConsoleMystic:DyeItem").str());
            }
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        if (m_tDyeItemView.IsSelectedItemDyed())
        {
            InputRemapSingleJoyconButton(8, 14);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_STICK_BUTTON, ACTION_REMOVE_DYE, StringListFetch("X1_ConsoleMystic:RemoveDye").str());
        }
    }

    if (!bHasSelectedItem)
    {
        return;
    }

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
    if (GetBrowsingMode() == BROWSE_DETAILS && GetFocusedView() != &m_tDyeItemView)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
    }
}

void PlayerMysticScreen::UpdateTrainingActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    if (ItemCrafting_CanTrain(*GetPlayer()))
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_TRAIN, StringListFetch("ConsoleCrafting:Train").str());
    }
}

void PlayerMysticScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "Mystic");
        UpdateNavBar();
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
    UIID idStats;
    idStats.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "CharacterStats");
    UI_GetWindowFromID(idStats).SetActive(!bActive);
}

void PlayerMysticScreen::OnRequestedUpdate()
{
    m_tPlayerItemsProvider.Refresh();
    m_tPlayerTransmogItemsProvider.Refresh();
    m_tPlayerDyeableItemsProvider.Refresh();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerMysticScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, RingIncludeControlPredicate, NULL);
}

UIFilterType PlayerMysticScreen::GetDefaultRingSelection() const
{
    return FILTER_MAIN_HAND;
}

void PlayerMysticScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    switch (GetMysticMode())
    {
    case MYSTIC_MODE_ENCHANT:
        m_tPlayerItemsProvider.SetFilter(eNewFilterType);
        break;
    case MYSTIC_MODE_TRANSMOGRIFY:
        m_tPlayerTransmogItemsProvider.SetFilter(eNewFilterType);
        break;
    case MYSTIC_MODE_DYE_ITEM:
        m_tPlayerDyeableItemsProvider.SetFilter(eNewFilterType);
        break;
    case MYSTIC_MODE_TRAIN:
        m_tTrainingItemsProvider.SetFilter(eNewFilterType);
        break;
    }

    m_tPlayerItemsProvider.Refresh();
    m_tPlayerTransmogItemsProvider.Refresh();
    m_tPlayerDyeableItemsProvider.Refresh();
    UIInventoryView * ptPrimaryView = GetPrimaryView();
    if (ptPrimaryView != NULL)
    {
        ptPrimaryView->ForceUpdate(RESET_SELECTION);
    }
    UIInventoryView * ptSecondaryView = GetSecondaryView();
    if (ptSecondaryView != NULL)
    {
        ptSecondaryView->ForceUpdate(RESET_SELECTION);
    }
    UpdateLockTextColoring();
}

void PlayerMysticScreen::InitRingFilters(UIIDToFilterMap &tFilterMap)
{
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, FILTER_MAIN_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, FILTER_OFF_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_BAG, FILTER_BAG);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_BRACERS, FILTER_WRIST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_FEET, FILTER_FEET);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, FILTER_RING_LEFT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, FILTER_RING_RIGHT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_HANDS, FILTER_HANDS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_HEAD, FILTER_HEAD);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_LEGS, FILTER_LEGS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_NECK, FILTER_NECK);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, FILTER_SHOULDERS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_TORSO, FILTER_CHEST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_RING_WAIST, FILTER_BELT);
}

UIInventoryView * PlayerMysticScreen::GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
{
    switch (GetMysticMode())
    {
    case MYSTIC_MODE_TRAIN:
        return &m_tTrainingListView;
    case MYSTIC_MODE_ENCHANT:
        if (m_tEnchantView.IsChoosingAffix() || eBrowsingMode == BROWSE_DETAILS)
        {
            return &m_tPlayerDetailsView;
        }
        else
        {
            return &m_tPlayerListView;
        }
    case MYSTIC_MODE_TRANSMOGRIFY:
        switch (eBrowsingMode)
        {
        case BROWSE_LIST:
            return &m_tPlayerTransmogListView;
        case BROWSE_DETAILS:
            return &m_tPlayerTransmogDetailsView;
        }
        break;
    case MYSTIC_MODE_DYE_ITEM:
        switch (eBrowsingMode)
        {
        case BROWSE_LIST:
            return &m_tPlayerDyeableItemsListView;
        case BROWSE_DETAILS:
            return &m_tPlayerDyeableItemsDetailsView;
        }
        break;
    case MYSTIC_MODE_INVALID:
        break;
    default:
        switch (eBrowsingMode)
        {
        case BROWSE_LIST:
            return &m_tPlayerListView;
        case BROWSE_DETAILS:
            return &m_tPlayerDetailsView;
        }
        break;
    }
    return NULL;
}

UIInventoryView * PlayerMysticScreen::GetSecondaryViewForMode(const BrowsingMode)
{
    switch (GetMysticMode())
    {
    case MYSTIC_MODE_ENCHANT:
        return &m_tEnchantView;
    case MYSTIC_MODE_TRANSMOGRIFY:
        return &m_tTransmogView;
    case MYSTIC_MODE_DYE_ITEM:
        return &m_tDyeItemView;
    case MYSTIC_MODE_TRAIN:
        return &m_tTrainingView;
    }
    return NULL;
}

inline void PlayerMysticScreen::OnTrainingResults(const IntDataMessage * pMsg)  // line 1013
{
    m_tTrainingItemsProvider.Refresh();
    m_tTrainingListView.ForceUpdate(RESET_SELECTION);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerMysticScreen::OnEnchantAffixChoose(const EnchantAffixChooseMessage * ptMessage)  // line 1035
{
    if (GetMysticMode() == MYSTIC_MODE_ENCHANT && m_tEnchantView.IsWaitingForEnchantResponse())
    {
        m_tEnchantView.OnEnchantAffixChoose(ptMessage);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

inline void PlayerMysticScreen::OnEnchantAffixResults(const ANNDataMessage * ptMessage)  // line 1048
{
    if (GetMysticMode() == MYSTIC_MODE_ENCHANT && m_tEnchantView.IsWaitingToFinish())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_ENCHANT_ITEM_FINISH), cgtPSODefaults);
        m_tEnchantView.OnEnchantAffixResults(ptMessage);
    }
}

inline void PlayerMysticScreen::OnTransmogResults(const TransmogItemMessage & tTransmogResults)  // line 1061
{
    if (GetMysticMode() == MYSTIC_MODE_TRANSMOGRIFY)
    {
        m_tTransmogView.OnTransmogResults(tTransmogResults);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

inline void PlayerMysticScreen::OnDyeItemResults(const DyeItemMessage & tDyeItemResults)  // line 1109
{
    if (GetMysticMode() == MYSTIC_MODE_DYE_ITEM)
    {
        m_tDyeItemView.OnDyeItemResults(tDyeItemResults);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void OnMysticUpdate()
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->Update();
    }
}

void OnMysticLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnMysticRightStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->HandleRightStickInput(nControllerIndex);
    }
}

void OnMysticButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}
BOOL UIMysticCanCloseSafely()
{
    if (sgptMysticGlobals != NULL)
    {
        return sgptMysticGlobals->CanCloseSafely();
    }
    return TRUE;
}

inline BOOL PlayerMysticScreen::CanCloseSafely()  // line 2742
{
    switch (GetMysticMode())
    {
    case MYSTIC_MODE_ENCHANT:
        if (!m_tEnchantView.IsChoosingAffix() && !m_tEnchantView.IsChoosingReplacementAffix())
            return TRUE;
        return FALSE;
    default:
        return TRUE;
    }
}

}  // namespace Console
}  // namespace UIInventory

namespace UIMystic {
namespace Console {
using namespace UIInventory::Console;

void InitializeForGame()
{
    if (!GameIsStartUp())
    {
        sgptMysticGlobals = new PlayerMysticScreen();
    }
}

void DisposeForGame()
{
    if (!GameIsStartUp())
    {
        ::delete sgptMysticGlobals;
        sgptMysticGlobals = NULL;
    }
}

void OpenForController(int32 nControllerIndex)
{
    UIFastEquip::Console::CloseForPlayer(PlayerGetByControllerIndex(nControllerIndex)->nPlayerIndex);
    sgptMysticGlobals->OpenForController(nControllerIndex);
}

void Close()
{
    if (sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->Close();
    }
}

void OpenEnchantScreen(int32 nControllerIndex)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->SetMysticMode(PlayerMysticScreen::MYSTIC_MODE_ENCHANT);
    }
}

void OpenTransmogrifyScreen(int32 nControllerIndex)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->SetMysticMode(PlayerMysticScreen::MYSTIC_MODE_TRANSMOGRIFY);
    }
}

void OpenDyeItemScreen(int32 nControllerIndex)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->SetMysticMode(PlayerMysticScreen::MYSTIC_MODE_DYE_ITEM);
    }
}

void OpenTrainScreen(int32 nControllerIndex)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->SetMysticMode(PlayerMysticScreen::MYSTIC_MODE_TRAIN);
    }
}

void OnTrainingResults(const IntDataMessage * pMsg)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->OnTrainingResults(pMsg);
    }
}

void OnMysticChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)
{
    if (sgptMysticGlobals == NULL || !sgptMysticGlobals->IsActive())
    {
        return;
    }
    if (!bForce && tOldLoc.eActorLocation == tNewLoc.eActorLocation && tNewLoc.tInvLocation.eSlot == tOldLoc.tInvLocation.eSlot)
    {
        return;
    }
    Player * ptPlayer = sgptMysticGlobals->GetPlayer();
    if (ptPlayer == NULL || tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD || tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
    {
        sgptMysticGlobals->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}

void OnEnchantAffixChoose(const EnchantAffixChooseMessage * ptMessage)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->OnEnchantAffixChoose(ptMessage);
    }
}

void OnEnchantAffixResults(const ANNDataMessage * ptMessage)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->OnEnchantAffixResults(ptMessage);
    }
}

void OnTransmogResults(const TransmogItemMessage & tTransmogResults)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->OnTransmogResults(tTransmogResults);
    }
}

void OnUnlockTransmogResults(const GBIDDataMessage * ptMessage)
{
    if (sgptMysticGlobals != NULL)
    {
        sgptMysticGlobals->OnUnlockTransmogResults(ptMessage);
    }
}

void OnDyeItemResults(const DyeItemMessage & tDyeItemResults)
{
    if (sgptMysticGlobals != NULL && sgptMysticGlobals->IsActive())
    {
        sgptMysticGlobals->OnDyeItemResults(tDyeItemResults);
    }
}

}  // namespace Console
}  // namespace UIMystic

namespace {

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

void EnableTabNavigation(const BOOL bEnable)
{
    ::Console::Screens::EnableScreen(::Console::Screens::MYSTIC_SCREEN_SET, ::Console::Screens::MYSTIC_ENCHANT_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::MYSTIC_SCREEN_SET, ::Console::Screens::MYSTIC_TRANSMOGRIFY_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::MYSTIC_SCREEN_SET, ::Console::Screens::MYSTIC_DYE_ITEM_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::MYSTIC_SCREEN_SET, ::Console::Screens::MYSTIC_TRAIN_SCREEN, bEnable);

    UIID idNavBar = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE, ".LayoutRoot.Navigation.Mystic");
    static const LPCSTR arTabs[] = { ".EnchantTab", ".TransmogrifyTab", ".DyeItemTab", ".TrainTab" };
    for (int32 i = 0; i < 4; ++i)
    {
        UIID idTab = UIIDCreateFromParent(idNavBar, arTabs[i]);
        UI_GetWindowFromID(UIIDCreateFromParent(idTab, ".KeyBinding1")).SetActive(bEnable);
        UI_GetWindowFromID(UIIDCreateFromParent(idTab, ".KeyBinding2")).SetActive(bEnable);
    }
}

void HideRingNewItemIndicators()
{
    UIID idKey;
    const UIIDToFilterMap & tFilterMap = UIInventory::Console::sgptMysticGlobals->GetFilterMap();
    UIFilterType eFilterType;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
        UIID idNewEliteIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStarElite");

        UI_GetWindowFromID(idNewIndicator).SetActive(FALSE);
        UI_GetWindowFromID(idNewEliteIndicator).SetActive(FALSE);
    }
}

}  // namespace
