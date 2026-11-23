// Adopted from the DWARF skeleton (build/dwarf/headers/X/Client/Sound/Sound.h);
// hand-maintained from here on.
// Original file: X/Client/Sound/Sound.h
//
// SoundManager::PlaySound is declared at the bottom because its definition lives
// in X/Client/Sound/Sound.cpp (_ZN12SoundManager9PlaySoundEiRK16PlaySoundOptions);
// the skeleton carried only the POD layouts. Its namespace scope and signature come
// from the DWARF record at Sound.cpp:1810.
//
// SoundManager::ListenerSet and SoundManager::ListenerSetParams are declared here for
// the same reason: their definitions live in X/Client/Sound/Sound.cpp (relocations
// 0x7100238ea0 and 0x71002390a0) and this header is the DWARF owning header of the
// SoundManager namespace. Additive: no existing declaration or layout changed.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../AttachmentParent.h"
#include "../Definitions/SoundBankDefinition.h"
#include "../Definitions/SoundDefinition.h"
#include "../Gfx/GfxCommon.h"
#include "../RActors/RActors.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

typedef int32 FalloffType;

enum {
    BAC_BACKGROUND_AMBIENT = 0,
    BAC_WEATHER = 1,
    NUM_BACKGROUND_AMBIENT_CONTROLLERS = 2,
};

enum {
    SOUNDINSTANCE_3D_BIT = 0,
    SOUNDINSTANCE_ENABLE_REVERB_BIT = 1,
    SOUNDINSTANCE_OCCLUDED_BIT = 2,
    SOUNDINSTANCE_IS_MOVING_BIT = 3,
    SOUNDINSTANCE_AFFECTED_BY_WEATHER_BIT = 4,
    SOUNDINSTANCE_RANDOM_START_POSITION_BIT = 5,
    SOUNDINSTANCE_HAS_PLAYED_BIT = 6,
    SOUNDINSTANCE_IS_MUSIC_BIT = 7,
    SOUNDINSTANCE_DONT_DUCK_BIT = 8,
};

enum SoundReverbPreset {
    SOUND_REVERB_EAX_PRESET_OFF = 0,
    SOUND_REVERB_EAX_PRESET_GENERIC = 1,
    SOUND_REVERB_EAX_PRESET_PADDEDCELL = 2,
    SOUND_REVERB_EAX_PRESET_ROOM = 3,
    SOUND_REVERB_EAX_PRESET_BATHROOM = 4,
    SOUND_REVERB_EAX_PRESET_LIVINGROOM = 5,
    SOUND_REVERB_EAX_PRESET_STONEROOM = 6,
    SOUND_REVERB_EAX_PRESET_AUDITORIUM = 7,
    SOUND_REVERB_EAX_PRESET_CONCERTHALL = 8,
    SOUND_REVERB_EAX_PRESET_CAVE = 9,
    SOUND_REVERB_EAX_PRESET_ARENA = 10,
    SOUND_REVERB_EAX_PRESET_HANGAR = 11,
    SOUND_REVERB_EAX_PRESET_CARPETTEDHALLWAY = 12,
    SOUND_REVERB_EAX_PRESET_HALLWAY = 13,
    SOUND_REVERB_EAX_PRESET_STONECORRIDOR = 14,
    SOUND_REVERB_EAX_PRESET_ALLEY = 15,
    SOUND_REVERB_EAX_PRESET_FOREST = 16,
    SOUND_REVERB_EAX_PRESET_CITY = 17,
    SOUND_REVERB_EAX_PRESET_MOUNTAINS = 18,
    SOUND_REVERB_EAX_PRESET_QUARRY = 19,
    SOUND_REVERB_EAX_PRESET_PLAIN = 20,
    SOUND_REVERB_EAX_PRESET_PARKINGLOT = 21,
    SOUND_REVERB_EAX_PRESET_SEWERPIPE = 22,
    SOUND_REVERB_EAX_PRESET_UNDERWATER = 23,
    SOUND_REVERB_NUM_PRESETS = 24,
};

struct BkgrndAmbientState {  // size 0x8
    SNO snoSound;  // 0x00
    uint32 dwPos;  // 0x04
};
static_assert(sizeof(BkgrndAmbientState) == 0x8, "BkgrndAmbientState");

struct SavedMusicState {  // size 0x4
    SNO snoMusic;  // 0x00
};
static_assert(sizeof(SavedMusicState) == 0x4, "SavedMusicState");

