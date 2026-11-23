// FMOD low level C API (FMOD Studio 1.10.05, lowlevel_api/inc/fmod.h).
// Only the entry points the client calls are declared; the prototypes are FMOD's public C API.
// The types come from the DWARF-adopted fmod_common.h / fmod_dsp.h / fmod_dsp_effects.h.

#pragma once

#include "fmod_common.h"
#include "fmod_dsp.h"
#include "fmod_dsp_effects.h"

typedef unsigned int FMOD_INITFLAGS;

#define FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED 0x00000001
#define FMOD_SYSTEM_CALLBACK_DEVICELOST 0x00000002
#define FMOD_MEMORY_PERSISTENT 0x00100000
#define FMOD_TIMEUNIT_MS 0x00000001
#define FMOD_TIMEUNIT_PCM 0x00000002
#define FMOD_TIMEUNIT_PCMBYTES 0x00000004
#define FMOD_TIMEUNIT_RAWBYTES 0x00000008
#define FMOD_SYSTEM_CALLBACK_ALL 0xFFFFFFFF
#define FMOD_INIT_NORMAL 0x00000000
#define FMOD_VERSION 0x00011005
#define FMOD_DEFAULT 0x00000000
#define FMOD_LOOP_OFF 0x00000001
#define FMOD_LOOP_NORMAL 0x00000002
#define FMOD_LOOP_BIDI 0x00000004
#define FMOD_2D 0x00000008
#define FMOD_3D 0x00000010
#define FMOD_CREATESTREAM 0x00000080
#define FMOD_CREATESAMPLE 0x00000100
#define FMOD_CREATECOMPRESSEDSAMPLE 0x00000200
#define FMOD_OPENUSER 0x00000400
#define FMOD_OPENMEMORY 0x00000800
#define FMOD_OPENMEMORY_POINT 0x10000000
#define FMOD_OPENRAW 0x00001000
#define FMOD_OPENONLY 0x00002000
#define FMOD_ACCURATETIME 0x00004000
#define FMOD_MPEGSEARCH 0x00008000
#define FMOD_NONBLOCKING 0x00010000
#define FMOD_UNIQUE 0x00020000
#define FMOD_3D_HEADRELATIVE 0x00040000
#define FMOD_3D_WORLDRELATIVE 0x00080000
#define FMOD_3D_INVERSEROLLOFF 0x00100000
#define FMOD_3D_LINEARROLLOFF 0x00200000
#define FMOD_3D_LINEARSQUAREROLLOFF 0x00400000
#define FMOD_3D_INVERSETAPEREDROLLOFF 0x00800000
#define FMOD_3D_CUSTOMROLLOFF 0x04000000
#define FMOD_3D_IGNOREGEOMETRY 0x40000000
#define FMOD_IGNORETAGS 0x02000000
#define FMOD_LOWMEM 0x08000000
#define FMOD_LOADSECONDARYRAM 0x20000000
#define FMOD_VIRTUAL_PLAYFROMSTART 0x80000000
#define FMOD_REVERB_MAXINSTANCES 4

