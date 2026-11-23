// Original file: X/Client/UserInterface/Console/UIConsoleJeweler.cpp
// The console jeweler screen: combine, socket, remove-gem and training tabs.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "ConsoleModalNotificationManager.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Controls/UIRActorControl.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "UIConsoleInventoryCategorySummaryView.h"
#include "UIConsoleInventoryCategoryView.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryGemCombineView.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryRemoveGemsView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventorySocketView.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleItemCategories.h"
#include "UIConsoleTraining.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../CGame.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Crafting/ItemCrafting.h
struct CraftingResultsMessage;
enum {
    CRAFTER_INVALID = -1,
    CRAFTER_JEWELER = 1,
};
// X/Common/Items.cpp
SocketReason ItemCanSocketInto(const ActorCommonData & tACDItemToSocket, ActorCommonData & tACDItemSocketedInto);
// X/Common/Player.h



// X/Common/Networking/GameMessage.h
struct IntDataMessage;




enum {
    CMSG_CLOSE_INTERACTION = 22,



};
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/CActorInventory.cpp
void CACDInventoryRequestToSocket(const ActorCommonData & tACDGem, const ActorCommonData & tACDItemToReceiveGem, Player * ptPlayer);
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/Interface/UIGame.cpp




namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIInventory {
void PlayItemInventorySound(const ActorCommonData & tACDItem);
void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD);
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
void UpdateTrainingUI(const CrafterType eCrafterType, const Player & tPlayer);
BOOL ShowTieredRiftMessage(const Player * ptPlayer);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
enum InfoPanelType {
    CRAFTING_SUMMARY_INFO_PANEL = 3,
    JEWELER_COMBINE_INFO_PANEL = 4,
    JEWELER_SOCKETING_INFO_PANEL = 5,
    JEWELER_REMOVE_INFO_PANEL = 6,
};
void ClearStatusLine(UIInventoryView * ptView);
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
}  // namespace Console
}  // namespace UIInventory
// X/Client/RActors/RActors.h:278 and :596. Only the member this file touches is declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    uint8 _unadopted0[0x410 - 0x04];  // 0x04 (invented name: idSScene .. end)
};
static_assert(sizeof(RActor) == 0x410, "RActor");

inline RActor * RActorTryToGet(const RActorID idRActor) {  // line 596
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}
// X/Client/RActors/RActorAnimation.cpp
BOOL RActorPlayAnimation(RActor * ptRActor, const uint32 dwTag, BOOL bDisableBlending, BOOL fOnlyPlayIfDifferent);
// X/Common/TagMaps/AnimTagsSwig.h
enum {
    TAG_ACTOR_ANIMATION_GIZMO_STATE_10 = 70304,
};
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
// X/Client/Render/Light.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGameAsync.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS,
    UIDLG_INVENTORY_CONSOLE_EQUIPPED, UIDLG_INVENTORY_CONSOLE_SOCKETING, UIDLG_INVENTORY_CONSOLE_COMBINE_GEMS,
    UIDLG_INVENTORY_CONSOLE_REMOVE_GEMS, UIDLG_INVENTORY_CONSOLE_REMOVE_GEMS_PANEL,
    UIDLG_INVENTORY_CONSOLE_TRAINING, UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_LIST,
    UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_DETAILS;
extern const UIID UIRACTOR_CONSOLE_RING, UITXT_CONSOLE_PLAYER_LABEL_RING; extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;
extern const UIID UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR,
    UIWND_INVENTORY_CONSOLE_ROOT, UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER, UITXT_INVENTORY_CONSOLE_GOLD; extern const UIID UITXT_INVENTORY_CONSOLE_GOLD_SPENT,  // DWARF: UIGlobals.cpp:3144 (HEADER), :3139 (GOLD); no header declarations recorded.
    UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED;

namespace {
void OnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory);
void UpdateGemRemovalPrice(const UIID & idPriceDisplay, const ACDID idItem);
bool sHasRoomToUnsocketGems(const Player * ptPlayer, const ACDID idItem);
}  // namespace

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_OPEN_CATEGORY = 1,
    ACTION_SELECT_GEM = 2,
    ACTION_COMBINE = 3,
    ACTION_ACCEPT_COMBINE = 4,
    ACTION_BEGIN_SOCKET = 5,
    ACTION_END_SOCKET = 6,
    ACTION_ACCEPT_SOCKETING = 7,
    ACTION_BACK = 8,
    ACTION_REMOVE = 9,
    ACTION_ACCEPT_REMOVE = 10,
    ACTION_TRAIN = 11,
    ACTION_CLOSE = 12,
    ACTION_DETAILS = 13,
    ACTION_LIST = 14,
    ACTION_FILTER_ALL = 15,
    ACTION_FILTER_MATERIALS = 16,
    ACTION_SCROLL_LIST = 17,
};

class PlayerJewelerScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x1630
public:
    enum JewelerMode {
        JEWELER_MODE_INVALID = -1,
        JEWELER_MODE_COMBINE = 0,
        JEWELER_MODE_SOCKET = 1,
        JEWELER_MODE_REMOVE = 2,
        JEWELER_MODE_TRAIN = 3,
    };
    enum RecipeListFilter {
        RECIPE_FILTER_INVALID = -1,
        RECIPE_FILTER_ALL = 0,
        RECIPE_FILTER_MATERIALS = 1,
    };

    virtual ~PlayerJewelerScreen() {}  // line 106

    JewelerMode GetJewelerMode() const  // line 129
    {
        return m_eJewelerMode;
    }

    PlayerJewelerScreen();
    void SetJewelerMode(const JewelerMode eJewelerMode);
    void SetRecipeListFilter(const RecipeListFilter eRecipeFilter);
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void OpenSelectedCategory();
    void SelectGem();
    void CombineGems();
    void BackUpCombineFlow();
    void CancelCombineGems();
    void OnCraftingResults(const CraftingResultsMessage * pMsg);
    void OnTrainingResults(const IntDataMessage * pMsg);
    void SetGemToSocket(const ACDID idGem);
    void BackUpSocketingFlow();
    void CancelSocketing();
    void SocketItem();
    void CloseSocketUI();
    void RemoveGem();
    void CloseRemoveGemUI();
    void Train();
    void OnButton1();
    void OnButton2();
    void OnButton3();
    void OnButton4();
    void OnJewelerScrollUp();
    void OnJewelerScrollDown();
    void UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory);
