// Original file: X/Client/Definitions/MusicDefinition.cpp

#include "MusicDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs
// in this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "SoundBankDefinition.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

enum {
    MUSIC_COMMON_SNO_VERSION = 18,
    MUSIC_PC_SNO_VERSION = 18,
    MUSIC_CONSOLE_SNO_VERSION = 21,
    MUSIC_AXE_SNO_VERSION = 20,
    MUSIC_XML_VERSION = 1,
    PLAYLIST_ENTRY_XML_VERSION = 1,
};

static const int32 scnDefaultFlags = 1;  // line 41
static const float scflDefaultGlobalVolumeDB = -3.0f;  // line 42

static const TypeDescriptorField RandomMusicSoundParams_Fields[5] = {  // line 51
    FilterTypeDescriptorField("", DT_SNO, offsetof(RandomMusicSoundParams, snoSound), "", SOUND_SNO, 0, -1),
    TypeDescriptorField("", DT_TIME, offsetof(RandomMusicSoundParams, dwPeriodMin), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RandomMusicSoundParams, dwPeriodDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(RandomMusicSoundParams, dwOffset), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RandomMusicSoundParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RandomMusicSoundParams("RandomMusicSoundParams", 0, RandomMusicSoundParams_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 56
const XTypeDescriptor * DT_RANDOM_MUSIC_SOUND_PARAMS = &TypeDescriptor_RandomMusicSoundParams;

static const TypeDescriptorField PlaylistEntry_Fields[18] = {  // line 58
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlaylistEntry, snoSoundBank), "", SOUNDBANK_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwSampleHash), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(PlaylistEntry, flVolumeDB), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwStartPosMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwStartPosDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwEndPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwLoopCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwLoopStartPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwLoopEndPos), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwFadeInOffset), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwFadeInTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwFadeOutOffset), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, dwFadeOutTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, eFadeInShape), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlaylistEntry, eFadeOutShape), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PlaylistEntry, flFadeInAccel), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PlaylistEntry, flFadeOutAccel), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlaylistEntry), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlaylistEntry("PlaylistEntry", PLAYLIST_ENTRY_XML_VERSION, PlaylistEntry_Fields, 18, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 81
const XTypeDescriptor * DT_PLAYLIST_ENTRY = &TypeDescriptor_PlaylistEntry;

static const TypeDescriptorField MusicDefinition_Fields[9] = {  // line 83
    TypeDescriptorField("", DT_INT, offsetof(MusicDefinition, dwFlags), (const void *)scnDefaultFlags, 0x80001),
    TypeDescriptorField("", DT_FLOAT, offsetof(MusicDefinition, flGlobalVolumeDB), (const void *)0xc0400000, 1),
    TypeDescriptorField("", DT_TIME, offsetof(MusicDefinition, dwDelayBetweenEntriesMin), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(MusicDefinition, dwDelayBetweenEntriesDelta), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MusicDefinition, serPlaylistEntries), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_PLAYLIST_ENTRY, offsetof(MusicDefinition, ptPlaylistEntries), 0x21,
                                     (int32)offsetof(MusicDefinition, serPlaylistEntries) - (int32)offsetof(MusicDefinition, ptPlaylistEntries)),
    VariableArrayCountTypeDescriptorField("", offsetof(MusicDefinition, nPlaylistEntryCount), 0x10000, DT_PLAYLIST_ENTRY,
                                          (int32)offsetof(MusicDefinition, serPlaylistEntries) - (int32)offsetof(MusicDefinition, nPlaylistEntryCount)),
    FixedArrayTypeDescriptorField("", DT_RANDOM_MUSIC_SOUND_PARAMS, offsetof(MusicDefinition, tRandomSounds), NULL, 1, 2, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MusicDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MusicDefinition("Music", MUSIC_XML_VERSION, MusicDefinition_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 91
const XTypeDescriptor * DT_MUSIC = &TypeDescriptor_MusicDefinition;

MusicGroupDef * gptGroupDefMusic;  // line 97

// line 103
MusicGroupDef::MusicGroupDef()
    : GroupDefinition("Music", MUSIC_SNO,
                      SNOVersions(MUSIC_PC_SNO_VERSION,
                                  MUSIC_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  MUSIC_CONSOLE_SNO_VERSION,
                                  MUSIC_CONSOLE_SNO_VERSION,
                                  MUSIC_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(MusicDefinition), DT_MUSIC, 1, 0x140a) {
}

// line 126
uint32 MusicGroupDef::GetMaxCount() {
    return 16;
}

// line 134
void MusicGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(MusicDefinition));

    MusicDefinition * pdef = (MusicDefinition *)ptDefaultDef;
    pdef->dwFlags = scnDefaultFlags;
    pdef->flGlobalVolumeDB = scflDefaultGlobalVolumeDB;

    for (int32 i = 0; i < 2; i++) {
        RandomMusicSoundParams & tParams = pdef->tRandomSounds[i];
        tParams.snoSound = SNO_NONE;
        tParams.dwPeriodMin = 180;
        tParams.dwPeriodDelta = 120;
        tParams.dwOffset = 0;
    }
}

// line 156
void MusicGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    MusicDefinition * pdef = (MusicDefinition *)lpDefinition;
    for (int32 i = 0; i < pdef->nPlaylistEntryCount; i++) {
        PlaylistEntry & tEntry = pdef->ptPlaylistEntries[i];
        tEntry.nSampleIndex = SoundBankSampleIndexFromHash(tEntry.snoSoundBank, tEntry.dwSampleHash);
        if (tEntry.nSampleIndex == -1) {
            tEntry.snoValidatedSoundBank = gptGroupDefSoundBank->GetFallbackDataSNO();
            tEntry.nSampleIndex = 0;
        } else {
            tEntry.snoValidatedSoundBank = tEntry.snoSoundBank;
        }
    }
}

// line 199
void MusicGroupDef::Delete(void * lpDefinition) const {
    GroupDefinition::Delete(lpDefinition);
}

// line 402
uint32 PlaylistEntry::GetSamplesBeforeStart(const uint32 dwFreq) const {
    uint32 dwSamplesBeforeStart = dwFadeInTime * dwFreq / 60 - dwFadeInOffset;
    return dwSamplesBeforeStart;
}

// line 412
uint32 PlaylistEntry::GetBaseDurationSamples(const uint32 dwStartSample) const {
    uint32 dwDurationInSamples = dwEndPos - dwStartSample;
    if ((dwLoopStartPos != 0 || dwLoopEndPos != 0) && dwEndPos > dwLoopEndPos)
        dwDurationInSamples += dwLoopCount * (dwLoopEndPos - dwLoopStartPos);
    return dwDurationInSamples;
}

// line 431
uint32 PlaylistEntry::GetSamplesAfterEnd(const uint32 dwFreq) const {
    uint32 dwSamplesAfterEnd = 0;
    if (dwFadeOutTime != 0)
        dwSamplesAfterEnd = dwFadeOutTime * dwFreq / 60 + dwFadeOutOffset;
    return dwSamplesAfterEnd;
}
