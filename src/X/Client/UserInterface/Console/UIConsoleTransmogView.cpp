// Original file: X/Client/UserInterface/Console/UIConsoleTransmogView.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleTransmogView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryViewUtilities.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../Controls/UIConsoleListboxControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UITransmogrifyCommon.h"
#include "../../Gfx/GfxCommon.h"
#include "../../Sound/Sound.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/RGBAColor.h"
#include "ControllerActions.h"
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); const UIID & UI_GetControllerFocus();
namespace Console {  namespace Screens { ControllerActionManager & GetActionManager(); } }
namespace UIInventory { namespace Console { void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode); void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags); } }
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType, const PlayerClass eClass, const BOOL fIsMale);
namespace UITransmogrify { void EnumDisplayedTransmogs(const Player * ptPlayer, InventorySlotType eSelectedSlot, const ActorCommonData * ptACDItem, DisplayedTransmogArray & arTransmogs, BOOL bAllowUnavailable); void UpdateTransmogLegendaryBackground(const UIID & idBackground, GBID gbidItem); }
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer); struct GBIDDataMessage { RequiredMessageHeader tHeader; GBID gbid; }; struct TransmogItemMessage { RequiredMessageHeader tHeader; ACDNetworkName annItem; GBID gbidTransmog; };
static void sOnListSelectionChanged(const UIID & id, const int32 nOldVal, const int32 nNewVal)
{
    Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);

    const UIID & idItem = UI_GetConsoleListBoxFromID(id).GetItemUIID(nNewVal);
    if (idItem != GetUIIDNone())
    {
        const UIID idStoryboard = UIIDCreateFromParent(idItem, ".ItemListHighlight");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(FALSE, 0.0f); }
}
namespace UIMystic { namespace Console {

UITransmogView::UITransmogView(const UIID & idRoot)
    : UIInventoryView(idRoot)
    , m_eState(STATE_CHOOSING_ITEM)
    , m_gbidLastTransmog(GBID_NONE)
{
    GetListBox().SetDataSource(m_tDataSource);
    GetListBox().SetOnSelChangeFunction(sOnListSelectionChanged);
}



void UITransmogView::GoBack()
{
    if (m_eState == STATE_CHOOSING_TRANSMOG)
    {
        m_eState = STATE_CHOOSING_ITEM;
        GetListBox().SetSelectedIndex(-1);
    }
}





void UITransmogView::Reset()
{

    m_eState = STATE_CHOOSING_ITEM;
    m_tDataSource.Clear();
    GetListBox().ResetAllItems();
    SelectItem(ACDID_INVALID);
}




BOOL UITransmogView::HasTransmogs() const
{
    return m_tDataSource.GetNumItems() > 0;
}





void UITransmogView::ChooseTransmogForSelectedItem()
{

    if (m_eState == STATE_CHOOSING_ITEM)
    {
        m_eState = STATE_CHOOSING_TRANSMOG;


        const int32 nIndex = GetCurrentTransmogIndex();
        GetListBox().SetSelectedIndex(nIndex != -1 ? nIndex : 0);
    }
}




void UITransmogView::Transmogrify()
{

    const ACDID idSelectedItem = GetSelectedItem();
    const GBID gbidTransmog = GetSelectedTransmog();

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem != NULL)
    {



        if (!ItemCrafting_ItemTransmogsMatch(*ptACDItem, gbidTransmog))
        {
            m_gbidLastTransmog = ptACDItem->GetVisualGBID();

            TransmogItemMessage tMessage;
            tMessage.annItem = ANNFromACD(idSelectedItem);
            tMessage.gbidTransmog = gbidTransmog;
            MessageSendToServer(0x160, &tMessage, sizeof(tMessage), GetParentScreen()->GetPlayer());


            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TRANSMOG_ITEM), cgtPSODefaults);

            m_eState = STATE_WAITING_FOR_REPLY;

            UI_SetControllerFocus(GetUIIDNone());
        }
        else
        {
            FinishTransmog();
        }
    }
}



BOOL UITransmogView::IsChoosingTransmog() const
{
    return m_eState == STATE_CHOOSING_TRANSMOG;
}





BOOL UITransmogView::IsFinishedTransmogrifying() const
{
    return m_eState == STATE_FINISHED;
}





BOOL UITransmogView::CanAcceptFocus() const
{
    return TRUE;
}





GBID UITransmogView::GetSelectedTransmog() const
{

    const int32 nIndex = GetListBox().GetSelectedIndex();
    if (nIndex != -1)
    {



        return m_tDataSource.GetTransmog(nIndex);
    }
    return GBID_NONE;
}


