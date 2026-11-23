#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleModalNotificationManager.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../CGame.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics, which the original's static
// initialiser runs ahead of this file's own.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/Trade.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/UnicodeText.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/Player.h:269 and :280






// X/Common/Player.h




// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
// X/Common/ActorCommonData.cpp
ACDID ACDFromANN(const ACDNetworkName ann);
ACDNetworkName ANNFromACD(ACDID idACD);
// X/Common/Items.cpp
BOOL ItemIsUnidentified(const ActorCommonData & tACDItem);
BOOL ItemIsEquippable(const ActorCommonData & tACDItem);
BOOL ItemIsTradeable(const Player & tPlayer, const Player * ptPlayerDest, const ActorCommonData & tACDItem, BOOL bUnused);
CRefString ItemGetDisplayName(const ActorCommonData & tACDItem, BOOL bUnused0, BOOL bUnused1, BOOL bUnused2, BOOL bUnused3);
// X/Common/Trade.cpp
void ResetTradeSessionData(Player * ptPlayer);
// X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Common/UserInterface/UITextControl.cpp
void Set_Text_Color(CRefString & usString, const RGBAColor & rgbaColor);
// X/Client/CPlayer.cpp
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);
// X/Client/CTrade.cpp
void CTradeCancel(const Player * ptLocalPlayer, BOOL bForce, BOOL bSendMessage);
void CTradeAccept(const Player & tLocalPlayer);
void CTradeUpdateOffer(Player & tLocalPlayer, uint64 uGoldOffer);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/Input/Input.cpp
void InputClearRemappings();
void InputRemapSingleJoyconButton(int32 eButton, int32 eRemappedButton);
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
const UIID & UI_GetControllerFocus();
// X/Client/UserInterface/UIText.cpp
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_TRADE_CONSOLE;
extern UIID UIWND_TRADE_CONSOLE_ITEMS_MINE;
extern UIID UIWND_TRADE_CONSOLE_ITEMS_OTHER;
// X/Client/Console/ControllerUtils.cpp
namespace Console {
namespace ControllerUtils {
BOOL IsButtonDown(int32 nControllerIndex, int32 eButton);
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bUnused);
}  // namespace ControllerUtils
}  // namespace Console
#include "ControllerActions.h"
namespace Console {
typedef int32 ControllerInput;
typedef int32 ControllerActionId;









}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
ConsoleModalNotificationManager & GetModalNotificationManager();
void UpdateButtonLegendText();
void OpenToScreen(int32 nScreenSet, int32 nScreen, int32 nControllerIndex, BOOL bUnused);
BOOL IsScreenOpen(int32 nScreenSet, int32 nScreen);
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
BOOL IsModalDialogVisible();
BOOL IsScreenSetOpen();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
namespace UIStackSplit {
namespace Console {
enum StackSplitType {
    STACKSPLIT_GOLD_TRADE = 1,
};
typedef void (*StackSplitCallback)(const ACDID, const uint64);
void Open(ACDID idACD, uint64 uMin, uint64 uMax, StackSplitCallback pfnCallback, StackSplitType eType);
BOOL IsVisible();
void Close();
}  // namespace Console
}  // namespace UIStackSplit
// X/Client/UserInterface/Console/UIConsoleFastEquip.cpp
namespace UIFastEquip {
namespace Console {
void CloseForPlayer(int32 nPlayerIndex);
}  // namespace Console
}  // namespace UIFastEquip
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
}  // namespace UIGame
// X/Client/UserInterface/Interface/UIInventory.cpp
namespace UIInventory {
void PlayItemInventorySound(const ActorCommonData & tACDItem);
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
void SetUpItemButton(const UIID & idButton, ACDID idACDItem, ACDID idACDPlayer, ACDID idACDOwner, BOOL bUnused);
void UpdateComparisonInfo(const UIID & idParent, const ActorCommonData * ptACDItem, ACDID idACDPlayer, BOOL bUnused);
void ShowItemPrimaryStats(const UIID & idParent, const ActorCommonData * ptACDItem, ACDID idACDPlayer);
void ShowItemDetails(const UIID & idDetailsParent, const BOOL bShow);
CRefString GetItemDescription(ACDID idACDPlayer, ACDID idACDItem, BOOL bUnused);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
void ResetScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils

namespace UITrade {
namespace Console {

enum ButtonActions {
    ACTION_ACCEPT = 0,
    ACTION_CLOSE = 1,
    ACTION_OFFER_GOLD = 2,
    ACTION_ADD_ITEM = 3,
    ACTION_REMOVE_ITEM = 4,
    ACTION_DETAILS = 5,
    ACTION_LIST = 6,
    ACTION_SCROLL_LIST = 7,
};

class ListViewDataSource : public ConsoleListBoxDataSource {  // size 0x50
public:
    ListViewDataSource();
    virtual ~ListViewDataSource() {}  // line 64
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    bool CanHaveFocus();
    int32 nNumEntries;  // 0x08
    struct ItemEntry {  // size 0x10
        ACDNetworkName ann;  // 0x00
        ActorCommonData * ptACD;  // 0x08
    };
    UITrade::Console::ListViewDataSource::ItemEntry tItems[4];  // 0x10
};
static_assert(sizeof(ListViewDataSource) == 0x50, "ListViewDataSource");

class TradeView {  // size 0x80
public:
    TradeView()  // line 100
    {
        m_fShowingDetails = FALSE;
        m_idWnd = GetUIIDNone();
        m_idListBox = GetUIIDNone();
        m_idDetails = GetUIIDNone();
        m_idDetailsList = GetUIIDNone();
        m_idTxtGold = GetUIIDNone();
    }

