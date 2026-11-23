// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Chroma/ChromaEffectsPlatform.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Input/Input.h"
#include "../../Sigma/RGBAColor.h"

enum ChromaEffectType {
    CE_ITEM_DROP = 0,
    CE_LEVEL_UP = 1,
};

struct ChromaEffect {  // size 0x10
    ChromaEffectType eType;  // 0x00
    uint32 dwStartTime;  // 0x04
    float flDurationInSecs;  // 0x08
    RGBAColor rgbaColor;  // 0x0c
};
static_assert(sizeof(ChromaEffect) == 0x10, "ChromaEffect");

struct HotbarButtonState {  // size 0xc
    HotbarButtonState();
    void Clear();
    bool fUsable;  // 0x00
    bool fOnCooldown;  // 0x01
    Key eKey;  // 0x04
    uint32 dwFlashStartTime;  // 0x08
};
static_assert(sizeof(HotbarButtonState) == 0xc, "HotbarButtonState");
