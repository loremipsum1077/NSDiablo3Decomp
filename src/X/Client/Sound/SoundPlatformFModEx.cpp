// X/Client/Sound/SoundPlatformFModEx.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/r5c-sound/frag.cpp by build/try/r5-particle/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod_common.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod_dsp.h"
#include "../../../Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod_dsp_effects.h"
#include "../RActors/RActors.h"
#include "../Gfx/GfxVB.h"
#include "../CGame.h"
#include "../Cutscene.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/Console/IO/ConsoleIO.h"
#include "../Definitions/MusicDefinition.h"
#include "../Definitions/ReverbDefinition.h"
#include "../Definitions/SoundBankDefinition.h"
#include "../Definitions/SoundDefinition.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "Sound.h"
#include "SoundPlatform.h"
#include "SoundPlatformFModExAsync.h"
#include "../../Sigma/DataStructures/XFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XList.h"
#include "../../Sigma/DataStructures/XMap.h"
#include "../../Sigma/DataStructures/XTemplate.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/SigmaTiming.h"
#include <float.h>
#include "../../Sigma/UnicodeText.h"
#include "../../Common/Player.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/WorldPlace.h"
#include "../CPlayer.h"

// X/Client/Sound/Sound.cpp (not in Sound.h yet).
namespace SoundManager {
BOOL GetGlobalMute();
}

namespace Console { namespace IO { class CPKArchiveReader; } }
struct ReverbParams;
struct ReverbParamsExtended;

class AsyncAudio;












// X/Client/Sound/CheapAmpModelDSP (the retro filter DSP).
namespace CheapAmpModelDSP {
BOOL gGetCheapAmpModelDesc(FMOD_DSP_DESCRIPTION * ptFmodDSPDescription);
}

// FMOD thread affinity (platform code).
void ConfigureFMODThreads();

// X/Sigma/Memory/SigmaMemory.cpp.
extern void * gpFMODMemoryBuffer;
extern uint32 gdwFMODMemoryBufferSize;


// SoundPlatform::Preload calls CPKArchiveReader::GetLocation (original CU references it).
#include "../../Common/Console/IO/ConsoleIO_ArchiveReader.h"





static AsyncAudio * sptAsyncAudio;

// BlizzardCore Storm file handle (SFileWrapper.cpp).
struct HSFILERecord;
typedef HSFILERecord * HSFILE;
bool SFileReadFilePriority(HSFILERecord * pFile, void * pBuffer, unsigned int dwBytesToRead, unsigned int * pdwBytesRead, Console::IO::TaskPriorities ePriority);

#include "../../Common/FileIO/FileIO.h"
namespace FileIO {

















int32 GetLastErrorCode();
}  // namespace FileIO

// Sleep: X/Sigma/SigmaTiming.h:67.






// X/Client/Cutscene.cpp:2108, 2153.
CutsceneID CutsceneGetFullScreenCutscene();
void CutsceneMarkToBeStopped(const CutsceneID idCutscene);

// X/Client/Sound/SoundUtils.cpp:313.
int32 SelectSoundPermutation(const SoundDefinition * pdefSound, SoundPlayHistory * ptSoundPlayHistory);

// X/Client/Sound/SoundUtils.cpp:652.
float SoundGetRolloffAtten(const float flDist, const RolloffParams & tRolloffParams);

// X/Client/Sound/Sound.cpp:2620.
float SoundGetAdjustedVolume(const SoundInstance * ptSound);

// X/Client/Sound/SoundUtils.cpp:584, 606.
uint32 SelectLoopingSoundStartSample(const BOOL fRandomStartPosition, SoundSample & tSoundSample);
float GetDspParamValue(DspParam & tParam);
// X/Client/Sound/Sound.cpp:2685.
float SoundGetAdjustedPitch(const SoundInstance * ptSound);














































static const uint32 FMOD_SOFTWARE_IF_BEFORE_FMOD5 = 0;








float gf3DSoundSpread = 55.0f;



float gf3DPanEndVolume = 1.0f;


BOOL gb3DPanEnabled;
BOOL gbForce5_1Enabled = TRUE;





BOOL gbReverbEnabled = TRUE;











































enum CachedSoundType {
    FMOD_CACHED_SOUND_STREAM = 0,
    FMOD_CACHED_SOUND_SAMPLE = 1,
};

struct FModCachedSoundKey {  // size 0x8
    SNO snoSoundBank;  // 0x00
    int32 nSampleIndex;  // 0x04
    BOOL operator==(const FModCachedSoundKey & rhs) const
    {
        return snoSoundBank == rhs.snoSoundBank &&
               nSampleIndex == rhs.nSampleIndex;
    }
};

template <>
inline uint32 XHashKey<const FModCachedSoundKey &>(const FModCachedSoundKey & key)
{
    uint8 * bKey = (uint8 *)&key;
    uint32 hash = 0x811C9DC5;
    for (uint32 i = 0; i < sizeof(FModCachedSoundKey); i++)
    {


        hash ^= bKey[i];
        hash *= 0x01000193;
    }
    return hash;
}

struct FModCachedSound {  // size 0x38
    FMOD_SOUND * ptFModParentSound;  // 0x00
    FMOD_SOUND * ptFModSound;  // 0x08
    BOOL fPending;  // 0x10
    BOOL fIsFSB;  // 0x14
    BOOL fAlreadyReloaded;  // 0x18
    BOOL fPreloading;  // 0x1c
    int32 nRefCount;  // 0x20
    CachedSoundType eSoundType;  // 0x24
    POS tLRUPos;  // 0x28
    FModCachedSound * ptNext;  // 0x30
};

typedef XMap<FModCachedSoundKey, const FModCachedSoundKey &, FModCachedSound *, FModCachedSound *, 2048> FModCachedSoundSampleMap;
typedef XMap<FModCachedSoundKey, const FModCachedSoundKey &, FModCachedSound *, FModCachedSound *, 32> FModCachedSoundStreamMap;
typedef XFixedBlockAllocator<FModCachedSound, 2176> FModCachedSoundAllocator;
typedef XList<FModCachedSoundKey, FModCachedSoundKey, 2048> FModCachedSoundList;


















































static FMOD_DSP_TYPE sgarFModDspTypeConvert[DSP_NUM_TYPES] = {
    FMOD_DSP_TYPE_CHORUS,
    FMOD_DSP_TYPE_COMPRESSOR,
    FMOD_DSP_TYPE_DISTORTION,
    FMOD_DSP_TYPE_ECHO,
    FMOD_DSP_TYPE_FLANGE,
    FMOD_DSP_TYPE_HIGHPASS,
    FMOD_DSP_TYPE_LOWPASS,
    FMOD_DSP_TYPE_LOWPASS_SIMPLE,
    FMOD_DSP_TYPE_PARAMEQ,
    FMOD_DSP_TYPE_SFXREVERB,
};

struct FModPooledDSP {  // size 0x10
    FMOD_DSP * ptFModDsp;  // 0x00
    BOOL fDspInUse;  // 0x08
};

struct FModDSPPool {  // size 0x18
    FModPooledDSP * ptFModDspPool;  // 0x00
    int32 nPoolCount;  // 0x08
    int32 nCurUseCount;  // 0x0c
    int32 nMaxUseCount;  // 0x10
};



















static FModDSPPool sgarFModDspPools[DSP_NUM_TYPES] = {
    { NULL, 4 },
    { NULL, 2 },
    { NULL, 8 },
    { NULL, 4 },
    { NULL, 2 },
    { NULL, 32 },
    { NULL, 16 },
    { NULL, 16 },
    { NULL, 4 },
    { NULL, 2 },
};







static struct {  // size 0xe8
    FMOD_DSP_DESCRIPTION tFmodDSPDescription;  // 0x00
    FMOD_DSP * ptFModDsp;  // 0xd8
    BOOL fDspConnected;  // 0xe0


} sFModDspRetroFilterPool;



enum {
    FMOD_DSP_FLAG_ENABLED = 0,
};

struct FModDSPEffect {  // size 0x10
    FMOD_DSP * ptFModDsp;  // 0x00
    FMOD_DSP_TYPE eEffectType;  // 0x08
    uint32 dwFlags;  // 0x0c
};

struct PlayingSoundData {  // size 0xd8






    void Clear()
    {

        idSound = SOUNDID_INVALID;

        ptFModChannel = NULL; ptCachedSound = NULL; ptFModSound = NULL;

        flAppliedVolume = 1.0f; flDefaultFrequency = 0.0f;
    }


    SoundID idSound;  // 0x00
    FMOD_SOUND * ptFModSound;  // 0x08
    FMOD_CHANNEL * ptFModChannel;  // 0x10
    FModCachedSound * ptCachedSound;  // 0x18
    FModDSPEffect arDspEffects[10];  // 0x20
    float flAppliedVolume;  // 0xc0
    float flDefaultFrequency;  // 0xc4


    RolloffParams tRolloffParams;  // 0xc8
};






struct SoundDriverSettings {  // size 0x104
    CHAR szName[256];  // 0x00
    FMOD_SPEAKERMODE eSpeakerMode;  // 0x100
};

struct FModSampleToPreload {  // size 0x18
    FModCachedSoundKey info;  // 0x00
    Console::IO::CPKArchiveReader * ptArchive;  // 0x08
    int64 nLocation;  // 0x10
};

struct SoundPlatformFModGlobals {  // size 0xb40
    SoundPlatformFModGlobals();
    BOOL bCustomListenerMode;  // 0x00
    BOOL bInGameMusicEnabled;  // 0x04
    FMOD_CHANNELGROUP * ptMusicChannelGroup;  // 0x08
    float flLastMusicVolume;  // 0x10
    typedef XGrowableMap<int, int, int, int> FModSoundInstanceMap;
    SoundPlatformFModGlobals::FModSoundInstanceMap tSoundInstanceMap;  // 0x18
    Blizzard::Lock::RMutex tPreloadListLock;  // 0x98
    XGrowableList<FModSampleToPreload, const FModSampleToPreload &> pendingPreloadList;  // 0xb8
    XGrowableMap<FModCachedSoundKey, const FModCachedSoundKey &, int, int> pendingPreloadMap;  // 0x110
    Vector3D vPlayerPositions[4];  // 0x190
    uint32 dwPlayerCount;  // 0x1c0
    FMOD_CHANNELGROUP * ptMasterChannelGroup;  // 0x1c8
    float flLastMasterVolume;  // 0x1d0
    Vector3D wpVolumeListener;  // 0x1d4
    BOOL fCustomVolumeListener;  // 0x1e0
    BOOL bResetDevice;  // 0x1e4
    int32 nNumDrivers;  // 0x1e8
    SoundDriverSettings tSoundDriverSettings[8];  // 0x1ec
    int32 nDriverToUse;  // 0xa0c
    FMOD_SYSTEM * ptFModSystem;  // 0xa10
    void * pFModMemory;  // 0xa18
    int32 nFModMemoryAmount;  // 0xa20
    int32 nNumChannels;  // 0xa24
    FModCachedSoundAllocator * ptCachedSoundAllocator;  // 0xa28
    FModCachedSoundSampleMap * ptCachedSoundSampleMap;  // 0xa30
    FModCachedSoundStreamMap * ptCachedSoundStreamMap;  // 0xa38
    FModCachedSoundList * ptLRUSoundSampleList;  // 0xa40
    FModCachedSoundList * ptLRUSoundStreamList;  // 0xa48
    PlayingSoundData * arPlayingSounds;  // 0xa50
    PlayingSoundData tOVTStreamSound;  // 0xa58
    int32 nCacheSampleSize;  // 0xb30
    int32 nCacheStreamSize;  // 0xb34
    int32 nFModCurrentAlloced;  // 0xb38
    int32 nFModMaxAlloced;  // 0xb3c
};
static_assert(sizeof(SoundPlatformFModGlobals) == 0xb40, "SoundPlatformFModGlobals");
FModCachedSound * sReloadSound(FModCachedSound *, SoundInstance *, const SNO, int32); // DWARF SoundPlatformFModEx.cpp:3894
static BOOL sCheckResult(FMOD_RESULT result, LPCSTR szMessage, LPCSTR szParam);
inline void sUpdateMemoryStats(FMOD_BOOL blocking);
void sReleaseFModSound(FMOD_SOUND * ptFModSound, BOOL fForceRelease);
void sReleaseSound(FModCachedSound * ptCachedSound, BOOL fRemoveFromCache);
static void sGarbageCollectFModMemory(BOOL bForceGarbageCollect);
static void sFreeSound(SoundInstance * ptSound);
static float sFMod3DRolloffCallback(FMOD_CHANNELCONTROL * channelControl, float wdDist);
static void sReleaseChannel(PlayingSoundData * ptPlayingSoundData);
static BOOL sPurgeSoundSample(const FModCachedSoundKey & tKey, const BOOL fSuppressTracing, const BOOL fForceRelease);
static BOOL sPurgeLowestPrioritySoundSample(const BOOL fSuppressTracing);
static BOOL sPurgeSoundStream(const FModCachedSoundKey & tKey, const BOOL fForceRelease);
static BOOL sPurgeLowestPrioritySoundStream();
static PlayingSoundData * sGetPlayingSoundData(const SoundInstance * ptSound, int32 nWhichChannel);
static FMOD_RESULT sFileOpen(const char * name, uint32 * filesize, void ** handle, void * userdata);
static FMOD_RESULT sFileClose(void * handle, void * userdata);
static FMOD_RESULT sFileRead(void * handle, void * buffer, uint32 sizebytes, uint32 * bytesread, void * userdata);
static FMOD_RESULT sFileSeek(void * handle, uint32 pos, void * userdata);
FModCachedSound * sReloadMusic(FModCachedSound *, PlaylistEntry &, const float); // DWARF SoundPlatformFModEx.cpp:3915
static BOOL sPlaySound(FModCachedSound * ptCachedSound, SoundInstance * ptSound, const SNO snoSoundBank, int32 nWhichChannel, SoundSample & tSoundSample);
static BOOL sLoadAndPlaySound(SoundInstance * ptSound, const SNO snoSoundBank, int32 nWhichChannel, SoundSample & tSoundSample);
static FModCachedSound * sGetSound(const SNO snoSoundBank, const int32 nSampleIndex, const CachedSoundType eSoundType, const BOOL fSuppressTracing, BOOL fPreload);
static FMOD_SOUND * sCreateFModSound(const SNO snoSoundBank, const uint32 nSampleIndex, const StreamMode eStreamMode, const BOOL fSuppressTracing, FMOD_SOUND ** ptParentFModSound, BOOL * pfPending, BOOL * pfIsFSB, BOOL fPreload);
BOOL sPlayMusic(FModCachedSound *, SoundInstance *, PlaylistEntry &, const uint32, const float); // DWARF SoundPlatformFModEx.cpp:4335
#define SOUNDPLATFORMFMODEX_INCLUDES_ASYNC
#include "SoundPlatformFModExAsync.cpp"
































































static SoundPlatformFModGlobals sFModSoundGlobals;

static BOOL sfDumpedDeviceInfo;





























































































































inline void sRemoveSoundInstance(SNO snoSound)
{


    int32 * pnInstanceCount = sFModSoundGlobals.tSoundInstanceMap.LookupPtr(snoSound);

    if (*pnInstanceCount > 1)
    {
        (*pnInstanceCount)--;
    }
    else
    {
        sFModSoundGlobals.tSoundInstanceMap.RemoveAt(snoSound);
    }
}


void sAddSoundInstance(SNO snoSound)
{

    int32 * pnInstanceCount = sFModSoundGlobals.tSoundInstanceMap.LookupPtr(snoSound);
    if (pnInstanceCount)
    {
        (*pnInstanceCount)++;
    }
    else
    {
        sFModSoundGlobals.tSoundInstanceMap.SetAt_Tracked(snoSound, 1);
    }
}



inline int32 sGetFmodEmergencyMemory()
{
    return CmdLineGetParams()->uFmodEmergencyMemory ? CmdLineGetParams()->uFmodEmergencyMemory : 32 * 1024 * 1024;
}







static void sWaitForReady(FMOD_SOUND * ptFModSound)
{
    for (int32 i = 0; i < 200; i++)
    {
        FMOD_OPENSTATE eOpenState;
        FMOD_RESULT result = FMOD_Sound_GetOpenState(ptFModSound, &eOpenState, NULL, NULL, NULL);
        if (result == FMOD_OK && (eOpenState == FMOD_OPENSTATE_READY || eOpenState == FMOD_OPENSTATE_PLAYING))
            break;
        Sleep(1);
    }
}



















FMOD_RESULT sFMODChannelSetPosition(FMOD_SOUND * ptFModSound, FMOD_CHANNEL * ptFModChannel, unsigned int position, FMOD_TIMEUNIT postype)
{

    // 2.7.7: no sWaitForReady here (SetStreamPosition tail-calls FMOD_Channel_SetPosition).
    return FMOD_Channel_SetPosition(ptFModChannel, position, postype);
}








inline void sUpdateMemoryStats(FMOD_BOOL blocking)
{
    FMOD_Memory_GetStats(&sFModSoundGlobals.nFModCurrentAlloced, &sFModSoundGlobals.nFModMaxAlloced, blocking);
}



inline SoundPlatformFModGlobals::SoundPlatformFModGlobals()
    : ptFModSystem(NULL), pFModMemory(NULL), nFModMemoryAmount(0),

      ptCachedSoundStreamMap(NULL), ptLRUSoundSampleList(NULL), ptLRUSoundStreamList(NULL), arPlayingSounds(NULL),



      nCacheSampleSize(0), nCacheStreamSize(0), nFModCurrentAlloced(0), nFModMaxAlloced(0)
{


















    tOVTStreamSound.idSound = SOUNDID_INVALID;

    tOVTStreamSound.ptFModSound = NULL; tOVTStreamSound.ptFModChannel = NULL; tOVTStreamSound.ptCachedSound = NULL;


    tOVTStreamSound.tRolloffParams.eFalloffType = 0; tOVTStreamSound.tRolloffParams.flRolloffFactor = 0.0f; tOVTStreamSound.tRolloffParams.flDistanceMin = 0.0f; tOVTStreamSound.tRolloffParams.flDistanceMax = 0.0f;




    bCustomListenerMode = FALSE;
}