protected:
    virtual BOOL CanOpenForController(int32 nControllerIndex) const;  // vtable slot 15
    virtual void OnOpen();  // vtable slot 10
    virtual void OnClose();  // vtable slot 11
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);  // vtable slot 14
    virtual void OnSetActive(BOOL bActive);  // vtable slot 16
    virtual void OnUpdate();  // vtable slot 13
    virtual void OnRequestedUpdate();  // vtable slot 12
    virtual void InitializeRingWrapper(::Console::RingWrapper &);  // vtable slot 17
    virtual UIFilterType GetDefaultRingSelection() const;  // vtable slot 18
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
    enum CraftState {
        BROWSE_CATEGORY_STATE = 0,
        BROWSE_GEMS_STATE = 1,
        BROWSE_ITEMS_TO_SOCKET_STATE = 2,
        FINISHED_SOCKETING_STATE = 3,
        READY_TO_SOCKET_AGAIN_STATE = 4,
        BROWSE_ITEMS_STATE = 5,
        READY_TO_CRAFT_STATE = 6,
        BUSY_CRAFTING_STATE = 7,
        WAITING_FOR_RESULTS_STATE = 8,
        FINISHED_CRAFTING_STATE = 9,
    };
private:
    JewelerMode m_eJewelerMode;  // 0x1f0
    CraftState m_eCraftState;  // 0x1f4
    RecipeListFilter m_eRecipeFilter;  // 0x1f8
    InventoryItems m_tPlayerItemsProvider;  // 0x200
    CrafterRecipes m_tRecipeItemsProvider;  // 0x9f8
    TrainingItems m_tTrainingItemsProvider;  // 0xfd8
    SingleItemProvider m_tSingleItemProvider;  // 0x1028
    UIInventoryListView m_tPlayerListView;  // 0x1060
    UIInventoryDetailsView m_tPlayerDetailsView;  // 0x10e8
    UIInventoryCategoryView m_tGemCategoriesView;  // 0x1160
    UIInventoryCategorySummaryView m_tCategorySummaryView;  // 0x11f8
    UIInventoryListView m_tRecipesListView;  // 0x1260
    UIInventoryDetailsView m_tRecipesDetailsView;  // 0x12e8
    UIInventorySocketView m_tSocketView;  // 0x1360
    UIInventoryGemCombineView m_tCombineGemsView;  // 0x13f0
    UIInventoryRemoveGemsView m_tRemoveGemsView;  // 0x1460
    UIInventoryTrainingView m_tTrainingView;  // 0x14c8
    UIInventoryListView m_tTrainingListView;  // 0x1528
    UIInventoryDetailsView m_tTrainingDetailsView;  // 0x15b0
    ACDID m_idLastSelectedItem;  // 0x1628
    void RefreshWithFilter(UIFilterType eFilterType);
    BOOL CanCombineGems();
    BOOL CanOpenSelectedCategory();
    BOOL CanOpenCategory(const ItemCategory eCategory);
    void ShowGemCategories();
    void RefreshProviders(const MaintainSelectionOptions eSelectionType);
    void UpdateNavBar();
    void AddFilterActions(::Console::ControllerActionManager & tActionManager);
    void UpdateCombineActions(::Console::ControllerActionManager & tActionManager);
    void UpdateSocketingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateRemoveActions(::Console::ControllerActionManager & tActionManager);
    void UpdateTrainingActions(::Console::ControllerActionManager & tActionManager);
    void UpdateRingAndCharacter();
    void ShowSocketingUI();
    void UpdateSocketingState();
    void UpdateSocketingUI();
    void ShowCombineUI();
    void UpdateCombineState();
    void UpdateCombineUI();
    void SyncGemCategories();
    void SyncViews();
    void ShowRemoveGemUI();
    void UpdateRemoveGemState();
    void UpdateRemoveGemUI();
    void ShowTrainingUI();
    void UpdateTrainingUI();
    void OnNotificationDismissed(const UIID &);
    BOOL CanAffordToRemoveGem(ACDID idSelectedItem);
    void UpdateGemCategories();
};
static_assert(sizeof(PlayerJewelerScreen) == 0x1630, "PlayerJewelerScreen");

PlayerJewelerScreen * sgptJewelerGlobals;  // line 389
static const float SCROLLBOX_SCROLLDELTA = 4.0f;

void ShowJewelerModel(const Player & tPlayer);

