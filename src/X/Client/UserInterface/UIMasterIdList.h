// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIMasterIdList.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaTypes.h"

struct UIID;

enum {
    UIE_INVALID = -1,
    UIE_QUESTLOG = 0,
    UIE_NUM_ELEMENTS = 1,
};

typedef int32 UIElement;

enum UIElementPosition {
    UIEP_INVALID = 0,
    UIEP_LEFT = 1,
    UIEP_RIGHT = 2,
    UIEP_CENTER = 3,
    UIEP_FLOATING = 4,
    UIEP_NUM_ELEMENTS = 5,
};

typedef void (*SIDESCREEN_CALLBACK)(const UIID &, BOOL);

void UIUIIDInit();  // UIMasterIdList.cpp:84
