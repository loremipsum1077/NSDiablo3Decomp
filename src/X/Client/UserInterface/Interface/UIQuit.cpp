#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Battlenet/Hub.h"
#include "../../CGame.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../UI.h"






// X/Client/OnlineService/ClientService.h:227 and X/Client/OnlineService/Party.h (neither is adopted;
// only what UIQuit_OnCancel reaches is declared). Party is a pure interface that the DWARF carries
// as a declaration with no members, so its virtual list is not recoverable. The one slot the
// function calls (2.6.2 vtable offset 0x2e0, slot 92) is therefore reached by index, and its name
// is invented.
namespace OnlineService {
class Party {
public:
    void VSlot92(const BOOL bArg) {
        typedef void (*SlotFn)(Party *, BOOL);
        (*reinterpret_cast<SlotFn **>(this))[92](this, bArg);
    }
};
class ClientService {
public:
    OnlineService::Party & Party() { return *m_pParty; }  // line 227
private:
    uint8 _unadopted0[0x10];
    OnlineService::Party * m_pParty;  // 0x10
};
}  // namespace OnlineService
// X/Client/Battlenet/Hub.cpp
Hub & HubGet();
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_LOGOUT;
// X/Client/UserInterface/Interface/UIQuit.h
void UIQuit_Reset();
void UIQuit_DisposeForGame();


static void sUIQuitDialogShow(const BOOL bShow) {  // line 29
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (UI_TestID(UIDLG_LOGOUT)) {
        UIWindow & tDialog = UI_GetWindowFromID(UIDLG_LOGOUT);
        tDialog.SetActive(bShow);
        ptUIGlobals->bUIQuitDialogVisible = bShow;
    }
}

void UIQuit_Reset() {  // line 66
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    if (ptUIGlobals != NULL) {
        sUIQuitDialogShow(FALSE);
        ptUIGlobals->dwUIQuitDisplayUntil = 0;
    }
}

void UIQuit_DisposeForGame() {  // line 92
    UIQuit_Reset();
}

// Blank lines keep the original line numbers for UIQuit_OnCancel (line 151).
















































































void UIQuit_OnCancel()  // line 151

{

    HubGet().ClientServiceGet().Party().VSlot92(FALSE);

    UIQuit_Reset();
}
