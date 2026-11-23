#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../../Contrib/Contrib/blz/include/blz/iter_range.h"
#include "UIConsoleDyeItemView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIConsoleListBox & UI_GetConsoleListBoxFromID(const UIID & id);
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
void UI_SetControllerFocus(const UIID & id);
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
// X/Client/Console/DLCManager.cpp
namespace Console {
namespace DLCManager {
uint32 GetLicenses();
}  // namespace DLCManager
}  // namespace Console
// X/Client/Console/GamerProfile.cpp
namespace Console {
namespace GamerProfile {
D3::Account::SavedDefinition & GetAccount(int32 nUserIdx);
}  // namespace GamerProfile
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.h
namespace UIInventory {
namespace Console {
enum DetailsViewMode {
    DETAILS_VIEW = 0,
    DYE_ITEM_VIEW = 9,
};
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory {
namespace Console {
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode);
}  // namespace Console
}  // namespace UIInventory
// X/Client/UserInterface/Interface/UIDyeItemCommon.cpp
namespace UIDyeItem {
void GetAllDyes(GBIDArray & arDyes);
DyeType GetDyeTypeForItemGBID(GBID gbidItem);
bool ItemHasDye(const ActorCommonData & tACDItem, GBID gbidDye);
bool IsPromoDye(GBID gbidDye);
}  // namespace UIDyeItem
// X/Common/Crafting/ItemCrafting.h
struct DyeItemMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    DyeType eDyeType;  // 0x0c
};
static_assert(sizeof(DyeItemMessage) == 0x10, "DyeItemMessage");
#include "../../../Common/Networking/GameMessage.h"



// X/Client/CGame.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Common/ActorCommonData.cpp
ACDNetworkName ANNFromACD(const ACDID idACD);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Common/Items.cpp
GBID ItemGetGBIDForDyeType(DyeType eDyeType);
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
CRefString ItemGetDisplayName(const GBID gbidItem);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(LPCSTR szLabel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/Controls/UITextControl.cpp
void Set_Text_Color(CRefString & sText, const RGBAColor & rgbaColor);
// X/Common/Player.cpp
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
BOOL PlayerIsMale(const Player & tPlayer);
// X/Client/ACDClientInventory.cpp
typedef int32 ItemDrawType;
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);

static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    UIConsoleListBox & tListBox = UI_GetConsoleListBoxFromID(id);
    const UIID & idItem = tListBox.GetItemUIID(nNewVal);
    if (idItem != GetUIIDNone())
    {
        UIID idStoryboard = UIIDCreateFromParent(idItem, ".ItemListHighlight");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f);
    }
}

static bool sPlayerHasCollectorsEdition(const Player & tPlayer)
{
    uint32 uLicenses = Console::DLCManager::GetLicenses();
    uLicenses |= Console::GamerProfile::GetAccount(tPlayer.nControllerIndex).console_data().legacy_license_bits();
    return (uLicenses & 1) != 0;
}

namespace UIMystic {
namespace Console {

UIDyeItemView::UIDyeItemView(const UIID & idRoot)
    : UIInventoryView(idRoot)
    , m_eState(STATE_CHOOSING_ITEM)
    , m_eLastDyeType(0)
{
    UIConsoleListBox & tListBox = GetListBox();
    tListBox.SetDataSource(m_tDataSource);
    tListBox.SetOnSelChangeFunction(sOnListSelectionChanged);
    tListBox.SetUserData(this);
}

UIDyeItemView::~UIDyeItemView()
{
    UIConsoleListBox & tListBox = GetListBox();
    tListBox.SetUserData(NULL);
}

void UIDyeItemView::GoBack()
{
    if (m_eState == STATE_CHOOSING_DYE)
    {
        m_eState = STATE_CHOOSING_ITEM;
        GetListBox().SetSelectedIndex(-1);
    }
}

void UIDyeItemView::Reset()
{
    m_eState = STATE_CHOOSING_ITEM;
    m_tDataSource.Clear();
    GetListBox().ResetAllItems();
    SelectItem(ACDID_INVALID);
}

BOOL UIDyeItemView::HasDyes() const
{
    return m_tDataSource.GetNumItems() > 0;
}

void UIDyeItemView::ChooseDyeForSelectedItem()
{
    if (m_eState == STATE_CHOOSING_ITEM)
    {
        m_eState = STATE_CHOOSING_DYE;

        int32 nIndex = GetCurrentDyeIndex();
        GetListBox().SetSelectedIndex(nIndex == -1 ? 0 : nIndex);
    }
}

void UIDyeItemView::ApplyDye()
{
    ACDID idSelectedItem = GetSelectedItem();
    GBID gbidDye = GetSelectedDye();
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem == NULL)
    {
        return;
    }