PlayerJewelerScreen::PlayerJewelerScreen()
    : m_eJewelerMode(JEWELER_MODE_INVALID)
    , m_eCraftState(BROWSE_CATEGORY_STATE)
    , m_eRecipeFilter(RECIPE_FILTER_INVALID)
    , m_tPlayerItemsProvider(InventoryItems::PROVIDERMODE_BACKPACK_AND_STASH)
    , m_tPlayerListView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPlayerDetailsView(m_tPlayerItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, FALSE)
    , m_tGemCategoriesView(UIDLG_INVENTORY_CONSOLE_LIST, OnUpdateCategoryUI)
    , m_tCategorySummaryView(UIDLG_INVENTORY_CONSOLE_EQUIPPED)
    , m_tRecipesListView(m_tRecipeItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tRecipesDetailsView(m_tRecipeItemsProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, FALSE)
    , m_tSocketView(UIDLG_INVENTORY_CONSOLE_SOCKETING)
    , m_tCombineGemsView(m_tRecipeItemsProvider, UIDLG_INVENTORY_CONSOLE_COMBINE_GEMS)
    , m_tRemoveGemsView(UIDLG_INVENTORY_CONSOLE_REMOVE_GEMS)
    , m_tTrainingView(UIDLG_INVENTORY_CONSOLE_TRAINING)
    , m_tTrainingListView(m_tTrainingItemsProvider, UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_LIST, TRUE)
    , m_tTrainingDetailsView(m_tTrainingItemsProvider, UIDLG_INVENTORY_CONSOLE_TRAINING_REWARDS_DETAILS, FALSE)
{
    m_tRecipeItemsProvider.SetFilter(FILTER_JEWELER_RECIPES_START);
    m_idLastSelectedItem = ACDID_INVALID;
    m_tGemCategoriesView.SetHeaderText(StringListFetch("ConsoleCrafting:Recipes"));
    m_tCategorySummaryView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    m_tRecipesListView.SetHeaderText(StringListFetch("ConsoleCrafting:Recipes"));
    m_tRecipesDetailsView.SetHeaderText(StringListFetch("ConsoleCrafting:Recipes"));
    m_tSocketView.SetHeaderText(StringListFetch("ConsoleSocketing:Socketing"));
    m_tCombineGemsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    UpdateGemCategories();
}

void PlayerJewelerScreen::SetJewelerMode(const JewelerMode eJewelerMode)
{
    if (m_eJewelerMode == eJewelerMode)
    {
        return;
    }

    if (m_eJewelerMode == JEWELER_MODE_SOCKET)
    {
        CancelSocketing();
        m_tPlayerItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
    }
    if (m_eJewelerMode == JEWELER_MODE_COMBINE)
    {
        CancelCombineGems();
    }
    if (GetPrimaryView() != NULL)
    {
        GetPrimaryView()->SelectItem(ACDID_INVALID);
    }
    if (GetSecondaryView() != NULL)
    {
        GetSecondaryView()->SelectItem(ACDID_INVALID);
    }

    m_eJewelerMode = eJewelerMode;
    m_eCraftState = BROWSE_CATEGORY_STATE;
    UpdateGemCategories();
    SetRecipeListFilter(RECIPE_FILTER_ALL);
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    switch (m_eJewelerMode)
    {
    case JEWELER_MODE_COMBINE:
        RefreshWithFilter(FILTER_NONE);
        ShowCombineUI();
        break;
    case JEWELER_MODE_SOCKET:
        RefreshWithFilter(FILTER_NONE);
        ShowSocketingUI();
        break;
    case JEWELER_MODE_REMOVE:
        RefreshWithFilter(FILTER_SOCKETED);
        ShowRemoveGemUI();
        break;
    case JEWELER_MODE_TRAIN:
        RefreshWithFilter(FILTER_NONE);
        ShowTrainingUI();
        break;
    default:
        RefreshWithFilter(FILTER_NONE);
        break;
    }
    UpdateNavBar();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::SetRecipeListFilter(const RecipeListFilter eRecipeFilter)
{
    if (m_eRecipeFilter == eRecipeFilter)
    {
        return;
    }

    m_eRecipeFilter = eRecipeFilter;
    CRefString sTitle;
    switch (m_eRecipeFilter)
    {
    case RECIPE_FILTER_ALL:
        m_tRecipeItemsProvider.SetRecipeOptions(0);
        sTitle = StringListFetch("ConsoleArtisan:FilterAllTitle");
        break;
    case RECIPE_FILTER_MATERIALS:
        m_tRecipeItemsProvider.SetRecipeOptions(1 << 1);
        sTitle = StringListFetch("ConsoleArtisan:FilterMaterialsTitle");
        break;
    }

    m_tRecipesListView.SetHeaderText(sTitle);
    m_tRecipesDetailsView.SetHeaderText(sTitle);
    m_tGemCategoriesView.SetHeaderText(sTitle);
    if (GetPrimaryView() != NULL)
    {
        GetPrimaryView()->RequestUpdate(RESET_SELECTION);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::RefreshWithFilter(UIFilterType eFilterType)
{
    if (GetFilter() != eFilterType)
    {
        SetFilterType(eFilterType);
        return;
    }
    RefreshProviders(RESET_SELECTION);
}

inline void PlayerJewelerScreen::ShowCombineUI()  // line 581
{
    SwitchToInfoPanel(JEWELER_COMBINE_INFO_PANEL);
    GetPrimaryView()->Enable(TRUE);
    SetFocusedView(GetPrimaryView());
    UpdateCombineUI();
}

void PlayerJewelerScreen::UpdateCombineState()
{
    if (m_eCraftState == BUSY_CRAFTING_STATE && m_tCombineGemsView.IsFinishedCombining())
    {
        int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(GetPrimaryView()->GetSelectedItem());
        if (nIndex != -1)
        {
            GBID gbidRecipe = m_tRecipeItemsProvider.GetRecipes()[nIndex];
            GBIDDataMessage tMessage;
            tMessage.gbidData = gbidRecipe;
            MessageSendToServer(CMSG_TRY_CRAFT_ITEM, &tMessage, sizeof(tMessage), GetPlayer());
            m_eCraftState = WAITING_FOR_RESULTS_STATE;
        }
    }
}

void ShowJewelerModel(const Player & tPlayer)
{
    ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_NPC, FALSE, TRUE);
    UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
    SNO snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_JEWELER);
    tModel.CreateRActor(RENDERCONTEXT_HUD_0, snoActor);
    tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_CRAFTER));

    RActor * ptRActor = RActorTryToGet(tModel.GetRActor());
    if (ptRActor != NULL)
    {
        RActorPlayAnimation(ptRActor, TAG_ACTOR_ANIMATION_GIZMO_STATE_10, TRUE, FALSE);
    }

    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", 0);
    UIID idGold;
    idGold.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "GoldBalance");
    UpdateGoldAmount(idGold, tPlayer.idPlayerACD);
}

void PlayerJewelerScreen::UpdateCombineUI()
{
    CRefString sTitle = StringListFetch("ConsoleCrafting:Combine");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, 0);
    if (m_eCraftState == BROWSE_CATEGORY_STATE)
    {
        SyncGemCategories();
    }
    else
    {
        int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(GetPrimaryView()->GetSelectedItem());
        if (nIndex == -1)
        {
            m_tCombineGemsView.SelectItem(ACDID_INVALID);
        }
        else
        {
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            if (idSelectedItem != m_tCombineGemsView.GetSelectedItem())
            {
                m_tCombineGemsView.SelectItem(idSelectedItem);
            }
        }
    }
}

inline void PlayerJewelerScreen::SyncGemCategories()  // line 648
{
    ItemCategory eCategory = m_tGemCategoriesView.GetSelectedItem();
    if (eCategory != ACDID_INVALID)
    {
        m_tCategorySummaryView.SetCategory(eCategory);
    }
}

void PlayerJewelerScreen::ShowRemoveGemUI()
{
    SwitchToInfoPanel(JEWELER_REMOVE_INFO_PANEL);
    m_eCraftState = READY_TO_CRAFT_STATE;
    m_tPlayerItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
    GetPrimaryView()->RequestUpdate(RESET_SELECTION);
    m_tRemoveGemsView.SetHeaderText(StringListFetch("ConsoleCrafting:GemmedItem"));
    m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleCrafting:GemmedItems"));
    m_tPlayerDetailsView.SetHeaderText(StringListFetch("ConsoleCrafting:GemmedItems"));
    UpdateRemoveGemUI();
}

inline void PlayerJewelerScreen::UpdateRemoveGemState()  // line 696
{
    if (m_eCraftState == BUSY_CRAFTING_STATE && m_tRemoveGemsView.IsFinishedRemoving())
    {
        m_tPlayerItemsProvider.Refresh();
        GetPrimaryView()->RequestUpdate(RESET_SELECTION);
        m_eCraftState = FINISHED_CRAFTING_STATE;
    }
}

void PlayerJewelerScreen::UpdateRemoveGemUI()
{
    CRefString sTitle = StringListFetch("ConsoleCrafting:Remove");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, 0);

    UIID idPrice;
    idPrice.SetFromParent(UIDLG_INVENTORY_CONSOLE_REMOVE_GEMS_PANEL, "RemovalCost");
    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    UpdateGemRemovalPrice(idPrice, idSelectedItem);

    if (m_eCraftState != FINISHED_CRAFTING_STATE)
    {
        m_tRemoveGemsView.SelectItem(idSelectedItem);
    }
}

void PlayerJewelerScreen::ShowTrainingUI()
{
    CRefString sTitle = StringListFetch("ConsoleCrafting:Train");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, 0);
    SwitchToInfoPanel(CRAFTING_SUMMARY_INFO_PANEL);
    m_tTrainingView.SetHeaderText(StringListFetch("ConsoleCrafting:ArtisanRank"));
    m_tTrainingListView.SetHeaderText(StringListFetch("ConsoleCrafting:Rewards"));
    m_tTrainingDetailsView.SetHeaderText(StringListFetch("ConsoleCrafting:Rewards"));
    OpenSelectedSlot();
    UpdateTrainingUI();
}