FMOD_DSP * sAcquireFModDSP(
    DspType eDspType,
    SNO snoSound)
{








    FModDSPPool & pool = sgarFModDspPools[eDspType];
    int32 nCount = pool.nPoolCount;
    for (int32 i = 0; i < nCount; i++)
    {
        if (pool.ptFModDspPool[i].fDspInUse == FALSE)
        {
            pool.ptFModDspPool[i].fDspInUse = TRUE;
            pool.nCurUseCount++;
            return pool.ptFModDspPool[i].ptFModDsp;
        }
    }

    pool.nCurUseCount++;
    if (pool.nCurUseCount > pool.nMaxUseCount)
    {


        pool.nMaxUseCount = pool.nCurUseCount;
    }
    // Pool exhausted: callers treat (FMOD_DSP *)-1 as "counted but no DSP" (sReleaseFModDSP line 906).
    return (FMOD_DSP *)-1;
}
void sReleaseFModDSP(DspType eDspType, FMOD_DSP * ptFModDsp)
{

    if (ptFModDsp == (FMOD_DSP *)-1)
    {
        sgarFModDspPools[eDspType].nCurUseCount--;
        return;
    }


    FModDSPPool & pool = sgarFModDspPools[eDspType];
    int32 nCount = pool.nPoolCount;
    for (int32 i = 0; i < nCount; i++)
    {
        if (pool.ptFModDspPool[i].ptFModDsp == ptFModDsp)
        {

            pool.ptFModDspPool[i].fDspInUse = FALSE; pool.nCurUseCount--;
            return;
        }
    }
}












static void sCleanupFMod()
{
    FMOD_System_Release(sFModSoundGlobals.ptFModSystem);
    sFModSoundGlobals.ptFModSystem = NULL;
    delete sptAsyncAudio;
    sptAsyncAudio = NULL;
}



static BOOL sFModMemoryIsCritical()
{
    return sFModSoundGlobals.nFModCurrentAlloced > sFModSoundGlobals.nFModMemoryAmount - sGetFmodEmergencyMemory();
}


static void sDumpFModCachedSoundStreamMap()
{
    int32 nTotalEntries = 0;
    POS pos = sFModSoundGlobals.ptCachedSoundStreamMap->GetStartAssoc();




    while (pos)
    {
        char szFileName[4096];
        FModCachedSoundKey tKey;
        FModCachedSound * ptFModCachedSound;
        sFModSoundGlobals.ptCachedSoundStreamMap->GetNextAssoc(pos, tKey, ptFModCachedSound);


        SoundBankGetSampleFullFilename(szFileName, sizeof(szFileName), tKey.snoSoundBank, tKey.nSampleIndex);




        while (ptFModCachedSound)
        {


            nTotalEntries++;
            ptFModCachedSound = ptFModCachedSound->ptNext;
        }
    }
}


















































































































































































































































// NON_MATCHING: 420 vs 420 bytes, 1 insn: orr operands swapped at 1230 (critical|force vs force|critical). Locals sit at function scope in the DWARF, so the early-return form is kept (col 34 = && after !sFModMemoryIsCritical()); tried == FALSE, !(a||b), swapped operands, positive block
static void sGarbageCollectFModMemory(
    BOOL bForceGarbageCollect)
{
    sUpdateMemoryStats(FALSE);
    if (!sFModMemoryIsCritical() && !bForceGarbageCollect)
    {
        return;
    }





    uint32 nSamplesToFree = 8;

    uint32 nStreamsFreed = 0;
    uint32 nSamplesFreed = 0;

    while (nSamplesToFree > 0 || bForceGarbageCollect)
    {
        if (!sPurgeLowestPrioritySoundStream())
        {
            break;
        }
        nStreamsFreed++;
        nSamplesToFree--;
    }


    while (nSamplesToFree > 0 || bForceGarbageCollect)
    {
        if (!sPurgeLowestPrioritySoundSample(FALSE))
        {
            break;
        }
        nSamplesFreed++;
        nSamplesToFree--;
    }

    sUpdateMemoryStats(FALSE);











    // nStreamsFreed/nSamplesFreed have no DWARF location (only read by compiled-out logging); original compares nSamplesToFree with 8 (cmp w22, #0x8 at 1278:59)
    if (nSamplesToFree == 8 && sFModMemoryIsCritical())
    {
        return;
    }








    if (sFModSoundGlobals.nFModCurrentAlloced >= sFModSoundGlobals.nFModMemoryAmount - sGetFmodEmergencyMemory() && !bForceGarbageCollect)
    {
        sGarbageCollectFModMemory(FALSE);
    }
}



















































































































static FMOD_RESULT sCreateFModSoundCheckMemory(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND ** sound)
{





    sGarbageCollectFModMemory(FALSE);

    FMOD_RESULT eRes = FMOD_System_CreateSound(system, name_or_data, mode, exinfo, sound);

    if (eRes == FMOD_ERR_MEMORY)
    {


        sGarbageCollectFModMemory(TRUE);
        eRes = FMOD_System_CreateSound(system, name_or_data, mode, exinfo, sound);
    }
    return eRes;
}








static FMOD_RESULT sCreateFModStreamCheckMemory(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND ** sound)
{
    sGarbageCollectFModMemory(FALSE);
    FMOD_RESULT eRes = FMOD_System_CreateStream(system, name_or_data, mode, exinfo, sound);
    if (eRes == FMOD_ERR_MEMORY)
    {
        sGarbageCollectFModMemory(TRUE);
        eRes = FMOD_System_CreateStream(system, name_or_data, mode, exinfo, sound);
    }
    return eRes;
}
















static CRefString sFMODSpeakerModeToString(const FMOD_SPEAKERMODE eSpeakerMode)
{
    CRefString szSpeakerMode;
    switch (eSpeakerMode)
    {
        case FMOD_SPEAKERMODE_MONO:
            szSpeakerMode = "Mono";
            break;
        case FMOD_SPEAKERMODE_STEREO:
            szSpeakerMode = "Stereo";
            break;
        case FMOD_SPEAKERMODE_QUAD:
            szSpeakerMode = "Quad";
            break;
        case FMOD_SPEAKERMODE_SURROUND:
            szSpeakerMode = "Surround";
            break;
        case FMOD_SPEAKERMODE_5POINT1:
            szSpeakerMode = "5.1";
            break;
        case FMOD_SPEAKERMODE_7POINT1:
            szSpeakerMode = "7.1";
            break;
        default:
            szSpeakerMode = "Default";
            break;
    }
    return szSpeakerMode;
}



static FMOD_SPEAKERMODE ConvertToFmodSpeakerMode(const SpeakerModes eSpeakerMode)
{
    switch (eSpeakerMode)
    {
        case 1:
            return FMOD_SPEAKERMODE_MONO;
        case 2:
            return FMOD_SPEAKERMODE_STEREO;
        case 3:
            return FMOD_SPEAKERMODE_QUAD;
        case 4:
            return FMOD_SPEAKERMODE_SURROUND;
        case 5:
            return FMOD_SPEAKERMODE_5POINT1;
        case 6:
            return FMOD_SPEAKERMODE_7POINT1;
        default:
            return FMOD_SPEAKERMODE_DEFAULT;
    }
}



static BOOL sSelectDriver(
    const int32 nDriver,
    const SpeakerModes eSpeakerMode)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_NO_SOUND_BIT))
        return FALSE;




    if (sFModSoundGlobals.nNumDrivers <= 0)
        return FALSE;




    sFModSoundGlobals.nDriverToUse = Math::Min(nDriver, sFModSoundGlobals.nNumDrivers);

    const SoundDriverSettings & tDriverSettings = sFModSoundGlobals.tSoundDriverSettings[sFModSoundGlobals.nDriverToUse];




    FMOD_RESULT result;
    result = FMOD_System_SetDriver(sFModSoundGlobals.ptFModSystem, sFModSoundGlobals.nDriverToUse);
    sCheckResult(result, "FMOD_System_SetDriver", NULL);

    FMOD_SPEAKERMODE eFmodSpeakerMode = ConvertToFmodSpeakerMode(eSpeakerMode);






    const BOOL fSuccess = TRUE;














    CRefString sSpeakerMode = sFMODSpeakerModeToString(eFmodSpeakerMode);








    ErrorManagerAddCrashMetadata("Sound_Speaker_Mode", sSpeakerMode.str());

    if (strstr(tDriverSettings.szName, "SigmaTel"))
    {

        result = FMOD_System_SetSoftwareFormat(sFModSoundGlobals.ptFModSystem, 48000, eFmodSpeakerMode, 0);



        sCheckResult(result, "FMOD_System_SetSoftwareFormat", NULL);
    }
    return fSuccess;
}



static void sDumpSoundPlatformFModGlobals()
{























    sUpdateMemoryStats(TRUE);
}






static void sInitDeviceInfo()
{
    if (sfDumpedDeviceInfo)
        return;
    sfDumpedDeviceInfo = TRUE;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_NO_SOUND_BIT))
        sFModSoundGlobals.nNumDrivers = 0;
    else
        FMOD_System_GetNumDrivers(sFModSoundGlobals.ptFModSystem, &sFModSoundGlobals.nNumDrivers);
}














































































































FMOD_RESULT systemcallback(FMOD_SYSTEM * system, FMOD_SYSTEM_CALLBACK_TYPE type, void * commanddata1, void * commanddata2, void * userdata)
{
    if (type == FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED || type == FMOD_SYSTEM_CALLBACK_DEVICELOST)
    {
        sFModSoundGlobals.bResetDevice = TRUE;
        sfDumpedDeviceInfo = FALSE;
    }
    return FMOD_OK;
}












































BOOL SoundPlatform::ResetDevice()
{
    if (sFModSoundGlobals.bResetDevice)
    {
        sFModSoundGlobals.bResetDevice = FALSE;
        return TRUE;
    }
    return FALSE;
}




void SoundPlatform::ReleaseMemory()
{
}





















void * sUserMemAlloc(unsigned int size, FMOD_MEMORY_TYPE type, const char * sourcestr)
{
    if (type == FMOD_MEMORY_PERSISTENT)
        return NULL;
    return malloc(size);
}















void * sUserMemRealloc(void * ptr, unsigned int size, FMOD_MEMORY_TYPE type, const char * sourcestr)
{
    if (type == FMOD_MEMORY_PERSISTENT)
        return NULL;
    return realloc(ptr, size);
}













void sUserMemFree(void * ptr, FMOD_MEMORY_TYPE type, const char * sourcestr)
{
    if (type == FMOD_MEMORY_PERSISTENT)
        return;
    free(ptr);
}