/*                                    Decay   EarlyDly LateDly HFRef   HFDecay Diffus  Densty  LoFreq  LoGain  HiCut   EarlyLate WetLvl */
#define FMOD_PRESET_OFF              {  1000,    7,  11, 5000, 100, 100, 100, 250, 0,    20,  96, -80.0f }
#define FMOD_PRESET_GENERIC          {  1500,    7,  11, 5000,  83, 100, 100, 250, 0, 14500,  96,  -8.0f }
#define FMOD_PRESET_PADDEDCELL       {   170,    1,   2, 5000,  10, 100, 100, 250, 0,   160,  84,  -7.8f }
#define FMOD_PRESET_ROOM             {   400,    2,   3, 5000,  83, 100, 100, 250, 0,  6050,  88,  -9.4f }
#define FMOD_PRESET_BATHROOM         {  1500,    7,  11, 5000,  54, 100,  60, 250, 0,  2900,  83,   0.5f }
#define FMOD_PRESET_LIVINGROOM       {   500,    3,   4, 5000,  10, 100, 100, 250, 0,   160,  58, -19.0f }
#define FMOD_PRESET_STONEROOM        {  2300,   12,  17, 5000,  64, 100, 100, 250, 0,  7800,  71,  -8.5f }
#define FMOD_PRESET_AUDITORIUM       {  4300,   20,  30, 5000,  59, 100, 100, 250, 0,  5850,  64, -11.7f }
#define FMOD_PRESET_CONCERTHALL      {  3900,   20,  29, 5000,  70, 100, 100, 250, 0,  5650,  80,  -9.8f }
#define FMOD_PRESET_CAVE             {  2900,   15,  22, 5000, 100, 100, 100, 250, 0, 20000,  59, -11.3f }
#define FMOD_PRESET_ARENA            {  7200,   20,  30, 5000,  33, 100, 100, 250, 0,  4500,  80,  -9.6f }
#define FMOD_PRESET_HANGAR           { 10000,   20,  30, 5000,  23, 100, 100, 250, 0,  3400,  72,  -7.4f }
#define FMOD_PRESET_CARPETTEDHALLWAY {   300,    2,  30, 5000,  10, 100, 100, 250, 0,   500,  56, -24.0f }
#define FMOD_PRESET_HALLWAY          {  1500,    7,  11, 5000,  59, 100, 100, 250, 0,  7800,  87,  -5.5f }
#define FMOD_PRESET_STONECORRIDOR    {   270,   13,  20, 5000,  79, 100, 100, 250, 0,  9000,  86,  -6.0f }
#define FMOD_PRESET_ALLEY            {  1500,    7,  11, 5000,  86, 100, 100, 250, 0,  8300,  80,  -9.8f }
#define FMOD_PRESET_FOREST           {  1500,  162,  88, 5000,  54,  79, 100, 250, 0,   760,  94, -12.3f }
#define FMOD_PRESET_CITY             {  1500,    7,  11, 5000,  67,  50, 100, 250, 0,  4050,  66, -26.0f }
#define FMOD_PRESET_MOUNTAINS        {  1500,  300, 100, 5000,  21,  27, 100, 250, 0,  1220,  82, -24.0f }
#define FMOD_PRESET_QUARRY           {  1500,   61,  25, 5000,  83, 100, 100, 250, 0,  3400, 100,  -5.0f }
#define FMOD_PRESET_PLAIN            {  1500,  179, 100, 5000,  50,  21, 100, 250, 0,  1670,  65, -28.0f }
#define FMOD_PRESET_PARKINGLOT       {  1700,    8,  12, 5000, 100, 100, 100, 250, 0, 20000,  56, -19.5f }
#define FMOD_PRESET_SEWERPIPE        {  2800,   14,  21, 5000,  14,  80,  60, 250, 0,  3400,  66,   1.2f }
#define FMOD_PRESET_UNDERWATER       {  1500,    7,  11, 5000,  10, 100, 100, 250, 0,   500,  92,   7.0f }
typedef int FMOD_OUTPUTTYPE;

typedef void * (*FMOD_MEMORY_ALLOC_CALLBACK)(unsigned int size, FMOD_MEMORY_TYPE type, const char * sourcestr);
typedef void * (*FMOD_MEMORY_REALLOC_CALLBACK)(void * ptr, unsigned int size, FMOD_MEMORY_TYPE type, const char * sourcestr);
typedef void (*FMOD_MEMORY_FREE_CALLBACK)(void * ptr, FMOD_MEMORY_TYPE type, const char * sourcestr);
typedef FMOD_RESULT (*FMOD_SYSTEM_CALLBACK)(FMOD_SYSTEM * system, FMOD_SYSTEM_CALLBACK_TYPE type, void * commanddata1, void * commanddata2, void * userdata);
typedef float (*FMOD_3D_ROLLOFF_CALLBACK)(FMOD_CHANNELCONTROL * channelcontrol, float distance);

