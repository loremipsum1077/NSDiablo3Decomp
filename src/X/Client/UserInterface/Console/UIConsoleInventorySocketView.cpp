// Original file: X/Client/UserInterface/Console/UIConsoleInventorySocketView.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventorySocketView.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryViewUtilities.h"
#include "UIConsoleUtils.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet (UI.cpp, UIConsoleInventoryViewUtilities.cpp, UIConsoleUtils.cpp, Interface/UIInventory.cpp:4434).
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); namespace UIUtils { namespace Console { BOOL CanScrollContentInScrollBox(const UIID & idScrollBox); } }
namespace UIInventory { CRefString GetRequirementsTooltip(const ActorCommonData * ptACDPlayer, const ActorCommonData & tACDItem, GBID gbidRecipe); namespace Console {
void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode); void SetUpItemButton(const UIID & idButton, const ACDID idACDItem, const ACDID idACDOwner, const ACDID idACDOther, const int32 nFlags);
void UpdateItemSocketsUI(const UIID & idSockets, const ACDID idACDItem, const ACDID idACDOwner); CRefString GetJewelEffectsString(const ActorCommonData & tACDJewel, const ACDID idACDItem);
CRefString GetItemDescription(const ACDID idACDOwner, const ACDID idACDItem, const BOOL bShowStatRanges); } }
namespace UIInventory {
namespace Console {



UIInventorySocketView::UIInventorySocketView(const UIID & idRoot)