BOOL SoundPlatform::Initialize(
    const SoundFormat eSoundFormat,
    const int32 nNumChannels,
    const uint32 dwDriverHash,
    const SpeakerModes eSpeakerMode)
{

    sptAsyncAudio = new AsyncAudio;



    sFModSoundGlobals.bInGameMusicEnabled = TRUE;



    sFModSoundGlobals.pendingPreloadMap.SetHashTableSize_Tracked(256);











    sFModSoundGlobals.bResetDevice = FALSE;

    FMOD_RESULT result;

    uint32 channels, freq, bitsPerSample;
    SoundFormatToCSB(eSoundFormat, channels, freq, bitsPerSample);






    sFModSoundGlobals.nFModMemoryAmount = gdwFMODMemoryBufferSize;
    sFModSoundGlobals.pFModMemory = gpFMODMemoryBuffer;
    // DWARF: dwType is the constant 0 over the whole function (no FMOD_MEMORY_* name for 0 in src).
    FMOD_MEMORY_TYPE dwType = 0;

    if (sFModSoundGlobals.pFModMemory == NULL)
    {
        result = FMOD_Memory_Initialize(NULL, 0, sUserMemAlloc, sUserMemRealloc, sUserMemFree, dwType);
    }
    else
    {
        result = FMOD_Memory_Initialize(sFModSoundGlobals.pFModMemory, sFModSoundGlobals.nFModMemoryAmount, NULL, NULL, NULL, dwType);
    }
    if (!sCheckResult(result, "FMOD_Memory_Initialize", NULL))
    {
        delete sptAsyncAudio;
        sptAsyncAudio = NULL;
        return FALSE;
    }






















    result = FMOD_System_Create(&sFModSoundGlobals.ptFModSystem);
    if (!sCheckResult(result, "FMOD_System_Create", NULL))
    {
        delete sptAsyncAudio;
        sptAsyncAudio = NULL;
        return FALSE;
    }



    result = FMOD_System_SetFileSystem(sFModSoundGlobals.ptFModSystem, sFileOpen, sFileClose, sFileRead, sFileSeek, NULL, NULL, -1);
    if (!sCheckResult(result, "FMOD_System_SetFileSystem", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }


    result = FMOD_System_SetCallback(sFModSoundGlobals.ptFModSystem, systemcallback, FMOD_SYSTEM_CALLBACK_ALL);



    if (!sCheckResult(result, "FMOD_System_SetCallback", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }
    // Inference (2009): the negated && form; "!a || b" re-tests the inlined sCheckResult result after the version compare, the original does not.
    uint32 nFModDLLVersion;
    result = FMOD_System_GetVersion(sFModSoundGlobals.ptFModSystem, &nFModDLLVersion);
    if (!(sCheckResult(result, "FMOD_System_GetVersion", NULL) && nFModDLLVersion == FMOD_VERSION))
    {

        sCleanupFMod();
        return FALSE;
    }

    sInitDeviceInfo();

    int32 nDriverIndexToUse = 0;
    if (dwDriverHash)
    {

        for (int32 i = 0; i < sFModSoundGlobals.nNumDrivers; i++)
        {
            uint32 dwHash = XHashKey<const char *>(sFModSoundGlobals.tSoundDriverSettings[i].szName);
            if (dwHash == dwDriverHash)
            {
                nDriverIndexToUse = i;
                break;
            }
        }
    }
    if (!sSelectDriver(nDriverIndexToUse, eSpeakerMode))
    {
        result = FMOD_System_SetOutput(sFModSoundGlobals.ptFModSystem, FMOD_OUTPUTTYPE_NOSOUND);
        if (!sCheckResult(result, "FMOD_System_SetOutput", NULL))
            return FALSE;
    }




    FMOD_SPEAKERMODE eFmodSpeakerMode = ConvertToFmodSpeakerMode(eSpeakerMode);
    result = FMOD_System_SetSoftwareFormat(sFModSoundGlobals.ptFModSystem, freq, eFmodSpeakerMode, 0);
    if (!sCheckResult(result, "FMOD_System_SetSoftwareFormat", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }

























    result = FMOD_System_SetSoftwareChannels(sFModSoundGlobals.ptFModSystem, nNumChannels);
    if (!sCheckResult(result, "FMOD_System_SetSoftwareChannels", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }


    uint32 dwInitFlags = FMOD_INIT_NORMAL;




    void * pExtraDriverData = NULL;



    ConfigureFMODThreads();


    result = FMOD_System_Init(sFModSoundGlobals.ptFModSystem, 128, dwInitFlags, pExtraDriverData);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {


        result = FMOD_System_SetSoftwareFormat(sFModSoundGlobals.ptFModSystem, 48000, FMOD_SPEAKERMODE_STEREO, 0);
        if (!sCheckResult(result, "FMOD_System_SetSoftwareFormat", NULL))
        {
            sCleanupFMod();
            return FALSE;
        }




        result = FMOD_System_Init(sFModSoundGlobals.ptFModSystem, 128, dwInitFlags, pExtraDriverData);
        if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
        {

            result = FMOD_System_SetOutput(sFModSoundGlobals.ptFModSystem, FMOD_OUTPUTTYPE_WINMM);
            if (!sCheckResult(result, "FMOD_System_SetOutput", NULL))
            {
                sCleanupFMod();
                return FALSE;
            }

            result = FMOD_System_Init(sFModSoundGlobals.ptFModSystem, 128, dwInitFlags, pExtraDriverData);
        }
    }

    if (!sCheckResult(result, "FMOD_System_Init", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }















    result = FMOD_System_SetStreamBufferSize(sFModSoundGlobals.ptFModSystem, 64 * 1024, FMOD_TIMEUNIT_RAWBYTES);
    if (!sCheckResult(result, "FMOD_System_SetStreamBufferSize", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }


    result = FMOD_System_Set3DRolloffCallback(sFModSoundGlobals.ptFModSystem, sFMod3DRolloffCallback);
    if (!sCheckResult(result, "FMOD_System_Set3DRolloffCallback", NULL))
    {
        sCleanupFMod();
        return FALSE;
    }


    sFModSoundGlobals.nNumChannels = 128;

    sFModSoundGlobals.arPlayingSounds = (PlayingSoundData *)SigmaMemoryNew(sFModSoundGlobals.nNumChannels * sizeof(PlayingSoundData), 4, NULL, TRUE);


    sFModSoundGlobals.tOVTStreamSound.idSound = SOUNDID_INVALID;



    sFModSoundGlobals.ptCachedSoundSampleMap = new FModCachedSoundSampleMap();
    sFModSoundGlobals.ptCachedSoundStreamMap = new FModCachedSoundStreamMap();
    sFModSoundGlobals.ptLRUSoundSampleList = new FModCachedSoundList();
    sFModSoundGlobals.ptLRUSoundStreamList = new FModCachedSoundList();
    sFModSoundGlobals.ptCachedSoundAllocator = new FModCachedSoundAllocator;


    sFModSoundGlobals.nCacheSampleSize = sFModSoundGlobals.nCacheStreamSize = 0;











    // Inference: the original loop count is the immediate 128 (no reload of nNumChannels after the allocations above).
    for (int32 i = 0; i < 128; i++)
    {
        PlayingSoundData * ptPlayingSoundData = &sFModSoundGlobals.arPlayingSounds[i];
        ptPlayingSoundData->Clear();


        for (int32 j = 0; j < DSP_NUM_TYPES; j++)
        {
            FModDSPEffect * ptDspEffect = &ptPlayingSoundData->arDspEffects[j];

            ptDspEffect->eEffectType = sgarFModDspTypeConvert[j];









            // Inference: dwFlags is cleared after ptFModDsp (store order in the unrolled loop); it has no line row of its own, so it shares 2210.
            ptDspEffect->ptFModDsp = NULL; ptDspEffect->dwFlags = 0;
        }
    }






    for (int32 nDsp = 0; nDsp < DSP_NUM_TYPES; nDsp++)
    {

        int32 nCount = sgarFModDspPools[nDsp].nPoolCount;
        sgarFModDspPools[nDsp].ptFModDspPool = (FModPooledDSP *)SigmaMemoryNew(nCount * sizeof(FModPooledDSP), 4, NULL, FALSE);
        for (int32 nItem = 0; nItem < nCount; nItem++)
        {
            FModPooledDSP & poolItem = sgarFModDspPools[nDsp].ptFModDspPool[nItem];
            poolItem.fDspInUse = FALSE;
            result = FMOD_System_CreateDSPByType(sFModSoundGlobals.ptFModSystem, sgarFModDspTypeConvert[nDsp], &poolItem.ptFModDsp);
            if (!sCheckResult(result, "FMOD_System_CreateDSPByType", NULL))
            {
                sCleanupFMod();
                return FALSE;
            }
        }
    }






    FMOD_DSP_DESCRIPTION * ptFmodDSPDescription = &sFModDspRetroFilterPool.tFmodDSPDescription;

    if (CheapAmpModelDSP::gGetCheapAmpModelDesc(ptFmodDSPDescription))
    {
        result = FMOD_System_CreateDSP(sFModSoundGlobals.ptFModSystem, ptFmodDSPDescription, &sFModDspRetroFilterPool.ptFModDsp);
        sCheckResult(result, "FMOD_System_CreateDSP", NULL);
    }
    sFModDspRetroFilterPool.fDspConnected = FALSE;









    result = FMOD_System_CreateChannelGroup(sFModSoundGlobals.ptFModSystem, "Music", &sFModSoundGlobals.ptMusicChannelGroup);
    sCheckResult(result, "FMOD_System_CreateChannelGroup", NULL);
    sFModSoundGlobals.flLastMusicVolume = -1.0f;


    result = FMOD_System_GetMasterChannelGroup(sFModSoundGlobals.ptFModSystem, &sFModSoundGlobals.ptMasterChannelGroup);
    sCheckResult(result, "FMOD_System_GetMasterChannelGroup", NULL);
    sFModSoundGlobals.flLastMasterVolume = -1.0f;

    SigmaMemoryDumpOSStats();

    return TRUE;





}





void sReleaseFModSound(
    FMOD_SOUND * ptFModSound,
    BOOL fForceRelease)
{





    if (!fForceRelease)
    {
        FMOD_OPENSTATE eOpenState;
        if (FMOD_Sound_GetOpenState(ptFModSound, &eOpenState, NULL, NULL, NULL) == FMOD_OK)
        {
            if (eOpenState != FMOD_OPENSTATE_READY && eOpenState != FMOD_OPENSTATE_ERROR)
            {
                sptAsyncAudio->ReleaseFModSound(ptFModSound);
                return;
            }
        }
    }





    FMOD_RESULT result = FMOD_Sound_Release(ptFModSound);

    sCheckResult(result, "FMOD_Sound_Release", "");
}


void SoundPlatform::Dispose()

{


    FMOD_RESULT result;



    StopOVTStream();

    for (int32 i = 0; i < sFModSoundGlobals.nNumChannels; i++)
    {
        FMOD_CHANNEL * ptFModChannel;
        result = FMOD_System_GetChannel(sFModSoundGlobals.ptFModSystem, i, &ptFModChannel);
        // the result test has no line row of its own (cbnz at line 0)
        if (result == FMOD_OK)
        {
            FMOD_Channel_Stop(ptFModChannel);
        }
    }



    POS pos = sFModSoundGlobals.ptCachedSoundSampleMap->GetStartAssoc();

    while (pos)
    {
        FModCachedSoundKey tKey;
        FModCachedSound * ptCachedSound;
        sFModSoundGlobals.ptCachedSoundSampleMap->GetNextAssoc(pos, tKey, ptCachedSound);

        // inlined body has no FMOD_Sound_GetOpenState call: fForceRelease is TRUE

        sReleaseFModSound(ptCachedSound->ptFModParentSound ? ptCachedSound->ptFModParentSound : ptCachedSound->ptFModSound, TRUE);
		ptCachedSound->ptFModSound = ptCachedSound->ptFModParentSound = NULL;
    }


    pos = sFModSoundGlobals.ptCachedSoundStreamMap->GetStartAssoc();

    while (pos)
    {
        FModCachedSoundKey tKey;
        FModCachedSound * ptCachedSound;
        sFModSoundGlobals.ptCachedSoundStreamMap->GetNextAssoc(pos, tKey, ptCachedSound);

        while (ptCachedSound)
        {
            FModCachedSound * ptCachedSoundFree = ptCachedSound;
            ptCachedSound = ptCachedSound->ptNext;

			sReleaseFModSound(ptCachedSoundFree->ptFModParentSound ? ptCachedSoundFree->ptFModParentSound : ptCachedSoundFree->ptFModSound, TRUE);
			ptCachedSoundFree->ptFModSound = ptCachedSoundFree->ptFModParentSound = NULL;
        }
    }




    for (int32 i = 0; i < sFModSoundGlobals.nNumChannels; i++)
    {
        PlayingSoundData * ptPlayingSoundData = &sFModSoundGlobals.arPlayingSounds[i];

        // j and ptDspEffect have no location: the loop is fully unrolled
        for (int32 j = 0; j < DSP_NUM_TYPES; j++)
        {
            FModDSPEffect * ptDspEffect = &ptPlayingSoundData->arDspEffects[j];










            ptDspEffect->ptFModDsp = NULL;
        }
    }




    for (int32 nDsp = 0; nDsp < DSP_NUM_TYPES; nDsp++)
    {
        int32 nCount = sgarFModDspPools[nDsp].nPoolCount;
        for (int32 nItem = 0; nItem < nCount; nItem++)
        {
            FModPooledDSP & poolItem = sgarFModDspPools[nDsp].ptFModDspPool[nItem];






            result = FMOD_DSP_Release(poolItem.ptFModDsp);

            sCheckResult(result, "FMOD_DSP_Release", NULL);
        }

        XFREE(sgarFModDspPools[nDsp].ptFModDspPool, NULL);
    }




    if (sFModDspRetroFilterPool.ptFModDsp)
    {









        result = FMOD_DSP_Release(sFModDspRetroFilterPool.ptFModDsp);
        sCheckResult(result, "FMOD_DSP_Release", NULL);

        sFModDspRetroFilterPool.ptFModDsp = NULL;
    }








    // every instruction of lines 2447 and 2459-2463 is at column 5 (a delete-and-NULL macro; its name is not in the DWARF)
    delete sptAsyncAudio; sptAsyncAudio = NULL;



    result = FMOD_System_Release(sFModSoundGlobals.ptFModSystem);
    sCheckResult(result, "FMOD_System_Release", NULL);
    sFModSoundGlobals.ptFModSystem = NULL;

    XFREE(sFModSoundGlobals.arPlayingSounds, NULL);



    delete sFModSoundGlobals.ptCachedSoundSampleMap; sFModSoundGlobals.ptCachedSoundSampleMap = NULL;
    delete sFModSoundGlobals.ptCachedSoundStreamMap; sFModSoundGlobals.ptCachedSoundStreamMap = NULL;
    delete sFModSoundGlobals.ptLRUSoundSampleList; sFModSoundGlobals.ptLRUSoundSampleList = NULL;
    delete sFModSoundGlobals.ptLRUSoundStreamList; sFModSoundGlobals.ptLRUSoundStreamList = NULL;
    delete sFModSoundGlobals.ptCachedSoundAllocator; sFModSoundGlobals.ptCachedSoundAllocator = NULL;

















}












































































































































inline BOOL sSoundHasBecomeInvalid(const SoundInstance * ptSound, int32 nWhichChannel, FMOD_CHANNEL *& ptFModChannel)
{




    if (sptAsyncAudio->IsCancelled(ptSound))
        return TRUE;




    int32 nChannelIndex = ptSound->tChannelState[nWhichChannel].nChannelIndex;



    FMOD_RESULT result = FMOD_System_GetChannel(sFModSoundGlobals.ptFModSystem, nChannelIndex, &ptFModChannel);
    if (result != FMOD_OK)
        return TRUE;















    void * pUserData;
    result = FMOD_Channel_GetUserData(ptFModChannel, &pUserData);
    SoundID idSound = (SoundID)(intptr_t)pUserData; sCheckResult(result, "FMOD_Channel_GetUserData", "");

    return ptSound->id != idSound;
}






static void sUpdate3DPanLevel(SoundInstance * ptSound, float flAppliedChannelVolume, FMOD_CHANNEL * ptFModChannel)
{



    ScopedGlobalDefinitionLockR pdefGlobal;
    if (pdefGlobal->flAudioMin3DPanLevel < 1.0f && gb3DPanEnabled)
    {
        Vector3D wpListener;
        SoundManager::ListenerGetPanningPosition(wpListener);
        float flDist = Vector3D::Distance(wpListener, ptSound->wp);
        float fl3DPanLevel = pdefGlobal->flAudioMin3DPanLevel + (1.0f - pdefGlobal->flAudioMin3DPanLevel) * Math::Pin((flDist - pdefGlobal->flAudioMin3DPanLevelRadius) / (pdefGlobal->flAudioMax3DPanLevelRadius - pdefGlobal->flAudioMin3DPanLevelRadius), 0.0f, 1.0f);

        FMOD_RESULT result = FMOD_Channel_Set3DLevel(ptFModChannel, fl3DPanLevel);
        if (result == FMOD_OK)
        {
            float flChannelVolume;
            float flOldVolume;
            float fl3DVolume = sFMod3DRolloffCallback((FMOD_CHANNELCONTROL *)ptFModChannel, flDist);
            float flExpectedFinalVolume = fl3DVolume * flAppliedChannelVolume;
            float flPanFactor = fl3DPanLevel * fl3DVolume; float flNewChannelVolume = flExpectedFinalVolume / ((1.0f - fl3DPanLevel) + flPanFactor);
            flChannelVolume = (1.0f - fl3DPanLevel) * gf3DPanEndVolume * flNewChannelVolume + fl3DPanLevel * flNewChannelVolume;
            if (FMOD_Channel_GetVolume(ptFModChannel, &flOldVolume) == FMOD_OK && flOldVolume != flChannelVolume &&
                (flChannelVolume == 0.0f || Math::Abs(flOldVolume - flChannelVolume) > (1.0f / 256.0f)))
            {
                FMOD_Channel_SetVolume(ptFModChannel, flChannelVolume);
            }
        }
    }
}







































inline void sTouchSoundBank(PlayingSoundData * ptPlayingSound)
{


    if (ptPlayingSound->ptCachedSound)
    {
        FModCachedSoundKey & tKey = sFModSoundGlobals.ptLRUSoundSampleList->GetAt(ptPlayingSound->ptCachedSound->tLRUPos);
        ScopedSoundBankDefinitionLockR pdefSoundBank(tKey.snoSoundBank);
    }
}

















static BOOL sUpdateChannel(SoundInstance * ptSound, int32 nWhichChannel)
{
    int32 nChannelIndex = ptSound->tChannelState[nWhichChannel].nChannelIndex;
    if (nChannelIndex < 0 || nChannelIndex >= sFModSoundGlobals.nNumChannels)
        return FALSE;





    PlayingSoundData * ptPlayingSoundData = &sFModSoundGlobals.arPlayingSounds[nChannelIndex];

    sTouchSoundBank(ptPlayingSoundData);

    FMOD_CHANNEL * ptFModChannel;
    if (sSoundHasBecomeInvalid(ptSound, nWhichChannel, ptFModChannel))
        return FALSE;




    FMOD_BOOL fIsPlaying = TRUE;
    FMOD_RESULT result = FMOD_Channel_IsPlaying(ptFModChannel, &fIsPlaying);
    if (result != FMOD_OK)
    {
        fIsPlaying = FALSE;
    }
    if (!fIsPlaying)
    {


        if (!ptSound->fStitching)
            return FALSE;


        sReleaseChannel(ptPlayingSoundData);
        ptSound->tChannelState[nWhichChannel].Initialize();


        if (ptSound->nCurrentChannel == nWhichChannel)
        {
            ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
            int32 nPermutationIndexToUse = SelectSoundPermutation(pdefSound, const_cast<SoundPlayHistory *>(&pdefSound->tSoundPlayHistory));

            const SoundPermutation & tPermutation = pdefSound->ptPermutations[nPermutationIndexToUse];
            ScopedSoundBankDefinitionLockR pdefSoundBank(tPermutation.snoSoundBank);
            SoundSample & tSoundSample = pdefSoundBank->ptSoundSamples[tPermutation.nSampleIndex];

            ptSound->flWantedStartTime = CGameGetClientTime();

            if (!sLoadAndPlaySound(ptSound, tPermutation.snoSoundBank, ptSound->nCurrentChannel, tSoundSample))
                return FALSE;


            ptSound->nCurrentChannel = 1 - ptSound->nCurrentChannel;
        }
        return TRUE;
    }

    if (ptSound->tParams.f3D)
    {
        sUpdate3DPanLevel(ptSound, ptPlayingSoundData->flAppliedVolume, ptFModChannel);
    }
    return TRUE;
}

inline void sUpdateChannelGroupVolumes()
{






    float flMasterVolume =
        SoundManager::GetGlobalMute() ? 0.0f : 1.0f;



    if (flMasterVolume != sFModSoundGlobals.flLastMasterVolume)
    {
        FMOD_RESULT result = FMOD_ChannelGroup_SetVolume(sFModSoundGlobals.ptMasterChannelGroup, flMasterVolume);
        sCheckResult(result, "FMOD_ChannelGroup_SetVolume", "");
        sFModSoundGlobals.flLastMasterVolume = flMasterVolume;
    }

    float flMusicVolume = sFModSoundGlobals.bInGameMusicEnabled && !SoundManager::GetGlobalMute() ? 1.0f : 0.0f;
    if (flMusicVolume != sFModSoundGlobals.flLastMusicVolume)
    {
        FMOD_RESULT result = FMOD_ChannelGroup_SetVolume(sFModSoundGlobals.ptMusicChannelGroup, flMusicVolume);
        sCheckResult(result, "FMOD_ChannelGroup_SetVolume", "");
        sFModSoundGlobals.flLastMusicVolume = flMusicVolume;
    }
}

// X/Client/Sound/SoundPlatformFModExAsync.h:104.
inline BOOL AsyncAudio::AnyPending()
{
    return m_queuedCalls.Count() > 0 || m_queuedMusicCalls.Count() > 0;
}

void SoundPlatform::Update()
{
    sUpdateChannelGroupVolumes();








    FMOD_RESULT result = FMOD_System_Update(sFModSoundGlobals.ptFModSystem);
    sCheckResult(result, "FMOD_System_Update", "");



    uint32 dwPlayerCount = 0;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (!PlayerIsLocal(ptPlayer))
            continue;




        ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
        WorldPlace tPlace = tACD.GetPlace();
        sFModSoundGlobals.vPlayerPositions[dwPlayerCount++] = tPlace.wp;
    }

    sFModSoundGlobals.dwPlayerCount = dwPlayerCount;



    sptAsyncAudio->ProcessPendingSounds();

    sGarbageCollectFModMemory(FALSE);





    if (!sptAsyncAudio->AnyPending() && sFModSoundGlobals.pendingPreloadList.Count() > 0)
    {
        Blizzard::Lock::ScopedMutex lock(sFModSoundGlobals.tPreloadListLock);




        FModSampleToPreload & tPreload = sFModSoundGlobals.pendingPreloadList.GetHead();
        FModCachedSound * ptCachedSound = sGetSound(tPreload.info.snoSoundBank, tPreload.info.nSampleIndex, FMOD_CACHED_SOUND_SAMPLE, FALSE, TRUE);
        if (ptCachedSound)
        {
            sptAsyncAudio->PreloadSound(ptCachedSound);
        }
        int32 fValue;
        if (sFModSoundGlobals.pendingPreloadMap.Lookup(tPreload.info, fValue))
            sFModSoundGlobals.pendingPreloadMap.RemoveAt(tPreload.info);
        sFModSoundGlobals.pendingPreloadList.RemoveHead();
    }



    TDataArrayIterator<SoundInstance> tIt;
    while (SoundInstance * ptSound = gptSounds->Iterate(tIt))
    {



        if (ptSound->flDelay != 0.0f)
            continue;





        if (sptAsyncAudio->IsPending(ptSound))
            continue;



        if (!sUpdateChannel(ptSound, 0))
        {
            sFreeSound(ptSound);
            continue;
        }

        if (ptSound->fStitching)
        {
            if (!sUpdateChannel(ptSound, 1))
            {
                sFreeSound(ptSound);
                break;
            }
        }
    }
}

void SoundPlatform::SetListener(const Vector3D & wpVolume, const Vector3D & wpPanning, const Vector3D & wvVel, const Vector3D & wvView, const Vector3D & wvUp)
{






    Vector3D wvNegUp = -wvUp;
    sFModSoundGlobals.wpVolumeListener = wpVolume;
    sFModSoundGlobals.fCustomVolumeListener = wpVolume != wpPanning;
    FMOD_System_Set3DListenerAttributes(sFModSoundGlobals.ptFModSystem, 0, (const FMOD_VECTOR *)&wpPanning, (const FMOD_VECTOR *)&wvVel, (const FMOD_VECTOR *)&wvView, (const FMOD_VECTOR *)&wvNegUp);
}






void SoundPlatform::SetListenerParams(const float flRolloff, const float flDopplerFactor)
{
    FMOD_System_Set3DSettings(sFModSoundGlobals.ptFModSystem, flDopplerFactor, 3.28f, flRolloff);
}







void SoundPlatform::OnBankLoaded(const SoundBankDefinition *)
{
}



void SoundPlatform::OnBankUnloaded(const SoundBankDefinition * ptSoundBankDef)
{
    OnBankPurge(ptSoundBankDef->tHeader.snoHandle);
}



void SoundPlatform::OnBankPurge(SNO snoSoundBank)
{
    POS pos;

    if (!sFModSoundGlobals.ptFModSystem)
        return;

    for (int32 i = 0; i < sFModSoundGlobals.nNumChannels; i++)
    {
        PlayingSoundData * ptPlayingSound = &sFModSoundGlobals.arPlayingSounds[i];
        if (ptPlayingSound->idSound != SOUNDID_INVALID)
        {



            SoundInstance * ptSoundInst = SoundTryToGet(ptPlayingSound->idSound);
            if (ptSoundInst)
            {
                if (ptSoundInst->tChannelState[0].snoSoundBank == snoSoundBank ||
                    ptSoundInst->tChannelState[1].snoSoundBank == snoSoundBank)
                {
                    SoundInstance * ptSound = ptSoundInst;
                    Stop(ptSound);
                }
            }
        }
    }


    pos = sFModSoundGlobals.ptLRUSoundStreamList->GetHeadPosition();
    while (pos)
    {

        FModCachedSoundKey tKey = sFModSoundGlobals.ptLRUSoundStreamList->GetNext(pos);
        if (tKey.snoSoundBank == snoSoundBank)
        {
            BOOL bPurged = sPurgeSoundStream(tKey, TRUE);
            if (!bPurged)
            {
                CRefString sSoundBankName = SNOToString(SOUNDBANK_SNO, tKey.snoSoundBank, FALSE);
            }
        }
    }


    pos = sFModSoundGlobals.ptLRUSoundSampleList->GetHeadPosition();
    while (pos)
    {

        FModCachedSoundKey tKey = sFModSoundGlobals.ptLRUSoundSampleList->GetNext(pos);
        if (tKey.snoSoundBank == snoSoundBank)
        {
            BOOL bPurged = sPurgeSoundSample(tKey, FALSE, TRUE);
            if (!bPurged)
            {
                CRefString sSoundBankName = SNOToString(SOUNDBANK_SNO, tKey.snoSoundBank, FALSE);
            }
        }
    }
}











BOOL SoundPlatform::SetRetroFilter(
    const BOOL fEnable)
{
    FMOD_RESULT result = FMOD_OK;



    if (!sFModSoundGlobals.ptFModSystem ||
        !sFModSoundGlobals.ptMasterChannelGroup ||
        !sFModDspRetroFilterPool.ptFModDsp)
    {
        return FALSE;
    }

    if (sFModDspRetroFilterPool.fDspConnected)
    {
        result = FMOD_DSP_SetBypass(sFModDspRetroFilterPool.ptFModDsp, !fEnable);
        sCheckResult(result, "FMOD_DSP_SetBypass", "");
    }
    else if (fEnable)
    {

        result = FMOD_ChannelGroup_AddDSP(sFModSoundGlobals.ptMasterChannelGroup, FMOD_CHANNELCONTROL_DSP_HEAD, sFModDspRetroFilterPool.ptFModDsp);



        sCheckResult(result, "FMOD_ChannelGroup_AddDSP", "");
        sFModDspRetroFilterPool.fDspConnected = TRUE;
    }


    return result == FMOD_OK;
}


































static FMOD_RESULT sSetDspParameter(
    FModDSPEffect * ptFModDspEffect,
    int32 nIndex,
    DspParam & tParam, // Inference: callers without a multiply imply the default 1.0f below.
    float flMultiplier = 1.0f)
{
    float flValue = GetDspParamValue(tParam) * flMultiplier;


    FMOD_RESULT result = FMOD_DSP_SetParameterFloat(ptFModDspEffect->ptFModDsp, nIndex, flValue);
    return result;
}






static FMOD_RESULT sSetDspParameterLerp(
    FModDSPEffect * ptFModDspEffect,
    int32 nIndex,
    DspParam & tParam,
    float flLow,
    float flHigh,
    float flMultiplier,
    float flAdd)
{
    float flValue = (GetDspParamValue(tParam) + flAdd)*flMultiplier;




    flValue = Math::Pin(flValue, 0.0f, 1.0f);

    flValue = Math::Lerp(flLow, flHigh, flValue);


    FMOD_RESULT result = FMOD_DSP_SetParameterFloat(ptFModDspEffect->ptFModDsp, nIndex, flValue);
    return result;
}







static void sSetDspParameters(
    FModDSPEffect * ptFModDspEffect,
    DspEffect * ptDsp)
{
    switch(ptDsp->eDspType)
    {
    case DSP_TYPE_CHORUS:

        sSetDspParameter(ptFModDspEffect, FMOD_DSP_CHORUS_MIX, ptDsp->tChorusDryMix, 100.0f);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_CHORUS_RATE, ptDsp->tChorusRate);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_CHORUS_DEPTH, ptDsp->tChorusDepth, 100.0f);
        break;












    case DSP_TYPE_COMPRESSOR:

        sSetDspParameter(ptFModDspEffect, FMOD_DSP_COMPRESSOR_THRESHOLD, ptDsp->tCompressorThreshold);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_COMPRESSOR_ATTACK, ptDsp->tCompressorAttack);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_COMPRESSOR_RELEASE, ptDsp->tCompressorRelease);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_COMPRESSOR_GAINMAKEUP, ptDsp->tCompressorGainMakeup);
        break;
    case DSP_TYPE_DISTORTION:
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_DISTORTION_LEVEL, ptDsp->tDistortionLevel);
        break;
    case DSP_TYPE_ECHO:

        sSetDspParameter(ptFModDspEffect, FMOD_DSP_ECHO_DELAY, ptDsp->tEchoDelay);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_ECHO_FEEDBACK, ptDsp->tEchoDecayRatio, 100.0f);
        sSetDspParameterLerp(ptFModDspEffect, FMOD_DSP_ECHO_DRYLEVEL, ptDsp->tEchoDryMix, -80.0f, 10.0f, 1.0f, 0.0f);
        sSetDspParameterLerp(ptFModDspEffect, FMOD_DSP_ECHO_WETLEVEL, ptDsp->tEchoWetMix, -80.0f, 10.0f, 1.0f, 0.0f);
        break;







    case DSP_TYPE_FLANGE:

        sSetDspParameter(ptFModDspEffect, FMOD_DSP_FLANGE_MIX, ptDsp->tFlangeWetMix, 100.0f);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_FLANGE_DEPTH, ptDsp->tFlangeDepth);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_FLANGE_RATE, ptDsp->tFlangeRate);
        break;
    case DSP_TYPE_HIGHPASS:
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_HIGHPASS_CUTOFF, ptDsp->tHighpassCutoff);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_HIGHPASS_RESONANCE, ptDsp->tHighpassResonance);
        break;






    case DSP_TYPE_LOWPASS:
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_LOWPASS_CUTOFF, ptDsp->tLowpassCutoff);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_LOWPASS_RESONANCE, ptDsp->tLowpassResonance);
        break;
    case DSP_TYPE_LOWPASS_SIMPLE:
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, ptDsp->tLowpassSimpleCutoff);
        break;
    case DSP_TYPE_PARAMEQ:
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_PARAMEQ_CENTER, ptDsp->tParamEqCenter);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_PARAMEQ_BANDWIDTH, ptDsp->tParamEqBandwidth);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_PARAMEQ_GAIN, ptDsp->tParamEqGain);
        break;
    case DSP_TYPE_SFXREVERB:

        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_DECAYTIME, ptDsp->tReverbDecayTime, 1000.0f);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_EARLYDELAY, ptDsp->tReverbReflectionsDelay, 1000.0f);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_LATEDELAY, ptDsp->tReverbReverbDelay, 1000.0f);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_HFREFERENCE, ptDsp->tReverbHFReference);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_HFDECAYRATIO, ptDsp->tReverbDecayHFRatio, 100.0f);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_DIFFUSION, ptDsp->tReverbDiffusion);
        sSetDspParameter(ptFModDspEffect, FMOD_DSP_SFXREVERB_DENSITY, ptDsp->tReverbDensity);




        sSetDspParameterLerp(ptFModDspEffect, FMOD_DSP_SFXREVERB_WETLEVEL, ptDsp->tReverbReverbLevel, -80.0f, 20.0f, 0.0001f, 10000.0f);
        sSetDspParameterLerp(ptFModDspEffect, FMOD_DSP_SFXREVERB_DRYLEVEL, ptDsp->tReverbDryLevel, -80.0f, 20.0f, 0.0001f, 10000.0f);
        break;
    default:
        break;
    }
}






















