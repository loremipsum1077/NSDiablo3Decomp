// Original file: X/Client/UserInterface/Console/UIConsoleInventoryAddSocketsView.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryAddSocketsView.h"
#include "UIConsoleInventoryScreen.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet (UI.cpp, Items.cpp, UIConsoleInventoryViewUtilities.cpp).
UIWindow & UI_GetWindowFromID(const UIID & id); UIText & UI_GetTextFromID(const UIID & id); UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
CRefString ItemGetFlavorText(const ActorCommonData & tACDItem); CRefString ItemGetPrimaryDescription(const ActorCommonData & tACD);  // Items.cpp:9962, 11319
namespace UIInventory { namespace Console {
enum DetailsViewMode { SOCKETING_VIEW = 2 };
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode);
void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags);
void UpdateItemSocketsUI(const UIID & idSockets, const ACDID idACDItem, const ACDID idACDOwner);
} }

namespace UIInventory {
namespace Console {




UIInventoryAddSocketsView::UIInventoryAddSocketsView(const UIID & idRoot)
    : UIInventoryView(idRoot)
    , m_idAddSocketsItem(ACDID_INVALID)
    , m_idAddSocketsRecipient(ACDID_INVALID)
    , m_eState(DEFAULT_STATE)
{
}





void UIInventoryAddSocketsView::SetAddSocketsItem(const ACDID idAddSocketsItem)
{
    if (m_idAddSocketsItem != idAddSocketsItem)
    {
        m_idAddSocketsItem = idAddSocketsItem;
        RequestUpdate(1);
    }
}





void UIInventoryAddSocketsView::SetAddSocketsRecipient(const ACDID idAddSocketsRecipient)
{
    if (m_idAddSocketsRecipient != idAddSocketsRecipient)
    {
        m_idAddSocketsRecipient = idAddSocketsRecipient;
        RequestUpdate(1);
    }
}





// 2.7.7 calls separate state setters before the storyboard operations: sub_7100374540 / sub_7100374590
// (declared in the header; defined in sub_7100374540.cpp because this file's Reset/StartAddingSockets must not inline them).
void UIInventoryAddSocketsView::Reset()
{
    sub_7100374540(this);

    const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".AddSocket1");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Reset();
}






void UIInventoryAddSocketsView::StartAddingSockets()
{
    sub_7100374590(this);

    const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".AddSocket1");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Play(FALSE, 0.0f);
}






BOOL UIInventoryAddSocketsView::IsFinishedAddingSockets() const
{

    return m_eState == FINISHED_STATE;
}




void UIInventoryAddSocketsView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    const UIID idItemSocket = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemSocket");
    UI_GetWindowFromID(idItemSocket).SetActive(bShow);
}








void UIInventoryAddSocketsView::OnUpdate()
{
    if (m_eState == ADDING_SOCKETS_STATE)
    {
        const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".AddSocket1");


        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        BOOL bAnimIsFinished = tStoryboard.IsPlaying(); ActorCommonData * ptACDRecipient = ACDTryToGet(GetAddSocketsRecipient());









        BOOL bIsFinished = bAnimIsFinished && ptACDRecipient == NULL;

        if (bIsFinished)
        {


            m_eState = FINISHED_STATE;

            if (GetParentScreen() != NULL)
            {
                GetParentScreen()->RequestUpdate(1);
            }
        }
    }

    UpdateUI();
}


void UIInventoryAddSocketsView::OnSelectedItemChanged()
{
    UpdateUI();
}





void UIInventoryAddSocketsView::UpdateUI()
{
    UIInventoryScreen * ptParentScreen = GetParentScreen(); if (ptParentScreen == NULL)
        return;
    const UIID & idRoot = GetRootUIID();






    ShowDetailsMode(idRoot, SOCKETING_VIEW);

    const UIID idDetails = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemSocket.SocketDetails");

    ActorCommonData * ptACDItem = ACDTryToGet(GetAddSocketsItem());
    if (ptACDItem == NULL)
        return;




    const UIID idJewelInfo = UIIDCreateFromParent(idDetails, ".JewelInfo");
    CRefString sName = ItemGetDisplayName(*ptACDItem, TRUE, FALSE, TRUE, TRUE);
    UI_GetTextFromID(idJewelInfo).SetText(sName.str(), FALSE);


    const ACDID idSelectedItem = GetSelectedItem();

    const UIID idFrameBottom = UIIDCreateFromParent(idDetails, ".FrameBottom");
    UI_GetWindowFromID(idFrameBottom).SetActive(idSelectedItem != ACDID_INVALID);

    Player * ptPlayer = ptParentScreen->GetPlayer();
    if (ptPlayer != NULL)
    {

        const ACDID idJewel = IsFinishedAddingSockets() ? ACDID_INVALID : m_idAddSocketsItem;

        const UIID idSocketJewel = UIIDCreateFromParent(idDetails, ".SocketJewel");
        SetUpItemButton(idSocketJewel, idJewel, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);


        const UIID idItemVisual = UIIDCreateFromParent(idFrameBottom, ".ItemVisual");
        SetUpItemButton(idItemVisual, GetSelectedItem(), ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 1);


        const UIID idGemSockets = UIIDCreateFromParent(idFrameBottom, ".GemSockets");
        UpdateItemSocketsUI(idGemSockets, ACDID_INVALID, ptPlayer->idPlayerACD);
    }


    const UIID idItemInfo = UIIDCreateFromParent(idDetails, ".ScrollBox.ItemInfo");
    UIText & tItemInfo = UI_GetTextFromID(idItemInfo);
    tItemInfo.SetActive(TRUE);

    CRefString sDescription = ItemGetPrimaryDescription(*ptACDItem);
    if (idSelectedItem == ACDID_INVALID)
    {
        sDescription += "\n\n";
        sDescription += ItemGetFlavorText(*ptACDItem);
    }

    tItemInfo.SetText(sDescription.str(), FALSE);


    const UIID idArrowGlow = UIIDCreateFromParent(idFrameBottom, ".ArrowGlow");
    UI_GetWindowFromID(idArrowGlow).SetActive(idSelectedItem != ACDID_INVALID && IsFinishedAddingSockets());
}

}  // namespace Console
}  // namespace UIInventory
