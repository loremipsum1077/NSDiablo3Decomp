#include "X/Client/UserInterface/Controls/UIConsoleEditControl.h"
#include "../../../Common/Game.h"
#include "../UI.h"
#include "../../../Common/Shell/ConsoleCommand.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
// Blank lines keep the original line numbers.





































struct UIConsoleEditGlobals {  // size 0x18
    LPSTR uzLogFile;  // 0x00
    BOOL fUseLogFile;  // 0x08
    int32 nCurrentCommandIndex;  // 0x0c
    int32 nMaxBufferedCommands;  // 0x10
    int32 nPlayerOwner;  // 0x14
};










































































































































































































BOOL UIConsoleEdit::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{

    if (msg == UI_REGISTER_INPUT_LISTENER)
        UI_RegisterInputListener(this);






















    return UIEdit::HandleMessage(msg, dwParam);
}


namespace UIConsole {

void InitializeForApp()

{

    AppGlobals & tAppGlobals = sgtAppGlobals;

    tAppGlobals.ptConsole = (UIConsoleEditGlobals *)SigmaMemoryNew(sizeof(UIConsoleEditGlobals), 4, NULL, TRUE);







    tAppGlobals.ptConsole->uzLogFile = (LPSTR)SigmaMemoryNew(MAX_TEXT_LEN, 4, NULL, TRUE);

    tAppGlobals.ptConsole->fUseLogFile = FALSE;
    tAppGlobals.ptConsole->nCurrentCommandIndex = -1;
    tAppGlobals.ptConsole->nMaxBufferedCommands = 0;
    tAppGlobals.ptConsole->nPlayerOwner = -1;
}


void DisposeForApp()

{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    if (tAppGlobals.ptConsole) {







        SigmaMemoryFree(tAppGlobals.ptConsole->uzLogFile, NULL); tAppGlobals.ptConsole->uzLogFile = NULL;


        SigmaMemoryFree(tAppGlobals.ptConsole, NULL); tAppGlobals.ptConsole = NULL;
    }
}










































void OutputString(const CHAR * puzString)
{

































}



BOOL IsActive()
{











    return FALSE;
}

}  // namespace UIConsole
