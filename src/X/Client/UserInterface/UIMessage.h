// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIMessage.h

#pragma once

enum UIMsg {
    UI_UPDATE = 0,
    UI_INPUT = 1,
    UI_REGISTER_INPUT_LISTENER = 2,
    UI_RESET = 3,
    UI_GFX_INIT = 4,
    UI_GFX_DISPOSE = 5,
    UI_INIT_FOR_WORLD = 6,
    UI_DISPOSE_FOR_WORLD = 7,
    UI_INIT_FOR_GAME = 8,
    UI_DISPOSE_FOR_GAME = 9,
    UI_AXE_DUMP = 10,
};

