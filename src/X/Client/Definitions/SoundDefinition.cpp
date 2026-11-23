// Original file: X/Client/Definitions/SoundDefinition.cpp

#include "SoundDefinition.h"

#include "SoundBankDefinition.h"
// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_TIME;
extern const XTypeDescriptor * DT_IMPULSE;

// Defined in X/Client/InterpolationScalar.cpp:31; no adopted header declares it yet.
extern const XTypeDescriptor * DT_INTERPOLATION_SCALAR;

// Defined in X/Common/SNO/SymbolicValue.cpp:4069; no adopted header declares it yet.
extern const SymbolicEntry gDspTypes[11];

// Defined in X/Common/SNO/SNOFiles.cpp:5565; no adopted header declares it yet.
BOOL SNOFileExists(const SNOName & snoname);

// Defined in X/Client/Sound/SoundUtils.cpp:207; X/Client/Sound/SoundUtils.h is not adopted.
StreamMode SoundShouldBeStreamed(const SoundDefinition * pdefSound);

// X/Client/Sound/SoundPlatform.h (not adopted). Only the static member called here is declared;
// it is defined in X/Client/Sound/SoundPlatformFModEx.cpp.
class SoundPlatform {
public:
    static void Preload(SNO snoSoundBank, int32 nSampleIndex);
};

enum {
    SOUND_COMMON_SNO_VERSION = 67,
    SOUND_XML_VERSION = 8,
    SOUND_PERMUTATION_VERSION = 1,
    SOUND_PC_SNO_VERSION = 67,
    SOUND_CONSOLE_SNO_VERSION = 70,
    SOUND_AXE_SNO_VERSION = 73,
    DSP_PARAM_XML_VERSION = 1,
};

static const int32 scnDefaultFlags = 0;  // line 46
static const float scflDefaultGlobalVolumeDB = -3.0f;  // line 47

// The field names are "" in the binary.
static const TypeDescriptorField DspParam_Fields[5] = {  // line 68
    TypeDescriptorField("", DT_FLOAT, offsetof(DspParam, flMinValue), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DspParam, flDeltaValue), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DspParam, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(DspParam, eSourceScaler), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DspParam), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DspParam("DspParam", DSP_PARAM_XML_VERSION, DspParam_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 73
const XTypeDescriptor * DT_DSP_PARAM = &TypeDescriptor_DspParam;

static const TypeDescriptorField DspEffect_Fields[48] = {  // line 75
    EnumTypeDescriptorField("", offsetof(DspEffect, eDspType), NULL, gDspTypes),
    TypeDescriptorField("", DT_INT, offsetof(DspEffect, dwFlags), NULL, 1),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusDryMix), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusWetMix1), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusWetMix2), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusWetMix3), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusDelay), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusRate), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusDepth), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tChorusFeedback), NULL, DSP_TYPE_CHORUS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tCompressorThreshold), NULL, DSP_TYPE_COMPRESSOR),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tCompressorAttack), NULL, DSP_TYPE_COMPRESSOR),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tCompressorRelease), NULL, DSP_TYPE_COMPRESSOR),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tCompressorGainMakeup), NULL, DSP_TYPE_COMPRESSOR),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tDistortionLevel), NULL, DSP_TYPE_DISTORTION),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tEchoDelay), NULL, DSP_TYPE_ECHO),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tEchoDecayRatio), NULL, DSP_TYPE_ECHO),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tEchoMaxChannels), NULL, DSP_TYPE_ECHO),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tEchoDryMix), NULL, DSP_TYPE_ECHO),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tEchoWetMix), NULL, DSP_TYPE_ECHO),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tFlangeDryMix), NULL, DSP_TYPE_FLANGE),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tFlangeWetMix), NULL, DSP_TYPE_FLANGE),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tFlangeDepth), NULL, DSP_TYPE_FLANGE),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tFlangeRate), NULL, DSP_TYPE_FLANGE),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tHighpassCutoff), NULL, DSP_TYPE_HIGHPASS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tHighpassResonance), NULL, DSP_TYPE_HIGHPASS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tLowpassCutoff), NULL, DSP_TYPE_LOWPASS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tLowpassResonance), NULL, DSP_TYPE_LOWPASS),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tLowpassSimpleCutoff), NULL, DSP_TYPE_LOWPASS_SIMPLE),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tParamEqCenter), NULL, DSP_TYPE_PARAMEQ),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tParamEqBandwidth), NULL, DSP_TYPE_PARAMEQ),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tParamEqGain), NULL, DSP_TYPE_PARAMEQ),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbDryLevel), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbRoom), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbRoomHF), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbRoomRolloffFactor), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbDecayTime), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbDecayHFRatio), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbReflectionsLevel), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbReflectionsDelay), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbReverbLevel), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbReverbDelay), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbDiffusion), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbDensity), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbHFReference), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbRoomLF), NULL, DSP_TYPE_SFXREVERB),
    VariantTypeDescriptorField("", DT_DSP_PARAM, offsetof(DspEffect, tReverbLFReference), NULL, DSP_TYPE_SFXREVERB),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DspEffect), NULL, 0),
};
VariantTypeDescriptor TypeDescriptor_DspEffect("DspEffect", 0, DspEffect_Fields, 48, NULL, 0x80, NULL, NULL, NULL, offsetof(DspEffect, eDspType));  // line 133
const XTypeDescriptor * DT_DSP_EFFECT = &TypeDescriptor_DspEffect;

