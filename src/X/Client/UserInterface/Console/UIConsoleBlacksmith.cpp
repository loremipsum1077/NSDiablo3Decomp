// Original file: X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp
// The console blacksmith screen: crafting, salvage, repair and training tabs.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleSalvage.h"
#include "UIConsoleTraining.h"
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
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/Crafting/Salvage.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UIBlinkerControl.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"
#include "ConsoleModalNotificationManager.h"
// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Crafting/ItemCrafting.h
struct CraftingResultsMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidItem;  // 0x0c
    ItemQualityLevel eIQL;  // 0x10
};
// X/Common/Crafting/ItemCrafting.cpp
BOOL ItemCrafting_ArtisanCanCraftItem(GBID gbidRecipe, CrafterType eType, const Player * ptPlayer);
BOOL ItemCrafting_CanTrain(Player & tPlayer);
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);
enum {
    CRAFTER_INVALID = -1,
    CRAFTER_BLACKSMITH = 0,
};
// X/Common/ActorCommonData.cpp
ACDID ACDFromANN(const ACDNetworkName ann);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Common/Networking/GameMessage.h

// DWARF UIConsoleRepair.cpp:51.
namespace UIRepair { namespace Console { void RepairAllItems(Player * ptPlayer, const BOOL bEquippedItemsOnly); } }

enum {
    CMSG_CLOSE_INTERACTION = 22,


};
// X/Client/Input/Input.cpp
void InputClearRemappings();
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
void UpdateTrainingUI(const CrafterType eCrafterType, const Player & tPlayer);
void UpdateRecipeRequirementsUI(const UIID & idParent, const GBID gbidRecipe, const Player & tPlayer);
}  // namespace Console
}  // namespace UIUtils
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/Console/UIConsoleFastEquip.cpp:1321
namespace UIFastEquip { namespace Console { void CloseForPlayer(const int32 nPlayerIndex); } }
// X/Client/UserInterface/Console/UIConsoleRepair.cpp:95
namespace UIRepair { namespace Console { void UpdateRepairUI(Player * ptPlayer); } }
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
enum InfoPanelType {
    REPAIR_SUMMARY_INFO_PANEL = 2,
    CRAFTING_SUMMARY_INFO_PANEL = 3,
    BLACKSMITH_SALVAGE_INFO_PANEL = 7,
    BLACKSMITH_SALVAGE_RESULTS_PANEL = 8,
};
void ClearStatusLine(UIInventoryView * ptView);
BOOL FilterHasNewItems(const UIFilterType eFilterType, const ACDID idPlayerACD, const NewItemIndicatorManager<int> & tNewItemsMgr);
void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight);
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
}  // namespace Console
}  // namespace UIInventory

// X/Client/UserInterface/UI.cpp
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_BAG, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_FEET, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_HANDS, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_HEAD, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_LEGS, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_NECK, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_TORSO, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_WAIST, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_FIST, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_BOW, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_SWORD, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_CROSSBOW, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_DAGGER, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_MACE, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_WAND, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_OFF_HAND, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_MIGHTY, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_STAFF, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_AXE, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_FLAIL, UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_SCYTHE;  // X/Client/UserInterface/UIIDs.h
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS,
    UIDLG_INVENTORY_CONSOLE_EQUIPPED, UIDLG_INVENTORY_CONSOLE_BLACKSMITH_CRAFTING,
    UIDLG_INVENTORY_CONSOLE_TRAINING, UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_LIST,
    UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_DETAILS, UIDLG_INVENTORY_CONSOLE_SALVAGE,
    UIDLG_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING, UIDLG_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING,
    UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL;
extern const UIID UIWND_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_CURSOR,
    UIWND_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_CURSOR, UIWND_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_GLOW,
    UIWND_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_GLOW, UIWND_INVENTORY_CONSOLE_RING,
    UIWND_INVENTORY_CONSOLE_RING_CURSOR, UIWND_INVENTORY_CONSOLE_ROOT,
    UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER; extern const UIID UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED;  // DWARF: HEADER at UIGlobals.cpp:3144; no header declaration recorded.

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_SCROLL_LIST = 4,
    ACTION_CREATE = 5,
    ACTION_ACCEPT_CRAFTING = 6,
    ACTION_SELECT_ITEM_TO_SALVAGE = 7,
    ACTION_CANCEL_SALVAGING = 8,
    ACTION_SALVAGE = 9,
    ACTION_ACCEPT_SALVAGE = 10,
    ACTION_REPAIR = 11,
    ACTION_REPAIR_EQUIPPED = 12,
    ACTION_REPAIR_ALL = 13,
    ACTION_TRAIN = 14,
    ACTION_SALVAGE_ALL_JUNK = 15,
    ACTION_MASS_SALVAGE = 16,
    ACTION_CLOSE = 17,
    ACTION_DETAILS = 18,
    ACTION_LIST = 19,
    ACTION_FILTER_ALL = 20,
    ACTION_FILTER_CLASS = 21,
    ACTION_FILTER_MATERIALS = 22,
    ACTION_CREATE_ANOTHER = 23,
    ACTION_EQUIP = 24,
    ACTION_CANCEL_CRAFTING = 25,
    ACTION_FINISH_CRAFTING = 26,
    ACTION_TOGGLE_STAT_RANGES = 27,
};

class PlayerBlacksmithScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x1648
public:
    enum BlacksmithMode {
        BLACKSMITH_MODE_INVALID = -1,
        BLACKSMITH_MODE_WEAPONS = 0,
        BLACKSMITH_MODE_ARMOR = 1,
        BLACKSMITH_MODE_SALVAGE = 2,
        BLACKSMITH_MODE_REPAIR = 3,
        BLACKSMITH_MODE_TRAIN = 4,
    };
    enum RecipeListFilter {
        RECIPE_FILTER_INVALID = -1,
        RECIPE_FILTER_ALL = 0,
        RECIPE_FILTER_CLASS = 1,
        RECIPE_FILTER_MATERIALS = 2,
    };

    virtual ~PlayerBlacksmithScreen() {}  // line 117

    BlacksmithMode GetBlacksmithMode() const  // line 142
    {
        return m_eBlacksmithMode;
    }

    PlayerBlacksmithScreen();
    void SetBlacksmithMode(const BlacksmithMode eBlacksmithMode);
    void SetRecipeListFilter(const RecipeListFilter eRecipeFilter);
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void ViewCraftingResults();
    void CraftItemAgain();
    void CraftSelectedRecipe();
    void OnCraftingResults(const CraftingResultsMessage * pMsg);
    void SalvageItem();
    void SalvageAllJunk();
    void MassSalvage();
    void BeginSalvage();
    void EndSalvage();
    void OnSalvageResults(const SalvageResultsMessage * pMsg);
    void OnTrainingResults(const IntDataMessage * pMsg);
    void OnRepair();
    void RepairItem();
    void RepairAllItems(const BOOL bEquippedOnly);
    void Train();
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
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
private:
    static const float CAST_TIME;
    static const uint32 CRAFTING_DURATION;
    enum CraftingState {
        CRAFTING_STATE_START = 0,
        CRAFTING_STATE_SHOW_PROGRESS = 1,
        CRAFTING_STATE_WAITING_FOR_REPLY = 2,
        CRAFTING_STATE_WAITING_FOR_RESULT = 3,
        CRAFTING_STATE_DONE = 4,
        CRAFTING_STATE_SHOW_RESULTS = 5,
    };
    enum SalvageState {
        SALVAGE_STATE_START = 0,
        SALVAGE_STATE_WAITING_TO_CONFIRM = 1,
        SALVAGE_STATE_SALVAGING = 2,
        SALVAGE_STATE_RESULTS = 3,
    };
    BlacksmithMode m_eBlacksmithMode;  // 0x1f0
    CraftingState m_eCraftingState;  // 0x1f4
    SalvageState m_eSalvageState;  // 0x1f8
    uint32 m_dwCraftingEndTime;  // 0x1fc
    ACDNetworkName m_annCraftingResult;  // 0x200
    RecipeListFilter m_eRecipeFilter;  // 0x204
    InventoryItems m_tPlayerItemsProvider;  // 0x208
    CrafterRecipes m_tRecipeItemsProvider;  // 0xa00
    TrainingItems m_tTrainingItemsProvider;  // 0xfe0
    SingleItemProvider m_tSingleItemProvider;  // 0x1030
    EquippedItem m_tEquippedItemProvider;  // 0x1068
    CraftingItem m_tCrafingItemProvider;  // 0x10a0
    UIInventoryListView m_tPlayerListView;  // 0x10e8
    UIInventoryDetailsView m_tPlayerDetailsView;  // 0x1170
    UIInventoryListView m_tRecipesListView;  // 0x11e8
    UIInventoryDetailsView m_tRecipesDetailsView;  // 0x1270
    UIInventoryDetailsView m_tCraftingView;  // 0x12e8
    UIInventoryDetailsView m_tEquippedItemView;  // 0x1360
    UIInventoryTrainingView m_tTrainingView;  // 0x13d8
    UIInventoryListView m_tTrainingListView;  // 0x1438
    UIInventoryDetailsView m_tTrainingDetailsView;  // 0x14c0
    UIInventoryDetailsView m_tSelectedDetailsView;  // 0x1538
    UIInventorySalvageView m_tSalvageView;  // 0x15b0
    ACDID m_idLastSelectedItem;  // 0x1640
    ACDID m_idCraftedItem;  // 0x1644
    void OnButton1();
    void OnButton2();
    void OnButton3();
    void OnButton4();
    void OnLeftTrigger();
    void OnRightTrigger();
    void OnRightStickButton();
    void OnDPadLeft();
    BOOL CanClose();
    GBID GetSelectedRecipe();
    BOOL ArtisanCanCraftSelectedRecipe();
    void SelectItemToSalvage();
    void SetSalvageState(const SalvageState eSalvageState);
    void RefreshProviders(const MaintainSelectionOptions eSelectionType);
    void UpdateNavBar();
    void UpdateCraftingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateSalvageActions(::Console::ControllerActionManager & tActionManager);
    BOOL HasSalvageableJunk() const;
    BOOL CanMassSalvage() const;
    void UpdateRepairActions(::Console::ControllerActionManager & tActionManager);
    void UpdateTrainingActions(::Console::ControllerActionManager & tActionManager);
    void CraftItem(const GBID gbidRecipe, const ACDID idItem);
    void CancelCrafting();
    void HideCraftingProgress();
    void ShowCraftingUI();
    void UpdateCraftingProgress();
    void UpdateProgressBar();
    void UpdateCraftingUI();
    void ShowSalvageUI();
    void UpdateSalvageUI();
    void ShowSalvageResultsUI();
    void ShowRepairUI();
    void UpdateRepairUI();
    void SyncViews();
    void ShowTrainingUI();
    void UpdateTrainingUI();
    void OnNotificationDismissed(const UIID &);
    void ToggleStatRangeDisplay();
    void ShowError(CRefString sErrorMsg);
};
static_assert(sizeof(PlayerBlacksmithScreen) == 0x1648, "PlayerBlacksmithScreen");