    void Reset(const UIID & idWnd);
    void RefreshList();
    void Update(const Player * ptPlayer, const BOOL fOther);

    BOOL IsShowingDetails() const  // line 121
    {
        return m_fShowingDetails;
    }

    void SetShowDetails(const BOOL fShow);
    void SetFocus();
    BOOL CanScrollDetails();
    void ScrollDetails(const float flDelta);
    void ResetDetailsScroll();

    const UIID & GetListBox() const  // line 145
    {
        return m_idListBox;
    }

    const UIID & GetDetailsListBox() const  // line 150
    {
        return m_idDetailsList;
    }

    const UIID & GetWindow() const  // line 156
    {
        return m_idWnd;
    }

    void UpdateItemDetailsUI(const Player & tPlayer);
    UITrade::Console::ListViewDataSource m_tDataSrc;  // 0x00
    static void sUpdateGoldOffer(const Player & tPlayer, const UIID & idText);
    static void sUpdatePortrait(const Player & tPlayer, const UIID & idWnd, const BOOL bUseAlt);
    static void sUpdateNameBar(const Player & tPlayer, const UIID & idText);
    BOOL m_fShowingDetails;  // 0x50
    UIID m_idWnd;  // 0x58
    UIID m_idListBox;  // 0x60
    UIID m_idDetails;  // 0x68
    UIID m_idDetailsList;  // 0x70
    UIID m_idTxtGold;  // 0x78
};
static_assert(sizeof(TradeView) == 0x80, "TradeView");

enum TradeUIState {
    TRADEUI_STATE_INACTIVE = 0,
    TRADEUI_STATE_NORMAL = 1,
    TRADEUI_STATE_WAITING_FOR_ACDS = 2,
};

struct UITradeGlobals {  // size 0x140
    UITradeGlobals()  // line 196
        : m_fSwitchingToOfferScreen(FALSE)
        , m_uNextAcceptTime(0)
        , m_nNumUnknownTradeACDs(0)
        , m_eState(TRADEUI_STATE_INACTIVE)
    {
    }

    void Reset()  // line 204
    {
        m_nNumUnknownTradeACDs = 0;
        m_eState = TRADEUI_STATE_INACTIVE;

        const Blizzard::Time::Microsecond DEFAULT_STICK_DELAY = 200;
        m_tStickInputFilter = ::Console::GetDefaultStickInputFilter(0);
        m_tStickInputFilter.SetMinimumInputDelay(DEFAULT_STICK_DELAY);

        m_tMine.Reset(UIWND_TRADE_CONSOLE_ITEMS_MINE);
        m_tOther.Reset(UIWND_TRADE_CONSOLE_ITEMS_OTHER);
    }

    UITrade::Console::TradeView * GetCurrentView()  // line 218
    {
        if (UI_GetControllerFocus() == m_tOther.m_idListBox)
            return &m_tOther;
        return &m_tMine;
    }

    BOOL m_fSwitchingToOfferScreen;  // 0x00
    UITrade::Console::TradeView m_tMine;  // 0x08
    UITrade::Console::TradeView m_tOther;  // 0x88
    ::Console::AnalogStickInputFilter m_tStickInputFilter;  // 0x108
    uint32 m_uNextAcceptTime;  // 0x130
    int32 m_nNumUnknownTradeACDs;  // 0x134
    UITrade::Console::TradeUIState m_eState;  // 0x138
};
static_assert(sizeof(UITradeGlobals) == 0x140, "UITradeGlobals");

static UITradeGlobals sgtUITradeGlobals;  // line 244

static int32 snOpeningControllerIndex;  // line 246
static Player * sptPlayer;  // line 247

inline ListViewDataSource::ListViewDataSource()  // line 252
{
    nNumEntries = 0;
}

int32 ListViewDataSource::GetNumItems() const
{
    return nNumEntries;
}

UIControlSourceKey ListViewDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse("ConsoleInventoryListEquipmentTemplate:ConsoleInventoryListEquipmentTemplate_main", SNO_NONE, NULL, NULL);
    return ITEM_SOURCE_KEY;
}

void ListViewDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

void ListViewDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    ACDID idACDItem = ACDFromANN(tItems[nIndex].ann);
    UIID idRoot = UIIDCreateFromParent(idItemUI, ".LayoutRoot");

