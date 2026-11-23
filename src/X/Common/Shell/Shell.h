// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Shell/Shell.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Game.h"

typedef int32 GameMode;

enum AppState {
    APPSTATE_STARTING = 0,
    APPSTATE_IDLE = 1,
    APPSTATE_FINISHED = 2,
};

enum SNOChangeFlags {
    SCF_RESTART_UI = 1,
    SCF_RESTART_ANIM_2D = 2,
    SCF_RESTART_GFX = 3,
    SCF_RESET_ITEMMAKER = 4,
    SCF_FIX_ANIMATION_CHANNELS = 5,
    SCF_STOP_USING_SOUNDS = 6,
    SCF_STOP_USING_SOUNDBANKS = 7,
    SCF_FIX_ANIMATION_TREES = 8,
    SCF_CANCEL_CONVERSATIONS = 9,
    SCF_CONSOLE_RELOAD_WORLD = 10,
    SCF_CONSOLE_REBUILD_IMAGE_COLLECTIONS = 11,
};

struct SNOChangeState {  // size 0x8
    SNOChangeState();
    void Reset();
    uint32 dwChangeFlags;  // 0x00
    GameMode eGameModeToRestore;  // 0x04
};
static_assert(sizeof(SNOChangeState) == 0x8, "SNOChangeState");


// Only the declarations this repo uses so far.
void ShellEnableSaveOnQuit(BOOL bEnabled);
BOOL ShellRenderTargetsCreated();
BOOL ShellIsInBoostMode();

// Defined in the platform Shell*.cpp file (ShellNX64.cpp:23).
extern AppState geAppState;

inline AppState SHELL_APP_STATE_GET()
{
    return geAppState;
}

inline void SHELL_APP_STATE_SET(AppState eState)
{
    geAppState = eState;
}

// X/Common/Shell/ShellNX64.cpp:142 and :203.
void ShellInvalidateWindow();
BOOL ShellAppCanInitialize();
