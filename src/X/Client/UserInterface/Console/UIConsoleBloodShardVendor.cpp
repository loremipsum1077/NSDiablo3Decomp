// Original file: X/Client/UserInterface/Console/UIConsoleBloodShardVendor.cpp
// The console blood shard vendor (Kadala) screen: buy a mystery item, reveal it, equip it.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Console/ControllerUtils.h"
#include "ConsoleUIRActorManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UIRActorControl.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryListView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../RActors/RActors.h"
#include "../../../Common/Monsters.h"
#include "../../../Common/TagMaps/AnimTagsSwig.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../CGame.h"
#include "../../Input/Input.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Crafting/ItemCraftingSwig.h
enum {
    CRAFTER_INVALID = -1,
};
// X/Common/Networking/GameMessage.h
enum {
    CMSG_CLOSE_INTERACTION = 22,

};
// X/Common/Player.h




// X/Client/UserInterface/Interface/UIGame.cpp




namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Common/ActorInventory.cpp
BuyResponse ACDInventoryCanBuy(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);
void ACDInventoryWhereCanThisGo(const ActorCommonData & tACDPlayer, const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots);
SNO ActorInventory_GetItemLandingSound(const ActorCommonData & tACDItem);  // DWARF: X/Common/ActorInventory.cpp:12327; no header declaration recorded.
// X/Common/Items.cpp
BOOL ItemIsLegendary(GBID gbidItem);
BOOL ItemIsEquippable(const ActorCommonData & tACDItem);
// X/Client/UserInterface/Controls/UIItemButtonControl.cpp
typedef int32 UIItemOverlay;

UIItemOverlay ItemGetOverlay(const ActorCommonData & tACDItem, const ActorCommonData & tACDPlayer);
// X/Client/Input/Input.cpp
void InputClearRemappings();
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIInventory {
void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD);
void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer);
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
enum InfoPanelType {
    VENDOR_INFO_PANEL = 9,
};
void ClearStatusLine(UIInventoryView * ptView);
void SwitchToInfoPanel(const InfoPanelType eInfoPanel);
void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Interface/UITutorial.cpp
struct UITutorial;
#include "../Interface/UITutorials.h"




extern UITutorialManager gtTutorialMgr;
// X/Client/UserInterface/UI.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
// X/Client/Render/Light.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Common/Definitions/ActorDefinition.cpp
SNO ActorGetAnimSetSNO(const SNO snoActor);
// X/Client/RActors/RActorAnimation.cpp
BOOL RActorPlayAnimation(RActor * ptRActor, const uint32 dwTag, BOOL bDisableBlending, BOOL fOnlyPlayIfDifferent);
// X/Client/UserInterface/UIGlobals.cpp
extern const UIID UIDLG_INVENTORY_CONSOLE, UIDLG_INVENTORY_CONSOLE_LIST, UIDLG_INVENTORY_CONSOLE_DETAILS,
    UIDLG_INVENTORY_CONSOLE_EQUIPPED;
extern const UIID UIRACTOR_CONSOLE_RING, UITXT_CONSOLE_PLAYER_LABEL_RING; extern const UIID UIBTN_INVENTORY_CONSOLE_RING_ARM_LEFT, UIBTN_INVENTORY_CONSOLE_RING_ARM_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_BAG, UIBTN_INVENTORY_CONSOLE_RING_BRACERS, UIBTN_INVENTORY_CONSOLE_RING_FEET, UIBTN_INVENTORY_CONSOLE_RING_FINGER_LEFT, UIBTN_INVENTORY_CONSOLE_RING_FINGER_RIGHT, UIBTN_INVENTORY_CONSOLE_RING_HANDS, UIBTN_INVENTORY_CONSOLE_RING_HEAD, UIBTN_INVENTORY_CONSOLE_RING_LEGS, UIBTN_INVENTORY_CONSOLE_RING_NECK, UIBTN_INVENTORY_CONSOLE_RING_SHOULDER, UIBTN_INVENTORY_CONSOLE_RING_TORSO, UIBTN_INVENTORY_CONSOLE_RING_WAIST;
extern const UIID UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, UIWND_INVENTORY_CONSOLE_RING_GLOW,
    UIWND_INVENTORY_CONSOLE_ROOT, UIWND_INVENTORY_CONSOLE_VENDOR_PANEL,
    UIWND_INVENTORY_CONSOLE_BLOODSHARD_VENDOR_CONTAINER, UIWND_BLOODSHARD_VENDOR_CONSOLE_BACKGROUND,
    UIWND_BLOODSHARD_VENDOR_CONSOLE_FOCUS_HIGHLIGHT;
extern UIID UITXT_INVENTORY_CONSOLE_HEADER, UITXT_INVENTORY_CONSOLE_GOLD; extern const UIID UITXT_INVENTORY_CONSOLE_GOLD_SPENT,  // DWARF: UIGlobals.cpp:3144 (HEADER), :3139 (GOLD); no header declarations recorded.
    UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED,
    UITXT_INVENTORY_CONSOLE_BLOODSHARD_VENDOR_NAME;