static void sApplySoundDspEffectsToChannel(
    FMOD_CHANNEL * ptFModChannel,
    FModDSPEffect * arDspEffects,
    SNO snoSound)
{
    FMOD_RESULT result;

    for (int32 i=0; i<DSP_NUM_TYPES; i++)
    {
        FModDSPEffect * ptDspEffect = &arDspEffects[i];

        if ( ptDspEffect->dwFlags & (1 << FMOD_DSP_FLAG_ENABLED) )
        {

            if (ptDspEffect->ptFModDsp)
            {
                sReleaseFModDSP((DspType)i, ptDspEffect->ptFModDsp);
                ptDspEffect->ptFModDsp = NULL;
            }




            ptDspEffect->dwFlags &= ~(1 << FMOD_DSP_FLAG_ENABLED);
        }
    }

    if ( snoSound != SNO_NONE )
    {
        ScopedSoundDefinitionLockR ptSoundDef(snoSound);
        DspEffect * ptReverbDsp = NULL;
        DspEffect * ptEchoDsp = NULL;

        int32 nDspCount = ptSoundDef->serDspEffects.dwSize / sizeof(DspEffect);
        for (int32 i = 0; i < nDspCount; i++)
        {
            DspEffect * ptDsp = &ptSoundDef->arDspEffects[i];


            FModDSPEffect * ptFModDspEffect = &arDspEffects[ptDsp->eDspType];
            // -1: no DspType enumerator for it in the DWARF.
            if ( !(ptDsp->dwFlags & (1 << DSP_FLAG_DISABLED)) && ptDsp->eDspType != -1 )
            { // Inference: FMOD_ERR_INTERNAL suppresses success handling for NULL/sentinel DSPs.
                result = FMOD_ERR_INTERNAL;
                ptFModDspEffect->ptFModDsp = sAcquireFModDSP(ptDsp->eDspType, snoSound);
                if (ptFModDspEffect->ptFModDsp)
                {
                    if (ptFModDspEffect->ptFModDsp != (FMOD_DSP *)-1)
                    {

                        result = FMOD_Channel_AddDSP(ptFModChannel, FMOD_CHANNELCONTROL_DSP_HEAD, ptFModDspEffect->ptFModDsp);
                    }
                    else
                    {



                        ptFModDspEffect->dwFlags |= (1 << FMOD_DSP_FLAG_ENABLED);
                    }
                }













                if ( result == FMOD_OK )
                {
                    ptFModDspEffect->dwFlags |= (1 << FMOD_DSP_FLAG_ENABLED);

                    sSetDspParameters(ptFModDspEffect, ptDsp);

                    if (ptDsp->eDspType == DSP_TYPE_SFXREVERB)
                        ptReverbDsp = ptDsp;
                    else if (ptDsp->eDspType == DSP_TYPE_ECHO)
                        ptEchoDsp = ptDsp;
                }
            }
        }






        if (ptReverbDsp)
        {
            float flExtendedTimeInS = GetDspParamValue(ptReverbDsp->tReverbDecayTime);
            uint32 dwExtendedTimeInMsec;


            unsigned long long nParentDSPClock = 0;

            result = FMOD_Channel_GetDSPClock(ptFModChannel, NULL, &nParentDSPClock);
            sCheckResult(result, "FMOD_Channel_GetDSPClock", "");

            int32 nOutputSampleRate;
            result = FMOD_System_GetSoftwareFormat(sFModSoundGlobals.ptFModSystem, &nOutputSampleRate, NULL, NULL);
            sCheckResult(result, "FMOD_System_GetSoftwareFormat", "");

            result = FMOD_Channel_SetDelay(ptFModChannel, 0, nParentDSPClock + flExtendedTimeInS*nOutputSampleRate, TRUE);




            sCheckResult(result, "FMOD_Channel_SetDelay", "");
        }
        else if (ptEchoDsp)
        {
            const float TARGET_INTENSITY = 0.01f;
            float flDecayRatio = GetDspParamValue(ptEchoDsp->tEchoDecayRatio);
            if (flDecayRatio > EPSILON && flDecayRatio < 1.0f - EPSILON)
            {

                float flEchoDelayMsec = GetDspParamValue(ptEchoDsp->tEchoDelay);


                float flRepeats = logf(TARGET_INTENSITY) / logf(flDecayRatio);


                unsigned long long parentTickFull = 0;

                result = FMOD_Channel_GetDSPClock(ptFModChannel, NULL, &parentTickFull);
                sCheckResult(result, "FMOD_Channel_GetDSPClock", "");

                int32 nOutputSampleRate;
                result = FMOD_System_GetSoftwareFormat(sFModSoundGlobals.ptFModSystem, &nOutputSampleRate, NULL, NULL);
                sCheckResult(result, "FMOD_System_GetSoftwareFormat", "");

                float flExtendedTimeInSec = flEchoDelayMsec * flRepeats / 1000.0f;
                result = FMOD_Channel_SetDelay(ptFModChannel, 0, parentTickFull + flExtendedTimeInSec*nOutputSampleRate, TRUE);




                sCheckResult(result, "FMOD_Channel_SetDelay", "");
            }
        }
    }
}



static void sClearChannelReverb(FMOD_CHANNEL * ptFModChannel)
{
    for (int32 i = 0; i < FMOD_REVERB_MAXINSTANCES; i++)
    {
        FMOD_RESULT result = FMOD_Channel_SetReverbProperties(ptFModChannel, i, 0.0f);
        sCheckResult(result, "FMOD_Channel_SetReverbProperties", "");
    }
}



















static void sSetChannel3DParams(
    FMOD_CHANNEL * ptFModChannel,
    SoundInstance * ptSound,
    PlayingSoundData * ptPlayingSoundData)
{



    FMOD_RESULT result;
    result = FMOD_Channel_SetMode(ptFModChannel, FMOD_3D);

    result = FMOD_Channel_Set3DAttributes(ptFModChannel, (const FMOD_VECTOR*)&ptSound->wp, (const FMOD_VECTOR*)&ptSound->wvVel, NULL);







    result = FMOD_Channel_Set3DSpread(ptFModChannel, gf3DSoundSpread);
    sCheckResult(result, "FMOD_Channel_Set3DSpread", "");

    float flDistanceMinToUse = Math::Max(ptSound->tRolloffParams.flDistanceMin, FLT_MIN);
    float flDistanceMaxToUse = Math::Max(ptSound->tRolloffParams.flDistanceMax, FLT_MIN);

    result = FMOD_Channel_GetVolume(ptFModChannel, &ptPlayingSoundData->flAppliedVolume);

    if (result == FMOD_OK)
    {
        sUpdate3DPanLevel(ptSound, ptPlayingSoundData->flAppliedVolume, ptFModChannel);
    }

    result = FMOD_Channel_Set3DMinMaxDistance(ptFModChannel, flDistanceMinToUse, flDistanceMaxToUse);
    result = FMOD_Channel_Set3DDopplerLevel(ptFModChannel, ptSound->flDopplerFactor);
}


static void sAdjustSpeakerMix(
    FMOD_CHANNEL * ptFModChannel,
    const SoundPlaybackParams & tParams,
    const BOOL f3D)
{
    FMOD_RESULT result;
    float left = 1.0f, right = 1.0f, center = 1.0f, lfe = 1.0f, rear_left = 1.0f, rear_right = 1.0f, side_left = 1.0f, side_right = 1.0f;


    if (tParams.eType == SOUNDTYPE_AMBIENT &&
        !f3D)
    {
        rear_left = rear_right = side_left = side_right = 0.8f;
    }


    else if (tParams.eType == SOUNDTYPE_NARRATION)
    {
        lfe = 0.0f;
    }



    if (gbForce5_1Enabled)
    {
        rear_left = rear_right = side_left = side_right = 0.5f;
    }



    result = FMOD_Channel_SetMixLevelsOutput(ptFModChannel, left, right, center, lfe, side_left, side_right, rear_left, rear_right);
    sCheckResult(result, "FMOD_Channel_SetMixLevelsOutput", "");
}







static void sSetChannelProperties(
    FMOD_SOUND * ptFModSound,
    FMOD_CHANNEL * ptFModChannel,
    SoundInstance * ptSound,
    SoundSample & tSoundSample,
    SoundPlaybackParams & tParams,
    FModDSPEffect * arDspEffects,
    uint32 dwFreq,
    PlayingSoundData * ptPlayingSoundData)
{
    FMOD_RESULT result;


    result = FMOD_Channel_SetChannelGroup(ptFModChannel, sFModSoundGlobals.ptMasterChannelGroup);
    sCheckResult(result, "FMOD_Channel_SetChannelGroup", "");


    BOOL fReverb = !tParams.fDisableReverb && gbReverbEnabled;
    if ( fReverb )
    {

        FMOD_REVERB_PROPERTIES tfmodReverb;
        SigmaMemoryClear(&tfmodReverb, sizeof(tfmodReverb));

        result = FMOD_System_GetReverbProperties(sFModSoundGlobals.ptFModSystem, 0, &tfmodReverb);
        sCheckResult(result, "FMOD_System_GetReverbProperties", "");

        float flWetLevel0to1 = (tfmodReverb.WetLevel+80.0f)/100.0f;

        result = FMOD_Channel_SetReverbProperties(ptFModChannel, 0, flWetLevel0to1);
        sCheckResult(result, "FMOD_Channel_SetReverbProperties", "");
    }


















    else
    {
        sClearChannelReverb(ptFModChannel);
    }

    if (tParams.f3D)
    {
        sSetChannel3DParams(ptFModChannel, ptSound, ptPlayingSoundData);
    }
    else
    {
        result = FMOD_Channel_SetMode(ptFModChannel, FMOD_2D);
    }


    sAdjustSpeakerMix(ptFModChannel, tParams, tParams.f3D);

    sApplySoundDspEffectsToChannel(ptFModChannel, arDspEffects, ptSound->tSoundParams.snoSound);


    float flPitch = SoundGetAdjustedPitch(ptSound);
    if ( Math::Abs(1.0f - flPitch) > EPSILON )
    {



        uint32 dwFrequency = Math::FloatRoundToInt(flPitch * dwFreq);
        result = FMOD_Channel_SetFrequency(ptFModChannel, (float)dwFrequency);
    }

    if ( tParams.fLoops && !ptSound->fStitching )
    {
        result = FMOD_Channel_SetMode(ptFModChannel, FMOD_LOOP_NORMAL);
        result = FMOD_Channel_SetLoopCount(ptFModChannel, -1);

        uint32 dwStartSample = SelectLoopingSoundStartSample((ptSound->dwFlags & (1 << SOUNDINSTANCE_RANDOM_START_POSITION_BIT)) != 0, tSoundSample);

        if ( dwStartSample )
        {

            float flSamplesToMS = 1000.0f / dwFreq;
            uint32 nStartMS = flSamplesToMS * dwStartSample;

            result = sFMODChannelSetPosition(ptFModSound, ptFModChannel, nStartMS, FMOD_TIMEUNIT_MS);
            sCheckResult(result, "FMOD_Channel_SetPosition", "");
        }
    }

    else if ( tParams.flStartOffset > 0.0f )
    {
        uint32 nStartMS = Math::FloatToInt(tParams.flStartOffset * 1000.0f);

        result = sFMODChannelSetPosition(ptFModSound, ptFModChannel, nStartMS, FMOD_TIMEUNIT_MS);

        sCheckResult(result, "FMOD_Channel_SetPosition", "");
    }

    result = FMOD_Channel_SetUserData(ptFModChannel, (void *)(intptr_t)ptSound->id);
}