inline void PlayerJewelerScreen::UpdateTrainingUI()  // line 755
{
    if (GetPlayer() != NULL)
    {
        UIUtils::Console::UpdateTrainingUI(CRAFTER_JEWELER, *GetPlayer());
    }
}

void PlayerJewelerScreen::UpdateNavBar()
{
    static const LPCSTR arTabs[] = { "CombineTab", "SocketTab", "RemoveTab", "TrainTab" };
    UIID idNavBar = GetUIIDNone();
    idNavBar.SetFromParent(UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.Jeweler");
    JewelerMode eMode = GetJewelerMode();
    for (int32 nTabIndex = 0; nTabIndex < 4; ++nTabIndex)
    {
        UIID idTab = GetUIIDNone();
        idTab.SetFromParent(idNavBar, arTabs[nTabIndex]);
        UI_GetWindowFromID(idTab).SetActive(eMode == nTabIndex);
    }
}

void PlayerJewelerScreen::ShowSocketingUI()
{
    SwitchToInfoPanel(JEWELER_SOCKETING_INFO_PANEL);
    m_tSocketView.SetHeaderText(StringListFetch("ConsoleSocketing:Socketing"));
    m_tPlayerListView.SetHeaderText(StringListFetch("ConsoleInventory:Inventory"));
    m_tPlayerDetailsView.SetHeaderText(StringListFetch("ConsoleUI:Details"));
    GetPrimaryView()->Enable(TRUE);
    SetFocusedView(GetPrimaryView());
    UpdateSocketingUI();
}

void PlayerJewelerScreen::UpdateSocketingUI()
{
    CRefString sTitle = StringListFetch("ConsoleNavMenu:Socket");
    Set_Text_Color(sTitle, 0);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(sTitle, 0);
    if (m_eCraftState == BROWSE_CATEGORY_STATE)
    {
        SyncGemCategories();
    }
    else
    {
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        if (m_eCraftState == BROWSE_GEMS_STATE)
        {
            m_tSocketView.SetJewel(idSelectedItem);
        }
        else if (m_eCraftState == BROWSE_ITEMS_TO_SOCKET_STATE)
        {
            m_tSocketView.SelectItem(idSelectedItem);
        }
    }
}

void PlayerJewelerScreen::UpdateSocketingState()
{
    if (m_eCraftState == BUSY_CRAFTING_STATE)
    {
        if (m_tSocketView.IsFinishedSocketing())
        {
            m_tPlayerItemsProvider.Refresh();
            GetPrimaryView()->RequestUpdate(RESET_SELECTION);

            ACDID idSelectedItem = m_tSocketView.GetSelectedItem();
            ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
            if (ptACDItem)
            {
                int32 nNumSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
                int32 nTotalSockets = ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS));
                if (nTotalSockets > nNumSocketsFilled)
                {
                    ACDID idJewel = m_tSocketView.GetJewel();
                    ActorCommonData * ptACDJewel = ACDTryToGet(idJewel);
                    if (ptACDJewel)
                    {
                        const ACDIDArray & arGems = m_tPlayerItemsProvider.GetItemsWithFilter(FILTER_GEMS);
                        for (uint32 i = 0; i < arGems.size(); ++i)
                        {
                            ActorCommonData * ptACDOtherGem = ACDTryToGet(arGems[i]);
                            if (ptACDOtherGem && ptACDJewel->hGB.gbid == ptACDOtherGem->hGB.gbid)
                            {
                                m_tSocketView.ResetToSocketAgain(arGems[i]);
                                m_eCraftState = READY_TO_SOCKET_AGAIN_STATE;
                                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
                                return;
                            }
                        }
                    }
                }
            }

            m_eCraftState = FINISHED_SOCKETING_STATE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        }
    }
}

void PlayerJewelerScreen::OpenSelectedCategory()
{
    ItemCategory eCategory = m_tGemCategoriesView.GetSelectedItem();
    UIFilterType eFilterType = GetFilterTypeForCategory(eCategory, m_eJewelerMode == JEWELER_MODE_COMBINE);
    switch (m_eJewelerMode)
    {
    case JEWELER_MODE_COMBINE:
        m_tRecipeItemsProvider.SetFilter(eFilterType);
        m_eCraftState = BROWSE_ITEMS_STATE;
        break;
    case JEWELER_MODE_SOCKET:
        m_tPlayerItemsProvider.SetFilter(eFilterType);
        m_eCraftState = BROWSE_GEMS_STATE;
        break;
    }
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
}

