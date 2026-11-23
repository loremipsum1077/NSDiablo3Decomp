// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/AnimCommon.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Client/Gfx/GfxCommon.h"
#include "SNO/SNOFiles.h"

struct AnimationDefinition;
struct RNG;
struct Vector3D;

struct AnimChannelPlayParams {  // size 0x10
    SNO snoAnim;  // 0x00
    int32 nPermutationIndex;  // 0x04
    float flAnimSpeedScale;  // 0x08
    PlaybackMode ePlaybackMode;  // 0x0c
};
static_assert(sizeof(AnimChannelPlayParams) == 0x10, "AnimChannelPlayParams");

// Defined in X/Common/AnimCommon.cpp.
void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim, const int32 nPermutationIndex, const float flAnimSpeedScale);  // :25
void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim, RNG & rng);  // :50
uint32 AnimGetDurationTimeFromPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim);  // :98
uint32 AnimGetContactTimeFromPlayParams(AnimChannelPlayParams & tChannelPlayParams, int32 nContactFrameIndex, const AnimationDefinition * pdefAnim);  // :131
BOOL AnimCanRagdollFromPlayParams(const AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim);  // :165
void AnimationGetAvgVelocity(const AnimationDefinition * pdefAnim, const int32 nPermutationIndex, const float flTranslationScale, const BOOL bMultipleBySpeedScale, Vector3D & wvVel);  // :187
uint32 AnimGetInterruptTimeFromPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim);  // :213
int32 AnimationSelectPermutation(const AnimationDefinition * pdefAnim, RNG & rng);  // :245
float AnimationGetGlobalSpeedScale();  // :285
