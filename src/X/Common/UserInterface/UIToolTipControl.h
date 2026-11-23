// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/UserInterface/UIToolTipControl.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

// The DWARF puts the enum and the TooltipType typedef (int32) both on line 12, so they
// share one physical line here.


typedef int32 TooltipType; enum {
    TOOLTIPTYPE_UNKNOWN = -1,
    TOOLTIPTYPE_TEXT = 0,
    TOOLTIPTYPE_ARMOR = 1,
    TOOLTIPTYPE_WEAPON = 2,
    TOOLTIPTYPE_ITEM_OTHER = 3,
    TOOLTIPTYPE_SKILL = 4,
    TOOLTIPTYPE_TRAIT = 5,
    TOOLTIPTYPE_RECIPE = 6,
    TOOLTIPTYPE_ENCHANT = 7,
    TOOLTIPTYPE_TITLED = 8,
    TOOLTIPTYPE_BANNER = 9,
    TOOLTIPTYPE_PLAYERNAME = 10,
    TOOLTIPTYPE_ACHIEVEMENT = 11,
    TOOLTIPTYPE_BNETSKILL = 12,
    TOOLTIPTYPE_REWARD = 13,
    TOOLTIPTYPE_FRIENDINVITE = 14,
    TOOLTIPTYPE_BNETPLAYER = 15,
    TOOLTIPTYPE_INGAMEPORTRAIT = 16,
    TOOLTIPTYPE_FRIEND = 17,
    TOOLTIPTYPE_ARMORSETREWARD = 18,
};
