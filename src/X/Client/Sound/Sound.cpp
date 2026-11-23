// X/Client/Sound/Sound.cpp
// Generated from build/try/r5c-sound/sound.cpp by build/try/r5-particle/gen.py.
// Functions sit on their original lines.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../RActors/RActors.h"
#include "../CGame.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../Definitions/ReverbDefinition.h"
#include "../Definitions/SoundDefinition.h"
#include "../Definitions/MusicDefinition.h"
#include "../../Common/GameConstants.h"
#include "../../Sigma/Random.h"
#include <math.h>
#include "Sound.h"
#include "SoundPlatform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../RWindow.h"
#include "../../Sigma/SigmaTiming.h"

// Declared here: X/Client/RWindow.cpp:40 has no header declaration.
RWindowID RWindowIDFromContext(const RenderContext eContext);

enum {
    MAX_SOUND_INSTANCES = 256,
};
const float FADE_MIN = 0.0001f;
















const CHAR * AUDIO_DIALOG_PREFIX;
const PlaySoundOptions cgtPSODefaults;
class ActivePlaylistEntry {  // size 0xc
public:
    ActivePlaylistEntry();
    ActivePlaylistEntry(const int32);
    int32 nPlaylistEntry;  // 0x00
    uint32 dwStartMachineTime;  // 0x04
    uint32 dwStartSample;  // 0x08
};

typedef blz::vector<ActivePlaylistEntry, blz::allocator<ActivePlaylistEntry> > PlaylistArray;

struct BkgrndAmbientController {  // size 0x20
    void Initialize(const BOOL fAffectedByWeather);
    void Reset();
    void Update(const float flDeltaTime);
    void GetCurrentState(BkgrndAmbientState & tState) const;
    void SetCurrentState(const BkgrndAmbientState & tState);
    SoundID Play(const SNO snoSound, const float flBlendTime, const BOOL fOverlay);
private:
    SNO m_snoSound;  // 0x00
    SNO m_snoSoundOverlay;  // 0x04
    SoundID m_idCurrent;  // 0x08
    SoundID m_idOld;  // 0x0c
    float m_flBlendTime;  // 0x10
    float m_flT;  // 0x14
    float m_flTimePlayedThisTrack;  // 0x18
    BOOL m_fAffectedByWeather;  // 0x1c
};

struct RandomMusicSoundState {  // size 0xc
    float m_flTimeDelay;  // 0x00
    float m_flTimeUntilRepeat;  // 0x04
    SoundID m_idSound;  // 0x08
};

class MusicState {  // size 0x58
public:
    void Initialize();
    void Dispose();
    void Stop();
    void StopRandomSounds();
    void CreatePlaylist(const int32 nFirstIndexToPlay, const BOOL fLooped);
    void StartSample();
    void Pause(const BOOL);
    void Update(const float flDeltaTime);
private:
    friend class MusicManager;
    SNO m_snoMusic;  // 0x00
    PlaylistArray m_arPlaylist;  // 0x08
    int32 m_nPlaylistIndex;  // 0x20
    uint32 m_dwPausedTime;  // 0x24
    uint32 m_dwPlaylistStartTime;  // 0x28
    uint32 m_dwEndMachineTime;  // 0x2c
    BOOL m_fDisableShuffle;  // 0x30
    BOOL m_fOverlay;  // 0x34
    int32 m_nNewEntryIndexOnLoop;  // 0x38
    SoundID m_idSound;  // 0x3c
    RandomMusicSoundState m_tRandomSoundState[2];  // 0x40
};

class MusicManager {  // size 0xd8
public:
    void Initialize();
    void Dispose();
    void Play(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle);
    void Stop();
    void StopOverlay();
    void Update(const float flDeltaTime);
    void Pause(const BOOL);
    void GetCurrentState(SavedMusicState & tSavedState) const;
    void SetCurrentState(const SavedMusicState & tSavedState);
private:
    MusicState m_tState[2];  // 0x00
    MusicState * m_ptStateCurrent;  // 0xb0
    MusicState * m_ptStateOld;  // 0xb8
    BOOL m_fPaused;  // 0xc0
    SNO m_snoOverlayReturnMusic;  // 0xc4
    SNO m_snoOverlay;  // 0xc8
    float m_flBlendTime;  // 0xcc
    float m_flT;  // 0xd0
    float m_flTimePlayedThisTrack;  // 0xd4
};

struct SoundGlobals {  // size 0x1b0
    BOOL fInitialized;  // 0x00
    BOOL fSpeakersReversed;  // 0x04
    Vector3D wpVolume;  // 0x08
    Vector3D wpPanning;  // 0x14
    Vector3D wvVel;  // 0x20
    Vector3D wvView;  // 0x2c
    Vector3D wvUp;  // 0x38
    float flRolloff;  // 0x44
    float flDopplerFactor;  // 0x48
    BOOL fGlobalMute;  // 0x4c
    float flMasterVolume;  // 0x50
    float flDuckedVolumeDB;  // 0x54
    float flDuckedVolumeAdjustedDB;  // 0x58
    BOOL fTypeMute[5];  // 0x5c
    float flTypeVolume[5];  // 0x70
    uint32 dwSuppressedTypes;  // 0x84
    SNO snoReverb;  // 0x88
    BkgrndAmbientController tBkgrndAmbController[2];  // 0x8c
    MusicManager tMusicMgr;  // 0xd0
    SoundPerfStats tStats;  // 0x1a8
    BOOL fAllowRetroFilter;  // 0x1ac
};
static_assert(sizeof(SoundGlobals) == 0x1b0, "SoundGlobals");

// X/Client/Sound/SoundUtils.cpp.
float SoundPitchSemitonesToLinear(const float flSemitones);
void SoundUtilsInitialize();
void SoundUtilsDispose();
float SoundBelToLinear(const int32 nBel);
float SoundGetAdjustedVolume(const SoundInstance * ptSound);

// X/Client/Weather.cpp:1570
namespace Weather {
float GetIntensity(const WeatherRWindowData & tData);
}  // namespace Weather

namespace SoundManager {
BOOL Initialize();
void SaveControllerStates(ControllerStates & tStates);
void RestoreControllerStates(const ControllerStates & tStates);
SoundID PlayEntry(const SNO snoMusic, const int32 nPlaylistIndex, const uint32 dwStartSample);
void ResetControllers();
void SetReverbParams(const SNO snoReverb);
void Dispose();
void ApplyPrefSettings();
void SetMasterVolume(const float flVolume);
void SetTypeMute(const SoundType eType, const BOOL fMute);
void SetTypeVolume(const SoundType eType, const float flVolume);
BOOL IsInitialized();
void DeleteAll(const uint32 dwSoundTypesToDelete, const RenderContext eContext);
void InitializeForGame();
void DisposeForGame();
void SuppressSounds(const uint32 dwSuppressedTypes);
void ListenerGetVolumePosition(Vector3D & wp);
void ListenerGetPanningPosition(Vector3D & wp);
void ListenerGetAxes(Vector3D & wvView, Vector3D & wvUp, Vector3D & wvRight);
BOOL GetGlobalMute();
float GetTypeVolume(const SoundType eType);
SoundID PlayBkgrndAmbient(const SNO snoSound, const int32 nBkgrndAmbientController, const float flBlendTime, const BOOL fOverlay);
BOOL OpenOVTStream(SoundOVT * ptSoundOVT);
void PlayOVTStream();
void StopOVTStream();
BOOL GetOVTStreamPos(float & flStreamPos);
void SetCustomListenerMode(const BOOL fVal);
void PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle);
void StopMusic();
void StopMusicOverlay();
}

void SoundFadeOut(const SoundID idSound);
void SoundSetPaused(const SoundID idSound, BOOL fPaused);
float SoundGetAdjustedPitch(const SoundInstance * ptSound);































static SoundGlobals * sgptGlobals;
TDataArray<SoundInstance, int> * gptSounds;

// X/Client/Sound/Sound.h:485 (written here until Sound.h carries it; SoundPlatformFModEx.cpp
// has the same inline copy).
inline SoundInstance * SoundGet(SoundID id)  // Sound.h:479
{
    return gptSounds->ElementGet(id);
}

















inline void BkgrndAmbientController::Initialize(const BOOL fAffectedByWeather)
{
    m_snoSound = SNO_NONE;
    m_snoSoundOverlay = SNO_NONE;
    m_idCurrent = SOUNDID_INVALID;
    m_idOld = SOUNDID_INVALID;
    m_flBlendTime = 0.0f;
    m_flT = 0.0f;
    m_fAffectedByWeather = fAffectedByWeather;
}





void BkgrndAmbientController::Reset()

{
    SoundDelete(m_idCurrent);
    SoundDelete(m_idOld);
    // DWARF Initialize inlined at 285; arg inferred (its m_fAffectedByWeather store is dead)
    Initialize(m_fAffectedByWeather);
}




inline void BkgrndAmbientController::GetCurrentState(BkgrndAmbientState & tState) const
{
    tState.snoSound = m_snoSound;
    SoundInstance * ptSound = SoundTryToGet(m_idCurrent);
    if (ptSound)
        tState.dwPos = SoundPlatform::GetStreamPosition(ptSound);
    else
        tState.dwPos = 0;
}










inline void BkgrndAmbientController::SetCurrentState(const BkgrndAmbientState & tState)
{

    if (tState.snoSound != SNO_NONE)
    {




        Play(tState.snoSound, 0.0f, FALSE);

        SoundInstance * ptSound = SoundTryToGet(m_idCurrent);
        if (ptSound)
        {
            SoundPlatform::SetStreamPosition(ptSound, tState.dwPos);
        }
    }
}


inline BOOL sSoundsMatch(const SoundID dataID, const SNO snoSound)
{


    SoundInstance * ptSound = SoundTryToGet(dataID);
    return (ptSound && ptSound->tSoundParams.snoSound == snoSound) ||
           (dataID == SOUNDID_INVALID && snoSound == SNO_NONE);
}









SoundID BkgrndAmbientController::Play(
    const SNO snoSound,
    const float flBlendTime,
    const BOOL fOverlay)
{
    if (!SoundManager::IsInitialized())
        return SOUNDID_INVALID;



    if (sSoundsMatch(m_idCurrent, snoSound))
        return m_idCurrent;






    if (flBlendTime != 0.0f)
    {
        if (sSoundsMatch(m_idOld, snoSound))
        {

            Math::Swap(m_idCurrent, m_idOld);
            m_flT = 1.0f - m_flT;
            return m_idCurrent;
        }


        if (m_idOld != SOUNDID_INVALID)
        {
            SoundInstance * ptSoundOld = SoundTryToGet(m_idOld);


            if (ptSoundOld)
            {
                SoundPlatform::Stop(ptSoundOld);
            }
            m_flT = 0.0f;
        }
    }
    // Inference: fCrossfade initialiser (DWARF local at 389; its fcmp is rematerialised at 392/421).
    BOOL fCrossfade = flBlendTime > 0.0f;
    // Inference: 1.0f / FADE_MIN split as decl + if (select row 392:9).
    float flFadeAtten = 1.0f;
    if (fCrossfade)
        flFadeAtten = FADE_MIN;



    SoundID dataID = SOUNDID_INVALID;
    if (snoSound != SNO_NONE)
    {
        const int32 nPermutationIndex = -1;  // Inference: value from the stored constant (403:36)

        PlaySoundOptions tOptions;
        tOptions.nPermutationIndex = nPermutationIndex;
        tOptions.fForce2D = TRUE;
        tOptions.fAffectedByWeather = m_fAffectedByWeather;
        tOptions.flInitialFadeAtten = flFadeAtten;
        tOptions.flImpulse = 0.0f;
        dataID = SoundManager::PlaySound(snoSound, tOptions);
    }

    m_snoSoundOverlay = SNO_NONE;
    if (fOverlay)
        m_snoSoundOverlay = snoSound;
    else
        m_snoSound = snoSound;





    if (fCrossfade)
    {
        m_flBlendTime = flBlendTime;
    }
    else
    {
        m_flBlendTime = 0.0f;


        SoundInstance * ptSoundCurrent = SoundTryToGet(m_idCurrent);
        if (ptSoundCurrent)
        {
            SoundPlatform::Stop(ptSoundCurrent);
        }
        m_idCurrent = SOUNDID_INVALID;
    }

    m_idOld = m_idCurrent;
    m_idCurrent = dataID;
    m_flTimePlayedThisTrack = 0.0f;

    return dataID;
}