    BOOL bSelected = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UIID idSelected = UIIDCreateFromParent(idRoot, ".Selected");
    UIWindow & tWndSelected = UI_GetWindowFromID(idSelected);
    tWndSelected.SetActive(bSelected);

    UIID idIcon = UIIDCreateFromParent(idRoot, ".ItemVisual");
    UIInventory::Console::SetUpItemButton(idIcon, idACDItem, sptPlayer->idPlayerACD, sptPlayer->idPlayerACD, TRUE);

    CRefString sName;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem != NULL)
    {
        BOOL bIsIdentified = !ItemIsUnidentified(*ptACDItem);
        if (bIsIdentified)
        {
            sName = ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, TRUE);
        }
        else
        {
            sName = StringListFetch("ConsoleInventory:UnidentifiedItemName");
        }
    }

    UIID idName = UIIDCreateFromParent(idRoot, ".ItemName");
    UI_GetTextFromID(idName).SetText(sName.str(), FALSE);

    UIID idComparison = UIIDCreateFromParent(idRoot, ".Bonuses");
    BOOL bShouldDisplay = ptACDItem != NULL && !ItemIsUnidentified(*ptACDItem) && ItemIsEquippable(*ptACDItem);
    UI_GetWindowFromID(idComparison).SetActive(bShouldDisplay);

    UIInventory::Console::UpdateComparisonInfo(idRoot, ptACDItem, sptPlayer->idPlayerACD, FALSE);
    UIInventory::Console::ShowItemPrimaryStats(idRoot, ptACDItem, sptPlayer->idPlayerACD);
}

inline bool ListViewDataSource::CanHaveFocus()  // line 330
{
    for (int32 i = 0; i < nNumEntries; i++)
    {
        if (tItems[i].ann != ANN_INVALID && tItems[i].ptACD != NULL)
        {
            return true;
        }
    }

    return false;
}

void TradeView::Reset(const UIID & idWnd)
{
    m_fShowingDetails = FALSE;
    m_idWnd = idWnd;
    m_idListBox = UIIDCreateFromParent(idWnd, ".ItemList");
    m_idDetails = UIIDCreateFromParent(idWnd, ".Details");
    m_idDetailsList = UIIDCreateFromParent(m_idDetails, ".LayoutRoot.ItemDetails.Item.ItemList");
    m_idTxtGold = UIIDCreateFromParent(idWnd, ".Gold");

    m_tDataSrc.nNumEntries = 0;

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(m_idListBox);
    tListBox.SetDataSource(m_tDataSrc);
    tListBox.ResetAllItems();

    UIConsoleListBox & tDetailsListBox = UI_GetConsoleListBoxFromID(m_idDetailsList);
    tDetailsListBox.SetDataSource(m_tDataSrc);
    tDetailsListBox.ResetAllItems();

    CRefString sInitialGoldVal = UIText_ValueToGameCurrency(0, 0);
    UI_GetTextFromID(m_idTxtGold).SetText(sInitialGoldVal, FALSE);
}

void TradeView::Update(const Player * ptPlayer, const BOOL fOther)
{
    sUpdateGoldOffer(*ptPlayer, m_idTxtGold);

    UIID idPortrait = UIIDCreateFromParent(m_idWnd, ".Portrait");
    sUpdatePortrait(*ptPlayer, idPortrait, fOther);

    UIID idName = UIIDCreateFromParent(m_idWnd, ".Name");
    sUpdateNameBar(*ptPlayer, idName);

    UIID idHighlight = UIIDCreateFromParent(m_idWnd, ".Highlight");
    UI_GetWindowFromID(idHighlight).SetActive(ptPlayer->tTradeData.bTradeAccepted);
}

void TradeView::UpdateItemDetailsUI(const Player & tPlayer)
{
    UIConsoleListBox & tDetailsListBox = UI_GetConsoleListBoxFromID(m_idDetailsList);
    ActorCommonData * ptACDItem = NULL;

    int32 nSelIndex = tDetailsListBox.GetSelectedIndex();
    if (nSelIndex >= 0 || nSelIndex < m_tDataSrc.nNumEntries)
    {
        ACDID idACDItem = ACDFromANN(m_tDataSrc.tItems[nSelIndex].ann);
        ptACDItem = ACDTryToGet(idACDItem);
    }

    BOOL fNoItems = ptACDItem == NULL;
    UIID idNoItems = UIIDCreateFromParent(m_idDetails, ".LayoutRoot.ItemDetails.NoItem");
    UI_GetWindowFromID(idNoItems).SetActive(fNoItems);

    if (fNoItems)
    {
        UIID idNoItemsMessage = UIIDCreateFromParent(idNoItems, ".NoItemMessage");
        UIText & tNoItemText = UI_GetTextFromID(idNoItemsMessage);
        tNoItemText.SetActive(TRUE);
        tNoItemText.SetText(StringListFetch("ConsoleInventory:NoItemSelected"), FALSE);
        return;
    }

    BOOL bIsIdentified = !ItemIsUnidentified(*ptACDItem);
    UIID idDetailsParent = UIIDCreateFromParent(m_idDetails, ".LayoutRoot.ItemDetails.Item");
    UI_GetWindowFromID(idDetailsParent).SetActive(TRUE);

    UIInventory::Console::ShowItemDetails(idDetailsParent, bIsIdentified);
    if (!bIsIdentified)
    {
        return;
    }

    UIID idStats = GetUIIDNone();
    idStats.SetFromParent(idDetailsParent, "ScrollBox.ScrollingStack.Affixes");
    CRefString sItemDescription = UIInventory::Console::GetItemDescription(tPlayer.idPlayerACD, ptACDItem->id, FALSE);
    UI_GetTextFromID(idStats).SetText(sItemDescription, FALSE);
}