PlayerBlacksmithScreen * sgptBlacksmithGlobals;  // line 425

const uint32 PlayerBlacksmithScreen::CRAFTING_DURATION = 150;
static const float SCROLLBOX_SCROLLDELTA = 4.0f;

}  // namespace Console
}  // namespace UIInventory

namespace {
BOOL RingIncludeControlPredicate(const UIID & idControl);
void EnableTabNavigation(const BOOL bEnable);
}  // namespace

namespace UIInventory {
namespace Console {

PlayerBlacksmithScreen::PlayerBlacksmithScreen()
    : m_eBlacksmithMode(BLACKSMITH_MODE_INVALID)
    , m_eCraftingState(CRAFTING_STATE_START)
    , m_eSalvageState(SALVAGE_STATE_START)
    , m_dwCraftingEndTime(0)
    , m_annCraftingResult(ANN_INVALID)
    , m_eRecipeFilter(RECIPE_FILTER_INVALID)
    , m_tPlayerItemsProvider(InventoryItems::PROVIDERMODE_BACKPACK)
    , m_tPlayerListView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPlayerDetailsView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, TRUE)
    , m_tRecipesListView(m_tRecipeItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tRecipesDetailsView(m_tRecipeItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, TRUE)
    , m_tCraftingView(m_tCrafingItemProvider, UIDLG_INVENTORY_CONSOLE_BLACKSMITH_CRAFTING, FALSE)
    , m_tEquippedItemView(m_tEquippedItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, FALSE)
    , m_tTrainingView(UIDLG_INVENTORY_CONSOLE_TRAINING)
    , m_tTrainingListView(m_tTrainingItemsProvider, UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_LIST, TRUE)
    , m_tTrainingDetailsView(m_tTrainingItemsProvider, UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_DETAILS, FALSE)
    , m_tSelectedDetailsView(m_tSingleItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, FALSE)
    , m_tSalvageView(m_tSingleItemProvider, UIDLG_INVENTORY_CONSOLE_SALVAGE)
{
    m_tRecipeItemsProvider.SetFilter(FILTER_BLACKSMITH_RECIPES_SWORD);
    m_idLastSelectedItem = ACDID_INVALID;
    m_idCraftedItem = ACDID_INVALID;
    m_tRecipesListView.SetHeaderText(StringListFetch("ConsoleCrafting:Recipes"));
    m_tRecipesDetailsView.SetHeaderText(StringListFetch("ConsoleCrafting:Recipes"));
    m_tCraftingView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tEquippedItemView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_tSalvageView.SetHeaderText(StringListFetch("ConsoleCrafting:ItemToSalvage"));
}

void PlayerBlacksmithScreen::SetRecipeListFilter(const RecipeListFilter eRecipeFilter)
{
    if (m_eRecipeFilter == eRecipeFilter)
    {
        return;
    }

    CRefString sTitle;
    m_eRecipeFilter = eRecipeFilter;
    switch (eRecipeFilter)
    {
    case RECIPE_FILTER_ALL:
        m_tRecipeItemsProvider.SetRecipeOptions(0);
        sTitle = StringListFetch("ConsoleArtisan:FilterAllTitle");
        break;
    case RECIPE_FILTER_CLASS:
        m_tRecipeItemsProvider.SetRecipeOptions(1 << 0);
        sTitle = StringListFetch("ConsoleArtisan:FilterClassTitle");
        break;
    case RECIPE_FILTER_MATERIALS:
        m_tRecipeItemsProvider.SetRecipeOptions(1 << 1);
        sTitle = StringListFetch("ConsoleArtisan:FilterMaterialsTitle");
        break;
    }

    m_tRecipesListView.SetHeaderText(sTitle);
    m_tRecipesDetailsView.SetHeaderText(sTitle);
    if (GetPrimaryView() != NULL)
    {
        GetPrimaryView()->RequestUpdate(RESET_SELECTION);
    }
    UpdateRingIcons();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::SyncViews()
{
    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (idSelectedItem != m_idLastSelectedItem)
    {
        m_tSingleItemProvider.SetItem(idSelectedItem);
        GetSecondaryView()->RequestUpdate(RESET_SELECTION);
        m_idLastSelectedItem = idSelectedItem;
    }
}

void PlayerBlacksmithScreen::CancelCrafting()
{
    ACDID idSelectedRecipe = m_tCrafingItemProvider.GetRecipeItem();
    EnableTabNavigation(TRUE);

    UIID idCrafting = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.Artisan");
    UI_GetWindowFromID(idCrafting).SetActive(FALSE);
    UIID idAffixes = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UI_GetWindowFromID(idAffixes).SetActive(TRUE);
    UIID idCraftingStoryboard = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".CraftItem");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idCraftingStoryboard);
    tStoryboard.Reset();

    m_tCraftingView.RequestUpdate(RESET_SELECTION);
    m_eCraftingState = CRAFTING_STATE_START;
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    GetPrimaryView()->SelectItem(idSelectedRecipe);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::ShowSalvageUI()
{
    SetSalvageState(SALVAGE_STATE_START);
    SwitchToInfoPanel(BLACKSMITH_SALVAGE_INFO_PANEL);
    m_tSelectedDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    OpenSelectedSlot();
}

void PlayerBlacksmithScreen::ShowRepairUI()
{
    SwitchToInfoPanel(REPAIR_SUMMARY_INFO_PANEL);
    m_tSelectedDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleInventory:Damaged"));
    m_tPlayerDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    OpenSelectedSlot();
    UpdateRepairUI();
}

void PlayerBlacksmithScreen::UpdateRepairUI()
{
    UIRepair::Console::UpdateRepairUI(GetPlayer());
    CRefString sTitle = StringListFetch("ConsoleShop:Repair");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, 0);
}

void PlayerBlacksmithScreen::ShowTrainingUI()
{
    SwitchToInfoPanel(CRAFTING_SUMMARY_INFO_PANEL);
    m_tTrainingView.SetHeaderText(StringListFetch("ConsoleCrafting:ArtisanRank"));
    m_tTrainingListView.SetHeaderText(StringListFetch("ConsoleCrafting:Rewards"));
    m_tTrainingDetailsView.SetHeaderText(StringListFetch("ConsoleCrafting:Rewards"));
    OpenSelectedSlot();
    UpdateTrainingUI();
}

void PlayerBlacksmithScreen::OnCraftingResults(const CraftingResultsMessage * pMsg)
{
    if (pMsg->annItem != ANN_INVALID)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_ITEM_COMPLETE), cgtPSODefaults);
        m_annCraftingResult = pMsg->annItem;
        m_eCraftingState = CRAFTING_STATE_WAITING_FOR_RESULT;
    }
}

void PlayerBlacksmithScreen::OnSalvageResults(const SalvageResultsMessage * pMsg)
{
    BlacksmithMode eMode = GetBlacksmithMode();
    if (eMode == BLACKSMITH_MODE_SALVAGE)
    {
        SalvageState eSalvageState = m_eSalvageState;
        RequestUpdate(KEEP_INDEX_SELECTED);
        if (eSalvageState != SALVAGE_STATE_START)
        {
            m_tSalvageView.SetSalvageResultsFromMessage(*pMsg);
            SetSalvageState(SALVAGE_STATE_RESULTS);
        }
    }
    else if (eMode == BLACKSMITH_MODE_WEAPONS || eMode == BLACKSMITH_MODE_ARMOR)
    {
        CancelCrafting();
        SetBrowsingMode(BROWSE_LIST);
        SetSalvageState(SALVAGE_STATE_START);
    }
}

void PlayerBlacksmithScreen::OnTrainingResults(const IntDataMessage * pMsg)
{
    m_tTrainingItemsProvider.Refresh();
    m_tRecipeItemsProvider.Refresh();
    m_tTrainingListView.ForceUpdate(RESET_SELECTION);
    m_tTrainingDetailsView.ForceUpdate(RESET_SELECTION);
    UpdateRingNewItemIndicators();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::OnRepair()
{
    m_tPlayerItemsProvider.Refresh();
    RequestUpdate(KEEP_INDEX_SELECTED);
}

BOOL PlayerBlacksmithScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerBlacksmithScreen::OnRequestedUpdate()
{
    m_tPlayerItemsProvider.Refresh();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    if (m_eBlacksmithMode == BLACKSMITH_MODE_WEAPONS)
    {
        tRingWrapper.WrapUsingContainerControl(UIDLG_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING, UIWND_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_CURSOR, RingIncludeControlPredicate, NULL);
    }
    else
    {
        tRingWrapper.WrapUsingContainerControl(UIDLG_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING, UIWND_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_CURSOR, RingIncludeControlPredicate, NULL);
    }
}

void PlayerBlacksmithScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    switch (GetBlacksmithMode())
    {
    case BLACKSMITH_MODE_WEAPONS:
    case BLACKSMITH_MODE_ARMOR:
        m_tRecipeItemsProvider.SetFilter(eNewFilterType);
        m_tEquippedItemProvider.SetFilter(eNewFilterType);
        break;
    case BLACKSMITH_MODE_SALVAGE:
    case BLACKSMITH_MODE_REPAIR:
        m_tPlayerItemsProvider.SetFilter(eNewFilterType);
        m_tSingleItemProvider.SetFilter(eNewFilterType);
        break;
    case BLACKSMITH_MODE_TRAIN:
        m_tTrainingItemsProvider.SetFilter(eNewFilterType);
        break;
    default:
        m_tPlayerItemsProvider.SetFilter(eNewFilterType);
        break;
    }

    m_tPlayerItemsProvider.Refresh();
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
    UpdateRingNewItemIndicators();
}

