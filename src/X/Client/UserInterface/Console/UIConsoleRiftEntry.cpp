// Original file: X/Client/UserInterface/Console/UIConsoleRiftEntry.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
// Not in the DWARF and chosen by what compiles to the original code: `::new`/`::delete` for the
// globals and data sources, the member-initialiser layout of RiftEntryGlobals, the value 4 of
// RiftSelectionDataSource::GetNumItems, and the button indices 7/6 passed to IsButtonDown.
#include "UIConsoleRiftEntry.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Console/ControllerUtils.h"
#include "../../Sound/Sound.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleUtils.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
extern "C" int32 sub_710051abb0(Player * ptPlayer);  // NephalemRiftCommon.cpp

// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UIDLG_RIFT_ENTRY_MAIN, UIWND_RIFT_ENTRY_LIST_VIEW, UILIST_RIFT_ENTRY_TYPE_SELECTION, UIWND_RIFT_ENTRY_RIGHT_REGULAR_RIFT_ROOT, UIWND_RIFT_ENTRY_RIGHT_GRIFT_ROOT, UIWND_RIFT_ENTRY_RIGHT_GRIFT_MODE_PANEL, UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_LIST, UITEXT_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_INFO, UITEXT_RIFT_ENTRY_RIGHT_GRIFT_EMPOWER_COST;  // X/Client/UserInterface/UIGlobals.cpp
extern UIID UITEXT_RIFT_ENTRY_RIGHT_GRIFT_KEYSTONES_AVAILABLE, UITEXT_RIFT_ENTRY_RIGHT_GRIFT_NO_KEYSTONES_AVAILABLE, UITEXT_RIFT_ENTRY_RIGHT_TITLE, UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION, UILIST_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION, UISCROLL_RIFT_ENTRY_GREATER_RIFT_EMPOWERED, UISCROLL_RIFT_ENTRY_REGULAR_RIFT_DESCRIPTION, UISCROLL_RIFT_ENTRY_GREATER_RIFT_DESCRIPTION, UITXT_CONSOLE_BUTTON_LEGEND_TEXT;  // X/Client/UserInterface/UIGlobals.cpp
UIWindow & UI_GetWindowFromID(const UIID & id); UIText & UI_GetTextFromID(const UIID & id); UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);  // X/Common/Definitions/StringListDefinition.cpp
namespace UIUtils { namespace Console { BOOL CanScrollContentInScrollBox(const UIID & idScrollBox); } } /* UIConsoleUtils.cpp */ class UITutorial;
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr; /* X/Client/UserInterface/Interface/UITutorials.h (only the member this file calls) */ namespace UIConsoleRiftEntry { void InitializeForGame(); void DisposeForGame(); void OpenForController(int32 nControllerIndex); void Close(); void OnUpdate(const UIID & id); void OnLeftStick(const UIID & id, const int32 nControllerIndex); void OnButton1(const UIID & id, const int32 nControllerIndex); void OnButton2(const UIID & id, const int32 nControllerIndex); void OnButton3(const UIID & id, const int32 nControllerIndex); void OnButton4(const UIID & id, const int32 nControllerIndex); void OnButtonL2(const UIID & id, const int32 nControllerIndex); void OnButtonR2(const UIID & id, const int32 nControllerIndex); BOOL PlayerInstigatorEmpoweredRift(int32 nPlayerIndex); }
static void sUpdateControllerActions(); static BOOL TryToStartRift(); static void sGoBack(); static void sConfirmSelection(); static BOOL sCanScrollText(); static void sScrollText(float flDelta);

enum ButtonActions {
    ACTION_SELECT = 1,
    ACTION_CONFIRM = 2,
    ACTION_DETAILS = 3,
    ACTION_LIST = 4,
    ACTION_SCROLL_LIST = 5,
    ACTION_CLOSE = 6,
    ACTION_BACK = 7,
    ACTION_EMPOWER = 8,
};

enum LevelScrollDirection {
    SCROLL_NONE = 0,
    SCROLL_UP = 1,
    SCROLL_DOWN = 2,
};

enum RiftEntryState {
    SELECTING_RIFT_TYPE = 0,
    PICKING_RIFT_LEVEL = 1,
};

enum RiftSelectionIndexes {
    REGULAR_RIFT_LISTBOX_INDEX = 0,
    HIGHEST_GREATER_RIFT_LISTBOX_INDEX = 1,
    LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX = 2,
    SELECT_GREATER_RIFT_LISTBOX_INDEX = 3,
};





