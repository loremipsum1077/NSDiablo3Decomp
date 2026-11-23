// Original file: X/Client/UserInterface/Console/UIConsoleJewelUpgrade.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Console/ControllerUtils.h"
#include "../../RActors/RActors.h"
#include "../../Sound/Sound.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleUtils.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Crafting/ItemCraftingSwig.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UIDLG_JEWEL_UPGRADE, UILIST_JEWEL_UPGRADE_LIST_ITEMS, UIWND_JEWEL_UPGRADE_FOCUS_HIGHLIGHT, UIBTN_JEWEL_UPGRADE_SELECTED_GEM, UITEXT_JEWEL_UPGRADE_RANK, UITEXT_JEWEL_UPGRADE_CHANCE, UITEXT_JEWEL_UPGRADE_CHANCES_LEFT, UITEXT_JEWEL_UPGRADE_CHANCES_LEFT_OLD, UITEXT_JEWEL_UPGRADE_NO_DEATH;  // X/Client/UserInterface/UIGlobals.cpp
extern UIID UITEXT_JEWEL_UPGRADE_STATUS, UISCROLL_JEWEL_UPGRADE_GEM_DESCRIPTION, UITEXT_JEWEL_UPGRADE_GEM_DESCRIPTION, UIRACTOR_JEWEL_UPGRADE_SUCCESS_FX, UIRACTOR_JEWEL_UPGRADE_FAIL_FX, UIWND_JEWEL_UPGRADE_FAILURE, UISTORYBOARD_JEWEL_UPGRADE_ONSELECTION, UISTORYBOARD_JEWEL_UPGRADE_ONUPGRADE, UISTORYBOARD_JEWEL_UPGRADE_ON_RANK_REVEAL;  // X/Client/UserInterface/UIGlobals.cpp
UIWindow & UI_GetWindowFromID(const UIID & id); UIText & UI_GetTextFromID(const UIID & id); UIStoryboard & UI_GetStoryboardFromID(const UIID & id); const UIID & UI_GetControllerFocus();  // X/Client/UserInterface/UI.cpp
void ItemsFindUpgradeableJewels(const Player * ptPlayer, BOOL bExcludeUnidentified, ACDIDArray & aridItems);  // X/Common/Items.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...); /* X/Common/Definitions/StringListDefinition.cpp */ 
namespace UIInventory { namespace Console { CRefString GetItemDescription(const ACDID idACDPlayer, const ACDID idACDItem, const BOOL bShowStatRanges); void UpdateViewItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const int32 eFilterType, const BOOL bIsSelected, const BOOL bIsDetailedView, const BOOL bIsNew, const BOOL bIdentifyAllowed, const Blizzard::Time::Timestamp nExpiration);
void UpdateViewItemSelectionUI(const UIID & idItemUI, const BOOL bIsSelected); void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags); float GetIdentifyingProgress(uint32 dwEndTime, uint32 dwDuration); BOOL SendIdentifyRequest(const Player * ptPlayer, ACDID idACDItem); } }  // UIConsoleInventoryViewUtilities.cpp, UIConsoleInventoryView.cpp
namespace UIUtils { namespace Console { BOOL CanScrollContentInScrollBox(const UIID & idScrollBox); } }  // UIConsoleUtils.cpp
  // size 0x10; X/Common/Crafting/ItemCrafting.h
namespace UIJewelUpgrade { int32 GetNumBonusUpgradesLeftForPlayer(const Player * ptPlayer); int32 GetNumUpgradesLeftForPlayer(const Player * ptPlayer); BOOL IsJewelMaxRank(const ActorCommonData & tACDJewel); CRefString GetJewelRankString(ACDID idJewel); CRefString GetUpgradeChanceString(const Player * ptPlayer, ACDID idJewel); }  // UIJewelUpgradeCommon.cpp
namespace UIJewelUpgrade { void InitializeForGame(); void DisposeForGame(); void OpenForController(int32 nControllerIndex); void Close(); void OnUpdate(const UIID & id); void OnButton1(const UIID & id, const int32 nControllerIndex); void OnButton2(const UIID & id, const int32 nControllerIndex); void OnButton3(const UIID & id, const int32 nControllerIndex);
void OnButtonL2(const UIID & id, const int32 nControllerIndex); void OnButtonR2(const UIID & id, const int32 nControllerIndex); void UpgradeResults(const JewelUpgradeResultsMessage * ptMsg); void JewelUpgradeMaxChanged(); }
static void sResetListBox(UIID & idListBox); static void sRebuildItemLists(); static ACDID sGetSelectedItem(); static void sUpdateScrollingControllerActions(); static void sUpdateControllerActions(); static void sUpdateGemUpgradePane(); static void sUpdateIdentifyProgress(); static void sGoBack(); static void sScrollDetails(float flDelta);
static void sSelectGem(); static void sUpgradeGem(); static void sIdentify(); static void sDoneIdentifying(BOOL bSuccess); static void sCancelIdentify(); static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal);

