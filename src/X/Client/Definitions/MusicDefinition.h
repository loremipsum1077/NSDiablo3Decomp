// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/MusicDefinition.h
// Subset: the definition structs, the group definition class and its global.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

class MusicGroupDef : public GroupDefinition {  // size 0xa0
public:
    MusicGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SOUND; }  // vtable slot 7
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
};
static_assert(sizeof(MusicGroupDef) == 0xa0, "MusicGroupDef");

extern MusicGroupDef * gptGroupDefMusic;  // MusicDefinition.cpp

// X/Client/Definitions/MusicDefinition.h (DWARF layout); used by the sound platform.
typedef int32 FadeShape;  // SoundDefinition.h
struct PlaylistEntry {  // size 0x4c
    uint32 GetSamplesBeforeStart(const uint32 dwFreq) const;
    uint32 GetBaseDurationSamples(const uint32 dwStartSample) const;
    uint32 GetSamplesAfterEnd(const uint32 dwFreq) const;
    SNO snoSoundBank;  // 0x00
    uint32 dwSampleHash;  // 0x04
    float flVolumeDB;  // 0x08
    uint32 dwStartPosMin;  // 0x0c
    uint32 dwStartPosDelta;  // 0x10
    uint32 dwEndPos;  // 0x14
    uint32 dwLoopCount;  // 0x18
    uint32 dwLoopStartPos;  // 0x1c
    uint32 dwLoopEndPos;  // 0x20
    uint32 dwFadeInOffset;  // 0x24
    uint32 dwFadeInTime;  // 0x28
    uint32 dwFadeOutOffset;  // 0x2c
    uint32 dwFadeOutTime;  // 0x30
    FadeShape eFadeInShape;  // 0x34
    FadeShape eFadeOutShape;  // 0x38
    float flFadeInAccel;  // 0x3c
    float flFadeOutAccel;  // 0x40
    int32 nSampleIndex;  // 0x44
    SNO snoValidatedSoundBank;  // 0x48
};
static_assert(sizeof(PlaylistEntry) == 0x4c, "PlaylistEntry");

#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"

enum {
    MUSIC_LOOPS_BIT = 0,
    MUSIC_SHUFFLE_BIT = 1,
    MUSIC_DONT_DUCK_BIT = 2,
};

struct RandomMusicSoundParams {  // size 0x10
    SNO snoSound;  // 0x00
    uint32 dwPeriodMin;  // 0x04
    uint32 dwPeriodDelta;  // 0x08
    uint32 dwOffset;  // 0x0c
};
static_assert(sizeof(RandomMusicSoundParams) == 0x10, "RandomMusicSoundParams");

struct MusicDefinition {  // size 0x50
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    float flGlobalVolumeDB;  // 0x10
    uint32 dwDelayBetweenEntriesMin;  // 0x14
    uint32 dwDelayBetweenEntriesDelta;  // 0x18
    SerializeData serPlaylistEntries;  // 0x1c
    int32 nPlaylistEntryCount;  // 0x24
    XPtr64<PlaylistEntry> ptPlaylistEntries;  // 0x28
    RandomMusicSoundParams tRandomSounds[2];  // 0x30
};
static_assert(sizeof(MusicDefinition) == 0x50, "MusicDefinition");

typedef ScopedDefinitionLockR<MusicDefinition> ScopedMusicDefinitionLockR;

template <> inline const MusicDefinition * DefinitionLockR<MusicDefinition>(SNO sno) {  // line 170
    return (const MusicDefinition *)gptGroupDefMusic->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<MusicDefinition>(const MusicDefinition * pdef) {  // line 171
    void * lpDef = (void *)pdef;
    gptGroupDefMusic->UnlockR(lpDef);
}
