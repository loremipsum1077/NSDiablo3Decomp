// Original file: X/Client/UserInterface/Console/UIConsoleEnchantView.cpp
// The console mystic enchant view: choose an affix on the selected item, roll replacements, pick one.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleEnchantView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "UIConsoleInventoryViewUtilities.h"
#include "UIConsoleUtils.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIEnchantCommon.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../CGame.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
const UIID & UI_GetControllerFocus();
#include "ControllerActions.h"
namespace Console {




}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode);
void UpdateViewItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const UIFilterType eFilterType, const BOOL bIsSelected, const BOOL bIsDetailedView, const BOOL bIsNew, const BOOL bIdentifyAllowed, const Blizzard::Time::Timestamp nExpiration);
}  // namespace Console
}  // namespace UIInventory
// X/Common/Crafting/ItemCrafting.h
struct EnchantAffixMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidAffixToReroll;  // 0x0c
};
static_assert(sizeof(EnchantAffixMessage) == 0x10, "EnchantAffixMessage");
struct EnchantAffixChooseMessage {  // size 0x40
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    EnchantAffixChoice tEnchantAffixChoices[6];  // 0x0c
    int32 nEnchantAffixChoicesCount;  // 0x3c
};
static_assert(sizeof(EnchantAffixChooseMessage) == 0x40, "EnchantAffixChooseMessage");
// X/Common/Networking/GameMessage.h




// X/Client/CGame.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Common/Player.cpp
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);

// UIEnchantCommon.h line 24; only the enchant view's constructor inlines it.
inline UIEnchant::AffixInfo::AffixInfo()
    : gbidAffix(-1)
    , bIsInherentAffix(FALSE)
{
}

// line 27
static inline void sUpdateAffixListItemUI(const UIID & idItemUI, const UIEnchant::AffixInfo & tAffixInfo, BOOL bIsSelected, BOOL bCanScroll)
{
    UIText & tAffixText = UI_GetTextFromID(UIIDCreateFromParent(idItemUI, ".ScrollBox.Affix"));
    tAffixText.SetText(tAffixInfo.sAffixTooltipText, FALSE);

    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".Selector")).SetActive(bIsSelected);
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".ButtonLegend")).SetActive(bIsSelected && bCanScroll);
}

