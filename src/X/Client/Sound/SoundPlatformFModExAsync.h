// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Sound/SoundPlatformFModExAsync.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod_common.h"
#include "../Definitions/MusicDefinition.h"
#include "../RActors/RActors.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Math/Vector.h"

struct FModCachedSound;
struct SoundInstance;
typedef int32 SoundID;

enum {
    ASYNCAUDIO_READY_CHANNEL = -1,
    ASYNCAUDIO_PENDING_CHANNEL = -2,
    ASYNCAUDIO_CANCELLED_CHANNEL = -3,
};

class AsyncAudio {  // size 0x130
public:
    AsyncAudio();
    ~AsyncAudio();
    void QueueSound(FModCachedSound * ptCachedSound, SoundInstance * ptSound, const SNO snoSoundBank, int32 nWhichChannel, int32 nSampleIndex);
    void QueueMusic(FModCachedSound * ptCachedSound, SoundInstance * ptSound, PlaylistEntry & tEntry, const uint32 dwStartSample, const float flVolume);
    void PreloadSound(FModCachedSound * ptCachedSound);
    void Cancel(SoundInstance * ptSound);
    void SetPosition(SoundInstance * ptSound, const Vector3D & wp);
    void SetVelocity(SoundInstance * ptSound, const Vector3D & wvVel);
    void SetPitch(SoundInstance * ptSound, const float flPitch);
    void SetPaused(SoundInstance * ptSound, BOOL fPaused);
    void SetVolume(SoundInstance * ptSound, const float flVolume);
    void SetOcclusion(SoundInstance * ptSound, const float flOcclusion);
    void SetStreamPosition(SoundInstance * ptSound, const uint32 dwPos);
    void ProcessPendingSounds();
    void ReleaseFModSound(FMOD_SOUND * ptFModSound);
    BOOL IsPending(const SoundInstance * ptSound);
    BOOL IsCancelled(const SoundInstance * ptSound);
    BOOL AnyPending();
    void Dump();
    enum QueuedAPICallTypes {
        QueuedAPICallType_QueueSound = 0,
        QueuedAPICallType_PreloadSound = 1,
        QueuedAPICallType_ReleaseFModSound = 2,
    };
    enum QueuedMusicAPICallTypes {
        QueuedMusicAPICallType_QueueMusic = 0,
    };
    struct Queue_Params {  // size 0x10
        FModCachedSound * ptCachedSound;  // 0x00
        int32 nChannelIndex;  // 0x08
    };
    struct QueueSound_Params : public AsyncAudio::Queue_Params {  // size 0x18
        SNO snoSoundBank;  // 0x10
        int32 nSampleIndex;  // 0x14
    };
    struct PreloadSound_Params {  // size 0x8
        FModCachedSound * ptCachedSound;  // 0x00
    };
    struct QueueMusic_Params : public AsyncAudio::Queue_Params {  // size 0x68
        PlaylistEntry tEntry;  // 0x10
        uint32 dwStartSample;  // 0x5c
        float flVolume;  // 0x60
    };
    struct SetPosition_Params {  // size 0xc
        Vector3D wp;  // 0x00
    };
    struct SetVelocity_Params {  // size 0xc
        Vector3D wvVel;  // 0x00
    };
    struct SetPitch_Params {  // size 0x4
        float flPitch;  // 0x00
    };
    struct SetPaused_Params {  // size 0x4
        BOOL fPaused;  // 0x00
    };
    struct SetVolume_Params {  // size 0x4
        float flVolume;  // 0x00
    };
    struct SetOcclusion_Params {  // size 0x4
        float flOcclusion;  // 0x00
    };
    struct SetStreamPosition_Params {  // size 0x4
        uint32 dwPos;  // 0x00
    };
    enum {
        DELAYED_SET_POSITION_BIT = 0,
        DELAYED_SET_VELOCITY_BIT = 1,
        DELAYED_SET_PITCH_BIT = 2,
        DELAYED_SET_PAUSED_BIT = 3,
        DELAYED_SET_VOLUME_BIT = 4,
        DELAYED_SET_OCCLUSION_BIT = 5,
        DELAYED_SET_STREAM_POSITION_BIT = 6,
    };
    struct DelayedSettings {  // size 0x30
        BOOL dwFlags;  // 0x00
        AsyncAudio::SetPosition_Params posParam;  // 0x04
        AsyncAudio::SetVelocity_Params velParam;  // 0x10
        AsyncAudio::SetPitch_Params pitchParam;  // 0x1c
        AsyncAudio::SetPaused_Params pausedParam;  // 0x20
        AsyncAudio::SetVolume_Params volParam;  // 0x24
        AsyncAudio::SetOcclusion_Params occParam;  // 0x28
        AsyncAudio::SetStreamPosition_Params streamPosParam;  // 0x2c
    };
    struct ReleaseFModSound_Params {  // size 0x10
        FMOD_SOUND * ptFModSound;  // 0x00
        uint32 nFrame;  // 0x08
    };
    struct QueuedAPICall {  // size 0x30
        AsyncAudio::QueuedAPICallTypes type;  // 0x00
        SoundInstance * ptSound;  // 0x08
        SoundID idSound;  // 0x10
        union {  // size 0x18
            AsyncAudio::Queue_Params queue_Params;  // 0x00
            AsyncAudio::QueueSound_Params queueSound_Params;  // 0x00
            AsyncAudio::PreloadSound_Params preloadSound_Params;  // 0x00
            AsyncAudio::ReleaseFModSound_Params releaseFModSound_Params;  // 0x00
        };  // 0x18
    };
    struct QueuedMusicAPICall {  // size 0x80
        AsyncAudio::QueuedMusicAPICallTypes type;  // 0x00
        SoundInstance * ptSound;  // 0x08
        SoundID idSound;  // 0x10
        union {  // size 0x68
            AsyncAudio::Queue_Params queue_Params;  // 0x00
            AsyncAudio::QueueMusic_Params queueMusic_Params;  // 0x00
        };  // 0x18
    };
private:
    AsyncAudio::DelayedSettings & GetDelayedSetting(SoundInstance * ptSound);
    void QueueAPICall(const AsyncAudio::QueuedAPICall & call);
    void QueueMusicAPICall(const AsyncAudio::QueuedMusicAPICall & call);
    XGrowableMap<SoundInstance *, SoundInstance *, AsyncAudio::DelayedSettings, const AsyncAudio::DelayedSettings &> m_delayedSettings;  // 0x00
    XGrowableList<AsyncAudio::QueuedAPICall, const AsyncAudio::QueuedAPICall &> m_queuedCalls;  // 0x80
    XGrowableList<AsyncAudio::QueuedMusicAPICall, const AsyncAudio::QueuedMusicAPICall &> m_queuedMusicCalls;  // 0xd8
};
static_assert(sizeof(AsyncAudio) == 0x130, "AsyncAudio");


// Functions defined in this file (bodies are not in the DWARF):
//   line    82  BOOL AsyncAudio::IsPending(const SoundInstance * ptSound)  [inlined into 12 functions]
//   line    93  BOOL AsyncAudio::IsCancelled(const SoundInstance * ptSound)  [inlined into 2 functions]
//   line   104  BOOL AsyncAudio::AnyPending()  [inlined into 1 functions]
//   line   149  AsyncAudio::SetPosition_Params & AsyncAudio::SetPosition_Params::operator=(const AsyncAudio::SetPosition_Params &)  [inlined into 1 functions]
//   line   154  AsyncAudio::SetVelocity_Params & AsyncAudio::SetVelocity_Params::operator=(const AsyncAudio::SetVelocity_Params &)  [inlined into 1 functions]
//   line   195  AsyncAudio::DelayedSettings & AsyncAudio::DelayedSettings::operator=(const AsyncAudio::DelayedSettings &)  [inlined into 1 functions]