    : UIInventoryView(idRoot)
    , m_idJewel(ACDID_INVALID)
    , m_nItemNumFilledSockets(0)
    , m_bJewelIsUnidentified(FALSE)
{
    m_idStoryboard = UIIDCreateFromParent(idRoot, ".SocketGem");
}




void UIInventorySocketView::SetJewel(const ACDID idJewel)
{

    if (IsFinishedSocketing() || m_idJewel != idJewel)
    {
        SetFilterJewel(idJewel);
        m_idJewel = idJewel;
        m_sJewelStats = GetJewelStatsString(idJewel, GetSelectedItem(), GetParentScreen() != NULL ? GetParentScreen()->GetPlayer() : NULL);
        ActorCommonData * ptACDJewel = ACDTryToGet(GetJewel());
        if (ptACDJewel != NULL)
        {
            m_bJewelIsUnidentified = ItemIsUnidentified(*ptACDJewel);
        }

        RequestUpdate(1);

        const UIID idScrollBox = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemSocket.SocketDetails.ScrollBox");

        UIUtils::Console::ResetScrollBox(idScrollBox);
    }
}




void UIInventorySocketView::Reset()
{

    m_nItemNumFilledSockets = 0;

    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(m_idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Reset();
}




void UIInventorySocketView::ResetToSocketAgain(const ACDID idJewel)
{

    SetJewel(idJewel);
    Reset();


    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (ptACDItem != NULL)
    {
        m_nItemNumFilledSockets = ItemGetNumFilledSockets(*ptACDItem);
    }
}



void UIInventorySocketView::SocketItem()
{

    UIStoryboard & tStoryboard = UI_GetStoryboardFromID(m_idStoryboard);
    tStoryboard.Stop();
    tStoryboard.Play(FALSE, 0.0f);
}




BOOL UIInventorySocketView::IsFinishedSocketing() const
{

    const BOOL bIsPlaying = UI_GetStoryboardFromID(m_idStoryboard).IsPlaying();

    BOOL bHasNewSocket = FALSE;
    ActorCommonData * ptACDItem = ACDTryToGet(GetSelectedItem());
    if (ptACDItem != NULL)
    {

        bHasNewSocket = ItemGetNumFilledSockets(*ptACDItem)
            > m_nItemNumFilledSockets;
    }





    return !bIsPlaying && bHasNewSocket;
}




void UIInventorySocketView::OnShow(const BOOL bShow)
{
    UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);

    const UIID idItemSocket = UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemSocket");
    UI_GetWindowFromID(idItemSocket).SetActive(bShow);
}





void UIInventorySocketView::OnUpdate()
{
    UpdateUI();
}





void UIInventorySocketView::OnSelectedItemChanged()
{

    const ACDID idSelectedItem = GetSelectedItem();
    ActorCommonData * ptACDItem = ACDTryToGet(idSelectedItem);
    if (ptACDItem != NULL)
    {


        m_nItemNumFilledSockets = ItemGetNumFilledSockets(*ptACDItem);
    }


    m_sJewelStats = GetJewelStatsString(m_idJewel, idSelectedItem, GetParentScreen() != NULL ? GetParentScreen()->GetPlayer() : NULL);
}




CRefString sGetJewelName(const ActorCommonData * ptJewelACD)
{

    CRefString sName;
    if (ptJewelACD != NULL)
    {
        CRefString sDisplayName = ItemGetDisplayName(*ptJewelACD, TRUE, FALSE, FALSE, TRUE);

        if (ItemIsGem(*ptJewelACD))
        {
            const int32 nGemTier = ItemCrafting_GetGemTier(*ptJewelACD);
            sName = LocalizedSprintf(StringListFetch("ConsoleCrafting:GemInfoFormat").str(),
                UnicodeText::FormatNumber(nGemTier, TRUE, FNS_NORMAL, 0).str(),
                sDisplayName.str(), NULL);
        }

        else if (ItemIsUpgradeableJewel(*ptJewelACD))
        {
            const int32 nJewelRank = ptJewelACD->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
            sName = LocalizedSprintf(StringListFetch("ConsoleInventory:UpgradeableGemNameFormat").str(),
                sDisplayName.str(),
                UnicodeText::FormatNumber(nJewelRank, TRUE, FNS_NORMAL, 0).str(), NULL);
        }
        else
        {

            sName = sDisplayName;
        }
    }


    return sName;
}



UIID UIInventorySocketView::GetScrollBoxUIID() const
{
    return UIIDCreateFromParent(GetRootUIID(), ".LayoutRoot.ItemSocket.SocketDetails.ScrollBox");
}







BOOL UIInventorySocketView::CanScrollWithTriggers() const
{
    const UIID idScrollBox = GetScrollBoxUIID();
    BOOL bCanScroll = FALSE;
    if (UI_TestID(idScrollBox))
    {
        bCanScroll = UIUtils::Console::CanScrollContentInScrollBox(idScrollBox);
    }
    return bCanScroll;
}






CRefString UIInventorySocketView::GetJewelStatsString(const ACDID idJewelACD, const ACDID idItemACD, const Player * ptPlayer)
{

    CRefString sStats;

    if (idJewelACD == ACDID_INVALID || ptPlayer == NULL) return sStats;

    const ActorCommonData & tACDJewel = ACDGet(idJewelACD);
    if (ItemIsUpgradeableJewel(tACDJewel))
    {

        if (idItemACD != ACDID_INVALID)
        {
            sStats = "";
        }
        else if (ItemIsUnidentified(tACDJewel))
        {
            sStats = StringListFetch("ConsoleInventory:UnidentifiedInstructions");
        }
        else
        {
            sStats = GetItemDescription(ptPlayer->idPlayerACD, idJewelACD, FALSE);
        }
    }
    else
    {
        sStats = GetJewelEffectsString(tACDJewel, idItemACD);


        if (idItemACD == ACDID_INVALID)
        {
            const ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            CRefString sRequirements = GetRequirementsTooltip(ptACDPlayer, tACDJewel, GBID_NONE);
            sStats += CRefString("\n\n") + sRequirements;
        }
    }



    return sStats;
}



void UIInventorySocketView::UpdateUI()
{

    if (GetParentScreen() == NULL)
        return;



    const BOOL bIsSocketing = UI_GetStoryboardFromID(m_idStoryboard).IsPlaying();
    const BOOL bIsFinishedSocketing = IsFinishedSocketing();

    const UIID & idRoot = GetRootUIID();
    const UIID idDetails = UIIDCreateFromParent(idRoot, ".LayoutRoot.ItemSocket.SocketDetails");

    ActorCommonData * ptACDJewel = ACDTryToGet(GetJewel());
    if (ptACDJewel == NULL)
        return;



    if (!bIsSocketing)
    {
        ShowDetailsMode(idRoot, SOCKETING_VIEW);


        CRefString sJewelName = sGetJewelName(ptACDJewel);
        const UIID idJewelInfo = UIIDCreateFromParent(idDetails, ".JewelInfo");
        UI_GetTextFromID(idJewelInfo).SetText(sJewelName.str(), FALSE);
    }

    const ACDID idSelectedItem = GetSelectedItem();

    const UIID idFrameBottom = UIIDCreateFromParent(idDetails, ".FrameBottom");
    UI_GetWindowFromID(idFrameBottom).SetActive(idSelectedItem != ACDID_INVALID);

    Player * ptPlayer = GetParentScreen()->GetPlayer();
    if (ptPlayer == NULL)
        return;

    if (!bIsSocketing)
    {

        const ACDID idJewel = bIsFinishedSocketing ? ACDID_INVALID : m_idJewel;

        const UIID idSocketJewel = UIIDCreateFromParent(idDetails, ".SocketJewel");
        SetUpItemButton(idSocketJewel, idJewel, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 0);


        const UIID idItemVisual = UIIDCreateFromParent(idFrameBottom, ".ItemVisual");
        SetUpItemButton(idItemVisual, idSelectedItem, ptPlayer->idPlayerACD, ptPlayer->idPlayerACD, 1);
    }


    const UIID idGemSockets = UIIDCreateFromParent(idFrameBottom, ".GemSockets");
    UpdateItemSocketsUI(idGemSockets, idSelectedItem, ptPlayer->idPlayerACD);

    if (!bIsSocketing)
    {

        const UIID idItemInfo = UIIDCreateFromParent(idDetails, ".ScrollBox.ItemInfo");
        UIText & tItemInfo = UI_GetTextFromID(idItemInfo);
        tItemInfo.SetActive(TRUE);

        const BOOL bIsUnidentified = ItemIsUnidentified(*ptACDJewel);
        if (bIsUnidentified != m_bJewelIsUnidentified)
        {
            m_sJewelStats = GetJewelStatsString(m_idJewel, GetSelectedItem(), GetParentScreen() != NULL ? GetParentScreen()->GetPlayer() : NULL);
            m_bJewelIsUnidentified = bIsUnidentified;
        }

        tItemInfo.SetText(m_sJewelStats.str(), FALSE);
        tItemInfo.SetJustification(ItemIsShard(*ptACDJewel) ? UIJH_LEFT : UIJH_CENTER, UIJV_TOP);








        const UIID idArrowGlow = UIIDCreateFromParent(idFrameBottom, ".ArrowGlow");
        UI_GetWindowFromID(idArrowGlow).SetActive(bIsFinishedSocketing && idSelectedItem != ACDID_INVALID);
    }
}

}  // namespace Console
}  // namespace UIInventory