inline BOOL TradeView::CanScrollDetails()  // line 467
{
    UIID idItemDetails = UIIDCreateFromParent(m_idDetails, ".LayoutRoot.ItemDetails.Item.ScrollBox");
    return UIUtils::Console::CanScrollContentInScrollBox(idItemDetails);
}

inline void TradeView::ScrollDetails(const float flDelta)  // line 477
{
    UIID idItemDetails = UIIDCreateFromParent(m_idDetails, ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UIUtils::Console::ScrollContentInScrollBox(idItemDetails, flDelta);
}

inline void TradeView::ResetDetailsScroll()  // line 487
{
    UIID idItemDetails = UIIDCreateFromParent(m_idDetails, ".LayoutRoot.ItemDetails.Item.ScrollBox");
    UIUtils::Console::ResetScrollBox(idItemDetails);
}

void TradeView::SetShowDetails(const BOOL fShow)
{
    if (m_fShowingDetails != fShow)
    {
        ResetDetailsScroll();
    }

    m_fShowingDetails = fShow;

    if (m_idListBox == GetUIIDNone())
    {
        return;
    }

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(m_idListBox);
    int32 nSelIndex = tListBox.GetSelectedIndex();
    tListBox.SetActive(!m_fShowingDetails);

    UIConsoleListBox & tDetailsListBox = UI_GetConsoleListBoxFromID(m_idDetailsList);
    if (tDetailsListBox.GetSelectedIndex() != nSelIndex)
    {
        tDetailsListBox.SetSelectedIndex(nSelIndex);
    }
    else
    {
        tDetailsListBox.RefreshItem(nSelIndex);
    }

    UI_GetWindowFromID(m_idDetails).SetActive(m_fShowingDetails);

    if (m_fShowingDetails && sptPlayer != NULL)
    {
        UpdateItemDetailsUI(*sptPlayer);
    }
}

inline void TradeView::SetFocus()  // line 537
{
    TradeView * ptOldView = sgtUITradeGlobals.GetCurrentView();
    UI_SetControllerFocus(m_idListBox);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    if (ptOldView != this)
    {
        ptOldView->SetShowDetails(FALSE);
    }
}

inline void TradeView::sUpdateGoldOffer(const Player & tPlayer, const UIID & idText)  // line 553
{
    CRefString sCurrencyCount = UIText_ValueToGameCurrency(tPlayer.tTradeData.uMyGoldOffer, 0);
    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(sCurrencyCount, FALSE);
}

inline void TradeView::sUpdatePortrait(const Player & tPlayer, const UIID & idWnd, const BOOL bUseAlt)  // line 565
{
    ImageTextureFrame tFrame;
    ActorGetPortraitTexFrame(tFrame, tPlayer.snoActorPortrait, bUseAlt);

    UIWindow & tWnd = UI_GetWindowFromID(idWnd);
    tWnd.SetTextureFrame(tFrame);
}

inline void TradeView::sUpdateNameBar(const Player & tPlayer, const UIID & idText)  // line 580
{
    UIText & tText = UI_GetTextFromID(idText);
    tText.SetActive(TRUE);
    tText.SetText(CPlayerGetServiceName(tPlayer, FALSE), FALSE);
    tText.SetTextColor(crgbaWhite);
}

void StartTrade(int32 nControllerIndex)
{
    if (nControllerIndex != -1 && PlayerGetByControllerIndex(nControllerIndex) != NULL)
    {
        sptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

        ResetTradeSessionData(sptPlayer);

        sgtUITradeGlobals.Reset();
        sgtUITradeGlobals.m_eState = TRADEUI_STATE_NORMAL;
    }

    ::Console::Screens::OpenToScreen(26, 0, nControllerIndex, FALSE);
}

void ForceCancel()
{
    if (sgtUITradeGlobals.m_eState != TRADEUI_STATE_INACTIVE && sptPlayer != NULL)
    {
        CTradeCancel(sptPlayer, FALSE, TRUE);
    }
}

void EndTrade()
{
    UIWindow & tWnd = UI_GetWindowFromID(UIDLG_TRADE_CONSOLE);
    if (tWnd.IsActive() || ::Console::Screens::IsScreenOpen(27, 0))
    {
        tWnd.SetActive(FALSE);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRADE_CLOSE), cgtPSODefaults);
        UI_SetControllerFocus(GetUIIDNone());
        ::Console::Screens::CloseScreenSet(1, 0);
    }

    sgtUITradeGlobals.Reset();
}

