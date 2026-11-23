// X/Client/CActTransitions.cpp -- the client half of the act-transition countdown.
//
// The module is a single heap-allocated CActTransitionGlobals hanging off CGame (+0xbf8):
// CActTransitionStart records the server-supplied client tick the act flips on in
// dwActTransitionTime, CActTransitionEnded clears it, and CActTransitionInProgress reports
// whether a transition is running.  In the 2.6.2 build these are at 0x3476c0 (initialize),
// 0x347700 (dispose), 0x347740 (start), 0x347820 (ended) and 0x347910 (in progress).

#include "CActTransitions.h"

#include "CGame.h"
#include "Battlenet/Subsystems/ModalNotificationManager.h"
#include "UserInterface/Console/ConsoleModalNotificationManager.h"
#include "UserInterface/Controls/UITextControl.h"
#include "UserInterface/Controls/UIWindowControl.h"
#include "UserInterface/Interface/UIConfirmation.h"
#include "UserInterface/UIID.h"
#include "../Common/ActTransitions.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/Player.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIWND_ACTTRANSITIONS;
extern UIID UITEXT_ACTTRANSITIONS_TITLE;
extern UIID UITEXT_ACTTRANSITIONS_TIME;
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ConsoleModalNotificationManager & GetModalNotificationManager();
void CloseScreenSet(BOOL fClearScreenSetHistory, BOOL fSkipTransition);
}  // namespace Screens
}  // namespace Console

void CActTransitionsInitializeForGame()
{
    CGame & tCGame = CGameGet();
    tCGame.ptCActTransitionGlobals = new CActTransitionGlobals;
    tCGame.ptCActTransitionGlobals->dwActTransitionTime = NO_ACT_TRANSITION_TIME;
    tCGame.ptCActTransitionGlobals->bRecreateGameWithParty = FALSE;
}

void CActTransitionsDisposeForGame()
{
    CGame & tCGame = CGameGet();
    delete tCGame.ptCActTransitionGlobals;
    tCGame.ptCActTransitionGlobals = NULL;
}

void CActTransitionStart(int32 nPlayerIndex, uint32 dwActTransitionTime, BOOL bRecreateGameWithParty)
{
    CGame & tCGame = CGameGet();
    if (tCGame.ptCActTransitionGlobals->dwActTransitionTime == NO_ACT_TRANSITION_TIME)
    {
        tCGame.ptCActTransitionGlobals->dwActTransitionTime = dwActTransitionTime;
        tCGame.ptCActTransitionGlobals->bRecreateGameWithParty = bRecreateGameWithParty;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer != NULL)
    {
        ptPlayer->fOptedIntoActTransition = TRUE;
    }

    UIWindow & tWnd = UI_GetWindowFromID(UIWND_ACTTRANSITIONS);
    if (!tWnd.IsActive())
    {
        Console::Screens::CloseScreenSet(TRUE, FALSE);
    }

    Console::Screens::GetModalNotificationManager().RemoveAllNotifications();

    tWnd.SetActive(TRUE);

    UIText & tTextTitle = UI_GetTextFromID(UITEXT_ACTTRANSITIONS_TITLE);
    tTextTitle.SetActive(TRUE);

    UIText & tTextTime = UI_GetTextFromID(UITEXT_ACTTRANSITIONS_TIME);
    tTextTime.SetActive(TRUE);
}

void CActTransitionEnded()
{
    CGame & tCGame = CGameGet();
    tCGame.ptCActTransitionGlobals->dwActTransitionTime = NO_ACT_TRANSITION_TIME;
    tCGame.ptCActTransitionGlobals->bRecreateGameWithParty = FALSE;

    PlayerGlobals * ptPlayerGlobals = GameGetActiveGameCommonData().ptPlayerGlobals;
    for (int32 i = 0; i < 4; i++)
    {
        Player * ptPlayer = &ptPlayerGlobals->tPlayerArray[i];
        ptPlayer->fOptedIntoActTransition = FALSE;
    }

    UIWindow & tWnd = UI_GetWindowFromID(UIWND_ACTTRANSITIONS);
    tWnd.SetActive(FALSE);

    UIText & tTextTitle = UI_GetTextFromID(UITEXT_ACTTRANSITIONS_TITLE);
    tTextTitle.SetActive(FALSE);

    UIText & tTextTime = UI_GetTextFromID(UITEXT_ACTTRANSITIONS_TIME);
    tTextTime.SetActive(FALSE);

    UIConfirmation::Close();
}

BOOL CActTransitionInProgress()
{
    CGame & tCGame = CGameGet();
    return tCGame.ptCActTransitionGlobals->dwActTransitionTime != NO_ACT_TRANSITION_TIME;
}