enum ButtonActions {
    ACTION_SELECT = 1,
    ACTION_UPGRADE = 2,
    ACTION_IDENTIFY = 3,
    ACTION_IDENTIFY_CANCEL = 4,
    ACTION_SCROLL_LIST = 5,
    ACTION_CLOSE = 6,
    ACTION_BACK = 7,
    ACTION_ACCEPT = 8,
};






































class LegendaryGemDataSource : public ConsoleListBoxDataSource {  // size 0x20
public:
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
    ACDID GetItem(const int32 nIndex) const;
    void Refresh();
private:
    ACDIDArray m_aridKeys;  // 0x08
};
static_assert(sizeof(LegendaryGemDataSource) == 0x20, "LegendaryGemDataSource");

enum class JewelUpgradeState : int {
    SELECTING_GEM = 0,
    CONFIRMING_GEM = 1,
    UPGRADING = 2,
    UPGRADE_SUCCEEDED = 3,
    UPGRADE_FAILED = 4,
    IDENTIFYING = 5,
    IDENTIFY_REQUEST_SENT = 6,
    IDENTIFY_DONE = 7,
    WAITING_TO_CONTINUE = 8,
};

struct JewelUpgradeGlobals {  // size 0x30
    JewelUpgradeGlobals();
    ~JewelUpgradeGlobals();
    int32 nControllerIndex;  // 0x00
    LegendaryGemDataSource * ptDataSource;  // 0x08
    JewelUpgradeState eState;  // 0x10
    ACDID idLastSelectedItem;  // 0x14
    int32 nLastRank;  // 0x18
    BOOL bLastIdentified;  // 0x1c
    struct IdentifyInfo {  // size 0xc
        ACDID idACDItemBeingIdentified;  // 0x00
        uint32 dwIdentifyingEndTime;  // 0x04
        SoundID idIdentifyingSoundLoop;  // 0x08
        IdentifyInfo();
    };
    JewelUpgradeGlobals::IdentifyInfo tIdentifyInfo;  // 0x20
};
static_assert(sizeof(JewelUpgradeGlobals) == 0x30, "JewelUpgradeGlobals");






























JewelUpgradeGlobals * sgptJewelUpgradeGlobals = NULL;





static const uint32 IDENTIFYING_DURATION = 150;




JewelUpgradeGlobals::JewelUpgradeGlobals()
    : nControllerIndex(-1), eState(JewelUpgradeState::SELECTING_GEM), idLastSelectedItem(ACDID_INVALID), nLastRank(-1), bLastIdentified(FALSE)



{
    ptDataSource = new LegendaryGemDataSource();
}




JewelUpgradeGlobals::~JewelUpgradeGlobals()

{
    ::delete ptDataSource;
}




JewelUpgradeGlobals::IdentifyInfo::IdentifyInfo()
    : idACDItemBeingIdentified(ACDID_INVALID), dwIdentifyingEndTime(0), idIdentifyingSoundLoop(-1)
{
}




int32 LegendaryGemDataSource::GetNumItems() const

{
    return (int32)m_aridKeys.size();
}




UIControlSourceKey LegendaryGemDataSource::GetSourceKeyForItem(const int32 nIndex) const


{
    const LPCSTR ITEM_TEMPLATE = "ConsoleInventoryListEquipmentTemplate:ConsoleInventoryListEquipmentTemplate_main";

    const UIControlSourceKey ITEM_SOURCE_KEY = UIControlSourceKey::Parse(ITEM_TEMPLATE, SNO_NONE, NULL, NULL);
    return ITEM_SOURCE_KEY;
}





