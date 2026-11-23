// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/SoundDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../InterpolationScalar.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

class XMemoryAllocatorInterface;
class XTypeDescriptor;

enum {
    SOUND_DELETED_WITH_SOURCE_BIT = 0,
    SOUND_PLAY_EACH_PERMUTATION_ONCE_BIT = 1,
    SOUND_LOOPS_BIT = 2,
    SOUND_LIMIT_INSTANCES_BIT = 3,
    SOUND_CHOOSE_NEW_PERMUTATION_ON_LOOP_BIT = 4,
    SOUND_DONT_PLAY_WHEN_NEAR_PLAYER_BIT = 5,
    SOUND_PLAY_IN_ORDER_BIT = 6,
    SOUND_UPDATE_POSITION_BIT = 7,
    SOUND_DONT_OCCLUDE_BIT = 8,
    SOUND_NO_BACK_TO_BACK_REPEATS_BIT = 9,
    SOUND_RANDOM_START_POSITION_BIT = 10,
    SOUND_DONT_DUCK_BIT = 11,
    SOUND_IS_CONVERSATION = 12,
    SOUND_BYPASS_RETRO_BIT = 13,
};

enum {
    SOUNDTYPE_EFFECT = 0,
    SOUNDTYPE_MUSIC = 1,
    SOUNDTYPE_NARRATION = 2,
    SOUNDTYPE_AMBIENT = 3,
    SOUNDTYPE_EFFECT_UI = 4,
    NUMBER_OF_SOUND_TYPES = 5,
};

typedef int32 SoundType;

enum {
    PRIORITY_LOWEST = 0,
    PRIORITY_LOW = 1,
    PRIORITY_MEDIUM = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_HIGHEST = 4,
};

enum {
    FALLOFF_NONE = 0,
    FALLOFF_INVERSE_DISTANCE = 1,
    FALLOFF_LINEAR = 2,
    FALLOFF_LINEAR_LOG = 3,
};

typedef int32 FalloffType;

enum {
    ILB_FAIL = 0,
    ILB_STEAL_OLDEST = 1,
    ILB_STEAL_NEWEST = 2,
    ILB_STEAL_QUIETEST = 3,
    ILB_STEAL_FARTHEST = 4,
};

typedef int32 InstanceLimitBehavior;

enum {
    FADE_SHAPE_LINEAR = 0,
    FADE_SHAPE_LOGARITHMIC = 1,
    FADE_SHAPE_EXPONENTIAL = 1,
    FADE_SHAPE_S_CURVE = 2,
    NUM_FADE_SHAPES = 3,
};

typedef int32 FadeShape;

enum DspType {
    DSP_TYPE_CHORUS = 0,
    DSP_TYPE_COMPRESSOR = 1,
    DSP_TYPE_DISTORTION = 2,
    DSP_TYPE_ECHO = 3,
    DSP_TYPE_FLANGE = 4,
    DSP_TYPE_HIGHPASS = 5,
    DSP_TYPE_LOWPASS = 6,
    DSP_TYPE_LOWPASS_SIMPLE = 7,
    DSP_TYPE_PARAMEQ = 8,
    DSP_TYPE_SFXREVERB = 9,
    DSP_NUM_TYPES = 10,
};

enum {
    DSP_FLAG_DISABLED = 0,
};

struct DspParam {  // size 0x10
    float flMinValue;  // 0x00
    float flDeltaValue;  // 0x04
    uint32 dwFlags;  // 0x08
    int32 eSourceScaler;  // 0x0c
};
static_assert(sizeof(DspParam) == 0x10, "DspParam");