static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    UIID idItem = UI_GetConsoleListBoxFromID(id).GetItemUIID(nOldVal);
    if (idItem != GetUIIDNone())
    {
        UI_GetConsoleScrollBoxFromID(UIIDCreateFromParent(idItem, ".ScrollBox")).SetScrollPos(0.0f);
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

namespace UIMystic {
namespace Console {

// lines 68-71
static const int32 MAX_AFFIXES_VISIBLE = 5;
static const int32 MAX_REPLACEMENT_AFFIXES_VISIBLE = 3;
static const uint32 REPLACEMENT_LIST_REVEAL_DURATION = 120;

// line 76
UIEnchantView::UIEnchantView(const UIID & idRoot)
    : UIInventoryView(idRoot)
    , m_tAffixDataSource(MAX_AFFIXES_VISIBLE)
    , m_tReplacementAffixDataSource(MAX_REPLACEMENT_AFFIXES_VISIBLE)
    , m_eState(STATE_CHOOSING_ITEM)
    , m_annPendingItem(ANN_INVALID)
    , m_dwRollingReplacementsEndTime(0)
{
    m_idAffixPanel = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemEnchant.EnchantDetails.AffixContainer");
    m_idAffixListBox = UIIDCreateFromParent(m_idAffixPanel, ".AffixList");

    m_idPossibleAffixPanel = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemEnchant.EnchantDetails.PossibleAffixContainer");

    m_idItemDataPanel = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemEnchant.EnchantDetails.ItemDataContainer");
    m_idPreviouslyEnchantedPanel = UIIDCreateFromParent(m_idItemDataPanel, ".PreviouslyEnchantedContainer");
    m_idRollingReplacementPanel = UIIDCreateFromParent(m_idItemDataPanel, ".RollingContainer");
    m_idRollingReplacementProgress = UIIDCreateFromParent(m_idRollingReplacementPanel, ".Roll.ProgressBar");
    m_idReplacementAffixPanel = UIIDCreateFromParent(m_idItemDataPanel, ".ReplacingContainer");
    m_idReplacementAffixListBox = UIIDCreateFromParent(m_idReplacementAffixPanel, ".AffixListLG");
    m_idFinishedPanel = UIIDCreateFromParent(m_idItemDataPanel, ".FinalPresentationContainer");

    UIConsoleListBox & tAffixListBox = UI_GetConsoleListBoxFromID(m_idAffixListBox);
    tAffixListBox.SetDataSource(m_tAffixDataSource);
    tAffixListBox.SetOnSelChangeFunction(sOnListSelectionChanged);

    UIConsoleListBox & tReplacementAffixListBox = UI_GetConsoleListBoxFromID(m_idReplacementAffixListBox);
    tReplacementAffixListBox.SetDataSource(m_tReplacementAffixDataSource);
    tReplacementAffixListBox.SetOnSelChangeFunction(sOnListSelectionChanged);
}

BOOL UIEnchantView::IsChoosingItem() const
{
    return m_eState == STATE_CHOOSING_ITEM;
}

BOOL UIEnchantView::IsChoosingAffix() const
{
    return m_eState == STATE_CHOOSING_AFFIX;
}

BOOL UIEnchantView::IsWaitingForEnchantResponse() const
{
    return m_eState == STATE_WAITING_FOR_ENCHANT_RESPONSE;
}

BOOL UIEnchantView::IsRollingReplacementAffixes() const
{
    return m_eState == STATE_SHOW_PROGRESS_BAR;
}

BOOL UIEnchantView::IsChoosingReplacementAffix() const
{
    return m_eState == STATE_CHOOSING_REPLACEMENT_AFFIX;
}

BOOL UIEnchantView::IsWaitingToFinish() const
{
    return m_eState == STATE_WAITING_FOR_RESULTS || m_eState == STATE_WAITING_FOR_ITEM_ACDID;
}

// line 167
// Inferred shape: if/else-if with a store in each branch (SimplifyCFG turns the chain into one switch, so both
// compares sit on 170:17; the two stores merge and lose their rows); the scroll-box code is one level deep (176:36)
void UIEnchantView::GoBack()
{
    if (m_eState == STATE_CHOOSING_AFFIX)
    {
        m_eState = STATE_CHOOSING_ITEM;
    }
    else if (m_eState == STATE_SHOWING_POSSIBLE_REPLACEMENTS)
    {
        UIID idScrollBox = GetScrollBoxUIID();
        if (UI_TestID(idScrollBox))
        {
            UIUtils::Console::ResetScrollBox(idScrollBox);
        }
        m_eState = STATE_CHOOSING_ITEM;
    }
}

// line 188
void UIEnchantView::Reset()
{
    m_eState = STATE_CHOOSING_ITEM;
    m_tAffixDataSource.Clear();
    m_tReplacementAffixDataSource.Clear();
    UI_GetConsoleListBoxFromID(m_idAffixListBox).SetSelectedIndex(-1);
    UI_GetConsoleListBoxFromID(m_idReplacementAffixListBox).SetSelectedIndex(-1);
    SelectItem(ACDID_INVALID);
}

// line 202
void UIEnchantView::ChooseAffixesForItem(const ACDID idItemACD)
{
    if (m_eState == STATE_CHOOSING_ITEM)
    {
        m_eState = STATE_CHOOSING_AFFIX;
        UI_GetConsoleListBoxFromID(m_idAffixListBox).SetSelectedIndex(0);
    }
}

// line 215
void UIEnchantView::SelectCurrentAffix()
{
    if (m_eState == STATE_CHOOSING_AFFIX)
    {
        m_tSelectedAffixInfo = GetSelectedAffixInfo();
        m_eState = STATE_SHOW_PROGRESS_BAR;
        m_dwRollingReplacementsEndTime = CGameGetClientTicks() + REPLACEMENT_LIST_REVEAL_DURATION;
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONSOLE_MYSTIC_PROGRESS_BAR), cgtPSODefaults);
    }
}

// line 232
void UIEnchantView::ShowPossibleReplacements(BOOL bShow)
{
    // Operand order of both conditions from the columns (235:17/50, 260:70: the state test comes first).
    if (m_eState == STATE_CHOOSING_AFFIX && bShow)
    {
        if (UI_GetConsoleListBoxFromID(m_idAffixListBox).GetSelectedIndex() == -1)
        {
            return;
        }

        m_eState = STATE_SHOWING_POSSIBLE_REPLACEMENTS;

        UIEnchant::ReplacementAffixInfoArray arReplacementAffixes;
        UIEnchant::EnumPossibleReplacementAffixes(GetSelectedItem(),
                                                  PlayerGetPlayerClass(GetParentScreen()->GetPlayer()),
                                                  -1, 0, GetSelectedAffixInfo().gbidAffix, arReplacementAffixes);

        CRefString sReplacementAffixString;
        for (uint32 i = 0; i < arReplacementAffixes.size(); ++i)
        {
            sReplacementAffixString += arReplacementAffixes[i];
        }

        UI_GetTextFromID(UIIDCreateFromParent(m_idPossibleAffixPanel, ".PossibleAffixScrollBox.PossibleAffixText")).SetText(sReplacementAffixString.str(), FALSE);
    }
    else if (m_eState == STATE_SHOWING_POSSIBLE_REPLACEMENTS && !bShow)
    {
        UIID idScrollBox = GetScrollBoxUIID();
        if (UI_TestID(idScrollBox))
        {
            UIUtils::Console::ResetScrollBox(idScrollBox);
        }

        m_eState = STATE_CHOOSING_AFFIX;
    }
}

// line 274
void UIEnchantView::OnEnchantAffixChoose(const EnchantAffixChooseMessage * ptMessage)
{
    if (m_eState == STATE_WAITING_FOR_ENCHANT_RESPONSE)
    {
        m_eState = STATE_CHOOSING_REPLACEMENT_AFFIX;

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MYSTIC_ENCHANT), cgtPSODefaults);

        m_tReplacementAffixDataSource.PopulateReplacementAffixList(
            GetSelectedItem(),
            PlayerGetPlayerClass(GetParentScreen()->GetPlayer()),
            m_tSelectedAffixInfo.gbidAffix,
            ptMessage->tEnchantAffixChoices,
            ptMessage->nEnchantAffixChoicesCount);
        UI_GetConsoleListBoxFromID(m_idReplacementAffixListBox).ResetAllItems();
        UI_SetControllerFocus(m_idReplacementAffixListBox);
    }
}

