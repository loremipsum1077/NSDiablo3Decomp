// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Sound/SoundPlatform.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 SoundFormat;

typedef int32 StreamMode;

typedef int32 SoundCompression;

typedef int32 SpeakerModes;

typedef int32 InstanceLimitBehavior;


// The SoundPlatform namespace (defined in X/Client/Sound/SoundPlatformFModEx.cpp). The DWARF
// skeleton of this header carries only the typedefs; the declarations come from the
// definitions' DWARF records (SoundPlatformFModEx.cpp lines in the comments).
#include "Sound.h"

struct PlaylistEntry;
struct ReverbParams;
struct ReverbParamsExtended;
struct SoundBankDefinition;
struct SoundDefinition;
struct SoundInstance;
struct SoundOVT;
struct SoundPlaybackParams;
struct SoundSample;
struct Vector3D;

namespace SoundPlatform {
BOOL ResetDevice();  // 1802
void ReleaseMemory();  // 1815
BOOL Initialize(const SoundFormat eSoundFormat, const int32 nNumChannels, const uint32 dwDriverHash, const SpeakerModes eSpeakerMode);  // 1899
void Dispose();  // 2314
void Update();  // 2863
void SetListener(const Vector3D & wpVolume, const Vector3D & wpPanning, const Vector3D & wvVel, const Vector3D & wvView, const Vector3D & wvUp);  // 2963
void SetListenerParams(const float flRolloff, const float flDopplerFactor);  // 2982
void OnBankLoaded(const SoundBankDefinition *);  // 2993
void OnBankUnloaded(const SoundBankDefinition * ptSoundBankDef);  // 2999
void OnBankPurge(SNO snoSoundBank);  // 3006
BOOL SetRetroFilter(const BOOL fEnable);  // 3077
void SetCustomListenerMode(const BOOL fVal);  // 3996
BOOL InCustomListenerMode();  // 4008
void Preload(const SNO snoSoundBank, int32 nSampleIndex);  // 4074
BOOL IsPreloaded(const SNO snoSoundBank, int32 nSampleIndex);  // 4107
BOOL IsReady(const SNO snoSoundBank, int32 nSampleIndex);  // 4150
BOOL Play(SoundInstance * ptSound, const SNO snoSoundBank, SoundSample & tSoundSample, SoundPlaybackParams & tParams);  // 4174
BOOL PlayMusicEntry(SoundInstance * ptSound, PlaylistEntry & tEntry, const uint32 dwStartSample, const float flVolume);  // 4452
BOOL OpenOVTStream(SoundOVT * ptSoundOVT);  // 4527
void PlayOVTStream(const float flVolume);  // 4565
void StopOVTStream();  // 4589
BOOL GetOVTStreamPos(float & flStreamPos);  // 4615
void Stop(SoundInstance * ptSound);  // 4666
void SetPosition(SoundInstance * ptSound, const Vector3D & wp);  // 4673
void SetVelocity(SoundInstance * ptSound, const Vector3D & wvVel);  // 4704
void SetPaused(SoundInstance * ptSound, BOOL fPaused);  // 4735
void SetPitch(SoundInstance * ptSound, const float flPitch);  // 4762
void SetVolume(SoundInstance * ptSound, const float flVolume);  // 4811
void SetReverbParams(const ReverbParams & tReverb, const ReverbParamsExtended * ptReverbExtended);  // 5036
void SetOcclusion(SoundInstance * ptSound, const float flOcclusion);  // 5056
void SetReverbParamsFromPreset(ReverbParams & tReverb, SoundReverbPreset ePreset, ReverbParamsExtended * ptReverbExtended);  // 5081
uint32 GetStreamPosition(const SoundInstance * ptSound);  // 5120
void SetStreamPosition(SoundInstance * ptSound, const uint32 dwPos);  // 5145
int32 GetSoundInstanceCount(const SoundDefinition * pdefSound, const Vector3D * pwp, SoundID & idSoundToSteal);  // 5164
}  // namespace SoundPlatform
