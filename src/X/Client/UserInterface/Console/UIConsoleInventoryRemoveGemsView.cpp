#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryRemoveGemsView.h"
#include "UIConsoleInventoryScreen.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"

// Declared here because their headers are not adopted yet (UI.cpp, Items.cpp,
// UIConsoleInventoryViewUtilities.cpp).
UIWindow & UI_GetWindowFromID(const UIID & id);
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
int32 ItemGetNumFilledSockets(const ActorCommonData & tACDItem);
namespace UIInventory { namespace Console {
enum DetailsViewMode { SOCKETING_VIEW = 2, GEM_REMOVAL_VIEW = 3 };
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode);
void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags);
void UpdateItemSocketsUI(const UIID & idSockets, const ACDID idACDItem, const ACDID idACDOwner);
} }
namespace UIInventory {
namespace Console {







void UIInventoryRemoveGemsView::Reset()
{
    m_eState = START_STATE;
    m_nItemNumFilledSockets = 0;

    const UIID idRemoveGem = UIIDCreateFromParent(GetRootUIID(), ".RemoveGem");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idRemoveGem);
    tStoryboard.Stop();
    tStoryboard.Reset();
}






void UIInventoryRemoveGemsView::StartRemoving()
{
    m_eState = REMOVING_STATE;

    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (ptACDItem != NULL)
    {
        m_nItemNumFilledSockets = ItemGetNumFilledSockets(*ptACDItem);
    }

    const UIID idRemoveGem = UIIDCreateFromParent(GetRootUIID(), ".RemoveGem");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idRemoveGem);
    tStoryboard.Stop();
    tStoryboard.Play(FALSE, 0.0f);
}







BOOL UIInventoryRemoveGemsView::IsFinishedRemoving() const
{
    return m_eState == FINISHED_STATE;
}





void UIInventoryRemoveGemsView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    UIID idGemRemoval;
    idGemRemoval.SetFromParent(GetRootUIID(), "LayoutRoot.GemRemoval");
    UI_GetWindowFromID(idGemRemoval).SetActive(bShow);
}






void UIInventoryRemoveGemsView::OnUpdate()
{

    if (m_eState == REMOVING_STATE)
    {
        const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".RemoveGem");
        BOOL bAnimIsFinished = UI_GetStoryboardFromID(idStoryboard).IsPlaying();

        BOOL bIsGemRemoved = FALSE;

        ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
        if (ptACDItem != NULL)
        {
            int32 nNumSocketsFilled = ItemGetNumFilledSockets(*ptACDItem);
            bIsGemRemoved = nNumSocketsFilled < m_nItemNumFilledSockets;
        }




        BOOL bIsFinished = bAnimIsFinished && bIsGemRemoved;
        if (bIsFinished)
        {

            m_eState = FINISHED_STATE;
        }
    }

    UpdateUI();
}




void UIInventoryRemoveGemsView::UpdateUI()
{

    if (GetParentScreen() != NULL)
    {



        const UIID & idRoot = GetRootUIID();

        ShowDetailsMode(idRoot, GEM_REMOVAL_VIEW);

        Player * ptPlayer = GetParentScreen()->GetPlayer();
        if (ptPlayer != NULL)
        {



            ACDID idSelectedItem = GetSelectedItem();


            UIID idRemoval;
            idRemoval.SetFromParent(GetRootUIID(), "LayoutRoot.GemRemoval");


            UIID idItemIcon;
            idItemIcon.SetFromParent(idRemoval, "ItemVisual");
            SetUpItemButton(idItemIcon, idSelectedItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);


            UIID idSockets;
            idSockets.SetFromParent(idRemoval, "GemSockets");
            UpdateItemSocketsUI(idSockets, idSelectedItem, ptPlayer->idPlayerACD);


            UIID idComplete;
            idComplete.SetFromParent(idRemoval, "GemsRemoved");
            UI_GetWindowFromID(idComplete).SetActive(m_eState == FINISHED_STATE);


            UIID idArrow;
            idArrow.SetFromParent(idRemoval, "ArrowGlow");
            UI_GetWindowFromID(idArrow).SetActive(m_eState == FINISHED_STATE);
        }
    }
}

}  // namespace Console
}  // namespace UIInventory