void PlayerBlacksmithScreen::InitRingFilters(UIIDToFilterMap & tFilterMap)
{
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_BAG, FILTER_BLACKSMITH_RECIPES_OTHER);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_BRACERS, FILTER_BLACKSMITH_RECIPES_WRIST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_FEET, FILTER_BLACKSMITH_RECIPES_FEET);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_FINGER_LEFT, FILTER_BLACKSMITH_RECIPES_RING_LEFT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_FINGER_RIGHT, FILTER_BLACKSMITH_RECIPES_RING_RIGHT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_HANDS, FILTER_BLACKSMITH_RECIPES_HANDS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_HEAD, FILTER_BLACKSMITH_RECIPES_HEAD);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_LEGS, FILTER_BLACKSMITH_RECIPES_LEGS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_NECK, FILTER_BLACKSMITH_RECIPES_NECK);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_SHOULDER, FILTER_BLACKSMITH_RECIPES_SHOULDERS);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_TORSO, FILTER_BLACKSMITH_RECIPES_CHEST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_WAIST, FILTER_BLACKSMITH_RECIPES_BELT);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_FIST, FILTER_BLACKSMITH_RECIPES_FIST);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_BOW, FILTER_BLACKSMITH_RECIPES_BOW);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_SWORD, FILTER_BLACKSMITH_RECIPES_SWORD);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_CROSSBOW, FILTER_BLACKSMITH_RECIPES_CROSSBOW);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_DAGGER, FILTER_BLACKSMITH_RECIPES_DAGGER);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_MACE, FILTER_BLACKSMITH_RECIPES_MACE);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_WAND, FILTER_BLACKSMITH_RECIPES_WAND);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_OFF_HAND, FILTER_BLACKSMITH_RECIPES_OFF_HAND);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_MIGHTY, FILTER_BLACKSMITH_RECIPES_MIGHTY);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_STAFF, FILTER_BLACKSMITH_RECIPES_STAFF);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_AXE, FILTER_BLACKSMITH_RECIPES_AXE);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_FLAIL, FILTER_BLACKSMITH_RECIPES_FLAIL);
    tFilterMap.SetAt_Tracked(UIBTN_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_SCYTHE, FILTER_BLACKSMITH_RECIPES_SCYTHE);
}

UIInventoryView * PlayerBlacksmithScreen::GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
{
    BlacksmithMode eBlacksmithMode = GetBlacksmithMode();
    if (eBlacksmithMode == BLACKSMITH_MODE_WEAPONS || eBlacksmithMode == BLACKSMITH_MODE_ARMOR)
    {
        if (m_eCraftingState != CRAFTING_STATE_START)
        {
            return &m_tCraftingView;
        }
        if (eBrowsingMode == BROWSE_LIST)
        {
            return &m_tRecipesListView;
        }
        return &m_tRecipesDetailsView;
    }
    if (eBlacksmithMode == BLACKSMITH_MODE_INVALID)
    {
        return NULL;
    }
    if (eBlacksmithMode == BLACKSMITH_MODE_TRAIN)
    {
        if (eBrowsingMode == BROWSE_LIST)
        {
            return &m_tTrainingListView;
        }
        return &m_tTrainingDetailsView;
    }
    switch (eBrowsingMode)
    {
    case BROWSE_LIST:
        return &m_tPlayerListView;
    case BROWSE_DETAILS:
        return &m_tPlayerDetailsView;
    }
    return NULL;
}

UIInventoryView * PlayerBlacksmithScreen::GetSecondaryViewForMode(const BrowsingMode)
{
    switch (GetBlacksmithMode())
    {
    case BLACKSMITH_MODE_WEAPONS:
    case BLACKSMITH_MODE_ARMOR:
        if (m_eCraftingState == CRAFTING_STATE_START)
        {
            return &m_tEquippedItemView;
        }
        return &m_tCraftingView;
    case BLACKSMITH_MODE_SALVAGE:
        return &m_tSalvageView;
    case BLACKSMITH_MODE_REPAIR:
        return &m_tSelectedDetailsView;
    case BLACKSMITH_MODE_TRAIN:
        return &m_tTrainingView;
    }
    return NULL;
}

BOOL PlayerBlacksmithScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    switch (eFilterType)
    {
    case FILTER_MAIN_HAND:
    case FILTER_OFF_HAND:
    case FILTER_BAG:
        return FALSE;
    }

    switch (GetBlacksmithMode())
    {
    case BLACKSMITH_MODE_WEAPONS:
    case BLACKSMITH_MODE_ARMOR:
        return m_tRecipeItemsProvider.GetItemsWithFilter(eFilterType).size() != 0;
    }
    return TRUE;
}