ACDID LegendaryGemDataSource::GetItem(const int32 nIndex) const

{
    return m_aridKeys[nIndex];
}




static int32 sSortGems(const void * pItem1, const void * pItem2)


{
    const ACDID * pidItem1 = (const ACDID *)pItem1;
    const ACDID * pidItem2 = (const ACDID *)pItem2;

    ActorCommonData * ptACDItem1 = ACDTryToGet(*pidItem1);
    ActorCommonData * ptACDItem2 = ACDTryToGet(*pidItem2);

    if (!ptACDItem1 || !ptACDItem2)
    {
        return 0;
    }

    int32 nRank1 = ptACDItem1->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
    int32 nRank2 = ptACDItem2->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);

    return nRank1 - nRank2;
}




void LegendaryGemDataSource::Refresh()

{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    m_aridKeys.clear();
    ItemsFindUpgradeableJewels(ptPlayer, FALSE, m_aridKeys);
    blzext::vector_sort(m_aridKeys, sSortGems);
}




void LegendaryGemDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)







{
}



void LegendaryGemDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)



{
    ActorCommonData * ptACDItem = ACDTryToGet(GetItem(nIndex));
    if (!ptACDItem)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    BOOL bShowSelection = tListBox.GetSelectedIndex() == nIndex && UI_GetControllerFocus() == tListBox.GetID();
    UIInventory::Console::UpdateViewItemUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, FILTER_UPGRADEABLE_JEWEL, bShowSelection, FALSE, FALSE, TRUE, 0);
    UIInventory::Console::UpdateViewItemSelectionUI(idItemUI, bShowSelection);
}




static BOOL sIsActive()

{
    return UI_GetWindowFromID(UIDLG_JEWEL_UPGRADE).IsActive();
}




void UIJewelUpgrade::InitializeForGame()

{

    sgptJewelUpgradeGlobals = new JewelUpgradeGlobals();
}




void UIJewelUpgrade::DisposeForGame()

{

    delete sgptJewelUpgradeGlobals; sgptJewelUpgradeGlobals = NULL;
}















void UIJewelUpgrade::OpenForController(int32 nControllerIndex)

{
    if (sIsActive())
    {
        return;
    }

    CancelAllChannelingAbilities();


    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_RANK).SetText("", FALSE);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_GEM_DESCRIPTION).SetText("", FALSE);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCE).SetText("", FALSE);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCES_LEFT).SetText("", FALSE);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCES_LEFT_OLD).SetText("", FALSE);

    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::SELECTING_GEM;

    sgptJewelUpgradeGlobals->nControllerIndex = nControllerIndex;


    BOOL bCanUpgradeJewels = FALSE;
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        ptPlayer->eActiveCrafterType = CRAFTER_TYPE_JEWEL_UPGRADE;

        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        bCanUpgradeJewels = tACDPlayer.AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE) == 0;
    }

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_JEWEL_UPGRADE_LIST_ITEMS);
    tListBox.SetDataSource(*sgptJewelUpgradeGlobals->ptDataSource);
    tListBox.SetOnSelChangeFunction(sOnListSelectionChanged);

    sRebuildItemLists();

    ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_JEWEL_UPGRADE, FALSE, GetUIIDNone());

    if (bCanUpgradeJewels)
    {
        UI_SetControllerFocus(UILIST_JEWEL_UPGRADE_LIST_ITEMS);
        UI_GetWindowFromID(UIWND_JEWEL_UPGRADE_FAILURE).SetActive(FALSE);
    }
    else
    {
        UI_GetWindowFromID(UIWND_JEWEL_UPGRADE_FAILURE).SetActive(TRUE);
    }







    UIStoryboard & tUpgradeStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JEWEL_UPGRADE_ONUPGRADE);
    tUpgradeStoryboard.Reset();


    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

















static void sResetUI()