inline void PlayerJewelerScreen::SelectGem()  // line 959
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
    m_eCraftState = READY_TO_CRAFT_STATE;
    GetPrimaryView()->Enable(FALSE);
    SetFocusedView(GetSecondaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::CombineGems()
{
    int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(GetPrimaryView()->GetSelectedItem());
    if (nIndex == -1)
    {
        return;
    }
    m_eCraftState = BUSY_CRAFTING_STATE;
    m_tCombineGemsView.StartCombining();
    GetPrimaryView()->Enable(FALSE);
    SetFocusedView(GetSecondaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerJewelerScreen::OnCraftingResults(const CraftingResultsMessage * pMsg)  // line 992
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CRAFT), cgtPSODefaults);
    m_eCraftState = FINISHED_CRAFTING_STATE;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerJewelerScreen::OnTrainingResults(const IntDataMessage * pMsg)  // line 1003
{
    m_tTrainingItemsProvider.Refresh();
    m_tRecipeItemsProvider.Refresh();
    m_tTrainingListView.ForceUpdate(RESET_SELECTION);
    m_tTrainingDetailsView.ForceUpdate(RESET_SELECTION);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::SetGemToSocket(const ACDID idGem)
{
    m_eCraftState = BROWSE_ITEMS_TO_SOCKET_STATE;
    m_tSocketView.SetJewel(idGem);
    ActorCommonData * ptACDGem = ACDTryToGet(idGem);
    if (ptACDGem != NULL && ItemIsUpgradeableJewel(*ptACDGem))
    {
        m_tPlayerItemsProvider.SetFilter(FILTER_CAN_SOCKET_UPGRADEABLE_JEWEL);
    }
    else
    {
        m_tPlayerItemsProvider.SetFilter(FILTER_AVAILABLE_SOCKETS);
    }
    m_tPlayerItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
    m_tPlayerItemsProvider.Refresh();
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    if (m_tPlayerItemsProvider.GetItems().size() == 0)
    {
        SetBrowsingMode(BROWSE_DETAILS);
    }
    else
    {
        SetBrowsingMode(BROWSE_LIST);
    }
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::BackUpSocketingFlow()
{
    switch (m_eCraftState)
    {
    case BROWSE_GEMS_STATE:
        m_eCraftState = BROWSE_CATEGORY_STATE;
        m_tSocketView.SetJewel(ACDID_INVALID);
        break;
    case BROWSE_ITEMS_TO_SOCKET_STATE:
    case FINISHED_SOCKETING_STATE:
    case READY_TO_SOCKET_AGAIN_STATE:
    {
        SetBrowsingMode(BROWSE_LIST);
        if (m_eCraftState == FINISHED_SOCKETING_STATE || m_eCraftState == READY_TO_SOCKET_AGAIN_STATE)
        {
            m_tSocketView.SetJewel(ACDID_INVALID);
            m_tSocketView.Reset();
        }
        m_tSocketView.SelectItem(ACDID_INVALID);

        ItemCategory eCategory = m_tGemCategoriesView.GetSelectedItem();
        if (CanOpenCategory(eCategory))
        {
            UIFilterType eFilterType = GetFilterTypeForCategory(eCategory, FALSE);
            m_tPlayerItemsProvider.SetFilter(eFilterType);
            m_eCraftState = BROWSE_GEMS_STATE;
        }
        else
        {
            ShowGemCategories();
        }
        m_tPlayerItemsProvider.SetItemOptions(0);
        m_tPlayerItemsProvider.Refresh();
        break;
    }
    default:
        return;
    }

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::CancelSocketing()
{
    m_eCraftState = BROWSE_CATEGORY_STATE;
    m_tSocketView.SetJewel(ACDID_INVALID);
    m_tSocketView.SelectItem(ACDID_INVALID);
    m_tSocketView.Reset();
    m_tPlayerItemsProvider.SetItemOptions(0);
    m_tPlayerItemsProvider.Refresh();
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    GetPrimaryView()->ForceUpdate(RESET_SELECTION);
    SetFocusedView(GetPrimaryView());
    SetBrowsingMode(BROWSE_LIST);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerJewelerScreen::BackUpCombineFlow()  // line 1127
{
    m_eCraftState = BROWSE_ITEMS_STATE;
    m_tCombineGemsView.Reset();
    GetPrimaryView()->Enable(TRUE);
    SetFocusedView(GetPrimaryView());
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::CancelCombineGems()
{
    m_eCraftState = BROWSE_CATEGORY_STATE;
    GetPrimaryView()->Enable(TRUE);
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(GetPrimaryView());
    m_tCombineGemsView.Reset();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::SocketItem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    ACDID idGem = m_tSocketView.GetJewel();
    ACDID idItem = m_tSocketView.GetSelectedItem();
    ActorCommonData * ptACDGem = ACDTryToGet(idGem);
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDGem == NULL || ptACDItem == NULL)
    {
        return;
    }

    if (ACDIsPlayer(ptPlayer->idPlayerACD) && ItemHasSockets(*ptACDItem))
    {
        SocketReason eReason = ItemCanSocketInto(*ptACDGem, *ptACDItem);
        switch (eReason)
        {
        case SOCKET_REASON_SUCCESS:
            m_tSocketView.SocketItem();
            SetFocusedView(GetSecondaryView());
            PlayItemInventorySound(*ptACDGem);
            CACDInventoryRequestToSocket(*ptACDGem, *ptACDItem, ptPlayer);
            m_eCraftState = BUSY_CRAFTING_STATE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            break;
        case SOCKET_REASON_LEVEL_RESTRICTION_NOT_MET:
        {
            GameErrorData tError;
            tError.eError = GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_LEVEL;
            tError.dwVal = 0;
            UIGame::DisplayError(tError, FALSE, ptPlayer);
            break;
        }
        }
    }
}

inline void PlayerJewelerScreen::CloseSocketUI()  // line 1201
{
    CancelSocketing();
}

void PlayerJewelerScreen::RemoveGem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
    if (!sHasRoomToUnsocketGems(ptPlayer, idSelectedItem))
    {
        NotificationCreationParams tParams;
        tParams.uszMessage = StringListFetch("ConsoleErrors:NoRoomForUnsocketedGem").str();
        tParams.nControllerIndex = GetPlayer()->nControllerIndex;
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return;
    }

    if (CanAffordToRemoveGem(idSelectedItem))
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_REMOVEGEM), cgtPSODefaults);
        ANNDataMessage tMsg;
        tMsg.ann = ANNFromACD(idSelectedItem);
        MessageSendToServer(CMSG_TRY_REMOVE_GEMS, &tMsg, sizeof(tMsg), ptPlayer);
        GetPrimaryView()->Enable(FALSE);
        SetFocusedView(NULL);
        m_eCraftState = BUSY_CRAFTING_STATE;
        m_tRemoveGemsView.StartRemoving();
    }
}

inline void PlayerJewelerScreen::CloseRemoveGemUI()  // line 1246
{
    m_eCraftState = READY_TO_CRAFT_STATE;
    m_tRemoveGemsView.Reset();
}

inline void PlayerJewelerScreen::Train()  // line 1256
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRAIN), cgtPSODefaults);
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_VENDOR_TRY_TRAIN, &tMessage, sizeof(tMessage), GetPlayer());
}

void PlayerJewelerScreen::OnButton1()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction)
    {
    case ACTION_OPEN_CATEGORY:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        OpenSelectedCategory();
        break;
    case ACTION_SELECT_GEM:
        SelectGem();
        break;
    case ACTION_COMBINE:
        CombineGems();
        break;
    case ACTION_ACCEPT_COMBINE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        BackUpCombineFlow();
        m_tRecipeItemsProvider.Refresh();
        GetPrimaryView()->RequestUpdate(KEEP_ITEM_SELECTED);
        break;
    case ACTION_BEGIN_SOCKET:
        if (UIUtils::Console::ShowTieredRiftMessage(GetPlayer()))
        {
            break;
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        SetGemToSocket(GetPrimaryView()->GetSelectedItem());
        break;
    case ACTION_END_SOCKET:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        SocketItem();
        break;
    case ACTION_ACCEPT_SOCKETING:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        BackUpSocketingFlow();
        break;
    case ACTION_REMOVE:
        RemoveGem();
        break;
    case ACTION_ACCEPT_REMOVE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        CloseRemoveGemUI();
        GetPrimaryView()->Enable(TRUE);
        SetFocusedView(GetPrimaryView());
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        break;
    case ACTION_TRAIN:
        Train();
        break;
    }
}

void PlayerJewelerScreen::OnButton2()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    switch (idAction)
    {
    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        break;
    case ACTION_BACK:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        switch (GetJewelerMode())
        {
        case JEWELER_MODE_SOCKET:
            if (m_eCraftState == FINISHED_SOCKETING_STATE)
            {
                BackUpSocketingFlow();
            }
            BackUpSocketingFlow();
            break;
        case JEWELER_MODE_COMBINE:
            switch (m_eCraftState)
            {
            case READY_TO_CRAFT_STATE:
                BackUpCombineFlow();
                break;
            case FINISHED_CRAFTING_STATE:
                BackUpCombineFlow();
            case BROWSE_ITEMS_STATE:
                SetBrowsingMode(BROWSE_LIST);
                CancelCombineGems();
                break;
            }
            break;
        }
        break;
    }
}