void ResetAcceptTimer();

void OpenForController(int32 nControllerIndex)
{
    UI_GetWindowFromID(UIDLG_TRADE_CONSOLE).SetActive(TRUE);
    sgtUITradeGlobals.m_tMine.SetFocus();
    sgtUITradeGlobals.m_eState = TRADEUI_STATE_NORMAL;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRADE_OPEN), cgtPSODefaults);
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    ResetAcceptTimer();

    sgtUITradeGlobals.m_tMine.SetShowDetails(FALSE);
    sgtUITradeGlobals.m_tOther.SetShowDetails(FALSE);

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer != NULL)
    {
        UIFastEquip::Console::CloseForPlayer(ptPlayer->nPlayerIndex);
    }
}

void ResetAcceptTimer()
{
    const uint32 TRADE_ACCEPT_DELAY_TICKS = 120;
    sgtUITradeGlobals.m_uNextAcceptTime = CGameGetClientTicks() + TRADE_ACCEPT_DELAY_TICKS;
}

BOOL CanTrade(int32 nControllerIndex)
{
    return !::Console::Screens::IsModalDialogVisible() && !::Console::Screens::IsScreenSetOpen();
}

void OtherIsBusy()
{
    UIGame::DisplayError(GameErrorData(GAMEERROR_TRADE_OTHER_BUSY), FALSE, NULL);
}

void Close()
{
    InputClearRemappings();

    if (!sgtUITradeGlobals.m_fSwitchingToOfferScreen)
    {
        CTradeCancel(sptPlayer, FALSE, TRUE);

        if (UIStackSplit::Console::IsVisible())
        {
            UIStackSplit::Console::Close();
        }

        sgtUITradeGlobals.m_eState = TRADEUI_STATE_INACTIVE;
    }

    UIWindow & tWnd = UI_GetWindowFromID(UIDLG_TRADE_CONSOLE);
    tWnd.SetActive(FALSE);
    UI_SetControllerFocus(GetUIIDNone());
}

static BOOL sCanAccept()  // line 753
{
    if (sgtUITradeGlobals.m_eState == TRADEUI_STATE_WAITING_FOR_ACDS)
    {
        return FALSE;
    }

    const Player * ptPlayerMe = sptPlayer;
    if (ptPlayerMe == NULL)
    {
        return FALSE;
    }

    const Player * ptPlayerOther = PlayerGetByACD(ptPlayerMe->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return FALSE;
    }

    return sgtUITradeGlobals.m_tMine.m_tDataSrc.GetNumItems() > 0 ||
           sgtUITradeGlobals.m_tOther.m_tDataSrc.GetNumItems() > 0 ||
           ptPlayerOther->tTradeData.uMyGoldOffer != 0 || ptPlayerMe->tTradeData.uMyGoldOffer != 0;
}

static BOOL sAcceptCooldownDone()  // line 783
{
    uint32 uNow = CGameGetClientTicks();
    return uNow >= sgtUITradeGlobals.m_uNextAcceptTime && !sptPlayer->tTradeData.bTradeAccepted;
}

void OnButton1(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(4) == ACTION_ACCEPT)
    {
        if (sCanAccept() && sAcceptCooldownDone())
        {
            CTradeAccept(*sptPlayer);
        }
    }
}

void OnButton2(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(7) == ACTION_CLOSE)
    {
        CTradeCancel(sptPlayer, FALSE, TRUE);
    }
}

