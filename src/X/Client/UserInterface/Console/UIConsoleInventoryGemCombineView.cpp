#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryGemCombineView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleUtils.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"

// Declared here because their headers are not adopted yet (UI.cpp, ItemCrafting.cpp,
// UIConsoleInventoryViewUtilities.cpp, UIConsoleUtils.cpp).
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
int32 ItemCrafting_GetGemTier(const ActorCommonData & tACDItem);
namespace UIUtils { namespace Console { void UpdateRecipeRequirementsUI(const UIID & idRoot, GBID gbidRecipe, const Player & tPlayer); } }
namespace UIInventory { namespace Console {
enum DetailsViewMode { GEM_COMBINING_VIEW = 1, SOCKETING_VIEW = 2 };
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode);
void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags);
} }

namespace UIInventory {
namespace Console {









void UIInventoryGemCombineView::Reset()
{
    m_eState = START_STATE;
}





void UIInventoryGemCombineView::StartCombining()
{
    m_eState = FINISHED_STATE;
}






BOOL UIInventoryGemCombineView::IsFinishedCombining() const
{
    return m_eState == FINISHED_STATE;
}





void UIInventoryGemCombineView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    UIID idGemCombining;
    idGemCombining.SetFromParent(GetRootUIID(), "LayoutRoot.GemCombining");
    UI_GetWindowFromID(idGemCombining).SetActive(bShow);
}






void UIInventoryGemCombineView::OnUpdate()
{
    UpdateUI();
}





void UIInventoryGemCombineView::UpdateUI()
{

    if (GetParentScreen() == NULL) return;





    ACDID idSelectedItem = GetSelectedItem();
    if (idSelectedItem == ACDID_INVALID) return;

    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem == NULL) return;






    const UIID & idRoot = GetRootUIID();
    ShowDetailsMode(idRoot, GEM_COMBINING_VIEW);

    UIID idCombining = UIIDCreateFromParent(idRoot, ".LayoutRoot.GemCombining");



    UIID idJewelInfo; idJewelInfo.SetFromParent(idCombining, "JewelInfo");

    CRefString sText;

    {

        CRefString sItemName = ItemGetDisplayName(*ptACDItem, TRUE, 0, 0, TRUE);

        int32 nTierNumber = ItemCrafting_GetGemTier(*ptACDItem);
        sText = LocalizedSprintf(StringListFetch("ConsoleCrafting:GemInfoFormat").str(),
            UnicodeText::FormatNumber(nTierNumber, TRUE, FNS_NORMAL, 0).str(),
            sItemName.str(), NULL);

    }

    UI_GetTextFromID(idJewelInfo).SetText(sText.str(), FALSE);


    Player * ptPlayer = GetParentScreen()->GetPlayer();
    if (ptPlayer != NULL)
    {
        UIID idItemIcon; idItemIcon.SetFromParent(idCombining, "CreatedJewel");

        SetUpItemButton(idItemIcon, idSelectedItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);
    }



    UIID idComplete; idComplete.SetFromParent(idCombining, "Complete");
    UI_GetWindowFromID(idComplete).SetActive(m_eState == FINISHED_STATE);


    if (m_eState == START_STATE)
    {
        int32 nIndex = m_tItemsProvider.GetIndexOfItem(idSelectedItem);
        if (nIndex != -1)
        {
            GBID gbidRecipe = m_tItemsProvider.GetRecipes()[nIndex];
            UIUtils::Console::UpdateRecipeRequirementsUI(idCombining, gbidRecipe, *ptPlayer);
        }
    }
    else
    {

        UIUtils::Console::UpdateRecipeRequirementsUI(idCombining, -1, *ptPlayer);
    }



    UIID idArrow; idArrow.SetFromParent(idCombining, "Arrow");
    UI_GetWindowFromID(idArrow).SetActive(!IsFocused());

    idArrow.SetFromParent(idCombining, "ArrowGlow");
    UI_GetWindowFromID(idArrow).SetActive(IsFocused());
}

}  // namespace Console
}  // namespace UIInventory
