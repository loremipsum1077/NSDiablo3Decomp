// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Camera/Cinematography.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum CinematographyState {
    CS_NONE = 0,
    CS_HOLODECK_VIEW = 1,
    CS_FIXED_VIEW = 2,
    CS_AV_VIEW = 3,
    CS_STATIC_VIEW = 4,
    CS_HARDPOINT_VIEW = 5,
};