void BkgrndAmbientController::Update(const float flDeltaTime)
{
    SoundInstance * ptCurrent = SoundTryToGet(m_idCurrent);
    SoundInstance * ptOld = SoundTryToGet(m_idOld);


    if (m_snoSound != SNO_NONE && !ptCurrent && !ptOld)
    {


        Play(m_snoSound, 0.0f, FALSE);
        return;
    }

    m_flTimePlayedThisTrack += flDeltaTime;

    if (m_snoSoundOverlay != SNO_NONE)
    {
        BOOL fRestart = TRUE;


        if (ptCurrent)
        {
            ScopedSoundDefinitionLockR pdefSound(m_snoSoundOverlay);





            const SoundPermutation & tPermutation = pdefSound->ptPermutations[ptCurrent->tSoundParams.nPermutationIndex];
            ScopedSoundBankDefinitionLockR pdefSoundBank(tPermutation.snoValidatedSoundBank);
            const SoundSample & tSample = pdefSoundBank->ptSoundSamples[tPermutation.nSampleIndex];
            float flDuration = tSample.ComputeDuration() * OO_TICKS_PER_SECOND;

            fRestart = m_flTimePlayedThisTrack > flDuration - 0.25f;
        }







        if (fRestart)
            Play(m_snoSound, 0.25f, FALSE);
    }


    if (m_flBlendTime != 0.0f)
    {

        m_flT += (1.0f / m_flBlendTime) * flDeltaTime;

        if (ptCurrent)

            ptCurrent->flFadeAtten = Math::Pin(m_flT, FADE_MIN, 1.0f);

        if (ptOld)

            ptOld->flFadeAtten = Math::Pin(1.0f - m_flT, 0.0f, 1.0f);

        if (m_flT >= 1.0f)
        {

            m_flBlendTime = 0.0f;
            m_flT = 0.0f;

            SoundInstance * ptSound = SoundTryToGet(m_idOld);

            if (ptSound)
                SoundPlatform::Stop(ptSound);

            m_idOld = SOUNDID_INVALID;
        }
    }
}

BOOL SoundManager::Initialize()

{


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_NO_SOUND_BIT))
        return FALSE;







    sgptGlobals = (SoundGlobals *)SigmaMemoryNew(sizeof(SoundGlobals), 4, NULL, TRUE);

    int32 nNumChannels = Prefs::GetSoundPrefs().GetChannelsToUse();
    SpeakerModes eSpeakerMode = Prefs::GetSoundPrefs().GetSpeakerMode();
    uint32 dwDriverHash = Prefs::GetSoundPrefs().GetDriverHash();






    sgptGlobals->fInitialized = SoundPlatform::Initialize(SOUNDFORMAT_STEREO_44KHz_16BIT, nNumChannels, dwDriverHash, 5);
    if (!sgptGlobals->fInitialized)
        return FALSE;




    SoundUtilsInitialize();


    sgptGlobals->flMasterVolume = 1.0f;
    for (int32 i = 0; i < NUMBER_OF_SOUND_TYPES; i++)
    {
        sgptGlobals->flTypeVolume[i] = 1.0f;
    }
    sgptGlobals->flDuckedVolumeDB = 0.0f;  // Inference: line (store merged into 562's row)
    sgptGlobals->flDuckedVolumeAdjustedDB = 0.0f;

    sgptGlobals->flRolloff = 1.0f;
    sgptGlobals->flDopplerFactor = 1.0f;

    sgptGlobals->snoReverb = SNO_NONE;

    sgptGlobals->fAllowRetroFilter = TRUE;

    for (int32 i = 0; i < NUM_BACKGROUND_AMBIENT_CONTROLLERS; i++)
    {
        sgptGlobals->tBkgrndAmbController[i].Initialize(i == BAC_WEATHER);
    }
    sgptGlobals->tMusicMgr.Initialize();

    gptSounds = TDataArrayCreateTracked<SoundInstance, int>("Sounds", AppGetCurrentMemoryInterface(), MAX_SOUND_INSTANCES);

    ApplyPrefSettings();

    return sgptGlobals->fInitialized;
}




void SoundManager::Dispose()
{
    if (!sgptGlobals)
        return;

    SoundUtilsDispose();

    if (sgptGlobals->fInitialized)
    {
        DeleteAll(0xFFFFFFFF, RENDERCONTEXT_INVALID);
        if (gptSounds)
            TDataArrayDispose(gptSounds);




        sgptGlobals->tMusicMgr.Dispose();

        SoundPlatform::Dispose();
        sgptGlobals->fInitialized = FALSE;
    }

    SigmaMemoryFree(sgptGlobals, NULL);
    sgptGlobals = NULL;
}










void SoundManager::InitializeForGame()
{
    ApplyPrefSettings();
}





void SoundManager::DisposeForGame()
{
    ReverbParams tReverbParams;
    ReverbParamsExtended tReverbExtended;
    ReverbParamsExtended * ptReverbExtended = &tReverbExtended;
    SoundPlatform::SetReverbParamsFromPreset(tReverbParams, SOUND_REVERB_EAX_PRESET_OFF, ptReverbExtended);
    SoundPlatform::SetReverbParams(tReverbParams, ptReverbExtended);
}









BOOL SoundManager::IsInitialized()
{
    if (sgptGlobals)
        return sgptGlobals->fInitialized != FALSE;
    return FALSE;
}



void SoundManager::SaveControllerStates(ControllerStates & tStates)
{
    if (!IsInitialized())
        return;

    tStates.dwSuppressedTypes = sgptGlobals->dwSuppressedTypes;
    tStates.snoReverb = sgptGlobals->snoReverb;
    for (int32 i = 0; i < NUM_BACKGROUND_AMBIENT_CONTROLLERS; i++)
        sgptGlobals->tBkgrndAmbController[i].GetCurrentState(tStates.tState[i]);
    sgptGlobals->tMusicMgr.GetCurrentState(tStates.tMusicState);
}










void SoundManager::RestoreControllerStates(const ControllerStates & tStates)
{

    if (!IsInitialized())
        return;




    sgptGlobals->dwSuppressedTypes = tStates.dwSuppressedTypes;
    SetReverbParams(tStates.snoReverb);

    sgptGlobals->tMusicMgr.SetCurrentState(tStates.tMusicState);
    for (int32 i = 0; i < NUM_BACKGROUND_AMBIENT_CONTROLLERS; i++)
    {
        sgptGlobals->tBkgrndAmbController[i].SetCurrentState(tStates.tState[i]);
    }
}


























void SoundManager::ResetControllers()
{
    if (!IsInitialized())
        return;

    for (int32 i = 0; i < NUM_BACKGROUND_AMBIENT_CONTROLLERS; i++)
        sgptGlobals->tBkgrndAmbController[i].Reset();
}









void SoundInstance::SetParent(const ParentData & _tParentData)
{
    tParentData = _tParentData;
}




























































































































































































































void SoundManager::ApplyPrefSettings()

{
    if (!IsInitialized())
        return;



    const SoundPrefs& tSoundPrefs = Prefs::GetSoundPrefs();
    sgptGlobals->fSpeakersReversed = tSoundPrefs.bReverseSpeakers;
    SoundManager::SetGlobalMute(tSoundPrefs.bMuteSound);
    SoundManager::SetMasterVolume(tSoundPrefs.flMasterVolume);

    for (int32 i = 0; i < NUMBER_OF_SOUND_TYPES; i++)
    {
        SoundType eType = (SoundType)i;
        if (eType == SOUNDTYPE_EFFECT_UI)
        {
            SoundManager::SetTypeMute(eType, tSoundPrefs.bMuteSoundType[SOUNDTYPE_EFFECT]);
            SoundManager::SetTypeVolume(eType, tSoundPrefs.flTypeVolume[SOUNDTYPE_EFFECT]);
        }
        else
        {
            SoundManager::SetTypeMute(eType, tSoundPrefs.bMuteSoundType[eType]);
            SoundManager::SetTypeVolume(eType, tSoundPrefs.flTypeVolume[eType]);
        }
    }
}




void SoundManager::SuppressSounds(const uint32 dwSuppressedTypes)
{
    if (!IsInitialized())
        return;

    sgptGlobals->dwSuppressedTypes = dwSuppressedTypes;
    DeleteAll(dwSuppressedTypes, RENDERCONTEXT_DEFAULT);
}









void SoundManager::DeleteAll(
    const uint32 dwSoundTypesToDelete,
    const RenderContext eContext)
{
    if (!gptSounds ||
        !IsInitialized())
        return;



    if (dwSoundTypesToDelete & (1 << SOUNDTYPE_MUSIC))
    {
        if (RWindowIDFromContext(eContext) == RWindowMainWindow)
        {
            StopMusic();
        }
    }

    TDataArrayIterator<SoundInstance> tIt;
    while (gptSounds->Iterate(tIt))
    {
        SoundInstance * ptSound = tIt.ptElement;

        if (eContext != RENDERCONTEXT_INVALID && ptSound->eContext != eContext)
            continue;



        if (ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT))
        {
            if (dwSoundTypesToDelete & (1 << SOUNDTYPE_MUSIC))
            {
                SoundDelete(ptSound->id);
            }
        }
        else
        {
            ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
            if (dwSoundTypesToDelete & (1 << pdefSound->eType))
            {
                SoundDelete(ptSound->id);
            }
        }
    }
}





























































































void SoundManager::ListenerSet(const Vector3D & wpVolume, const Vector3D & wpPanning, const Vector3D & wvView, const Vector3D & wvUp)
{




    if (!IsInitialized())
        return;



    if (SoundPlatform::InCustomListenerMode())
        return;




    Vector3D wvListenerUp(wvUp);
    if (sgptGlobals->fSpeakersReversed)
    {
        wvListenerUp = -wvListenerUp;
    }

    Vector3D wvVel = wpVolume - sgptGlobals->wpVolume;
    wvVel *= 60.0f;

    if (sgptGlobals->wpPanning == wpPanning && sgptGlobals->wpVolume == wpVolume &&

        sgptGlobals->wvVel == wvVel &&
        sgptGlobals->wvView == wvView &&
        sgptGlobals->wvUp == wvListenerUp)
    {
        return;
    }

    sgptGlobals->wpVolume = wpVolume;
    sgptGlobals->wpPanning = wpPanning;
    sgptGlobals->wvVel = wvVel;
    sgptGlobals->wvView = wvView;
    sgptGlobals->wvUp = wvListenerUp;

    SoundPlatform::SetListener(sgptGlobals->wpVolume, sgptGlobals->wpPanning, sgptGlobals->wvVel, sgptGlobals->wvView, sgptGlobals->wvUp);
}