namespace {
void UpdatePrice(UIInventory::Console::UIInventoryView * ptView);
BOOL RingIncludeControlPredicate(const UIID & idControl);
void HideRingNewItemIndicators();
}  // namespace

namespace UIInventory {
namespace Console {
enum ButtonActions {
    ACTION_CHOOSE_SLOT = 1,
    ACTION_OPEN_SLOT = 2,
    ACTION_CLOSE_SLOT = 3,
    ACTION_SCROLL_LIST = 4,
    ACTION_CLOSE = 5,
    ACTION_BUY = 6,
    ACTION_CANCEL = 7,
    ACTION_CONTINUE = 8,
    ACTION_EQUIP = 9,
    ACTION_FINISH_PURCHASE = 10,
};

static BOOL sShouldShowItemInFilter(const GBID gbidItem, const UIFilterType eFilterType)  // line 75
{
    // Original rows 79-125 (one per filter, four lines apart; columns fit `eFilterType == FILTER_X && ItemHasLabel(...)`):
    // an if chain the compiler turned into the jump table (inferred from the line table).
    if (eFilterType == FILTER_MAIN_HAND && ItemHasLabel(gbidItem, ITL_WEAPON))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_OFF_HAND &&
        (ItemHasLabel(gbidItem, ITL_WEAPON) || ItemHasLabel(gbidItem, ITL_SHIELD) || ItemHasLabel(gbidItem, ITL_OFFHAND)))
    {
        return TRUE;
    }
    // Order of the two ring tests inferred (the jump table hides it).
    if ((eFilterType == FILTER_RING_LEFT || eFilterType == FILTER_RING_RIGHT) &&
        ItemHasLabel(gbidItem, ITL_RING))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_NECK && ItemHasLabel(gbidItem, ITL_AMULET))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_HANDS && ItemHasLabel(gbidItem, ITL_GLOVES))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_SHOULDERS && ItemHasLabel(gbidItem, ITL_SHOULDERS))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_CHEST && ItemHasLabel(gbidItem, ITL_CHESTARMOR))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_HEAD && ItemHasLabel(gbidItem, ITL_HELM))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_BELT && ItemHasLabel(gbidItem, ITL_BELT))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_LEGS && ItemHasLabel(gbidItem, ITL_LEGS))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_FEET && ItemHasLabel(gbidItem, ITL_BOOTS))
    {
        return TRUE;
    }
    if (eFilterType == FILTER_WRIST && ItemHasLabel(gbidItem, ITL_BRACERS))
    {
        return TRUE;
    }
    return FALSE;
}

static BOOL sItemIteratorCallback(const InventoryLocation & tInvLoc, void * pCallBackData)  // line 136
{
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem != NULL)
    {
        ACDIDArray * parFilteredItems = (ACDIDArray *)pCallBackData;
        for (int32 i = FILTER_PLAYER_START; i < FILTER_PLAYER_END; ++i)
        {
            if (sShouldShowItemInFilter(ptACDItem->hGB.gbid, (UIFilterType)i))
            {
                parFilteredItems[i].push_back(idACDItem);
            }
        }
    }
    return TRUE;
}

class BloodShardVendorItems : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x170, line 163
public:
    virtual ProviderType GetProviderType() const  // vtable slot 7, line 166
    {
        return ITEM_PROVIDER_BLOODSHARD_VENDOR;
    }

    virtual CRefString GetItemName(const int32 nIndex) const  // vtable slot 2, line 172
    {
        ActorCommonData * ptACDItem = ACDTryToGet(GetItems()[nIndex]);
        if (ptACDItem != NULL)
        {
            return ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, TRUE);
        }
        return CRefString("");
    }

    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const  // vtable slot 6, line 184
    {
        static ACDIDArray arNone;
        if (eFilterType < FILTER_PLAYER_START || eFilterType >= FILTER_PLAYER_END)
        {
            return arNone;
        }
        return m_arFilteredItems[eFilterType];
    }

    void Refresh()  // line 196
    {
        Player * ptPlayer = GetPlayer();
        ClearItems();
        if (ptPlayer != NULL)
        {
            ActorCommonData & tACDInvSource = ACDGet(ptPlayer->idACDActiveVendor);
            ACDInventoryIterateSlot(tACDInvSource, SLOT_MERCHANT_SHOPLIST, NULL, sItemIteratorCallback, m_arFilteredItems, FALSE, FALSE);
        }
    }

private:
    virtual void OnPlayerChanged()  // vtable slot 14, line 212
    {
        Refresh();
    }

    void ClearItems()  // line 218
    {
        for (int32 i = 0; i < 14; ++i)
        {
            m_arFilteredItems[i].clear();
        }
    }

    ACDIDArray m_arFilteredItems[14];  // 0x20
};
static_assert(sizeof(BloodShardVendorItems) == 0x170, "BloodShardVendorItems");

