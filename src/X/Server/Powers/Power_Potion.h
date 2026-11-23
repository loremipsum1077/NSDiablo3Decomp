// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/Power_Potion.h

#pragma once

#include "../../Common/CommonTypes.h"

struct PotionPowerData {  // size 0x4
    ACDID idACDItem;  // 0x00
};
static_assert(sizeof(PotionPowerData) == 0x4, "PotionPowerData");