static LPCSTR CENTRE_LIST_BACKGROUND_RELATIVE_PATH_ROOT = ".LayoutRoot.Backgrounds";
static LPCSTR CENTRE_LIST_BACKGROUND_RELATIVE_PATH_REGULAR_RIFT = ".LayoutRoot.Backgrounds.NephalemRift";
static LPCSTR CENTRE_LIST_BACKGROUND_RELATIVE_PATH_HIGHEST_GRIFT = ".LayoutRoot.Backgrounds.GriftHighest";
static LPCSTR CENTRE_LIST_BACKGROUND_RELATIVE_PATH_LAST_PLAYED_GRIFT = ".LayoutRoot.Backgrounds.GriftLastRift";
static LPCSTR CENTRE_LIST_BACKGROUND_RELATIVE_PATH_CHOOSE_GRIFT = ".LayoutRoot.Backgrounds.GriftChooseRift";

static LPCSTR CENTRE_LIST_TITLE_RELATIVE_PATH_NORMAL_RIFT = ".LayoutRoot.Backgrounds.NephalemRift.Title";
static LPCSTR CENTRE_LIST_TITLE_RELATIVE_PATH_HIGHEST_GRIFT = ".LayoutRoot.Backgrounds.GriftHighest.Title";
static LPCSTR CENTRE_LIST_TITLE_RELATIVE_PATH_LAST_PLAYED_GRIFT = ".LayoutRoot.Backgrounds.GriftLastRift.Title";
static LPCSTR CENTRE_LIST_TITLE_RELATIVE_PATH_CHOOSE_GRIFT = ".LayoutRoot.Backgrounds.GriftChooseRift.Title";

static LPCSTR RIGHT_LIST_RELATIVE_PATH_TEXT = ".LayoutRoot.RiftText.RiftLevelText";

static LPCSTR RIGHT_LIST_RELATIVE_PATH_EMPOWERED_GLOW = ".LayoutRoot.Backgrounds.RiftListItemEmpowered";

struct RiftEntryGlobals {  // size 0x38
    RiftEntryGlobals();
    ~RiftEntryGlobals();
    int32 nControllerIndex;  // 0x00
    UIConsoleRiftEntry::RiftSelectionDataSource * ptRiftSelectionDataSource;  // 0x08
    UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource * ptGriftLevelSelectionDataSource;  // 0x10
    RiftEntryState eState;  // 0x18
    int32 nSelectedRiftLevel;  // 0x1c
    int32 nLastRiftSelectedIndex;  // 0x20
    uint32 nScrollingTicks;  // 0x24
    BOOL bCheckForAccelleration;  // 0x28
    LevelScrollDirection eScrollDir;  // 0x2c
    BOOL bEmpower;  // 0x30
};





















RiftEntryGlobals * sgptRiftEntryGlobals = NULL;




























static void RiftSelectionChangedCallback(const UIID & idItemUI, const UIInventory::Console::ItemCategory eCategory)
{

}




UIConsoleRiftEntry::RiftSelectionDataSource::RiftSelectionDataSource(UIInventory::Console::CategoryItemUIUpdateCallback tUpdate)
    : UIInventory::Console::CategoryViewDataSource(tUpdate)
{
}



int32 UIConsoleRiftEntry::RiftSelectionDataSource::GetNumItems() const
{

    return 4;
}


UIControlSourceKey UIConsoleRiftEntry::RiftSelectionDataSource::GetSourceKeyForItem(const int32 nIndex) const
{

    const LPCSTR CATEGORY_ITEM_TEMPLATE = "ConsoleInventoryListCategoryTemplate:ConsoleInventoryListCategoryTemplate_main";

    const UIControlSourceKey CATEGORY_ITEM_SOURCE_KEY = UIControlSourceKey::Parse(CATEGORY_ITEM_TEMPLATE, SNO_NONE, NULL, NULL);

    return CATEGORY_ITEM_SOURCE_KEY;
}


void UIConsoleRiftEntry::RiftSelectionDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)



{




    UIID uidParentWindow = UIIDCreateFromParent(idItemUI, CENTRE_LIST_BACKGROUND_RELATIVE_PATH_ROOT);
    UIWindow & tWindowParent = UI_GetWindowFromID(uidParentWindow);
    const UIControlItemArray & arChildren = tWindowParent.GetChildList();
    for (uint32 i = 0; i < arChildren.size(); i++)
    {
        UIID idCurrentChild = arChildren[i].id;
        if (UI_TestID(idCurrentChild))
        {
            UI_GetWindowFromID(idCurrentChild).SetActive(FALSE);
        }
    }

    UIID uidToShow = GetUIIDNone();
    switch (nIndex)
    {
    case REGULAR_RIFT_LISTBOX_INDEX:
        uidToShow = UIIDCreateFromParent(idItemUI, CENTRE_LIST_BACKGROUND_RELATIVE_PATH_REGULAR_RIFT);
        break;

    case HIGHEST_GREATER_RIFT_LISTBOX_INDEX:
        uidToShow = UIIDCreateFromParent(idItemUI, CENTRE_LIST_BACKGROUND_RELATIVE_PATH_HIGHEST_GRIFT);
        break;

    case LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX:
        uidToShow = UIIDCreateFromParent(idItemUI, CENTRE_LIST_BACKGROUND_RELATIVE_PATH_LAST_PLAYED_GRIFT);
        break;

    case SELECT_GREATER_RIFT_LISTBOX_INDEX:
        uidToShow = UIIDCreateFromParent(idItemUI, CENTRE_LIST_BACKGROUND_RELATIVE_PATH_CHOOSE_GRIFT);
        break;
    }




    UIWindow & tWindowToShow = UI_GetWindowFromID(uidToShow);
    tWindowToShow.SetActive(TRUE);

    UIInventory::Console::CategoryViewDataSource::InitializeItemUI(tListBox, idItemUI, nIndex);
}