void SoundManager::ListenerSetParams(const float flRolloff, const float flDopplerFactor)
{

    if (!IsInitialized())
        return;




    if (!Math::FloatEqual(sgptGlobals->flRolloff, flRolloff) ||
        !Math::FloatEqual(sgptGlobals->flDopplerFactor, flDopplerFactor))
    {
        sgptGlobals->flRolloff = flRolloff;
        sgptGlobals->flDopplerFactor = flDopplerFactor;
        SoundPlatform::SetListenerParams(flRolloff, flDopplerFactor);
    }
}




void SoundManager::ListenerGetVolumePosition(Vector3D & wp)
{
    wp = sgptGlobals->wpVolume;
}





void SoundManager::ListenerGetPanningPosition(Vector3D & wp)
{
    wp = sgptGlobals->wpPanning;
}





void SoundManager::ListenerGetAxes(Vector3D & wvView, Vector3D & wvUp, Vector3D & wvRight)
{
    wvView = sgptGlobals->wvView;
    wvUp = sgptGlobals->wvUp;
    wvRight = Vector3D::CrossProduct(wvView, wvUp);
}







void SoundManager::SetMasterVolume(
    const float flVolume)
{
    if (!IsInitialized())
        return;



    sgptGlobals->flMasterVolume = Math::Max(flVolume, 0.0f);
}




void SoundManager::SetGlobalMute(
    const BOOL fMute)
{
    if (!IsInitialized())
        return;



    sgptGlobals->fGlobalMute = fMute;
}




BOOL SoundManager::GetGlobalMute()
{
    if (!IsInitialized())
        return FALSE;

    return sgptGlobals->fGlobalMute;
}







void SoundManager::SetTypeMute(
    const SoundType eType,
    const BOOL fMute)
{
    if (!IsInitialized())
        return;



    sgptGlobals->fTypeMute[eType] = fMute;
}




















































static float sGetTypeVolume(
    const SoundType eType,
    const RenderContext eContext)
{
    if (eType == SOUNDTYPE_EFFECT && eContext == RENDERCONTEXT_DEFAULT &&

        (GameIsPaused() || GameIsSoftPaused()))
    {
        return 0.0f;
    }

    float flVolForType = 0.0f;
    if (!sgptGlobals->fTypeMute[eType])
    {
        flVolForType = sgptGlobals->flTypeVolume[eType];
    }
    return flVolForType;
}














































float SoundManager::GetTypeVolume(const SoundType eType)
{

    if (!IsInitialized())
        return 0.0f;




    return sGetTypeVolume(eType, RENDERCONTEXT_DEFAULT);
}



void SoundManager::SetTypeVolume(
    const SoundType eType,
    const float flVolume)
{
    if (!IsInitialized())
        return;



    sgptGlobals->flTypeVolume[eType] = Math::Max(flVolume, 0.0f);
}





void SoundManager::SetReverbParams(const SNO snoReverb)
{
    if (snoReverb == INVALID_SNO)
        return;

    if (!IsInitialized())
        return;

    sgptGlobals->snoReverb = snoReverb;
    if (snoReverb != SNO_NONE)
    {
        ScopedReverbDefinitionLockR pdefReverb(snoReverb);
        SoundPlatform::SetReverbParams(pdefReverb->tParams, NULL);
    }
    else
    {
        ReverbParams tReverbParams;
        ReverbParamsExtended tReverbExtended;
        ReverbParamsExtended * ptReverbExtended = &tReverbExtended;
        SoundPlatform::SetReverbParamsFromPreset(tReverbParams, SOUND_REVERB_EAX_PRESET_GENERIC, ptReverbExtended);
        SoundPlatform::SetReverbParams(tReverbParams, ptReverbExtended);
    }
}
















void SoundDelete(const SoundID idSound)
{

    if (!SoundManager::IsInitialized())
        return;




    SoundInstance * ptSound = SoundTryToGet(idSound);
    if (ptSound)
        SoundPlatform::Stop(ptSound);
}





void SoundFadeOut(const SoundID idSound)
{

    SoundInstance * ptSound = SoundTryToGet(idSound);
    if (!ptSound)
        return;

    if (ptSound->flFadeOutTime <= 0.0f)
    {

        SoundDelete(idSound);
        return;
    }

    if (ptSound->flRealStartTime != 0.0f)
    {
        ptSound->flFadeOutStartTime = CGameGetClientTime() + (ptSound->flRealStartTime - ptSound->flWantedStartTime);
    }
    else
    {
        ptSound->flFadeOutStartTime = CGameGetClientTime();
    }
}




void SoundSetPaused(const SoundID idSound, BOOL fPaused)
{
    SoundInstance * ptSound = SoundTryToGet(idSound);
    if (ptSound)
        SoundPlatform::SetPaused(ptSound, fPaused);
}








































































































































































































































































































































































































































































































































































static BOOL sMusicTriggerInstance(SoundInstance * ptSound, const MusicDefinition * pdefMusic, const uint32 dwStartSample)
{




    float flVolume = SoundGetAdjustedVolume(ptSound);

    PlaylistEntry & tEntry = pdefMusic->ptPlaylistEntries[ptSound->tMusicParams.nPlaylistIndex];
    if (!SoundPlatform::PlayMusicEntry(ptSound, tEntry, dwStartSample, flVolume))
        return FALSE;




    if (flVolume > 0.000001f)
    {
        if (CGameGetDebugSoundMode())
        {
        }
    }
    return TRUE;
}












SoundID SoundManager::PlayEntry(const SNO snoMusic, const int32 nPlaylistIndex, const uint32 dwStartSample)
{













    if (!IsInitialized() || (sgptGlobals->dwSuppressedTypes & (1 << SOUNDTYPE_MUSIC)))
        return SOUNDID_INVALID;

    ScopedMusicDefinitionLockR pdefMusic(snoMusic);










    if (pdefMusic->nPlaylistEntryCount == 0)
        return SOUNDID_INVALID;








    PlaylistEntry & tEntry = pdefMusic->ptPlaylistEntries[nPlaylistIndex];

    SoundID idSound = gptSounds->ElementNew();
    if (idSound == SOUNDID_INVALID)
    {
        if (CGameGetDebugSoundMode())
        {
        }
        return SOUNDID_INVALID;
    }

    SoundInstance * ptSound = SoundGet(idSound);



    ptSound->fStitching = FALSE;
    ptSound->fHasRTPC = FALSE;
    ptSound->tChannelState[0].Initialize();
    ptSound->tChannelState[1].Initialize();
    ptSound->nCurrentChannel = 0;

    if (pdefMusic->dwFlags & (1 << MUSIC_DONT_DUCK_BIT)) ptSound->dwFlags |= (1 << SOUNDINSTANCE_DONT_DUCK_BIT); else ptSound->dwFlags &= ~(1 << SOUNDINSTANCE_DONT_DUCK_BIT);
    ptSound->dwFlags |= (1 << SOUNDINSTANCE_IS_MUSIC_BIT);
    ptSound->tMusicParams.snoMusic = snoMusic;
    ptSound->tMusicParams.nPlaylistIndex = nPlaylistIndex;

    ptSound->dwStartTime = GetMachineTicks();
    ptSound->flDopplerFactor = 0.0f;
    ptSound->flDuckAmountDB = 0.0f;
    ptSound->wvVel = cgwvNull;
    ptSound->flDelay = 0.0f;
    ptSound->wp = cgwpNull;
    ptSound->flFadeAtten = 1.0f;
    ptSound->flFadeInTime = 0.0f;
    ptSound->flFadeOutTime = 0.0f;
    ptSound->flFadeInStartTime = -1.0f;
    ptSound->flFadeOutStartTime = -1.0f;

    ptSound->eFadeInShape = tEntry.eFadeInShape;
    ptSound->eFadeOutShape = tEntry.eFadeOutShape;
    ptSound->flFadeInAccel = tEntry.flFadeInAccel;
    ptSound->flFadeOutAccel = tEntry.flFadeOutAccel;

    ptSound->flVolumeDB = pdefMusic->flGlobalVolumeDB + tEntry.flVolumeDB;

    ptSound->flPitchSemitones = 0.0f;

    ptSound->tRolloffParams.eFalloffType = FALLOFF_NONE; ptSound->tRolloffParams.flRolloffFactor = 0.0f;
    ptSound->tRolloffParams.flDistanceMin = 0.0f; ptSound->tRolloffParams.flDistanceMax = 0.0f;




    ptSound->dwPriority = PRIORITY_HIGHEST;
    ptSound->eType = SOUNDTYPE_MUSIC;
    ptSound->eContext = RENDERCONTEXT_DEFAULT;



    {
        ScopedSoundBankDefinitionLockR pdefSoundBank(tEntry.snoSoundBank);
        SoundSample & tSoundSample = pdefSoundBank->ptSoundSamples[tEntry.nSampleIndex];
        uint32 dwChannels, dwSampleRate, dwBitRate;
        SoundFormatToCSB(tSoundSample.GetFormat(), dwChannels, dwSampleRate, dwBitRate);

        float flOOSampleRate = 1.0f / dwSampleRate;
        ptSound->tMusicParams.flFadeInDuration = tEntry.dwFadeInTime * OO_TICKS_PER_SECOND;
        ptSound->tMusicParams.flFadeOutDuration = tEntry.dwFadeOutTime * OO_TICKS_PER_SECOND;
        uint32 dwEndSample = tEntry.GetSamplesBeforeStart(dwSampleRate) + tEntry.GetBaseDurationSamples(dwStartSample);
        ptSound->tMusicParams.flFadeOutTime = flOOSampleRate * (dwEndSample + tEntry.dwFadeOutOffset);
    }


    ptSound->tParentData.Init();











    if (!sMusicTriggerInstance(ptSound, pdefMusic, dwStartSample))
    {
        gptSounds->ElementDeleteByID(idSound);
        return SOUNDID_INVALID;
    }

    return idSound;
}




































































SoundID SoundManager::PlayBkgrndAmbient(const SNO snoSound, const int32 nBkgrndAmbientController, const float flBlendTime, const BOOL fOverlay)
{
    return sgptGlobals->tBkgrndAmbController[nBkgrndAmbientController].Play(snoSound, flBlendTime, fOverlay);
}








BOOL SoundManager::OpenOVTStream(SoundOVT * ptSoundOVT)
{
    if (!IsInitialized())
        return FALSE;

    return SoundPlatform::OpenOVTStream(ptSoundOVT);
}







void SoundManager::PlayOVTStream()
{
    if (!IsInitialized())
        return;

    if (sgptGlobals->fGlobalMute)
        return;

    SoundPlatform::PlayOVTStream(sgptGlobals->flMasterVolume);
}









void SoundManager::StopOVTStream()
{
    if (!IsInitialized())
        return;

    SoundPlatform::StopOVTStream();
}







BOOL SoundManager::GetOVTStreamPos(float & flStreamPos)
{
    if (!IsInitialized())
        return FALSE;

    if (sgptGlobals->fGlobalMute)
        return FALSE;

    return SoundPlatform::GetOVTStreamPos(flStreamPos);
}










void SoundManager::SetCustomListenerMode(const BOOL fVal)
{
    if (!IsInitialized())
        return;

    SoundPlatform::SetCustomListenerMode(fVal);
}






























