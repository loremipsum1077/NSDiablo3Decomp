// Original file: X/Client/UserInterface/UIMasterIdList.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIMasterIdList.h"
#include "UI.h"
#include "UIID.h"
#include "UIIDs.h"
#include "Controls/UIControlControl.h"
#include "Console/ConsoleScreenManager.h"
#include "../CGame.h"
#include "../CPlayer.h"
#include "../Input/Input.h"
#include "../../Common/ActorCommonData.h"
// Declared here because their headers are not adopted yet.

namespace UIQuestLore { void OnSideScreenShowHide(const UIID & id, BOOL fWindowOpen); }  // UIQuestLore.cpp:310
namespace UIMap { BOOL IsOpen(); }  // UIMap.cpp:366
namespace UIGame { void Update(float flDeltaTime); }  // UIGame.cpp
void InputBufferInput(const BufferedInput & tInput, InputContext eContext, int32 nControllerIndex);  // Input.cpp

struct SideScreenTable {  // size 0x28
    UIElement eElement;  // 0x00
    UIID uId;  // 0x08
    UIElementPosition eVisualPosition;  // 0x10
    SIDESCREEN_CALLBACK pfnShow;  // 0x18
    SIDESCREEN_CALLBACK pfnHide;  // 0x20
};

































static SideScreenTable sgtSideScreenTable[UIE_NUM_ELEMENTS];



static void sSetTableEntry(int32 nEntry, const UIElement & eElement, UIID & uId, UIElementPosition eVisualPosition, SIDESCREEN_CALLBACK pfnShow, SIDESCREEN_CALLBACK pfnHide)
{









    sgtSideScreenTable[nEntry].eElement = eElement;
    sgtSideScreenTable[nEntry].uId = uId;
    sgtSideScreenTable[nEntry].pfnShow = pfnShow;
    sgtSideScreenTable[nEntry].pfnHide = pfnHide;
    sgtSideScreenTable[nEntry].eVisualPosition = eVisualPosition;
}



void UIUIIDInit()
{

    sSetTableEntry(UIE_QUESTLOG, UIE_QUESTLOG, UIDLG_QUESTLORE, UIEP_LEFT, UIQuestLore::OnSideScreenShowHide, UIQuestLore::OnSideScreenShowHide);



























}



static int32 sGetIndexFromId(const UIID & id)
{
    for (int32 i = 0; i < UIE_NUM_ELEMENTS; i++)
    {
        if (sgtSideScreenTable[i].uId == id)
            return i;
    }
    return UIE_INVALID;
}










































BOOL UISideScreenVisible(UIElementPosition eVisualPosition)
{



    for (int32 i = 0; i < UIE_NUM_ELEMENTS; i++)
    {
        if (UI_TestID(sgtSideScreenTable[i].uId))
        {



            if (sgtSideScreenTable[i].eVisualPosition == eVisualPosition)
            {
                if (UI_GetControlFromID(sgtSideScreenTable[i].uId).IsActive())
                    return TRUE;
            }
        }
    }
    return FALSE;
}





















