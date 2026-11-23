// Original file: X/Client/UserInterface/Interface/UIStash.cpp
// InitializeForGame and DisposeForGame were removed by the linker (DW_AT_low_pc 0); their bodies
// here are reconstructed and exist to keep sgptStashVariables a run-time value. Also removed and
// not written: OpenStash (399), Close (437), OnSideScreenShowHide (472), sDoPurchase (760).
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIRadioButtonControl.h"
#include "../Controls/UIRadioButtonGroupControl.h"
#include "../Controls/UIScrollbarControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIWrapPanelControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "UIGame.h"
#include "../../CPlayer.h"
#include "../../CPlayerInput.h"
#include "../../ImageCollection.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Player.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"
// Declared here because their headers are not adopted yet.
struct StashIconStateMessage { RequiredMessageHeader tHeader; uint8 bStashIcons[12]; };  // size 0x14; X/Common/Networking/GameMessage.h

void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // CGameMessage.cpp
void ACDInventoryGetSlotPageSize(const InventorySlotType eSlot, IVector2D & tSize); int32 ACDInventoryGetNumUnlockedStashTabs(ActorCommonData & tACDPlayer); int32 ACDInventoryGetMaxStashSlots(ActorCommonData & tACDPlayer); int32 ACDInventoryStashTabsBuyableWithGold(ActorCommonData & tACDPlayer); int32 ACDInventoryGetStashUpgradeCost(ActorCommonData & tACDPlayer);  // ActorInventory.cpp
CursorMgr & PlayerGetCursorMgr();  // CPlayerInput.cpp
void UISideScreenSetVisible(const UIID & id, const BOOL fVisible);  // UISideScreen.cpp
UIWrapPanel & UI_GetWrapPanelFromID(const UIID & id); UIRadioButton & UI_GetRadioButtonFromID(const UIID & id);  // UI.cpp
typedef int32 UIInventoryControl;
extern const float UIDC_DIALOG_Z_START; extern const float UIDC_DIALOG_Z_INCREMENT;  // X/Client/UserInterface/UIDraw.cpp
namespace UIInventory { void OnClick(const UIID & uId); void OnRightClick(const UIID & uId); void SetStashPageIndex(const int32 nPage); void SlotDrawItems(ActorCommonData & tACD, const PlayerClass eClassToDisplay, const BOOL fIsMale, const InventorySlotType eSlot, float & flZ, UIInventoryControl eInventoryControl); void TrackItemDrop(const ActorCommonData & tACDItem, InventorySlotType & eSlot, IVector2D & tSlot); void HighlightSlot(const InventorySlotType eSlot, const IVector2D & tSlot, const ActorCommonData & tACD, const ActorCommonData & tACDItem, float & flZ); }  // UIInventory.cpp
namespace UIStash { void InitializeForGame(); void DisposeForGame(); void ToggleOnClick(const UIID & id); void OnRightClick(const UIID & id); void OnTab(const UIID &); void OnPage(const UIID &); void OnIconPickerOpen(const UIID &); void OnIconPickerClose(const UIID &); void IconPickerAccept(const UIID &); void OnTabRightClick(const UIID & id); void Update(); void Draw(const uint8, float & flZSlice); void Purchase(const UIID &); }  // UIStash.h
static const int32 MAX_VISIBLE_TABS = 5;

// The DWARF records `static const int32 MAX_PAGES` on this line; no surviving code uses it, so its value is not observable and it is left out.

struct StashVariables {  // size 0x24
    BOOL bCanBuyMoreTabs;  // 0x00
    BOOL bEditingIcons;  // 0x04
    BOOL bStashTabsAvailableInStore;  // 0x08
    int32 nLastStashPage;  // 0x0c
    int32 nLastSlotCount;  // 0x10
    int32 nLastSelectedTab[3];  // 0x14; literal 3 from the struct size (probably MAX_PAGES in the original)
    int32 nCurrentEditedTabIcon;  // 0x20
};
static StashVariables * sgptStashVariables = NULL;














void UIStash::InitializeForGame()
{
    sgptStashVariables = new StashVariables;
}














void UIStash::DisposeForGame()
{
    delete sgptStashVariables; sgptStashVariables = NULL;
}



void UIStash::ToggleOnClick(const UIID & id)