{

    UIUtils::Console::ResetScrollBox(UISCROLL_JEWEL_UPGRADE_GEM_DESCRIPTION);


    UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_SUCCESS_FX).SetActive(FALSE);
    UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_FAIL_FX).SetActive(FALSE);


    sgptJewelUpgradeGlobals->nControllerIndex = -1;
    sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified = ACDID_INVALID;
    UI_SetControllerFocus(GetUIIDNone());


    sgptJewelUpgradeGlobals->idLastSelectedItem = ACDID_INVALID;
    sgptJewelUpgradeGlobals->nLastRank = -1;
    sgptJewelUpgradeGlobals->bLastIdentified = FALSE;
}




void UIJewelUpgrade::Close()

{

    if (sgptJewelUpgradeGlobals->eState == JewelUpgradeState::IDENTIFYING)
    {
        sDoneIdentifying(FALSE);
    }

    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    if (ptPlayer)
    {
        ptPlayer->idACDActiveVendor = ACDID_INVALID;
        ptPlayer->eActiveCrafterType = CRAFTER_TYPE_INVALID;
    }

    sResetUI();

    UI_GetConsoleListBoxFromID(UILIST_JEWEL_UPGRADE_LIST_ITEMS).SetOnSelChangeFunction(NULL);

    ::Console::Screens::CloseModalDialog(UIDLG_JEWEL_UPGRADE);
}









void UIJewelUpgrade::OnUpdate(const UIID & id)

{
    if (!sIsActive())
    {
        return;
    }

    sUpdateGemUpgradePane();
    sUpdateIdentifyProgress();


    if (::Console::Screens::GetActionManager().AreActionsDirty())
    {
        sUpdateControllerActions();
        ::Console::Screens::UpdateButtonLegendText();
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(FALSE);
    }
}




void UIJewelUpgrade::OnButton1(const UIID & id, const int32 nControllerIndex)


{
    if (sgptJewelUpgradeGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_1);
    switch (idAction)
    {
        case ACTION_SELECT: sSelectGem();
            break;


        case ACTION_UPGRADE: sUpgradeGem();
            break;


        case ACTION_ACCEPT: sGoBack();
            break;
    }
}



void UIJewelUpgrade::OnButton2(const UIID & id, const int32 nControllerIndex)


{
    if (sgptJewelUpgradeGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2);
    switch (idAction)
    {
        case ACTION_CLOSE: ::Console::Screens::CloseScreenSet(TRUE, TRUE);
            break;

        case ACTION_BACK: sGoBack();
            break;

        case ACTION_IDENTIFY_CANCEL: sCancelIdentify();
            break;
    }
}





void UIJewelUpgrade::OnButton3(const UIID & id, const int32 nControllerIndex)


{
    if (sgptJewelUpgradeGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    ::Console::ControllerActionId idAction = ::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_3);
    if (idAction == ACTION_IDENTIFY)
    {
        sIdentify();
    }
}




void UIJewelUpgrade::OnButtonL2(const UIID & id, const int32 nControllerIndex)


{
    if (sgptJewelUpgradeGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_LEFT_TRIGGER) == ACTION_SCROLL_LIST)
    {



        if (!::Console::ControllerUtils::IsButtonDown(sgptJewelUpgradeGlobals->nControllerIndex, 7))
        {
            sScrollDetails(-4.0f);
        }
    }
}






void UIJewelUpgrade::OnButtonR2(const UIID & id, const int32 nControllerIndex)


{
    if (sgptJewelUpgradeGlobals->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER) == ACTION_SCROLL_LIST)
    {



        if (!::Console::ControllerUtils::IsButtonDown(sgptJewelUpgradeGlobals->nControllerIndex, 6))
        {
            sScrollDetails(4.0f);
        }
    }
}






void UIJewelUpgrade::UpgradeResults(const JewelUpgradeResultsMessage * ptMsg)