static const TypeDescriptorField RTPCParams_Fields[5] = {  // line 135
    TypeDescriptorField("", DT_INT, offsetof(RTPCParams, eTarget), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RTPCParams, eCurveShape), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(RTPCParams, flCurveShapeAccel), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_INTERPOLATION_SCALAR, offsetof(RTPCParams, tScalar), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RTPCParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RTPCParams("RTPCParams", 0, RTPCParams_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 140
const XTypeDescriptor * DT_RTPC_PARAMS = &TypeDescriptor_RTPCParams;

static const TypeDescriptorField SoundPermutation_Fields[15] = {  // line 142
    FilterTypeDescriptorField("", DT_SNO, offsetof(SoundPermutation, snoSoundBank), "", SOUNDBANK_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SoundPermutation, dwSampleHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SoundPermutation, dwWeight), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundPermutation, flVolumeDBMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundPermutation, flVolumeDBDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundPermutation, flPitchSemitonesMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundPermutation, flPitchSemitonesDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(SoundPermutation, dwFadeInTime), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(SoundPermutation, dwFadeOutTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SoundPermutation, eFadeInShape), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SoundPermutation, eFadeOutShape), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundPermutation, flFadeInAccel), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundPermutation, flFadeOutAccel), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(SoundPermutation, dwDelay), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SoundPermutation), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SoundPermutation("SoundPermutation", SOUND_PERMUTATION_VERSION, SoundPermutation_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 158
const XTypeDescriptor * DT_SOUNDPERMUTATION = &TypeDescriptor_SoundPermutation;

static const TypeDescriptorField SoundDefinition_Fields[27] = {  // line 160
    TypeDescriptorField("", DT_INT, offsetof(SoundDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(SoundDefinition, dwPriority), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, wdFalloffRadiusMin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, wdFalloffRadiusDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flRolloffFactor), (const void *)0x3e99999a, 1),
    TypeDescriptorField("", DT_INT, offsetof(SoundDefinition, eFalloffType), (const void *)FALLOFF_INVERSE_DISTANCE, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flDopplerFactor), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flDuckAmountDB), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flGlobalVolumeDB), (const void *)0xc0400000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flGlobalPitch), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(SoundDefinition, flImpulseMin), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(SoundDefinition, flImpulseDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flImpulseAttenMinDB), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(SoundDefinition, flImpulseAttenMaxDB), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SoundDefinition, serPermutations), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SOUNDPERMUTATION, offsetof(SoundDefinition, ptPermutations), 0x21,
                                     (int32)offsetof(SoundDefinition, serPermutations) - (int32)offsetof(SoundDefinition, ptPermutations)),
    VariableArrayCountTypeDescriptorField("", offsetof(SoundDefinition, nPermutationCount), 0x10000, DT_SOUNDPERMUTATION,
                                          (int32)offsetof(SoundDefinition, serPermutations) - (int32)offsetof(SoundDefinition, nPermutationCount)),
    TypeDescriptorField("", DT_INT, offsetof(SoundDefinition, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SoundDefinition, nInstanceLimit), (const void *)1, 1),
    TypeDescriptorField("", DT_INT, offsetof(SoundDefinition, eLimitBehavior), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(SoundDefinition, dwCrossFadeTime), (const void *)120, 1),
    TypeDescriptorField("", DT_TIME, offsetof(SoundDefinition, dwCooldownTime), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SoundDefinition, serDspEffects), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_DSP_EFFECT, offsetof(SoundDefinition, arDspEffects), 0x21,
                                     (int32)offsetof(SoundDefinition, serDspEffects) - (int32)offsetof(SoundDefinition, arDspEffects)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SoundDefinition, serRTPCParams), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_RTPC_PARAMS, offsetof(SoundDefinition, arRTPCParams), 0x21,
                                     (int32)offsetof(SoundDefinition, serRTPCParams) - (int32)offsetof(SoundDefinition, arRTPCParams)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SoundDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SoundDefinition("Sound", SOUND_XML_VERSION, SoundDefinition_Fields, 27, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 184
const XTypeDescriptor * DT_SOUND = &TypeDescriptor_SoundDefinition;

SoundGroupDef * gptGroupDefSound;  // line 190

// line 196
SoundGroupDef::SoundGroupDef()
    : GroupDefinition("Sound", SOUND_SNO,
                      SNOVersions(SOUND_PC_SNO_VERSION, 75, SOUND_CONSOLE_SNO_VERSION, SOUND_CONSOLE_SNO_VERSION, 71),
                      SNODATATYPE_TITLE, sizeof(SoundDefinition), DT_SOUND, 1, 0x140a) {
}

// line 219
uint32 SoundGroupDef::GetMaxCount() {
    return 128;
}

static const float SOUND_DEFAULT_FALLOFF_RADIUS_MIN = 10.0f;  // line 227
static const float SOUND_DEFAULT_FALLOFF_RADIUS_MAX = 80.0f;  // line 228
static const float SOUND_DEFAULT_ROLLOFF = 1.0f;  // line 229

// line 231
void SoundGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(SoundDefinition));

    SoundDefinition * pdef = (SoundDefinition *)ptDefaultDef;
    pdef->dwFlags = scnDefaultFlags;
    pdef->dwPriority = PRIORITY_LOWEST;
    pdef->wdFalloffRadiusMin = SOUND_DEFAULT_FALLOFF_RADIUS_MIN;
    pdef->wdFalloffRadiusDelta = SOUND_DEFAULT_FALLOFF_RADIUS_MAX - SOUND_DEFAULT_FALLOFF_RADIUS_MIN;
    pdef->flRolloffFactor = SOUND_DEFAULT_ROLLOFF;
    pdef->eFalloffType = FALLOFF_INVERSE_DISTANCE;
    pdef->nInstanceLimit = 1;
    pdef->eLimitBehavior = ILB_FAIL;
    pdef->dwCrossFadeTime = 120;
    pdef->flGlobalVolumeDB = scflDefaultGlobalVolumeDB;
}