void PlayerBlacksmithScreen::SetBlacksmithMode(const BlacksmithMode eBlacksmithMode)
{
    if (m_eBlacksmithMode == eBlacksmithMode)
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
    if (m_eBlacksmithMode == BLACKSMITH_MODE_WEAPONS || m_eBlacksmithMode == BLACKSMITH_MODE_ARMOR)
    {
        CancelCrafting();
    }

    m_eBlacksmithMode = eBlacksmithMode;
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    CloseSelectedSlot();
    SetRecipeListFilter(RECIPE_FILTER_ALL);

    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(FALSE);
    UI_GetWindowFromID(UIDLG_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING).SetActive(m_eBlacksmithMode == BLACKSMITH_MODE_WEAPONS);
    UI_GetWindowFromID(UIDLG_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING).SetActive(m_eBlacksmithMode == BLACKSMITH_MODE_ARMOR);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(m_eBlacksmithMode == BLACKSMITH_MODE_WEAPONS || m_eBlacksmithMode == BLACKSMITH_MODE_ARMOR);
    ResetRing();
    UpdateRingIcons();
    UpdateRingNewItemIndicators();
    ::Console::UIRActorManager::SetActive(m_eBlacksmithMode != BLACKSMITH_MODE_SALVAGE && m_eBlacksmithMode != BLACKSMITH_MODE_REPAIR);

    switch (m_eBlacksmithMode)
    {
    case BLACKSMITH_MODE_WEAPONS:
    case BLACKSMITH_MODE_ARMOR:
        SetFilterType(GetDefaultRingSelection());
        ShowCraftingUI();
        break;
    case BLACKSMITH_MODE_SALVAGE:
        m_tPlayerItemsProvider.SetItemOptions(1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS);
        SetFilterType(FILTER_SALVAGEABLE);
        ShowSalvageUI();
        break;
    case BLACKSMITH_MODE_REPAIR:
        m_tPlayerItemsProvider.SetItemOptions(1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS);
        SetFilterType(FILTER_DAMAGED);
        ShowRepairUI();
        break;
    case BLACKSMITH_MODE_TRAIN:
        SetFilterType(FILTER_NONE);
        ShowTrainingUI();
        break;
    default:
        SetFilterType(FILTER_NONE);
        break;
    }

    UpdateNavBar();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::ShowCraftingUI()
{
    SwitchToInfoPanel(CRAFTING_SUMMARY_INFO_PANEL);
    m_tSelectedDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    UpdateCraftingUI();
}

void PlayerBlacksmithScreen::UpdateCraftingProgress()
{
    if (m_eCraftingState == CRAFTING_STATE_SHOW_PROGRESS)
    {
        UpdateProgressBar();
        uint32 dwNow = CGameGetClientTicks();
        if (dwNow >= m_dwCraftingEndTime)
        {
            m_eCraftingState = CRAFTING_STATE_WAITING_FOR_REPLY;
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(idSelectedItem);
            if (nIndex != -1)
            {
                GBID gbidRecipe = m_tRecipeItemsProvider.GetRecipes()[nIndex];
                GBIDDataMessage tMessage;
                tMessage.gbidData = gbidRecipe;
                MessageSendToServer(CMSG_TRY_CRAFT_ITEM, &tMessage, sizeof(tMessage), GetPlayer());
            }
        }
    }
    else if (m_eCraftingState == CRAFTING_STATE_WAITING_FOR_RESULT)
    {
        if (m_annCraftingResult == ANN_INVALID)
        {
            CancelCrafting();
            return;
        }
        ACDID idCraftedItem = ACDFromANN(m_annCraftingResult);
        if (idCraftedItem != ACDID_INVALID)
        {
            m_idCraftedItem = idCraftedItem;
            m_eCraftingState = CRAFTING_STATE_DONE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }
}

void PlayerBlacksmithScreen::UpdateProgressBar()
{
    float flProgress = 1.0f;
    uint32 dwNow = CGameGetClientTicks();
    if (dwNow < m_dwCraftingEndTime)
    {
        uint32 dwRemaining = Math::Min(m_dwCraftingEndTime - dwNow, CRAFTING_DURATION);
        flProgress = 1.0f - (float)dwRemaining / (float)CRAFTING_DURATION;
    }

    UIID idCraftingView = GetPrimaryView()->GetRootUIID();
    UIID idProgress = UIIDCreateFromParent(idCraftingView, ".LayoutRoot.ItemDetails.Item.Artisan.Blacksmith.ProgressBar");
    UI_GetProgressBarFromID(idProgress).SetExtentsAndPosition(0.0f, 1.0f, flProgress, -1.0f);
    BOOL bFinished = Math::Abs(flProgress - 1.0f) <= 1e-06f;
    CRefString sText = bFinished ? StringListFetch("ConsoleArtisan:Complete") : StringListFetch("ConsoleArtisan:Creating");
    UIID idStatus = UIIDCreateFromParent(idCraftingView, ".LayoutRoot.ItemDetails.Item.Artisan.Blacksmith.ProgressStatus");
    UI_GetTextFromID(idStatus).SetText(sText.str(), 0);
    UIID idGlow = UIIDCreateFromParent(idCraftingView, ".LayoutRoot.ItemDetails.Item.Artisan.Blacksmith.Glow");
    UI_GetWindowFromID(idGlow).SetActive(bFinished);
    if (bFinished)
    {
        UIID idCraftingStoryboard = UIIDCreateFromParent(idCraftingView, ".CraftItem");
        UI_GetStoryboardFromID(idCraftingStoryboard).Play(FALSE, 0.0f);
    }
}

void PlayerBlacksmithScreen::UpdateCraftingUI()
{
    ShowFilterName(UITXT_INVENTORY_CONSOLE_HEADER, GetFilter(), !RingHasFocus());
    if (!UI_GetWindowFromID(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL).IsItselfActive())
    {
        return;
    }

    GBID gbidRecipe = GBID_NONE;
    if (!RingHasFocus())
    {
        if (m_eCraftingState != CRAFTING_STATE_START)
        {
            gbidRecipe = m_tCrafingItemProvider.GetRecipe();
        }
        else
        {
            int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(GetPrimaryView()->GetSelectedItem());
            if (nIndex != -1)
            {
                gbidRecipe = m_tRecipeItemsProvider.GetRecipes()[nIndex];
            }
        }
    }
    UIUtils::Console::UpdateRecipeRequirementsUI(UIDLG_INVENTORY_CONSOLE_CRAFTING_PANEL, gbidRecipe, *GetPlayer());
}

void PlayerBlacksmithScreen::SetSalvageState(const SalvageState eSalvageState)
{
    if (m_eSalvageState == eSalvageState)
    {
        return;
    }

    m_eSalvageState = eSalvageState;
    if (m_eBlacksmithMode != BLACKSMITH_MODE_SALVAGE)
    {
        return;
    }

    SwitchToInfoPanel(BLACKSMITH_SALVAGE_INFO_PANEL);
    switch (m_eSalvageState)
    {
    case SALVAGE_STATE_START:
        GetPrimaryView()->Enable(TRUE);
        GetSecondaryView()->Enable(TRUE);
        SetFocusedView(GetPrimaryView());
        m_tSalvageView.Reset();
        break;
    case SALVAGE_STATE_WAITING_TO_CONFIRM:
        GetPrimaryView()->Enable(FALSE);
        GetSecondaryView()->Enable(TRUE);
        SetFocusedView(NULL);
        m_tSalvageView.WaitForConfirmation();
        break;
    case SALVAGE_STATE_SALVAGING:
        GetPrimaryView()->Enable(FALSE);
        GetSecondaryView()->Enable(FALSE);
        break;
    case SALVAGE_STATE_RESULTS:
        GetPrimaryView()->Enable(FALSE);
        GetSecondaryView()->Enable(FALSE);
        break;
    }
    ::Console::UIRActorManager::SetActive(FALSE);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::UpdateTrainingUI()
{
    if (GetPlayer() != NULL)
    {
        UIUtils::Console::UpdateTrainingUI(CRAFTER_BLACKSMITH, *GetPlayer());
    }
    CRefString sTitle = StringListFetch("ConsoleCrafting:Train");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, 0);
}

void PlayerBlacksmithScreen::CraftItem(const GBID gbidRecipe, const ACDID idItem)
{
    m_tCrafingItemProvider.OnCraftingStarted(gbidRecipe, idItem);
    SetPrimaryView(&m_tCraftingView);
    m_tCraftingView.RequestUpdate(RESET_SELECTION);
    m_eCraftingState = CRAFTING_STATE_SHOW_PROGRESS;
    m_dwCraftingEndTime = CGameGetClientTicks() + CRAFTING_DURATION;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    UIID idCrafting = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.Artisan");
    UI_GetWindowFromID(idCrafting).SetActive(TRUE);
    UIID idAffixes = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UI_GetWindowFromID(idAffixes).SetActive(FALSE);
    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(UIIDCreateFromParent(idCrafting, ".Blacksmith.ProgressBar"));
    tProgressBar.SetExtentsAndPosition(0.0f, 1.0f, 0.0f, -1.0f);
}

void PlayerBlacksmithScreen::ViewCraftingResults()
{
    m_eCraftingState = CRAFTING_STATE_SHOW_RESULTS;
    m_tCrafingItemProvider.OnCraftingFinished(m_idCraftedItem);
    HideCraftingProgress();
    m_tCraftingView.RequestUpdate(RESET_SELECTION);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBlacksmithScreen::HideCraftingProgress()
{
    UIID idCrafting = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.Artisan");
    UI_GetWindowFromID(idCrafting).SetActive(FALSE);
    UIID idAffixes = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UI_GetWindowFromID(idAffixes).SetActive(TRUE);
    UIID idCraftingStoryboard = UIIDCreateFromParent(m_tCraftingView.GetRootUIID(), ".CraftItem");
    UI_GetStoryboardFromID(idCraftingStoryboard).Reset();
}

void PlayerBlacksmithScreen::BeginSalvage()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    ACDID idACDItem = ACDID_INVALID;
    BlacksmithMode eMode = GetBlacksmithMode();
    if (eMode == BLACKSMITH_MODE_SALVAGE)
    {
        if (m_eSalvageState == SALVAGE_STATE_WAITING_TO_CONFIRM)
        {
            UIInventoryView * ptView = GetPrimaryView();
            if (ptView != NULL)
            {
                idACDItem = ptView->GetSelectedItem();
            }
        }
    }
    else if (eMode == BLACKSMITH_MODE_WEAPONS || eMode == BLACKSMITH_MODE_ARMOR)
    {
        if (m_eCraftingState == CRAFTING_STATE_SHOW_RESULTS)
        {
            idACDItem = m_tCrafingItemProvider.GetCraftedItem();
        }
    }

    if (idACDItem != ACDID_INVALID)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SALVAGE_OPERATE), cgtPSODefaults);
        SetSalvageState(SALVAGE_STATE_SALVAGING);
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(idACDItem);
        MessageSendToServer(CMSG_TRY_SALVAGE, &tMessage, sizeof(tMessage), ptPlayer);
    }
}

void PlayerBlacksmithScreen::SalvageAllJunk()
{
    const ACDIDArray & aJunk = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_JUNK);
    if (aJunk.size() == 0)
    {
        return;
    }
    ActorCommonData * ptPlayerACD = ACDTryToGet(GetPlayer()->idPlayerACD);
    if (ptPlayerACD == NULL)
    {
        return;
    }

    BOOL bSalvagedJunk = FALSE;
    for (int32 n = (int32)aJunk.size() - 1; n >= 0; --n)
    {
        ACDID id = aJunk[n];
        if (id != 0)
        {
            ActorCommonData * ptItemACD = ACDTryToGet(id);
            if (ptItemACD != NULL && SalvageCommon::ItemCanSalvage(*ptItemACD, *ptPlayerACD) == GAMEERROR_NONE)
            {
                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(id);
                MessageSendToServer(CMSG_TRY_SALVAGE, &tMessage, sizeof(tMessage), GetPlayer());
                bSalvagedJunk = TRUE;
            }
        }
    }
    if (bSalvagedJunk)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SALVAGE_OPERATE), cgtPSODefaults);
    }
}

void PlayerBlacksmithScreen::MassSalvage()
{
    const ACDIDArray & aItems = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_MASS_SALVAGEABLE);
    if (aItems.size() == 0)
    {
        return;
    }
    ActorCommonData * ptPlayerACD = ACDTryToGet(GetPlayer()->idPlayerACD);
    if (ptPlayerACD == NULL)
    {
        return;
    }

    BOOL bSalvagedItems = FALSE;
    for (int32 n = (int32)aItems.size() - 1; n >= 0; --n)
    {
        ACDID id = aItems[n];
        if (id != 0)
        {
            ActorCommonData * ptItemACD = ACDTryToGet(id);
            if (ptItemACD != NULL && SalvageCommon::ItemCanSalvage(*ptItemACD, *ptPlayerACD) == GAMEERROR_NONE)
            {
                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(id);
                MessageSendToServer(CMSG_TRY_SALVAGE, &tMessage, sizeof(tMessage), GetPlayer());
                bSalvagedItems = TRUE;
            }
        }
    }
    if (bSalvagedItems)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SALVAGE_OPERATE), cgtPSODefaults);
    }
}

void PlayerBlacksmithScreen::ToggleStatRangeDisplay()
{
    m_tPlayerDetailsView.ToggleStatRangeDisplay();
    m_tEquippedItemView.ToggleStatRangeDisplay();
    m_tCraftingView.ToggleStatRangeDisplay();
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_TOGGLE_STATS), cgtPSODefaults);
}

void PlayerBlacksmithScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
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
    case BUTTON_4:
        OnButton4();
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
    case BUTTON_RIGHT_STICK_BUTTON:
        OnRightStickButton();
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

void PlayerBlacksmithScreen::OnLeftTrigger()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        !InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[GetControllerIndex()].tCurrentFrame.IsButtonDown(BUTTON_RIGHT_TRIGGER))
    {
        ScrollDetails(-SCROLLBOX_SCROLLDELTA);
    }
}

void PlayerBlacksmithScreen::OnRightTrigger()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        !InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[GetControllerIndex()].tCurrentFrame.IsButtonDown(BUTTON_LEFT_TRIGGER))
    {
        ScrollDetails(SCROLLBOX_SCROLLDELTA);
    }
}