{
    if (sgptJewelUpgradeGlobals->eState != JewelUpgradeState::UPGRADING)
    {
        return;
    }

    BOOL bUpgraded = ptMsg->nNumRanksGained != 0;
    sgptJewelUpgradeGlobals->eState = bUpgraded ? JewelUpgradeState::UPGRADE_SUCCEEDED : JewelUpgradeState::UPGRADE_FAILED;

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




void UIJewelUpgrade::JewelUpgradeMaxChanged()

{
    if (sgptJewelUpgradeGlobals && sIsActive())
    {

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}




static void sResetListBox(UIID & idListBox)

{
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(idListBox);
    tListBox.ResetAllItems();
}




static void sRebuildItemLists()

{
    sgptJewelUpgradeGlobals->ptDataSource->Refresh();

    sResetListBox(UILIST_JEWEL_UPGRADE_LIST_ITEMS);
}




static ACDID sGetSelectedItem()

{
    int32 nSelectedIndex = UI_GetConsoleListBoxFromID(UILIST_JEWEL_UPGRADE_LIST_ITEMS).GetSelectedIndex();
    if (nSelectedIndex == -1)
    {
        return ACDID_INVALID;
    }


    return sgptJewelUpgradeGlobals->ptDataSource->GetItem(nSelectedIndex);
}





static void sUpdateScrollingControllerActions()

{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();

    if (UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_JEWEL_UPGRADE_GEM_DESCRIPTION))
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}




static void sUpdateControllerActions()

{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();

    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    BOOL bCanUpgradeJewels = tACDPlayer.AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE) == 0;

    if (bCanUpgradeJewels)
    {
        switch (sgptJewelUpgradeGlobals->eState)
        {
            case JewelUpgradeState::SELECTING_GEM: {
                ActorCommonData * ptACDItem = ACDTryToGet(sGetSelectedItem());
                if (ptACDItem)
                {
                    if (ItemIsUnidentified(*ptACDItem))
                    {
                        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_IDENTIFY, StringListFetch("ConsoleInventory:Identify"));
                    }
                    else
                    {
                        if (UIJewelUpgrade::GetNumUpgradesLeftForPlayer(PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex)) > 0)
                        {
                            tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_SELECT, StringListFetch("H2OLayout:Select"));
                        }
                    }

                    sUpdateScrollingControllerActions();
                }

                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));
                break;
            }
            case JewelUpgradeState::CONFIRMING_GEM:
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_UPGRADE, StringListFetch("Vendor:UpgradeJewel"));
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back"));

                sUpdateScrollingControllerActions();
                break;

            case JewelUpgradeState::IDENTIFYING:
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_IDENTIFY_CANCEL, StringListFetch("H2OLayout:Cancel"));
                break;

            case JewelUpgradeState::WAITING_TO_CONTINUE:
                tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, ACTION_ACCEPT, StringListFetch("H2OLayout:Continue"));
                break;
        }
    }
    else
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_CLOSE, StringListFetch("ConsoleUI:Close"));
    }
}


static void sUpdateGemInfo(Player * ptPlayer, ACDID idSelectedItem)



{
    UIInventory::Console::SetUpItemButton(UIBTN_JEWEL_UPGRADE_SELECTED_GEM, idSelectedItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);


    CRefString sRank = UIJewelUpgrade::GetJewelRankString(idSelectedItem);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_RANK).SetText(sRank, FALSE);


    CRefString sDesc = UIInventory::Console::GetItemDescription(ptPlayer->idPlayerACD, idSelectedItem, FALSE);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_GEM_DESCRIPTION).SetText(sDesc, FALSE);
}




static void sUpdateUnidentifiedInfo(const Player * ptPlayer, ACDID idSelectedItem)



{
    UIInventory::Console::SetUpItemButton(UIBTN_JEWEL_UPGRADE_SELECTED_GEM, idSelectedItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);


    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_RANK).SetText("", FALSE);


    CRefString sDesc = StringListFetch("ConsoleInventory:UnidentifiedInstructions");
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_GEM_DESCRIPTION).SetText(sDesc, FALSE);
}




static void sUpdateGemUpgradePane()