// line 297
void UIEnchantView::OnEnchantAffixResults(const ANNDataMessage * ptMessage)
{
    if (m_eState == STATE_WAITING_FOR_RESULTS)
    {
        m_eState = STATE_WAITING_FOR_ITEM_ACDID;
        m_annPendingItem = ptMessage->ann;
    }
}

// line 310
void UIEnchantView::EnchantItem()
{
    if (m_eState == STATE_CHOOSING_REPLACEMENT_AFFIX)
    {
        m_eState = STATE_WAITING_FOR_RESULTS;

        IntDataMessage tMessage;
        tMessage.nData = UI_GetConsoleListBoxFromID(m_idReplacementAffixListBox).GetSelectedIndex();
        MessageSendToServer(CMSG_CHOOSE_ENCHANT_AFFIX, &tMessage, sizeof(tMessage), GetParentScreen()->GetPlayer());
    }
}

BOOL UIEnchantView::IsFinishedEnchanting() const
{
    return m_eState == STATE_FINISHED;
}

BOOL UIEnchantView::IsShowingPossibleReplacements() const
{
    return m_eState == STATE_SHOWING_POSSIBLE_REPLACEMENTS;
}

BOOL UIEnchantView::CanAcceptFocus() const
{
    return m_eState != STATE_CHOOSING_ITEM;
}