void SoundInstance::SetPosition(const Vector3D & _wp, const float flDeltaTime)
{
    if (flDeltaTime > 0.0f && flDopplerFactor > 0.0f)
    {




        Vector3D wvVel = (_wp - wp) * (1.0f / flDeltaTime);
        Vector3D wvToListener = sgptGlobals->wpVolume - _wp;




        SoundPlatform::SetVelocity(this, flDopplerFactor * (Vector3D::Projection(wvVel, wvToListener) - Vector3D::Projection(sgptGlobals->wvVel, wvToListener)));
    }


    SoundPlatform::SetPosition(this, _wp);
}

void SoundInstance::SetControlledVolumeAtten(const float flAtten)
{
    flFadeAtten = flAtten;
}





void SoundInstance::SetPitchSemitones(const float _flPitchSemitones)
{
    flPitchSemitones = _flPitchSemitones;
}





static float sSoundGetBaseLinearVolume(const SoundType eType, const RenderContext eContext, const float flFadeAtten, const BOOL fAffectedByWeather)
{


    float flVolForType = sGetTypeVolume(eType, eContext);


    if (flVolForType < 0.000001f)
        return 0.0f;








    float flLinear = flFadeAtten * (flVolForType * sgptGlobals->flMasterVolume);

    if (fAffectedByWeather)
    {
        RWindow * ptRWindow = GetRWindowMgr()->GetActiveRWindowForAudio();
        if (ptRWindow)
        {
            float flIntensity = Weather::GetIntensity(ptRWindow->GetWeatherRWindowData());
            flLinear *= flIntensity;
        }
    }
    return flLinear;
}





float sLogBaseN(double flValue, double flBase)
{

    return log(flValue) / log(flBase);
}




float SoundCalculateFadeAttenuation(FadeShape eFadeShape, float flShapeAccel, float flT)
{
    float flAtten;

    flT = Math::Pin(flT, 0.0f, 1.0f);



    switch (eFadeShape)
    {
    case 0:
        flAtten = flT;
        break;
    case 2:
        flAtten = 3.0f * Math::Pow(flT, 2.0f) - 2.0f * Math::Pow(flT, 3.0f);
        break;
    case 1:


        if (flShapeAccel < 0.999999f || flShapeAccel > 1.000001f)
            flAtten = (float)sLogBaseN((flShapeAccel - 1.0f) * flT + 1.0f, flShapeAccel);
        else if (flShapeAccel <= 0.0f)  // original 0x7100239cdc: fcmp #0.0; fcsel s0, s0, s3, hi (if/else select, merged line 0:39)
            flAtten = 1.0f;
        else flAtten = flT;
        break;












    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    return flAtten;
}



static float sSoundGetFadeOutAttenuation(const SoundInstance * ptSound)
{
    float flClientTime = CGameGetClientTime();


    if (ptSound->flFadeOutStartTime >= flClientTime)
        return 1.0f;




    float flT;
    if (ptSound->flFadeOutTime == 0.0f)
    {
        flT = flClientTime > ptSound->flFadeOutStartTime ? 1.0f : 0.0f;
    }
    else
    {
        flT = (flClientTime - ptSound->flFadeOutStartTime) / ptSound->flFadeOutTime;
    }



    float flMaxAtten = 1.0f;

    if (ptSound->flFadeInStartTime != -1.0f)
    {
        flMaxAtten = Math::Pin((ptSound->flFadeOutStartTime - ptSound->flFadeInStartTime) / ptSound->flFadeInTime, 0.0f, 1.0f);
    }



    float flAtten = SoundCalculateFadeAttenuation(ptSound->eFadeOutShape, ptSound->flFadeOutAccel, 1.0f - flT);
    return Math::Pin(flAtten, 0.0f, flMaxAtten);
}









float SoundGetAdjustedVolume(const SoundInstance * ptSound)
{
    float flFadeAtten = ptSound->flFadeAtten;

    if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT)))
    {

        if (ptSound->flFadeOutStartTime != -1.0f)
        {
            flFadeAtten = sSoundGetFadeOutAttenuation(ptSound);
        }

        else if (ptSound->flFadeInStartTime != -1.0f)
        {
            float flT = (CGameGetClientTime() - ptSound->flFadeInStartTime) / ptSound->flFadeInTime;
            flFadeAtten = Math::Pin(SoundCalculateFadeAttenuation(ptSound->eFadeInShape, ptSound->flFadeInAccel, flT), FADE_MIN, 1.0f);
        }
    }
    else
    {

        uint32 dwElapsed = GetMachineTicks() - ptSound->dwStartTime;
        float flTime = (float)((dwElapsed * TICKS_PER_SECOND) / 1000) * OO_TICKS_PER_SECOND;


        if (flTime >= ptSound->tMusicParams.flFadeOutTime)
        {
            float flT = 1.0f - (flTime - ptSound->tMusicParams.flFadeOutTime) / ptSound->tMusicParams.flFadeOutDuration;
            flFadeAtten = SoundCalculateFadeAttenuation(ptSound->eFadeOutShape, ptSound->flFadeOutAccel, flT);
        }

        else if (flTime < ptSound->tMusicParams.flFadeInDuration)
        {

            float flT = flTime / ptSound->tMusicParams.flFadeInDuration;
            flFadeAtten = Math::Pin(SoundCalculateFadeAttenuation(ptSound->eFadeInShape, ptSound->flFadeInAccel, flT), FADE_MIN, 1.0f);
        }
    }



    if (flFadeAtten <= 0.000001f)
        return 0.0f;



    float flVolumeDB = ptSound->flVolumeDB + ptSound->flRTPCAdjustment[RTPC_TARGET_VOLUME];
    if (ptSound->eType != SOUNDTYPE_AMBIENT &&
        ptSound->flDuckAmountDB > -0.000001f)
    {
        if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_DONT_DUCK_BIT)) &&
            sgptGlobals->flDuckedVolumeAdjustedDB < -0.000001f)
        {
            flVolumeDB += sgptGlobals->flDuckedVolumeAdjustedDB;
        }
    }

    float flLinear = sSoundGetBaseLinearVolume(ptSound->eType, ptSound->eContext, flFadeAtten, ptSound->dwFlags & (1 << SOUNDINSTANCE_AFFECTED_BY_WEATHER_BIT));
    float flAdjusted = flLinear * SoundBelToLinear(Math::FloatToInt(flVolumeDB * 100.0f));

    return flAdjusted > 0.000001f ? flAdjusted : 0.0f;
}

float SoundGetAdjustedPitch(const SoundInstance * ptSound)
{
    float flAdjustedPitch = SoundPitchSemitonesToLinear(ptSound->flPitchSemitones + ptSound->flRTPCAdjustment[1]);
    return flAdjustedPitch;
}






















void SoundManager::PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle)
{
    if (!IsInitialized())
        return;

    sgptGlobals->tMusicMgr.Play(snoMusic, nFirstIndexToPlay, fDisableShuffle);
}













void SoundManager::StopMusic()
{
    if (!IsInitialized())
        return;

    sgptGlobals->tMusicMgr.Stop();
}







void SoundManager::StopMusicOverlay()
{
    if (!IsInitialized())
        return;

    sgptGlobals->tMusicMgr.StopOverlay();
}







inline void MusicState::Initialize()
{
    m_snoMusic = SNO_NONE;

    new (&m_arPlaylist) PlaylistArray();
    m_nPlaylistIndex = -1;
    m_dwPausedTime = 0;
    m_dwPlaylistStartTime = 0;
    m_dwEndMachineTime = 0;
    m_fDisableShuffle = FALSE;
    m_fOverlay = FALSE;
    m_nNewEntryIndexOnLoop = 0;
    m_idSound = SOUNDID_INVALID;
    for (int32 i = 0; i < 2; i++)
    {
        m_tRandomSoundState[i].m_flTimeDelay = 0.0f;
        m_tRandomSoundState[i].m_flTimeUntilRepeat = -1.0f;
        m_tRandomSoundState[i].m_idSound = SOUNDID_INVALID;
    }
}





inline void MusicState::Dispose()
{

    m_arPlaylist.~PlaylistArray();
}




inline void MusicState::Stop()
{
    StopRandomSounds();
    SoundDelete(m_idSound);
    m_idSound = SOUNDID_INVALID;
    m_snoMusic = SNO_NONE;
    m_arPlaylist.clear();
}





void MusicState::StopRandomSounds()
{
    for (int32 i = 0; i < 2; i++)
    {
        RandomMusicSoundState & tState = m_tRandomSoundState[i];
        SoundFadeOut(tState.m_idSound);
        tState.m_idSound = SOUNDID_INVALID;
    }
}































































































































































































// DWARF low_pc 0: standalone body is not checkable; its inlined CreatePlaylist copy is verified.
void MusicState::StartSample()
{
    // Inference: original line 3011 guards the increment/play with a nonempty playlist test.
    if (m_arPlaylist.size() == 0)
        return;



    m_nPlaylistIndex++;

    ActivePlaylistEntry & tListEntry = m_arPlaylist[m_nPlaylistIndex];


    m_idSound = SoundManager::PlayEntry(m_snoMusic, tListEntry.nPlaylistEntry, tListEntry.dwStartSample);
}




































void MusicState::Update(
    const float flDeltaTime)
{
    if (m_snoMusic == SNO_NONE)
        return;




    CGameGlobals& tCGameGlobals = CGameGlobalsGet();
    ScopedMusicDefinitionLockR pdefMusic(m_snoMusic);
    for (int32 i = 0; i < 2; i++)
    {
        const RandomMusicSoundParams & tParams = pdefMusic->tRandomSounds[i];
        RandomMusicSoundState & tState = m_tRandomSoundState[i];

        if (tState.m_flTimeUntilRepeat == -1.0f)
            continue;




        // Inference: `> 0.0f` + continue (3083 b.gt reuses Max's compare); no row for the continue.
        tState.m_flTimeDelay = Math::Max(tState.m_flTimeDelay - flDeltaTime, 0.0f);
        if (tState.m_flTimeDelay > 0.0f)
            continue;



        tState.m_flTimeUntilRepeat = Math::Max(tState.m_flTimeUntilRepeat - flDeltaTime, 0.0f);
        if (tState.m_flTimeUntilRepeat != 0.0f)
            continue;

        if (tParams.snoSound != SNO_NONE)
        {
            SoundID idSound = SoundManager::PlaySound(tParams.snoSound, cgtPSODefaults);
            if (idSound != SOUNDID_INVALID)
            {


                tState.m_idSound = idSound;
            }
            uint32 dwTicksUntilRepeat = RandRange_Internal(tCGameGlobals.rngLocal, tParams.dwPeriodMin, tParams.dwPeriodDelta);
            tState.m_flTimeUntilRepeat = dwTicksUntilRepeat == (uint32)-1 ? -1.0f : dwTicksUntilRepeat * OO_TICKS_PER_SECOND;
        }
        else
        {
            // Inference: store merged into 3102's (line 0, col 40), value is 3075's -1.0f.
            tState.m_flTimeUntilRepeat = -1.0f;
        }
    }


    uint32 dwTicksElapsed = GetMachineTicks() - m_dwPlaylistStartTime;
    if (m_nPlaylistIndex < static_cast<int32>(m_arPlaylist.size()) - 1)
    {
        if (dwTicksElapsed > m_arPlaylist[m_nPlaylistIndex + 1].dwStartMachineTime)
            StartSample();
    }
    else
    {



        if (!m_fOverlay &&
            dwTicksElapsed > m_dwEndMachineTime)
        {

            CreatePlaylist(m_nNewEntryIndexOnLoop, TRUE);
        }

        if (m_fOverlay &&
            !SoundTryToGet(m_idSound))
        {
            Stop();
        }
    }
}







