{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }


    BOOL bShowHighlight = sgptJewelUpgradeGlobals->eState == JewelUpgradeState::CONFIRMING_GEM
        || sgptJewelUpgradeGlobals->eState == JewelUpgradeState::UPGRADING
        || sgptJewelUpgradeGlobals->eState == JewelUpgradeState::UPGRADE_SUCCEEDED
        || sgptJewelUpgradeGlobals->eState == JewelUpgradeState::UPGRADE_FAILED;

    UI_GetWindowFromID(UIWND_JEWEL_UPGRADE_FOCUS_HIGHLIGHT).SetActive(bShowHighlight);

    ACDID idSelectedItem = sGetSelectedItem();
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (!ptACDItem)
    {
        return;
    }

    BOOL bIdentified = !ItemIsUnidentified(*ptACDItem);
    int32 nRank = ptACDItem->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);

    if (idSelectedItem != sgptJewelUpgradeGlobals->idLastSelectedItem
        || nRank != sgptJewelUpgradeGlobals->nLastRank
        || bIdentified != sgptJewelUpgradeGlobals->bLastIdentified)
    {

        sgptJewelUpgradeGlobals->idLastSelectedItem = idSelectedItem;
        sgptJewelUpgradeGlobals->nLastRank = nRank;
        sgptJewelUpgradeGlobals->bLastIdentified = bIdentified;


        if (bIdentified)
        {
            sUpdateGemInfo(ptPlayer, idSelectedItem);
        }
        else
        {
            sUpdateUnidentifiedInfo(ptPlayer, idSelectedItem);
        }


        UIUtils::Console::ResetScrollBox(UISCROLL_JEWEL_UPGRADE_GEM_DESCRIPTION);
    }


    CRefString sChance = ItemIsUnidentified(*ptACDItem) ? CRefString("") : UIJewelUpgrade::GetUpgradeChanceString(ptPlayer, idSelectedItem);
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCE).SetText(sChance, FALSE);

    int32 nUpgradesLeft = UIJewelUpgrade::GetNumUpgradesLeftForPlayer(ptPlayer);
    int32 nBonusUpgradesLeft = UIJewelUpgrade::GetNumBonusUpgradesLeftForPlayer(ptPlayer);

    UIText & tNoDeathText = UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_NO_DEATH);
    tNoDeathText.SetActive(FALSE);

    CRefString sUpgradeNumber("%s");
    if (nBonusUpgradesLeft > 0)
    {
        if (NephalemRift::PlayerHasNoRiftDeaths(ptPlayer))
        {
            tNoDeathText.SetActive(TRUE);
        }

        nUpgradesLeft -= nBonusUpgradesLeft;

        CRefString sBonusUpgrades("+%s");

        Set_Text_Color(sBonusUpgrades, (FontSpecifier)3);

        sUpgradeNumber = sUpgradeNumber + sBonusUpgrades;

        sUpgradeNumber = sUpgradeNumber.Format(sUpgradeNumber,
            UnicodeText::FormatNumber(nUpgradesLeft, TRUE, FNS_NORMAL, (XLocale)0).str(),
            UnicodeText::FormatNumber(nBonusUpgradesLeft, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);
    }
    else
    {

        sUpgradeNumber = UnicodeText::FormatNumber(nUpgradesLeft, TRUE, FNS_NORMAL, (XLocale)0).str();
    }

    CRefString sUpgradesLeft = LocalizedSprintf(StringListFetch("ConsoleArtisan:GemUpgradesLeft").str(),
        sUpgradeNumber.str(), NULL);

    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCES_LEFT).SetText(sUpgradesLeft, FALSE);
}




static void sUpdateIdentifyProgress()



