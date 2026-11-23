// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UISeasonJourneyUtil.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    SEASON_JOURNEY_VIEW_INVALID = -1,
    SEASON_JOURNEY_VIEW_NOT_COMPLETED = 0,
    SEASON_JOURNEY_VIEW_COMPLETED = 1,
    SEASON_JOURNEY_VIEW_CASUAL_COMPLETED = 2,
};

typedef int32 SeasonJourneyProgressionView;

