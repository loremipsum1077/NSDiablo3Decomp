// Original file: X/Client/UserInterface/Console/UIConsoleControllerLayout.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Interface/UIFunctions.h"
#include "../UIID.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
// Local declarations stand in for headers not adopted yet; each comment names the defining file.
extern UIID UIWND_CONSOLE_CONTROLLER_LAYOUT, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT;  // X/Client/UserInterface/UIGlobals.cpp
UIWindow & UI_GetWindowFromID(const UIID & id); UIText & UI_GetTextFromID(const UIID & id); const UIID & UI_GetControllerFocus();  // X/Client/UserInterface/UI.cpp
int32 InputGetControllerType(int32 nControllerIndex);  // X/Client/Input/Input.cpp

namespace UIControllerLayout {
namespace Console {

enum {
    ACTION_BACK = 0,
};

void SetControllerFields();
void RefreshDisplayedControllerLayout();
void Close();






struct ConsoleControllerLayout {  // size 0x18
    ConsoleControllerLayout();
    int32 nOpenedControllerIndex;  // 0x00
    UIID idPreviousWindow;  // 0x08
    UIFunctionPtr pfnOnClose;  // 0x10
};
inline ConsoleControllerLayout::ConsoleControllerLayout()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)

    : idPreviousWindow(GetUIIDNone())
    , pfnOnClose(NULL)
{
}













static ConsoleControllerLayout * sgptConsoleControllerLayoutData;

void SetControllerField(CRefString sControlName,
    CRefString sStringLabel,
    CRefString sIcon,
    BOOL bTextOnLeft,
    UIID idParentWindow,
    BOOL bShowIcon)
{
    UIID idText;
    CRefString sText, sFormattedIcon;
    idText.SetFromParent(idParentWindow, sControlName);

    if (bShowIcon)
    {
        sFormattedIcon.Format("{%s, 2.5}", sIcon.str());
        if (bTextOnLeft)
        {
            sText.Format("%s %s", StringListFetch(sStringLabel).str(), sFormattedIcon.str());
        }
        else
        {
            sText.Format("%s %s", sFormattedIcon.str(), StringListFetch(sStringLabel).str());
        }
    }
    else
    {
        sText = StringListFetch(sStringLabel);
    }
    UI_GetTextFromID(idText).SetText(sText, FALSE);
}



void SetControllerFields()

{

    SetControllerField("GRIPZL", "Console:ControllerLayoutSkill5", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPZR", "Console:ControllerLayoutSkill6", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPL", "Console:ControllerLayoutLock", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPR", "Console:ControllerLayoutPotion", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPMinus", "Console:ControllerLayoutMenu", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPPlus", "Console:ControllerLayoutInventory", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPl_stick", "Console:ControllerLayoutMove", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPup", "Console:ControllerLayoutEquip", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPleft", "Console:ControllerLayoutSeasonJourney", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPdown", "Console:ControllerLayoutMap", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPright", "Console:ControllerLayoutPortal", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPx", "Console:ControllerLayoutSkill4", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPy", "Console:ControllerLayoutSkill1", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPb", "Console:ControllerLayoutSkill2", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPa", "Console:ControllerLayoutSkill3", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);
    SetControllerField("GRIPr_stick", "Console:ControllerLayoutEvade", "", TRUE, UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT, FALSE);


    SetControllerField("ZL", "Console:ControllerLayoutPotion", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSR", "Console:ControllerLayoutSkill6", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSL", "Console:ControllerLayoutSkill5", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("Minus", "Console:ControllerLayoutInventory", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJStick", "Console:ControllerLayoutMove", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSPup", "Console:ControllerLayoutEquip", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSPleft", "Console:ControllerLayoutSeasonJourney", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSPdown", "Console:ControllerLayoutMap", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSPright", "Console:ControllerLayoutPortal", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJSPminus", "Console:ControllerLayoutMenu", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJup", "Console:ControllerLayoutSkill4", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJleft", "Console:ControllerLayoutSkill1", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJdown", "Console:ControllerLayoutSkill2", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("LJright", "Console:ControllerLayoutSkill3", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);

    SetControllerField("ZR", "Console:ControllerLayoutPotion", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSL", "Console:ControllerLayoutSkill5", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSR", "Console:ControllerLayoutSkill6", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("Plus", "Console:ControllerLayoutInventory", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJStick", "Console:ControllerLayoutMove", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSPup", "Console:ControllerLayoutEquip", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSPleft", "Console:ControllerLayoutSeasonJourney", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSPdown", "Console:ControllerLayoutMap", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSPright", "Console:ControllerLayoutPortal", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJSPplus", "Console:ControllerLayoutMenu", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJup", "Console:ControllerLayoutSkill4", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJleft", "Console:ControllerLayoutSkill1", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJdown", "Console:ControllerLayoutSkill2", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
    SetControllerField("RJright", "Console:ControllerLayoutSkill3", "", TRUE, UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT, FALSE);
}

void RefreshDisplayedControllerLayout()

{
    int32 nControllerType = InputGetControllerType(sgptConsoleControllerLayoutData->nOpenedControllerIndex);

    if (nControllerType == 0 || nControllerType == 6)
    {
        return;
    }

    // The controller-type enumerators are not in the DWARF of this file; the literals are the compared values.
    if (nControllerType == 4 || nControllerType == 5)
    {
        if (UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT).IsActive())
        {
            UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT).SetActive(FALSE);
            UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT).SetActive(TRUE);
        }
    }
    else
    {
        if (UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT).IsActive())
        {
            UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_GRIP_CONTENT).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_JOYCON_CONTENT).SetActive(FALSE);
        }
    }
}



























































