// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/SoundBankDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;

enum {
    SOUNDCOMPRESSION_NONE = 0,
    SOUNDCOMPRESSION_ADPCM = 1,
    SOUNDCOMPRESSION_MP3 = 2,
    SOUNDCOMPRESSION_VORBIS = 3,
    SOUNDCOMPRESSION_FSBVORBIS = 4,
    SOUNDCOMPRESSION_FSBAT9 = 5,
    SOUNDCOMPRESSION_FSBXMA = 6,
    SOUNDCOMPRESSION_FSBFADPCM = 7,
};

typedef int32 SoundCompression;

enum {
    SOUNDFORMAT_INVALID = -1,
    SOUNDFORMAT_MONO_11KHz_8BIT = 0,
    SOUNDFORMAT_MONO_22KHz_8BIT = 1,
    SOUNDFORMAT_MONO_44KHz_8BIT = 2,
    SOUNDFORMAT_STEREO_11KHz_8BIT = 3,
    SOUNDFORMAT_STEREO_22KHz_8BIT = 4,
    SOUNDFORMAT_STEREO_44KHz_8BIT = 5,
    SOUNDFORMAT_MONO_11KHz_16BIT = 6,
    SOUNDFORMAT_MONO_22KHz_16BIT = 7,
    SOUNDFORMAT_MONO_44KHz_16BIT = 8,
    SOUNDFORMAT_STEREO_11KHz_16BIT = 9,
    SOUNDFORMAT_STEREO_22KHz_16BIT = 10,
    SOUNDFORMAT_STEREO_44KHz_16BIT = 11,
    NUM_SOUND_FORMATS = 12,
};

typedef int32 SoundFormat;

enum {
    STREAMMODE_NO_STREAMING = 0,
    STREAMMODE_FROM_DISK = 1,
};

typedef int32 StreamMode;

enum {
    SPEAKERMODE_DEFAULT = 0,
    SPEAKERMODE_MONO = 1,
    SPEAKERMODE_STEREO = 2,
    SPEAKERMODE_QUAD = 3,
    SPEAKERMODE_SURROUND = 4,
    SPEAKERMODE_5POINT1 = 5,
    SPEAKERMODE_7POINT1 = 6,
    SPEAKERMODE_PROLOGIC = 7,
    NUM_SPEAKERMODES = 8,
};

typedef int32 SpeakerModes;

struct CompressionAndFormat {  // size 0x4
    uint32 bIsVerifiedVO : 1;  // 0x00
    uint32 bIsTemporaryVO : 1;  // 0x00
    uint32 eCompression : 10;  // 0x00
    uint32 nCompressionQuality : 10;  // 0x00
    uint32 eFormat : 10;  // 0x00
};
static_assert(sizeof(CompressionAndFormat) == 0x4, "CompressionAndFormat");

struct CompAndFormatUnion {  // size 0x4
    union {  // size 0x4
        CompressionAndFormat tCompAndFormat;  // 0x00
        uint32 dwCompAndFormat;  // 0x00
    };  // 0x00
};
static_assert(sizeof(CompAndFormatUnion) == 0x4, "CompAndFormatUnion");

struct SoundSample {  // size 0x10
    uint32 dwHash;  // 0x00
    CompAndFormatUnion tCompAndFormatUnion;  // 0x04
    BOOL IsTemporaryVO() const;
    BOOL IsVerifiedVO() const;
    SoundCompression GetCompression() const;
    int32 GetCompressionQuality() const;
    SoundFormat GetFormat() const;
    uint32 dwDecompressedSize;  // 0x08
    uint32 dwSampleSize;  // 0x0c
    uint32 GetSampleCount() const;
    uint32 ComputeDuration() const;
};
static_assert(sizeof(SoundSample) == 0x10, "SoundSample");

struct SoundBankDefinition {  // size 0x28
    RequiredDefinitionHeader tHeader;  // 0x00
    SerializeData serSoundSamples;  // 0x0c
    XPtr64<SoundSample> ptSoundSamples;  // 0x18
    int32 nSoundSampleCount;  // 0x20
    uint32 dwPad;  // 0x24
};
static_assert(sizeof(SoundBankDefinition) == 0x28, "SoundBankDefinition");

class SoundBankGroupDef : public GroupDefinition {  // size 0xa0, line 153
public:
    SoundBankGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 158; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 161
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 163
        return SNOPACKTYPE_SOUND;
    }
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void PurgeDefinitionAndSNO(const SNO sno);  // vtable slot 25
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual uint32 GetContiguousReadSize(SNOElement * ptSNOElement);  // vtable slot 27
};
static_assert(sizeof(SoundBankGroupDef) == 0xa0, "SoundBankGroupDef");

extern SoundBankGroupDef * gptGroupDefSoundBank;  // SoundBankDefinition.cpp:99

typedef ScopedDefinitionLockR<SoundBankDefinition> ScopedSoundBankDefinitionLockR;

template <> inline const SoundBankDefinition * DefinitionLockR<SoundBankDefinition>(SNO sno) {  // line 248
    return (const SoundBankDefinition *)gptGroupDefSoundBank->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<SoundBankDefinition>(const SoundBankDefinition * pdef) {  // line 249
    void * lpDef = (void *)pdef;
    gptGroupDefSoundBank->UnlockR(lpDef);
}

void SoundFormatToCSB(const SoundFormat eFormat, uint32 & dwChannelCount, uint32 & dwFrequency, uint32 & dwBitRate);
BOOL SoundFormatIsMono(const SoundFormat eFormat);
int32 SoundBankSampleIndexFromHash(const SNO snoSoundBank, uint32 dwHash);
BOOL SoundBankGetSampleFullFilename(char * szDest, int32 nDestLength, SNO tSno, int32 nIndex);


// Functions defined in this file (bodies are not in the DWARF):
//   line   136  SoundBankDefinition::SoundBankDefinition()  [inlined into 1 functions]
//   line   153  SoundBankGroupDef::~SoundBankGroupDef()  [0x71003dca40]
//   line   158  void * SoundBankGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71003dca70 (+1 more)]
//   line   158  void SoundBankGroupDef::MemFree(void *& ptr) const  [0x71003dcac0 (+1 more)]
//   line   161  BOOL SoundBankGroupDef::CanReferenceOtherSNOs() const  [0x71003dcb50]
//   line   163  SNOPackType SoundBankGroupDef::GetSNOPackDestination(const void *) const  [0x71003dcb40]
//   line   248  const SoundBankDefinition * DefinitionLockR<SoundBankDefinition>(SNO sno)  [inlined into 16 functions]
//   line   249  void DefinitionUnlockR<SoundBankDefinition>(const SoundBankDefinition * pdef)  [inlined into 16 functions]