inline void MusicManager::Initialize()
{


    for (int32 i = 0; i < 2; i++)
        m_tState[i].Initialize();
    m_ptStateCurrent = &m_tState[0];
    m_ptStateOld = &m_tState[1];
    m_fPaused = FALSE;
    m_snoOverlayReturnMusic = SNO_NONE;
    m_snoOverlay = SNO_NONE;
    m_flBlendTime = 0.0f;
    m_flT = 0.0f;
    m_flTimePlayedThisTrack = 0.0f;
}







inline void MusicManager::Dispose()
{




    for (int32 i = 0; i < 2; i++)
        m_tState[i].Dispose();
}

























void MusicManager::Update(const float flDeltaTime)
{


    for (int32 i = 0; i < 2; i++) m_tState[i].Update(flDeltaTime);


    SoundInstance * ptCurrent = SoundTryToGet(m_ptStateCurrent->m_idSound);
    SoundInstance * ptOld = SoundTryToGet(m_ptStateOld->m_idSound);

    m_flTimePlayedThisTrack += flDeltaTime;

    if (m_snoOverlay != SNO_NONE)
    {


        if (!ptCurrent)
        {
            StopOverlay();
        }
        else
        {

            uint32 dwElapsed = GetMachineTicks() - ptCurrent->dwStartTime;
            float flTime = (float)((dwElapsed * TICKS_PER_SECOND) / 1000) * OO_TICKS_PER_SECOND;
            if (flTime >= ptCurrent->tMusicParams.flFadeOutTime)
            {
                StopOverlay();
            }
        }
    }









    if (m_flBlendTime != 0.0f)
    {




        m_flT += (1.0f / m_flBlendTime) * flDeltaTime;

        if (ptCurrent)
        {


            ptCurrent->flFadeAtten = m_ptStateCurrent->m_snoMusic == m_snoOverlay ? 1.0f : Math::Pin(m_flT, FADE_MIN, 1.0f);
        }








        if (ptOld)

            ptOld->flFadeAtten = Math::Pin(1.0f - m_flT, 0.0f, 1.0f);

        if (m_flT >= 1.0f)
        {

            m_flBlendTime = 0.0f;
            m_flT = 0.0f;

            m_ptStateOld->Stop();
        }
    }
}

void MusicManager::Play(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle)  // DWARF 3305-3308
{
    if (m_ptStateCurrent->m_snoMusic == snoMusic)
        return;






    if (m_fPaused ||
        (sgptGlobals->dwSuppressedTypes & (1 << SOUNDTYPE_MUSIC)))
        return;



    if (m_snoOverlay != SNO_NONE &&
        m_snoOverlayReturnMusic == snoMusic)
        return;





    if (m_flBlendTime != 0.0f)
    {
        if (m_ptStateOld->m_snoMusic == snoMusic)
        {

            Math::Swap(m_ptStateCurrent, m_ptStateOld);
            m_flT = 1.0f - m_flT;
            return;
        }

        if (m_ptStateOld->m_idSound != SOUNDID_INVALID)
        {


            m_ptStateOld->Stop();
            m_flT = 0.0f;
        }
    }

    BOOL fOverlay = FALSE;
    if (snoMusic != SNO_NONE)
    {
        ScopedMusicDefinitionLockR pdefMusic(snoMusic);
        fOverlay = !(pdefMusic->dwFlags & (1 << MUSIC_LOOPS_BIT));
    }

    if (fOverlay)
    {
        m_flBlendTime = 1.0f;


        if (m_snoOverlay == SNO_NONE)
        {
            m_snoOverlayReturnMusic = m_ptStateCurrent->m_snoMusic;
        }

        m_snoOverlay = snoMusic;
    }
    else
    {
        if (m_ptStateCurrent->m_snoMusic != SNO_NONE)
        {


            SoundInstance * ptCurrent = SoundTryToGet(m_ptStateCurrent->m_idSound);
            if (snoMusic == SNO_NONE && ptCurrent)
            {
                m_flBlendTime = ptCurrent->tMusicParams.flFadeOutDuration;
            }
            else
            {
                m_flBlendTime = 2.0f;
            }
        }



        // Inference: chained assignment (one merged 8-byte store on row 3391:27).
        m_snoOverlayReturnMusic = m_snoOverlay = SNO_NONE;
    }


    m_ptStateOld->Stop();
    Math::Swap(m_ptStateCurrent, m_ptStateOld);

    if (snoMusic != SNO_NONE)
    {
        m_ptStateCurrent->m_snoMusic = snoMusic;
        m_ptStateCurrent->m_fDisableShuffle = fDisableShuffle;
        m_ptStateCurrent->m_fOverlay = fOverlay;
        m_ptStateCurrent->CreatePlaylist(nFirstIndexToPlay, FALSE);

        CGameGlobals & tCGameGlobals = CGameGlobalsGet();
        ScopedMusicDefinitionLockR pdefMusic(snoMusic);
        for (int32 i = 0; i < 2; i++)
        {
            const RandomMusicSoundParams & tParams = pdefMusic->tRandomSounds[i];
            if (tParams.snoSound != SNO_NONE)
            {
                RandomMusicSoundState & tState = m_ptStateCurrent->m_tRandomSoundState[i];
                tState.m_flTimeDelay = tParams.dwOffset * OO_TICKS_PER_SECOND;
                uint32 dwTicksUntilRepeat = RandRange_Internal(tCGameGlobals.rngLocal, tParams.dwPeriodMin, tParams.dwPeriodDelta);
                tState.m_flTimeUntilRepeat = dwTicksUntilRepeat == (uint32)-1 ? -1.0f : dwTicksUntilRepeat * OO_TICKS_PER_SECOND;
                tState.m_idSound = SOUNDID_INVALID;
            }
        }
    }
    else
    {
        m_ptStateCurrent->Stop();
    }

    m_flTimePlayedThisTrack = 0.0f;
}




void MusicManager::Stop()
{
    for (int32 i = 0; i < 2; i++)
        m_tState[i].Stop();
    m_snoOverlay = SNO_NONE;
    m_ptStateCurrent = &m_tState[0];
    m_ptStateOld = &m_tState[1];
}









inline void MusicManager::StopOverlay()
{
    m_snoOverlay = SNO_NONE;
    Play(m_snoOverlayReturnMusic, 0, FALSE);
}







inline void MusicManager::GetCurrentState(SavedMusicState & tSavedState) const
{
    if (m_snoOverlayReturnMusic != SNO_NONE)
        tSavedState.snoMusic = m_snoOverlayReturnMusic;
    else
        tSavedState.snoMusic = m_ptStateCurrent->m_snoMusic;
}










inline void MusicManager::SetCurrentState(const SavedMusicState & tSavedState)
{


    if (tSavedState.snoMusic != SNO_NONE)
    {





        Play(tSavedState.snoMusic, 0, FALSE);
    }
}


#line 104
// Recovered from the constructor's inlined stores; no standalone original body is checkable.
inline ActivePlaylistEntry::ActivePlaylistEntry(  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
    const int32 nEntryIndex)
{
    nPlaylistEntry = nEntryIndex;
    dwStartMachineTime = dwStartSample = 0;
}
#line 2824
// No standalone original body is checkable; inference: duration sums base and pre-start samples (both inline copies).
static float sComputeEntryDurationMS(
    ActivePlaylistEntry & tListEntry,
    const MusicDefinition * pdefMusic)
{
    const PlaylistEntry & tEntry = pdefMusic->ptPlaylistEntries[tListEntry.nPlaylistEntry];

    float flSamplesToMS;
    uint32 dwChannels;
#line 2832
    uint32 dwFreq;
#line 2832
    uint32 dwBitRate;
    {
        ScopedSoundBankDefinitionLockR pdefSoundBank(tEntry.snoSoundBank);
        SoundSample * ptSample = &pdefSoundBank->ptSoundSamples[tEntry.nSampleIndex];
        SoundFormatToCSB(ptSample->GetFormat(), dwChannels, dwFreq, dwBitRate);
        flSamplesToMS = 1000.0f / dwFreq;
    }


    float flDelayMS = tEntry.GetBaseDurationSamples(tListEntry.dwStartSample) * flSamplesToMS;
    flDelayMS += tEntry.GetSamplesBeforeStart(dwFreq) * flSamplesToMS;
    return flDelayMS;
}
#line 2849
// Inference: negative sample conversion reproduces the original fdiv/fadd fade adjustment.
static void sAdjustStartTimeForFadeIn(
    const PlaylistEntry & tEntry,
    const MusicDefinition * pdefMusic,
    float & flDelayMS)
{
    float flSamplesToMS;
    uint32 dwChannels;
#line 2856
    uint32 dwFreq;
#line 2856
    uint32 dwBitRate;
    {
        ScopedSoundBankDefinitionLockR pdefSoundBank(tEntry.snoSoundBank);
        SoundSample * ptSample = &pdefSoundBank->ptSoundSamples[tEntry.nSampleIndex];
        SoundFormatToCSB(ptSample->GetFormat(), dwChannels, dwFreq, dwBitRate);
        flSamplesToMS = -1000.0f / dwFreq;
    }
    flDelayMS += tEntry.GetSamplesBeforeStart(dwFreq) * flSamplesToMS;
    flDelayMS = Math::Max(flDelayMS, 0.0f);


    float flDelayBetweenEntriesTime = RandRange_Internal(CGameGlobalsGet().rngLocal, pdefMusic->dwDelayBetweenEntriesMin, pdefMusic->dwDelayBetweenEntriesDelta) * OO_TICKS_PER_SECOND;
    flDelayMS += flDelayBetweenEntriesTime * 1000.0f;
}
#line 2873
// Inference: shuffle selection, loop exclusion and timing accumulation follow original branch/line evidence.
void MusicState::CreatePlaylist(
    const int32 nFirstIndexToPlay,
    const BOOL fLooped)
{
    m_arPlaylist.clear();

    m_nNewEntryIndexOnLoop = 0;
    ScopedMusicDefinitionLockR pdefMusic(m_snoMusic);
    if (pdefMusic->nPlaylistEntryCount <= 0)
    {


        Stop();
        return;
    }
    // Inference: the unshuffled path retains the requested index; the looped shuffle starts at zero.
    int32 nActualFirstIndexToPlay = nFirstIndexToPlay;

    if (m_fDisableShuffle || pdefMusic->nPlaylistEntryCount == 1 || !(pdefMusic->dwFlags & (1 << MUSIC_SHUFFLE_BIT)))
    {


        for (int32 i = 0; i < pdefMusic->nPlaylistEntryCount; ++i)
        {
            ActivePlaylistEntry tListEntry(i);
            m_arPlaylist.push_back(tListEntry);
        }
    }
    else
    {
        // Inference: line 2905 dynamically allocates count*4 stack bytes; cast converts allocated storage only.
        int32 * pnTemp = static_cast<int32 *>(__builtin_alloca(pdefMusic->nPlaylistEntryCount * sizeof(int32)));

        if (fLooped)
        {


            ActivePlaylistEntry tListEntry(nFirstIndexToPlay);

            m_arPlaylist.push_back(tListEntry);
            nActualFirstIndexToPlay = 0;
        }

        // Inference: DWARF location records zero at the start of the candidate collection loop.
        int32 nCount = 0;
        for (int32 i = 0; i < pdefMusic->nPlaylistEntryCount; ++i)
        {
            if (!fLooped || i != nFirstIndexToPlay)
            {
                pnTemp[nCount] = i;
                nCount++;
            }
        }

        CGameGlobals & tCGameGlobals = CGameGlobalsGet();
        while (nCount > 0)
        {
            int32 nIndex = Rand_Internal(tCGameGlobals.rngLocal, nCount - 1);

            ActivePlaylistEntry tListEntry(pnTemp[nIndex]);
            m_arPlaylist.push_back(tListEntry);

            pnTemp[nIndex] = pnTemp[--nCount];
        }




        do
        {
            m_nNewEntryIndexOnLoop = Rand_Internal(tCGameGlobals.rngLocal, pdefMusic->nPlaylistEntryCount - 1);
        } while (m_nNewEntryIndexOnLoop ==
                 m_arPlaylist[pdefMusic->nPlaylistEntryCount - 1].nPlaylistEntry);
    }

    // DWARF location fixes conversion to 1.0f and initial machine-time accumulator to zero.
    const float flMSToMachineTicks = 1.0f;
    uint32 dwTimeMachineTicks = 0;

    RNG & rngLocal = CGameGlobalsGet().rngLocal;
    for (int32 i = nActualFirstIndexToPlay; i < pdefMusic->nPlaylistEntryCount; ++i)
    {
        ActivePlaylistEntry & tCurrListEntry = m_arPlaylist[i];

        PlaylistEntry & tEntry = pdefMusic->ptPlaylistEntries[tCurrListEntry.nPlaylistEntry];

        tCurrListEntry.dwStartSample = RandRange_Internal(rngLocal, tEntry.dwStartPosMin, tEntry.dwStartPosDelta);

        if (i <= nActualFirstIndexToPlay)
        {
            tCurrListEntry.dwStartMachineTime = 0;
        }
        else
        {

            ActivePlaylistEntry & tPrevListEntry = m_arPlaylist[i - 1];
            float flDelayMS = sComputeEntryDurationMS(tPrevListEntry, pdefMusic);

            sAdjustStartTimeForFadeIn(tEntry, pdefMusic, flDelayMS);

            dwTimeMachineTicks += (uint32)(flDelayMS * flMSToMachineTicks);

            m_arPlaylist[i].dwStartMachineTime = dwTimeMachineTicks;
        }
    }


    {
        ActivePlaylistEntry & tLastListEntry = m_arPlaylist[pdefMusic->nPlaylistEntryCount - 1];
        float flDelayMS = sComputeEntryDurationMS(tLastListEntry, pdefMusic);

        if (!m_fOverlay)
        {

            ActivePlaylistEntry & tListEntryOnLoop = m_arPlaylist[m_nNewEntryIndexOnLoop];
            PlaylistEntry & tEntryOnLoop = pdefMusic->ptPlaylistEntries[tListEntryOnLoop.nPlaylistEntry];
            sAdjustStartTimeForFadeIn(tEntryOnLoop, pdefMusic, flDelayMS);
        }
        dwTimeMachineTicks += (uint32)(flDelayMS * flMSToMachineTicks);

        m_dwEndMachineTime = dwTimeMachineTicks;
    }


    m_nPlaylistIndex = nActualFirstIndexToPlay - 1;
    m_dwPlaylistStartTime = GetMachineTicks();


    StartSample();
}

