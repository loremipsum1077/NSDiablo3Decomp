// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/RActors/AnimationChain.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XGrowableList.h"

enum AnimationLoopMode {
    ANIMLOOP_NONE = 0,
    ANIMLOOP_LAST = 1,
    ANIMLOOP_ALL = 2,
    ANIMLOOP_PLAYONCE = 3,
};

struct AnimationChainEntry {  // size 0x8
    SNO snoAnim;  // 0x00
    int32 iPermutation;  // 0x04
};
static_assert(sizeof(AnimationChainEntry) == 0x8, "AnimationChainEntry");

typedef XGrowableList<AnimationChainEntry, const AnimationChainEntry &> TAnimChainEntries;

struct AnimationChain {  // size 0x68
    TAnimChainEntries listChainEntries;  // 0x00
    AnimationLoopMode nLoopMode;  // 0x58
    float flAnimationEndTime;  // 0x5c
    BOOL fNextUpdateIsAtEnd;  // 0x60
    uint32 dwCurrentIndex;  // 0x64
};
static_assert(sizeof(AnimationChain) == 0x68, "AnimationChain");


// Functions defined in this file (bodies are not in the DWARF):
//   line    32  AnimationChain::AnimationChain()  [inlined into 1 functions]
//   line    32  AnimationChain::~AnimationChain()  [inlined into 1 functions]
