// Original file: X/Client/UserInterface/Console/UIConsoleInventoryDyeView.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryDyeView.h"
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




UIInventoryDyeView::UIInventoryDyeView(const UIID & idRoot)
    : UIInventoryView(idRoot)
    , m_idDye(ACDID_INVALID)
    , m_eExpectedDyeType(0)
    , m_eState(DEFAULT_STATE)
{
}





void UIInventoryDyeView::SetDye(const ACDID idDye)
{
    if (m_idDye != idDye)
    {
        m_idDye = idDye;

        ActorCommonData * ptACDDye = ACDTryToGet(m_idDye);
        if (ptACDDye != NULL)
        {
            m_eExpectedDyeType = ptACDDye->GetDyeType();
        }
        else
        {
            m_eExpectedDyeType = 0;
        }




        RequestUpdate(1);
    }
}


void UIInventoryDyeView::Reset()
{
    m_eState = DEFAULT_STATE;

    const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".DyeItem");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Reset();
}






void UIInventoryDyeView::StartDyeing()
{
    m_eState = DYEING_STATE;

    const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".DyeItem");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Play(FALSE, 0.0f);
}






BOOL UIInventoryDyeView::IsFinishedDyeing() const
{

    return m_eState == FINISHED_STATE;
}




void UIInventoryDyeView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    const UIID idItemSocket = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemSocket");
    UI_GetWindowFromID(idItemSocket).SetActive(bShow);
}








void UIInventoryDyeView::OnUpdate()
{
    if (m_eState == DYEING_STATE)
    {
        const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".DyeItem");


        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        BOOL bAnimIsFinished = tStoryboard.IsPlaying();

        BOOL bIsDyed = FALSE;
        ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
        if (ptACDItem != NULL)
        {
            DyeType eDyeType = ptACDItem->GetDyeType();
            bIsDyed = eDyeType == m_eExpectedDyeType;
        }






        BOOL bIsFinished = bAnimIsFinished && bIsDyed;

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



void UIInventoryDyeView::OnSelectedItemChanged()
{
    UpdateUI();
}





void UIInventoryDyeView::UpdateUI()
{
    UIInventoryScreen * ptParentScreen = GetParentScreen(); if (ptParentScreen == NULL)
        return;
    const UIID & idRoot = GetRootUIID();






    ShowDetailsMode(idRoot, SOCKETING_VIEW);

    const UIID idDetails = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemSocket.SocketDetails");

    ActorCommonData * ptACDItem = ACDTryToGet(GetDye());
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

        const ACDID idJewel = IsFinishedDyeing() ? ACDID_INVALID : m_idDye;

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
    UI_GetWindowFromID(idArrowGlow).SetActive(idSelectedItem != ACDID_INVALID && IsFinishedDyeing());
}

}  // namespace Console
}  // namespace UIInventory
