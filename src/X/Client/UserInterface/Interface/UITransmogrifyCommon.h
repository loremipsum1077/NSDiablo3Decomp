// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UITransmogrifyCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Common/GameBalance.h"

typedef int32 InventorySlotType;

namespace UITransmogrify {
struct DisplayedTransmog {  // size 0x10
    GBID gbidTransmog;  // 0x00
    SNO snoActor;  // 0x04
    BOOL bUnlocked;  // 0x08
    BOOL bCanUnlock;  // 0x0c
};
static_assert(sizeof(DisplayedTransmog) == 0x10, "DisplayedTransmog");

typedef blz::vector<UITransmogrify::DisplayedTransmog, blz::allocator<UITransmogrify::DisplayedTransmog> > DisplayedTransmogArray;

}  // namespace UITransmogrify