void UIConsoleRiftEntry::RiftSelectionDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)



{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptRiftEntryGlobals->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    UIID uidTitle = GetUIIDNone();
    CRefString sTitleStringlistEntry;
    switch (nIndex)
    {
    case REGULAR_RIFT_LISTBOX_INDEX:
        uidTitle = UIIDCreateFromParent(idItemUI, CENTRE_LIST_TITLE_RELATIVE_PATH_NORMAL_RIFT);
        sTitleStringlistEntry = StringListFetch("ConsoleRifts:p_ConsoleTieredNephalemRift");
        break;
    case HIGHEST_GREATER_RIFT_LISTBOX_INDEX:
        uidTitle = UIIDCreateFromParent(idItemUI, CENTRE_LIST_TITLE_RELATIVE_PATH_HIGHEST_GRIFT);
        sTitleStringlistEntry = StringListFetch("ConsoleRifts:p_ConsoleTieredGRift");
        break;
    case LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX:
        uidTitle = UIIDCreateFromParent(idItemUI, CENTRE_LIST_TITLE_RELATIVE_PATH_LAST_PLAYED_GRIFT);
        sTitleStringlistEntry = StringListFetch("ConsoleRifts:p_ConsoleTieredGRift");
        break;
    case SELECT_GREATER_RIFT_LISTBOX_INDEX:
        uidTitle = UIIDCreateFromParent(idItemUI, CENTRE_LIST_TITLE_RELATIVE_PATH_CHOOSE_GRIFT);
        sTitleStringlistEntry = StringListFetch("ConsoleRifts:p_ConsoleTieredGRift");
        break;
    }



    if (uidTitle != GetUIIDNone())
    {
        UI_GetTextFromID(uidTitle).SetText(sTitleStringlistEntry.str(), FALSE);
    }

    UIInventory::Console::CategoryViewDataSource::UpdateItemUI(tListBox, idItemUI, nIndex);
}






UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource::GreaterRiftLevelSelectionDataSource(UIInventory::Console::CategoryItemUIUpdateCallback tUpdate)
    : UIInventory::Console::CategoryViewDataSource(tUpdate)
{
}



int32 UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource::GetNumItems() const

{
    if (sgptRiftEntryGlobals == NULL || sgptRiftEntryGlobals->nControllerIndex == -1)
    {
        return 1;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(sgptRiftEntryGlobals->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return 1;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return 1;
    }

    int32 nHighestUnlockedLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL);
    return nHighestUnlockedLevel > 1 ? nHighestUnlockedLevel : 1;
}


UIControlSourceKey UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource::GetSourceKeyForItem(const int32 nIndex) const
{

    const LPCSTR CATEGORY_ITEM_TEMPLATE = "ConsoleTieredRift_EntryLevelListTemplate:ConsoleTieredRift_EntryLevelListTemplate_main";

    const UIControlSourceKey CATEGORY_ITEM_SOURCE_KEY = UIControlSourceKey::Parse(CATEGORY_ITEM_TEMPLATE, SNO_NONE, NULL, NULL);

    return CATEGORY_ITEM_SOURCE_KEY;
}


void UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)



{

    UIID uidEmpoweredGlow = UIIDCreateFromParent(idItemUI, RIGHT_LIST_RELATIVE_PATH_EMPOWERED_GLOW);
    UI_GetWindowFromID(uidEmpoweredGlow).SetTransparency(sgptRiftEntryGlobals->bEmpower ? 255 : 0);
}


void UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)