extern "C" {
FMOD_RESULT FMOD_Memory_Initialize(void * poolmem, int poollen, FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree, FMOD_MEMORY_TYPE memtypeflags);
FMOD_RESULT FMOD_Memory_GetStats(int * currentalloced, int * maxalloced, FMOD_BOOL blocking);

FMOD_RESULT FMOD_System_Create(FMOD_SYSTEM ** system);
FMOD_RESULT FMOD_System_Release(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_SetFileSystem(FMOD_SYSTEM * system, FMOD_FILE_OPEN_CALLBACK useropen, FMOD_FILE_CLOSE_CALLBACK userclose, FMOD_FILE_READ_CALLBACK userread, FMOD_FILE_SEEK_CALLBACK userseek, FMOD_FILE_ASYNCREAD_CALLBACK userasyncread, FMOD_FILE_ASYNCCANCEL_CALLBACK userasynccancel, int blockalign);
FMOD_RESULT FMOD_System_SetOutput(FMOD_SYSTEM * system, FMOD_OUTPUTTYPE output);
FMOD_RESULT FMOD_System_GetNumDrivers(FMOD_SYSTEM * system, int * numdrivers);
FMOD_RESULT FMOD_System_SetDriver(FMOD_SYSTEM * system, int driver);
FMOD_RESULT FMOD_System_SetSoftwareChannels(FMOD_SYSTEM * system, int numsoftwarechannels);
FMOD_RESULT FMOD_System_SetSoftwareFormat(FMOD_SYSTEM * system, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers);
FMOD_RESULT FMOD_System_GetSoftwareFormat(FMOD_SYSTEM * system, int * samplerate, FMOD_SPEAKERMODE * speakermode, int * numrawspeakers);
FMOD_RESULT FMOD_System_SetStreamBufferSize(FMOD_SYSTEM * system, unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
FMOD_RESULT FMOD_System_SetCallback(FMOD_SYSTEM * system, FMOD_SYSTEM_CALLBACK callback, FMOD_SYSTEM_CALLBACK_TYPE callbackmask);
FMOD_RESULT FMOD_System_Init(FMOD_SYSTEM * system, int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata);
FMOD_RESULT FMOD_System_Update(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_Set3DSettings(FMOD_SYSTEM * system, float dopplerscale, float distancefactor, float rolloffscale);
FMOD_RESULT FMOD_System_Set3DListenerAttributes(FMOD_SYSTEM * system, int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
FMOD_RESULT FMOD_System_Set3DRolloffCallback(FMOD_SYSTEM * system, FMOD_3D_ROLLOFF_CALLBACK callback);
FMOD_RESULT FMOD_System_GetVersion(FMOD_SYSTEM * system, unsigned int * version);
FMOD_RESULT FMOD_System_CreateSound(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND ** sound);
FMOD_RESULT FMOD_System_CreateStream(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND ** sound);
FMOD_RESULT FMOD_System_CreateDSP(FMOD_SYSTEM * system, const FMOD_DSP_DESCRIPTION * description, FMOD_DSP ** dsp);
FMOD_RESULT FMOD_System_CreateDSPByType(FMOD_SYSTEM * system, FMOD_DSP_TYPE type, FMOD_DSP ** dsp);
FMOD_RESULT FMOD_System_CreateChannelGroup(FMOD_SYSTEM * system, const char * name, FMOD_CHANNELGROUP ** channelgroup);
FMOD_RESULT FMOD_System_PlaySound(FMOD_SYSTEM * system, FMOD_SOUND * sound, FMOD_CHANNELGROUP * channelgroup, FMOD_BOOL paused, FMOD_CHANNEL ** channel);
FMOD_RESULT FMOD_System_GetChannel(FMOD_SYSTEM * system, int channelid, FMOD_CHANNEL ** channel);
FMOD_RESULT FMOD_System_GetMasterChannelGroup(FMOD_SYSTEM * system, FMOD_CHANNELGROUP ** channelgroup);
FMOD_RESULT FMOD_System_SetReverbProperties(FMOD_SYSTEM * system, int instance, const FMOD_REVERB_PROPERTIES * prop);
FMOD_RESULT FMOD_System_GetReverbProperties(FMOD_SYSTEM * system, int instance, FMOD_REVERB_PROPERTIES * prop);

FMOD_RESULT FMOD_Sound_Release(FMOD_SOUND * sound);
FMOD_RESULT FMOD_Sound_SetDefaults(FMOD_SOUND * sound, float frequency, int priority);
FMOD_RESULT FMOD_Sound_GetDefaults(FMOD_SOUND * sound, float * frequency, int * priority);
FMOD_RESULT FMOD_Sound_GetSubSound(FMOD_SOUND * sound, int index, FMOD_SOUND ** subsound);
FMOD_RESULT FMOD_Sound_GetOpenState(FMOD_SOUND * sound, FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, FMOD_BOOL * starving, FMOD_BOOL * diskbusy);
FMOD_RESULT FMOD_Sound_SetMode(FMOD_SOUND * sound, FMOD_MODE mode);
FMOD_RESULT FMOD_Sound_SetLoopCount(FMOD_SOUND * sound, int loopcount);
FMOD_RESULT FMOD_Sound_SetUserData(FMOD_SOUND * sound, void * userdata);
FMOD_RESULT FMOD_Sound_GetUserData(FMOD_SOUND * sound, void ** userdata);
FMOD_RESULT FMOD_Sound_GetLength(FMOD_SOUND * sound, unsigned int * length, FMOD_TIMEUNIT lengthtype);
FMOD_RESULT FMOD_Channel_Stop(FMOD_CHANNEL * channel);
FMOD_RESULT FMOD_Channel_SetPaused(FMOD_CHANNEL * channel, FMOD_BOOL paused);
FMOD_RESULT FMOD_Channel_SetVolume(FMOD_CHANNEL * channel, float volume);
FMOD_RESULT FMOD_Channel_GetVolume(FMOD_CHANNEL * channel, float * volume);
FMOD_RESULT FMOD_Channel_SetFrequency(FMOD_CHANNEL * channel, float frequency);
FMOD_RESULT FMOD_Channel_GetFrequency(FMOD_CHANNEL * channel, float * frequency);
FMOD_RESULT FMOD_Channel_SetReverbProperties(FMOD_CHANNEL * channel, int instance, float wet);
FMOD_RESULT FMOD_Channel_SetDelay(FMOD_CHANNEL * channel, unsigned long long dspclock_start, unsigned long long dspclock_end, FMOD_BOOL stopchannels);
FMOD_RESULT FMOD_Channel_GetDSPClock(FMOD_CHANNEL * channel, unsigned long long * dspclock, unsigned long long * parentclock);
FMOD_RESULT FMOD_Channel_AddDSP(FMOD_CHANNEL * channel, int index, FMOD_DSP * dsp);
FMOD_RESULT FMOD_Channel_SetMode(FMOD_CHANNEL * channel, FMOD_MODE mode);
FMOD_RESULT FMOD_Channel_Set3DAttributes(FMOD_CHANNEL * channel, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * alt_pan_pos);
FMOD_RESULT FMOD_Channel_Get3DAttributes(FMOD_CHANNEL * channel, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * alt_pan_pos);
FMOD_RESULT FMOD_Channel_Set3DMinMaxDistance(FMOD_CHANNEL * channel, float mindistance, float maxdistance);
FMOD_RESULT FMOD_Channel_Set3DOcclusion(FMOD_CHANNEL * channel, float directocclusion, float reverbocclusion);
FMOD_RESULT FMOD_Channel_Set3DSpread(FMOD_CHANNEL * channel, float angle);
FMOD_RESULT FMOD_Channel_Set3DLevel(FMOD_CHANNEL * channel, float level);
FMOD_RESULT FMOD_Channel_Set3DDopplerLevel(FMOD_CHANNEL * channel, float level);
FMOD_RESULT FMOD_Channel_SetUserData(FMOD_CHANNEL * channel, void * userdata);
FMOD_RESULT FMOD_Channel_GetUserData(FMOD_CHANNEL * channel, void ** userdata);
FMOD_RESULT FMOD_Channel_SetChannelGroup(FMOD_CHANNEL * channel, FMOD_CHANNELGROUP * channelgroup);
FMOD_RESULT FMOD_Channel_SetPosition(FMOD_CHANNEL * channel, unsigned int position, FMOD_TIMEUNIT postype);
FMOD_RESULT FMOD_Channel_GetPosition(FMOD_CHANNEL * channel, unsigned int * position, FMOD_TIMEUNIT postype);
FMOD_RESULT FMOD_Channel_SetLoopCount(FMOD_CHANNEL * channel, int loopcount);
FMOD_RESULT FMOD_Channel_GetIndex(FMOD_CHANNEL * channel, int * index);
FMOD_RESULT FMOD_Channel_IsPlaying(FMOD_CHANNEL * channel, FMOD_BOOL * isplaying);

FMOD_RESULT FMOD_ChannelGroup_SetVolume(FMOD_CHANNELGROUP * channelgroup, float volume);
FMOD_RESULT FMOD_ChannelGroup_AddDSP(FMOD_CHANNELGROUP * channelgroup, int index, FMOD_DSP * dsp);

FMOD_RESULT FMOD_DSP_Release(FMOD_DSP * dsp);
FMOD_RESULT FMOD_DSP_SetBypass(FMOD_DSP * dsp, FMOD_BOOL bypass);
FMOD_RESULT FMOD_DSP_SetParameterFloat(FMOD_DSP * dsp, int index, float value);

FMOD_RESULT FMOD_Channel_SetLoopPoints(FMOD_CHANNEL * channel, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
FMOD_RESULT FMOD_Channel_SetPriority(FMOD_CHANNEL * channel, int priority);
FMOD_RESULT FMOD_Channel_SetMixLevelsOutput(FMOD_CHANNEL * channel, float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright);
FMOD_RESULT FMOD_Channel_RemoveDSP(FMOD_CHANNEL * channel, FMOD_DSP * dsp);
FMOD_RESULT FMOD_DSP_SetParameterInt(FMOD_DSP * dsp, int index, int value);
FMOD_RESULT FMOD_DSP_SetParameterBool(FMOD_DSP * dsp, int index, FMOD_BOOL value);
}