#line 3690
#include "../../Common/Hyperlink.h"
#include "../InterpolationScalar.h"
#include "../CWorld.h"
// DWARF SoundUtils.cpp:190; original signature, declared locally because the owner has no header.
StreamMode SoundShouldBeStreamed(const SoundSample & tSample);
// Sound.cpp:864; declaration for the definition below.
namespace SoundManager { void Update(); }
#line 61
const float DUCK_IN_SPEED = -12.0f;
const float DUCK_OUT_SPEED = -9.0f;
#line 1557
// NON_MATCHING: 644 vs 648 bytes; original materialises w0=1 before the eDSM test (early skip returns straight to the epilogue) while the "neither mode" path shares the final return block; ours shares one return-TRUE block for all. Tried switch, early `return TRUE` (gives b.ls), inner second `return TRUE`. Inference: diagnostic formatting kept via its calls.
static BOOL sSoundPlaySample(
    SoundInstance * ptSound,
    const SNO snoSoundBank,
    SoundSample & tSample,
    SoundPlaybackParams & tParams)
{


    DebugSoundMode eDSM = CGameGetDebugSoundMode();

    if (snoSoundBank == gptGroupDefSoundBank->GetFallbackDataSNO())
        return FALSE;







    BOOL fSuccess = SoundPlatform::Play(ptSound, snoSoundBank, tSample, tParams);
    if (!fSuccess)
        return FALSE;



    if (ptSound->dwFlags & (1 << SOUNDINSTANCE_OCCLUDED_BIT))
    {
        SoundPlatform::SetOcclusion(ptSound, 1.0f);
    }

    if (eDSM && tParams.flVolume > 0.000001f)
    {

        if (eDSM == DSM_MINIMAL)
        {

            CRefString sSoundName = CRefString().Format("Sound %s, perm %d (Bank %s, Sample hash %u)",
                HyperlinkCreateFromSNO(SNOName(SOUND_SNO, ptSound->tSoundParams.snoSound), NULL).str(),
                ptSound->tSoundParams.nPermutationIndex,
                HyperlinkCreateFromSNO(SNOName(SOUNDBANK_SNO, snoSoundBank), NULL).str(),
                tSample.dwHash);








            ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
            CRefString sParams = CRefString().Format(
                "Global Volume = %0.1f, Ducking = %0.1f, Min Dist = %0.1f, Max Dist = %0.1f",
                pdefSound->flGlobalVolumeDB,
                pdefSound->flDuckAmountDB,
                pdefSound->wdFalloffRadiusMin, pdefSound->wdFalloffRadiusMin + pdefSound->wdFalloffRadiusDelta);





        }
        else if (eDSM == DSM_FULL)
        {

            CRefString sSoundName = CRefString().Format("Sound %s, perm %d (Bank %s, Sample hash %u), channel %d",
                HyperlinkCreateFromSNO(SNOName(SOUND_SNO, ptSound->tSoundParams.snoSound), NULL).str(),
                ptSound->tSoundParams.nPermutationIndex,
                HyperlinkCreateFromSNO(SNOName(SOUNDBANK_SNO, snoSoundBank), NULL).str(),
                tSample.dwHash,
                ptSound->tChannelState[ptSound->nCurrentChannel].nChannelIndex);
        }
    }



























    return TRUE;
}
#line 1662
// Inference: instance-limit switch follows original fail/farthest/steal branches; standalone body is not checkable.
static BOOL sLimitInstances(
    const SoundDefinition * pdefSound,
    const Vector3D * pwp,
    BOOL fSuppressTracing)
{

    SoundID idSoundToSteal = SOUNDID_INVALID; // DWARF snoSound below has no location/value; do not infer its initializer.
    SNO snoSound;
    int32 nInstanceCount = SoundPlatform::GetSoundInstanceCount(pdefSound, pwp, idSoundToSteal);
    if (nInstanceCount >= pdefSound->nInstanceLimit)
    {
        if (!fSuppressTracing && // Inference: `&&` branch row at 1674:31, call on the next line; empty body (trace output compiled out).
            CGameGetDebugSoundMode())
        {
        }


        switch (pdefSound->eLimitBehavior)
        {
            case ILB_FAIL:
                return FALSE;
            case ILB_STEAL_FARTHEST:

                if (idSoundToSteal == SOUNDID_INVALID)
                    return FALSE;
                break;
            default:
                break;
        }






        SoundFadeOut(idSoundToSteal);
    }


    return TRUE;

}
#line 1708
// Inference: cooldown rejects only a nonzero prior timestamp inside the interval; standalone body is not checkable.
static BOOL sCheckCooldownTime(
    const SoundDefinition * pdefSound,
    const BOOL fSuppressTracing)
{
    if (pdefSound->dwCooldownTime * OO_TICKS_PER_SECOND > 0.0f &&
        pdefSound->tSoundPlayHistory.dwLastPlayedTime != 0)
    {
        uint32 dwElapsedTicks = GetMachineTicks() - pdefSound->tSoundPlayHistory.dwLastPlayedTime;
        if (dwElapsedTicks < pdefSound->dwCooldownTime)
        {
            if (!fSuppressTracing && // Inference: `&&` branch row at 1719:35, call on the next line; empty body (trace output compiled out).
                CGameGetDebugSoundMode())
            {
            }
            return FALSE;
        }
    }


    return TRUE;
}
#line 1733
// Inference: mute/volume suppress diagnostics; instance and cooldown checks precede the real playback call.
static BOOL sSoundTriggerInstance(
    SoundInstance * ptSound,
    const SoundDefinition * pdefSound,
    float flStartOffset)
{


    BOOL fSuppressTracing = sgptGlobals->fTypeMute[ptSound->eType] || sgptGlobals->flTypeVolume[ptSound->eType] < 0.000001f;
    if (pdefSound->dwFlags & (1 << SOUND_LIMIT_INSTANCES_BIT))
    {
        if (!sLimitInstances(pdefSound, &ptSound->wp, fSuppressTracing))
            return FALSE;
    }



    if (!sCheckCooldownTime(pdefSound, fSuppressTracing))
        return FALSE;

    // Inference: DWARF const parameter nevertheless writes offset0x94; current header lacks mutable, so cast only this history field.
    const_cast<SoundPlayHistory &>(pdefSound->tSoundPlayHistory).dwLastPlayedTime = GetMachineTicks();

    SoundPlaybackParams tParams;

    tParams.flVolume = SoundGetAdjustedVolume(ptSound);
    tParams.flPitch = SoundGetAdjustedPitch(ptSound);

    tParams.dwPriority = pdefSound->dwPriority;
    tParams.eType = pdefSound->eType;
    tParams.fLoops = !!(pdefSound->dwFlags & (1 << SOUND_LOOPS_BIT));
    tParams.fChooseNewPermutationOnLoop = !!(pdefSound->dwFlags & (1 << SOUND_CHOOSE_NEW_PERMUTATION_ON_LOOP_BIT));
    tParams.flStartOffset = flStartOffset;
    if (pdefSound->nPermutationCount < 2)
    {
        tParams.fChooseNewPermutationOnLoop = FALSE;
    }

    SoundPermutation & tPermutation = pdefSound->ptPermutations[ptSound->tSoundParams.nPermutationIndex];
    ScopedSoundBankDefinitionLockR pdefSoundBank(tPermutation.snoValidatedSoundBank);
    SoundSample & tSample = pdefSoundBank->ptSoundSamples[tPermutation.nSampleIndex];
    tParams.eStreamMode = SoundShouldBeStreamed(tSample);
    tParams.f3D = !!(ptSound->dwFlags & (1 << SOUNDINSTANCE_3D_BIT));
    tParams.fDisableReverb = !(ptSound->dwFlags & (1 << SOUNDINSTANCE_ENABLE_REVERB_BIT));

    BOOL fSuccess = sSoundPlaySample(ptSound, tPermutation.snoValidatedSoundBank, tSample, tParams);

    return fSuccess;
}
#line 771
// Inference: update only real nonmusic RTPC sounds, using original parent/scalar and accumulation branches; standalone body not checkable.
static void sUpdateRTPCs(
    SoundInstance * ptSound)
{
    if (ptSound->tSoundParams.snoSound == SNO_NONE || !ptSound->fHasRTPC)
        return;




    SigmaMemoryClear(ptSound->flRTPCAdjustment, sizeof(ptSound->flRTPCAdjustment));

    ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
    int32 nRTPCCount = pdefSound->serRTPCParams.dwSize / sizeof(RTPCParams);
    if (nRTPCCount == 0)
        return;




    InterpScalarParams tScalarParams;
    if (ptSound->tParentData.m_eParentType == PARENT_RACTOR)
    {
        RActor * ptRActor = RActorTryToGet((RActorID)ptSound->tParentData.m_idParent);
        if (ptRActor)
        {
            tScalarParams.InitParamsFromRActor(ptRActor);
        }
    }

    for (int32 i = 0; i < nRTPCCount; ++i)
    {
        RTPCParams & tParams = pdefSound->arRTPCParams[i];
        // Inference: failed scalar evaluation leaves a zero output, as original floating-zero branch shows.
        float flOutputVal = 0.0f;
        float flScalar;
        if (tScalarParams.Evaluate(tParams.tScalar.nScalarFunction, flScalar))
        {

            float flRemappedScalar = SoundCalculateFadeAttenuation(tParams.eCurveShape, tParams.flCurveShapeAccel, flScalar);

            Math::Lerp(tParams.tScalar.flScalarLerpZero, tParams.tScalar.flScalarLerpOne, flRemappedScalar, flOutputVal);
        }

        ptSound->flRTPCAdjustment[tParams.eTarget] += flOutputVal;
    }
}
#line 820
// Inference: ducking accumulates minimum attenuation; standalone body uncheckable. DWARF speeds61=-12/62=-9.
static void sUpdateDuckingAndRTPCs(
    float flDeltaTime)
{ // Inference: original minimum-duck accumulator starts at floating zero.
    float flDuckAmountDB = 0.0f;
    TDataArrayIterator<SoundInstance> tIt;
    while (gptSounds->Iterate(tIt))
    {
        SoundInstance * ptSound = tIt.ptElement;


        flDuckAmountDB = Math::Min(flDuckAmountDB, ptSound->flDuckAmountDB);

        if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT)))
        {
            sUpdateRTPCs(ptSound);
        }
    }

    BOOL fShouldDuck = flDuckAmountDB < -0.000001f;
    if (fShouldDuck)
    {
        float flDuckDeltaDB = DUCK_IN_SPEED * flDeltaTime;
        sgptGlobals->flDuckedVolumeDB = Math::Max(sgptGlobals->flDuckedVolumeDB + flDuckDeltaDB, flDuckAmountDB);
    }
    else
    {
        float flDuckDeltaDB = DUCK_OUT_SPEED * flDeltaTime;
        sgptGlobals->flDuckedVolumeDB = Math::Min(sgptGlobals->flDuckedVolumeDB - flDuckDeltaDB, 0.0f);
    }


    sgptGlobals->flDuckedVolumeAdjustedDB = sgptGlobals->flDuckedVolumeDB;


    if (sgptGlobals->flTypeVolume[SOUNDTYPE_NARRATION] < 0.5f)
    {
        sgptGlobals->flDuckedVolumeAdjustedDB = 0.0f;
    }

}
namespace SoundManager { void Reset(); } // Sound.cpp:705
#line 704
// Required by Update: standalone original body not checkable; four calls authenticated by its inline copy.
void SoundManager::Reset()
{

    ControllerStates tStates;
    SaveControllerStates(tStates);

    Dispose();
    Initialize();

    RestoreControllerStates(tStates);
}
#line 753
// Required by Update: standalone original body not checkable; valid game/parent guard follows inline evidence.
void SoundInstance::UpdatePropertiesFromParent(
    const float flDeltaTime)
{
    if (!CGameIsValid() ||
        !tParentData.ParentIsValid())
        return;



    PRTransform transform;
    tParentData.GetTransform(transform);

    SetPosition(transform.wp, flDeltaTime);
}
#line 863