{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptRiftEntryGlobals->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    UIID uidTextLevel = UIIDCreateFromParent(idItemUI, RIGHT_LIST_RELATIVE_PATH_TEXT);
    UI_GetTextFromID(uidTextLevel).SetActive(TRUE);



    int32 nRiftLevelToDisplay = tListBox.GetNumItems() - nIndex;


    UIID uidEmpoweredGlow = UIIDCreateFromParent(idItemUI, RIGHT_LIST_RELATIVE_PATH_EMPOWERED_GLOW);

    CRefString sEmpowerCost;

    if (sgptRiftEntryGlobals->bEmpower)
    {
        uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(nRiftLevelToDisplay - 1);
        sEmpowerCost = UIText_ValueToGameCurrency(nEmpowerCost, 0);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && ptACDPlayer->GetCurrencyAmount(0) < (int64)nEmpowerCost)
        {
            Set_Text_Color(sEmpowerCost, 1);
        }
        UI_GetWindowFromID(uidEmpoweredGlow).SetTransparency(255);
    }
    else
    {
        UI_GetWindowFromID(uidEmpoweredGlow).SetTransparency(0);
    }

    CRefString sTierNumber;
    switch (nRiftLevelToDisplay)
    {
    case 1: sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryHardConsole").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 4:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryExpertConsole").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 7:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryMasterConsole").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 10:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment1Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 13:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment2Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 16:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment3Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 19:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment4Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 22:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment5Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 25:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment6Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 30:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment7Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 35:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment8Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 40:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment9Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 45:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment10Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 50:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment11Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 55:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment12Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 60:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment13Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 65:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment14Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 70:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment15Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    case 75:
        sTierNumber = LocalizedSprintf(StringListFetch("NephalemRift:TieredRiftLevelSelectionEntryTorment16Console").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;

    default:
        sTierNumber = LocalizedSprintf(StringListFetch("ConsoleRifts:p_ConsoleTieredRiftLevelSelectEntry").str(),
            UnicodeText::FormatNumber(nRiftLevelToDisplay, TRUE, FNS_NORMAL, 0).str());
        break;
    }
    sTierNumber = sTierNumber + CRefString("\n") + sEmpowerCost;

    UI_GetTextFromID(uidTextLevel).SetText(sTierNumber.str(), FALSE);

    UIInventory::Console::CategoryViewDataSource::UpdateItemUI(tListBox, idItemUI, nIndex);
}





RiftEntryGlobals::RiftEntryGlobals()
    : nControllerIndex(-1)
    , eState(SELECTING_RIFT_TYPE)
    , nSelectedRiftLevel(-1)
    , nLastRiftSelectedIndex(0)
    , nScrollingTicks(0)
    , bCheckForAccelleration(FALSE)
    , eScrollDir(SCROLL_NONE)
    , bEmpower(FALSE)



{
    ptRiftSelectionDataSource = ::new UIConsoleRiftEntry::RiftSelectionDataSource(RiftSelectionChangedCallback);
    ptGriftLevelSelectionDataSource = ::new UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource(RiftSelectionChangedCallback);
}

RiftEntryGlobals::~RiftEntryGlobals()

{
    ::delete ptRiftSelectionDataSource; ptRiftSelectionDataSource = NULL;
    ::delete ptGriftLevelSelectionDataSource; ptGriftLevelSelectionDataSource = NULL;
}





static BOOL sIsActive()
{

    return UI_GetWindowFromID(UIDLG_RIFT_ENTRY_MAIN).IsActive();
}




void UIConsoleRiftEntry::InitializeForGame()

{

    sgptRiftEntryGlobals = ::new RiftEntryGlobals;
}




void UIConsoleRiftEntry::DisposeForGame()

{

    ::delete sgptRiftEntryGlobals; sgptRiftEntryGlobals = NULL;
}















static void sResetEmpoweredUIState()
{

    UIConsoleListBox & tRiftTypeListBox = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);

    for (int32 index = 1; index < tRiftTypeListBox.GetNumItems(); index++)
    {
        UI_GetStoryboardFromID(UIIDCreateFromParent(tRiftTypeListBox.GetItemUIID(index), ".Empowered")).Reset();
    }

    UI_GetWindowFromID(UISCROLL_RIFT_ENTRY_GREATER_RIFT_EMPOWERED).SetActive(FALSE);
}



void UIConsoleRiftEntry::OpenForController(int32 nControllerIndex)

{
    if (sIsActive())
    {
        return;
    }


    sgptRiftEntryGlobals->eState = SELECTING_RIFT_TYPE;
    sgptRiftEntryGlobals->nControllerIndex = nControllerIndex;
    sgptRiftEntryGlobals->nSelectedRiftLevel = -1;
    sgptRiftEntryGlobals->nLastRiftSelectedIndex = 0;
    sgptRiftEntryGlobals->bEmpower = FALSE;

    sResetEmpoweredUIState();


    UIConsoleListBox & tCentreListBox = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
    tCentreListBox.SetDataSource(*sgptRiftEntryGlobals->ptRiftSelectionDataSource);
    tCentreListBox.ResetAllItems();

    UIConsoleListBox & tRightListBox = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION);
    tRightListBox.SetDataSource(*sgptRiftEntryGlobals->ptGriftLevelSelectionDataSource);
    tRightListBox.ResetAllItems();

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);


    ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_RIFT_ENTRY_MAIN, FALSE, GetUIIDNone());

    UI_SetControllerFocus(UILIST_RIFT_ENTRY_TYPE_SELECTION);





}

















static void sResetUI()
{

    UI_SetControllerFocus(GetUIIDNone());
}




void UIConsoleRiftEntry::Close()

