#include "Shell.h"
#include "../../Client/CGame.h"
#include "../../Client/UserInterface/UI.h"
#include "../Game.h"
#include "../GameCommonData.h"

// ShellCommon.cpp:241
void ShellShutdownUI()
{
    GameCommonData & tGCD = CGameGetGameCommonData();

    if (GameModeIsAPlayingMode(tGCD.GameModeGet()))
    {
        UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;

        if (ptUIGlobals != NULL)
        {
            UI_SetIsResetting(TRUE);
            UI_Reset(true);
            UI_SaveState();
            UI_DisposeForWorld();
            UI_DisposeForGame();
            UI_SetIsResetting(FALSE);
        }
    }
}

// ShellCommon.cpp:413
void ShellRestoreUI()
{
    GameCommonData & tGCD = CGameGetGameCommonData();

    if (GameModeIsAPlayingMode(tGCD.GameModeGet()))
    {
        UI_InitializeForGame();
        UI_InitializeForWorld();
        UI_RestoreState();
    }
}