static BOOL sPlaySound(
    FModCachedSound * ptCachedSound,
    SoundInstance * ptSound,
    const SNO snoSoundBank,
    int32 nWhichChannel,
    SoundSample & tSoundSample)
{
    FMOD_RESULT result;
    FMOD_SOUND *ptFModSound = ptCachedSound->ptFModSound;
    SoundPlaybackParams & tParams = ptSound->tParams;

    uint32 dwChannels, dwFreq, dwBitRate;
    SoundFormatToCSB(tSoundSample.GetFormat(), dwChannels, dwFreq, dwBitRate);











    {







        uint32 dwPriority = 128 - Math::Pin(ptSound->dwPriority, 0u, 128u);


        result = FMOD_Sound_SetDefaults(ptFModSound, (float)dwFreq, dwPriority);



        if (!sCheckResult(result, "FMOD_Sound_SetDefaults", ""))
        {
            sReleaseSound(ptCachedSound, FALSE);
            return FALSE;
        }
    }
    {

        FMOD_MODE dwMode = FMOD_LOOP_OFF;
        int32 nLoopCount = 0;

        if ( tParams.fLoops && !ptSound->fStitching )
        {
            dwMode = FMOD_LOOP_NORMAL;
        }
        else
        {
            dwMode = FMOD_LOOP_OFF;
        }

        result = FMOD_Sound_SetMode(ptFModSound, dwMode);
        result = FMOD_Sound_SetLoopCount(ptFModSound, nLoopCount);
    }

    FMOD_CHANNEL *ptFModChannel = NULL;
    {









        result = FMOD_System_PlaySound(sFModSoundGlobals.ptFModSystem, ptFModSound, NULL, TRUE, &ptFModChannel);
    }


    if (!sCheckResult(result, "FMOD_System_PlaySound", ""))
    {
        sReleaseSound(ptCachedSound, FALSE);
        return FALSE;
    }




    result = FMOD_Channel_SetVolume(ptFModChannel, tParams.flVolume);




    int32 nChannelIndex;
    result = FMOD_Channel_GetIndex(ptFModChannel, &nChannelIndex);

    ptSound->tChannelState[nWhichChannel].Set(nChannelIndex, snoSoundBank);

    PlayingSoundData *ptPlayingSoundData = &sFModSoundGlobals.arPlayingSounds[nChannelIndex];
    ptPlayingSoundData->flAppliedVolume = 1.0f;
    ptPlayingSoundData->tRolloffParams = ptSound->tRolloffParams;

    sSetChannelProperties(ptFModSound, ptFModChannel, ptSound, tSoundSample, tParams, ptPlayingSoundData->arDspEffects, dwFreq, ptPlayingSoundData);


    if (ptSound->fStitching)
    {


        if (ptSound->tChannelState[1 - nWhichChannel].nChannelIndex >= 0)
        {


            PlayingSoundData *ptOtherSoundData = &sFModSoundGlobals.arPlayingSounds[ptSound->tChannelState[1 - nWhichChannel].nChannelIndex];
            if (ptOtherSoundData->ptFModChannel)
            {



                unsigned long long nParentDSPClock = 0;






                result = FMOD_Channel_GetDSPClock(ptOtherSoundData->ptFModChannel, NULL, &nParentDSPClock);



				uint32 dwEndDSP;
				if (result == FMOD_OK)
				{
					uint32 length = 0;
					float freq = 0.0f;


					result = FMOD_Sound_GetLength(ptOtherSoundData->ptFModSound, &length, FMOD_TIMEUNIT_PCM);
					sCheckResult(result, "FMOD_Sound_GetLength", "");
					result = FMOD_Channel_GetFrequency(ptOtherSoundData->ptFModChannel, &freq);
					sCheckResult(result, "FMOD_Channel_GetFrequency", "");


					int32 nOutputSampleRate;

                    result = FMOD_System_GetSoftwareFormat(sFModSoundGlobals.ptFModSystem, &nOutputSampleRate, NULL, NULL);



                    sCheckResult(result, "FMOD_System_GetSoftwareFormat", "");

					if (freq != 0.0f)
					{
						float flConversionFactor = nOutputSampleRate / freq;
						dwEndDSP = (uint32)(length * flConversionFactor);


                        nParentDSPClock += dwEndDSP;
					}
				}










                result = FMOD_Channel_SetDelay(ptFModChannel, nParentDSPClock, 0, TRUE);



                sCheckResult(result, "FMOD_Channel_SetDelay", "");
            }
        }
    }



    result = FMOD_Channel_SetPaused(ptFModChannel, FALSE);
    if (!sCheckResult(result, "FMOD_Channel_SetPaused", ""))
    {
        result = FMOD_Channel_Stop(ptFModChannel);
        sReleaseSound(ptCachedSound, FALSE);
        return FALSE;
    }



    if (ptPlayingSoundData->ptCachedSound)
    {
        sReleaseSound(ptPlayingSoundData->ptCachedSound, FALSE);
    }

    ptPlayingSoundData->idSound         = ptSound->id;
    ptPlayingSoundData->ptFModSound     = ptFModSound;
    ptPlayingSoundData->ptFModChannel   = ptFModChannel;
    ptPlayingSoundData->ptCachedSound   = ptCachedSound;
    ptPlayingSoundData->flDefaultFrequency = 0.0f;
    ptSound->flRealStartTime = CGameGetClientTime();
    return TRUE;

}



static CachedSoundType sGetCachedSoundType(SoundInstance * ptSound)
{
    return ptSound->tParams.eStreamMode == 1 ? FMOD_CACHED_SOUND_STREAM : FMOD_CACHED_SOUND_SAMPLE;
}








FModCachedSound * sReloadSound(
    FModCachedSound * ptOldCachedSound,
    SoundInstance * ptSound,
    const SNO snoSoundBank,
    int32 nSampleIndex)
{




    sGarbageCollectFModMemory(TRUE);

    sReleaseSound(ptOldCachedSound, TRUE);

    CachedSoundType eSoundType = sGetCachedSoundType(ptSound);
    BOOL fSuppressTracing = ptSound->tParams.flVolume < EPSILON;


    return sGetSound(snoSoundBank, nSampleIndex, eSoundType, fSuppressTracing, FALSE);
}

FModCachedSound * sReloadMusic(
    FModCachedSound * ptOldCachedSound,
    PlaylistEntry & tEntry,
    const float flVolume)
{


    sGarbageCollectFModMemory(TRUE);

    sReleaseSound(ptOldCachedSound, TRUE);

    BOOL fSuppressTracing = flVolume < EPSILON;


    return sGetSound(tEntry.snoValidatedSoundBank, tEntry.nSampleIndex, FMOD_CACHED_SOUND_STREAM, fSuppressTracing, FALSE);
}



// Inferred: the HAS_PLAYED block appears in both the pending and the played path (its instructions carry no line rows: merged locations of two sunk copies); 3980:9 puts `if (sPlaySound(...))` at function level.
static BOOL sLoadAndPlaySound(
    SoundInstance * ptSound,
    const SNO snoSoundBank,
    int32 nWhichChannel,
    SoundSample & tSoundSample)
{
    CachedSoundType eSoundType = sGetCachedSoundType(ptSound);

    int32 nSampleIndex;

    {
        ScopedSoundBankDefinitionLockR ptSoundBankDef(snoSoundBank);

        nSampleIndex = (int32)(&tSoundSample - ptSoundBankDef->ptSoundSamples);

        if (nSampleIndex < 0 || nSampleIndex >= ptSoundBankDef->nSoundSampleCount)
        {
            return FALSE;
        }
    }


    BOOL fSuppressTracing = ptSound->tParams.flVolume < EPSILON;
    FModCachedSound * ptCachedSound = sGetSound(snoSoundBank, nSampleIndex, eSoundType, fSuppressTracing, FALSE);
    if (!ptCachedSound)
    {
        return FALSE;
    }


    if (ptCachedSound->fPending)
    {

        sptAsyncAudio->QueueSound(ptCachedSound, ptSound, snoSoundBank, nWhichChannel, nSampleIndex);
        if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_HAS_PLAYED_BIT)))
        {
            ptSound->dwFlags |= (1 << SOUNDINSTANCE_HAS_PLAYED_BIT);
            sAddSoundInstance(ptSound->tSoundParams.snoSound);
        }
        return TRUE;
    }




    if (sPlaySound(ptCachedSound, ptSound, snoSoundBank, nWhichChannel, tSoundSample))
    {
        if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_HAS_PLAYED_BIT)))
        {
            ptSound->dwFlags |= (1 << SOUNDINSTANCE_HAS_PLAYED_BIT);
            sAddSoundInstance(ptSound->tSoundParams.snoSound);
        }
        return TRUE;
    }

    return FALSE;

}



void SoundPlatform::SetCustomListenerMode(const BOOL fVal)
{
    sFModSoundGlobals.bCustomListenerMode = fVal;
}








BOOL SoundPlatform::InCustomListenerMode()
{
    return sFModSoundGlobals.bCustomListenerMode;
}









static void sInsertBestLocation(
    const FModSampleToPreload & sampleToPreload)
{
    Blizzard::Lock::ScopedMutex lock(sFModSoundGlobals.tPreloadListLock);

    BOOL fDummy;
    if (sFModSoundGlobals.pendingPreloadMap.Lookup(sampleToPreload.info, fDummy))
    {
        // returns have no line row; placed inside the if (branch goes straight to ~ScopedMutex)
        return;
    }

    BOOL fSameArchiveFound = FALSE;
    for (POS preloadPOS = sFModSoundGlobals.pendingPreloadList.GetHeadPosition(); preloadPOS != NULL;)
    {
        POS checkPreloadPOS = preloadPOS;
        FModSampleToPreload * ptSampleToPreload = &sFModSoundGlobals.pendingPreloadList.GetNext(preloadPOS);


        if (ptSampleToPreload->info.snoSoundBank == sampleToPreload.info.snoSoundBank && ptSampleToPreload->info.nSampleIndex == sampleToPreload.info.nSampleIndex)
        {
            return;
        }

        if (ptSampleToPreload->ptArchive == sampleToPreload.ptArchive)
        {
            if (ptSampleToPreload->nLocation > sampleToPreload.nLocation)
            {

                sFModSoundGlobals.pendingPreloadList.InsertBefore_Tracked(checkPreloadPOS, sampleToPreload);
                sFModSoundGlobals.pendingPreloadMap.SetAt_Tracked(sampleToPreload.info, TRUE);
                return;
            }
            // no line row of its own (line 0 orr w8, #1 after the 4047 compare)
            fSameArchiveFound = TRUE;
        }
        else if (fSameArchiveFound)
        {

            sFModSoundGlobals.pendingPreloadList.InsertBefore_Tracked(checkPreloadPOS, sampleToPreload);
            sFModSoundGlobals.pendingPreloadMap.SetAt_Tracked(sampleToPreload.info, TRUE);
            return;
        }
    }


    sFModSoundGlobals.pendingPreloadList.AddTail_Tracked(sampleToPreload);
    sFModSoundGlobals.pendingPreloadMap.SetAt_Tracked(sampleToPreload.info, TRUE);
}




void SoundPlatform::Preload(
    const SNO snoSoundBank,
    int32 nSampleIndex)
{

    char szFileName[4096];
    SoundBankGetSampleFullFilename(szFileName, sizeof(szFileName), snoSoundBank, nSampleIndex);

    Console::IO::FileInfo info;
    Console::IO::CPKArchiveReader * ptArchive = Console::IO::FindArchiveContaining(szFileName, &info);
    if (ptArchive != NULL)
    {
        FModSampleToPreload sampleToPreload;
        sampleToPreload.info.snoSoundBank = snoSoundBank;
        sampleToPreload.info.nSampleIndex = nSampleIndex;
        sampleToPreload.ptArchive = ptArchive;
        sampleToPreload.nLocation = ptArchive->GetLocation(info.nLocationIndex);
        sInsertBestLocation(sampleToPreload);
    }










}



BOOL SoundPlatform::IsPreloaded(
    const SNO snoSoundBank,
    int32 nSampleIndex)
{
    FModCachedSoundKey tKey;
    tKey.snoSoundBank = snoSoundBank;
    tKey.nSampleIndex = (uint16)nSampleIndex;
    FModCachedSoundSampleMap * ptSoundMap = sFModSoundGlobals.ptCachedSoundSampleMap;

    FModCachedSound * ptCachedSound;
    if (ptSoundMap->Lookup(tKey, ptCachedSound))
    {
        return TRUE;
    }


    Blizzard::Lock::ScopedMutex lock(sFModSoundGlobals.tPreloadListLock);

    BOOL fValue;
    return sFModSoundGlobals.pendingPreloadMap.Lookup(tKey, fValue);






}
















BOOL SoundPlatform::IsReady(const SNO snoSoundBank, int32 nSampleIndex)
{
    FModCachedSoundSampleMap * ptSoundMap;
    FModCachedSound * ptCachedSound;
    FModCachedSoundKey tKey;
    tKey.snoSoundBank = snoSoundBank;
    tKey.nSampleIndex = (uint16)nSampleIndex;
    ptSoundMap = sFModSoundGlobals.ptCachedSoundSampleMap;



    if (!ptSoundMap->Lookup(tKey, ptCachedSound))
        return FALSE;




    return !ptCachedSound->fPending;



}


BOOL SoundPlatform::Play(
    SoundInstance * ptSound,
    const SNO snoSoundBank,
    SoundSample & tSoundSample,
    SoundPlaybackParams & tParams)
{




    ptSound->tParams = tParams;
    ptSound->fStitching = tParams.fChooseNewPermutationOnLoop && tParams.fLoops;
    ptSound->nCurrentChannel = 0;
    if (ptSound->fStitching)
    {


        if (!sLoadAndPlaySound(ptSound, snoSoundBank, ptSound->nCurrentChannel, tSoundSample))
            return FALSE;



        ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
        int32 nPermutationIndexToUse = SelectSoundPermutation(pdefSound, const_cast<SoundPlayHistory *>(&pdefSound->tSoundPlayHistory));

        const SoundPermutation & tPermutation = pdefSound->ptPermutations[nPermutationIndexToUse];
        ScopedSoundBankDefinitionLockR pdefSoundBank(tPermutation.snoSoundBank);
        SoundSample * ptSoundSample = &pdefSoundBank->ptSoundSamples[tPermutation.nSampleIndex];


        if (!sLoadAndPlaySound(ptSound, tPermutation.snoSoundBank, 1 - ptSound->nCurrentChannel, *ptSoundSample))
        {

            if (sptAsyncAudio->IsPending(ptSound))
            {
                sptAsyncAudio->Cancel(ptSound);
            }



            if (ptSound->dwFlags & (1 << SOUNDINSTANCE_HAS_PLAYED_BIT))
            {
                sRemoveSoundInstance(ptSound->tSoundParams.snoSound);
            }

            return FALSE;
        }

        return TRUE;
    }



    return sLoadAndPlaySound(ptSound, snoSoundBank, ptSound->nCurrentChannel, tSoundSample);

}



static void sSetMusicChannelProperties(FMOD_SOUND * ptFModSound, FMOD_CHANNEL * ptFModChannel, SoundInstance * ptSound, PlaylistEntry & tEntry, const uint32 dwStartSample, uint32 dwFreq)
{





    float flSamplesToMS = 1000.0f / dwFreq;


    FMOD_RESULT result = FMOD_Channel_SetChannelGroup(ptFModChannel, sFModSoundGlobals.ptMusicChannelGroup);
    sCheckResult(result, "FMOD_Channel_SetChannelGroup", "");


    sClearChannelReverb(ptFModChannel);

    FMOD_Channel_SetMode(ptFModChannel, FMOD_2D);


    uint32 dwSamplesBeforeStart = tEntry.GetSamplesBeforeStart(dwFreq);

    dwSamplesBeforeStart = Math::Min(dwSamplesBeforeStart, dwStartSample);
    uint32 dwStartSampleToUse = dwStartSample - dwSamplesBeforeStart;
    if (dwStartSampleToUse > 0)
    {
        uint32 nStartMS = (uint32)(flSamplesToMS * dwStartSampleToUse);
        result = sFMODChannelSetPosition(ptFModSound, ptFModChannel, nStartMS, FMOD_TIMEUNIT_MS);
        sCheckResult(result, "FMOD_Channel_SetPosition", "");
    }



    {
        FMOD_MODE dwMode = FMOD_LOOP_OFF;
        int32 nLoopCount = 0;

        if (tEntry.dwLoopStartPos != 0 || tEntry.dwLoopEndPos != 0)
        {
            nLoopCount = tEntry.dwLoopCount;
            dwMode = FMOD_LOOP_NORMAL;
            uint32 dwLoopStartMS = (uint32)(flSamplesToMS * tEntry.dwLoopStartPos);
            uint32 dwLoopEndMS = (uint32)(flSamplesToMS * tEntry.dwLoopEndPos);
            result = FMOD_Channel_SetLoopPoints(ptFModChannel, dwLoopStartMS, FMOD_TIMEUNIT_MS, dwLoopEndMS, FMOD_TIMEUNIT_MS);
            sCheckResult(result, "FMOD_Channel_SetLoopPoints", "");
        }



        result = FMOD_Channel_SetMode(ptFModChannel, dwMode);
        sCheckResult(result, "FMOD_Channel_SetMode", "");
        result = FMOD_Channel_SetLoopCount(ptFModChannel, nLoopCount);
        sCheckResult(result, "FMOD_Channel_SetLoopCount", "");
    }





    {
        unsigned long long nParentDSPClock = 0;




        if (tEntry.dwEndPos != 0)
        {

            uint32 dwSamplesToWait = tEntry.GetBaseDurationSamples(dwStartSample) + dwSamplesBeforeStart + tEntry.GetSamplesAfterEnd(dwFreq);


            int32 nOutputSampleRate;
            FMOD_System_GetSoftwareFormat(sFModSoundGlobals.ptFModSystem, &nOutputSampleRate, NULL, NULL);



            float flConversionFactor = (float)nOutputSampleRate / (float)dwFreq;
            uint32 dwEndDSP = (uint32)(flConversionFactor * dwSamplesToWait);


            result = FMOD_Channel_GetDSPClock(ptFModChannel, NULL, &nParentDSPClock);
            nParentDSPClock += dwEndDSP;
            sCheckResult(result, "FMOD_Channel_GetDSPClock", "");
        }







        result = FMOD_Channel_SetDelay(ptFModChannel, 0, nParentDSPClock, TRUE);



        sCheckResult(result, "FMOD_Channel_SetDelay", "");
    }


    result = FMOD_Channel_SetUserData(ptFModChannel, (void *)(intptr_t)ptSound->id);
    sCheckResult(result, "FMOD_Channel_SetUserData", "");
}