void PlayerBlacksmithScreen::OnDPadLeft()
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_DPAD_LEFT) == ACTION_TOGGLE_STAT_RANGES)
    {
        ToggleStatRangeDisplay();
    }
}

void PlayerBlacksmithScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    InputClearRemappings();
    if ((m_eBlacksmithMode == BLACKSMITH_MODE_WEAPONS || m_eBlacksmithMode == BLACKSMITH_MODE_ARMOR) && m_eCraftingState == CRAFTING_STATE_START)
    {
        ::Console::ControllerActionId idAction;
        switch (m_eRecipeFilter)
        {
        case RECIPE_FILTER_ALL:
            idAction = ACTION_FILTER_CLASS;
            break;
        case RECIPE_FILTER_CLASS:
            idAction = ACTION_FILTER_MATERIALS;
            break;
        case RECIPE_FILTER_MATERIALS:
            idAction = ACTION_FILTER_ALL;
            break;
        default:
            goto skip;
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, idAction, StringListFetch("ConsoleArtisan:ChangeFilter").str());
    }
skip:
    switch (m_eBlacksmithMode)
    {
    case BLACKSMITH_MODE_WEAPONS:
    case BLACKSMITH_MODE_ARMOR:
        UpdateCraftingActions(tActionManager);
        break;
    case BLACKSMITH_MODE_SALVAGE:
        UpdateSalvageActions(tActionManager);
        break;
    case BLACKSMITH_MODE_REPAIR:
        UpdateRepairActions(tActionManager);
        break;
    case BLACKSMITH_MODE_TRAIN:
        UpdateTrainingActions(tActionManager);
        break;
    }
}

void PlayerBlacksmithScreen::OnClose()
{
    InputClearRemappings();
    ResetCurrencyAnimations(UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED);
    SetBlacksmithMode(BLACKSMITH_MODE_INVALID);
    SetRecipeListFilter(RECIPE_FILTER_INVALID);
    m_tPlayerItemsProvider.SetPlayer(NULL);
    m_tSingleItemProvider.SetPlayer(NULL);
    m_tTrainingItemsProvider.SetPlayer(NULL);
    m_tRecipeItemsProvider.SetPlayer(NULL);
    Player * ptPlayer = GetPlayer();
    ptPlayer->idACDActiveVendor = ACDID_INVALID;
    ptPlayer->eActiveCrafterType = CRAFTER_INVALID;
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_CLOSE_INTERACTION, &tMessage, sizeof(tMessage), ptPlayer);
}

void PlayerBlacksmithScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "Blacksmith");
        UpdateNavBar();
        m_tPlayerItemsProvider.SetItemOptions(1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS);
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

UIFilterType PlayerBlacksmithScreen::GetDefaultRingSelection() const
{
    UIFilterType eWeaponFilters[] = {
        FILTER_BLACKSMITH_RECIPES_SWORD, FILTER_BLACKSMITH_RECIPES_CROSSBOW, FILTER_BLACKSMITH_RECIPES_DAGGER,
        FILTER_BLACKSMITH_RECIPES_MACE, FILTER_BLACKSMITH_RECIPES_OFF_HAND, FILTER_BLACKSMITH_RECIPES_FLAIL,
        FILTER_BLACKSMITH_RECIPES_MIGHTY, FILTER_BLACKSMITH_RECIPES_STAFF, FILTER_BLACKSMITH_RECIPES_AXE,
        FILTER_BLACKSMITH_RECIPES_FIST, FILTER_BLACKSMITH_RECIPES_BOW, FILTER_BLACKSMITH_RECIPES_SCYTHE,
    };
    UIFilterType eArmorFilters[] = {
        FILTER_BLACKSMITH_RECIPES_CHEST, FILTER_BLACKSMITH_RECIPES_HEAD, FILTER_BLACKSMITH_RECIPES_NECK,
        FILTER_BLACKSMITH_RECIPES_WRIST, FILTER_BLACKSMITH_RECIPES_RING_LEFT, FILTER_BLACKSMITH_RECIPES_RING_RIGHT,
        FILTER_BLACKSMITH_RECIPES_OTHER, FILTER_BLACKSMITH_RECIPES_FEET, FILTER_BLACKSMITH_RECIPES_LEGS,
        FILTER_BLACKSMITH_RECIPES_BELT, FILTER_BLACKSMITH_RECIPES_HANDS, FILTER_BLACKSMITH_RECIPES_SHOULDERS,
    };
    int32 nLength = sizeof(eWeaponFilters) / sizeof(eWeaponFilters[0]);
    UIFilterType * pFilters = m_eBlacksmithMode == BLACKSMITH_MODE_WEAPONS ? eWeaponFilters : eArmorFilters;
    for (int32 i = 0; i < nLength; ++i)
    {
        if (m_tRecipeItemsProvider.GetItemsWithFilter(pFilters[i]).size() != 0)
        {
            return pFilters[i];
        }
    }
    return FILTER_NONE;
}

void PlayerBlacksmithScreen::UpdateRingNewItemIndicators()
{
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    const Player * ptPlayer = GetPlayer();
    NewItemIndicatorManager<int> * ptNewRecipeManager = ptPlayer->ptNewRecipeManager;
    UIFilterType eFilterType;
    UIID idKey;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);
        UIID idNewIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.NewStar");
        BOOL bNewItem = GetRingUI().IsEnabled() && FilterHasNewItems(eFilterType, ptPlayer->idPlayerACD, *ptNewRecipeManager);
        UI_GetWindowFromID(idNewIndicator).SetActive(bNewItem);
    }
}

GBID PlayerBlacksmithScreen::GetSelectedRecipe()
{
    BlacksmithMode eMode = GetBlacksmithMode();
    if (eMode == BLACKSMITH_MODE_WEAPONS || eMode == BLACKSMITH_MODE_ARMOR)
    {
        if (m_eCraftingState == CRAFTING_STATE_START)
        {
            int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(GetPrimaryView()->GetSelectedItem());
            if (nIndex != -1)
            {
                return m_tRecipeItemsProvider.GetRecipes()[nIndex];
            }
        }
        else if (m_eCraftingState == CRAFTING_STATE_SHOW_RESULTS)
        {
            return m_tCrafingItemProvider.GetRecipe();
        }
    }
    return GBID_NONE;
}

BOOL PlayerBlacksmithScreen::ArtisanCanCraftSelectedRecipe()
{
    GBID gbidRecipe = GetSelectedRecipe();
    return ItemCrafting_ArtisanCanCraftItem(gbidRecipe, CRAFTER_BLACKSMITH, GetPlayer());
}

void OnBlacksmithUpdate()
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->Update();
    }
}

void OnBlacksmithLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnBlacksmithRightStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->HandleRightStickInput(nControllerIndex);
    }
}

void OnBlacksmithButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}

}  // namespace Console
}  // namespace UIInventory

namespace UIBlacksmith {
namespace Console {
using namespace UIInventory::Console;

void InitializeForGame()
{
    if (!GameIsStartUp())
    {
        sgptBlacksmithGlobals = new PlayerBlacksmithScreen();
    }
}

void DisposeForGame()
{
    if (!GameIsStartUp())
    {
        ::delete sgptBlacksmithGlobals;
        sgptBlacksmithGlobals = NULL;
    }
}

void OpenForController(int32 nControllerIndex)
{
    UIFastEquip::Console::CloseForPlayer(PlayerGetByControllerIndex(nControllerIndex)->nPlayerIndex);
    sgptBlacksmithGlobals->OpenForController(nControllerIndex);
}

void Close()
{
    if (sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->Close();
    }
}

} } namespace UIInventory { namespace Console {















void PlayerBlacksmithScreen::RepairAllItems(
    const BOOL bEquippedOnly)
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer != NULL)
    {
        UIRepair::Console::RepairAllItems(ptPlayer, bEquippedOnly);
    }
}













} } namespace UIBlacksmith { namespace Console {

void OnCraftingResults(const CraftingResultsMessage * pMsg)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->OnCraftingResults(pMsg);
    }
}

void OnSalvageResults(const SalvageResultsMessage * pMsg)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->OnSalvageResults(pMsg);
    }
}

} } // namespace UIBlacksmith::Console


namespace UIInventory { namespace Console {

void PlayerBlacksmithScreen::ShowError(
    CRefString sErrorMsg)
{
    NotificationCreationParams tParams;
    tParams.uszMessage = sErrorMsg.str();
    tParams.nControllerIndex = GetPlayer()->nControllerIndex;

    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}
} } namespace UIBlacksmith { namespace Console {

void OnBlacksmithChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)
{
    if (sgptBlacksmithGlobals == NULL || !sgptBlacksmithGlobals->IsActive())
    {
        return;
    }
    if (!bForce && tOldLoc.eActorLocation == tNewLoc.eActorLocation && tNewLoc.tInvLocation.eSlot == tOldLoc.tInvLocation.eSlot)
    {
        return;
    }
    Player * ptPlayer = sgptBlacksmithGlobals->GetPlayer();
    if (ptPlayer == NULL || tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD || tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
    {
        sgptBlacksmithGlobals->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}

}  // namespace Console
}  // namespace UIBlacksmith

namespace {

BOOL RingIncludeControlPredicate(const UIID & idControl)
{
    return idControl != UIWND_INVENTORY_CONSOLE_BLACKSMITH_WEAPONS_RING_GLOW && idControl != UIWND_INVENTORY_CONSOLE_BLACKSMITH_ARMOR_RING_GLOW;
}

}  // namespace

#include "../Controls/UIRActorControl.h"
#include "../../RActors/RActors.h"
#include "../../../Common/TagMaps/AnimTagsSwig.h"
// DWARF UI.cpp:5757 for UI_GetRActorFromID; UIGlobals.cpp for UIIDs.
UIRActor & UI_GetRActorFromID(const UIID & id);
extern const UIID UIRACTOR_CONSOLE_RING, UITXT_CONSOLE_PLAYER_LABEL_RING;
// DWARF RActorAnimation.cpp:1342.
BOOL RActorPlayAnimation(RActor * ptRActor, const uint32 dwTag, BOOL bDisableBlending, BOOL fOnlyPlayIfDifferent);
// DWARF UIInventoryCommon.cpp:478.
namespace UIInventory { void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD); }
// DWARF InputNX64.cpp:1993.
void InputRemapSingleJoyconButton(InputButton eFrom, InputButton eTo);