// NON_MATCHING: 564 vs 584 bytes; everything after the bConflicts test (244-246) matches. The original unswitches the one-iteration loop on eVisualPosition being LEFT/RIGHT and keeps eVisualPosition == FLOATING && side(eComparePosition) as an and of two csets; ours unswitches on eVisualPosition == FLOATING. Tried four operand orders of the three || terms and a |= split. Rows: 245:38 is eComparePosition == UIEP_FLOATING, 246:38 eComparePosition == eVisualPosition, 246:72 the side(eComparePosition) range check.
static void sSideScreenShow(
    const UIID & uDlgId,
    UIElementPosition eVisualPosition,
    uint32 dwSideScreenAlterMask)
{














    BOOL fSomethingFound = FALSE;
    for (int32 i = 0; i < UIE_NUM_ELEMENTS; i++)
    {
        if (dwSideScreenAlterMask & (1 << i))
        {



            SideScreenTable & tOtherScreen = sgtSideScreenTable[i];


            UIElementPosition eComparePosition = tOtherScreen.eVisualPosition;
            BOOL bConflicts = ((eVisualPosition == UIEP_LEFT || eVisualPosition == UIEP_RIGHT) &&
                               eComparePosition == UIEP_FLOATING) ||
                              eComparePosition == eVisualPosition || (eVisualPosition == UIEP_FLOATING && (eComparePosition == UIEP_LEFT || eComparePosition == UIEP_RIGHT));





            if (bConflicts)
            {
                if (UI_TestID(tOtherScreen.uId))
                {
                    fSomethingFound = TRUE;
                    UIControl & tControlSideScreen = UI_GetControlFromID(tOtherScreen.uId);
                    BOOL fControlActive = tControlSideScreen.IsActive();





                    if (tOtherScreen.uId == UIDLG_MAP_PAGE && Console::Screens::IsScreenSetOpen() && UIMap::IsOpen())
                    {
                        Console::Screens::CloseScreenSet(TRUE, FALSE);
                    }

                    tControlSideScreen.SetActive(FALSE);



                    if (fControlActive && tOtherScreen.pfnHide)
                    {
                        tOtherScreen.pfnHide(tOtherScreen.uId, FALSE);
                    }
                }
            }
        }
    }


    // inferred: with UIE_NUM_ELEMENTS 1 every path that does not hide the one table screen returns before the 288 block
    if (!fSomethingFound)
        return;


    if (uDlgId != GetUIIDNone())
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
        if (ptACDPlayer == NULL ||
            !ptACDPlayer->IsAlive())
        {
            return;
        }


        UI_GetControlFromID(uDlgId).SetActive(TRUE);


        int32 nEntry = sGetIndexFromId(uDlgId);
        if (nEntry != UIE_INVALID)
        {
            if (sgtSideScreenTable[nEntry].pfnShow)
            {
                sgtSideScreenTable[nEntry].pfnShow(uDlgId, TRUE);
            }
        }




        BufferedInput tTempInput;
        UIGlobals *ptUIGlobals = CGameGet().ptUIGlobals;
        tTempInput.eInputMsg = INPUTMSG_MOUSEMOVE;
        tTempInput.tBufMouseData.p = ptUIGlobals->tMouseLocation;
        tTempInput.tBufMouseData.nDeltaMouseWheel = 0;
        InputBufferInput(tTempInput, INPUTCONTEXT_DEFAULT, 0);
    }
}




void UISideScreenSetVisible(const UIID & id, const BOOL fVisible)
{



    const int32 nIndex = sGetIndexFromId(id); if (nIndex == UIE_INVALID) return;

    if (fVisible)
    {
        if (!UI_GetControlFromID(id).IsActive())
        {
            sSideScreenShow(id, sgtSideScreenTable[nIndex].eVisualPosition, 0xffffffff);
        }
    }
    else
    {
        if (UI_GetControlFromID(id).IsActive())
        {
            sSideScreenShow(GetUIIDNone(), sgtSideScreenTable[nIndex].eVisualPosition, 0xffffffff);
        }
    }
}





BOOL UISideScreenAnyAreVisible()
{
    return UISideScreenVisible(UIEP_LEFT) || UISideScreenVisible(UIEP_RIGHT) || UISideScreenVisible(UIEP_FLOATING);
}







void UISideScreenCloseAll(uint32 dwSideScreenAlterMask)
{

    sSideScreenShow(GetUIIDNone(), UIEP_LEFT, dwSideScreenAlterMask);
    sSideScreenShow(GetUIIDNone(), UIEP_RIGHT, dwSideScreenAlterMask);
    sSideScreenShow(GetUIIDNone(), UIEP_FLOATING, dwSideScreenAlterMask);
    sSideScreenShow(GetUIIDNone(), UIEP_CENTER, dwSideScreenAlterMask);

    UIGame::Update(1.0f);
}



void UISideScreenSetVisible(const UIElement eElement, const BOOL fVisible)
{



    for (int32 i = 0; i < UIE_NUM_ELEMENTS; i++)
    {
        if (sgtSideScreenTable[i].eElement == eElement)
        {
            if (UI_GetControlFromID(sgtSideScreenTable[i].uId).IsActive() != fVisible)
                UISideScreenSetVisible(sgtSideScreenTable[i].uId, fVisible);
            return;
        }
    }
}