void PlayerJewelerScreen::OnButton3()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    switch (idAction)
    {
    case ACTION_DETAILS:
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        SetBrowsingMode(BROWSE_DETAILS);
        GetPrimaryView()->SelectItem(idSelectedItem);
        break;
    }
    case ACTION_LIST:
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
        SetBrowsingMode(BROWSE_LIST);
        GetPrimaryView()->SelectItem(idSelectedItem);
        break;
    }
    }
}

void PlayerJewelerScreen::OnButton4()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    switch (idAction)
    {
    case ACTION_FILTER_ALL:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SetRecipeListFilter(RECIPE_FILTER_ALL);
        break;
    case ACTION_FILTER_MATERIALS:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        SetRecipeListFilter(RECIPE_FILTER_MATERIALS);
        break;
    }
}

inline void PlayerJewelerScreen::OnJewelerScrollUp()  // line 1430
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive() &&
        ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(sgptJewelerGlobals->GetControllerIndex(), BUTTON_RIGHT_TRIGGER))
    {
        sgptJewelerGlobals->ScrollDetails(-SCROLLBOX_SCROLLDELTA);
    }
}

inline void PlayerJewelerScreen::OnJewelerScrollDown()  // line 1449
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive() &&
        ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(sgptJewelerGlobals->GetControllerIndex(), BUTTON_LEFT_TRIGGER))
    {
        sgptJewelerGlobals->ScrollDetails(SCROLLBOX_SCROLLDELTA);
    }
}

inline void PlayerJewelerScreen::UpdateCategoryUI(const UIID & idItemUI, const ItemCategory eCategory)  // line 1468
{
    BOOL bIsEnabled = FALSE;
    JewelerMode eMode = GetJewelerMode();
    if (eMode != JEWELER_MODE_COMBINE && eMode != JEWELER_MODE_SOCKET || CanOpenCategory(eCategory)) bIsEnabled = TRUE;  // Inference: DWARF bIsEnabled (1472) is constant 0 then 1, set inside an if; 1474 col 47 is the `&&` with no leading parenthesis.
    UpdateCommonCategoryUI(idItemUI, eCategory, bIsEnabled, FALSE, FALSE);
}
BOOL PlayerJewelerScreen::CanAffordToRemoveGem(ACDID idSelectedItem)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem != NULL)
    {
        ActorCommonData & tACDPlayer = ACDGet(GetPlayer()->idPlayerACD);
        int32 nCost = ItemCrafting_GetGemRemovalCost(*ptACDItem);
        return tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD);
    }
    return FALSE;
}

void PlayerJewelerScreen::UpdateGemCategories()
{
    m_tGemCategoriesView.ClearCategories();

    m_tGemCategoriesView.AddCategory(CATEGORY_UPGRADEABLE_JEWEL);
    m_tGemCategoriesView.AddCategory(CATEGORY_DIAMOND);
    m_tGemCategoriesView.AddCategory(CATEGORY_RUBY);
    m_tGemCategoriesView.AddCategory(CATEGORY_TOPAZ);
    m_tGemCategoriesView.AddCategory(CATEGORY_EMERALD);
    m_tGemCategoriesView.AddCategory(CATEGORY_AMETHYST);
}

void PlayerJewelerScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
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
        OnJewelerScrollUp();
        break;
    case BUTTON_RIGHT_TRIGGER:
        OnJewelerScrollDown();
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

BOOL PlayerJewelerScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerJewelerScreen::OnOpen()
{
    Player * ptPlayer = GetPlayer();
    ptPlayer->eActiveCrafterType = CRAFTER_JEWELER;
    m_tPlayerItemsProvider.SetPlayer(ptPlayer);
    m_tRecipeItemsProvider.SetPlayer(ptPlayer);
    m_tSingleItemProvider.SetPlayer(ptPlayer);
    m_tTrainingItemsProvider.SetPlayer(ptPlayer);
    SetJewelerMode(JEWELER_MODE_COMBINE);
    m_eCraftState = BROWSE_CATEGORY_STATE;
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    ShowJewelerModel(*ptPlayer);
    UpdateRingIcons();
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText("", 0);

    UIRActor & tHero = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
    LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHero.SetLight(idLight);
}

void PlayerJewelerScreen::OnClose()
{
    ResetCurrencyAnimations(UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED);
    m_tRecipeItemsProvider.SetPlayer(NULL);
    m_tRecipesListView.ForceUpdate(RESET_SELECTION);
    m_tRecipesDetailsView.ForceUpdate(RESET_SELECTION);
    SetJewelerMode(JEWELER_MODE_INVALID);
    SetRecipeListFilter(RECIPE_FILTER_INVALID);
    m_tPlayerItemsProvider.SetPlayer(NULL);
    m_tSingleItemProvider.SetPlayer(NULL);
    m_tTrainingItemsProvider.SetPlayer(NULL);
    Player * ptPlayer = GetPlayer();
    ptPlayer->idACDActiveVendor = ACDID_INVALID;
    ptPlayer->eActiveCrafterType = CRAFTER_INVALID;
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_CLOSE_INTERACTION, &tMessage, sizeof(tMessage), ptPlayer);
}

void PlayerJewelerScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    switch (m_eJewelerMode)
    {
    case JEWELER_MODE_COMBINE:
        UpdateCombineActions(tActionManager);
        break;
    case JEWELER_MODE_SOCKET:
        UpdateSocketingActions(tActionManager);
        break;
    case JEWELER_MODE_REMOVE:
        UpdateRemoveActions(tActionManager);
        break;
    case JEWELER_MODE_TRAIN:
        UpdateTrainingActions(tActionManager);
        break;
    }
}

inline void PlayerJewelerScreen::AddFilterActions(::Console::ControllerActionManager & tActionManager)  // line 1684
{
    ::Console::ControllerActionId idAction = ::Console::ACTION_NONE;
    switch (m_eRecipeFilter)
    {
    case RECIPE_FILTER_ALL:
        idAction = ACTION_FILTER_MATERIALS;
        break;
    case RECIPE_FILTER_MATERIALS:
        idAction = ACTION_FILTER_ALL;
        break;
    }
    if (idAction != ::Console::ACTION_NONE)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, idAction, StringListFetch("ConsoleArtisan:ChangeFilter").str());
    }
}