{

    if (!UI_TestID(UIDLG_STASH_PAGE) ||
        !UI_TestID(id))
        return;

    if (UIGame::MenuIsActive())
        return;





    BOOL bStashState = UI_GetWindowFromID(UIDLG_STASH_PAGE).IsActive();
    UISideScreenSetVisible(UIDLG_STASH_PAGE, !bStashState);
}



void UIStash::OnRightClick(const UIID & id)

{
    if (!UI_TestID(id))
        return;

    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return;




    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (tCursorMgr.GetItemToMove() ||
        tCursorMgr.GetStackToSplit())
    {
        UIInventory::OnClick(id);
        return;
    }

    if (tCursorMgr.GetCursorState() != CURSORSTATE_NORMAL)
    {
        return;
    }

    UIInventory::OnRightClick(id);
}




void UIStash::OnTab(const UIID &)

{
    UIRadioButtonGroup & tRadioButtonGroup = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS);
    int32 nTab = tRadioButtonGroup.GetSelectedValue();

    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer != ACDID_INVALID)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer && sgptStashVariables->nLastStashPage * MAX_VISIBLE_TABS + nTab >= ACDInventoryGetNumUnlockedStashTabs(*ptACDPlayer))
        {
            sgptStashVariables->nLastStashPage = 0;
        }
    }

    sgptStashVariables->nLastSelectedTab[sgptStashVariables->nLastStashPage] = nTab;

    UIInventory::SetStashPageIndex(sgptStashVariables->nLastStashPage * MAX_VISIBLE_TABS + nTab);
}





void UIStash::OnPage(const UIID &)

{
    UIRadioButtonGroup & tRadioButtonGroup = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_PAGES);
    int32 nPage = tRadioButtonGroup.GetSelectedValue();
    if (nPage == -1)
        return;
    sgptStashVariables->nLastStashPage = nPage;

    IVector2D tSlotPageSize;
    ACDInventoryGetSlotPageSize(SLOT_PLAYER_SHARED_STASH, tSlotPageSize);

    int32 nCurrentTab = sgptStashVariables->nLastSelectedTab[nPage];

    UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS).SetSelection(nCurrentTab, FALSE);

    UIInventory::SetStashPageIndex(nPage * MAX_VISIBLE_TABS + nCurrentTab);

}




static void AdjustScrollbar(UIWindow & tWindowRegion, UIWindow & tWindowWrapPanel, UIScrollbar & tScrollbar)




{
    Vector2D vecRegionTL, vecRegionDim;
    tWindowRegion.GetRelativePosition(vecRegionTL, vecRegionDim);
    Vector2D vecStackTL, vecStackDim;
    tWindowWrapPanel.GetRelativePosition(vecStackTL, vecStackDim);

    float flExtents = vecStackDim.y - vecRegionDim.y;
    if (flExtents < EPSILON)
    {
        tScrollbar.SetExtentsAndPosition(0, 0, 0, TRUE);
    }
    else
    {
        tScrollbar.SetExtentsAndPosition(0, (int32)flExtents, tScrollbar.GetPosition(), TRUE);
        tScrollbar.SetIncrements(10, (int32)vecRegionDim.y);
    }
}



void UIStash::OnIconPickerOpen(const UIID &)