struct ControllerStates {  // size 0x1c
    uint32 dwSuppressedTypes;  // 0x00
    SNO snoReverb;  // 0x04
    BkgrndAmbientState tState[2];  // 0x08
    SavedMusicState tMusicState;  // 0x18
};
static_assert(sizeof(ControllerStates) == 0x1c, "ControllerStates");

struct SoundPerfStats {  // size 0x4
    int32 nPlayingSounds;  // 0x00
};
static_assert(sizeof(SoundPerfStats) == 0x4, "SoundPerfStats");

enum {
    SOUNDID_INVALID = -1,
};

typedef int32 SoundID;

struct RolloffParams {  // size 0x10
    FalloffType eFalloffType;  // 0x00
    float flRolloffFactor;  // 0x04
    float flDistanceMin;  // 0x08
    float flDistanceMax;  // 0x0c
};
static_assert(sizeof(RolloffParams) == 0x10, "RolloffParams");

struct SoundPlaybackParams {  // size 0x28
    StreamMode eStreamMode;  // 0x00
    uint32 dwPriority;  // 0x04
    SoundType eType;  // 0x08
    float flVolume;  // 0x0c
    float flPitch;  // 0x10
    BOOL fLoops;  // 0x14
    BOOL fChooseNewPermutationOnLoop;  // 0x18
    BOOL f3D;  // 0x1c
    BOOL fDisableReverb;  // 0x20
    float flStartOffset;  // 0x24
};
static_assert(sizeof(SoundPlaybackParams) == 0x28, "SoundPlaybackParams");

struct SoundChannelState {  // size 0x8
    int32 nChannelIndex;  // 0x00
    SNO snoSoundBank;  // 0x04
    void Initialize() { nChannelIndex = -1; snoSoundBank = -1; }
    void Set(const int32 _nChannelIndex, const SNO _snoSoundBank) { nChannelIndex = _nChannelIndex; snoSoundBank = _snoSoundBank; }
};
static_assert(sizeof(SoundChannelState) == 0x8, "SoundChannelState");

struct SoundInstance {  // size 0x108
    struct SoundInstanceSoundParams {  // size 0x8
        SNO snoSound;  // 0x00
        int32 nPermutationIndex;  // 0x04
    };
    struct SoundInstanceMusicParams {  // size 0x14
        SNO snoMusic;  // 0x00
        int32 nPlaylistIndex;  // 0x04
        float flFadeInDuration;  // 0x08
        float flFadeOutTime;  // 0x0c
        float flFadeOutDuration;  // 0x10
    };
    SoundID id;  // 0x00
    uint32 dwFlags;  // 0x04
    SoundPlaybackParams tParams;  // 0x08
    union {  // size 0x14
        SoundInstance::SoundInstanceSoundParams tSoundParams;  // 0x00
        SoundInstance::SoundInstanceMusicParams tMusicParams;  // 0x00
    };  // 0x30
    bool fStitching : 1;  // 0x44
    bool fHasRTPC : 1;  // 0x44
    SoundChannelState tChannelState[2];  // 0x48
    int32 nCurrentChannel;  // 0x58
    Vector3D wp;  // 0x5c
    Vector3D wvVel;  // 0x68
    float flVolumeDB;  // 0x74
    float flPitchSemitones;  // 0x78
    float flRTPCAdjustment[2];  // 0x7c
    float flFadeAtten;  // 0x84
    SoundType eType;  // 0x88
    RenderContext eContext;  // 0x8c
    uint32 dwPriority;  // 0x90
    RolloffParams tRolloffParams;  // 0x94
    float flDopplerFactor;  // 0xa4
    float flDuckAmountDB;  // 0xa8
    uint32 dwStartTime;  // 0xac
    float flDelay;  // 0xb0
    float flFadeInTime;  // 0xb4
    float flFadeOutTime;  // 0xb8
    float flFadeInStartTime;  // 0xbc
    float flFadeOutStartTime;  // 0xc0
    float flWantedStartTime;  // 0xc4
    float flRealStartTime;  // 0xc8
    FadeShape eFadeInShape;  // 0xcc
    FadeShape eFadeOutShape;  // 0xd0
    float flFadeInAccel;  // 0xd4
    float flFadeOutAccel;  // 0xd8
    ParentData tParentData;  // 0xdc
    BOOL HasStarted() { return flRealStartTime != 0.0f; }  // line 218; body from UIConversation::Update (fcmp #0.0, b.eq)
    void SetParent(const ParentData & _tParentData);
    void UpdatePropertiesFromParent(const float flDeltaTime);
    void SetPosition(const Vector3D & _wp, const float flDeltaTime);
    void SetControlledVolumeAtten(const float flAtten);
    void SetPitchSemitones(const float _flPitchSemitones);
};
static_assert(sizeof(SoundInstance) == 0x108, "SoundInstance");

