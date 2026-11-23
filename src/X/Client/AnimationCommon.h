// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/AnimationCommon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx/GfxCommon.h"

enum {
    ANIMATION_AT_END_BIT = 0,
    ANIMATION_FINISHED_BIT = 1,
};

struct AnimState {  // size 0x18
    float flTLast;  // 0x00
    float flT;  // 0x04
    int32 nFrame;  // 0x08
    bool fPending;  // 0x0c
    float flTMax;  // 0x10
    float flTVel;  // 0x14
};
static_assert(sizeof(AnimState) == 0x18, "AnimState");

// Defined in X/Client/AnimationCommon.cpp.
int32 AnimationComputeCurrentFrame( const AnimState & tState );  // line 25
uint32 AnimationAdvance( AnimState & tState, const PlaybackMode ePlaybackMode, const float flDeltaTime );  // line 80
void AnimationReset( AnimState & tAnimState, const FrameAnim & tFrameAnim, const int32 nFrameCount );  // line 190


