// Original file: X/Client/UserInterface/Console/UIConsoleSalvage.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleSalvage.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryViewUtilities.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIItemButtonControl.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet (UI.cpp, CActorInventory.cpp, UIConsoleInventoryViewUtilities.cpp).
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); UIItemButton & UI_GetItemButtonFromID(const UIID & id);
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType);
namespace UIInventory { namespace Console { void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode); void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags); } }
namespace UIInventory {
namespace Console {
UIInventorySalvageView::UIInventorySalvageView(SingleItemProvider & tItemProvider, const UIID & idRoot)
    : UIInventoryView(idRoot)
    , m_eState(START_STATE)
    , m_tItemProvider(tItemProvider)
{
}

void UIInventorySalvageView::Reset()
{
    m_eState = START_STATE;
}





void UIInventorySalvageView::WaitForConfirmation()
{
    m_eState = READY_STATE;
}





void UIInventorySalvageView::SetSalvageResultsFromMessage(const SalvageResultsMessage & tMsg)
{
    m_tSalvageResults.SetFromMessage(tMsg);

    const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".SalvageItem");
    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Play(FALSE, 0.0f);

    m_eState = ANIMATING_STATE;
}





void UIInventorySalvageView::ClearSalvageResults()
{
    m_tSalvageResults.Clear();
}





void UIInventorySalvageView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    UIID idSalvage;
    idSalvage.SetFromParent(GetRootUIID(), "LayoutRoot.Salvage");
    UI_GetWindowFromID(idSalvage).SetActive(bShow);
}





void UIInventorySalvageView::OnUpdate()
{
    if (m_eState == ANIMATING_STATE)
    {
        const UIID idStoryboard = UIIDCreateFromParent(GetRootUIID(), ".SalvageItem");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        if (!tStoryboard.IsPlaying())
        {
            m_eState = FINISHED_STATE;
        }
    }

    UpdateUI();
}




void UIInventorySalvageView::OnSelectedItemChanged()
{
    UpdateUI();
}





void UIInventorySalvageView::UpdateUI()
{
    const UIID & idRoot = GetRootUIID();


    ShowDetailsMode(idRoot, SALVAGE_VIEW);


    Player * ptPlayer = GetParentScreen() != NULL ? GetParentScreen()->GetPlayer() : NULL;


    const ACDID idItem = m_tItemProvider.GetItems().size() != 0

        ? m_tItemProvider.GetItems()[0] : ACDID_INVALID;


    UIID idSalvage;
    idSalvage.SetFromParent(idRoot, "LayoutRoot.Salvage");


    UIID idItemName;
    idItemName.SetFromParent(idSalvage, "ItemName");

    CRefString sItemName;
    if (idItem != ACDID_INVALID && m_eState != FINISHED_STATE)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(idItem);
        if (ptACDItem != NULL)
        {
            sItemName = ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, TRUE);
        }
    }

    UI_GetTextFromID(idItemName).SetText(sItemName, FALSE);


    UIID idInstructions;
    idInstructions.SetFromParent(idSalvage, "SalvageInstructions");

    CRefString sInstructions;
    switch (m_eState)
    {
    case START_STATE:
        sInstructions = StringListFetch("ConsoleArtisan:SalvageStep1");
        break;
    case READY_STATE:
        sInstructions = StringListFetch("ConsoleArtisan:SalvageStep2");
        break;
    }
    UI_GetTextFromID(idInstructions).SetText(sInstructions, FALSE);

    UIID idItemControl;
    idItemControl.SetFromParent(idSalvage, "Item");

    if (m_eState != FINISHED_STATE)
    {
        UIID idItemVisual;
        idItemVisual.SetFromParent(idItemControl, "ItemVisual");


        if (ptPlayer != NULL)
        {
            SetUpItemButton(idItemVisual, idItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);
        }
    }


    UIID idItemDimmer;
    idItemDimmer.SetFromParent(idItemControl, "ItemDimmer");
    UI_GetWindowFromID(idItemDimmer).SetActive(m_eState == START_STATE);

    UpdateSalvageResults();
}




