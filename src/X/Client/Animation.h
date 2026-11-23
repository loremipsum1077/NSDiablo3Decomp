// Minimal declarations adopted from X/Client/Animation.h DWARF.
#pragma once
#include "../Sigma/Math/Transform.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gfx/GfxCommon.h"
#include "../Sigma/Math/Matrix.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Vector.h"
struct RActor;
struct AnimPermutation;
struct AnimationDefinition;
class AnimationController {
public:
    // Empty inline base destructor: no emitted call in Ragdoll destruction.
    virtual ~AnimationController() {}
    virtual void ComputeBone(RActor *, PRSTransform &, const PRSTransform &, const PRSTransform &, const PRSTransform &, int32, float) = 0;
};
static_assert(sizeof(AnimationController) == 8, "AnimationController");
struct ProcessedBone {
    PRSTransform modelTransform;
    PRSTransform transformDelta;
    PRSTransform skinDelta;
};
static_assert(sizeof(ProcessedBone) == 0x60, "ProcessedBone");

// Added from the X/Client/Animation.h DWARF for X/Client/Animation.cpp.
enum {
    INITIAL_MAX_ANIMATING_RACTORS = 64,
    MAX_BONES_PER_ACTOR = 255,
    MAX_BONE_CACHE_ENTRIES = 3,
};

typedef int32 VBFormat;

typedef int32 AnimTreeUpdateMode;

typedef int32 WeaponClass;

struct AnimPermID {  // size 0x8
    SNO snoAnim;  // 0x00
    int32 nPermutationIndex;  // 0x04
};
static_assert(sizeof(AnimPermID) == 0x8, "AnimPermID");

struct BoneState {  // size 0xb8
    uint32 dwSizeOfFrameCount;  // 0x00
    int32 nTranslationKey;  // 0x04
    float flTransSegStart;  // 0x08
    int32 nTransSegStart;  // 0x0c
    int32 nTransSegEnd;  // 0x10
    int32 nTransKeyCount;  // 0x14
    Vector3D wpBase;  // 0x18
    uint8 dwSizeOfTrans;  // 0x24
    uint32 dwFirstTransKeyFrameOffset;  // 0x28
    uint32 dwFirstTransKeyDataOffset;  // 0x2c
    union {  // size 0x4
        int32 nTransSegSpanInv;  // 0x00
        float flTransSegSpanInv;  // 0x00
    };  // 0x30
    Vector3D wpTransKey1;  // 0x34
    Vector3D wpTransKey2;  // 0x40
    int32 nRotationKey;  // 0x4c
    float flRotSegStart;  // 0x50
    int32 nRotSegStart;  // 0x54
    int32 nRotSegEnd;  // 0x58
    int32 nRotKeyCount;  // 0x5c
    uint32 dwFirstRotKeyFrameOffset;  // 0x60
    uint32 dwFirstRotKeyDataOffset;  // 0x64
    union {  // size 0x4
        int32 nRotSegSpanInv;  // 0x00
        float flRotSegSpanInv;  // 0x00
    };  // 0x68
    Quaternion qRotKey1;  // 0x6c
    Quaternion qRotKey2;  // 0x7c
    int32 nScaleKey;  // 0x8c
    float flScaleSegStart;  // 0x90
    int32 nScaleSegStart;  // 0x94
    int32 nScaleSegEnd;  // 0x98
    int32 nScaleKeyCount;  // 0x9c
    uint8 dwSizeOfScale;  // 0xa0
    uint32 dwFirstScaleKeyFrameOffset;  // 0xa4
    uint32 dwFirstScaleKeyDataOffset;  // 0xa8
    union {  // size 0x4
        int32 nScaleSegSpanInv;  // 0x00
        float flScaleSegSpanInv;  // 0x00
    };  // 0xac
    float flScaleKey1;  // 0xb0
    float flScaleKey2;  // 0xb4
};
static_assert(sizeof(BoneState) == 0xb8, "BoneState");

struct BoneCacheEntry {  // size 0x18
    PRSTransform * ptBone;  // 0x00
    Vector3D wvNonlinearOffset;  // 0x08
};
static_assert(sizeof(BoneCacheEntry) == 0x18, "BoneCacheEntry");

struct AnimBoneCache {  // size 0x60
    int32 nNumEntries;  // 0x00
    BoneCacheEntry tBoneCache[3];  // 0x08
    int32 nCacheIndex;  // 0x50
    int32 nPrevCacheIndex;  // 0x54
    BOOL bCacheIsValid;  // 0x58
};
static_assert(sizeof(AnimBoneCache) == 0x60, "AnimBoneCache");