// DWARF UI.cpp:6354 and Lights.cpp:1104.
SNO UI_GetDefaultHeroLight();
LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);

#include "../../../Common/ItemsSwig.h"
// DWARF UIInventoryCommon.cpp:296.
namespace UIInventory { void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer); }
// DWARF ActorInventory.cpp:11604.
void ACDInventoryWhereCanThisGo(const ActorCommonData & tACDPlayer, const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots);

















































































namespace UIInventory { namespace Console {

void PlayerBlacksmithScreen::OnButton2()

{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    if (idAction == ACTION_CLOSE)
    {
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
    }
    // Inference: the line-0 ACTION_CLOSE_SLOT comparison forms the next branch.
    else if (idAction == ACTION_CLOSE_SLOT)
    {
        if (GetPrimaryView() != NULL)
        {
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            if (idSelectedItem != ACDID_INVALID)
            {
                m_tRecipeItemsProvider.UnMarkAsNew(idSelectedItem);
            }
        }

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseSelectedSlot();
        UpdateRingNewItemIndicators();
    } // Inference: the line-0 ACTION_CANCEL_SALVAGING comparison forms the next branch.
    else if (idAction == ACTION_CANCEL_SALVAGING)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SetSalvageState(SALVAGE_STATE_START);
    }
    else if (idAction == ACTION_CANCEL_CRAFTING || idAction == ACTION_FINISH_CRAFTING)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        CancelCrafting();
    }
}
} } // namespace UIInventory::Console

namespace UIInventory { namespace Console {

void PlayerBlacksmithScreen::OnButton3()

{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    switch (idAction)
    {
    case ACTION_REPAIR_EQUIPPED:
        RepairAllItems(TRUE); // Inference: each case exits through the common tail.
        break;
    case ACTION_DETAILS:
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            SetBrowsingMode(BROWSE_DETAILS);
            GetPrimaryView()->SelectItem(idSelectedItem); // Inference: this case exits after selection.
            break;
        }
    case ACTION_LIST:
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            SetBrowsingMode(BROWSE_LIST);
            GetPrimaryView()->SelectItem(idSelectedItem); // Inference: this case exits after selection.
            break;
        }
    case ACTION_EQUIP: {
            Player * ptPlayer = GetPlayer();
            if (ptPlayer == NULL)
            {
                // Inference: missing player exits the case via the common tail.
                break;
            }
            ACDID idItem = m_tCrafingItemProvider.GetCraftedItem();
            if (idItem != ACDID_INVALID)
            {
                ActorCommonData * ptACDItem = ACDTryToGet(idItem);
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDItem == NULL || ptACDPlayer == NULL)
                {
                    // Inference: either failed lookup exits through the shared line-0 tail.
                    break;
                }
                int32 nNumSlots = 0;
                InventorySlotType eValidSlots[4];
                eValidSlots[0] = GetSlotForFilterType(GetFilter());
                if (eValidSlots[0] == SLOT_NONE)
                {

                    ACDInventoryWhereCanThisGo(*ptACDPlayer, ptACDItem->hGB.gbid, nNumSlots, eValidSlots);
                    if (eValidSlots[0] == SLOT_NONE)
                    {
                        // Inference: no valid destination exits without moving the item.
                        break;
                    }
                }
                else
                {
                    nNumSlots = 1;
                }


                ptPlayer->ptNewItemManager->UnMarkAsNew(idItem);


                QuickMoveToEmptySlot(*ptACDPlayer, *ptACDItem, nNumSlots, eValidSlots, FALSE, ptPlayer);


                CancelCrafting();


                SetBrowsingMode(BROWSE_LIST);
            }
        }
    }
} } } // function and namespace UIInventory::Console
namespace UIInventory { namespace Console {
void DoMassSalvage(const UIID & id); // DWARF UIConsoleBlacksmith.cpp:2779

void PlayerBlacksmithScreen::OnButton4()

{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    switch (idAction)
    {
    case ACTION_REPAIR_ALL:
        RepairAllItems(FALSE); // Inference: each switch arm ends after its action; shared epilogue1682.
        break;

    case ACTION_FILTER_ALL:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SetRecipeListFilter(RECIPE_FILTER_ALL); // Inference: this arm exits via the shared tail.
        break;

    case ACTION_FILTER_CLASS:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SetRecipeListFilter(RECIPE_FILTER_CLASS); // Inference: this arm exits via the shared tail.
        break;

    case ACTION_FILTER_MATERIALS:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SetRecipeListFilter(RECIPE_FILTER_MATERIALS); // Inference: this arm exits via the shared tail.
        break;

    case ACTION_SALVAGE:
        SalvageItem(); // Inference: this arm exits via the shared tail.
        break;

    case ACTION_MASS_SALVAGE:
        {
            Player * ptPlayer = GetPlayer();
            if (ptPlayer == NULL)
            {
                // Inference: absent-player path jumps to the shared switch epilogue.
                break;
            }

            NotificationCreationParams tParams;
            tParams.uszMessage = StringListFetch("ConsoleArtisan:MassSalvageWarning").str();
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &DoMassSalvage>();
            tParams.nControllerIndex = ptPlayer->nControllerIndex;

            ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
        } // Inference: this arm exits after notification destruction at1679.
        break;
    }
}
} } // namespace UIInventory::Console


































namespace UIInventory { namespace Console {
void DoSalvageAllJunk(const UIID & id); // DWARF UIConsoleBlacksmith.cpp:2768

void PlayerBlacksmithScreen::OnRightStickButton()

{
    Player * ptPlayer = sgptBlacksmithGlobals->GetPlayer();

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON);
    switch (idAction)
    {
    case ACTION_SALVAGE_ALL_JUNK:
        {

            NotificationCreationParams tParams;
            tParams.uszMessage = StringListFetch("ConsoleArtisan:SalvageJunkWarning").str();
            tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &DoSalvageAllJunk>();
            tParams.nControllerIndex = ptPlayer->nControllerIndex;

            ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
            // Inference: the single-action switch exits after displaying the dialog (tail at 1740).
            break;
        } }
}
} } // namespace UIInventory::Console





















namespace UIInventory { namespace Console {
void ShowBlacksmithModel(const Player & tPlayer); // DWARF UIConsoleBlacksmith.cpp:2729

void PlayerBlacksmithScreen::OnOpen()

{
    Player * ptPlayer = GetPlayer();
    ptPlayer->eActiveCrafterType = CRAFTER_BLACKSMITH;

    m_tPlayerItemsProvider.SetPlayer(ptPlayer);
    m_tSingleItemProvider.SetPlayer(ptPlayer);
    m_tEquippedItemProvider.SetPlayer(ptPlayer);
    m_tTrainingItemsProvider.SetPlayer(ptPlayer);
    m_tRecipeItemsProvider.SetPlayer(ptPlayer);
    m_tCrafingItemProvider.SetPlayer(ptPlayer);

    SetBlacksmithMode(BLACKSMITH_MODE_WEAPONS);

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));


    SetFocusedView(NULL);

    ShowBlacksmithModel(*ptPlayer);


    UIRActor & tHero = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
    LightID idLight = LightAdd(
        0,
        UI_GetDefaultHeroLight(),
        PRTransform(cgqIdentity, cgwpNull),
        0,
        tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHero.SetLight(idLight);
}
} } // namespace UIInventory::Console




// DWARF: X/Client/CItems.cpp:117.
UIItemOverlay ItemGetOverlay(const ActorCommonData & tACDItem, const ActorCommonData & tACDPlayer);
























































































