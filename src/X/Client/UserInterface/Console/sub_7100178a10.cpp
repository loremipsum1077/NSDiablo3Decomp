// 2.7.7: Dark Alchemy console screen (UIDarkAlchemy), an unnamed unit between ConsoleInputUtilities.cpp and
// UIConsoleSeasonStash.cpp (0x7100177fc0..0x710017b0a4). Only the functions that do not need the undecompiled
// dark alchemy node/cost code (sub_7100179f60 -> sub_71004b8530 -> sub_71004b7340 and friends) are written here:
// the close callback, the "can close" test and the button-2 handler. The open callback sub_7100178a10, the update
// callback sub_7100179550 and the button-1 handler sub_710017a270 are not written yet.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/GlobalSNO.h"
#include "../../Sound/Sound.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"

void GameSoftPause(BOOL bPause);

extern UIID unk_7101862568;  // UIGlobals.cpp: "Root.NormalLayer.DarkAlchemyUI_main"

// Unit globals (2.7.7). 0x71017e55d0 is set to -1 by the unit's static initializer 0x710017b020.
static int32 unk_71017e55d0 = -1;  // 0x71017e55d0: controller index the screen was opened for
static int32 unk_71017e55d4;  // 0x71017e55d4: selected node
static BOOL unk_71017e55e0;  // 0x71017e55e0: keep the node selection when closing
static int32 unk_710109d4f8 = -1;  // 0x710109d4f8 (.data)

// NON_MATCHING: the static unit globals are folded/removed by clang while their other users (sub_7100178a10,
// sub_7100179550, sub_710017a270) are unwritten; same logic as the target
// Probably: UIDarkAlchemy::Console close callback of the screen set (paired with the open callback sub_7100178a10).
extern "C" void sub_71001794e0(int32 nControllerIndex)
{
    UI_GetWindowFromID(unk_7101862568).SetActive(FALSE);
    if (!unk_71017e55e0)
    {
        unk_71017e55d4 = 0;
        unk_710109d4f8 = -1;
    }
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAP_CLOSE), cgtPSODefaults);
    GameSoftPause(FALSE);
}

// Probably: UIDarkAlchemy::Console can-close test: TRUE unless the Dark Alchemy window exists and is active.
extern "C" BOOL sub_710017a0b0()
{
    BOOL bCanClose = TRUE;
    if (UI_TestID(unk_7101862568))
        bCanClose = !UI_GetWindowFromID(unk_7101862568).IsActive();
    return bCanClose;
}

// NON_MATCHING: unk_71017e55d0 is folded to its initializer -1 while its writer sub_7100178a10 is unwritten
// Probably: "DarkAlchemyUI_OnButton2": closes the screen set when the button-2 action is close (action 1).
extern "C" void sub_710017a510(const UIID & id, const int32 nControllerIndex)
{
    if (unk_71017e55d0 != nControllerIndex)
        return;
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2) != 1)
        return;
    if (UI_TestID(unk_7101862568) && UI_GetWindowFromID(unk_7101862568).IsActive())
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}