void UITransmogView::OnTransmogResults(const TransmogItemMessage & tTransmogResults)
{

    if (m_eState == STATE_WAITING_FOR_REPLY)
    {
        const ACDID idACDTransmogItem = ACDFromANN(tTransmogResults.annItem);
        SelectItem(idACDTransmogItem);
        m_eState = STATE_WAITING_FOR_RESULTS;
    }
}




void UITransmogView::OnUnlockTransmogResults(const GBIDDataMessage * ptMessage)
{

    if (m_eState < STATE_WAITING_FOR_REPLY || m_eState > STATE_FINISHED)
    {


        PopulateTransmogs();
    }
}




void UITransmogView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
}


void UITransmogView::OnParentScreenChanged()
{

    m_tDataSource.SetPlayer(GetParentScreen() != NULL ? GetParentScreen()->GetPlayer() : NULL);
}




void UITransmogView::OnUpdate()
{
    UpdateState();
    UpdateUI();
}





void UITransmogView::OnSelectedItemChanged()
{

    m_tDataSource.SetCurrentItem(GetSelectedItem());
    UpdateItemVisual();
    PopulateTransmogs();

    const int32 nIndex = GetCurrentTransmogIndex();
    GetListBox().SetSelectedIndex(nIndex != -1 ? nIndex : 0);
}




void UITransmogView::OnGotFocus()
{
    UI_SetControllerFocus(GetListBox().GetID());
}





void UITransmogView::OnLostFocus()
{
    UI_SetControllerFocus(GetUIIDNone());
}





inline UIConsoleListBox & UITransmogView::GetListBox() const
{

    const UIID idListBox = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemTransmog.TransmogDetails.TransmogList");
    return UI_GetConsoleListBoxFromID(idListBox);
}




void UITransmogView::UpdateState()
{

    if (m_eState == STATE_WAITING_FOR_RESULTS)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
        if (ptACDItem == NULL) { m_gbidLastTransmog = GBID_NONE; FinishTransmog(); }

        else if (m_gbidLastTransmog != ptACDItem->GetVisualGBID())
        {
            m_gbidLastTransmog = GBID_NONE;
            FinishTransmog();
        }
    }
}