// line 252
void SoundGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    SoundDefinition * pdef = (SoundDefinition *)lpDefinition;
    BOOL bIsConversation = SoundIsConversation(pdef);

    SigmaMemoryClear(&pdef->tSoundPlayHistory, sizeof(SoundPlayHistory));

    for (int32 i = 0; i < pdef->nPermutationCount; i++) {
        BOOL bSuccess = FALSE;
        SoundPermutation & tPermutation = pdef->ptPermutations[i];

        if (SNOFileExists(SNOName(SOUNDBANK_SNO, tPermutation.snoSoundBank))) {
            tPermutation.nSampleIndex = SoundBankSampleIndexFromHash(tPermutation.snoSoundBank, tPermutation.dwSampleHash);
            if (tPermutation.nSampleIndex != -1) {
                tPermutation.snoValidatedSoundBank = tPermutation.snoSoundBank;
                bSuccess = TRUE;
            } else if (bIsConversation) {
                tPermutation.nSampleIndex = 0;
                tPermutation.snoValidatedSoundBank = SNO_NONE;
                bSuccess = TRUE;
            }
        }

        if (!bSuccess) {
            tPermutation.snoValidatedSoundBank = gptGroupDefSoundBank->GetFallbackDataSNO();
            tPermutation.nSampleIndex = 0;
        }
    }

    if (SoundShouldBeStreamed(pdef) == 0)
        PreloadHighestPermutation(pdef);
}

// line 320
void SoundGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}

// line 659
BOOL SoundIsConversation(const SoundDefinition * pdefSound) {
    return (pdefSound->dwFlags >> SOUND_IS_CONVERSATION) & 1;
}

// line 699
inline void SoundGroupDef::PreloadHighestPermutation(const SoundDefinition * pdefSound) const {
    int32 nHighestPermutation = -1;
    uint32 dwHighestWeight = 0;
    for (int32 i = 0; i < pdefSound->nPermutationCount; i++) {
        if (pdefSound->ptPermutations[i].dwWeight > dwHighestWeight) {
            dwHighestWeight = pdefSound->ptPermutations[i].dwWeight;
            nHighestPermutation = i;
        }
    }

    if (nHighestPermutation != -1) {
        const SoundPermutation & tPermutation = pdefSound->ptPermutations[nHighestPermutation];
        SoundPlatform::Preload(tPermutation.snoValidatedSoundBank, tPermutation.nSampleIndex);
    }
}