struct AnimChannel {  // size 0x80
    void Init(int nBoneCount, SNO snoAppearance);
    void Dispose();
    BOOL IsStopped() const;
    int32 GetCurrentFrame() const;
    int32 GetNextFrame() const;
    BOOL ShouldApplyRetroFilter() const;
    BOOL Advance(float flDeltaTime);
    void BuildAnimToStructureMap(const AnimationDefinition * ptAnimDef, const AnimPermutation & tAnimPerm, int32 nPermutationIndex);
    void SetDirectlyControlled(const BOOL fVal);
    PlaybackMode ePlaybackMode;  // 0x00
    AnimPermID tAnimPermID;  // 0x04
    float flAnimSpeedScale;  // 0x0c
    float flTimeScale;  // 0x10
    float flFrameRate;  // 0x14
    int32 nFrameCount;  // 0x18
    float flTLast;  // 0x1c
    float flT;  // 0x20
    float flTMax;  // 0x24
    float flScaleDeltaT;  // 0x28
    int32 nBoneStateCount;  // 0x2c
    BoneState * arBoneStates;  // 0x30
    float flBlendTime;  // 0x38
    float flBlendT;  // 0x3c
    BOOL bPaused;  // 0x40
    BOOL fDirectlyControlled;  // 0x44
    BOOL fOneToOne;  // 0x48
    BOOL fNeedsInterpolation;  // 0x4c
    BOOL bDisableRetroFiltering;  // 0x50
    BOOL bFullRagdoll;  // 0x54
    float flTranslationScale;  // 0x58
    int16 * pnChannelAnimToStructure;  // 0x60
    SNO snoMyAppearance;  // 0x68
    SNO snoCurrentAnimToStructure;  // 0x6c
    int32 nCurrentAnimPermutation;  // 0x70
    int16 * pnCustomAnimToStructure;  // 0x78
};
static_assert(sizeof(AnimChannel) == 0x80, "AnimChannel");

inline BOOL AnimChannel::IsStopped() const
{
    return Math::Abs(flScaleDeltaT) < EPSILON;
}

struct ProcessedBoneSkin {  // size 0x40
    Matrix4x4 mSkin;  // 0x00
};
static_assert(sizeof(ProcessedBoneSkin) == 0x40, "ProcessedBoneSkin");

struct AnimTreeSyncGroup {  // size 0x28
    float flTotalBlendWeight;  // 0x00
    float flInvCycleTimeBlend;  // 0x04
    float flCycleTimeScale;  // 0x08
    float flCycleTime;  // 0x0c
    float flAnimSpeedScale;  // 0x10
    float flFrameRate;  // 0x14
    float flT;  // 0x18
    float flTMax;  // 0x1c
    PlaybackMode ePlaybackMode;  // 0x20
    AnimTreeUpdateMode eUpdateMode;  // 0x24
};
static_assert(sizeof(AnimTreeSyncGroup) == 0x28, "AnimTreeSyncGroup");

struct AnimTreeNodeUpdate {  // size 0x1c
    float flTSec;  // 0x00
    float flDurationSec;  // 0x04
    float flBlend;  // 0x08
    PlaybackMode ePlaybackMode;  // 0x0c
    BOOL bManualRampOut;  // 0x10
    uint32 dwChild;  // 0x14
    uint32 dwNextChild;  // 0x18
};
static_assert(sizeof(AnimTreeNodeUpdate) == 0x1c, "AnimTreeNodeUpdate");

struct AnimTreeLeafUpdate {  // size 0x10
    BOOL bActive;  // 0x00
    float flInvCycleTime;  // 0x04
    float flBlendWeight;  // 0x08
    BOOL bHitEnd;  // 0x0c
};
static_assert(sizeof(AnimTreeLeafUpdate) == 0x10, "AnimTreeLeafUpdate");

struct AnimTreeLeafBoneCache {  // size 0x18
    BOOL bDirty;  // 0x00
    PRSTransform * ptBonePose;  // 0x08
    float * pflBoneWeight;  // 0x10
};
static_assert(sizeof(AnimTreeLeafBoneCache) == 0x18, "AnimTreeLeafBoneCache");

struct AnimTreeLeafState {  // size 0x28
    AnimTreeLeafUpdate tUpdate;  // 0x00
    AnimTreeLeafBoneCache tBoneCache;  // 0x10
};
static_assert(sizeof(AnimTreeLeafState) == 0x28, "AnimTreeLeafState");

struct AnimTree {  // size 0x48
    int32 nBoneCount;  // 0x00
    uint32 dwNumChannels;  // 0x04
    AnimChannel * ptChannels;  // 0x08
    uint32 dwNumNodeUpdates;  // 0x10
    AnimTreeNodeUpdate * ptNodeUpdate;  // 0x18
    uint32 dwNumLeafStates;  // 0x20
    AnimTreeLeafState * ptLeafState;  // 0x28
    uint32 dwNumSyncGroups;  // 0x30
    AnimTreeSyncGroup * ptSyncGroup;  // 0x38
    BOOL bPaused;  // 0x40
};
static_assert(sizeof(AnimTree) == 0x48, "AnimTree");

void AnimationChannelEnableFullRagdoll(AnimChannel & tChannel);

inline void AnimChannel::SetDirectlyControlled(const BOOL fVal)
{
    fDirectlyControlled = fVal;
}

void AnimationChannelGetNonlinearOffset(AnimChannel & tChannel, AnimBoneCache & tCache, Vector3D & wvOffset);  // Animation.cpp:750