{
    sResetUI();

    ::Console::Screens::CloseModalDialog(UIDLG_RIFT_ENTRY_MAIN);
}









void UIConsoleRiftEntry::OnUpdate(const UIID & id)

{
    if (!sIsActive())
    {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(sgptRiftEntryGlobals->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return;
    }



    UI_GetWindowFromID(UIWND_RIFT_ENTRY_LIST_VIEW).SetActive(TRUE);

    UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
    int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();

    if (nSelectedIndex != sgptRiftEntryGlobals->nLastRiftSelectedIndex)
    {
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        sgptRiftEntryGlobals->nLastRiftSelectedIndex = nSelectedIndex;
    }


    switch (nSelectedIndex)
    {
    case REGULAR_RIFT_LISTBOX_INDEX:
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_ROOT).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_MODE_PANEL).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_LIST).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_REGULAR_RIFT_ROOT).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION).SetActive(FALSE);
        UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_TITLE).SetText(StringListFetch("ConsoleRifts:p_ConsoleTieredNephalemRift").str(), FALSE);
        UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_EMPOWER_COST).SetActive(FALSE);
        break;

    case HIGHEST_GREATER_RIFT_LISTBOX_INDEX:
    case LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX:
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_ROOT).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_MODE_PANEL).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_LIST).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_REGULAR_RIFT_ROOT).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION).SetActive(FALSE);
        {
            int32 nRiftLevel = -1;

            if (sgptRiftEntryGlobals->bEmpower)
            {
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_TITLE).SetText(StringListFetch("ConsoleRifts:p_ConsoleEmpoweredGRift").str(), FALSE);
            }
            else
            {
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_TITLE).SetText(StringListFetch("ConsoleRifts:p_ConsoleTieredGRift").str(), FALSE);
            }

            if (nSelectedIndex == HIGHEST_GREATER_RIFT_LISTBOX_INDEX)
            {
                nRiftLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL);
                CRefString sTitleStringlistEntry = LocalizedSprintf(StringListFetch("ConsoleRifts:p_ConsoleTieredGRiftLevel").str(), UnicodeText::FormatNumber(nRiftLevel, TRUE, FNS_NORMAL, 0).str());
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_INFO).SetText(sTitleStringlistEntry.str(), FALSE);
            }
            else if (nSelectedIndex == LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX)
            {

                nRiftLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LAST_RIFT_STARTED_LEVEL) + 1;
                CRefString sTitleStringlistEntry = LocalizedSprintf(StringListFetch("ConsoleRifts:p_ConsoleTieredGRiftLevel").str(), UnicodeText::FormatNumber(nRiftLevel, TRUE, FNS_NORMAL, 0).str());
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_INFO).SetText(sTitleStringlistEntry.str(), FALSE);
            }


            UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_EMPOWER_COST).SetActive(sgptRiftEntryGlobals->bEmpower);

            uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(nRiftLevel - 1);
            CRefString sEmpowerCost = UIText_ValueToGameCurrency(nEmpowerCost, 0);
            if ((uint64)ptACDPlayer->GetCurrencyAmount(0) < nEmpowerCost)
            {
                Set_Text_Color(sEmpowerCost, 1);
            }
            UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_EMPOWER_COST).SetText(sEmpowerCost, FALSE);


            if (ACDIsParticipatingInSetDungeon(*ptACDPlayer))
            {
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_KEYSTONES_AVAILABLE).SetActive(FALSE);
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_NO_KEYSTONES_AVAILABLE).SetActive(TRUE);
                UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_NO_KEYSTONES_AVAILABLE).SetText(
                    StringListFetch("ConsoleRifts:p_ConsoleTieredRiftInSetDungeon").str(), FALSE);
            }
            else
            {

                int32 nTotalKeys = sub_710051abb0(ptPlayer);
                if (nTotalKeys > 0)
                {
                    UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_KEYSTONES_AVAILABLE).SetActive(TRUE);
                    UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_NO_KEYSTONES_AVAILABLE).SetActive(FALSE);

                    UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_KEYSTONES_AVAILABLE).SetText(
                        LocalizedSprintf(StringListFetch("ConsoleRifts:p_ConsoleTieredRiftKeystonesAvailable").str(),
                                         UnicodeText::FormatNumber(nTotalKeys, TRUE, FNS_NORMAL, 0).str()), FALSE);
                }
                else
                {
                    UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_KEYSTONES_AVAILABLE).SetActive(FALSE);
                    UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_NO_KEYSTONES_AVAILABLE).SetActive(TRUE);
                    UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_NO_KEYSTONES_AVAILABLE).SetText(
                        StringListFetch("ConsoleRifts:p_ConsoleTieredRiftNoKeysAvailable").str(), FALSE);
                }
            }
        }
        break;
    case SELECT_GREATER_RIFT_LISTBOX_INDEX:
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_ROOT).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_MODE_PANEL).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_LIST).SetActive(TRUE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_REGULAR_RIFT_ROOT).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION).SetActive(TRUE);
        UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_GRIFT_EMPOWER_COST).SetActive(FALSE);

        if (sgptRiftEntryGlobals->bEmpower)
        {
            UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_TITLE).SetText(StringListFetch("ConsoleRifts:p_ConsoleEmpoweredGRift").str(), FALSE);
        }
        else
        {
            UI_GetTextFromID(UITEXT_RIFT_ENTRY_RIGHT_TITLE).SetText(StringListFetch("ConsoleRifts:p_ConsoleTieredGRift").str(), FALSE);
        }
        break;
    }

    if (nSelectedIndex != REGULAR_RIFT_LISTBOX_INDEX)
    {
        gtTutorialMgr.ShowTutorial(UITXT_CONSOLE_BUTTON_LEGEND_TEXT, GlobalSNOGet(GLOBALSNO_TUTORIAL_TIERED_LOOT_RUN_EMPOWER), NULL, TRUE);
    }


    if (::Console::Screens::GetActionManager().AreActionsDirty())
    {
        sUpdateControllerActions();
        ::Console::Screens::UpdateButtonLegendText();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }

    if (sgptRiftEntryGlobals->eState == PICKING_RIFT_LEVEL)
    {
        UIConsoleListBox & tRiftLevelSelect = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION);

        if (!::Console::ControllerUtils::LStickIsInDeadZone(sgptRiftEntryGlobals->nControllerIndex, FALSE))
        {
            Vector2D vStickPos = ::Console::ControllerUtils::GetRawLStickPosition(sgptRiftEntryGlobals->nControllerIndex, FALSE);



            if (!sgptRiftEntryGlobals->bCheckForAccelleration ||
                (vStickPos.y > 0.0f && sgptRiftEntryGlobals->eScrollDir == SCROLL_DOWN) ||
                (vStickPos.y < 0.0f && sgptRiftEntryGlobals->eScrollDir == SCROLL_UP))
            {

                sgptRiftEntryGlobals->nScrollingTicks = 0;
                sgptRiftEntryGlobals->bCheckForAccelleration = TRUE;
                if (vStickPos.y > 0.0f)
                {
                    sgptRiftEntryGlobals->eScrollDir = SCROLL_UP;
                }
                else
                {
                    sgptRiftEntryGlobals->eScrollDir = SCROLL_DOWN;
                }
                tRiftLevelSelect.ResetAnalogMinimumInputDelay();
            }

            int32 TIME_UNTIL_DOUBLE_SPEED_SCROLL_TICKS = 120;

            uint32 nBaseScrollSpeed = tRiftLevelSelect.GetAnalogStickMinimumInputDelay();
            sgptRiftEntryGlobals->nScrollingTicks++;

            if ((int32)sgptRiftEntryGlobals->nScrollingTicks >= TIME_UNTIL_DOUBLE_SPEED_SCROLL_TICKS)
            {
                tRiftLevelSelect.SetAnalogStickMinimumInputDelay(nBaseScrollSpeed / 2);
            }
        }
        else
        {
            if (sgptRiftEntryGlobals->bCheckForAccelleration)
            {
                sgptRiftEntryGlobals->nScrollingTicks = 0;
                sgptRiftEntryGlobals->bCheckForAccelleration = FALSE;
                tRiftLevelSelect.ResetAnalogMinimumInputDelay();
            }
        }
    }

}