class MysteryItem : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x40
public:
    enum PurchaseStage {
        BEFORE_PURCHASE = 0,
        STARTED_PURCHASE = 1,
        FINISHED_PURCHASE = 2,
    };

    virtual ~MysteryItem() {}  // line 234

    MysteryItem()  // line 237
        : m_eStage(BEFORE_PURCHASE)
    {
        m_aridItem.push_back(ACDID_INVALID);
    }

    void OnPurchase(const ACDID idMysteryItem)  // line 245
    {
        m_eStage = STARTED_PURCHASE;
        m_aridItem[0] = idMysteryItem;
    }

    void OnPurchaseFinished(const ACDID idPurchasedItem)  // line 254
    {
        m_eStage = FINISHED_PURCHASE;
        m_aridItem[0] = idPurchasedItem;
    }

    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const  // vtable slot 6, line 261
    {
        return m_aridItem;
    }

    virtual ProviderType GetProviderType() const  // vtable slot 7, line 267
    {
        if (m_eStage == FINISHED_PURCHASE)
        {
            return ITEM_PROVIDER_REAL_ITEMS;
        }
        return ITEM_PROVIDER_BLOODSHARD_VENDOR;
    }

    virtual BOOL CanProvideUnidentifiedItems() const  // vtable slot 8, line 280
    {
        return FALSE;
    }

    virtual CRefString GetItemName(const int32 nIndex) const  // vtable slot 2, line 286
    {
        ActorCommonData * ptACDItem = ACDTryToGet(m_aridItem[0]);
        if (ptACDItem != NULL)
        {
            return ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, TRUE);
        }
        return CRefString("");
    }

private:
    ACDIDArray m_aridItem;  // 0x20
    PurchaseStage m_eStage;  // 0x38
};
static_assert(sizeof(MysteryItem) == 0x40, "MysteryItem");

class PlayerBloodShardVendorScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x568
public:
    virtual ~PlayerBloodShardVendorScreen() {}  // line 312

    PlayerBloodShardVendorScreen();
    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);  // vtable slot 9
    void OnItemDelivered(const ANNDataMessage * pMsg);
    virtual void OnInventoryChanged();  // vtable slot 26
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
    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);  // vtable slot 22
    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap);  // vtable slot 21
    virtual UIInventoryView * GetPrimaryViewForMode(const BrowsingMode eBrowsingMode);  // vtable slot 23
    virtual UIInventoryView * GetSecondaryViewForMode(const BrowsingMode);  // vtable slot 24
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);  // vtable slot 20
private:
    static const float CAST_TIME;
    static const uint32 PURCHASING_DURATION;
    enum PurchasingState {
        PURCHASING_STATE_START = 0,
        PURCHASING_STATE_SHOW_PROGRESS = 1,
        PURCHASING_STATE_WAITING_FOR_REPLY = 2,
        PURCHASING_STATE_WAITING_FOR_RESULT = 3,
        PURCHASING_STATE_DONE = 4,
        PURCHASING_STATE_SHOW_RESULTS = 5,
    };
    PurchasingState m_ePurchasingState;  // 0x1f0
    uint32 m_dwPurchasingEndTime;  // 0x1f4
    ACDNetworkName m_annPurchaseResult;  // 0x1f8
    BloodShardVendorItems m_tVendorItemsProvider;  // 0x200
    MysteryItem m_tPurchasedItemProvider;  // 0x370
    EquippedItem m_tEquippedItemProvider;  // 0x3b0
    UIInventoryListView m_tVendorListView;  // 0x3e8
    UIInventoryDetailsView m_tPurchaseView;  // 0x470
    UIInventoryDetailsView m_tEquippedItemView;  // 0x4e8
    ACDID m_idPurchasedItem;  // 0x560
    void BuyItem();
    void ViewPurchasedItem();
    void OnButton1();
    void OnButton2();
    void OnButton3();
    void OnButton4();
    void OnLeftTrigger();
    void OnRightTrigger();
    BOOL CanClose();
    void RefreshProviders(const MaintainSelectionOptions eSelectionType);
    void UpdatePurchasingProgress();
    void UpdateProgressBar();
    void CancelPurchase();
    void HidePurchasingProgress();
};
static_assert(sizeof(PlayerBloodShardVendorScreen) == 0x568, "PlayerBloodShardVendorScreen");

PlayerBloodShardVendorScreen * sgptBloodShardVendorGlobals;  // line 445

const uint32 PlayerBloodShardVendorScreen::PURCHASING_DURATION = 6;
static const float SCROLLBOX_SCROLLDELTA = 4.0f;

void ShowVendorModel(const Player & tPlayer);