struct SoundOVT {  // size 0x38
    uint32 dwAudioHead;  // 0x00
    uint32 dwAudioTail;  // 0x04
    int16 * lpAudioBuffer;  // 0x08
    uint32 dwAudioPos;  // 0x10
    uint32 dwAudioBufferSize;  // 0x14
    uint32 dwFrequency;  // 0x18
    uint32 dwChannels;  // 0x1c
    uint32 dwFPS;  // 0x20
    uint32 dwFakeAudioPos;  // 0x24
    uint32 dwStartTick;  // 0x28
    uint32 nLastBufferPosPCMBytes;  // 0x2c
    uint32 nLastPosPCMBytes;  // 0x30
};
static_assert(sizeof(SoundOVT) == 0x38, "SoundOVT");

struct PlaySoundOptions {  // size 0x30
    PlaySoundOptions() : eContext(RENDERCONTEXT_DEFAULT), pwp(NULL), nPermutationIndex(-1),
        fForce2D(FALSE), fOccluded(FALSE), fAffectedByWeather(FALSE),
        flInitialFadeAtten(1.0f), flImpulse(1.0f), flStartOffset(0.0f) {}
    RenderContext eContext;  // 0x00
    const Vector3D * pwp;  // 0x08
    int32 nPermutationIndex;  // 0x10
    BOOL fForce2D;  // 0x14
    BOOL fOccluded;  // 0x18
    BOOL fAffectedByWeather;  // 0x1c
    float flInitialFadeAtten;  // 0x20
    float flImpulse;  // 0x24
    float flStartOffset;  // 0x28
};
static_assert(sizeof(PlaySoundOptions) == 0x30, "PlaySoundOptions");

// X/Client/Sound/Sound.cpp:50
extern const PlaySoundOptions cgtPSODefaults;

// X/Client/Sound/Sound.cpp:1810
//   _ZN12SoundManager9PlaySoundEiRK16PlaySoundOptions
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
void SetGlobalMute(const BOOL fMute);  // Sound.cpp:1269
BOOL GetGlobalMute();  // Sound.cpp:1283

// X/Client/Sound/Sound.cpp (relocations 0x7100238ea0 and 0x71002390a0).
//   _ZN12SoundManager11ListenerSetERK8Vector3DS2_S2_S2_
//   _ZN12SoundManager17ListenerSetParamsEff
void ListenerSet(const Vector3D & wpVolumePosition, const Vector3D & wpPanningPosition,
                 const Vector3D & wvView, const Vector3D & wvUp);
void ListenerSetParams(const float flRolloffFactor, const float flDopplerFactor);
void ListenerGetVolumePosition(Vector3D & wp);  // Sound.cpp:1224
void ListenerGetPanningPosition(Vector3D & wp);  // Sound.cpp:1233
}  // namespace SoundManager

// Functions defined in this file (bodies are not in the DWARF):
//   line   136  void SoundChannelState::Initialize()  [inlined into 3 functions]
//   line   143  void SoundChannelState::Set(const int32 _nChannelIndex, const SNO _snoSoundBank)  [inlined into 2 functions]
//   line   151  SoundInstance::SoundInstance()  [inlined into 1 functions]
//   line   218  BOOL SoundInstance::HasStarted()  [inlined into 1 functions]
//   line   268  PlaySoundOptions::PlaySoundOptions()  [inlined into 14 functions]
//   line   479  SoundInstance * SoundGet(SoundID id)  [inlined into 3 functions]

void SoundDelete(SoundID id);

extern TDataArray<SoundInstance, SoundID> * gptSounds;  // Sound.cpp:239.

#line 485
inline SoundInstance * SoundTryToGet(
    SoundID id)
{
    if (id == SOUNDID_INVALID || gptSounds == NULL)
    {
        return NULL;
    }

    return gptSounds->ElementTryToGet(id);
}
