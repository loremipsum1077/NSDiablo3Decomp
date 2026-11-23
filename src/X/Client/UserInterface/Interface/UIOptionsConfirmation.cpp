#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIFunctions.h"
#include "../../CPlayerInput.h"
#include "../UIID.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
// X/Client/Cursor.cpp
CursorMgr & PlayerGetCursorMgr();
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_OPTIONSCONFIRMATION, UITXT_OPTIONSCONFIRMATION_MSG;

class UIOptionsConfirmationGlobals {  // size 0x10
public:
    UIFunctionPtr pfnOnClickOK;  // 0x00
    UIFunctionPtr pfnOnClickCancel;  // 0x08
};
static_assert(sizeof(UIOptionsConfirmationGlobals) == 0x10, "UIOptionsConfirmationGlobals");

// The DWARF records a UIOptionsConfirmationGlobals() constructor, but the file keeps no
// static initialiser: both members are function pointers that default to null, so the object
// is zero-initialised and lives in .bss.
static UIOptionsConfirmationGlobals sgtUIOptionsConfirmGlobals;

namespace UIOptionsConfirmation {

BOOL IsVisible()
{
    if (UI_TestID(UIDLG_OPTIONSCONFIRMATION))
        return UI_GetWindowFromID(UIDLG_OPTIONSCONFIRMATION).IsActive();
    return FALSE;
}

void CancelAndClose()
{
    UI_GetWindowFromID(UIDLG_OPTIONSCONFIRMATION).SetActive(FALSE);
    if (sgtUIOptionsConfirmGlobals.pfnOnClickCancel)
    {
        sgtUIOptionsConfirmGlobals.pfnOnClickCancel(GetUIIDNone());
    }
}

void Show(LPCSTR szMessage, UIFunctionPtr pfnOnClickOK, UIFunctionPtr pfnOnClickCancel)
{
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    tCursorMgr.ResetCursorState(FALSE);

    UI_GetWindowFromID(UIDLG_OPTIONSCONFIRMATION).SetActive(TRUE);
    UI_GetTextFromID(UITXT_OPTIONSCONFIRMATION_MSG).SetText(szMessage, FALSE);

    sgtUIOptionsConfirmGlobals.pfnOnClickOK = pfnOnClickOK;
    sgtUIOptionsConfirmGlobals.pfnOnClickCancel = pfnOnClickCancel;
}

}  // namespace UIOptionsConfirmation