// line 353
BOOL UIEnchantView::CanScrollWithTriggers() const
{
    switch (m_eState)
    {
    case STATE_CHOOSING_AFFIX:
    case STATE_SHOWING_POSSIBLE_REPLACEMENTS:
    case STATE_CHOOSING_REPLACEMENT_AFFIX:
        {
            UIID idScrollBox = GetScrollBoxUIID();
            if (UI_TestID(idScrollBox))
            {
                return UIUtils::Console::CanScrollContentInScrollBox(idScrollBox);
            }
        }
        break;
    default:
        break;
    }

    return FALSE;
}

// line 374
UIID UIEnchantView::GetScrollBoxUIID() const
{
    UIID idScrollBox = GetUIIDNone();
    switch (m_eState)
    {
    case STATE_CHOOSING_AFFIX:
    case STATE_CHOOSING_REPLACEMENT_AFFIX:
        {
            UIID idItem = GetSelectedItemUIID();
            if (idItem != GetUIIDNone())
            {
                idScrollBox = UIIDCreateFromParent(idItem, ".ScrollBox");
            }
        }
        break;
    case STATE_SHOWING_POSSIBLE_REPLACEMENTS:
        idScrollBox = UIIDCreateFromParent(m_idPossibleAffixPanel, ".PossibleAffixScrollBox");
        break;
    case STATE_FINISHED:
        idScrollBox = UIIDCreateFromParent(m_idFinishedPanel, ".NewAffix.ScrollBox");
        break;
    default:
        break;
    }

    return idScrollBox;
}

// line 402
void UIEnchantView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
}

// line 411
void UIEnchantView::OnUpdate()
{
    UpdateState();
    UpdatePendingItem();
    UpdateUI();
}

// line 422
void UIEnchantView::OnSelectedItemChanged()
{
    UpdateItemInfo();

    if (m_eState == STATE_CHOOSING_ITEM)
    {
        PopulateItemAffixList();
    }
}

// line 436
void UIEnchantView::OnGotFocus()
{
    if (SelectedItemHasEnchantedAffix())
    {
        UI_SetControllerFocus(GetUIIDNone());
    }
    else
    {
        UI_SetControllerFocus(m_idAffixListBox);
    }
}

// line 455
void UIEnchantView::OnLostFocus()
{
    UI_SetControllerFocus(GetUIIDNone());
}

// line 464
BOOL UIEnchantView::SelectedItemHasEnchantedAffix() const
{
    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    return ptACDItem != NULL && ItemHasEnchantedAffix(*ptACDItem);
}

// line 474
int32 UIEnchantView::GetNumAffixes() const
{
    return m_tAffixDataSource.GetNumItems();
}

// line 483
UIID UIEnchantView::GetSelectedItemUIID() const
{
    UIID idItem = GetUIIDNone();
    if (m_eState == STATE_CHOOSING_AFFIX)
    {
        if (SelectedItemHasEnchantedAffix())
        {
            idItem = UIIDCreateFromParent(m_idPreviouslyEnchantedPanel, ".EnchantedAffix");
        }
        else
        {
            UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(m_idAffixListBox);
            idItem = tListBox.GetItemUIID(tListBox.GetSelectedIndex());
        }
    }
    else if (m_eState == STATE_CHOOSING_REPLACEMENT_AFFIX)
    {
        UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(m_idReplacementAffixListBox);
        idItem = tListBox.GetItemUIID(tListBox.GetSelectedIndex());
    }

    return idItem;
}

// line 512
UIEnchant::AffixInfo UIEnchantView::GetSelectedAffixInfo() const
{
    int32 nIndex = SelectedItemHasEnchantedAffix() ? 0 : UI_GetConsoleListBoxFromID(m_idAffixListBox).GetSelectedIndex();
    return m_tAffixDataSource.GetAffixInfo(nIndex);
}