void UIInventorySalvageView::UpdateSalvageResults()
{

    UIID idSalvageComplete;
    idSalvageComplete.SetFromParent(GetRootUIID(), "LayoutRoot.Salvage.SalvageComplete");

    UI_GetWindowFromID(idSalvageComplete).SetActive(m_eState == ANIMATING_STATE || m_eState == FINISHED_STATE);

    if (m_eState == ANIMATING_STATE || m_eState == FINISHED_STATE)
    {




        UIID idMaterials;
        idMaterials.SetFromParent(idSalvageComplete, "SalvageMaterials");

        UIWindow & tMaterials = UI_GetWindowFromID(idMaterials);
        for (size_t i = 0; i < tMaterials.GetChildList().size(); ++i)
        {

            UI_GetWindowFromID(tMaterials.GetChildList()[i].id).SetActive(i < m_tSalvageResults.Count());


            UIID idMaterialIcon;
            idMaterialIcon.SetFromParent(tMaterials.GetChildList()[i].id, "MaterialIcon");

            UIID idMaterialCount;
            idMaterialCount.SetFromParent(tMaterials.GetChildList()[i].id, "MaterialCount");

            UIItemButton & tMaterialIcon = UI_GetItemButtonFromID(idMaterialIcon);
            if (i < m_tSalvageResults.Count())
            {
                ImageTextureFrame tFrame;
                ACDInventoryGetItemDrawInfoWithGBID(m_tSalvageResults[i].gbidItem, tFrame, 0);
                tMaterialIcon.SetGBID(m_tSalvageResults[i].gbidItem);

                ScopedItemDefinitionLockR ptItemDef(m_tSalvageResults[i].gbidItem);
                ItemQualityLevel eIQL = IQL_NORMAL;
                if (ptItemDef != NULL && ptItemDef->eDisplayedQualityLevel != IQL_INVALID)
                {
                    eIQL = ptItemDef->eDisplayedQualityLevel;
                }
                tMaterialIcon.SetQualityLevel(eIQL);
                tMaterialIcon.SetAllTextureFrames(tFrame);


                UI_GetTextFromID(idMaterialCount).SetText(UnicodeText::FormatNumber(m_tSalvageResults[i].nCount, TRUE, FNS_NORMAL, 0).str(), FALSE);
            }
            else
            {
                const ImageTextureFrame tEmptyFrame;
                tMaterialIcon.SetGBID(GBID_NONE);
                tMaterialIcon.SetQualityLevel(IQL_INVALID);
                tMaterialIcon.SetAllTextureFrames(tEmptyFrame);

                UI_GetTextFromID(idMaterialCount).SetText("", FALSE);
            }
        }
    }
}



SalvageResults::SalvageResults()
{
    Clear();
}





void SalvageResults::SetFromMessage(const SalvageResultsMessage & tMsg)
{
    Clear();


    for (int32 i = 0; i < tMsg.nMaterialsResults; ++i)
    {
        const GBID gbidMaterial = tMsg.gbidMaterials[i];
        const int32 nCount = tMsg.nMaterialsCounts[i];




        size_t nIndex;
        for (nIndex = 0; nIndex < MAX_DISPLAYED_SALVAGE_RESULTS && m_arSalvagedItems[nIndex].gbidItem != gbidMaterial; ++nIndex) {}







        if (nIndex == MAX_DISPLAYED_SALVAGE_RESULTS)
        {

            m_arSalvagedItems[m_nNumItems].gbidItem = gbidMaterial;
            m_arSalvagedItems[m_nNumItems].nCount = nCount;
            ++m_nNumItems;
        }
        else
        {

            m_arSalvagedItems[nIndex].nCount += nCount;
        }
    }
}




void SalvageResults::Clear()
{

    m_nNumItems = 0;
    for (int32 i = 0; i < MAX_DISPLAYED_SALVAGE_RESULTS; ++i)
    {
        m_arSalvagedItems[i].gbidItem = GBID_NONE; m_arSalvagedItems[i].nCount = 0;
    }
}





size_t SalvageResults::Count() const
{

    return m_nNumItems;
}




const SalvagedItem SalvageResults::operator[](size_t nIndex) const
{

    return m_arSalvagedItems[nIndex];
}

}  // namespace Console
}  // namespace UIInventory