BOOL ItemIsBeingOffered(const ACDID idACDItem)
{
    ACDNetworkName annItem = ANNFromACD(idACDItem);
    ListViewDataSource * ptDataSrc = &sgtUITradeGlobals.m_tMine.m_tDataSrc;
    for (int32 i = 0; i < ptDataSrc->nNumEntries; i++)
    {
        if (annItem == ptDataSrc->tItems[i].ann)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void sAddTradeItem(const ActorCommonData & tACDItem)
{
    if (sgtUITradeGlobals.m_tMine.m_tDataSrc.nNumEntries >= 4)
    {
        return;
    }

    if (ItemIsBeingOffered(tACDItem.id))
    {
        return;
    }

    ACDNetworkName annItem = ANNFromACD(tACDItem.id);
    int32 nIndex = sgtUITradeGlobals.m_tMine.m_tDataSrc.nNumEntries;
    ListViewDataSource * ptDataSrc = &sgtUITradeGlobals.m_tMine.m_tDataSrc;
    ptDataSrc->tItems[nIndex].ann = annItem;
    ptDataSrc->tItems[nIndex].ptACD = NULL;
    ptDataSrc->nNumEntries++;

    TradeMessage tMessage;
    tMessage.dwData2 = annItem;
    tMessage.dwData = nIndex;
    tMessage.eMessageType = 6;
    MessageSendToServer(CMSG_TRADE, &tMessage, sizeof(tMessage), sptPlayer);

    UIInventory::PlayItemInventorySound(tACDItem);
}

void OnButton3(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(5);
    if (idAction == ACTION_DETAILS || idAction == ACTION_LIST)
    {
        TradeView * ptView = sgtUITradeGlobals.GetCurrentView();
        ptView->SetShowDetails(!ptView->IsShowingDetails());
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void OnButton4(const UIID &, const int32 nControllerIndex)
{
    const int32 MAX_NUM_ITEMS = 4;

    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(6) != ACTION_ADD_ITEM)
    {
        return;
    }

    const Player * ptPlayerOther = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return;
    }

    if (sgtUITradeGlobals.m_tMine.m_tDataSrc.GetNumItems() == MAX_NUM_ITEMS)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleTrade:MaxItemsError").str(), UnicodeText::FormatNumber(MAX_NUM_ITEMS, TRUE, FNS_NORMAL, 0).str(), NULL);
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
    }
    else
    {
        sgtUITradeGlobals.m_fSwitchingToOfferScreen = TRUE;
        ::Console::Screens::OpenToScreen(27, 0, nControllerIndex, FALSE);
        sgtUITradeGlobals.m_fSwitchingToOfferScreen = FALSE;
    }
}

void OnButtonL2(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(10) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
    {
        TradeView * ptView = sgtUITradeGlobals.GetCurrentView();
        ptView->ScrollDetails(-4.0f);
    }
}

void OnButtonR2(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(11) == ACTION_SCROLL_LIST &&
        !::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
    {
        TradeView * ptView = sgtUITradeGlobals.GetCurrentView();
        ptView->ScrollDetails(4.0f);
    }
}

void sOfferGold(const ACDID idACDPlayerDst, const uint64 uAmount)
{
    if (uAmount != 0)
    {
        CTradeUpdateOffer(*sptPlayer, uAmount);
    }
}

inline void sRemoveTradeItem(const int32 nIndex)  // line 1020
{
    ListViewDataSource & tDataSrc = sgtUITradeGlobals.m_tMine.m_tDataSrc;
    if (nIndex < 0 || nIndex >= tDataSrc.nNumEntries)
    {
        return;
    }

    ACDNetworkName annToRemove = tDataSrc.tItems[nIndex].ann;
    SigmaMemoryMove(&tDataSrc.tItems[nIndex], &tDataSrc.tItems[nIndex + 1], (tDataSrc.nNumEntries - nIndex - 1) * sizeof(ListViewDataSource::ItemEntry));
    tDataSrc.nNumEntries--;

    TradeMessage tMessage;
    tMessage.dwData = nIndex;
    tMessage.ann = annToRemove;
    tMessage.eMessageType = 9;
    MessageSendToServer(CMSG_TRADE, &tMessage, sizeof(tMessage), sptPlayer);
}

void OnButtonL3(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(21) != ACTION_REMOVE_ITEM)
    {
        return;
    }

    const Player * ptPlayerOther = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return;
    }

    UIConsoleListBox & tListBoxMine = UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tMine.m_idListBox);
    int32 nSelectedIndex = tListBoxMine.GetSelectedIndex();
    if (nSelectedIndex != -1)
    {
        sRemoveTradeItem(nSelectedIndex);
        tListBoxMine.ResetAllItems();
        UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tMine.m_idDetailsList).ResetAllItems();
    }
}

void OnButtonR3(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    const Player * ptPlayerOther = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(22) == ACTION_OFFER_GOLD)
    {
        ActorCommonData & tACDPlayer = ACDGet(sptPlayer->idPlayerACD);
        uint64 uAmount = tACDPlayer.GetCurrencyAmount(0);
        UIStackSplit::Console::Open(ptPlayerOther->idPlayerACD, sptPlayer->tTradeData.uMyGoldOffer, uAmount, sOfferGold, UIStackSplit::Console::STACKSPLIT_GOLD_TRADE);
    }
}

static BOOL sSwitchFocusToMine()  // line 1106
{
    sgtUITradeGlobals.m_tMine.SetFocus();
    return TRUE;
}

static BOOL sSwitchFocusToOther()  // line 1115
{
    if (sgtUITradeGlobals.m_tOther.m_tDataSrc.CanHaveFocus())
    {
        sgtUITradeGlobals.m_tOther.SetFocus();
        return TRUE;
    }

    return FALSE;
}

void OnDPadLeft(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    const Player * ptPlayerOther = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return;
    }

    sSwitchFocusToMine();
}

void OnDPadRight(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer == NULL || sptPlayer->nControllerIndex != nControllerIndex)
    {
        return;
    }

    const Player * ptPlayerOther = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return;
    }

    sSwitchFocusToOther();
}

