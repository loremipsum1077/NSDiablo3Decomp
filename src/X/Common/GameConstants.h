// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/GameConstants.h
// Subset: the tick-rate enum and OO_TICKS_PER_SECOND (line 31; DW_AT_const_value 0x3C888889 in
// the Wind.cpp compile unit). SLUGS_PER_LB (line 30) is left out until someone recovers its value.

#pragma once

enum {
    TICKS_PER_SECOND = 60,
    FIXED_TICKS_PER_SECOND = 60,
    TICKS_PER_MINUTE = 3600,
};

static const float OO_TICKS_PER_SECOND = 1.0f / 60.0f;