void PlayerJewelerScreen::UpdateCombineActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eCraftState)
    {
    case BROWSE_CATEGORY_STATE:
        if (CanOpenSelectedCategory())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        AddFilterActions(tActionManager);
        return;
    case BROWSE_ITEMS_STATE:
        if (CanCombineGems())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT_GEM, StringListFetch("ConsoleCrafting:Select").str());
        }
        if (GetBrowsingMode() == BROWSE_LIST)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_DETAILS, StringListFetch("ConsoleUI:Details").str());
        }
        else
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_LIST, StringListFetch("ConsoleUI:List").str());
        }
        AddFilterActions(tActionManager);
        break;
    case READY_TO_CRAFT_STATE:
        if (CanCombineGems())
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_COMBINE, StringListFetch("ConsoleCrafting:Combine").str());
        }
        break;
    case BUSY_CRAFTING_STATE:
    case WAITING_FOR_RESULTS_STATE:
        return;
    case FINISHED_CRAFTING_STATE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_COMBINE, StringListFetch("H2OLayout:Continue").str());
        break;
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
}

void PlayerJewelerScreen::UpdateSocketingActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eCraftState)
    {
    case BROWSE_CATEGORY_STATE:
    {
        ItemCategory eCategory = m_tGemCategoriesView.GetSelectedItem();
        if (CanOpenCategory(eCategory))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_CATEGORY, StringListFetch("ConsoleUI:Open").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        return;
    }
    case FINISHED_SOCKETING_STATE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_SOCKETING, StringListFetch("H2OLayout:Continue").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
        return;
    case BROWSE_GEMS_STATE:
        if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BEGIN_SOCKET, StringListFetch("ConsoleSocketing:SocketButton").str());
            if (CanScrollDetails())
            {
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
            }
        }
        break;
    case BROWSE_ITEMS_TO_SOCKET_STATE:
    case READY_TO_SOCKET_AGAIN_STATE:
        if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_END_SOCKET, StringListFetch("ConsoleSocketing:SocketButton").str());
        }
        break;
    }

    if (m_eCraftState != BUSY_CRAFTING_STATE)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleSocketing:BackButton").str());
    }
}

void PlayerJewelerScreen::UpdateRemoveActions(::Console::ControllerActionManager & tActionManager)
{
    switch (m_eCraftState)
    {
    case READY_TO_CRAFT_STATE:
        if (CanAffordToRemoveGem(GetPrimaryView()->GetSelectedItem()))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_REMOVE, StringListFetch("ConsoleCrafting:Remove").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        break;
    case FINISHED_CRAFTING_STATE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT_REMOVE, StringListFetch("H2OLayout:Continue").str());
        break;
    }
}

void PlayerJewelerScreen::UpdateTrainingActions(::Console::ControllerActionManager & tActionManager)
{
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
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
}

void PlayerJewelerScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "Jeweler");
        UpdateNavBar();
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(FALSE);
        m_tPlayerItemsProvider.SetItemOptions((1 << InventoryItems::INCLUDE_EQUIPPED_ITEMS) | (1 << InventoryItems::EXCLUDE_UNIDENTIFIED_ITEMS));
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
        OpenSelectedSlot();
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
    UIID idStats;
    idStats.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "CharacterStats");
    UI_GetWindowFromID(idStats).SetActive(!bActive);
}

void PlayerJewelerScreen::OnUpdate()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_PANEL_FOCUS_HIGHLIGHT).SetActive(!RingHasFocus() || m_eJewelerMode == JEWELER_MODE_REMOVE);
    UpdateCurrencyDisplay(UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED, CURRENCY_GOLD);
    switch (m_eJewelerMode)
    {
    case JEWELER_MODE_COMBINE:
        UpdateCombineState();
        UpdateCombineUI();
        break;
    case JEWELER_MODE_SOCKET:
        UpdateSocketingState();
        UpdateSocketingUI();
        break;
    case JEWELER_MODE_REMOVE:
        UpdateRemoveGemState();
        UpdateRemoveGemUI();
        break;
    case JEWELER_MODE_TRAIN:
        UpdateTrainingUI();
        break;
    }
    ClearStatusLine(GetPrimaryView());
    ClearStatusLine(GetSecondaryView());
}

void PlayerJewelerScreen::OnRequestedUpdate()
{
    m_tPlayerItemsProvider.Refresh();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerJewelerScreen::InitializeRingWrapper(::Console::RingWrapper &)
{
}

UIFilterType PlayerJewelerScreen::GetDefaultRingSelection() const
{
    return FILTER_NONE;
}

void PlayerJewelerScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    m_tPlayerItemsProvider.SetFilter(eNewFilterType);
    RefreshProviders(RESET_SELECTION);
}

inline BOOL PlayerJewelerScreen::CanCombineGems()  // line 1988
{
    BOOL bCanCombine = FALSE;
    if (GetJewelerMode() == JEWELER_MODE_COMBINE)
    {
        int32 nIndex = m_tRecipeItemsProvider.GetIndexOfItem(GetPrimaryView()->GetSelectedItem());
        if (nIndex != -1)
        {
            GBID gbidRecipe = m_tRecipeItemsProvider.GetRecipes()[nIndex];
            bCanCombine = ItemCrafting_CanCraftItem(gbidRecipe, CRAFTER_JEWELER, GetPlayer());
        }
    }
    return bCanCombine;
}

inline BOOL PlayerJewelerScreen::CanOpenSelectedCategory()  // line 2014
{
    ItemCategory eCategory = m_tGemCategoriesView.GetSelectedItem();
    return CanOpenCategory(eCategory);
}

inline BOOL PlayerJewelerScreen::CanOpenCategory(const ItemCategory eCategory)  // line 2024; reads m_eJewelerMode directly: its linker-removed concrete DIE has no GetJewelerMode inline
{
    if (m_eJewelerMode == JEWELER_MODE_COMBINE)
    {
        return CategoryHasItems(eCategory, m_tRecipeItemsProvider, TRUE);
    }
    return CategoryHasItems(eCategory, m_tPlayerItemsProvider, FALSE);
}

inline void PlayerJewelerScreen::ShowGemCategories()  // line 2040
{
    m_eCraftState = BROWSE_CATEGORY_STATE;
    PopulateGemCategories(m_tGemCategoriesView);
    if (GetPrimaryView() != &m_tGemCategoriesView)
    {
        SetPrimaryView(&m_tGemCategoriesView);
    }
    else
    {
        m_tGemCategoriesView.ForceUpdate(RESET_SELECTION);
    }
}

inline void PlayerJewelerScreen::RefreshProviders(const MaintainSelectionOptions eSelectionType)  // line 2059
{
    m_tPlayerItemsProvider.Refresh();
    UIInventoryView * ptPrimaryView = GetPrimaryView();
    if (ptPrimaryView != NULL)
    {
        ptPrimaryView->ForceUpdate(eSelectionType);
    }
    UIInventoryView * ptSecondaryView = GetSecondaryView();
    if (ptSecondaryView != NULL)
    {
        ptSecondaryView->ForceUpdate(eSelectionType);
    }
}

void PlayerJewelerScreen::InitRingFilters(UIIDToFilterMap &tFilterMap)
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