// line 522
inline void UIEnchantView::UpdateState()
{
    if (m_eState == STATE_SEND_ENCHANT_REQUEST)
    {
        m_eState = STATE_WAITING_FOR_ENCHANT_RESPONSE;

        EnchantAffixMessage tMessage;
        tMessage.annItem = ANNFromACD(GetSelectedItem());
        tMessage.gbidAffixToReroll = m_tSelectedAffixInfo.gbidAffix;
        MessageSendToServer(CMSG_TRY_ENCHANT_AFFIX, &tMessage, sizeof(tMessage), GetParentScreen()->GetPlayer());
    }
}

// line 539
void UIEnchantView::UpdatePendingItem()
{
    if (m_eState == STATE_WAITING_FOR_ITEM_ACDID && m_annPendingItem != ANN_INVALID)
    {
        ACDID idACDEnchantedItem = ACDFromANN(m_annPendingItem);
        ActorCommonData * ptACDEnchantedItem = ACDTryToGet(idACDEnchantedItem);
        if (ptACDEnchantedItem != NULL)
        {
            m_annPendingItem = ANN_INVALID;
            m_eState = STATE_FINISHED;
            SelectItem(idACDEnchantedItem);

            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(UIIDCreateFromParent(GetRootUIID(), ".EnchantItem"));
            tStoryboard.Reset();
            tStoryboard.Play(FALSE, 0.0f);
        }
    }
}

// line 568
void UIEnchantView::UpdateUI()
{
    if (GetParentScreen() == NULL)
    {
        return;
    }

    ACDID idACDItem = GetSelectedItem();
    if (idACDItem == ACDID_INVALID)
    {
        UIInventory::Console::ShowDetailsMode(GetRootUIID(), UIInventory::Console::DETAILS_VIEW);
        UIID idNoItems = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemDetails.NoItem");
        UI_GetWindowFromID(idNoItems).SetActive(TRUE);
        return;
    }

    UIInventory::Console::ShowDetailsMode(GetRootUIID(), UIInventory::Console::ENCHANT_VIEW);

    switch (m_eState)
    {
    case STATE_CHOOSING_ITEM:
    case STATE_CHOOSING_AFFIX:
        if (SelectedItemHasEnchantedAffix())
        {
            UpdateAlreadyEnchantedUI();
        }
        else
        {
            UpdateFullAffixListUI();
        }
        break;
    case STATE_SHOWING_POSSIBLE_REPLACEMENTS:
        UpdatePossibleReplacementsUI();
        break;
    case STATE_SHOW_PROGRESS_BAR:
        UpdateRollingUI();
        break;
    case STATE_CHOOSING_REPLACEMENT_AFFIX:
        UpdateReplacementAffixListUI();
        break;
    case STATE_FINISHED:
        UpdateFinishedUI();
        break;
    default:
        break;
    }
}

inline void UIEnchantView::UpdateFullAffixListUI()
{
    UI_GetWindowFromID(m_idAffixPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idPossibleAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idItemDataPanel).SetActive(FALSE);
}

// line 626
void UIEnchantView::UpdateAlreadyEnchantedUI()
{
    UI_GetWindowFromID(m_idAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idPossibleAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idItemDataPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idRollingReplacementPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idReplacementAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idPreviouslyEnchantedPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idFinishedPanel).SetActive(FALSE);

    if (m_tAffixDataSource.GetNumItems() > 0)
    {
        UIEnchant::AffixInfo tAffixInfo = m_tAffixDataSource.GetAffixInfo(0);
        sUpdateAffixListItemUI(UIIDCreateFromParent(m_idPreviouslyEnchantedPanel, ".EnchantedAffix"), tAffixInfo, IsFocused(), CanScrollWithTriggers());
    }
    else
    {
        UIText & tAffixText = UI_GetTextFromID(UIIDCreateFromParent(m_idPreviouslyEnchantedPanel, ".EnchantedAffix.ScrollBox.Affix"));
        tAffixText.SetText("", FALSE);
    }
}