void SoundManager::Update()

{
    if (!IsInitialized())
        return;



    if (SoundPlatform::ResetDevice())
    {
        Reset();
        return;
    }



    sgptGlobals->tStats.nPlayingSounds = gptSounds->GetTotalCount();

    float flDeltaTime = CGameGetRealDeltaTime();

    if (!(sgptGlobals->dwSuppressedTypes & (1 << SOUNDTYPE_AMBIENT)))
    {

        for (int32 i = 0; i < NUM_BACKGROUND_AMBIENT_CONTROLLERS; ++i)
        {
            sgptGlobals->tBkgrndAmbController[i].Update(flDeltaTime);
        }
    }
    if (!(sgptGlobals->dwSuppressedTypes & (1 << SOUNDTYPE_MUSIC)))
    {
        sgptGlobals->tMusicMgr.Update(flDeltaTime);
    }

    sUpdateDuckingAndRTPCs(flDeltaTime);


    TDataArrayIterator<SoundInstance> tIt;
    while (gptSounds->Iterate(tIt))
    {
        SoundInstance * ptSound = tIt.ptElement;

        if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT)))
        {
            if ((ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MOVING_BIT)) &&
                ptSound->tParentData.m_eParentType != PARENT_NONE)
            {

                ptSound->UpdatePropertiesFromParent(flDeltaTime);
            }


            if (ptSound->flDelay > 0.0f)
            {
                ptSound->flDelay = Math::Max(ptSound->flDelay - flDeltaTime, 0.0f);
                if (ptSound->flDelay == 0.0f)
                {
                    ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
                    BOOL fSuccess = sSoundTriggerInstance(ptSound, pdefSound, 0.0f);
                    if (!fSuccess)
                    {
                        gptSounds->ElementDeleteByID(ptSound->id);
                    }
                    continue;
                }
            }
        }

        if (ptSound->flDelay == 0.0f)
        {

            if (ptSound->flFadeOutStartTime != -1.0f &&
                CGameGetClientTime() > ptSound->flFadeOutStartTime + ptSound->flFadeOutTime)
            {
                SoundDelete(ptSound->id);
                continue;
            }


            if (ptSound->flFadeInStartTime != -1.0f &&
                CGameGetClientTime() > ptSound->flFadeInStartTime + ptSound->flFadeInTime)
            {
                ptSound->flFadeInStartTime = -1.0f;
            }

            float flVolume = SoundGetAdjustedVolume(ptSound);
            SoundPlatform::SetVolume(ptSound, flVolume);

            if (flVolume > 0.0f)
            {

                float flPitch = SoundGetAdjustedPitch(ptSound);
                SoundPlatform::SetPitch(ptSound, flPitch);
            }
        }
    }


    SoundPlatform::SetRetroFilter(sgptGlobals->fAllowRetroFilter && CWorldIsRetro());

    SoundPlatform::Update();
}
#line 58
// DWARF Sound.cpp:58 file-scope constant; src lacked it. Defined here before its only user.
const WorldDistance cwdNearPlayerThreshold = 25.0f;
// Inference: DWARF SoundDefinition.h:230 constant; declared here because SoundDefinition.h lacks it (no shared-header edit).
static const float cgflMinInverseFalloffDistance = 1.0f;
// DWARF SoundUtils.cpp:313; declared locally (same declaration as SoundPlatformFModEx.cpp:125).
int32 SelectSoundPermutation(const SoundDefinition * pdefSound, SoundPlayHistory * ptSoundPlayHistory);
#include "../CPlayer.h"
#line 1785
// Concrete DIE at low_pc 0 (linker-dropped copy) and external linkage: not declared inline.
float sComputeImpulseAttenuation(
    const float flImpulse,
    const SoundDefinition * pdefSound)
{
    float flImpulseFactor = 0.0f;


    if (flImpulse == 0.0f)
        return 0.0f;



    if (pdefSound->flImpulseDelta > 0.000001f)
    {
        flImpulseFactor = (flImpulse - pdefSound->flImpulseMin) / pdefSound->flImpulseDelta;
        flImpulseFactor = Math::Pin(flImpulseFactor, 0.0f, 1.0f);
    }

    return pdefSound->flImpulseAttenMinDB + flImpulseFactor * (pdefSound->flImpulseAttenMaxDB - pdefSound->flImpulseAttenMinDB);
}