namespace UIInventory { namespace Console {

void PlayerBlacksmithScreen::UpdateCraftingActions(
    ::Console::ControllerActionManager & tActionManager)
{
    if (RingHasFocus())
    {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    }
    else
    {
        switch (m_eCraftingState)
        { case CRAFTING_STATE_START: {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());

            if (GetSelectedRecipe() != GBID_NONE)
            {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CREATE, StringListFetch("ConsoleCrafting:Create").str());
            }


            if (GetBrowsingMode() == BROWSE_LIST)
            {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Compare").str());
            }
            else
            {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
            }

            if (CanScrollDetails())
            { // Inference: sibling UIConsoleMystic.cpp:2114 DWARF names this shared prompt sScrollPrompt; owner has no named local.
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
            }

            ACDID idEquippedItem = GetEquippedItem(GetPlayer(), GetFilter(), FALSE);
            if (idEquippedItem != ACDID_INVALID)
            {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
            } // Inference: case completion branches to the common return.
            break;
        }
        case CRAFTING_STATE_SHOW_PROGRESS:

            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL_CRAFTING, StringListFetch("General:Cancel").str()); // Inference: completed case branches to common return.
            break;
        case CRAFTING_STATE_DONE:

            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_CRAFTING, StringListFetch("H2OLayout:Continue").str()); // Inference: completed case branches to common return.
            break;
        case CRAFTING_STATE_SHOW_RESULTS: {

            if (GetSelectedRecipe() != GBID_NONE)
            {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CREATE_ANOTHER, StringListFetch("ConsoleCrafting:CreateAnother").str());
            }

            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_FINISH_CRAFTING, StringListFetch("ConsoleUI:Back").str());

            ACDID idCraftedItem = m_tCrafingItemProvider.GetCraftedItem();
            ActorCommonData * ptACDItem = ACDTryToGet(idCraftedItem);
            ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
            if (ptACDItem != NULL && ptACDPlayer != NULL)
            {


                ActorLocationType eLocationType = ptACDItem->GetLocationType();
                if (eLocationType == AL_INVENTORY)
                {
                    UIItemOverlay eOverlay = ItemGetOverlay(*ptACDItem, *ptACDPlayer);
                    if (ItemIsEquippable(*ptACDItem) && eOverlay == ITEMOVERLAY_NONE)
                    {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_EQUIP, StringListFetch("ConsoleInventory:Equip").str());
                    }

            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_SALVAGE, StringListFetch("ConsoleCrafting:Salvage").str());
                }
            }

            if (CanScrollDetails())
            { // Inference: sibling UIConsoleMystic.cpp:2114 DWARF names this shared prompt sScrollPrompt; owner has no named local.
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
            }

            ACDID idEquippedItem = GetEquippedItem(GetPlayer(), GetFilter(), FALSE);
            if (idEquippedItem != ACDID_INVALID)
            {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_DPAD_LEFT, ACTION_TOGGLE_STAT_RANGES, StringListFetch("ConsoleUI:ToggleStatRanges").str());
            }
        }
        } }
}
} }
namespace UIInventory { namespace Console {


void PlayerBlacksmithScreen::UpdateSalvageActions(
    ::Console::ControllerActionManager & tActionManager)
{
    if (m_eSalvageState == SALVAGE_STATE_START)
    {
        if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_ITEM_TO_SALVAGE, StringListFetch("ConsoleUI:Open").str());

            if (HasSalvageableJunk())
            {

                InputRemapSingleJoyconButton(BUTTON_DPAD_RIGHT, BUTTON_RIGHT_STICK_BUTTON);


                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_STICK_BUTTON, ACTION_SALVAGE_ALL_JUNK, StringListFetch("ConsoleCrafting:SalvageAllJunk").str());
            }

            if (CanMassSalvage())
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_MASS_SALVAGE, StringListFetch("ConsoleCrafting:MassSalvage").str());
            }

            if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
            {
                if (GetBrowsingMode() == BROWSE_LIST)
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Details").str());
                }
                else
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
                }
            }
        }

        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
    } // Inference: line-0 state comparison routes waiting-for-confirmation to these actions.
    else if (m_eSalvageState == SALVAGE_STATE_WAITING_TO_CONFIRM)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SALVAGE, StringListFetch("ConsoleCrafting:Salvage").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL_SALVAGING, StringListFetch("ConsoleUI:Back").str());
    } // Inference: line-0 results comparison selects the continue action.
    else if (m_eSalvageState == SALVAGE_STATE_RESULTS)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_SALVAGE, StringListFetch("H2OLayout:Continue").str());
    }
}
} } // namespace UIInventory::Console

namespace UIInventory { namespace Console {

void PlayerBlacksmithScreen::UpdateRepairActions(
    ::Console::ControllerActionManager & tActionManager)
{
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

    if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_REPAIR, StringListFetch("ConsoleShop:Repair").str());
    }

    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_REPAIR_EQUIPPED, StringListFetch("ConsoleShop:RepairEquipped").str());
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_REPAIR_ALL, StringListFetch("ConsoleShop:RepairAll").str());
}




void PlayerBlacksmithScreen::UpdateTrainingActions(
    ::Console::ControllerActionManager & tActionManager)
{
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

    if (ItemCrafting_CanTrain(*GetPlayer()))
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_TRAIN, StringListFetch("ConsoleCrafting:Train").str());
    }

    if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
    {
        if (GetBrowsingMode() == BROWSE_LIST)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Details").str());
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        }
    }
}
} } // namespace UIInventory::Console
























































































































































































































































































































































































































































































































namespace UIInventory { namespace Console {

BOOL PlayerBlacksmithScreen::HasSalvageableJunk() const
{
    const ACDIDArray & aJunk = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_JUNK);

    if (aJunk.size() == 0)
    {
        // Inference: the empty-list branch reaches the shared false-return tail.
        return FALSE;
    }
    const ActorCommonData * ptPlayerACD = ACDTryToGet(GetPlayer()->idPlayerACD);
    // Inference: the line-0 null-player branch reaches the same false-return tail.
    if (ptPlayerACD == NULL)
    {
        return FALSE;
    }

    for (int32 n = (int32)aJunk.size() - 1; n >= 0; --n)
    {
        ACDID id = aJunk[n];
        if (id != 0)
        {

            ActorCommonData * ptItemACD = ACDTryToGet(id);
            // Inference: the line-0 null-item branch continues the descending loop.
            if (ptItemACD == NULL)
            {
                continue;
            }

            if (SalvageCommon::ItemCanSalvage(*ptItemACD, *ptPlayerACD) == GAMEERROR_NONE)
            {
                // Inference: success returns TRUE through the shared line-0 epilogue.
                return TRUE;
            }
        }
    }
    // Inference: exhaustion returns FALSE through the shared line-0 epilogue.
    return FALSE;
}


BOOL PlayerBlacksmithScreen::CanMassSalvage() const
{
    const ACDIDArray & aSalvageList = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_MASS_SALVAGEABLE);

    if (aSalvageList.size() == 0)
    {
        // Inference: the empty-list branch reaches the shared false-return tail.
        return FALSE;
    }
    const ActorCommonData * ptPlayerACD = ACDTryToGet(GetPlayer()->idPlayerACD);
    // Inference: the line-0 null-player branch reaches the same false-return tail.
    if (ptPlayerACD == NULL)
    {
        return FALSE;
    }

    for (int32 n = (int32)aSalvageList.size() - 1; n >= 0; --n)
    {
        ACDID id = aSalvageList[n];
        if (id != 0)
        {

            ActorCommonData * ptItemACD = ACDTryToGet(id);
            // Inference: the line-0 null-item branch continues the descending loop.
            if (ptItemACD == NULL)
            {
                continue;
            }

            if (SalvageCommon::ItemCanSalvage(*ptItemACD, *ptPlayerACD) == GAMEERROR_NONE)
            {
                // Inference: success returns TRUE through the shared line-0 epilogue.
                return TRUE;
            }
        }
    }
    // Inference: exhaustion returns FALSE through the shared line-0 epilogue.
    return FALSE;
}

} } // namespace UIInventory::Console



















































namespace UIInventory { namespace Console {

void ShowBlacksmithModel(
    const Player & tPlayer)
{
    ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_NPC, FALSE, TRUE);
    UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
    SNO snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_BLACKSMITH);
    tModel.CreateRActor(RENDERCONTEXT_HUD_0, snoActor);
    tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_CRAFTER));

    RActor * ptRActor = RActorTryToGet(tModel.GetRActor());
    // Inference: the line-0 actor lookup failure skips animation and continues UI setup.
    if (ptRActor != NULL) {
        RActorPlayAnimation(ptRActor, TAG_ACTOR_ANIMATION_GIZMO_STATE_10, TRUE, FALSE);
    }


    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", 0);


    UIID idGold;
    idGold.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "GoldBalance");
    UpdateGoldAmount(idGold, tPlayer.idPlayerACD);
}
} } // namespace UIInventory::Console













namespace UIInventory { namespace Console {

void DoSalvageAllJunk(
    const UIID & id)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->SalvageAllJunk();
    }
}
} } // namespace UIInventory::Console
namespace UIInventory { namespace Console {

void DoMassSalvage(
    const UIID & id)
{
    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->MassSalvage();
    }
}
} } // namespace UIInventory::Console

























































namespace UIBlacksmith { namespace Console {

void OpenWeaponsScreen(
    int32 nControllerIndex)
{


    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->SetBlacksmithMode(PlayerBlacksmithScreen::BLACKSMITH_MODE_WEAPONS);
    }
}




void OpenArmorScreen(
    int32 nControllerIndex)
{


    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->SetBlacksmithMode(PlayerBlacksmithScreen::BLACKSMITH_MODE_ARMOR);
    }
}




void OpenSalvageScreen(
    int32 nControllerIndex)
{


    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->SetBlacksmithMode(PlayerBlacksmithScreen::BLACKSMITH_MODE_SALVAGE);
    }
}




void OpenRepairScreen(
    int32 nControllerIndex)
{


    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->SetBlacksmithMode(PlayerBlacksmithScreen::BLACKSMITH_MODE_REPAIR);
    }
}




void OpenTrainScreen(
    int32 nControllerIndex)
{


    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->SetBlacksmithMode(PlayerBlacksmithScreen::BLACKSMITH_MODE_TRAIN);
    }
}
} } // namespace UIBlacksmith::Console





























namespace UIBlacksmith { namespace Console {

void OnTrainingResults(
    const IntDataMessage * pMsg)
{


    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->OnTrainingResults(pMsg);
    }
}




void OnRepair()
{



    if (sgptBlacksmithGlobals != NULL && sgptBlacksmithGlobals->IsActive())
    {
        sgptBlacksmithGlobals->OnRepair();
    }
}
} } // namespace UIBlacksmith::Console