struct DspEffect {  // size 0x108
    DspType eDspType;  // 0x00
    uint32 dwFlags;  // 0x04
    union {  // size 0x100
        DspParam arDspParams[16];  // 0x00
        struct {  // size 0x80
            DspParam tChorusDryMix;  // 0x00
            DspParam tChorusWetMix1;  // 0x10
            DspParam tChorusWetMix2;  // 0x20
            DspParam tChorusWetMix3;  // 0x30
            DspParam tChorusDelay;  // 0x40
            DspParam tChorusRate;  // 0x50
            DspParam tChorusDepth;  // 0x60
            DspParam tChorusFeedback;  // 0x70
        };  // 0x00
        struct {  // size 0x40
            DspParam tCompressorThreshold;  // 0x00
            DspParam tCompressorAttack;  // 0x10
            DspParam tCompressorRelease;  // 0x20
            DspParam tCompressorGainMakeup;  // 0x30
        };  // 0x00
        struct {  // size 0x10
            DspParam tDistortionLevel;  // 0x00
        };  // 0x00
        struct {  // size 0x50
            DspParam tEchoDelay;  // 0x00
            DspParam tEchoDecayRatio;  // 0x10
            DspParam tEchoMaxChannels;  // 0x20
            DspParam tEchoDryMix;  // 0x30
            DspParam tEchoWetMix;  // 0x40
        };  // 0x00
        struct {  // size 0x40
            DspParam tFlangeDryMix;  // 0x00
            DspParam tFlangeWetMix;  // 0x10
            DspParam tFlangeDepth;  // 0x20
            DspParam tFlangeRate;  // 0x30
        };  // 0x00
        struct {  // size 0x20
            DspParam tHighpassCutoff;  // 0x00
            DspParam tHighpassResonance;  // 0x10
        };  // 0x00
        struct {  // size 0x20
            DspParam tLowpassCutoff;  // 0x00
            DspParam tLowpassResonance;  // 0x10
        };  // 0x00
        struct {  // size 0x10
            DspParam tLowpassSimpleCutoff;  // 0x00
        };  // 0x00
        struct {  // size 0x30
            DspParam tParamEqCenter;  // 0x00
            DspParam tParamEqBandwidth;  // 0x10
            DspParam tParamEqGain;  // 0x20
        };  // 0x00
        struct {  // size 0xf0
            DspParam tReverbDryLevel;  // 0x00
            DspParam tReverbRoom;  // 0x10
            DspParam tReverbRoomHF;  // 0x20
            DspParam tReverbRoomRolloffFactor;  // 0x30
            DspParam tReverbDecayTime;  // 0x40
            DspParam tReverbDecayHFRatio;  // 0x50
            DspParam tReverbReflectionsLevel;  // 0x60
            DspParam tReverbReflectionsDelay;  // 0x70
            DspParam tReverbReverbLevel;  // 0x80
            DspParam tReverbReverbDelay;  // 0x90
            DspParam tReverbDiffusion;  // 0xa0
            DspParam tReverbDensity;  // 0xb0
            DspParam tReverbHFReference;  // 0xc0
            DspParam tReverbRoomLF;  // 0xd0
            DspParam tReverbLFReference;  // 0xe0
        };  // 0x00
    };  // 0x08
};
static_assert(sizeof(DspEffect) == 0x108, "DspEffect");

enum RTPCTarget {
    RTPC_TARGET_VOLUME = 0,
    RTPC_TARGET_PITCH = 1,
    NUM_RTPC_TARGETS = 2,
};

struct RTPCParams {  // size 0x18
    void Init();
    RTPCTarget eTarget;  // 0x00
    InterpolationScalar tScalar;  // 0x04
    FadeShape eCurveShape;  // 0x10
    float flCurveShapeAccel;  // 0x14
};
static_assert(sizeof(RTPCParams) == 0x18, "RTPCParams");