PlayerBloodShardVendorScreen::PlayerBloodShardVendorScreen()
    : m_ePurchasingState(PURCHASING_STATE_START)
    , m_dwPurchasingEndTime(0)
    , m_annPurchaseResult(ANN_INVALID)
    , m_tVendorListView(m_tVendorItemsProvider, UIDLG_INVENTORY_CONSOLE_LIST, TRUE)
    , m_tPurchaseView(m_tPurchasedItemProvider, UIDLG_INVENTORY_CONSOLE_DETAILS, TRUE)
    , m_tEquippedItemView(m_tEquippedItemProvider, UIDLG_INVENTORY_CONSOLE_EQUIPPED, FALSE)
    , m_idPurchasedItem(ACDID_INVALID)
{
    m_tVendorListView.SetHeaderText(StringListFetch("ConsoleShop:Vendor"));
    m_tPurchaseView.SetHeaderText(StringListFetch("ConsoleShop:Vendor"));
    m_tEquippedItemView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
}

void PlayerBloodShardVendorScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
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
        break;
    case BUTTON_LEFT_TRIGGER:
        OnLeftTrigger();
        break;
    case BUTTON_RIGHT_TRIGGER:
        OnRightTrigger();
        break;
    default:
        UIInventoryScreen::HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

void PlayerBloodShardVendorScreen::BuyItem()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer != NULL)
    {
        ACDID idACDItem = m_tVendorListView.GetSelectedItem();
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem != NULL)
        {
            BuyResponse eBuyResponse = ACDInventoryCanBuy(tACDPlayer, *ptACDItem);
            switch (eBuyResponse)
            {
            case BUY_RESPONSE_CANT_PICKUP:
            case BUY_RESPONSE_ITEM_NOT_AT_VENDOR:
            {
                GameErrorData tError(GAMEERROR_IARESULT_NOT_ENOUGH_ROOM);
                UIGame::DisplayError(tError, FALSE, ptPlayer);
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_NEGATIVE), cgtPSODefaults);
                break;
            }
            case BUY_RESPONSE_CANT_AFFORD:
            {
                GameErrorData tError(GAMEERROR_INSUFFICIENT_FUNDS_BLOODSHARDS);
                UIGame::DisplayError(tError, FALSE, ptPlayer);
                break;
            }
            default:
            {
                m_tPurchasedItemProvider.OnPurchase(idACDItem);
                SetPrimaryView(&m_tPurchaseView);
                m_tPurchaseView.RequestUpdate(RESET_SELECTION);
                m_ePurchasingState = PURCHASING_STATE_SHOW_PROGRESS;
                m_dwPurchasingEndTime = CGameGetClientTicks() + PURCHASING_DURATION;
                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_SHARD_VENDOR_PROGRESS_BAR), cgtPSODefaults);

                UIID idPurchasing = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.BloodshardVendorWrapper");
                UI_GetWindowFromID(idPurchasing).SetActive(TRUE);
                UIID idAffixes = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.ScrollBox");
                UI_GetWindowFromID(idAffixes).SetActive(FALSE);
                UIID idProgress = UIIDCreateFromParent(idPurchasing, ".BloodshardVendor.ProgressBar");
                UI_GetProgressBarFromID(idProgress).SetExtentsAndPosition(0.0f, 1.0f, 0.0f, -1.0f);
                break;
            }
            }
        }
    }
}

void PlayerBloodShardVendorScreen::ViewPurchasedItem()
{
    m_ePurchasingState = PURCHASING_STATE_SHOW_RESULTS;
    m_tPurchasedItemProvider.OnPurchaseFinished(m_idPurchasedItem);
    m_idPurchasedItem = ACDID_INVALID;
    HidePurchasingProgress();
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
}

inline void PlayerBloodShardVendorScreen::OnButton1()  // line 586
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction)
    {
    case ACTION_OPEN_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SKILLS_SELECT_SLOT), cgtPSODefaults);
        OpenSelectedSlot();
        break;
    case ACTION_BUY:
        BuyItem();
        break;
    case ACTION_CONTINUE:
        ViewPurchasedItem();
        break;
    }
}

void PlayerBloodShardVendorScreen::OnButton2()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    switch (idAction)
    {
    case ACTION_CLOSE_SLOT:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CloseSelectedSlot();
        break;
    case ACTION_CLOSE:
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
        break;
    case ACTION_CANCEL:
    case ACTION_FINISH_PURCHASE:
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
        CancelPurchase();
        break;
    }
}

void PlayerBloodShardVendorScreen::OnButton3()
{
    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    if (idAction == ACTION_EQUIP)
    {
        Player * ptPlayer = GetPlayer();
        if (ptPlayer == NULL)
        {
            return;
        }
        ACDID idItem = m_tPurchasedItemProvider.GetItems()[0];
        if (idItem == ACDID_INVALID)
        {
            return;
        }
        int32 nNumSlots;
        InventorySlotType eValidSlots[4];
        ActorCommonData * ptACDItem = ACDTryToGet(idItem);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDItem != NULL && ptACDPlayer != NULL)
        {
            nNumSlots = 0;
            ACDInventoryWhereCanThisGo(*ptACDPlayer, ptACDItem->hGB.gbid, nNumSlots, eValidSlots);
            if (eValidSlots[0] != -1)
            {
                ptPlayer->ptNewItemManager->UnMarkAsNew(idItem);
                UIInventory::QuickMoveToEmptySlot(*ptACDPlayer, *ptACDItem, nNumSlots, eValidSlots, FALSE, ptPlayer);
                CancelPurchase();
            }
        }
    }
}

