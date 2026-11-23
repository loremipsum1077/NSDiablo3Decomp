// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/AnimPlayer.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "../Sigma/Math/Vector.h"

enum AnimOffset {
    ANIMOFFSET_CONTINUE = 0,
    ANIMOFFSET_START = 1,
    ANIMOFFSET_END = 2,
    NUM_ANIMOFFSETS = 3,
};

enum {
    ANIM_REASON_NOT_ANIMATING = 0,
    ANIM_REASON_IDLE = 1,
    ANIM_REASON_WALKING = 2,
    ANIM_REASON_POWER = 3,
    ANIM_REASON_SPAWN = 4,
    ANIM_REASON_GIZMO = 5,
    ANIM_REASON_STANCE = 6,
    ANIM_REASON_INTERRUPTABLE_ANIMATION = 7,
    ANIM_REASON_PREPLAY_POWER = 8,
    ANIM_REASON_CUTSCENE = 9,
    ANIM_REASON_PREPLAY_CHANNELLED = 10,
    ANIM_REASON_DEATH = 11,
    ANIM_REASON_FLOAT_CONVERSATION = 12,
    ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH = 13,
    ANIM_REASON_COUNT = 14,
};

typedef int32 AnimReason;

struct AnimSpec {  // size 0x20
    uint32 dwDuration;  // 0x00
    SNO snoAnim;  // 0x04
    uint32 dwAnimationTag;  // 0x08
    int32 nPermutationIndex;  // 0x0c
    float flAnimSpeedScale;  // 0x10
    Vector3D wvVelocity;  // 0x14
};
static_assert(sizeof(AnimSpec) == 0x20, "AnimSpec");

struct AnimationTiming {  // size 0xa0
    uint32 dwAnimationTag;  // 0x00
    AnimSpec tAnimSpec[3];  // 0x04
    uint32 dwCycleTime;  // 0x64
    uint32 dwExecuteTime[4];  // 0x68
    uint32 dwInterruptTime;  // 0x78
    uint32 dwStartAnimExecuteTime[4];  // 0x7c
    uint32 dwEndAnimExecuteTime[4];  // 0x8c
    float flUnitAnimStartTime;  // 0x9c
};
static_assert(sizeof(AnimationTiming) == 0xa0, "AnimationTiming");

struct PlayAnimationMessageSpec {  // size 0x14
    uint32 dwDuration;  // 0x00
    SNO snoAnim;  // 0x04
    uint32 dwAnimationTag;  // 0x08
    int32 nPermutationIndex;  // 0x0c
    float flAnimSpeedScale;  // 0x10
};
static_assert(sizeof(PlayAnimationMessageSpec) == 0x14, "PlayAnimationMessageSpec");

struct PlayAnimationMessage {  // size 0x54
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    AnimReason eAnimReason;  // 0x0c
    float flUnitAnimStartTime;  // 0x10
    int32 nUsedAnimArrayElements;  // 0x14
    PlayAnimationMessageSpec tAnim[3];  // 0x18
};
static_assert(sizeof(PlayAnimationMessage) == 0x54, "PlayAnimationMessage");

struct SetIdleAnimationMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    uint32 dwAnimTag;  // 0x0c
};
static_assert(sizeof(SetIdleAnimationMessage) == 0x10, "SetIdleAnimationMessage");

class ActorCommonData;
struct RNG;

static const uint32 ANIM_USE_ANIM_LENGTH = 0xffffffff;  // line 51
static const uint32 ANIM_PLAY_FOREVER = 0xfffffffe;  // line 52

BOOL AnimPlayerGetAnimationTiming(ActorCommonData & tACD, const uint32 dwAnimationTag, const uint32 dwLength, const float flSpeedScalar, AnimationTiming & tAnimationTiming);  // AnimPlayer.cpp:698
BOOL AnimPlayerGetAnimationTiming(ActorCommonData & tACD, const uint32 dwAnimationTag, const uint32 dwLength, const float flSpeedScalar, RNG & rng, AnimationTiming & tAnimationTiming);  // AnimPlayer.cpp:789
uint32 AnimPlayerPlayAnimationsWithTiming(ActorCommonData & tACD, AnimationTiming * ptAnimationTiming, int32 nNumAnims, BOOL bUseAnimationTranslation, AnimReason eAnimReason, BOOL fAnimWasPreplayed, BOOL bDontSendToClient);  // AnimPlayer.cpp:1119
uint32 AnimPlayerPlayAnimation(ActorCommonData & tACD, uint32 dwAnimationTag, uint32 dwLength, BOOL bUseAnimationTranslation, AnimReason eAnimReason, float flUnitAnimStartTime, const BOOL bDontSendToClient, float flSpeedScalar);  // AnimPlayer.cpp:1224
void AnimPlayerPlayNextAnimation(ActorCommonData & tACD);  // AnimPlayer.cpp:1964
void AnimPlayerSetIdleAnimation(ActorCommonData & tACD, uint32 dwIdleAnimationTag);  // AnimPlayer.cpp:1339
AnimReason AnimPlayerGetAnimReason(const ActorCommonData & tACD);  // AnimPlayer.cpp:1739
float AnimPlayerGetUnitTime(ActorCommonData & tACD);  // AnimPlayer.cpp:2047
class PlayerList;
void AnimPlayerSendAnimations(ActorCommonData & tACD, const PlayerList & listDestPlayers);  // AnimPlayer.cpp:1555
void AnimPlayerSendIdleAnimation(ActorCommonData & tACD, const PlayerList & listDestPlayers);  // AnimPlayer.cpp:1590
void AnimPlayerSendIdle(ActorCommonData & tACD);  // AnimPlayer.cpp:1872
BOOL AnimPlayerAnimExists(ActorCommonData & tACD, const uint32 dwAnimTag);  // AnimPlayer.cpp:1898


// Functions defined in this file (bodies are not in the DWARF):
//   line    54  AnimSpec & AnimSpec::operator=(const AnimSpec &)  [inlined into 1 functions]

// Variables defined in this file:
//   line    51  static const uint32 ANIM_USE_ANIM_LENGTH
//   line    52  static const uint32 ANIM_PLAY_FOREVER

void AnimPlayerUpdate(ActorCommonData & tACD);  // AnimPlayer.cpp:1429

void AnimPlayerPlayIdle(ActorCommonData & tACD);  // AnimPlayer.cpp:1376

// Owning-header placement inferred; raw DWARF has only AnimPlayer.cpp:655, no header declaration.
void AnimPlayerInit(ActorCommonData & tACD);