    if (UIDyeItem::ItemHasDye(*ptACDItem, gbidDye))
    {
        FinishApplyingDye();
    }
    else
    {
        SendDyeMessage(*ptACDItem, UIDyeItem::GetDyeTypeForItemGBID(gbidDye));
    }
}

void UIDyeItemView::RemoveDye()
{
    ACDID idSelectedItem = GetSelectedItem();
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem == NULL)
    {
        return;
    }

    if (ptACDItem->GetDyeType() == 0)
    {
        FinishApplyingDye();
    }
    else
    {
        SendDyeMessage(*ptACDItem, 0);
    }
}

inline void UIDyeItemView::SendDyeMessage(const ActorCommonData & tACDItem, DyeType eDyeType)
{
    m_eLastDyeType = tACDItem.GetDyeType();

    DyeItemMessage tMessage;
    tMessage.annItem = ANNFromACD(tACDItem.id);
    tMessage.eDyeType = eDyeType;
    MessageSendToServer(CMSG_TRY_DYE_ITEM, &tMessage, sizeof(tMessage), GetParentScreen()->GetPlayer());

    // The original passes 307, the transmog sound, not GLOBALSNO_SOUND_DYE_ITEM (310).
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRANSMOG_ITEM), cgtPSODefaults);

    m_eState = STATE_WAITING_FOR_REPLY;
    UI_SetControllerFocus(GetUIIDNone());
}

BOOL UIDyeItemView::IsChoosingDye() const
{
    return m_eState == STATE_CHOOSING_DYE;
}

BOOL UIDyeItemView::IsFinishedDyeing() const
{
    return m_eState == STATE_FINISHED;
}

BOOL UIDyeItemView::CanAcceptFocus() const
{
    return TRUE;
}

GBID UIDyeItemView::GetSelectedDye() const
{
    int32 nIndex = GetListBox().GetSelectedIndex();
    if (nIndex == -1)
    {
        return -1;
    }

    return m_tDataSource.GetDye(nIndex);
}

bool UIDyeItemView::IsSelectedItemDyed()
{
    ACDID idACDSelectedItem = GetSelectedItem();
    const ActorCommonData * ptACDSelectedItem = ACDTryToGet(idACDSelectedItem);
    if (ptACDSelectedItem == NULL)
    {
        return false;
    }

    return ptACDSelectedItem->GetDyeType() != 0;
}

void UIDyeItemView::OnDyeItemResults(const DyeItemMessage & tDyeItemResults)
{
    if (m_eState == STATE_WAITING_FOR_REPLY)
    {
        m_eState = STATE_WAITING_FOR_RESULTS;
    }
}

void UIDyeItemView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
}

void UIDyeItemView::OnParentScreenChanged()
{
    m_tDataSource.SetPlayer(GetParentScreen() != NULL ? GetParentScreen()->GetPlayer() : NULL);
}

void UIDyeItemView::OnUpdate()
{
    UpdateState();
    UpdateUI();
}

