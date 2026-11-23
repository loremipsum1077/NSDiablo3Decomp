// Minimal layout adopted from X/Client/RActors/RActorAnimation.h DWARF.
#pragma once
#include "../Animation.h"
#include "AnimationChain.h"
class Ragdoll;
class LookAt;
class BonePulse;
struct AnimTreeControl;

enum AnimLayer {
    ANIM_LAYER_NONE = -1,
    ANIM_LAYER_ACTION = 0,
    ANIM_LAYER_ACTION_ALT = 1,
    ANIM_LAYER_IDLE = 2,
    ANIM_LAYER_WALK = 3,
    ANIM_LAYER_RUN = 4,
    ANIM_LAYER_SPRINT = 5,
    ANIM_LAYER_SHUFFLE_LEFT = 6,
    ANIM_LAYER_SHUFFLE_RIGHT = 7,
    ANIM_LAYER_WALK_SLOW = 8,
    ANIM_LAYER_OUTRO = 9,
    ANIM_LAYER_FULL_TURN = 10,
    ANIM_LAYER_DEATH = 11,
    ANIM_LAYER_SPECIAL_MOVE = 12,
    ANIM_LAYER_FLINCH = 13,
    ANIM_LAYER_NPC = 14,
    ANIM_LAYER_GETHIT = 15,
    ANIM_LAYER_CACHE = 16,
    ANIM_LAYER_MAX = 17,
};

typedef int32 OutroAnimationType;
struct ActorAnim {
    AnimChannel tAnimChannel;  // 0x00
    AnimBoneCache tAnimBoneCache;  // 0x80
    AnimationController ** ptBoneAnimControllerList;
    ProcessedBone * ptProcessedBoneList;
    ProcessedBoneSkin * ptProcessedBoneSkinList;  // 0xf0
    Vector3D wpMin;  // 0xf8
    Vector3D wpMax;  // 0x104
    AnimationChain tAnimationChain;  // 0x110
    LookAt * ptLookAt;  // 0x178
    Ragdoll * ptRagdoll;  // 0x180
    int32 nBonePulseCount;  // 0x188
    BonePulse * ptBonePulses;  // 0x190
    AnimTree * ptAnimTree;  // 0x198
    SNO snoAnimTree;  // 0x1a0
    AnimTreeControl * ptAnimTreeControl;  // 0x1a8
    BOOL bHasNPCLayer;  // 0x1b0
    float flLastUpdateBonesTime;  // 0x1b4
};
static_assert(sizeof(ActorAnim) == 0x1b8, "ActorAnim");

void RActorsAnimDisposeForGame();  // RActorAnimation.cpp:870