inline void PlayerBloodShardVendorScreen::OnLeftTrigger()  // line 688
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST)
    {
        const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
        if (!tInputState.tControllerState[GetControllerIndex()].tCurrentFrame.IsButtonDown(BUTTON_RIGHT_TRIGGER))
        {
            ScrollDetails(-SCROLLBOX_SCROLLDELTA);
        }
    }
}

inline void PlayerBloodShardVendorScreen::OnRightTrigger()  // line 705
{
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST)
    {
        const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
        if (!tInputState.tControllerState[GetControllerIndex()].tCurrentFrame.IsButtonDown(BUTTON_LEFT_TRIGGER))
        {
            ScrollDetails(SCROLLBOX_SCROLLDELTA);
        }
    }
}

BOOL PlayerBloodShardVendorScreen::CanOpenForController(int32 nControllerIndex) const
{
    return UIInventoryScreen::CanOpenForController(nControllerIndex);
}

void PlayerBloodShardVendorScreen::OnOpen()
{
    Player * ptPlayer = GetPlayer();
    m_tVendorItemsProvider.SetPlayer(ptPlayer);
    m_tPurchasedItemProvider.SetPlayer(ptPlayer);
    m_tEquippedItemProvider.SetPlayer(ptPlayer);
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(NULL);
    ShowVendorModel(*ptPlayer);
    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(ptPlayer);
    HideRingNewItemIndicators();
    SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_BAG));
    gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_MYSTERY_ITEM), ptPlayer, TRUE);
}

void PlayerBloodShardVendorScreen::OnClose()
{
    ResetCurrencyAnimations(UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED);
    UIID idPurchasing = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.BloodshardVendorWrapper");
    UI_GetWindowFromID(idPurchasing).SetActive(FALSE);
    UIID idAffixes = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UI_GetWindowFromID(idAffixes).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_BLOODSHARD_VENDOR_CONTAINER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_VENDOR_PANEL).SetActive(TRUE);
    m_ePurchasingState = PURCHASING_STATE_START;
    m_tVendorItemsProvider.SetPlayer(NULL);
    m_tPurchasedItemProvider.SetPlayer(NULL);
    m_tEquippedItemProvider.SetPlayer(NULL);
    Player * ptPlayer = GetPlayer();
    ptPlayer->idACDActiveVendor = ACDID_INVALID;
    ptPlayer->eActiveCrafterType = CRAFTER_INVALID;
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
    CloseSelectedSlot();
    SimpleMessage tMessage;
    MessageSendToServer(CMSG_CLOSE_INTERACTION, &tMessage, sizeof(tMessage), ptPlayer);
    gtTutorialMgr.CloseTutorialIfOpen(GlobalSNOGet(GLOBALSNO_TUTORIAL_MYSTERY_ITEM));
}

void PlayerBloodShardVendorScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    if (RingHasFocus())
    {
        if (ShouldEnableRingIcon(GetFilter()))
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_OPEN_SLOT, StringListFetch("ConsoleUI:Open").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());
        return;
    }

    switch (m_ePurchasingState)
    {
    case PURCHASING_STATE_START:
        if (GetPrimaryView()->GetSelectedItem() != ACDID_INVALID)
        {
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BUY, StringListFetch("ConsoleShop:Buy").str());
        }
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE_SLOT, StringListFetch("ConsoleUI:Back").str());
        break;
    case PURCHASING_STATE_SHOW_PROGRESS:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CANCEL, StringListFetch("General:Cancel").str());
        return;
    case PURCHASING_STATE_DONE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_CONTINUE, StringListFetch("H2OLayout:Continue").str());
        return;
    case PURCHASING_STATE_SHOW_RESULTS:
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_BUY, StringListFetch("X1_ConsoleBloodshardVendor:BuyAnother").str());
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_FINISH_PURCHASE, StringListFetch("ConsoleUI:Back").str());
        ACDID idPurchasedItem = m_tPurchasedItemProvider.GetItems()[0];
        ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
        ActorCommonData * ptACDPurchasedItem = ACDTryToGet(idPurchasedItem);
        if (ptACDPlayer != NULL && ptACDPurchasedItem != NULL)
        {
            ActorLocationType eLocationType = ptACDPurchasedItem->GetLocationType();
            if (eLocationType == AL_INVENTORY)
            {
                UIItemOverlay eOverlay = ItemGetOverlay(*ptACDPurchasedItem, *ptACDPlayer);
                if (ItemIsEquippable(*ptACDPurchasedItem) && eOverlay == ITEMOVERLAY_NONE)
                {
                    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_EQUIP, StringListFetch("ConsoleInventory:Equip").str());
                }
            }
        }
        break;
    }
    default:
        return;
    }

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}

void PlayerBloodShardVendorScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        ShowNavBar(UIDLG_INVENTORY_CONSOLE, "BloodShardVendor");
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_RING_CURSOR).SetActive(TRUE);
        EnableRing(TRUE);
        RequestUpdate(RESET_SELECTION);
        ::Console::Screens::ShowModalDialog(UI_GetPrimaryUIController(), UIDLG_INVENTORY_CONSOLE, FALSE, GetUIIDNone());
    }
    else
    {
        ::Console::Screens::CloseModalDialog(UIDLG_INVENTORY_CONSOLE);
    }
    UI_GetWindowFromID(UIWND_BLOODSHARD_VENDOR_CONSOLE_BACKGROUND).SetActive(bActive);
    UIID idStats;
    idStats.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "CharacterStats");
    UI_GetWindowFromID(idStats).SetActive(!bActive);
}

void PlayerBloodShardVendorScreen::OnUpdate()
{
    Player * ptPlayer = GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    BOOL bShouldHighlight = !RingHasFocus();
    UI_GetWindowFromID(UIWND_BLOODSHARD_VENDOR_CONSOLE_FOCUS_HIGHLIGHT).SetActive(bShouldHighlight);

    UIFilterType eFilterType = GetFilter();
    if (eFilterType == FILTER_NONE)
    {
        UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_HEADER).SetText(StringListFetch("ConsoleInventory:Empty"), 0);
    }
    else
    {
        ShowFilterName(UITXT_INVENTORY_CONSOLE_HEADER, eFilterType, !RingHasFocus());
    }

    UpdateCurrencyDisplay(UITXT_INVENTORY_CONSOLE_GOLD, UITXT_INVENTORY_CONSOLE_GOLD_SPENT, UIANIM_INVENTORY_CONSOLE_GOLD_SPENT, UITXT_INVENTORY_CONSOLE_GOLD_RETURNED, UIANIM_INVENTORY_CONSOLE_GOLD_RETURNED, CURRENCY_BLOODSHARDS);

    UIInventoryView * ptBrowsingView = GetPrimaryView();
    if (ptBrowsingView != NULL)
    {
        if (m_ePurchasingState == PURCHASING_STATE_SHOW_RESULTS)
        {
            ClearStatusLine(ptBrowsingView);
        }
        else
        {
            UIID idStatusLine = ptBrowsingView->GetStatusLineUIID();
            if (idStatusLine != GetUIIDNone())
            {
                UpdatePrice(ptBrowsingView);
            }
        }
    }

    UIInventoryView * ptDetailsView = GetSecondaryView();
    if (ptDetailsView != NULL)
    {
        ClearStatusLine(ptDetailsView);
    }

    UpdatePurchasingProgress();
}

void PlayerBloodShardVendorScreen::OnRequestedUpdate()
{
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void PlayerBloodShardVendorScreen::InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)
{
    tRingWrapper.WrapUsingContainerControl(UIWND_INVENTORY_CONSOLE_RING, UIWND_INVENTORY_CONSOLE_RING_CURSOR, RingIncludeControlPredicate, NULL);
}

UIFilterType PlayerBloodShardVendorScreen::GetDefaultRingSelection() const
{
    return FILTER_MAIN_HAND;
}

void PlayerBloodShardVendorScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    m_tVendorItemsProvider.SetFilter(eNewFilterType);
    m_tEquippedItemProvider.SetFilter(eNewFilterType);
    RefreshProviders(RESET_SELECTION);
}

void PlayerBloodShardVendorScreen::RefreshProviders(const MaintainSelectionOptions eSelectionType)
{
    m_tVendorItemsProvider.Refresh();
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

void PlayerBloodShardVendorScreen::UpdatePurchasingProgress()
{
    if (m_ePurchasingState == PURCHASING_STATE_SHOW_PROGRESS)
    {
        UpdateProgressBar();
        uint32 dwNow = CGameGetClientTicks();
        if (dwNow >= m_dwPurchasingEndTime)
        {
            m_ePurchasingState = PURCHASING_STATE_WAITING_FOR_REPLY;
            ACDID idSelectedItem = GetPrimaryView()->GetSelectedItem();
            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(idSelectedItem);
            MessageSendToServer(CMSG_SHOP_TRY_BUY, &tMessage, sizeof(tMessage), PlayerGetByACD(GetPlayer()->idPlayerACD));
        }
    }
    else if (m_ePurchasingState == PURCHASING_STATE_WAITING_FOR_RESULT)
    {
        if (m_annPurchaseResult == ANN_INVALID)
        {
            CancelPurchase();
            return;
        }
        ACDID idPurchasedItem = ACDFromANN(m_annPurchaseResult);
        if (idPurchasedItem != ACDID_INVALID)
        {
            m_idPurchasedItem = idPurchasedItem;
            m_ePurchasingState = PURCHASING_STATE_DONE;
            ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_BLOODSHARDS_PICKUP), cgtPSODefaults);
            ActorCommonData * ptACDItem = ACDTryToGet(m_idPurchasedItem);
            if (ptACDItem != NULL)
            {
                SNO snoSound = ActorInventory_GetItemLandingSound(*ptACDItem);
                if (ItemIsLegendary(ptACDItem->GetVisualGBID()))
                {
                    snoSound = GlobalSNOGet(GLOBALSNO_SOUND_LEGENDARY_LANDING);
                }
                if (snoSound != SNO_NONE)
                {
                    SoundManager::PlaySound(snoSound, cgtPSODefaults);
                }
            }
        }
    }
}