void UIDyeItemView::OnSelectedItemChanged()
{
    m_tDataSource.SetCurrentItem(GetSelectedItem());
    PopulateDyes();

    int32 nIndex = GetCurrentDyeIndex();
    GetListBox().SetSelectedIndex(nIndex);
}

void UIDyeItemView::OnGotFocus()
{
    UI_SetControllerFocus(GetListBox().GetID());
}

void UIDyeItemView::OnLostFocus()
{
    UI_SetControllerFocus(GetUIIDNone());
}

inline UIConsoleListBox & UIDyeItemView::GetListBox() const
{
    UIID idListBox = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemDye.DyeDetails.DyeList");
    return UI_GetConsoleListBoxFromID(idListBox);
}

void UIDyeItemView::UpdateState()
{
    if (m_eState == STATE_WAITING_FOR_RESULTS)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
        if (ptACDItem == NULL)
        {
            m_eLastDyeType = 0;
            FinishApplyingDye();
        }
        else if (m_eLastDyeType != ptACDItem->GetDyeType())
        {
            m_eLastDyeType = 0;
            FinishApplyingDye();
        }
    }
}

void UIDyeItemView::UpdateUI()
{
    if (GetParentScreen() == NULL)
    {
        return;
    }

    if (GetSelectedItem() == ACDID_INVALID)
    {
        UIInventory::Console::ShowDetailsMode(GetRootUIID(), UIInventory::Console::DETAILS_VIEW);
        UIID idNoItems = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemDetails.NoItem");
        UI_GetWindowFromID(idNoItems).SetActive(TRUE);
        return;
    }

    UIInventory::Console::ShowDetailsMode(GetRootUIID(), UIInventory::Console::DYE_ITEM_VIEW);

    GBID gbidDye = GetSelectedDye();
    if (gbidDye == -1)
    {
        ACDID idACDItem = GetSelectedItem();
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem != NULL)
        {
            gbidDye = ItemGetGBIDForDyeType(ptACDItem->GetDyeType());
        }
    }

    DyeType eDyeType = UIDyeItem::GetDyeTypeForItemGBID(gbidDye);

    UIText & tCost = UI_GetTextFromID(UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemDye.DyeDetails.DyeCost"));
    CRefString sText;

    if (gbidDye != -1 && eDyeType != 0)
    {
        const ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
        const ActorCommonData * ptACDPlayer = ACDTryToGet(GetParentScreen()->GetPlayer()->idPlayerACD);
        if (ptACDItem == NULL || ptACDPlayer == NULL)
        {
            return;
        }

        ScopedItemDefinitionLockR pdefDye(gbidDye);
        CurrencyType eCurrencyType = 0;
        int32 nDyeCost = pdefDye->nBaseCostGold;
        CRefString sCostText = UIText_ValueToGameCurrency(nDyeCost, eCurrencyType);
        BOOL bPlayerCanAfford = ptACDPlayer->CanAffordCurrencyCost(nDyeCost, eCurrencyType);
        if (!bPlayerCanAfford)
        {
            Set_Text_Color(sCostText, crgbaRed);
        }

        if (UIDyeItem::ItemHasDye(*ptACDItem, gbidDye))
        {
            sCostText = StringListFetch("X1_ConsoleMystic:DyeItemCurrentLook");
            CRefString sItemName = ItemGetDisplayName(gbidDye);
            sText = LocalizedSprintf(StringListFetch("X1_ConsoleMystic:DyeItemInfo"), sItemName.str(), sCostText.str(), NULL);
        }
        else
        {
            sCostText = UIText_ValueToGameCurrency(nDyeCost, eCurrencyType);
            if (!ptACDPlayer->CanAffordCurrencyCost(nDyeCost, eCurrencyType))
            {
                Set_Text_Color(sCostText, crgbaRed);
            }

            CRefString sItemName = ItemGetDisplayName(gbidDye);
            sText = LocalizedSprintf(StringListFetch("X1_ConsoleMystic:DyeItemInfo"), sItemName.str(), sCostText.str(), NULL);
        }
    }
    else
    {
        sText = StringListFetch("X1_ConsoleMystic:NoDye");
    }

    tCost.SetText(sText, FALSE);
}