// line 664
void UIEnchantView::UpdateRollingUI()
{
    if (m_dwRollingReplacementsEndTime == 0)
    {
        return;
    }

    uint32 dwNow = CGameGetClientTicks();
    if (dwNow < m_dwRollingReplacementsEndTime)
    {
        UI_GetWindowFromID(m_idAffixPanel).SetActive(FALSE);
        UI_GetWindowFromID(m_idPossibleAffixPanel).SetActive(FALSE);
        UI_GetWindowFromID(m_idItemDataPanel).SetActive(TRUE);
        UI_GetWindowFromID(m_idRollingReplacementPanel).SetActive(TRUE);
        UI_GetWindowFromID(m_idReplacementAffixPanel).SetActive(FALSE);
        UI_GetWindowFromID(m_idPreviouslyEnchantedPanel).SetActive(FALSE);
        UI_GetWindowFromID(m_idFinishedPanel).SetActive(FALSE);

        uint32 dwRemaining = Math::Min(m_dwRollingReplacementsEndTime - dwNow, REPLACEMENT_LIST_REVEAL_DURATION);
        float flProgress = 1.0f - (float)dwRemaining / (float)REPLACEMENT_LIST_REVEAL_DURATION;
        UI_GetProgressBarFromID(m_idRollingReplacementProgress).SetExtentsAndPosition(0.0f, 1.0f, flProgress, -1.0f);
    }
    else
    {
        m_eState = STATE_SEND_ENCHANT_REQUEST;
        m_dwRollingReplacementsEndTime = 0;
    }
}

// line 700
void UIEnchantView::UpdateReplacementAffixListUI()
{
    UI_GetWindowFromID(m_idAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idPossibleAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idItemDataPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idRollingReplacementPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idReplacementAffixPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idPreviouslyEnchantedPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idFinishedPanel).SetActive(FALSE);

    UI_GetTextFromID(UIIDCreateFromParent(m_idReplacementAffixPanel, ".SelectedAffix")).SetText(m_tSelectedAffixInfo.sAffixTooltipText.str(), FALSE);
}

// line 717
void UIEnchantView::UpdateFinishedUI()
{
    UI_GetWindowFromID(m_idAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idPossibleAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idItemDataPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idRollingReplacementPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idReplacementAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idPreviouslyEnchantedPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idFinishedPanel).SetActive(TRUE);

    UI_GetTextFromID(UIIDCreateFromParent(m_idFinishedPanel, ".SelectedAffix")).SetText(m_tSelectedAffixInfo.sAffixTooltipText.str(), FALSE);

    if (m_tAffixDataSource.GetNumItems() > 0)
    {
        UIText & tAffixText = UI_GetTextFromID(UIIDCreateFromParent(m_idFinishedPanel, ".NewAffix.NewAffixText"));
        tAffixText.SetText(m_tReplacementAffixDataSource.GetAffixInfo(UI_GetConsoleListBoxFromID(m_idReplacementAffixListBox).GetSelectedIndex()).sAffixTooltipText, FALSE);
    }
}

// line 742
void UIEnchantView::UpdatePossibleReplacementsUI()
{
    UI_GetWindowFromID(m_idAffixPanel).SetActive(FALSE);
    UI_GetWindowFromID(m_idPossibleAffixPanel).SetActive(TRUE);
    UI_GetWindowFromID(m_idItemDataPanel).SetActive(FALSE);

    UI_GetTextFromID(UIIDCreateFromParent(m_idPossibleAffixPanel, ".InspectedAffixPersist")).SetText(GetSelectedAffixInfo().sAffixTooltipText.str(), FALSE);
}