BOOL sPlayMusic(FModCachedSound * ptCachedSound, SoundInstance * ptSound, PlaylistEntry & tEntry, const uint32 dwStartSample, const float flVolume)
{


    FMOD_SOUND * ptFModSound = ptCachedSound->ptFModSound;


    ScopedSoundBankDefinitionLockR pdefSoundBank(tEntry.snoValidatedSoundBank);
    SoundID idSound = ptSound->id;
    SoundSample & tSoundSample = pdefSoundBank->ptSoundSamples[tEntry.nSampleIndex];

    uint32 dwChannels;
    uint32 dwFreq;
    uint32 dwBitRate;
    SoundFormatToCSB(tSoundSample.GetFormat(), dwChannels, dwFreq, dwBitRate);











    FMOD_RESULT result;
    {








        uint32 dwPriority = Math::Pin(ptSound->dwPriority, 0u, 128u);


        result = FMOD_Sound_SetDefaults(ptFModSound, (float)dwFreq, 128 - dwPriority);
    }





    if (!sCheckResult(result, "FMOD_Sound_SetDefaults", ""))
    {
        sReleaseSound(ptCachedSound, FALSE);
        return FALSE;
    }
    FMOD_CHANNEL * ptFModChannel = NULL;











    result = FMOD_System_PlaySound(sFModSoundGlobals.ptFModSystem, ptFModSound, NULL, TRUE, &ptFModChannel);



    if (!sCheckResult(result, "FMOD_System_PlaySound", ""))
    {
        sReleaseSound(ptCachedSound, FALSE);
        return FALSE;
    }



    FMOD_Channel_SetVolume(ptFModChannel, flVolume);





    int32 nChannelIndex;
    FMOD_Channel_GetIndex(ptFModChannel, &nChannelIndex);

    ptSound->tChannelState[ptSound->nCurrentChannel].Set(nChannelIndex, tEntry.snoValidatedSoundBank);

    PlayingSoundData * ptPlayingSoundData = &sFModSoundGlobals.arPlayingSounds[nChannelIndex];

    sSetMusicChannelProperties(ptFModSound, ptFModChannel, ptSound, tEntry, dwStartSample, dwFreq);



    result = FMOD_Channel_SetPaused(ptFModChannel, FALSE);
    if (!sCheckResult(result, "FMOD_Channel_SetPaused", ""))
    {

        FMOD_Channel_Stop(ptFModChannel);
        sReleaseSound(ptCachedSound, FALSE);
        return FALSE;
    }




    if (ptPlayingSoundData->ptCachedSound)
    {

        sReleaseSound(ptPlayingSoundData->ptCachedSound, FALSE);
    }

    ptPlayingSoundData->idSound = idSound;
    ptPlayingSoundData->ptFModSound = ptFModSound;
    ptPlayingSoundData->ptFModChannel = ptFModChannel;
    ptPlayingSoundData->ptCachedSound = ptCachedSound;
    return TRUE;
}

BOOL SoundPlatform::PlayMusicEntry(SoundInstance * ptSound, PlaylistEntry & tEntry, const uint32 dwStartSample, const float flVolume)
{






    BOOL fSuppressTracing = flVolume < 1e-06f;
    FModCachedSound * ptCachedSound = sGetSound(tEntry.snoValidatedSoundBank, tEntry.nSampleIndex, FMOD_CACHED_SOUND_STREAM, fSuppressTracing, FALSE);
    if (!ptCachedSound)
    {



        return FALSE;
    }
    ptSound->dwFlags |= (1 << SOUNDINSTANCE_HAS_PLAYED_BIT);


    if (ptCachedSound->fPending)
    {


        sptAsyncAudio->QueueMusic(ptCachedSound, ptSound, tEntry, dwStartSample, flVolume);
        return TRUE;
    }



    return sPlayMusic(ptCachedSound, ptSound, tEntry, dwStartSample, flVolume);
}

FMOD_RESULT sOVTReadCallback(FMOD_SOUND * sound, void * data, unsigned int datalen)
{
    datalen /= sizeof(int16);
    void * pUserData = NULL;
    FMOD_Sound_GetUserData(sound, &pUserData);
    SoundOVT * ptSoundOVT = (SoundOVT *)pUserData;
    uint32 dwHead = ptSoundOVT->dwAudioHead;
    int16 * pSrc = ptSoundOVT->lpAudioBuffer;
    const uint32 dwTail = ptSoundOVT->dwAudioTail;
    const uint32 dwSize = ptSoundOVT->dwAudioBufferSize;
    int16 * pDst = (int16 *)data;
    for (uint32 i = 0; i < datalen; i++)
    {
        if (dwHead == dwTail)
        {
            pDst[i] = 0;
        }
        else
        {
            pDst[i] = pSrc[dwHead];
            dwHead = (dwHead + 1 < dwSize) ? dwHead + 1 : 0;
        }
    }
    ptSoundOVT->dwAudioHead = dwHead;
    ptSoundOVT->dwAudioPos += datalen;
    return FMOD_OK;
}















BOOL SoundPlatform::OpenOVTStream(SoundOVT * ptSoundOVT)
{
    SoundOVT & tOVT = *ptSoundOVT;
    FMOD_CREATESOUNDEXINFO tSoundExInfo;
    SigmaMemoryClear(&tSoundExInfo, sizeof(tSoundExInfo));
    tSoundExInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    tSoundExInfo.decodebuffersize = (tOVT.dwFrequency * tOVT.dwChannels) / tOVT.dwFPS;
    tSoundExInfo.length = tOVT.dwFrequency * tOVT.dwChannels * sizeof(int16);
    tSoundExInfo.numchannels = tOVT.dwChannels;
    tSoundExInfo.defaultfrequency = tOVT.dwFrequency;
    tSoundExInfo.format = FMOD_SOUND_FORMAT_PCM16;
    tSoundExInfo.pcmreadcallback = sOVTReadCallback;
    tSoundExInfo.userdata = ptSoundOVT;
    tSoundExInfo.fileuserdata = new int32;
    FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_2D | FMOD_OPENUSER | FMOD_LOWMEM;
    FMOD_RESULT result = sCreateFModStreamCheckMemory(sFModSoundGlobals.ptFModSystem, NULL, mode, &tSoundExInfo, &sFModSoundGlobals.tOVTStreamSound.ptFModSound);
    sCheckResult(result, "FMOD_System_CreateStream", "");
    return TRUE;
}



















void SoundPlatform::PlayOVTStream(const float flVolume)
{
    if (sFModSoundGlobals.tOVTStreamSound.ptFModSound)
    {
        FMOD_RESULT result = FMOD_System_PlaySound(sFModSoundGlobals.ptFModSystem, sFModSoundGlobals.tOVTStreamSound.ptFModSound, NULL, FALSE, &sFModSoundGlobals.tOVTStreamSound.ptFModChannel);
        sCheckResult(result, "FMOD_System_PlaySound", "");
        result = FMOD_Channel_SetVolume(sFModSoundGlobals.tOVTStreamSound.ptFModChannel, flVolume);
        sCheckResult(result, "FMOD_Channel_SetVolume", "");
        sClearChannelReverb(sFModSoundGlobals.tOVTStreamSound.ptFModChannel);
    }
}













void SoundPlatform::StopOVTStream()
{
    if (sFModSoundGlobals.tOVTStreamSound.ptFModSound)
    {
        if (sFModSoundGlobals.tOVTStreamSound.ptFModChannel)
        {




            FMOD_RESULT result = FMOD_Channel_Stop(sFModSoundGlobals.tOVTStreamSound.ptFModChannel);
            sCheckResult(result, "FMOD_Channel_Stop", "");
            sFModSoundGlobals.tOVTStreamSound.ptFModChannel = NULL;
        }
        sReleaseFModSound(sFModSoundGlobals.tOVTStreamSound.ptFModSound, FALSE);
        sFModSoundGlobals.tOVTStreamSound.ptFModSound = NULL;
        CutsceneMarkToBeStopped(CutsceneGetFullScreenCutscene());
    }
}







BOOL SoundPlatform::GetOVTStreamPos(float & flStreamPos)
{
    PlayingSoundData & tOVTStream = sFModSoundGlobals.tOVTStreamSound;
    if (tOVTStream.ptFModSound && tOVTStream.ptFModChannel)
    {
        uint32 nNewStreamPos = 0;
        FMOD_RESULT res = FMOD_Channel_GetPosition(tOVTStream.ptFModChannel, &nNewStreamPos, FMOD_TIMEUNIT_PCMBYTES);
        if (!sCheckResult(res, "FMOD_Channel_GetPosition", ""))
            return FALSE;
        void * pUserData = NULL;
        FMOD_Sound_GetUserData(tOVTStream.ptFModSound, &pUserData);
        SoundOVT * ptSoundOVT = (SoundOVT *)pUserData;
        uint32 nBufferSize = ptSoundOVT->dwFrequency * ptSoundOVT->dwChannels * sizeof(int16);
        if (nNewStreamPos >= nBufferSize)
            nNewStreamPos = nBufferSize - ptSoundOVT->dwChannels * sizeof(int16);
        if (nNewStreamPos < ptSoundOVT->nLastBufferPosPCMBytes) ptSoundOVT->nLastPosPCMBytes += nBufferSize - ptSoundOVT->nLastBufferPosPCMBytes + nNewStreamPos; else ptSoundOVT->nLastPosPCMBytes += nNewStreamPos - ptSoundOVT->nLastBufferPosPCMBytes;
        ptSoundOVT->nLastBufferPosPCMBytes = nNewStreamPos;
        flStreamPos = (float)ptSoundOVT->nLastPosPCMBytes / (float)nBufferSize;
        return TRUE;
    }
    return FALSE;
}





























void SoundPlatform::Stop(SoundInstance * ptSound)
{
    sFreeSound(ptSound);
}



void SoundPlatform::SetPosition(SoundInstance * ptSound, const Vector3D & wp)
{




    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetPosition(ptSound, wp);
        return;
    }

    if (ptSound->wp != wp)
    {
        ptSound->wp = wp;
        for (int32 i = 0; i < 2; i++)
        {
            PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
            if (ptPlayingSoundData)
                FMOD_Channel_Set3DAttributes(ptPlayingSoundData->ptFModChannel, (const FMOD_VECTOR *)&wp, NULL, NULL);
        }
    }
}








void SoundPlatform::SetVelocity(SoundInstance * ptSound, const Vector3D & wvVel)
{




    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetVelocity(ptSound, wvVel);
        return;
    }

    if (ptSound->wvVel != wvVel)
    {
        ptSound->wvVel = wvVel;
        for (int32 i = 0; i < 2; i++)
        {
            PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
            if (ptPlayingSoundData)
                FMOD_Channel_Set3DAttributes(ptPlayingSoundData->ptFModChannel, NULL, (const FMOD_VECTOR *)&wvVel, NULL);
        }
    }
}








void SoundPlatform::SetPaused(SoundInstance * ptSound, BOOL fPaused)
{








    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetPaused(ptSound, fPaused);
        return;
    }

    for (int32 i = 0; i < 2; i++)
    {
        PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
        if (ptPlayingSoundData)
            FMOD_Channel_SetPaused(ptPlayingSoundData->ptFModChannel, fPaused);
    }
}




void SoundPlatform::SetPitch(SoundInstance * ptSound, const float flPitch)
{




    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetPitch(ptSound, flPitch);
        return;
    }

    for (int32 i = 0; i < 2; i++)
    {
        PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
        if (ptPlayingSoundData)
        {
            float flDefaultFrequency = ptPlayingSoundData->flDefaultFrequency;
            if (flDefaultFrequency == 0.0f)
            {
                FMOD_Sound_GetDefaults(ptPlayingSoundData->ptFModSound, &flDefaultFrequency, NULL);
                ptPlayingSoundData->flDefaultFrequency = flDefaultFrequency;
            }





            uint32 dwFrequency = Math::FloatRoundToInt(flDefaultFrequency * flPitch);
            float flOldFrequency;
            FMOD_RESULT result = FMOD_Channel_GetFrequency(ptPlayingSoundData->ptFModChannel, &flOldFrequency);
            float flFrequency = (float)dwFrequency;
            if (result == FMOD_OK && flOldFrequency != flFrequency && Math::Abs(flOldFrequency - flFrequency) > (1.0f / 24.0f))
            {
                FMOD_Channel_SetFrequency(ptPlayingSoundData->ptFModChannel, flFrequency);
            }
        }
    }
}










void SoundPlatform::SetVolume(SoundInstance * ptSound, const float flVolume)
{




    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetVolume(ptSound, flVolume);
        return;
    }

    for (int32 i = 0; i < 2; i++)
    {
        PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
        if (ptPlayingSoundData)
        {
            float flOldVolume;
            FMOD_RESULT result = FMOD_Channel_GetVolume(ptPlayingSoundData->ptFModChannel, &flOldVolume);
            if (result == FMOD_OK && flOldVolume != flVolume && (flVolume == 0.0f || Math::Abs(flOldVolume - flVolume) > (1.0f / 256.0f)))
            {
                FMOD_RESULT result = FMOD_Channel_SetVolume(ptPlayingSoundData->ptFModChannel, flVolume);
            }
            ptPlayingSoundData->flAppliedVolume = flVolume;
            if (ptSound->tParams.f3D)
                sUpdate3DPanLevel(ptSound, flVolume, ptPlayingSoundData->ptFModChannel);
        }
    }
}

















static void sFModReverbPropertiesFromReverbParams(FMOD_REVERB_PROPERTIES & tfmodReverb, const ReverbParams & tReverb, const ReverbParamsExtended * ptReverbExtended)
{

    if (ptReverbExtended)
    {
        tfmodReverb.DecayTime = tReverb.flDecayTime;
        tfmodReverb.EarlyDelay = tReverb.flReflectionsDelay;
        tfmodReverb.LateDelay = tReverb.flReverbDelay;
        tfmodReverb.HFReference = tReverb.flHFReference;
        tfmodReverb.HFDecayRatio = tReverb.flDecayHFRatio;
        tfmodReverb.LowShelfFrequency = tReverb.flLFReference;
        tfmodReverb.LowShelfGain = (float)tReverb.nRoomLF;
        tfmodReverb.Diffusion = ptReverbExtended->flDiffusion;
        tfmodReverb.Density = ptReverbExtended->flDensity;
        tfmodReverb.HighCut = ptReverbExtended->flConvertedHighCut;
        tfmodReverb.EarlyLateMix = ptReverbExtended->flConvertedEarlyLateMix;
        tfmodReverb.WetLevel = ptReverbExtended->flConvertedWetLevel;
    }
    else
    {





        tfmodReverb.DecayTime = tReverb.flDecayTime * 1000.0f;
        tfmodReverb.EarlyDelay = tReverb.flReflectionsDelay * 1000.0f;
        tfmodReverb.LateDelay = tReverb.flReverbDelay * 1000.0f;
        tfmodReverb.HFReference = tReverb.flHFReference;
        tfmodReverb.HFDecayRatio = Math::Pin(tReverb.flDecayHFRatio * 100.0f, 0.0f, 100.0f);
        tfmodReverb.Diffusion = 100.0f;
        tfmodReverb.Density = 100.0f;
        tfmodReverb.LowShelfFrequency = tReverb.flLFReference;
        tfmodReverb.LowShelfGain = Math::Pin(tReverb.nRoomLF / 100.0f, -48.0f, 12.0f);

        if (tReverb.nRoomHF < 0)
        {
            float nHFGain = Math::Pow(10.0f, tReverb.nRoomHF / 2000.0f);
            tfmodReverb.HighCut = Math::Pin(tReverb.flHFReference / Math::Sqrt((1.0f - nHFGain) / nHFGain), 20.0f, 20000.0f);
        }
        else
        {
            tfmodReverb.HighCut = 20000.0f;
        }

        if (tReverb.nReflections > -10000)
        {
            float nLateEarlyRatio = Math::Pow(10.0f, (tReverb.nReverb - tReverb.nReflections) / 2000.0f);
            tfmodReverb.EarlyLateMix = (nLateEarlyRatio / (nLateEarlyRatio + 1.0f)) * 100.0f;
        }
        else
        {
            tfmodReverb.EarlyLateMix = 100.0f;
        }

        float nEarlyAndLatePower = Math::Pow(10.0f, tReverb.nReflections / 1000.0f) + Math::Pow(10.0f, tReverb.nReverb / 1000.0f);
        tfmodReverb.WetLevel = Math::Pin(log10f(nEarlyAndLatePower) * 10.0f + tReverb.nRoom / 100.0f, -80.0f, 20.0f);
    }
}























































static void sReverbParamsFromFModReverbProperties(ReverbParams & tReverb, const FMOD_REVERB_PROPERTIES & tfmodReverb, ReverbParamsExtended * ptReverbExtended)
{






    SigmaMemoryClear(&tReverb, sizeof(tReverb));
    tReverb.flDecayTime = tfmodReverb.DecayTime;
    tReverb.flReflectionsDelay = tfmodReverb.EarlyDelay;
    tReverb.flReverbDelay = tfmodReverb.LateDelay;
    tReverb.flHFReference = tfmodReverb.HFReference;
    tReverb.flDecayHFRatio = tfmodReverb.HFDecayRatio;
    tReverb.flLFReference = tfmodReverb.LowShelfFrequency;




    tReverb.nRoomLF = Math::FloatRoundToInt(tfmodReverb.LowShelfGain);

    if (ptReverbExtended)
    {
        ptReverbExtended->flDiffusion = tfmodReverb.Diffusion;
        ptReverbExtended->flDensity = tfmodReverb.Density;
        ptReverbExtended->flConvertedHighCut = tfmodReverb.HighCut;
        ptReverbExtended->flConvertedEarlyLateMix = tfmodReverb.EarlyLateMix;
        ptReverbExtended->flConvertedWetLevel = tfmodReverb.WetLevel;
    }
}



