{
    if (!UI_GetWindowFromID(UIWND_STASH_ICON_PICKER).IsActive())
        return;



    UIRadioButtonGroup & tRadioButtonGroupIcons = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_STASH_ICON_LIST);
    UIWrapPanel & tIconList = UI_GetWrapPanelFromID(UIWRAPPANEL_STASH_ICON_LIST);

    UIScrollbar & tScrollbar = UI_GetScrollbarFromID(UISCROLLBAR_STASH_ICON_LIST);
    tScrollbar.SetOwner(tIconList.GetID());
    tScrollbar.SetOnChangeFunction(UIScrollbar::OnScrollChangeDefault);

    tIconList.DeleteAllChildren();
    const UIControlSourceKey tTemplateSrcKey(tIconList.GetUISrcKey().snoUI, "stash_dialog_mainPage.IconPicker.ItemTemplate");

    const int32 ICON_INFINITE_LOOP_PREVENTER = 500;
    for (int32 i = 0; i < ICON_INFINITE_LOOP_PREVENTER; i++)
    {
        CRefString sFrame = CRefString().Format("UI:Tab_Stash%02d_Selected", i + 1);
        ImageTextureFrame tImageTextureFrame;
        if (!ImageCollectionGetImageInfo(sFrame, tImageTextureFrame))
        {
            break;
        }

        UIWindow * ptWindow = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey, DynamicControlCreationParams(CRefString().Format("icon %d", i), UIWRAPPANEL_STASH_ICON_LIST), NULL);
        if (ptWindow)
        {
            UIID idWndIcon = UIIDCreateFromParent(ptWindow->GetID(), ".icon");
            CRefString sFrameScoped = CRefString().Format("UI:Tab_Stash%02d_Selected", i + 1);
            UI_GetWindowFromID(idWndIcon).SetTextureFrame(sFrameScoped);

            UIID idRadioButton = UIIDCreateFromParent(ptWindow->GetID(), ".button");
            UI_GetRadioButtonFromID(idRadioButton).SetValue(i);

            tRadioButtonGroupIcons.AddChild(idRadioButton);
        }
    }


    const UIID & idTab = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS).GetSelection();
    Vector2D vecTabTL, vecTabDim;
    UI_GetWindowFromID(idTab).GetRelativePosition(vecTabTL, vecTabDim);
    UIWindow & tWndHighlight = UI_GetWindowFromID(UIWND_STASH_ICON_LIST_HIGHLIGHT);
    tWndHighlight.SetRelativePosition(vecTabTL, vecTabDim);
    tWndHighlight.SetActive(TRUE);

    UIWindow & tWndRegion = UI_GetWindowFromID(UIWND_STASH_ICON_LIST_REGION);
    AdjustScrollbar(tWndRegion, tIconList, tScrollbar);
}




void UIStash::OnIconPickerClose(const UIID &)

{
    UIWrapPanel & tIconList = UI_GetWrapPanelFromID(UIWRAPPANEL_STASH_ICON_LIST);
    tIconList.DeleteAllChildren();

    UIWindow & tWndHighlight = UI_GetWindowFromID(UIWND_STASH_ICON_LIST_HIGHLIGHT);
    tWndHighlight.SetActive(FALSE);

    if (sgptStashVariables->nCurrentEditedTabIcon != -1)
    {
        Player * ptPlayer = LocalPlayerGet();
        if (ptPlayer)
        {
            StashIconStateMessage tMessage;
            SigmaMemoryCopy(tMessage.bStashIcons, ptPlayer->bStashIcons, sizeof(tMessage.bStashIcons));
            MessageSendToServer(CMSG_STASH_ICON_STATE, &tMessage, sizeof(tMessage), NULL);
        }
    }

    sgptStashVariables->bEditingIcons = FALSE;
    sgptStashVariables->nCurrentEditedTabIcon = -1;
}




void UIStash::IconPickerAccept(const UIID &)

{
    sgptStashVariables->bEditingIcons = FALSE;

    Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
    {
        return;
    }

    int32 nCurrentTab = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS).GetSelectedValue() + UI_GetRadioButtonGroupFromID(UIRADIO_STASH_PAGES).GetSelectedValue() * MAX_VISIBLE_TABS;
    if (nCurrentTab < 0)
    {
        return;
    }

    int32 nSel = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_STASH_ICON_LIST).GetSelectedValue();
    ptPlayer->bStashIcons[nCurrentTab] = nSel;

    StashIconStateMessage tMessage;
    SigmaMemoryCopy(tMessage.bStashIcons, ptPlayer->bStashIcons, sizeof(tMessage.bStashIcons));
    MessageSendToServer(CMSG_STASH_ICON_STATE, &tMessage, sizeof(tMessage), NULL);

    UI_GetWindowFromID(UIWND_STASH_ICON_PICKER).SetActive(FALSE);
}



void UIStash::OnTabRightClick(const UIID & id)