inline void UIDyeItemView::PopulateDyes()
{
    m_tDataSource.PopulateDyes(GetParentScreen()->GetPlayer(), GetSelectedItem());
    GetListBox().ResetAllItems();
}

inline int32 UIDyeItemView::GetCurrentDyeIndex() const
{
    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (ptACDItem != NULL)
    {
        for (int32 i = 0; i < m_tDataSource.GetNumItems(); ++i)
        {
            GBID gbidDye = m_tDataSource.GetDye(i);
            if (UIDyeItem::ItemHasDye(*ptACDItem, gbidDye))
            {
                return i;
            }
        }
    }

    return -1;
}

void UIDyeItemView::FinishApplyingDye()
{
    m_eState = STATE_FINISHED;
    OnSelectedItemChanged();

    int32 nIndex = GetListBox().GetSelectedIndex();
    if (nIndex != -1)
    {
        const UIID & idItem = GetListBox().GetItemUIID(nIndex);
        if (idItem != GetUIIDNone())
        {
            UIID idStoryboard = UIIDCreateFromParent(idItem, ".DyeComplete");
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
            tStoryboard.Stop();
            tStoryboard.Play(FALSE, 0.0f);
        }
    }
}

inline UIDyeItemView::DyeDataSource::DyeDataSource()
    : m_ptPlayer(NULL)
    , m_idACDItem(ACDID_INVALID)
{
}

int32 UIDyeItemView::DyeDataSource::GetNumItems() const
{
    return m_arDyes.size();
}

inline GBID UIDyeItemView::DyeDataSource::GetDye(const int32 nIndex) const
{
    return m_arDyes[nIndex];
}

UIControlSourceKey UIDyeItemView::DyeDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleInventoryListDyeTemplate:ConsoleInventoryListDyeTemplate_main", SNO_NONE, NULL, NULL);
}

void UIDyeItemView::DyeDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}

void UIDyeItemView::DyeDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
    if (m_ptPlayer == NULL)
    {
        return;
    }

    GBID gbidDye = m_arDyes[nIndex];
    if (gbidDye == -1)
    {
        return;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(m_ptPlayer);
    BOOL bIsMale = PlayerIsMale(*m_ptPlayer);

    ImageTextureFrame tImageTexFrame;
    ACDInventoryGetItemDrawInfoWithGBID(gbidDye, tImageTexFrame, 0, ePlayerClass, bIsMale);
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisual")).SetTextureFrame(tImageTexFrame);

    ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
    BOOL bIsCurrentDye = ptACDItem != NULL && UIDyeItem::ItemHasDye(*ptACDItem, gbidDye);
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.CurrentDyeMarker")).SetActive(bIsCurrentDye);

    BOOL bIsHighlighted = UI_GetControllerFocus() == tListBox.GetID() && tListBox.GetSelectedIndex() == nIndex;
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.Selected")).SetActive(bIsHighlighted);
}

inline void UIDyeItemView::DyeDataSource::Clear()
{
    m_arDyes.clear();
}

void UIDyeItemView::DyeDataSource::PopulateDyes(const Player * ptPlayer, const ACDID idACDItem)
{
    m_arDyes.clear();
    UIDyeItem::GetAllDyes(m_arDyes);

    bool bHasCollectorsEdition = sPlayerHasCollectorsEdition(*ptPlayer);

    int * iter = blz::remove_if(blz::begin(m_arDyes), blz::end(m_arDyes),
        [bHasCollectorsEdition](GBID gbidDye) { return UIDyeItem::IsPromoDye(gbidDye) && !bHasCollectorsEdition; });
    m_arDyes.resize(iter - blz::begin(m_arDyes));
}

}  // namespace Console
}  // namespace UIMystic