void SoundPlatform::SetReverbParams(const ReverbParams & tReverb, const ReverbParamsExtended * ptReverbExtended)
{
    FMOD_REVERB_PROPERTIES tfmodReverb;
    SigmaMemoryClear(&tfmodReverb, sizeof(tfmodReverb));



    sFModReverbPropertiesFromReverbParams(tfmodReverb, tReverb, ptReverbExtended);
    FMOD_RESULT result = FMOD_System_SetReverbProperties(sFModSoundGlobals.ptFModSystem, 0, &tfmodReverb);
    sCheckResult(result, "FMOD_System_SetReverbProperties", NULL);
}









void SoundPlatform::SetOcclusion(SoundInstance * ptSound, const float flOcclusion)
{




    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetOcclusion(ptSound, flOcclusion);
        return;
    }

    for (int32 i = 0; i < 2; i++)
    {
        PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
        if (ptPlayingSoundData)
            FMOD_Channel_Set3DOcclusion(ptPlayingSoundData->ptFModChannel, flOcclusion, 1.0f);
    }
}






void SoundPlatform::SetReverbParamsFromPreset(ReverbParams & tReverb, SoundReverbPreset ePreset, ReverbParamsExtended * ptReverbExtended)
{
    static const FMOD_REVERB_PROPERTIES arFModReverbProperties[SOUND_REVERB_NUM_PRESETS] = {
        FMOD_PRESET_OFF,
        FMOD_PRESET_GENERIC,
        FMOD_PRESET_PADDEDCELL,
        FMOD_PRESET_ROOM,
        FMOD_PRESET_BATHROOM,
        FMOD_PRESET_LIVINGROOM,
        FMOD_PRESET_STONEROOM,
        FMOD_PRESET_AUDITORIUM,
        FMOD_PRESET_CONCERTHALL,
        FMOD_PRESET_CAVE,
        FMOD_PRESET_ARENA,
        FMOD_PRESET_HANGAR,
        FMOD_PRESET_CARPETTEDHALLWAY,
        FMOD_PRESET_HALLWAY,
        FMOD_PRESET_STONECORRIDOR,
        FMOD_PRESET_ALLEY,
        FMOD_PRESET_FOREST,
        FMOD_PRESET_CITY,
        FMOD_PRESET_MOUNTAINS,
        FMOD_PRESET_QUARRY,
        FMOD_PRESET_PLAIN,
        FMOD_PRESET_PARKINGLOT,
        FMOD_PRESET_SEWERPIPE,
        FMOD_PRESET_UNDERWATER,
    };



    sReverbParamsFromFModReverbProperties(tReverb, arFModReverbProperties[ePreset], ptReverbExtended);
}






uint32 SoundPlatform::GetStreamPosition(const SoundInstance * ptSound)
{
    uint32 dwPosition = 0xFFFFFFFF;
    PlayingSoundData * ptPlayingSoundData;

    if (sptAsyncAudio->IsPending(ptSound))
        return 0;



    ptPlayingSoundData = sGetPlayingSoundData(ptSound, ptSound->nCurrentChannel);
    if (ptPlayingSoundData)
    {
        uint32 nPos;
        FMOD_Channel_GetPosition(ptPlayingSoundData->ptFModChannel, &nPos, FMOD_TIMEUNIT_MS);
        dwPosition = nPos;
    }
    return dwPosition;
}






void SoundPlatform::SetStreamPosition(SoundInstance * ptSound, const uint32 dwPos)
{
    PlayingSoundData * ptPlayingSoundData;


    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->SetStreamPosition(ptSound, dwPos);
        return;
    }

    ptPlayingSoundData = sGetPlayingSoundData(ptSound, ptSound->nCurrentChannel);
    if (ptPlayingSoundData)
    {

        sFMODChannelSetPosition(ptPlayingSoundData->ptFModSound, ptPlayingSoundData->ptFModChannel, dwPos, FMOD_TIMEUNIT_MS);
    }
}
// NON_MATCHING: 920 vs 924 bytes; for OLDEST/NEWEST the original reloads ptSound->id before storing idSoundToSteal, ours reuses the id loaded by the inlined Iterate free-slot check; on DWARF lines
int32 SoundPlatform::GetSoundInstanceCount(
    const SoundDefinition * pdefSound,
    const Vector3D * pwp,
    SoundID & idSoundToSteal)
{
    int32 nLimitInstance = pdefSound->nInstanceLimit;
    InstanceLimitBehavior eBehavior = pdefSound->eLimitBehavior;
    SNO snoSound = pdefSound->tHeader.snoHandle;

    idSoundToSteal = SOUNDID_INVALID;
    uint32 dwBestTime = (eBehavior == ILB_STEAL_OLDEST) ? 0 : 0xFFFFFFFF;
    float flBestVol = FLT_MAX;
    float flBestDist = 0.0f;
    uint32 dwCurrTime = GetMachineTicks();

    int32 * pnInstanceCount = sFModSoundGlobals.tSoundInstanceMap.LookupPtr(snoSound);

    if (!pnInstanceCount)
        return 0;


    if (*pnInstanceCount < nLimitInstance || eBehavior == ILB_FAIL)
        return *pnInstanceCount;







    Vector3D wpListener;
    SoundManager::ListenerGetVolumePosition(wpListener);

    TDataArrayIterator<SoundInstance> tIt;
    while (gptSounds->Iterate(tIt))
    {
        SoundInstance * ptSound = tIt.ptElement;

        if (!(ptSound->dwFlags & (1 << SOUNDINSTANCE_HAS_PLAYED_BIT)) || (ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT)))
            continue;








        if (ptSound->tSoundParams.snoSound == snoSound)
        {




            switch (eBehavior)
            {
                case ILB_STEAL_OLDEST:
                {
                    uint32 dwAge = dwCurrTime - ptSound->dwStartTime;
                    if (idSoundToSteal == SOUNDID_INVALID || dwAge > dwBestTime)
                    {
                        idSoundToSteal = ptSound->id;
                        dwBestTime = dwAge;
                    }
                }
                break;

                case ILB_STEAL_NEWEST:
                {

                    uint32 dwAge = dwCurrTime - ptSound->dwStartTime;
                    if (idSoundToSteal == SOUNDID_INVALID || dwAge < dwBestTime)
                    {
                        idSoundToSteal = ptSound->id;
                        dwBestTime = dwAge;
                    }
                }
                break;

                case ILB_STEAL_QUIETEST:
                {


                    float flVolume = SoundGetAdjustedVolume(ptSound);

                    float flDist = Vector3D::Distance(wpListener, ptSound->wp);
                    float flDistAtten = SoundGetRolloffAtten(flDist, ptSound->tRolloffParams);
                    flVolume *= flDistAtten;
                    if (idSoundToSteal == SOUNDID_INVALID || flVolume < flBestVol)
                    {


                        idSoundToSteal = ptSound->id;
                        flBestVol = flVolume;
                    }
                }
                break;
                case ILB_STEAL_FARTHEST:
                {
                    float flDist = Vector3D::Distance(wpListener, ptSound->wp);
                    if (idSoundToSteal == SOUNDID_INVALID || flDist > flBestDist)
                    {


                        idSoundToSteal = ptSound->id;
                        flBestDist = flDist;
                    }
                }
                break;
            }
        }
    }


































    if (pwp && eBehavior == ILB_STEAL_FARTHEST)
    {
        float flDist = Vector3D::Distance(wpListener, *pwp);
        if (flDist > flBestDist)
        {
            idSoundToSteal = SOUNDID_INVALID;
        }
    }

    return *pnInstanceCount;
}









































































































































































































































































































































static float sFMod3DRolloffCallback(

    FMOD_CHANNELCONTROL * channelControl,



    float wdDist)
{
    FMOD_RESULT result;
    int nIndex = -1;
    float flDistAtten = 1.0f;

    FMOD_CHANNEL * channel = (FMOD_CHANNEL *)channelControl;

    result = FMOD_Channel_GetIndex(channel, &nIndex);

    if (result == FMOD_OK &&
        nIndex >= 0 && nIndex < sFModSoundGlobals.nNumChannels)
    {

        if (!sFModSoundGlobals.bCustomListenerMode &&
            sFModSoundGlobals.dwPlayerCount &&
            !CGameInScriptedSequence())
        {
            uint32 dwPlayerCount = sFModSoundGlobals.dwPlayerCount;
            Vector3D * vPlayerPositions = sFModSoundGlobals.vPlayerPositions;

            Vector3D vPos;

            result = FMOD_Channel_Get3DAttributes(channel, (FMOD_VECTOR *)&vPos, NULL, NULL);



            if (result == FMOD_OK)
            {

                wdDist = FLT_MAX;
                for (uint32 i = 0; i < dwPlayerCount; i++)
                {
                    float wdTempDist = Vector3D::Distance(vPos, vPlayerPositions[i]);
                    if (wdTempDist < wdDist)
                        wdDist = wdTempDist;
                }
            }
        }


















        PlayingSoundData * ptPlayingSound = &sFModSoundGlobals.arPlayingSounds[nIndex];

        flDistAtten = SoundGetRolloffAtten(wdDist, ptPlayingSound->tRolloffParams);
    }
    return flDistAtten;
}




static FMOD_RESULT sFileOpen(const char * name, uint32 * filesize, void ** handle, void * userdata)
{
    if (handle)
    {
        FileIO::File * ptFile = new FileIO::File(XLOCALE_GLOBAL);
        *(int32 *)userdata = name[0] - '0';
        if (ptFile->Open(name + 2))
        {
            if (filesize)
                *filesize = (uint32)ptFile->GetFileSize();
            *handle = ptFile;
            return FMOD_OK;
        }
        delete ptFile;
    }
    return FMOD_ERR_FILE_BAD;
}













































static FMOD_RESULT sFileClose(void * handle, void * userdata)
{
    FileIO::File * ptFile = (FileIO::File *)handle;
    delete (int32 *)userdata;
    if (ptFile)
    {
        delete ptFile;
        return FMOD_OK;
    }
    return FMOD_ERR_FILE_BAD;
}












static FMOD_RESULT sFileRead(void * handle, void * buffer, uint32 sizebytes, uint32 * bytesread, void * userdata)
{
    int32 nType;
    Console::IO::TaskPriorities ePriority;
    FileIO::File * ptFile = (FileIO::File *)handle;
    if (ptFile)
    {
        nType = *(int32 *)userdata;
        if (nType == 0)
            ePriority = Console::IO::TASKPRIORITY_MEDIUM;
        else if (nType == 2)
            ePriority = CGameLoadingScreenIsEnabled() ? Console::IO::TASKPRIORITY_HIGH : Console::IO::TASKPRIORITY_HIGHEST;
        else
            ePriority = Console::IO::TASKPRIORITY_HIGH;
        bool bRes = SFileReadFilePriority(ptFile->GetHSFILE(), buffer, sizebytes, bytesread, ePriority);
        if (bRes)
            return FMOD_OK;
        int32 nLastErrorCode = FileIO::GetLastErrorCode();
    }
    return FMOD_ERR_FILE_BAD;
}

































































static FMOD_RESULT sFileSeek(void * handle, uint32 pos, void * userdata)
{
    FileIO::File * ptFile = (FileIO::File *)handle;
    ptFile->SetFileCursor(pos, FileIO::FROM_BEGINNING);
    return FMOD_OK;
}










static void sReleaseChannel(PlayingSoundData * ptPlayingSoundData)
{

    FMOD_Channel_Stop(ptPlayingSoundData->ptFModChannel);







    for (int32 i = 0; i < DSP_NUM_TYPES; i++)
    {
        FModDSPEffect * ptDspEffect = &ptPlayingSoundData->arDspEffects[i];

        if (ptDspEffect->dwFlags & (1 << FMOD_DSP_FLAG_ENABLED))
        {
            if (ptDspEffect->ptFModDsp)
            {
                sReleaseFModDSP((DspType)i, ptDspEffect->ptFModDsp);
                ptDspEffect->ptFModDsp = NULL;
            }
            ptDspEffect->dwFlags &= ~(1 << FMOD_DSP_FLAG_ENABLED);
        }
    }



    if (ptPlayingSoundData->ptCachedSound)
    {
        sReleaseSound(ptPlayingSoundData->ptCachedSound, FALSE);
    }


    ptPlayingSoundData->Clear();
}



static void sFreeSound(SoundInstance * ptSound)
{


    if (sptAsyncAudio->IsPending(ptSound))
    {
        sptAsyncAudio->Cancel(ptSound);

        if ((ptSound->dwFlags & (1 << SOUNDINSTANCE_HAS_PLAYED_BIT)) && !(ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT)))
        {
            sRemoveSoundInstance(ptSound->tSoundParams.snoSound);
        }

        gptSounds->ElementDelete(ptSound);
        return;
    }




    for (int32 i = 0; i < 2; i++)
    {
        PlayingSoundData * ptPlayingSoundData = sGetPlayingSoundData(ptSound, i);
        if (ptPlayingSoundData)
            sReleaseChannel(ptPlayingSoundData);
    }




    if ((ptSound->dwFlags & (1 << SOUNDINSTANCE_HAS_PLAYED_BIT)) && !(ptSound->dwFlags & (1 << SOUNDINSTANCE_IS_MUSIC_BIT)))
    {
        sRemoveSoundInstance(ptSound->tSoundParams.snoSound);
    }

    gptSounds->ElementDelete(ptSound);
}

static PlayingSoundData * sGetPlayingSoundData(const SoundInstance * ptSound, int32 nWhichChannel)
{
    int32 nChannelIndex = ptSound->tChannelState[nWhichChannel].nChannelIndex;
    if (nChannelIndex < 0)
        return NULL;
    PlayingSoundData * ptPlayingSoundData = &sFModSoundGlobals.arPlayingSounds[nChannelIndex];
    if (ptPlayingSoundData->idSound == ptSound->id && ptPlayingSoundData)
        return ptPlayingSoundData;
    return NULL;
}















static BOOL sCheckResult(FMOD_RESULT result, LPCSTR szMessage, LPCSTR szParam)
{
    BOOL bOK = (result == FMOD_OK);
    if (!bOK)
    {
        sUpdateMemoryStats(FALSE);
    }
    return bOK;
}





















static BOOL sGetFileParameters(FMOD_CREATESOUNDEXINFO & tSoundExInfo, const SNO snoSoundBank, const uint32 dwSampleIndex, const uint32 dwSizeInBytes, CRefString & szFileNameOut)
{
    char szFileName[4096];




    tSoundExInfo.length = dwSizeInBytes;
    tSoundExInfo.fileoffset = 0;


    SoundBankGetSampleFullFilename(szFileName, sizeof(szFileName), snoSoundBank, dwSampleIndex);

    szFileNameOut = szFileName;
    return szFileName[0] != '\0';
}




FMOD_SOUND_FORMAT sGetFModSoundFormat(
    uint32,
    uint32,
    uint32 dwBitRate)
{
    FMOD_SOUND_FORMAT eFModSoundFormat = FMOD_SOUND_FORMAT_NONE;
    // switch inferred from the original's 6069:5 rows (switch keyword column); case bodies have only line-0 rows
    switch (dwBitRate)
    {
    case 8: eFModSoundFormat = FMOD_SOUND_FORMAT_PCM8; break;
    case 16: eFModSoundFormat = FMOD_SOUND_FORMAT_PCM16; break;
    case 24: eFModSoundFormat = FMOD_SOUND_FORMAT_PCM24; break;

    }
    return eFModSoundFormat;
}