{
    Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
    {
        return;
    }

    sgptStashVariables->bEditingIcons = TRUE;

    UIRadioButtonGroup & tRadioGroup = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS);
    tRadioGroup.SetSelection(id);
    OnTab(id);

    UI_GetWindowFromID(UIWND_STASH_ICON_PICKER).SetActive(TRUE);

    int32 nCurrentTab = tRadioGroup.GetSelectedValue() + UI_GetRadioButtonGroupFromID(UIRADIO_STASH_PAGES).GetSelectedValue() * MAX_VISIBLE_TABS;
    if (nCurrentTab < 0)
    {
        return;
    }
    int32 nSelectedVal = ptPlayer->bStashIcons[nCurrentTab];

    UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_STASH_ICON_LIST).SetSelection(nSelectedVal, FALSE);
}










































































































































void UIStash::Update()

{
    if (!UI_TestID(UIDLG_STASH_PAGE))
        return;

    if (!UI_GetWindowFromID(UIDLG_STASH_PAGE).IsActive())
        return;



    Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
        return;


    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return;




    if (sgptStashVariables->bEditingIcons)
    {
        UIRadioButtonGroup & tRadioGroup = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS);

        int32 nCurrentTab = tRadioGroup.GetSelectedValue() + UI_GetRadioButtonGroupFromID(UIRADIO_STASH_PAGES).GetSelectedValue() * MAX_VISIBLE_TABS;
        if (nCurrentTab >= 0)
        {
            int32 nSel = UI_GetRadioButtonGroupFromID(UIRADIOBUTTONGROUP_STASH_ICON_LIST).GetSelectedValue();
            if (nSel != sgptStashVariables->nCurrentEditedTabIcon)
            {
                sgptStashVariables->nCurrentEditedTabIcon = nSel;
                ptPlayer->bStashIcons[nCurrentTab] = nSel;
            }
        }
    }

    int32 nSlotCount = ptACDPlayer->AttributesGetInt(ATTRIBUTE_SHARED_STASH_SLOTS);

    IVector2D tSlotPageSize;
    ACDInventoryGetSlotPageSize(SLOT_PLAYER_SHARED_STASH, tSlotPageSize);
    int32 nSlotsPerPage = tSlotPageSize.x * tSlotPageSize.y;

    int32 nNumTabs = ACDInventoryGetNumUnlockedStashTabs(*ptACDPlayer);
    if (nNumTabs * nSlotsPerPage > nSlotCount)
    {
        nNumTabs = nSlotCount / nSlotsPerPage;
    }

    UIRadioButtonGroup & tRadioButtonGroup = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_TABS);
    UIRadioButtonGroup & tStashPageRadioButtonGroup = UI_GetRadioButtonGroupFromID(UIRADIO_STASH_PAGES);



    if (sgptStashVariables->nLastSlotCount != 0 && sgptStashVariables->nLastSlotCount < nSlotCount)
    {
        int32 nPage = Math::Max(nSlotCount - 1, 0) / nSlotsPerPage;

        sgptStashVariables->nLastStashPage = nPage >= MAX_VISIBLE_TABS;








        tRadioButtonGroup.SetSelection(nPage % MAX_VISIBLE_TABS, FALSE);
        tStashPageRadioButtonGroup.SetSelection(sgptStashVariables->nLastStashPage, FALSE);
        UIInventory::SetStashPageIndex(nPage);
    }

    sgptStashVariables->nLastSlotCount = nSlotCount;

    int32 nNumTotalPages = 0;
    if (nNumTabs > MAX_VISIBLE_TABS && !sgptStashVariables->bEditingIcons)
    {
        nNumTotalPages = (nNumTabs + MAX_VISIBLE_TABS - 1) / MAX_VISIBLE_TABS;
    }

    for (uint32 n = 0; n < tStashPageRadioButtonGroup.GetChildList().size(); n++)
    {
        UIRadioButton * pButton = tStashPageRadioButtonGroup.GetButtonByValue(n);
        if (pButton)
        {
            pButton->SetActive(n < nNumTotalPages);
        }
    }

    int32 nPage = tStashPageRadioButtonGroup.GetSelectedValue();

    for (int32 i = 0; i < 12; i++)
    {
        int32 nTab = i % MAX_VISIBLE_TABS;
        UIRadioButton * ptButton = tRadioButtonGroup.GetButtonByValue(nTab);
        if (ptButton)
        {
            ptButton->SetActive(nPage * MAX_VISIBLE_TABS + nTab < nNumTabs);

            BOOL fEnabled = nSlotCount > nTab * nSlotsPerPage;

            if (UI_GetWindowFromID(UIWND_STASH_ICON_PICKER).IsActive())
                fEnabled &= (nTab == tRadioButtonGroup.GetSelectedValue());



            ptButton->SetEnabled(fEnabled);

            ImageTextureFrame tTexFrameNormal;
            ImageTextureFrame tTexFrameMouseover;
            ImageTextureFrame tTexFramePushed;

            CRefString sTabBase = CRefString().Format("Tab_Stash%02d", ptPlayer->bStashIcons[nPage * MAX_VISIBLE_TABS + nTab] + 1);

            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, CRefString().Format("%s_Unselected", sTabBase.str()), tTexFrameNormal);
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, CRefString().Format("%s_Mouseover", sTabBase.str()), tTexFrameMouseover);
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, CRefString().Format("%s_Selected", sTabBase.str()), tTexFramePushed);
            ptButton->SetTextureFrames(tTexFrameNormal, tTexFrameMouseover, tTexFramePushed);
        }
    }

    UIButton & tBtnPurchase = UI_GetButtonFromID(UIBTN_STASH_PURCHASE);
    int32 nMaxSlots = ACDInventoryGetMaxStashSlots(*ptACDPlayer);
    if (nSlotCount >= nMaxSlots)
    {
        tBtnPurchase.SetActive(FALSE);
    }
    else
    {
        tBtnPurchase.SetActive(TRUE);

        BOOL fCanPurchase = FALSE;

        CRefString sCost;

        if (OnlineService::IsFreeToPlay())
        {

            fCanPurchase = sgptStashVariables->bStashTabsAvailableInStore;

            if (nNumTabs <= MAX_VISIBLE_TABS)
            {
                sCost = StringListFetch(GlobalSNOGet(1884), "BuyStashTabFromStore");
                sgptStashVariables->bCanBuyMoreTabs = TRUE;
            }
        }
        else
        {






















            if (ACDInventoryStashTabsBuyableWithGold(*ptACDPlayer) > 0)
            {
                sCost = StringListFetch(GlobalSNOGet(736), "BuyTab");

                int32 nUpgradeCost = ACDInventoryGetStashUpgradeCost(*ptACDPlayer);
                CurrencyType eCurrencyType = 0;
                CRefString sUpgradeCost = UIText_ValueToGameCurrency(nUpgradeCost, eCurrencyType);
                Set_Text_Color(sUpgradeCost, 0);
                sCost += sUpgradeCost;

                fCanPurchase = ptACDPlayer->CanAffordCurrencyCost(nUpgradeCost, eCurrencyType) != 0;
            }
        }












        tBtnPurchase.SetText(sCost, FALSE);
        tBtnPurchase.SetEnabled(fCanPurchase);
    }
}



