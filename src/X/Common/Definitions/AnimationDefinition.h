// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/AnimationDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Client/Gfx/GfxCommon.h"
#include "AttachmentDefinition.h"
#include "Definitions.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;

enum {
    ANIM_PERMUTATION_NAME_LENGTH = 64,
};

enum {
    ANIMATION_IMPORT_OVERWRITES_TRANSLATION_BIT = 0,
    ANIMATION_SHEATH_WEAPONS = 1,
    ANIMATION_LOOK_AT_PLAYER_BIT = 2,
    ANIMATION_PLAY_ALL_MESSAGES_BIT = 3,
    ANIMATION_HIDE_FIST_WEAPONS_BIT = 4,
    ANIMATION_DOESNT_TURN_TO_FACE_BIT = 5,
};

enum {
    ANIM_PERMUTATION_ENABLES_RAGDOLL_BIT = 0,
};

enum {
    ABW_ACTION = 0,
    ABW_GETHIT = 1,
    ABW_MAX = 2,
    ABW_NONE = 16,
};

typedef int32 AnimBoneWeightSet;

struct KeyframedAttachment {  // size 0xc0
    float flKeyframeIndex;  // 0x00
    TriggerEvent tTriggeredEvent;  // 0x04
};
static_assert(sizeof(KeyframedAttachment) == 0xc0, "KeyframedAttachment");

struct BoneName {  // size 0x4
    uint32 dwHash;  // 0x00
};
static_assert(sizeof(BoneName) == 0x4, "BoneName");

struct TranslationKey {  // size 0x10
    int32 nFrame;  // 0x00
    Vector3D wp;  // 0x04
};
static_assert(sizeof(TranslationKey) == 0x10, "TranslationKey");

struct RotationKey {  // size 0xc
    int32 nFrame;  // 0x00
    Quaternion16 q16;  // 0x04
};
static_assert(sizeof(RotationKey) == 0xc, "RotationKey");

struct ScaleKey {  // size 0x8
    int32 nFrame;  // 0x00
    float flScale;  // 0x04
};
static_assert(sizeof(ScaleKey) == 0x8, "ScaleKey");

struct TranslationCurve {  // size 0x10
    SerializeData serKeysComp;  // 0x00
    XPtr64<unsigned char> ptKeysComp;  // 0x08
};
static_assert(sizeof(TranslationCurve) == 0x10, "TranslationCurve");

struct RotationCurve {  // size 0x10
    SerializeData serKeysComp;  // 0x00
    XPtr64<unsigned char> ptKeysComp;  // 0x08
};
static_assert(sizeof(RotationCurve) == 0x10, "RotationCurve");

struct ScaleCurve {  // size 0x10
    SerializeData serKeysComp;  // 0x00
    XPtr64<unsigned char> ptKeysComp;  // 0x08
};
static_assert(sizeof(ScaleCurve) == 0x10, "ScaleCurve");

struct BoneBlendWeights {  // size 0x8
    float aflWeight[2];  // 0x00
};
static_assert(sizeof(BoneBlendWeights) == 0x8, "BoneBlendWeights");