void CheckForKnownItems()
{
    if (sgtUITradeGlobals.m_nNumUnknownTradeACDs == 0)
    {
        sgtUITradeGlobals.m_eState = TRADEUI_STATE_NORMAL;
        return;
    }

    int32 nCount = 0;
    ListViewDataSource & tDataSrc = sgtUITradeGlobals.m_tOther.m_tDataSrc;
    for (int32 i = 0; i < 4; i++)
    {
        ActorCommonData * ptACD = ACDTryToGet(ACDFromANN(tDataSrc.tItems[i].ann));
        if (tDataSrc.tItems[i].ptACD == NULL && ptACD != NULL)
        {
            tDataSrc.tItems[i].ptACD = ptACD;
            sgtUITradeGlobals.m_nNumUnknownTradeACDs--;
        }

        if (tDataSrc.tItems[i].ptACD != NULL)
        {
            nCount++;
        }
    }

    if (sgtUITradeGlobals.m_nNumUnknownTradeACDs == 0)
    {
        sgtUITradeGlobals.m_eState = TRADEUI_STATE_NORMAL;
        tDataSrc.nNumEntries = nCount;

        UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tOther.m_idListBox).ResetAllItems();
        UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tOther.m_idDetailsList).ResetAllItems();

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

void UpdateButtonLegend();
void UpdateAcceptButtonCountdown();

void OnUpdate(const UIID &)
{
    const Player * ptPlayerMe = sptPlayer;
    if (ptPlayerMe == NULL)
    {
        CTradeCancel(ptPlayerMe, TRUE, TRUE);
        return;
    }

    const Player * ptPlayerOther = PlayerGetByACD(ptPlayerMe->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        CTradeCancel(ptPlayerMe, TRUE, TRUE);
        return;
    }

    ActorCommonData * ptACDFirst = ACDTryToGet(ptPlayerMe->idPlayerACD);
    ActorCommonData * ptACDSecond = ACDTryToGet(ptPlayerOther->idPlayerACD);
    if (ptACDFirst == NULL || ptACDSecond == NULL)
    {
        CTradeCancel(ptPlayerMe, TRUE, TRUE);
        return;
    }

    if (::Console::Screens::GetActionManager().AreActionsDirty())
    {
        UpdateButtonLegend();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }

    UpdateAcceptButtonCountdown();
    ::Console::Screens::UpdateButtonLegendText();

    if (sgtUITradeGlobals.m_eState == TRADEUI_STATE_WAITING_FOR_ACDS)
    {
        CheckForKnownItems();
    }

    sgtUITradeGlobals.m_tMine.Update(ptPlayerMe, FALSE);
    sgtUITradeGlobals.m_tOther.Update(ptPlayerOther, TRUE);
}

BOOL OfferItem(const ACDID idACDItem)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return FALSE;
    }

    Player * ptPlayerTo = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (!ItemIsTradeable(*sptPlayer, ptPlayerTo, *ptACDItem, TRUE))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_CANNOT_TRADE), FALSE, NULL);
        return FALSE;
    }

    sAddTradeItem(*ptACDItem);

    UIConsoleListBox & tListBoxMine = UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tMine.m_idListBox);
    tListBoxMine.ResetAllItems();
    return TRUE;
}