void UITransmogView::UpdateUI()
{

    if (GetParentScreen() == NULL)
    {
        return;
    }


    if (GetSelectedItem() == ACDID_INVALID)
    {
        UIInventory::Console::ShowDetailsMode(GetRootUIID(), UIInventory::Console::DETAILS_VIEW);


        const UIID idNoItems = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemDetails.NoItem");
        UI_GetWindowFromID(idNoItems).SetActive(TRUE);
    }
    else
    {
        UIInventory::Console::ShowDetailsMode(GetRootUIID(), UIInventory::Console::TRANSMOG_VIEW);


        const GBID gbidTransmog = GetSelectedTransmog();
        UIText & tCost = UI_GetTextFromID(UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemTransmog.TransmogDetails.TransmogCost"));
        tCost.SetActive(gbidTransmog != GBID_NONE && IsChoosingTransmog());

        if (gbidTransmog != GBID_NONE)
        {
            const ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
            const ActorCommonData * ptACDPlayer = ACDTryToGet(GetParentScreen()->GetPlayer()->idPlayerACD);
            if (ptACDItem != NULL && ptACDPlayer != NULL)
            {



                if (ItemCrafting_ItemTransmogsMatch(*ptACDItem, gbidTransmog))
                {


                    tCost.SetText(StringListFetch("X1_ConsoleMystic:CurrentTransmog").str(), FALSE);
                }
                else
                {
                    ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
                    const CurrencyType eCurrencyType = CURRENCY_GOLD;
                    int32 nTransmogCost = pdefTransmog->nTransmogCost;
                    if (ItemCrafting_ItemTransmogsMatch(ptACDItem->hGB.gbid, gbidTransmog))
                    {
                        nTransmogCost = 0;
                    }



                    CRefString sCostText = UIText_ValueToGameCurrency(nTransmogCost, eCurrencyType);
                    const BOOL bPlayerCanAfford = ptACDPlayer->CanAffordCurrencyCost(nTransmogCost, eCurrencyType);
                    if (!bPlayerCanAfford)
                    {
                        Set_Text_Color(sCostText, crgbaRed);
                    }

                    CRefString sCostFormat = LocalizedSprintf(StringListFetch("X1_ConsoleMystic:TransmogCost").str(),
                        sCostText.str(), NULL);

                    tCost.SetText(sCostFormat, FALSE);
                }
            }
        }
    }
}



inline void UITransmogView::UpdateItemVisual()
{
    Player * ptPlayer = NULL;
    if (GetParentScreen() != NULL)
    {



        ptPlayer = GetParentScreen()->GetPlayer();
        if (ptPlayer != NULL)
        {
            const UIID idItemVisual = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemTransmog.TransmogDetails.ItemVisual");
            UIInventory::Console::SetUpItemButton(idItemVisual, GetSelectedItem(), ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 1);
        }
    }
}



inline void UITransmogView::PopulateTransmogs()
{

    m_tDataSource.PopulateTransmogs(GetParentScreen()->GetPlayer(), GetSelectedItem());
    GetListBox().ResetAllItems();
}




int32 UITransmogView::GetCurrentTransmogIndex() const
{

    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (ptACDItem != NULL)
    {
        for (int32 i = 0; i < m_tDataSource.GetNumItems(); ++i)
        {
            const GBID gbidTransmog = m_tDataSource.GetTransmog(i);
            ScopedItemDefinitionLockR pdefTransmog(gbidTransmog);
            if (pdefTransmog != NULL &&

                ItemCrafting_ItemTransmogsMatch(*ptACDItem, gbidTransmog))
            {
                return i;
            }
        }
    }



    return -1;
}



void UITransmogView::FinishTransmog()
{

    m_eState = STATE_FINISHED;
    OnSelectedItemChanged();


    const int32 nIndex = GetListBox().GetSelectedIndex();
    if (nIndex != -1)
    {
        const UIID & idItem = GetListBox().GetItemUIID(nIndex);
        if (idItem != GetUIIDNone())
        {
            const UIID idStoryboard = UIIDCreateFromParent(idItem, ".TransmogComplete");
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
            tStoryboard.Stop();
            tStoryboard.Play(FALSE, 0.0f);
        }
    }
}




inline UITransmogView::TransmogDataSource::TransmogDataSource()
    : m_ptPlayer(NULL)
    , m_idACDItem(ACDID_INVALID)
{
}



int32 UITransmogView::TransmogDataSource::GetNumItems() const
{

    return m_arTransmogs.size();
}




inline GBID UITransmogView::TransmogDataSource::GetTransmog(const int32 nIndex) const
{

    return m_arTransmogs[nIndex].gbidTransmog;
}




UIControlSourceKey UITransmogView::TransmogDataSource::GetSourceKeyForItem(const int32 nIndex) const
{
    return UIControlSourceKey::Parse("ConsoleInventoryListMysticTemplate:ConsoleInventoryListMysticTemplate_main", SNO_NONE, NULL, NULL);
}





void UITransmogView::TransmogDataSource::InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{
}












void UITransmogView::TransmogDataSource::UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex)
{



    if (m_ptPlayer != NULL)
    {



        UITransmogrify::DisplayedTransmog & tDisplayedTransmog = m_arTransmogs[nIndex];
        ScopedItemDefinitionLockR pdefTransmog(tDisplayedTransmog.gbidTransmog);
        if (pdefTransmog != NULL)
        {



            const PlayerClass ePlayerClass = PlayerGetPlayerClass(m_ptPlayer);
            const BOOL bIsMale = PlayerIsMale(*m_ptPlayer);

            ImageTextureFrame tImageTexFrame;
            ACDInventoryGetItemDrawInfoWithGBID(tDisplayedTransmog.gbidTransmog, tImageTexFrame, 0, ePlayerClass, bIsMale);
            UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisual")).SetTextureFrame(tImageTexFrame);

            const UIID idItemBackground = UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisualLoadBG");
            UITransmogrify::UpdateTransmogLegendaryBackground(idItemBackground, tDisplayedTransmog.gbidTransmog);

            BOOL bIsCurrentTransmog = FALSE;
            ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
            if (ptACDItem != NULL)
            {
                bIsCurrentTransmog = ItemCrafting_ItemTransmogsMatch(*ptACDItem, tDisplayedTransmog.gbidTransmog);
            }

            UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.CurrentTransmogMarker")).SetActive(bIsCurrentTransmog);

            const BOOL bIsHighlighted = UI_GetControllerFocus() == tListBox.GetID() && tListBox.GetSelectedIndex() == nIndex;
            UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.Selected")).SetActive(bIsHighlighted);
        }
    }
}


inline void UITransmogView::TransmogDataSource::Clear()
{

    m_arTransmogs.clear();
}




void UITransmogView::TransmogDataSource::PopulateTransmogs(const Player * ptPlayer, const ACDID idACDItem)
{


    m_arTransmogs.clear();

    const ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem != NULL && ItemCrafting_CanTransmogItem(*ptACDItem, GBID_NONE, ptPlayer, TRUE, TRUE))
    {
        const InventorySlotType eSlot = ItemCrafting_GetTransmogSlot(ptACDItem->hGB.gbid);
        UITransmogrify::EnumDisplayedTransmogs(ptPlayer, eSlot, ptACDItem, m_arTransmogs, FALSE);
    }
}

} }  // namespace UIMystic::Console