struct AnimPermutation {  // size 0x168
    uint32 dwFlags;  // 0x00
    char szPermutationName[68];  // 0x04
    float flFrameRate;  // 0x48
    float flCompression;  // 0x4c
    float flTranslationCompressionRatio;  // 0x50
    float flRotationCompressionRatio;  // 0x54
    float flScaleCompressionRatio;  // 0x58
    uint32 dwBlendTime;  // 0x5c
    uint32 dwFromPermBlendTime;  // 0x60
    uint32 dwWeight;  // 0x64
    float flSpeedMultMin;  // 0x68
    float flSpeedMultDelta;  // 0x6c
    float flRagdollVelocityFactor;  // 0x70
    float flRagdollMomentumFactor;  // 0x74
    int32 nBoneCount;  // 0x78
    SerializeData serBoneNames;  // 0x7c
    XPtr64<BoneName> ptBoneNames;  // 0x88
    int32 nNumBoneBlendWeights;  // 0x90
    int32 nPad;  // 0x94
    SerializeData serBoneBlendWeights;  // 0x98
    XPtr64<BoneBlendWeights> ptBoneBlendWeights;  // 0xa0
    int32 nKeyframeCount;  // 0xa8
    SerializeData serTranslationCurves;  // 0xac
    XPtr64<TranslationCurve> ptTranslationCurves;  // 0xb8
    SerializeData serRotationCurves;  // 0xc0
    XPtr64<RotationCurve> ptRotationCurves;  // 0xc8
    SerializeData serScaleCurves;  // 0xd0
    XPtr64<ScaleCurve> ptScaleCurves;  // 0xd8
    float flContactKeyframe[4];  // 0xe0
    Vector3D wpContactOffset[4];  // 0xf0
    float flEarliestInterruptKeyFrame;  // 0x120
    SerializeData serKeyedAttachments;  // 0x124
    int32 nKeyedAttachmentCount;  // 0x12c
    XPtr64<KeyframedAttachment> ptKeyedAttachments;  // 0x130
    SerializeData serNonlinearOffset;  // 0x138
    XPtr64<Vector3D> pwvNonlinearOffset;  // 0x140
    Vector3D wvAvgVel;  // 0x148
    HardpointLink tHardpointLink;  // 0x154
    XPtr64<short> pnAnimToStructure;  // 0x160
};
static_assert(sizeof(AnimPermutation) == 0x168, "AnimPermutation");

struct AnimationDefinition {  // size 0x38
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    PlaybackMode ePlaybackMode;  // 0x10
    SNO snoAppearance;  // 0x14
    SerializeData serPermutations;  // 0x18
    int32 nPermutationCount;  // 0x20
    XPtr64<AnimPermutation> ptPermutations;  // 0x28
    uint32 dwMachineTime;  // 0x30
};
static_assert(sizeof(AnimationDefinition) == 0x38, "AnimationDefinition");

class AnimationGroupDef : public GroupDefinition {  // size 0xa0
public:
    AnimationGroupDef();
    virtual ~AnimationGroupDef();  // vtable slot 0
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 238; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 243
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 245
        return SNOPACKTYPE_COREDATA;
    }
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
};
static_assert(sizeof(AnimationGroupDef) == 0xa0, "AnimationGroupDef");

extern AnimationGroupDef * gptGroupDefAnimation;  // AnimationDefinition.cpp:181

typedef ScopedDefinitionLockR<AnimationDefinition> ScopedAnimationDefinitionLockR;

// Same shape as DefinitionLockR<ActorDefinition> (ActorDefinition.h:281), which is proven.
template <> inline const AnimationDefinition * DefinitionLockR<AnimationDefinition>(SNO sno) {  // line 289
    return (const AnimationDefinition *)gptGroupDefAnimation->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<AnimationDefinition>(const AnimationDefinition * pdef) {  // line 290
    void * lpDef = (void *)pdef;
    gptGroupDefAnimation->UnlockR(lpDef);
}


struct Structure;
void AnimationBuildAnimToStructureMapping(const Structure & tStructure, const int32 nBoneCount, const BoneName * ptBoneNames, int16 * pnAnimToStructure, BOOL * pfOneToOne);  // AnimationDefinition.cpp:1240

// Functions defined in this file (bodies are not in the DWARF):
//   line   238  void * AnimationGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x710060b520 (+1 more)]
//   line   238  void AnimationGroupDef::MemFree(void *& ptr) const  [0x710060b570 (+1 more)]
//   line   243  BOOL AnimationGroupDef::CanReferenceOtherSNOs() const  [0x710060b600]
//   line   245  SNOPackType AnimationGroupDef::GetSNOPackDestination(const void *) const  [0x710060b5f0]
//   line   289  const AnimationDefinition * DefinitionLockR<AnimationDefinition>(SNO sno)  [inlined into 29 functions]
//   line   290  void DefinitionUnlockR<AnimationDefinition>(const AnimationDefinition * pdef)  [inlined into 29 functions]

// Variables defined in this file:
//   line   129  static const uint32 MAX_NUM_ANIM_CONTACT_FRAMES

#line 129 "X/Common/Definitions/AnimationDefinition.h"
static const uint32 MAX_NUM_ANIM_CONTACT_FRAMES = 4;