{
    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    float flProgress = 0.0f;

    if (sgptJewelUpgradeGlobals->eState == JewelUpgradeState::IDENTIFYING)
    {
        if (sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified == ACDID_INVALID)
        {


            sDoneIdentifying(FALSE);
            return;
        }

        flProgress = UIInventory::Console::GetIdentifyingProgress(sgptJewelUpgradeGlobals->tIdentifyInfo.dwIdentifyingEndTime, IDENTIFYING_DURATION);
        if (flProgress >= 1.0f)
        {

            BOOL bMessageSent = UIInventory::Console::SendIdentifyRequest(ptPlayer, sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified);
            if (bMessageSent)
            {
                sgptJewelUpgradeGlobals->eState = JewelUpgradeState::IDENTIFY_REQUEST_SENT;
            }
        }
    }
    else if (sgptJewelUpgradeGlobals->eState == JewelUpgradeState::IDENTIFY_REQUEST_SENT)
    {

        ActorCommonData * ptACDItem = ACDTryToGet(sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified);
        if (!ptACDItem)
        {


            sDoneIdentifying(FALSE);
            return;
        }

        if (!ItemIsUnidentified(*ptACDItem))
        {

            sDoneIdentifying(TRUE);
        }
    }


    UIID idActiveProgress = GetUIIDNone();
    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_JEWEL_UPGRADE_LIST_ITEMS);
    int32 nNumItems = sgptJewelUpgradeGlobals->ptDataSource->GetNumItems();
    for (int32 i = 0; i < nNumItems; i++)
    {
        const UIID & idItem = tListBox.GetItemUIID(i);
        if (idItem == GetUIIDNone())
        {
            continue;
        }

        UIID idProgress = UIIDCreateFromParent(idItem, ".LayoutRoot.IdentifyProgress");
        if (sgptJewelUpgradeGlobals->ptDataSource->GetItem(i) == sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified)
        {
            idActiveProgress = idProgress;
        }
        else
        {

            UI_GetProgressBarFromID(idProgress).SetExtentsAndPosition(0.0f, 1.0f, 0.0f, -1.0f);
        }
    }

    if (idActiveProgress == GetUIIDNone())
    {
        return;
    }



    UI_GetProgressBarFromID(idActiveProgress).SetExtentsAndPosition(0.0f, 1.0f, flProgress, -1.0f);
}



static void sGoBack()

{
    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::SELECTING_GEM;

    UI_SetControllerFocus(UILIST_JEWEL_UPGRADE_LIST_ITEMS);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_FOCUS_BACK), cgtPSODefaults);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sScrollDetails(float flDelta)

{
    UIUtils::Console::ScrollContentInScrollBox(UISCROLL_JEWEL_UPGRADE_GEM_DESCRIPTION, flDelta);
}




static void sSelectGem()

{
    if (sgptJewelUpgradeGlobals->eState != JewelUpgradeState::SELECTING_GEM)
    {
        return;
    }


    ActorCommonData * ptACDJewel = ACDTryToGet(sGetSelectedItem());
    if (!ptACDJewel)
    {
        return;
    }


    if (UIJewelUpgrade::IsJewelMaxRank(*ptACDJewel))
    {
        NotificationCreationParams tParams;
        tParams.uszMessage = StringListFetch("ConsoleArtisan:ErrorJewelUpgradeMaxRank").str();
        tParams.nControllerIndex = sgptJewelUpgradeGlobals->nControllerIndex;
        ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        return;
    }

    UI_SetControllerFocus(GetUIIDNone());

    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::CONFIRMING_GEM;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_JEWEL_UPGRADE_SELECT_GEM), cgtPSODefaults);


    UIStoryboard & tSelectionStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JEWEL_UPGRADE_ONSELECTION);
    tSelectionStoryboard.Reset();
    tSelectionStoryboard.Play(FALSE, 0.0f);
}




static void sOnRankRevealStoryboardEnded(const UIID &)

{
    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::WAITING_TO_CONTINUE;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sOnUpgradeStoryboardEnded(const UIID &)

{
    if (sgptJewelUpgradeGlobals->eState == JewelUpgradeState::UPGRADE_SUCCEEDED)
    {

        UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_SUCCESS_FX).SetActive(TRUE);
        UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_FAIL_FX).SetActive(FALSE);


        UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_STATUS).SetText(StringListFetch("Vendor:JewelUpgradeSuccess"), FALSE);


        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TIERED_RIFT_GEM_UPGRADE_SUCCESS), cgtPSODefaults);
    }
    else if (sgptJewelUpgradeGlobals->eState == JewelUpgradeState::UPGRADE_FAILED)
    {

        UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_SUCCESS_FX).SetActive(FALSE);
        UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_FAIL_FX).SetActive(TRUE);


        UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_STATUS).SetText(StringListFetch("Vendor:JewelUpgradeFailure"), FALSE);


        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TIERED_RIFT_GEM_UPGRADE_FAILED), cgtPSODefaults);
    }

    UIStoryboard & tRankRevealStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JEWEL_UPGRADE_ON_RANK_REVEAL);
    tRankRevealStoryboard.Stop();
    tRankRevealStoryboard.Reset();
    tRankRevealStoryboard.Play(FALSE, 0.0f);
    tRankRevealStoryboard.SetOnEndCallback(sOnRankRevealStoryboardEnded);
}