struct SoundPermutation {  // size 0x40
    SNO snoSoundBank;  // 0x00
    uint32 dwSampleHash;  // 0x04
    uint32 dwWeight;  // 0x08
    float flVolumeDBMin;  // 0x0c
    float flVolumeDBDelta;  // 0x10
    float flPitchSemitonesMin;  // 0x14
    float flPitchSemitonesDelta;  // 0x18
    uint32 dwFadeInTime;  // 0x1c
    uint32 dwFadeOutTime;  // 0x20
    FadeShape eFadeInShape;  // 0x24
    FadeShape eFadeOutShape;  // 0x28
    float flFadeInAccel;  // 0x2c
    float flFadeOutAccel;  // 0x30
    uint32 dwDelay;  // 0x34
    int32 nSampleIndex;  // 0x38
    SNO snoValidatedSoundBank;  // 0x3c
};
static_assert(sizeof(SoundPermutation) == 0x40, "SoundPermutation");

struct SoundPlayHistory {  // size 0x18
    int32 nLastPlayedPerm;  // 0x00
    uint32 dwLastPlayedTime;  // 0x04
    uint8 arPlayedFlags[16];  // 0x08
};
static_assert(sizeof(SoundPlayHistory) == 0x18, "SoundPlayHistory");

struct SoundDefinition {  // size 0xa8
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    uint32 dwPriority;  // 0x10
    float wdFalloffRadiusMin;  // 0x14
    float wdFalloffRadiusDelta;  // 0x18
    float flRolloffFactor;  // 0x1c
    FalloffType eFalloffType;  // 0x20
    float flDopplerFactor;  // 0x24
    float flDuckAmountDB;  // 0x28
    float flGlobalVolumeDB;  // 0x2c
    float flGlobalPitch;  // 0x30
    float flImpulseMin;  // 0x34
    float flImpulseDelta;  // 0x38
    float flImpulseAttenMinDB;  // 0x3c
    float flImpulseAttenMaxDB;  // 0x40
    SerializeData serPermutations;  // 0x44
    int32 nPermutationCount;  // 0x4c
    XPtr64<SoundPermutation> ptPermutations;  // 0x50
    SoundType eType;  // 0x58
    int32 nInstanceLimit;  // 0x5c
    InstanceLimitBehavior eLimitBehavior;  // 0x60
    XPtr64<DspEffect> arDspEffects;  // 0x68
    SerializeData serDspEffects;  // 0x70
    XPtr64<RTPCParams> arRTPCParams;  // 0x78
    SerializeData serRTPCParams;  // 0x80
    uint32 dwCrossFadeTime;  // 0x88
    uint32 dwCooldownTime;  // 0x8c
    SoundPlayHistory tSoundPlayHistory;  // 0x90
};
static_assert(sizeof(SoundDefinition) == 0xa8, "SoundDefinition");

class SoundGroupDef : public GroupDefinition {  // size 0xa0, line 321
public:
    SoundGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 326; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 329
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 331
        return SNOPACKTYPE_SOUND;
    }
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    void PreloadHighestPermutation(const SoundDefinition * pdefSound) const;
};
static_assert(sizeof(SoundGroupDef) == 0xa0, "SoundGroupDef");

extern SoundGroupDef * gptGroupDefSound;  // SoundDefinition.cpp:190

extern const XTypeDescriptor * DT_DSP_PARAM;  // SoundDefinition.cpp:73
extern const XTypeDescriptor * DT_DSP_EFFECT;  // SoundDefinition.cpp:133
extern const XTypeDescriptor * DT_RTPC_PARAMS;  // SoundDefinition.cpp:140
extern const XTypeDescriptor * DT_SOUNDPERMUTATION;  // SoundDefinition.cpp:158
extern const XTypeDescriptor * DT_SOUND;  // SoundDefinition.cpp:184

// Defined in SoundDefinition.cpp:659; where it was declared is not in the DWARF.
BOOL SoundIsConversation(const SoundDefinition * pdefSound);

typedef ScopedDefinitionLockR<SoundDefinition> ScopedSoundDefinitionLockR;

template <> inline const SoundDefinition * DefinitionLockR<SoundDefinition>(SNO sno) {  // line 385
    return (const SoundDefinition *)gptGroupDefSound->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<SoundDefinition>(const SoundDefinition * pdef) {  // line 386
    void * lpDef = (void *)pdef;
    gptGroupDefSound->UnlockR(lpDef);
}