void UIConsoleRiftEntry::OnLeftStick(const UIID & id, const int32 nControllerIndex)
{




}




void UIConsoleRiftEntry::OnButton1(const UIID & id, const int32 nControllerIndex)


{
    if (sgptRiftEntryGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    if (idAction == ACTION_SELECT)
    {
        if (sgptRiftEntryGlobals->eState == SELECTING_RIFT_TYPE)
        {
            UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
            int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();

            BOOL bCanStart = TRUE;


            if (nSelectedIndex != SELECT_GREATER_RIFT_LISTBOX_INDEX)
            {
                bCanStart = TryToStartRift();
            }

            if (bCanStart)
            {
                UI_SetControllerFocus(UILIST_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION);
                sgptRiftEntryGlobals->eState = PICKING_RIFT_LEVEL;
                ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
            }
        }
        else if (sgptRiftEntryGlobals->eState == PICKING_RIFT_LEVEL)
        {
            TryToStartRift();
        }
    }

}




void UIConsoleRiftEntry::OnButton2(const UIID & id, const int32 nControllerIndex)


{
    if (sgptRiftEntryGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    if (idAction == ACTION_CLOSE)
    {
        ::Console::Screens::CloseScreenSet(TRUE, TRUE);
    }
    else if (idAction == ACTION_BACK)
    {
        sGoBack();
    }
}




void UIConsoleRiftEntry::OnButton3(const UIID & id, const int32 nControllerIndex)


{
    if (sgptRiftEntryGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    if (idAction != ACTION_DETAILS && idAction != ACTION_LIST)
    {
        return;
    }

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
}




void UIConsoleRiftEntry::OnButton4(const UIID & id, const int32 nControllerIndex)


{
    if (sgptRiftEntryGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_4);
    if (idAction != ACTION_EMPOWER)
    {
        return;
    }

    sgptRiftEntryGlobals->bEmpower = !sgptRiftEntryGlobals->bEmpower;

    UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
    int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();

    UIConsoleListBox & tCentreListBox = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);

    for (int32 index = 1; index < 4; index++)
    {
        UIID idStoryboard;

        if (sgptRiftEntryGlobals->bEmpower)
        {
            idStoryboard.SetFromParent(tCentreListBox.GetItemUIID(index), "Empowered");
        }
        else
        {
            idStoryboard.SetFromParent(tCentreListBox.GetItemUIID(index), "EmpoweredOff");
        }

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);
    }

    UI_GetWindowFromID(UISCROLL_RIFT_ENTRY_GREATER_RIFT_EMPOWERED).SetActive(sgptRiftEntryGlobals->bEmpower);


    UIConsoleListBox & tRightListBox = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION);



    if (nSelectedIndex == SELECT_GREATER_RIFT_LISTBOX_INDEX)
    {
        for (int32 index = 0; index < tRightListBox.GetNumVisibleItems(); index++)
        {
            int32 nVisibleIndex = tRightListBox.GetFirstVisibleItemIndex() + index;

            UIID idStoryboard;

            if (sgptRiftEntryGlobals->bEmpower)
            {
                idStoryboard.SetFromParent(tRightListBox.GetItemUIID(nVisibleIndex), "Empowered");
            }
            else
            {
                idStoryboard.SetFromParent(tRightListBox.GetItemUIID(nVisibleIndex), "EmpoweredOff");
            }

            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
            tStoryboard.Stop();
            tStoryboard.Play(FALSE, 0.0f);
        }
    }

    tRightListBox.RefreshVisibleItems();

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
}