void PlayerBloodShardVendorScreen::UpdateProgressBar()
{
    float flProgress = 1.0f;
    uint32 dwNow = CGameGetClientTicks();
    if (dwNow < m_dwPurchasingEndTime)
    {
        uint32 dwRemaining = Math::Min(m_dwPurchasingEndTime - dwNow, PURCHASING_DURATION);
        flProgress = 1.0f - (float)dwRemaining / (float)PURCHASING_DURATION;
    }

    UIID idProgress = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.BloodshardVendorWrapper.BloodshardVendor.ProgressBar");
    UI_GetProgressBarFromID(idProgress).SetExtentsAndPosition(0.0f, 1.0f, flProgress, -1.0f);
    BOOL bFinished = Math::Abs(flProgress - 1.0f) <= 1e-06f;
    CRefString sText = bFinished ? StringListFetch("ConsoleArtisan:Complete") : StringListFetch("X1_ConsoleBloodshardVendor:MysteryReveal");
    UIID idStatus = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.BloodshardVendorWrapper.BloodshardVendor.ProgressStatus");
    UI_GetTextFromID(idStatus).SetText(sText.str(), 0);
    UIID idGlow = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.BloodshardVendorWrapper.BloodshardVendor.Glow");
    UI_GetWindowFromID(idGlow).SetActive(bFinished);
}

void PlayerBloodShardVendorScreen::CancelPurchase()
{
    HidePurchasingProgress();
    m_ePurchasingState = PURCHASING_STATE_START;
    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline void PlayerBloodShardVendorScreen::HidePurchasingProgress()  // line 1152
{
    UIID idPurchasing = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.BloodshardVendorWrapper");
    UI_GetWindowFromID(idPurchasing).SetActive(FALSE);
    UIID idAffixes = UIIDCreateFromParent(m_tPurchaseView.GetRootUIID(), ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UI_GetWindowFromID(idAffixes).SetActive(TRUE);
    m_tPurchaseView.RequestUpdate(RESET_SELECTION);
}

void PlayerBloodShardVendorScreen::InitRingFilters(UIIDToFilterMap & tFilterMap)
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

UIInventoryView * PlayerBloodShardVendorScreen::GetPrimaryViewForMode(const BrowsingMode eBrowsingMode)
{
    if (m_ePurchasingState == PURCHASING_STATE_START)
    {
        return &m_tVendorListView;
    }
    return &m_tPurchaseView;
}

UIInventoryView * PlayerBloodShardVendorScreen::GetSecondaryViewForMode(const BrowsingMode)
{
    return &m_tEquippedItemView;
}

BOOL PlayerBloodShardVendorScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    return m_tVendorItemsProvider.GetItemsWithFilter(eFilterType).size() != 0;
}

inline void PlayerBloodShardVendorScreen::OnItemDelivered(const ANNDataMessage * pMsg)  // line 1223
{
    if (m_ePurchasingState == PURCHASING_STATE_WAITING_FOR_REPLY && pMsg->ann != ANN_INVALID)
    {
        m_annPurchaseResult = pMsg->ann;
        m_ePurchasingState = PURCHASING_STATE_WAITING_FOR_RESULT;
    }
}

void PlayerBloodShardVendorScreen::OnInventoryChanged()
{
    m_tEquippedItemView.RequestUpdate(RESET_SELECTION);
}

void OnBloodShardVendorUpdate()
{
    if (sgptBloodShardVendorGlobals != NULL && sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->Update();
    }
}

void OnBloodShardVendorLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptBloodShardVendorGlobals != NULL && sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void OnBloodShardVendorRightStick(const UIID &, const int32 nControllerIndex)
{
    if (sgptBloodShardVendorGlobals != NULL && sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->HandleRightStickInput(nControllerIndex);
    }
}

void OnBloodShardVendorButton(const UIID &, const int32 nControllerIndex, const InputButton eInputButton)
{
    if (sgptBloodShardVendorGlobals != NULL && sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->HandleButtonInput(nControllerIndex, eInputButton);
    }
}

void ShowVendorModel(const Player & tPlayer)  // line 1303
{
    ActorCommonData & tACDVendor = ACDGet(tPlayer.idACDActiveVendor);

    ::Console::UIRActorManager::SetActive(TRUE);
    BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_NPC, FALSE, TRUE);
    if (bShouldCreateRActor)
    {
        UIRActor & tModel = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
        RActorID idRActor = (RActorID)tACDVendor.idOwner;
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor != NULL)
        {
            tModel.CreateRActor(RENDERCONTEXT_HUD_0, ptRActor->snoActor);

            LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
            tModel.SetLight(idLight);

            SNO snoAnimSet = ActorGetAnimSetSNO(ptRActor->snoActor);
            RActor * ptVendorRActor = RActorTryToGet(tModel.GetRActor());
            if (snoAnimSet == GlobalSNOGet(GLOBALSNO_ANIMSET_OMNINPC_MALE))
            {
                RActorPlayAnimation(ptVendorRActor, TAG_ACTOR_ANIMATION_GIZMO_STATE_10, TRUE, FALSE);
                tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_CRAFTER));
            }
            else
            {
                tModel.SetObserver(GlobalSNOGet(GLOBALSNO_OBSERVER_CONSOLE_INVENTORY_FOLLOWER));
            }
        }
    }

    SwitchToInfoPanel(VENDOR_INFO_PANEL);

    CRefString sVendorName;
    MonsterGetBaseName(tACDVendor.tSNOName, sVendorName);
    UI_GetTextFromID(UITXT_INVENTORY_CONSOLE_BLOODSHARD_VENDOR_NAME).SetText(sVendorName.str(), 0);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_BLOODSHARD_VENDOR_CONTAINER).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_INVENTORY_CONSOLE_VENDOR_PANEL).SetActive(FALSE);
    UI_GetTextFromID(UITXT_CONSOLE_PLAYER_LABEL_RING).SetText("", 0);

    UIID idGold;
    idGold.SetFromParent(UIWND_INVENTORY_CONSOLE_ROOT, "GoldBalance");
    UIInventory::UpdateGoldAmount(idGold, tPlayer.idPlayerACD);
}

}  // namespace Console
}  // namespace UIInventory