// Inference: single-exit do/while(FALSE) block (DWARF lexical block of locals 1899-2016 ends before the delete at 2088, reached after leaving the bank-lock scope via the cleanup-dest slot); idSound starts invalid so early exits skip the delete.
// NON_MATCHING: 2668 vs 2652; original materialises fSuppressTracing as a 0/1 phi in w26 (threaded from the mute/paused paths, branch b.mi at 1854:93), ours keeps the float volume in s9 and re-compares at each use (+d11 spill); ternary and if-forms tried. IR: our fcmp of the inlined sGetTypeVolume phi is never folded into the phi (InstCombine FoldOpIntoPhi gives up in big CFGs), so the original likely branches on the compare in the phi block (an operator at col 93, after the constant); not found.
SoundID SoundManager::PlaySound(
    const SNO snoSound,
    const PlaySoundOptions & tOptions)
{


    if (!IsInitialized() || snoSound == SNO_NONE)
        return SOUNDID_INVALID;
#line 1845
    ScopedSoundDefinitionLockR pdefSound(snoSound);
    if (sgptGlobals->dwSuppressedTypes & (1 << pdefSound->eType))
        return SOUNDID_INVALID;



    SoundID idSound = SOUNDID_INVALID;
    const SoundPermutation * ptPermutation;

    BOOL fSuppressTracing = (sGetTypeVolume(pdefSound->eType, tOptions.eContext) < 0.000001f);

    do
    {
    if (pdefSound->dwFlags & (1 << SOUND_LIMIT_INSTANCES_BIT))
    {
        if (!sLimitInstances(pdefSound, tOptions.pwp, fSuppressTracing))
            break;
    }



    if (!sCheckCooldownTime(pdefSound, fSuppressTracing))
        break;




    if (pdefSound->nPermutationCount == 0)
        break;





    if (tOptions.pwp && (pdefSound->dwFlags & (1 << SOUND_DONT_PLAY_WHEN_NEAR_PLAYER_BIT)))
    {

        ACDID idPlayerACD = LocalPlayerACDGet();
        if (idPlayerACD != ACDID_INVALID)
        {
            ActorCommonData & tACDPlayer = ACDGet(idPlayerACD);
            float flDistSqr = Vector3D::DistanceSquared(*tOptions.pwp, tACDPlayer.GetWorldLocation().transform.wp);
            if (flDistSqr <= cwdNearPlayerThreshold * cwdNearPlayerThreshold)
            {
                if (!fSuppressTracing && // Inference: `&&` branch row at none (merged), call on the next line; empty body (trace output compiled out).
                    CGameGetDebugSoundMode())
                {
                }
                break;
            }
        }
    }


    int32 nPermutationIndexToUse = tOptions.nPermutationIndex;
    if (nPermutationIndexToUse == -1)
        // DWARF: SelectSoundPermutation takes a non-const SoundPlayHistory * while pdefSound is const.
        nPermutationIndexToUse = SelectSoundPermutation(pdefSound, const_cast<SoundPlayHistory *>(&pdefSound->tSoundPlayHistory));



    ptPermutation = &pdefSound->ptPermutations[nPermutationIndexToUse];

    if (tOptions.flStartOffset > 0.0f)
    {
        ScopedSoundBankDefinitionLockR pdefSoundBank(ptPermutation->snoSoundBank);
        SoundSample * ptSample = &pdefSoundBank->ptSoundSamples[ptPermutation->nSampleIndex];
        float flSampleLength = ptSample->ComputeDuration() * OO_TICKS_PER_SECOND;

        if (tOptions.flStartOffset > flSampleLength)
        {

            break;
        }
    }

    idSound = gptSounds->ElementNew();
    if (idSound == SOUNDID_INVALID)
    {
        if (!fSuppressTracing && // Inference: `&&` branch row at 1924:31, call on the next line; empty body (trace output compiled out).
            CGameGetDebugSoundMode())
        {
        }
        break;
    }


    SoundInstance * ptSound = SoundGet(idSound);

    if (tOptions.fAffectedByWeather)

        ptSound->dwFlags |= (1 << SOUNDINSTANCE_AFFECTED_BY_WEATHER_BIT);


    ptSound->fStitching = FALSE;
    ptSound->fHasRTPC = pdefSound->serRTPCParams.dwSize != 0;
    ptSound->tChannelState[0].Initialize();
    ptSound->tChannelState[1].Initialize();
    ptSound->nCurrentChannel = 0;

    SigmaMemoryClear(ptSound->flRTPCAdjustment, sizeof(ptSound->flRTPCAdjustment));
    ptSound->tSoundParams.snoSound = snoSound;
    ptSound->tSoundParams.nPermutationIndex = nPermutationIndexToUse;

    float flDistanceMin = pdefSound->wdFalloffRadiusMin;
    float flDistanceMax = pdefSound->wdFalloffRadiusMin + pdefSound->wdFalloffRadiusDelta;

    BOOL fPlayAs2D = FALSE;
    ptSound->flDopplerFactor = 0.0f;
    ptSound->flDuckAmountDB = Math::Min(pdefSound->flDuckAmountDB, 0.0f);
    ptSound->wvVel = cgwvNull;
    ptSound->dwStartTime = GetMachineTicks();
    ptSound->flDelay = ptPermutation->dwDelay * OO_TICKS_PER_SECOND;
    ptSound->flWantedStartTime = CGameGetClientTime();
    ptSound->flRealStartTime = 0.0f;


    if (tOptions.fForce2D) ptSound->dwFlags |= (1 << SOUNDINSTANCE_ENABLE_REVERB_BIT); else ptSound->dwFlags &= ~(1 << SOUNDINSTANCE_ENABLE_REVERB_BIT);

    float flForce2DVolumeCompensation = 0.0f;
    if (tOptions.pwp)
    {
        if (tOptions.fForce2D)
        {
            fPlayAs2D = TRUE;
            flForce2DVolumeCompensation = -6.0f;
        }
        else
        {


            ScopedSoundBankDefinitionLockR pdefSoundBank(ptPermutation->snoValidatedSoundBank);
            SoundSample & tSample = pdefSoundBank->ptSoundSamples[ptPermutation->nSampleIndex];
            BOOL fIsMono = SoundFormatIsMono(tSample.GetFormat());
            if (!fIsMono)
            {
                fPlayAs2D = TRUE;
            }
            else
            {




                float flDistSqr = Vector3D::DistanceSquared(*tOptions.pwp, sgptGlobals->wpVolume);
                if (flDistSqr >= (flDistanceMax * flDistanceMax))
                    break;
            }


        }

        ptSound->wp = *tOptions.pwp;
    }
    else
    {
        ptSound->wp = cgwpNull;
        fPlayAs2D = TRUE;
    }

    ptSound->flFadeAtten = tOptions.flInitialFadeAtten;
    ptSound->flFadeInTime = ptPermutation->dwFadeInTime * OO_TICKS_PER_SECOND;
    ptSound->flFadeOutTime = ptPermutation->dwFadeOutTime * OO_TICKS_PER_SECOND;
    ptSound->flFadeInStartTime = (ptSound->flFadeInTime > 0.000001f) ? CGameGetClientTime() : -1.0f;
    ptSound->flFadeOutStartTime = -1.0f;
    ptSound->eFadeInShape = ptPermutation->eFadeInShape;
    ptSound->eFadeOutShape = ptPermutation->eFadeOutShape;
    ptSound->flFadeInAccel = ptPermutation->flFadeInAccel;
    ptSound->flFadeOutAccel = ptPermutation->flFadeOutAccel;

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    float flImpulseAtten = sComputeImpulseAttenuation(tOptions.flImpulse, pdefSound);

    ptSound->flVolumeDB = pdefSound->flGlobalVolumeDB + flImpulseAtten + flForce2DVolumeCompensation;
    ptSound->flPitchSemitones = pdefSound->flGlobalPitch;


    if (CWorldIsRetro() &&
        (pdefSound->eType == SOUNDTYPE_EFFECT || pdefSound->eType == SOUNDTYPE_EFFECT_UI))
    {
        ptSound->flVolumeDB += (ptPermutation->flVolumeDBMin + ptPermutation->flVolumeDBDelta) * 0.5f;
        ptSound->flPitchSemitones += (ptPermutation->flPitchSemitonesMin + ptPermutation->flPitchSemitonesDelta) * 0.5f;
    }
    else
    {
        ptSound->flVolumeDB += RandRangeFloat_Internal(tCGameGlobals.rngLocal, ptPermutation->flVolumeDBMin, ptPermutation->flVolumeDBDelta);
        ptSound->flPitchSemitones += RandRangeFloat_Internal(tCGameGlobals.rngLocal, ptPermutation->flPitchSemitonesMin, ptPermutation->flPitchSemitonesDelta);
    }

    ptSound->tRolloffParams.eFalloffType = pdefSound->eFalloffType;
    ptSound->tRolloffParams.flDistanceMin = flDistanceMin;
    ptSound->tRolloffParams.flDistanceMax = flDistanceMax;
    ptSound->tRolloffParams.flRolloffFactor = pdefSound->flRolloffFactor;

    ptSound->dwPriority = pdefSound->dwPriority;
    ptSound->eType = pdefSound->eType;
    ptSound->eContext = tOptions.eContext;
    ptSound->tParentData.Init();

    if (pdefSound->dwFlags & (1 << SOUND_DONT_DUCK_BIT)) ptSound->dwFlags |= (1 << SOUNDINSTANCE_DONT_DUCK_BIT); else ptSound->dwFlags &= ~(1 << SOUNDINSTANCE_DONT_DUCK_BIT);
    if (pdefSound->dwFlags & (1 << SOUND_RANDOM_START_POSITION_BIT)) ptSound->dwFlags |= (1 << SOUNDINSTANCE_RANDOM_START_POSITION_BIT); else ptSound->dwFlags &= ~(1 << SOUNDINSTANCE_RANDOM_START_POSITION_BIT);
    if (!fPlayAs2D)
    {

        ptSound->dwFlags |= (1 << SOUNDINSTANCE_3D_BIT) | (1 << SOUNDINSTANCE_ENABLE_REVERB_BIT);
        if (pdefSound->dwFlags & (1 << SOUND_UPDATE_POSITION_BIT))
        {
            ptSound->dwFlags |= (1 << SOUNDINSTANCE_IS_MOVING_BIT);
            ptSound->flDopplerFactor = pdefSound->flDopplerFactor;
        }
    }
    else
    {

        ptSound->tRolloffParams.eFalloffType = FALLOFF_NONE;
    }

    if (ptSound->tRolloffParams.eFalloffType == FALLOFF_INVERSE_DISTANCE)
    {

        ptSound->tRolloffParams.flDistanceMin = Math::Max(ptSound->tRolloffParams.flDistanceMin, cgflMinInverseFalloffDistance);
        ptSound->tRolloffParams.flDistanceMax = Math::Max(ptSound->tRolloffParams.flDistanceMax, ptSound->tRolloffParams.flDistanceMin);
    }

    if (tOptions.fOccluded && !(pdefSound->dwFlags & (1 << SOUND_DONT_OCCLUDE_BIT)))


        ptSound->dwFlags |= (1 << SOUNDINSTANCE_OCCLUDED_BIT);


    if (ptSound->flDelay == 0.0f)
    {
        BOOL fSuccess = sSoundTriggerInstance(ptSound, pdefSound, tOptions.flStartOffset);
        if (!fSuccess)
            break;
    }
    return idSound;
    }
    while (FALSE);



    if (idSound != SOUNDID_INVALID)
        gptSounds->ElementDeleteByID(idSound);
    return SOUNDID_INVALID;

}
#line 2249
// LINKER-REMOVED: Sound.cpp:2254 SoundManager::PlaySample: concrete DIE at low_pc 0 (linker-dropped, 0x2f8 bytes), body from its
// DWARF locals/inlines and address-0 line-table rows. It is the second real caller of sSoundPlaySample.
// Inference (unverifiable, no original bytes): field stores 2271-2303 chosen from '=' columns and the PlayEntry sibling.
namespace SoundManager { SoundID PlaySample(const SNO snoSoundBank, const int32 nSampleIndex, SoundType eType); }

SoundID SoundManager::PlaySample(
    const SNO snoSoundBank,
    const int32 nSampleIndex,
    SoundType eType)
{
    if (!IsInitialized())
        return SOUNDID_INVALID;



    SoundID idSound = gptSounds->ElementNew();
    if (idSound == SOUNDID_INVALID)
        return SOUNDID_INVALID;



    SoundInstance * ptSound = SoundGet(idSound);
    SigmaMemoryClear(ptSound->flRTPCAdjustment, sizeof(ptSound->flRTPCAdjustment));
    ptSound->tSoundParams.snoSound = SNO_NONE;

    ptSound->flFadeAtten = 1.0f;
    ptSound->flVolumeDB = 0.0f;

    ptSound->tRolloffParams.eFalloffType = FALLOFF_NONE;

    ptSound->eContext = RENDERCONTEXT_DEFAULT;

    ptSound->fStitching = FALSE;
    ptSound->fHasRTPC = FALSE;
    ptSound->tChannelState[0].Initialize();
    ptSound->tChannelState[1].Initialize();


    ptSound->flDuckAmountDB = 0.0f;
    ptSound->eType = eType;

    ptSound->dwPriority = PRIORITY_HIGHEST;


    SoundPlaybackParams tParams;
    tParams.flVolume = SoundGetAdjustedVolume(ptSound);
    tParams.flPitch = SoundGetAdjustedPitch(ptSound);
    tParams.dwPriority = ptSound->dwPriority;

    tParams.eType = eType;
    tParams.f3D = FALSE;
    tParams.fDisableReverb = TRUE;
    tParams.fLoops = FALSE;
    tParams.fChooseNewPermutationOnLoop = FALSE;
    tParams.flStartOffset = 0.0f;

    ScopedSoundBankDefinitionLockR pdefSoundBank(snoSoundBank);
    if (nSampleIndex >= 0 && nSampleIndex < pdefSoundBank->nSoundSampleCount)
    {
        SoundSample & tSample = pdefSoundBank->ptSoundSamples[nSampleIndex];

        tParams.eStreamMode = SoundShouldBeStreamed(tSample);

        BOOL fSuccess = sSoundPlaySample(ptSound, snoSoundBank, tSample, tParams);
        if (!fSuccess)
        {
            gptSounds->ElementDeleteByID(idSound);
            return SOUNDID_INVALID;
        }
    }
    else
    {
        gptSounds->ElementDeleteByID(idSound);
        return SOUNDID_INVALID;
    }

    return idSound;
}