void UIConsoleRiftEntry::OnButtonL2(const UIID & id, const int32 nControllerIndex)


{
    if (sgptRiftEntryGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);
    if (idAction == ACTION_SCROLL_LIST)
    {

        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
        {
            sScrollText(-4.0f);
        }
    }
}




void UIConsoleRiftEntry::OnButtonR2(const UIID & id, const int32 nControllerIndex)


{
    if (sgptRiftEntryGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER);  // the original asks for the left trigger's action here too
    if (idAction == ACTION_SCROLL_LIST)
    {

        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
        {
            sScrollText(4.0f);
        }
    }
}




BOOL UIConsoleRiftEntry::PlayerInstigatorEmpoweredRift(int32 nPlayerIndex)

{
    if (PlayerIsLocal(nPlayerIndex))
    {
        int32 nControllerIndex = PlayerGetByPlayerIndex(nPlayerIndex)->nControllerIndex;

        if (nControllerIndex == sgptRiftEntryGlobals->nControllerIndex)
        {
            return sgptRiftEntryGlobals->bEmpower;
        }
    }

    return FALSE;
}














static void sUpdateControllerActions()
{

    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();

    switch (sgptRiftEntryGlobals->eState)
    {
    case SELECTING_RIFT_TYPE:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("Console:Accept"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));
        {
            UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
            int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();

            if (nSelectedIndex != REGULAR_RIFT_LISTBOX_INDEX)
            {
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_EMPOWER, StringListFetch("ConsoleUI:Empower"));
            }
        }
        break;
    case PICKING_RIFT_LEVEL:
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("Console:Accept"));
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back")); tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_EMPOWER, StringListFetch("ConsoleUI:Empower"));
        break;
    }

    if (sCanScrollText())
    {
        CRefString sScrollPrompt = StringListFetch("ConsoleRifts:ScrollText");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}




static BOOL TryToStartRift()