void UpdateOfferLists(const BOOL fLocalPlayerTradeDataChanged)
{
    const Player * ptPlayerOther = PlayerGetByACD(sptPlayer->tTradeData.idACDPlayerTradingWith);
    if (ptPlayerOther == NULL)
    {
        return;
    }

    if (fLocalPlayerTradeDataChanged)
    {
        sgtUITradeGlobals.m_tMine.SetShowDetails(FALSE);
    }
    else
    {
        int32 nEntryCount = 0;
        ListViewDataSource * ptDataSrc = &sgtUITradeGlobals.m_tOther.m_tDataSrc;
        for (int32 i = 0; i < 4; i++)
        {
            ptDataSrc->tItems[i].ann = ANN_INVALID;
            ptDataSrc->tItems[i].ptACD = NULL;

            if (ptPlayerOther->tTradeData.arrItems[i] != ANN_INVALID)
            {
                ListViewDataSource::ItemEntry & tEntryDst = ptDataSrc->tItems[nEntryCount++];
                tEntryDst.ann = ptPlayerOther->tTradeData.arrItems[i];
            }
        }

        sgtUITradeGlobals.m_nNumUnknownTradeACDs = nEntryCount;
        sgtUITradeGlobals.m_tOther.SetShowDetails(FALSE);

        if (UI_GetWindowFromID(sgtUITradeGlobals.m_tMine.m_idWnd).IsActive() && nEntryCount == 0)
        {
            UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tOther.m_idListBox).SetSelectedIndex(-1);
            UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tOther.m_idDetailsList).SetSelectedIndex(-1);
            sgtUITradeGlobals.m_tMine.SetFocus();
        }
    }

    if (sgtUITradeGlobals.m_nNumUnknownTradeACDs > 0)
    {
        sgtUITradeGlobals.m_eState = TRADEUI_STATE_WAITING_FOR_ACDS;
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

static BOOL sCanToggleList()  // line 1349
{
    TradeView * ptView = sgtUITradeGlobals.GetCurrentView();
    UIID idListBox = ptView->m_idListBox;
    if (idListBox == GetUIIDNone())
    {
        return FALSE;
    }

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(idListBox);
    int32 nSelIndex = tListBox.GetSelectedIndex();
    return nSelIndex != -1;
}

void UpdateButtonLegend()
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    InputClearRemappings();

    tActionManager.SetAction(7, ACTION_CLOSE, StringListFetch("ConsoleUI:Close").str());

    if (sgtUITradeGlobals.m_eState == TRADEUI_STATE_WAITING_FOR_ACDS)
    {
        return;
    }

    if (sCanToggleList())
    {
        TradeView * ptView = sgtUITradeGlobals.GetCurrentView();
        if (ptView->IsShowingDetails())
        {
            tActionManager.SetAction(5, ACTION_LIST, StringListFetch("ConsoleUI:List").str());

            if (ptView->CanScrollDetails())
            {
                CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
                tActionManager.SetAction(10, ACTION_SCROLL_LIST, sScrollPrompt);
                tActionManager.SetAction(11, ACTION_SCROLL_LIST, sScrollPrompt);
            }
        }
        else
        {
            tActionManager.SetAction(5, ACTION_DETAILS, StringListFetch("ConsoleUI:Details").str());
        }
    }

    if (sgtUITradeGlobals.GetCurrentView() != &sgtUITradeGlobals.m_tOther)
    {
        tActionManager.SetAction(6, ACTION_ADD_ITEM, StringListFetch("ConsoleTrade:OfferItem").str());

        if (UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tMine.m_idListBox).GetSelectedIndex() != -1 ||
            UI_GetConsoleListBoxFromID(sgtUITradeGlobals.m_tMine.m_idDetailsList).GetSelectedIndex() != -1)
        {
            InputRemapSingleJoyconButton(8, 14);
            tActionManager.SetAction(21, ACTION_REMOVE_ITEM, StringListFetch("ConsoleTrade:RemoveItem").str());
        }
    }

    if (sptPlayer != NULL)
    {
        ActorCommonData & tACDPlayer = ACDGet(sptPlayer->idPlayerACD);
        if (tACDPlayer.GetCurrencyAmount(0) > 0)
        {
            InputRemapSingleJoyconButton(11, 15);
            tActionManager.SetAction(22, ACTION_OFFER_GOLD, StringListFetch("ConsoleTrade:OfferGold").str());
        }
    }
}

void UpdateAcceptButtonCountdown()
{
    if (sCanAccept() && (sptPlayer == NULL || !sptPlayer->tTradeData.bTradeAccepted))
    {
        CRefString sAcceptCommand = StringListFetch("ConsoleTrade:Accept");

        uint32 uNow = CGameGetClientTicks();
        if (uNow < sgtUITradeGlobals.m_uNextAcceptTime)
        {
            sAcceptCommand += CRefString().Format(" (%s)", UnicodeText::FormatNumber((uint64)((sgtUITradeGlobals.m_uNextAcceptTime - uNow) / 60 + 1), TRUE, FNS_NORMAL, 0).str());
            Set_Text_Color(sAcceptCommand, RGBAColor(0x40, 0x40, 0x40));
        }

        ::Console::Screens::GetActionManager().SetAction(4, ACTION_ACCEPT, sAcceptCommand.str());
    }
    else
    {
        ::Console::Screens::GetActionManager().ClearAction(4);
    }
}

BOOL HandleLeftStickViewSwitching(const int32 nControllerIndex)
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    ::Console::AnalogStickInputFilter::Rule::ID idRule = sgtUITradeGlobals.m_tStickInputFilter.HandleInput(vecStick, FALSE);

    switch (idRule)
    {
    case ::Console::RULE_STICK_RIGHT:
        return sSwitchFocusToOther();

    case ::Console::RULE_STICK_LEFT:
        return sSwitchFocusToMine();

    case ::Console::RULE_STICK_UP:
    case ::Console::RULE_STICK_DOWN:
    {
        TradeView * ptView = sgtUITradeGlobals.GetCurrentView();
        if (ptView->IsShowingDetails())
        {
            UIConsoleListBox & tDetailsListBox = UI_GetConsoleListBoxFromID(ptView->GetDetailsListBox());
            if (idRule == ::Console::RULE_STICK_UP)
            {
                tDetailsListBox.SelectPreviousItem(TRUE);
            }
            else
            {
                tDetailsListBox.SelectNextItem(TRUE);
            }

            int32 nSelIndex = tDetailsListBox.GetSelectedIndex();
            UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(ptView->GetListBox());
            tListBox.SetSelectedIndex(nSelIndex);
            ptView->UpdateItemDetailsUI(*sptPlayer);
            return TRUE;
        }
        break;
    }

    default:
        break;
    }

    return FALSE;
}

void OnLeftStick(const UIID &, const int32 nControllerIndex)
{
    if (sptPlayer != NULL && sptPlayer->nControllerIndex == nControllerIndex)
    {
        HandleLeftStickViewSwitching(nControllerIndex);
    }
}

}  // namespace Console
}  // namespace UITrade