// NON_MATCHING: 1380 vs 1452 bytes; our compiler tail-merges the two "sPath = CRefString(szHeader) + sPath; pbData = sPath.str();" sequences (6196, 6220) because FMOD_CREATESTREAM (6207) is ORed before the 6209 branch instead of being duplicated after it as in the original; rest follows the original
static FMOD_SOUND * sCreateFModSound(
    const SNO snoSoundBank,
    const uint32 nSampleIndex,
    const StreamMode eStreamMode,
    const BOOL fSuppressTracing,
    FMOD_SOUND ** ptParentFModSound,
    BOOL * pfPending,
    BOOL * pfIsFSB,
    BOOL fPreload)
{




    ScopedSoundBankDefinitionLockR ptSoundBank(snoSoundBank);

    if (nSampleIndex >= ptSoundBank->nSoundSampleCount)
        return NULL;



    SoundSample & tSoundSample = ptSoundBank->ptSoundSamples[nSampleIndex];

    BOOL fIsMono = SoundFormatIsMono(tSoundSample.GetFormat());
    BOOL f3D = fIsMono;

    uint32 dwChannels, dwFreq, dwBitRate;
    SoundFormatToCSB(tSoundSample.GetFormat(), dwChannels, dwFreq, dwBitRate);








    FMOD_CREATESOUNDEXINFO tSoundExInfo;
    SigmaMemoryClear(&tSoundExInfo, sizeof(tSoundExInfo));
    tSoundExInfo.cbsize = sizeof(tSoundExInfo);



    tSoundExInfo.fileuserdata = new int32;


    FMOD_SOUND * ptFModSound = NULL;
    FMOD_MODE fmodMode = FMOD_LOWMEM | FMOD_IGNORETAGS | (f3D ? (FMOD_3D | FMOD_3D_CUSTOMROLLOFF) : FMOD_2D);










    LPCSTR pbData = NULL;
    CRefString sPath;

    if (tSoundSample.GetCompression() != SOUNDCOMPRESSION_NONE)
    {
        if (tSoundSample.GetCompression() == SOUNDCOMPRESSION_VORBIS)
        {
            tSoundExInfo.suggestedsoundtype = FMOD_SOUND_TYPE_OGGVORBIS;
        }
        else if (tSoundSample.GetCompression() == SOUNDCOMPRESSION_ADPCM ||
                 tSoundSample.GetCompression() == SOUNDCOMPRESSION_MP3 ||
                 tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBXMA ||
                 tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBVORBIS ||
                 tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBAT9 ||
                 tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBFADPCM)
        {
            tSoundExInfo.suggestedsoundtype = FMOD_SOUND_TYPE_FSB;
            tSoundExInfo.defaultfrequency = dwFreq;
            tSoundExInfo.numchannels = dwChannels;
            fmodMode |= FMOD_CREATECOMPRESSEDSAMPLE;
        }
    }
    else
    {
        fmodMode |= FMOD_OPENRAW;
        tSoundExInfo.format = sGetFModSoundFormat(dwChannels, dwFreq, dwBitRate);
        tSoundExInfo.defaultfrequency = dwFreq;
        tSoundExInfo.numchannels = dwChannels;
    }

    if (eStreamMode == 0)
    {
        if (tSoundSample.GetCompression() == SOUNDCOMPRESSION_NONE)
        {
            fmodMode |= FMOD_CREATESAMPLE;
        }

        tSoundExInfo.defaultfrequency = dwFreq;
        tSoundExInfo.numchannels = dwChannels;
        if (!sGetFileParameters(tSoundExInfo, snoSoundBank, nSampleIndex, tSoundSample.dwSampleSize, sPath))
        {
            return NULL;
        }



        char szHeader[3] = " :";
        if (fPreload)
        {
            szHeader[0] = '0';
            tSoundExInfo.nonblockthreadid = 3;
        }
        else
        {
            szHeader[0] = '1';
            tSoundExInfo.nonblockthreadid = 2;
        }
        sPath = CRefString(szHeader) + sPath;

        pbData = sPath.str();
    }
    else
    {





        fmodMode |= FMOD_CREATESTREAM;

        if (eStreamMode == 1)
        {
            if (!sGetFileParameters(tSoundExInfo, snoSoundBank, nSampleIndex, tSoundSample.dwSampleSize, sPath))
            {
                return NULL;
            }


            char szHeader[3] = " :";
            szHeader[0] = '2';
            tSoundExInfo.nonblockthreadid = 1;
            sPath = CRefString(szHeader) + sPath;

            pbData = sPath.str();
        }
    }

    if (!fSuppressTracing && CGameGetDebugSoundMode() == DSM_FULL)
    {
    }




    fmodMode |= FMOD_NONBLOCKING;


    BOOL fMemCantFail = FALSE;
    static uint32 sdwAsyncThreadCreated = 0;
    if (sdwAsyncThreadCreated < 3)
    {
        tSoundExInfo.nonblockthreadid = ++sdwAsyncThreadCreated;
        fMemCantFail = TRUE;
    }




    FMOD_RESULT result;





    result = sCreateFModSoundCheckMemory(
            sFModSoundGlobals.ptFModSystem,
            pbData,
            fmodMode,
            &tSoundExInfo,
            &ptFModSound);


    if (fMemCantFail && result == FMOD_ERR_MEMORY)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    if (!sCheckResult(result, "FMOD_System_CreateSound", pbData))
    {
        return NULL;
    }


    *ptParentFModSound = NULL;

    BOOL fPending;
    BOOL fIsFSB = tSoundSample.GetCompression() == SOUNDCOMPRESSION_ADPCM ||
                  tSoundSample.GetCompression() == SOUNDCOMPRESSION_MP3 ||
                  tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBXMA ||
                  tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBVORBIS ||
                  tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBAT9 ||
                  tSoundSample.GetCompression() == SOUNDCOMPRESSION_FSBFADPCM;

    FMOD_OPENSTATE eOpenState;
    result = FMOD_Sound_GetOpenState(ptFModSound, &eOpenState, NULL, NULL, NULL);
    fPending = (result != FMOD_OK) || (eOpenState != FMOD_OPENSTATE_READY && eOpenState != FMOD_OPENSTATE_PLAYING);
    if (pfPending)
    {
        *pfPending = fPending;
    }
    if (pfIsFSB)
    {
        *pfIsFSB = fIsFSB;
    }



    if (fIsFSB && !fPending)
    {
        FMOD_SOUND * ptFModSubSound;
        result = FMOD_Sound_GetSubSound(ptFModSound, 0, &ptFModSubSound);
        sCheckResult(result, "FMOD_Sound_GetSubSound", "");
        *ptParentFModSound = ptFModSound;
        ptFModSound = ptFModSubSound;
    }

    return ptFModSound;
}




static FModCachedSound * sCreateSoundSample(
    const FModCachedSoundKey & tKey,
    const BOOL fSuppressTracing,
    BOOL fPreload)
{
    FMOD_RESULT result;
    FMOD_SOUND * ptFModParentSound = NULL;
    BOOL fPending;
    BOOL fIsFSB;
    FMOD_SOUND * ptFModSound = sCreateFModSound(tKey.snoSoundBank, tKey.nSampleIndex, 0, fSuppressTracing, &ptFModParentSound, &fPending, &fIsFSB, fPreload);
    FModCachedSound * ptCachedSound;

    if (!ptFModSound)
        return NULL;
    ptCachedSound = sFModSoundGlobals.ptCachedSoundAllocator->Alloc_Tracked();

    if (!ptCachedSound)
        return NULL;
    sFModSoundGlobals.nCacheSampleSize++;

    ptCachedSound->nRefCount = 1;
    ptCachedSound->ptFModSound = ptFModSound;
    ptCachedSound->ptFModParentSound = ptFModParentSound;
    ptCachedSound->eSoundType = FMOD_CACHED_SOUND_SAMPLE;
    ptCachedSound->ptNext = NULL;

    ptCachedSound->tLRUPos = sFModSoundGlobals.ptLRUSoundSampleList->AddTail_Tracked(tKey);

    ptCachedSound->fAlreadyReloaded = FALSE;
    ptCachedSound->fPreloading = FALSE;
    ptCachedSound->fPending = fPending;
    ptCachedSound->fIsFSB = fIsFSB;

    if (!fPending)
    {

        result = FMOD_Sound_SetUserData(ptFModSound, ptCachedSound);
        sCheckResult(result, "FMOD_Sound_SetUserData", "");
    }




    sFModSoundGlobals.ptCachedSoundSampleMap->SetAt_Tracked(tKey, ptCachedSound);
    return ptCachedSound;
}







static FModCachedSound * sCreateSoundStream(
    const FModCachedSoundKey & tKey,
    const BOOL fSuppressTracing)
{
    FMOD_RESULT result;
    FMOD_SOUND * ptFModParentSound = NULL;
    BOOL fPending;
    BOOL fIsFSB;
    FMOD_SOUND * ptFModSound = sCreateFModSound(tKey.snoSoundBank, tKey.nSampleIndex, 1, fSuppressTracing, &ptFModParentSound, &fPending, &fIsFSB, FALSE);
    FModCachedSound * ptCachedSound;

    if (!ptFModSound)
        return NULL;
    ptCachedSound = sFModSoundGlobals.ptCachedSoundAllocator->Alloc_Tracked();

    if (ptCachedSound)
    {
        sFModSoundGlobals.nCacheStreamSize++;

        ptCachedSound->nRefCount = 1;
        ptCachedSound->ptFModSound = ptFModSound;
        ptCachedSound->ptFModParentSound = ptFModParentSound;
        ptCachedSound->eSoundType = FMOD_CACHED_SOUND_STREAM;
        ptCachedSound->ptNext = NULL;

        ptCachedSound->fAlreadyReloaded = FALSE;
        ptCachedSound->fPreloading = FALSE;
        ptCachedSound->fPending = fPending;
        ptCachedSound->fIsFSB = fIsFSB;

        if (!fPending)
        {

            result = FMOD_Sound_SetUserData(ptFModSound, ptCachedSound);
            sCheckResult(result, "FMOD_Sound_SetUserData", "");
        }

        FModCachedSound * ptHead;

        if (sFModSoundGlobals.ptCachedSoundStreamMap->Lookup(tKey, ptHead))
        {
            ptCachedSound->ptNext = ptHead;
            ptCachedSound->tLRUPos = ptHead->tLRUPos;
            sFModSoundGlobals.ptLRUSoundStreamList->MoveToTail(ptCachedSound->tLRUPos);
        }
        else
        {
            ptCachedSound->tLRUPos = sFModSoundGlobals.ptLRUSoundStreamList->AddTail_Tracked(tKey);
        }


        sFModSoundGlobals.ptCachedSoundStreamMap->SetAt_Tracked(tKey, ptCachedSound);
    }
    return ptCachedSound;
}







static BOOL sPurgeSoundSample(const FModCachedSoundKey & tKey, const BOOL fSuppressTracing, const BOOL fForceRelease)
{
    BOOL bPurged = TRUE;
    FModCachedSound * ptCachedSound;
    if (sFModSoundGlobals.ptCachedSoundSampleMap->Lookup(tKey, ptCachedSound))
    {
        if (ptCachedSound->nRefCount == 0)
        {
            if (!fSuppressTracing && CGameGetDebugSoundMode() == DSM_FULL)
            {
            }
            sReleaseFModSound(ptCachedSound->ptFModParentSound ? ptCachedSound->ptFModParentSound : ptCachedSound->ptFModSound, fForceRelease);
            ptCachedSound->ptFModParentSound = NULL;
            ptCachedSound->ptFModSound = NULL;
            sFModSoundGlobals.ptCachedSoundSampleMap->RemoveAt(tKey);
            sFModSoundGlobals.ptLRUSoundSampleList->RemoveAt(ptCachedSound->tLRUPos);
            sFModSoundGlobals.ptCachedSoundAllocator->Free(ptCachedSound);
            sFModSoundGlobals.nCacheSampleSize--;
        }
        else
        {
            bPurged = FALSE;
        }
    }
    return bPurged;
}


















static BOOL sPurgeLowestPrioritySoundSample(const BOOL fSuppressTracing)
{
    BOOL bPurged = FALSE;
    POS pos = sFModSoundGlobals.ptLRUSoundSampleList->GetHeadPosition();
    while (pos)
    {
        FModCachedSoundKey & tKey = sFModSoundGlobals.ptLRUSoundSampleList->GetNext(pos);
        if (sPurgeSoundSample(tKey, fSuppressTracing, FALSE))
        {
            bPurged = TRUE;
            break;
        }
    }
    return bPurged;
}








static BOOL sPurgeSoundStream(const FModCachedSoundKey & tKey, const BOOL fForceRelease)
{
    BOOL bPurged = TRUE;
    FModCachedSound * ptCachedSound;
    if (sFModSoundGlobals.ptCachedSoundStreamMap->Lookup(tKey, ptCachedSound))
    {
        FModCachedSound * ptCachedSoundIter = ptCachedSound;
        while (ptCachedSoundIter)
        {
            if (ptCachedSoundIter->nRefCount > 0)
                return FALSE;
            ptCachedSoundIter = ptCachedSoundIter->ptNext;
        }
        sFModSoundGlobals.ptCachedSoundStreamMap->RemoveAt(tKey);
        sFModSoundGlobals.ptLRUSoundStreamList->RemoveAt(ptCachedSound->tLRUPos);
        ptCachedSoundIter = ptCachedSound;
        while (ptCachedSoundIter)
        {
            FModCachedSound * ptCachedSoundFree = ptCachedSoundIter;
            ptCachedSoundIter = ptCachedSoundIter->ptNext;
            if (CGameGetDebugSoundMode() == DSM_FULL)
            {
            }
            sReleaseFModSound(ptCachedSoundFree->ptFModParentSound ? ptCachedSoundFree->ptFModParentSound : ptCachedSoundFree->ptFModSound, fForceRelease);
            ptCachedSoundFree->ptFModParentSound = NULL;
            ptCachedSoundFree->ptFModSound = NULL;
            sFModSoundGlobals.ptCachedSoundAllocator->Free(ptCachedSoundFree);
            sFModSoundGlobals.nCacheStreamSize--;
        }
    }
    return bPurged;
}



























static BOOL sPurgeLowestPrioritySoundStream()
{
    BOOL bPurged = FALSE;
    POS pos = sFModSoundGlobals.ptLRUSoundStreamList->GetHeadPosition();
    while (pos)
    {
        FModCachedSoundKey & tKey = sFModSoundGlobals.ptLRUSoundStreamList->GetNext(pos);
        if (sPurgeSoundStream(tKey, FALSE))
        {
            bPurged = TRUE;
            break;
        }
    }
    return bPurged;
}









void sReleaseSound(
    FModCachedSound * ptCachedSound,
    BOOL fRemoveFromCache)
{
    if (!ptCachedSound)
        return;
    ptCachedSound->nRefCount--;


    if (ptCachedSound->nRefCount > 0)
        return;



    if (ptCachedSound->eSoundType == FMOD_CACHED_SOUND_STREAM)
    {

        FModCachedSoundKey & tKey = sFModSoundGlobals.ptLRUSoundStreamList->GetAt(ptCachedSound->tLRUPos);
        FModCachedSound * ptCachedSoundLookup;

        if (sFModSoundGlobals.ptCachedSoundStreamMap->Lookup(tKey, ptCachedSoundLookup))
        {
            int32 nNumZeroRefs = 0;
            FModCachedSound * ptCachedSoundIter = ptCachedSoundLookup;

            while (ptCachedSoundIter)
            {
                if (ptCachedSoundIter->nRefCount == 0)
                {
                    nNumZeroRefs++;
                }
                ptCachedSoundIter = ptCachedSoundIter->ptNext;
            }
            // inferred from 6611 (cset lt on nNumZeroRefs < 1, cset eq on fRemoveFromCache, and, tbnz to the epilogue)
            if (nNumZeroRefs > 0 || fRemoveFromCache)
            {

                FModCachedSound * ptPrev = NULL;
                ptCachedSoundIter = ptCachedSoundLookup;

                while (ptCachedSoundIter)
                {
                    FModCachedSound * ptCachedSoundFree = ptCachedSoundIter;
                    ptCachedSoundIter = ptCachedSoundIter->ptNext;

                    if (ptCachedSoundFree->nRefCount == 0)
                    {
                        if (ptPrev == NULL)
                        {
                            if (ptCachedSoundIter)
                            {
                                sFModSoundGlobals.ptCachedSoundStreamMap->SetAt_Tracked(tKey, ptCachedSoundIter);
                            }
                            else
                            {
                                sFModSoundGlobals.ptCachedSoundStreamMap->RemoveAt(tKey);
                                sFModSoundGlobals.ptLRUSoundStreamList->RemoveAt(ptCachedSound->tLRUPos);
                            }
                        }
                        else
                        {

                            ptPrev->ptNext = ptCachedSoundIter;
                        }


                        sReleaseFModSound(ptCachedSoundFree->ptFModParentSound ? ptCachedSoundFree->ptFModParentSound : ptCachedSoundFree->ptFModSound, FALSE);
                        ptCachedSoundFree->ptFModSound = NULL; ptCachedSoundFree->ptFModParentSound = NULL;


                        sFModSoundGlobals.ptCachedSoundAllocator->Free(ptCachedSoundFree);
                        sFModSoundGlobals.nCacheStreamSize--;
                        // inferred: the original leaves the loop right after freeing one entry (b to the epilogue at +0x274)
                        break;
                    }
                    ptPrev = ptCachedSoundFree;
                }
            }
        }
        else
        {



































            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
    else if (fRemoveFromCache)
    {
        FModCachedSoundKey & tKey = sFModSoundGlobals.ptLRUSoundSampleList->GetAt(ptCachedSound->tLRUPos);
        sPurgeSoundSample(tKey, FALSE, FALSE);
    }

}






static FModCachedSound * sGetSound(
    const SNO snoSoundBank,
    const int32 nSampleIndex,
    const CachedSoundType eSoundType,
    const BOOL fSuppressTracing,
    BOOL fPreload)
{
    if (!SNOFileExists(SOUNDBANK_SNO, snoSoundBank))
        return NULL;



    FModCachedSound * ptCachedSound;
    // Inference: only one tKey store has a line row (6690:23, either member fits); kept snoSoundBank first, nSampleIndex on 6691.
    FModCachedSoundKey tKey;
    tKey.snoSoundBank = snoSoundBank;
    tKey.nSampleIndex = (uint16)nSampleIndex;

    if (sFModSoundGlobals.ptCachedSoundAllocator->GetFreeBlockCount() == 0)
    {
        sPurgeLowestPrioritySoundSample(fSuppressTracing);

        if (sFModSoundGlobals.ptCachedSoundAllocator->GetFreeBlockCount() == 0)
        {
            BOOL bPurged = sPurgeLowestPrioritySoundStream();

            if (!bPurged)
            {

                sDumpSoundPlatformFModGlobals();
            }
        }
    }

    if (eSoundType == FMOD_CACHED_SOUND_SAMPLE)
    {
        FModCachedSoundSampleMap * ptSoundMap = sFModSoundGlobals.ptCachedSoundSampleMap;
        if (!ptSoundMap->Lookup(tKey, ptCachedSound))
        {
            if (sFModSoundGlobals.nCacheSampleSize >= 2048)
            {
                sPurgeLowestPrioritySoundSample(fSuppressTracing);
            }

            if (sFModSoundGlobals.nCacheSampleSize >= 2048 ||
                sFModSoundGlobals.ptCachedSoundAllocator->GetFreeBlockCount() <= 0)
            {
                return NULL;
            }

            ptCachedSound = sCreateSoundSample(tKey, fSuppressTracing, fPreload);
        }
        else
        {




            ptCachedSound->nRefCount++;

            sFModSoundGlobals.ptLRUSoundSampleList->MoveToTail(ptCachedSound->tLRUPos);
        }
    }
    else
    {
        FModCachedSoundStreamMap * ptSoundMap = sFModSoundGlobals.ptCachedSoundStreamMap;
        BOOL bSoundInMap = ptSoundMap->Lookup(tKey, ptCachedSound);

        if (bSoundInMap)
        {
            for (; ptCachedSound != NULL && ptCachedSound->nRefCount > 0; ptCachedSound = ptCachedSound->ptNext)
            {
            }
        }
        // Inference: the guard shape is not visible (both not-found exits branch straight to 6753); this form keeps ptCachedSound defined.
        if (!bSoundInMap || ptCachedSound == NULL)
        {

            if (sFModSoundGlobals.nCacheStreamSize >= 128 ||
                (!bSoundInMap && sFModSoundGlobals.ptCachedSoundStreamMap->Count() == 32))
            {
                BOOL bPurged = sPurgeLowestPrioritySoundStream();

                if (!bPurged)
                {

                    sDumpFModCachedSoundStreamMap();
                    sDumpSoundPlatformFModGlobals();
                }
            }

            if (sFModSoundGlobals.nCacheStreamSize >= 128 ||
                sFModSoundGlobals.ptCachedSoundAllocator->GetFreeBlockCount() <= 0 ||
                (!bSoundInMap && sFModSoundGlobals.ptCachedSoundStreamMap->Count() >= 32))
            {
                return NULL;
            }

            ptCachedSound = sCreateSoundStream(tKey, fSuppressTracing);
        }
        else
        {




            ptCachedSound->nRefCount++;

            sFModSoundGlobals.ptLRUSoundStreamList->MoveToTail(ptCachedSound->tLRUPos);
        }
    }

    return ptCachedSound;
}