{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptRiftEntryGlobals->nControllerIndex);
    if (ptPlayer == NULL)
    {
        return FALSE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }


    UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
    int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();

    int32 nLootRunLevel = -1;
    switch (nSelectedIndex)
    {
    case REGULAR_RIFT_LISTBOX_INDEX:
        break;

    case HIGHEST_GREATER_RIFT_LISTBOX_INDEX:

        nLootRunLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL) - 1;
        nLootRunLevel = nLootRunLevel > 0 ? nLootRunLevel : 0;
        break;

    case LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX:
        nLootRunLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LAST_RIFT_STARTED_LEVEL);
        break;

    case SELECT_GREATER_RIFT_LISTBOX_INDEX:
        {
            UIConsoleListBox & tRiftLevelSelect = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_RIGHT_GRIFT_LEVEL_SELECTION);
            int32 nSelectedLootRunLevel = tRiftLevelSelect.GetSelectedIndex();
            int32 nRiftLevel = tRiftLevelSelect.GetNumItems() - nSelectedLootRunLevel - 1;
            nLootRunLevel = nRiftLevel > 0 ? nRiftLevel : 0;
        }
        break;

    default:
        return FALSE;
    }

    BOOL bIsGreaterRift = (nLootRunLevel != -1);


    if (bIsGreaterRift)
    {
        if (ACDIsParticipatingInSetDungeon(*ptACDPlayer))
        {

            NotificationCreationParams tParams;

            tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleRifts:ErrorParticipatingInSetDungeon").str(), UnicodeText::FormatNumber(70, TRUE, FNS_NORMAL, 0).str(), NULL);

            tParams.nControllerIndex = sgptRiftEntryGlobals->nControllerIndex;
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return FALSE;
        }


        BOOL bHasKeyForGrift = NephalemRift::HasKeyForGreaterRift(ptPlayer);

        int32 nCurrentLevel = ptPlayer->nCurrentLevel;
        if (bHasKeyForGrift && nCurrentLevel != 70)
        {
            NotificationCreationParams tParams;

            tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleRifts:ErrorNotMaxLevelTieredRift").str(), UnicodeText::FormatNumber(70, TRUE, FNS_NORMAL, 0).str(), NULL);

            tParams.nControllerIndex = sgptRiftEntryGlobals->nControllerIndex;
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return FALSE;
        }

        if (!bHasKeyForGrift)
        {
            NotificationCreationParams tParams;

            tParams.uszMessage = LocalizedSprintf(StringListFetch("Errors:NephalemRiftWarning_PlayerNeedsTieredKey").str());
            tParams.nControllerIndex = sgptRiftEntryGlobals->nControllerIndex;
            ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            return FALSE;
        }

        if (sgptRiftEntryGlobals->bEmpower)
        {
            uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(nLootRunLevel);
            if ((uint64)ptACDPlayer->GetCurrencyAmount(0) < nEmpowerCost)
            {
                NotificationCreationParams tParams;

                tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleRifts:ErrorNotEnoughGoldForEmpower").str());
                tParams.nControllerIndex = sgptRiftEntryGlobals->nControllerIndex;
                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
                return FALSE;
            }
        }
    }


    sgptRiftEntryGlobals->nSelectedRiftLevel = nLootRunLevel;


    UI_SetControllerFocus(GetUIIDNone());


    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TIERED_RIFT_SOCKET_KEY), cgtPSODefaults);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    sConfirmSelection();

    return TRUE;
}




static void sGoBack()
{

    UI_SetControllerFocus(UILIST_RIFT_ENTRY_TYPE_SELECTION);

    sgptRiftEntryGlobals->eState = SELECTING_RIFT_TYPE;

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);
}




static void sConfirmSelection()
{

    Player * ptPlayer = PlayerGetByControllerIndex(sgptRiftEntryGlobals->nControllerIndex);
    if (ptPlayer)
    {



        RiftStartAcceptedMessage tMessage;
        tMessage.unk_08 = sgptRiftEntryGlobals->nSelectedRiftLevel;
        tMessage.unk_0c = sgptRiftEntryGlobals->bEmpower && tMessage.unk_08 != -1;

        MessageSendToServer(CMSG_RIFT_START_ACCEPTED, &tMessage, sizeof(tMessage), ptPlayer);

        ::Console::Screens::CloseScreenSet(TRUE, TRUE);

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_CONFIRM_KEYSTONE_SELECTION), cgtPSODefaults);
    }
}



static BOOL sCanScrollText()
{

    UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
    int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();

    if (nSelectedIndex == REGULAR_RIFT_LISTBOX_INDEX)
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_RIFT_ENTRY_REGULAR_RIFT_DESCRIPTION);
    }
    else if (nSelectedIndex == HIGHEST_GREATER_RIFT_LISTBOX_INDEX || nSelectedIndex == LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX)
    {

        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_RIFT_ENTRY_GREATER_RIFT_DESCRIPTION);
    }

    return FALSE;
}







static void sScrollText(float flDelta)
{

    if (sCanScrollText())
    {
        UIConsoleListBox & tRiftTypeList = UI_GetConsoleListBoxFromID(UILIST_RIFT_ENTRY_TYPE_SELECTION);
        int32 nSelectedIndex = tRiftTypeList.GetSelectedIndex();
        if (nSelectedIndex == REGULAR_RIFT_LISTBOX_INDEX)
        {
            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_RIFT_ENTRY_REGULAR_RIFT_DESCRIPTION, flDelta);
        }
        else if (nSelectedIndex == HIGHEST_GREATER_RIFT_LISTBOX_INDEX || nSelectedIndex == LAST_PLAYED_GREATER_RIFT_LISTBOX_INDEX)
        {

            UIUtils::Console::ScrollContentInScrollBox(UISCROLL_RIFT_ENTRY_GREATER_RIFT_DESCRIPTION, flDelta);
        }
    }
}