void UIStash::Draw(const uint8, float & flZSlice)


{
    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return;



    if (UI_GetWindowFromID(UIWND_STASH_ICON_PICKER).IsActive())
        return;



    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);


    float flLayerZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;
    float flZToUse = flLayerZ;

    PlayerClass eClass = ptPlayer->ePlayerClassPortrait;
    BOOL fIsMale = PlayerIsMale(*ptPlayer);
    UIInventory::SlotDrawItems(tACDPlayer, eClass, fIsMale, SLOT_PLAYER_SHARED_STASH, flZToUse, 0);


    const ActorCommonData * ptACDHand = NULL;
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    if (eCursorState == CURSORSTATE_MOVE_ITEM)
    {
        ptACDHand = tCursorMgr.GetItemToMove();
    }
    else if (eCursorState == CURSORSTATE_SPLIT_STACK)
    {
        ptACDHand = tCursorMgr.GetStackToSplit();
    }
    if (ptACDHand)
    {

        InventorySlotType eSlot;
        IVector2D tSlot;
        UIInventory::TrackItemDrop(*ptACDHand, eSlot, tSlot);
        if (eSlot == SLOT_PLAYER_SHARED_STASH)
        {
            UIInventory::HighlightSlot(SLOT_PLAYER_SHARED_STASH, tSlot, tACDPlayer, *ptACDHand, flZToUse);
        }
    }
}


























void UIStash::Purchase(const UIID &)

{

    if (!UI_TestID(UIDLG_STASH_PAGE))
        return;
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (!ptACDPlayer)
        return;
}