#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../Sigma/UnicodeText.h"
extern const SymbolicEntry gCrafterTypes[];
extern const UIID UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED;
BOOL ItemPlayerHasRecipeReagents(const Player& tPlayer, GBID gbidRecipe);
namespace UISalvage { BOOL ShouldWarnOnSalvage(const ActorCommonData& tACDItem); }
namespace UIGame { void DisplayError(const GameErrorData& tError, BOOL bSilent, const Player* ptPlayer); }
namespace UIRepair { namespace Console { void RepairItem(Player* ptPlayer, ACDID idItem); }
}
namespace Console { namespace Screens {
enum { BLACKSMITH_WEAPONS_SCREEN = 0, BLACKSMITH_ARMOR_SCREEN = 1, BLACKSMITH_SALVAGE_SCREEN = 2, BLACKSMITH_REPAIR_SCREEN = 3, BLACKSMITH_TRAIN_SCREEN = 4 };
}
}
namespace {
#line 3066 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void EnableTabNavigation(
    const BOOL bEnable)
{
    static LPCSTR arTabs[5] =
    {
        ".WeaponsTab",
        ".ArmorTab",
        ".SalvageTab",
        ".RepairTab",
        ".TrainTab",
    };

    ::Console::Screens::EnableScreen(::Console::Screens::BLACKSMITH_SCREEN_SET, ::Console::Screens::BLACKSMITH_WEAPONS_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::BLACKSMITH_SCREEN_SET, ::Console::Screens::BLACKSMITH_ARMOR_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::BLACKSMITH_SCREEN_SET, ::Console::Screens::BLACKSMITH_SALVAGE_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::BLACKSMITH_SCREEN_SET, ::Console::Screens::BLACKSMITH_REPAIR_SCREEN, bEnable);
    ::Console::Screens::EnableScreen(::Console::Screens::BLACKSMITH_SCREEN_SET, ::Console::Screens::BLACKSMITH_TRAIN_SCREEN, bEnable);

    UIID idNavBar = UIIDCreateFromParent(UIDLG_INVENTORY_CONSOLE, ".LayoutRoot.Navigation.Blacksmith");
    for (int32 i = 0; i < 5; ++i)
    {
        UIID idTab = UIIDCreateFromParent(idNavBar, arTabs[i]);
        UI_GetWindowFromID(UIIDCreateFromParent(idTab, ".KeyBinding1")).SetActive(bEnable);
        UI_GetWindowFromID(UIIDCreateFromParent(idTab, ".KeyBinding2")).SetActive(bEnable);
    }
}
}
namespace UIInventory {
namespace Console {
#line 884 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::UpdateNavBar()
{
    static LPCSTR arTabs[5] = { "WeaponsTab", "ArmorTab", "SalvageTab", "RepairTab", "TrainTab" };
    UIID idNavBar = GetUIIDNone();
    idNavBar.SetFromParent(UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.Blacksmith");
    int32 nTabIndex = GetBlacksmithMode();
    for (int32 i = 0; i < 5; ++i) {
        UIID idTab = GetUIIDNone();
        idTab.SetFromParent(idNavBar, arTabs[i]);
        UI_GetWindowFromID(idTab).SetActive(i == nTabIndex);
    }
    if (nTabIndex >= 0) {
        UIID idBlinker = GetUIIDNone();
        idBlinker.SetFromParent(idNavBar, CRefString().Format("%s.New5", arTabs[nTabIndex]));
        Player* ptPlayer = sgptBlacksmithGlobals->GetPlayer();
        BOOL bActive = nTabIndex != BLACKSMITH_MODE_TRAIN && ptPlayer != NULL && GetCraftLevel(CRAFTER_BLACKSMITH, *ptPlayer) <= 1 && ItemCrafting_CanTrain(*ptPlayer);
        UIBlinker& tBlinker = UI_GetBlinkerFromID(idBlinker);
        tBlinker.SetActive(bActive);
    }
}
#line 928 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::CraftSelectedRecipe()
{
    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    // Inferred local name for original retained recipe lookup index, matching original single lookup.
    int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(idSelectedItem);
    if (nIndex == -1) return;
    EnableTabNavigation(FALSE);
    GBID gbidRecipe = m_tRecipeItemsProvider.GetRecipes()[nIndex];
    CraftItem(gbidRecipe, idSelectedItem);
}
#line 960 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::CraftItemAgain()
{
    if (m_eCraftingState == CRAFTING_STATE_SHOW_RESULTS) {
        EnableTabNavigation(FALSE);
        ACDID idItem = m_tCrafingItemProvider.GetRecipeItem();
        GBID gbidRecipe = m_tCrafingItemProvider.GetRecipe();
        CraftItem(gbidRecipe, idItem);
    }
}
#line 2755 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void DoSalvage(const UIID& id)
{
    if (sgptBlacksmithGlobals && sgptBlacksmithGlobals->IsActive()) sgptBlacksmithGlobals->BeginSalvage();
}
#line 1024 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::SalvageItem()
{
    Player* ptPlayer = GetPlayer();
    if (!ptPlayer) return;
    BlacksmithMode eMode = GetBlacksmithMode();
    ActorCommonData* ptACDOwner;
    ActorCommonData* ptACDItem;
    if (eMode == BLACKSMITH_MODE_SALVAGE) {
        UIInventoryView* ptView = GetPrimaryView();
        if (!ptView) return;
        ptACDOwner = ACDTryToGet(ptPlayer->idPlayerACD);
        ptACDItem = ACDTryToGet(ptView->GetSelectedItem());
    }
    else if ((eMode == BLACKSMITH_MODE_WEAPONS || eMode == BLACKSMITH_MODE_ARMOR) && m_eCraftingState == CRAFTING_STATE_SHOW_RESULTS) {
        ptACDOwner = ACDTryToGet(ptPlayer->idPlayerACD);
        ptACDItem = ACDTryToGet(m_tCrafingItemProvider.GetCraftedItem());
    }
    else return;
    if (ptACDOwner && ptACDItem) {
        GameError eSalvageError = SalvageCommon::ItemCanSalvage(*ptACDItem, *ptACDOwner);
        if (eSalvageError == GAMEERROR_NONE) {
            if (UISalvage::ShouldWarnOnSalvage(*ptACDItem)) {
                NotificationCreationParams tParams;
                tParams.uszMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SALVAGE), "Warning").str();
                tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID&, &DoSalvage>();
                tParams.nControllerIndex = GetPlayer()->nControllerIndex;
                ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
            }
            else DoSalvage(GetUIIDNone());
        }
        else UIGame::DisplayError(GameErrorData(eSalvageError), FALSE, NULL);
    }
}
#line 1142 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::EndSalvage()
{
    m_tSalvageView.ClearSalvageResults();
    SetSalvageState(SALVAGE_STATE_START);
}
#line 1314 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::RepairItem()
{
    Player* ptPlayer = GetPlayer();
    UIInventoryView* ptView = GetPrimaryView();
    if (ptPlayer && ptView) UIRepair::Console::RepairItem(ptPlayer, ptView->GetSelectedItem());
}
#line 1341 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::Train()
{
    // Inferred message local spelling from sibling PlayerMysticScreen::Train; original sends eight bytes.
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_VENDOR_TRY_TRAIN, &tMessage, sizeof(tMessage), GetPlayer());
}
#line 2359 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::SelectItemToSalvage()
{
    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (idSelectedItem != ACDID_INVALID) SetSalvageState(SALVAGE_STATE_WAITING_TO_CONFIRM);
}
#line 1439 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::OnButton1()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction) {
    case ACTION_OPEN_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        OpenSelectedSlot();
        UpdateRingNewItemIndicators();
        break;
    case ACTION_CREATE: {
        GBID gbidRecipe = GetSelectedRecipe();
        if (!ArtisanCanCraftSelectedRecipe()) {
            ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
            LPCSTR szCrafterLabel = SymbolToName(CRAFTER_BLACKSMITH, gCrafterTypes);
            CRefString sCrafter = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_VENDOR), szCrafterLabel);
            CRefString sErrorMsg = LocalizedSprintf(StringListFetch("ConsoleCrafting:ErrorCrafterLevelTooLow").str(), sCrafter.str(), UnicodeText::FormatNumber(pdefRecipeEntry->nLevel, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            ShowError(sErrorMsg);
        }
        else if (!ItemPlayerHasRecipeReagents(*GetPlayer(), gbidRecipe)) ShowError(StringListFetch("ConsoleCrafting:ErrorIngredients"));
        else {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CRAFT), cgtPSODefaults);
            CraftSelectedRecipe();
        }
        break;
    }
    case ACTION_ACCEPT_CRAFTING:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        ViewCraftingResults();
        break;
    case ACTION_CREATE_ANOTHER:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CRAFT), cgtPSODefaults);
        CraftItemAgain();
        break;
    case ACTION_SELECT_ITEM_TO_SALVAGE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SelectItemToSalvage();
        break;
    case ACTION_SALVAGE: SalvageItem(); break;
    case ACTION_ACCEPT_SALVAGE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        EndSalvage();
        break;
    case ACTION_REPAIR: RepairItem(); break;
    case ACTION_TRAIN:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRAIN), cgtPSODefaults);
        Train();
        break;
    }
}
#line 795 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::UpdateSalvageUI()
{
    if (m_eSalvageState == SALVAGE_STATE_START) SyncViews();
    CRefString sTitle = StringListFetch("ConsoleArtisan:Salvage");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, FALSE);
}
#line 2120 "X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp"
void PlayerBlacksmithScreen::OnUpdate()
{
    Player* ptPlayer = GetPlayer();
    if (!ptPlayer) return;
    BOOL bShouldHighlight = !RingHasFocus() || m_eBlacksmithMode == BLACKSMITH_MODE_SALVAGE || m_eBlacksmithMode == BLACKSMITH_MODE_TRAIN || m_eBlacksmithMode == BLACKSMITH_MODE_REPAIR;
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(bShouldHighlight);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText("", FALSE);
    UpdateCurrencyDisplay(UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED, 0);
    switch (m_eBlacksmithMode) {
    case BLACKSMITH_MODE_WEAPONS:
    case BLACKSMITH_MODE_ARMOR: UpdateCraftingProgress(); UpdateCraftingUI(); break;
    case BLACKSMITH_MODE_SALVAGE: UpdateSalvageUI(); break;
    case BLACKSMITH_MODE_REPAIR: SyncViews(); UpdateRepairUI(); break;
    case BLACKSMITH_MODE_TRAIN: UpdateTrainingUI(); break;
    }
    UIInventoryView* ptBrowsingView = GetPrimaryView();
    if (ptBrowsingView) ClearStatusLine(ptBrowsingView);
    UIInventoryView* ptDetailsView = GetSecondaryView();
    if (ptDetailsView) ClearStatusLine(ptDetailsView);
}
}
}