// line 755
void UIEnchantView::UpdateItemInfo()
{
    if (GetParentScreen() == NULL)
    {
        return;
    }

    Player * ptPlayer = GetParentScreen()->GetPlayer();
    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (ptPlayer != NULL && ptACDItem != NULL)
    {
        UIID idItemUI = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemEnchant.EnchantDetails.ItemDataContainer.ItemInfo");
        UIInventory::Console::UpdateViewItemUI(idItemUI, *ptACDItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, -2, FALSE, TRUE, FALSE, TRUE, 0);
    }
}

// line 775
inline void UIEnchantView::PopulateItemAffixList()
{
    m_tAffixDataSource.PopulateItemAffixList(GetSelectedItem(), PlayerGetPlayerClass(GetParentScreen()->GetPlayer()));
    UI_GetConsoleListBoxFromID(m_idAffixListBox).ResetAllItems();
}

// line 785
inline UIEnchantView::EnchantAffixDataSource::EnchantAffixDataSource(int32 nMaxItemsWithoutScrolling)
    : m_nOriginalAffixIndex(-1)
    , m_nMaxItemsWithoutScrolling(nMaxItemsWithoutScrolling)
{
}

// line 793
int32 UIEnchantView::EnchantAffixDataSource::GetNumItems() const
{
    return m_arItemAffixes.size();
}

// line 802
UIControlSourceKey UIEnchantView::EnchantAffixDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    LPCSTR szSnoAndPath = "ConsoleInventoryItem:ConsoleInventoryItem_main.LayoutRoot.ItemEnchant.EnchantDetails.AffixContainer.AffixList.AffixListItem";
    if (m_arItemAffixes.size() <= m_nMaxItemsWithoutScrolling)
    {
        szSnoAndPath = "ConsoleInventoryItem:ConsoleInventoryItem_main.LayoutRoot.ItemEnchant.EnchantDetails.ItemDataContainer.ReplacingContainer.AffixListLG.AffixListItemLG";
    }

    return UIControlSourceKey::Parse(szSnoAndPath, SNO_NONE, NULL, NULL);
}

// line 818
void UIEnchantView::EnchantAffixDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

// line 833
void UIEnchantView::EnchantAffixDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    BOOL bIsSelected = UI_GetControllerFocus() == tListBox.GetID() && tListBox.GetSelectedIndex() == nIndex;
    BOOL bCanScroll = UIUtils::Console::CanScrollContentInScrollBox(UIIDCreateFromParent(idItemUI, ".ScrollBox"));

    sUpdateAffixListItemUI(idItemUI, m_arItemAffixes[nIndex], bIsSelected, bCanScroll);
}

// line 847
BOOL UIEnchantView::EnchantAffixDataSource::IsItemEnabled(const UIConsoleListBox & tListBox, const int32 nIndex)
{
    return !m_arItemAffixes[nIndex].bIsInherentAffix;
}

// line 858
inline UIEnchant::AffixInfo UIEnchantView::EnchantAffixDataSource::GetAffixInfo(const int32 nIndex) const
{
    return m_arItemAffixes[nIndex];
}

// line 867
inline void UIEnchantView::EnchantAffixDataSource::Clear()
{
    m_arItemAffixes.clear();
}

// line 876
inline void UIEnchantView::EnchantAffixDataSource::PopulateItemAffixList(const ACDID idACDItem, const PlayerClass ePlayerClass)
{
    m_arItemAffixes.clear();
    UIEnchant::EnumItemAffixes(idACDItem, ePlayerClass, m_arItemAffixes);
}

// line 888
inline void UIEnchantView::EnchantAffixDataSource::PopulateReplacementAffixList(const ACDID idACDItem, const PlayerClass ePlayerClass, const GBID gbidSelectedAffix, const EnchantAffixChoice * arEnchantAffixChoices, int32 nEnchantAffixChoicesCount)
{
    m_arItemAffixes.clear();
    UIEnchant::EnumReplacementItemAffixes(idACDItem, ePlayerClass, gbidSelectedAffix, arEnchantAffixChoices, nEnchantAffixChoicesCount, m_arItemAffixes, m_nOriginalAffixIndex);
}

}  // namespace Console
}  // namespace UIMystic