UIInventoryView * PlayerJewelerScreen::GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
{
    JewelerMode eJewelerMode = GetJewelerMode();
    switch (eJewelerMode)
    {
    case JEWELER_MODE_COMBINE:
        if (m_eCraftState == BROWSE_CATEGORY_STATE)
        {
            return &m_tGemCategoriesView;
        }
        if (eBrowsingMode == BROWSE_LIST)
        {
            return &m_tRecipesListView;
        }
        return &m_tRecipesDetailsView;
    case JEWELER_MODE_SOCKET:
        if (m_eCraftState == BROWSE_CATEGORY_STATE)
        {
            return &m_tGemCategoriesView;
        }
        switch (eBrowsingMode)
        {
        case BROWSE_LIST:
            return &m_tPlayerListView;
        case BROWSE_DETAILS:
            return &m_tPlayerDetailsView;
        }
        return NULL;
    case JEWELER_MODE_REMOVE:
        switch (eBrowsingMode)
        {
        case BROWSE_LIST:
            return &m_tPlayerListView;
        case BROWSE_DETAILS:
            return &m_tPlayerDetailsView;
        }
        return NULL;
    case JEWELER_MODE_TRAIN:
        if (eBrowsingMode == BROWSE_LIST)
        {
            return &m_tTrainingListView;
        }
        return &m_tTrainingDetailsView;
    }
    return NULL;
}

UIInventoryView * PlayerJewelerScreen::GetSecondaryViewForMode(const BrowsingMode)
{
    JewelerMode eJewelerMode = GetJewelerMode();
    switch (eJewelerMode)
    {
    case JEWELER_MODE_COMBINE:
        if (m_eCraftState == BROWSE_CATEGORY_STATE)
        {
            return &m_tCategorySummaryView;
        }
        return &m_tCombineGemsView;
    case JEWELER_MODE_SOCKET:
        if (m_eCraftState == BROWSE_CATEGORY_STATE)
        {
            return &m_tCategorySummaryView;
        }
        return &m_tSocketView;
    case JEWELER_MODE_REMOVE:
        return &m_tRemoveGemsView;
    case JEWELER_MODE_TRAIN:
        return &m_tTrainingView;
    }
    return NULL;
}

BOOL PlayerJewelerScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    return eFilterType == FILTER_BAG;
}

void OnJewelerUpdate()
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->Update();
    }
}

void OnJewelerLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnJewelerRightStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->HandleRightStickInput(nControllerIndex);
    }
}

void OnJewelerButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}

}  // namespace Console
}  // namespace UIInventory

namespace UIJeweler {
namespace Console {
using namespace UIInventory::Console;

void InitializeForGame()
{
    if (!GameIsStartUp())
    {
        sgptJewelerGlobals = new PlayerJewelerScreen();
    }
}

void DisposeForGame()
{
    if (!GameIsStartUp())
    {
        ::delete sgptJewelerGlobals;
        sgptJewelerGlobals = NULL;
    }
}

void OpenForController(int32 nControllerIndex)
{
    sgptJewelerGlobals->OpenForController(nControllerIndex);
}

void Close()
{
    if (sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->Close();
    }
}

void OpenCombineScreen(int32 nControllerIndex)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->SetJewelerMode(PlayerJewelerScreen::JEWELER_MODE_COMBINE);
    }
}

void OpenSocketScreen(int32 nControllerIndex)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->SetJewelerMode(PlayerJewelerScreen::JEWELER_MODE_SOCKET);
    }
}

void CloseSocketScreen()
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->CloseSocketUI();
    }
}

void OpenRemoveScreen(int32 nControllerIndex)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->SetJewelerMode(PlayerJewelerScreen::JEWELER_MODE_REMOVE);
    }
}

void CloseRemoveScreen()
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->CloseRemoveGemUI();
    }
}

void OpenTrainScreen(int32 nControllerIndex)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->SetJewelerMode(PlayerJewelerScreen::JEWELER_MODE_TRAIN);
    }
}

void OnCraftingResults(const CraftingResultsMessage * pMsg)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->OnCraftingResults(pMsg);
    }
}

void OnTrainingResults(const IntDataMessage * pMsg)
{
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->OnTrainingResults(pMsg);
    }
}

void OnJewelerChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)
{
    if (sgptJewelerGlobals == NULL || !sgptJewelerGlobals->IsActive())
    {
        return;
    }
    if (!bForce && tOldLoc.eActorLocation == tNewLoc.eActorLocation && tNewLoc.tInvLocation.eSlot == tOldLoc.tInvLocation.eSlot)
    {
        return;
    }
    Player * ptPlayer = sgptJewelerGlobals->GetPlayer();
    if (ptPlayer == NULL || tOldLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD || tNewLoc.tInvLocation.idACDContainer == ptPlayer->idPlayerACD)
    {
        sgptJewelerGlobals->RequestUpdate(KEEP_ITEM_SELECTED);
    }
}

}  // namespace Console
}  // namespace UIJeweler

namespace {

void OnUpdateCategoryUI(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory)
{
    using namespace UIInventory::Console;
    if (sgptJewelerGlobals != NULL && sgptJewelerGlobals->IsActive())
    {
        sgptJewelerGlobals->UpdateCategoryUI(idItemUI, eCategory);
    }
}

void UpdateGemRemovalPrice(const UIID & idPriceDisplay, const ACDID idItem)  // line 2505
{
    using namespace UIInventory::Console;
    if (sgptJewelerGlobals == NULL || !sgptJewelerGlobals->IsActive())
    {
        return;
    }

    Player * ptPlayer = sgptJewelerGlobals->GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    CRefString sText;
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDItem != NULL)
    {
        int32 nCost = ItemCrafting_GetGemRemovalCost(*ptACDItem);
        CRefString sPriceText = UIText_ValueToGameCurrency(nCost, CURRENCY_GOLD);
        BOOL bCanAfford = TRUE;
        if (nCost > 0)
        {
            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            bCanAfford = tACDPlayer.CanAffordCurrencyCost(nCost, CURRENCY_GOLD);
        }
        Set_Text_Color(sPriceText, bCanAfford ? FONTCOLOR_WHITE : FONTCOLOR_RED);
        sText = LocalizedSprintf(StringListFetch("ConsoleArtisan:GemRemovalPrice").str(), sPriceText.str(), NULL);
    }
    UI_GetTextFromID(idPriceDisplay).SetText(sText.str(), 0);
}

bool sHasRoomToUnsocketGems(const Player * ptPlayer, const ACDID idItem)  // line 2567
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDPlayer != NULL && ptACDItem != NULL)
    {
        int32 nGemCount = ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS_FILLED));
        int32 nUsedSpace = ACDInventoryGetNumFilledSlots(*ptACDPlayer, SLOT_PLAYER_BACKPACK);
        IVector2D tSlotSize;
        ACDInventorySlotGetSize(SLOT_PLAYER_BACKPACK, tSlotSize);
        int32 nTotalSpace = tSlotSize.x * tSlotSize.y;
        return nUsedSpace + nGemCount <= nTotalSpace;
    }
    return false;
}

}  // namespace