static void sUpgradeGem()

{
    if (sgptJewelUpgradeGlobals->eState != JewelUpgradeState::CONFIRMING_GEM)
    {
        return;
    }


    Player * ptPlayer = PlayerGetByControllerIndex(sgptJewelUpgradeGlobals->nControllerIndex);
    if (!ptPlayer)
    {
        return;
    }

    ACDID idSelectedItem = sGetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
    {
        return;
    }


    CRefString sChancesText = UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCES_LEFT).GetText();
    UI_GetTextFromID(UITEXT_JEWEL_UPGRADE_CHANCES_LEFT_OLD).SetText(sChancesText, FALSE);


    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idSelectedItem);
    MessageSendToServer(CMSG_TRY_UPGRADE_JEWEL, &tMessage, sizeof(tMessage), ptPlayer);

    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::UPGRADING;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TIERED_RIFT_GEM_UPGRADE), cgtPSODefaults);


    UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_SUCCESS_FX).SetActive(FALSE);
    UI_GetWindowFromID(UIRACTOR_JEWEL_UPGRADE_FAIL_FX).SetActive(FALSE);


    UIStoryboard & tUpgradeStoryboard = UI_GetStoryboardFromID(UISTORYBOARD_JEWEL_UPGRADE_ONUPGRADE);
    tUpgradeStoryboard.Reset();
    tUpgradeStoryboard.Play(FALSE, 0.0f);
    tUpgradeStoryboard.SetOnEndCallback(sOnUpgradeStoryboardEnded);
}




static void sIdentify()

{
    if (sgptJewelUpgradeGlobals->eState != JewelUpgradeState::SELECTING_GEM)
    {
        return;
    }


    ACDID idSelectedItem = sGetSelectedItem();
    if (idSelectedItem == ACDID_INVALID)
    {
        return;
    }

    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::IDENTIFYING;

    sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified = idSelectedItem;
    sgptJewelUpgradeGlobals->tIdentifyInfo.dwIdentifyingEndTime = sgtAppGlobals.dwFrameCounter + IDENTIFYING_DURATION;

    PlaySoundOptions tOptions;
    tOptions.fForce2D = TRUE;
    sgptJewelUpgradeGlobals->tIdentifyInfo.idIdentifyingSoundLoop = SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_IDENTIFY_LOOP), tOptions);

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sDoneIdentifying(BOOL bSuccess)

{
    SoundDelete(sgptJewelUpgradeGlobals->tIdentifyInfo.idIdentifyingSoundLoop);
    sgptJewelUpgradeGlobals->tIdentifyInfo.idIdentifyingSoundLoop = -1;

    if (bSuccess)
    {
        PlaySoundOptions tOptions;
        tOptions.fForce2D = TRUE;

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_IDENTIFY_FINISH1), tOptions);
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_IDENTIFY_FINISH2), tOptions);


        int32 nIndex = -1;
        for (int32 i = 0; i < sgptJewelUpgradeGlobals->ptDataSource->GetNumItems(); i++)
        {
            if (sgptJewelUpgradeGlobals->ptDataSource->GetItem(i) == sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified)
            {
                nIndex = i;
                break;
            }
        }

        if (nIndex != -1)
        {
            UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(UILIST_JEWEL_UPGRADE_LIST_ITEMS);
            const UIID & idItemUI = tListBox.GetItemUIID(nIndex);
            if (idItemUI != GetUIIDNone())
            {
                sgptJewelUpgradeGlobals->ptDataSource->UpdateItemUI(tListBox, idItemUI, nIndex);
            }
        }
    }

    sgptJewelUpgradeGlobals->tIdentifyInfo.idACDItemBeingIdentified = ACDID_INVALID;
    sgptJewelUpgradeGlobals->eState = JewelUpgradeState::SELECTING_GEM;

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}




static void sCancelIdentify()

{
    if (sgptJewelUpgradeGlobals->eState != JewelUpgradeState::IDENTIFYING)
    {
        return;
    }

    sDoneIdentifying(FALSE);
}





static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)



{
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}