namespace UIBloodShardVendor {
namespace Console {
using namespace UIInventory::Console;

void InitializeForGame()
{
    if (!GameIsStartUp())
    {
        sgptBloodShardVendorGlobals = new PlayerBloodShardVendorScreen();
    }
}

void DisposeForGame()
{
    if (!GameIsStartUp())
    {
        ::delete sgptBloodShardVendorGlobals;
        sgptBloodShardVendorGlobals = NULL;
    }
}

void OpenForController(int32 nControllerIndex)
{
    sgptBloodShardVendorGlobals->OpenForController(nControllerIndex);
}

void Close()
{
    if (sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->Close();
    }
}

void OnItemDelivered(const ANNDataMessage * pMsg)
{
    if (sgptBloodShardVendorGlobals != NULL && sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->OnItemDelivered(pMsg);
    }
}

void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce)
{
    if (sgptBloodShardVendorGlobals != NULL && sgptBloodShardVendorGlobals->IsActive())
    {
        sgptBloodShardVendorGlobals->OnInventoryChanged();
    }
}

}  // namespace Console
}  // namespace UIBloodShardVendor

namespace {

inline void UpdatePrice(UIInventory::Console::UIInventoryView * ptView)  // line 1458
{
    using namespace UIInventory::Console;
    if (sgptBloodShardVendorGlobals == NULL || !sgptBloodShardVendorGlobals->IsActive())
    {
        return;
    }
    Player * ptPlayer = sgptBloodShardVendorGlobals->GetPlayer();
    if (ptPlayer == NULL)
    {
        return;
    }

    CRefString sPriceText;
    ActorCommonData * ptACDItem = ACDTryToGet(ptView->GetSelectedItem());
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    ActorCommonData * ptACDVendor = ACDTryToGet(ptPlayer->idACDActiveVendor);
    if (ptACDItem != NULL && ptACDPlayer != NULL && ptACDVendor != NULL)
    {
        BOOL bCanAfford = ACDInventoryCanBuy(*ptACDPlayer, *ptACDItem) != BUY_RESPONSE_CANT_AFFORD;
        CurrencyType eCurrencyType = CURRENCY_BLOODSHARDS;
        uint64 uCost = ItemGetCost(*ptACDItem, 1, eCurrencyType, ptACDVendor, FALSE, FALSE);
        CRefString sPrice = UIText_ValueToGameCurrency(uCost, eCurrencyType);
        Set_Text_Color(sPrice, bCanAfford ? FONTCOLOR_WHITE : FONTCOLOR_RED);
        sPriceText = LocalizedSprintf(StringListFetch("ConsoleShop:CostFormat").str(), sPrice.str(), NULL);
    }
    UI_GetTextFromID(ptView->GetStatusLineUIID()).SetText(sPriceText.str(), 0);
}

BOOL RingIncludeControlPredicate(const UIID & idControl)  // line 1507
{
    return idControl != UIWND_INVENTORY_CONSOLE_RING_GLOW;
}

inline void HideRingNewItemIndicators()  // line 1517
{
    UIID idKey;
    const UIIDToFilterMap & tFilterMap = UIInventory::Console::sgptBloodShardVendorGlobals->GetFilterMap();
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