void OpenForController(int32 nControllerIndex)

{
    sgptConsoleControllerLayoutData->nOpenedControllerIndex = nControllerIndex;
    sgptConsoleControllerLayoutData->idPreviousWindow = UI_GetControllerFocus();
    UIID idWindow = UIWND_CONSOLE_CONTROLLER_LAYOUT;
    UIWindow & tCurrent = UI_GetWindowFromID(idWindow);
    tCurrent.SetActive(TRUE);

    SetControllerFields();


    RefreshDisplayedControllerLayout();


    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());
    ::Console::Screens::UpdateButtonLegendText();
}






BOOL IsVisible()

{
    return UI_GetWindowFromID(UIWND_CONSOLE_CONTROLLER_LAYOUT).IsActive();
}

void OnUpdate()

{
    if (!IsVisible())
    {
        return;
    }
    RefreshDisplayedControllerLayout();
}


void Back(const UIID & id, const int32 nControllerIndex)






{
    Close();
}

void Close()

{
    if (IsVisible())
    {







        UIID idWindow = UIWND_CONSOLE_CONTROLLER_LAYOUT;
        UIWindow & tCurrent = UI_GetWindowFromID(idWindow);
        tCurrent.SetActive(FALSE);


        if (sgptConsoleControllerLayoutData->idPreviousWindow != GetUIIDNone())
        {
            UI_SetControllerFocus(sgptConsoleControllerLayoutData->idPreviousWindow);
        }
        if (sgptConsoleControllerLayoutData->pfnOnClose)
        {
            sgptConsoleControllerLayoutData->pfnOnClose(idWindow);
        }
    }
}
void InitializeForGame()

{
    sgptConsoleControllerLayoutData = new ConsoleControllerLayout();
}

void DisposeForGame()

{
    delete sgptConsoleControllerLayoutData; sgptConsoleControllerLayoutData = NULL;
}







void SetOnCloseFunction(const UIFunctionPtr pfnOnClose)

{
    sgptConsoleControllerLayoutData->pfnOnClose = pfnOnClose;
}

}  // namespace Console
}  // namespace UIControllerLayout
