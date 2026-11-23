// Original file: X/Client/Definitions/SoundBankDefinition.cpp

#include "SoundBankDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/SNO/SNOFiles.cpp:6271; no adopted header declares it yet.
void SNOFileGetFilePath(const SNOName & snoname, char * szFilePath, int32 nFilePathMaxSize);

// Defined in BlizzardCore/Source/System/String/Path.cpp:280; no adopted header declares it yet.
namespace Blizzard {
namespace String {
const char * FindExtensionWithLeadingDot(const char * buf);
}
}

// X/Client/Sound/SoundPlatform.h (not adopted). Only the two static members called here are
// declared; they are defined in X/Client/Sound/SoundPlatformFModEx.cpp:2993 and :2999.
class SoundPlatform {
public:
    static void OnBankLoaded(const SoundBankDefinition *);
    static void OnBankUnloaded(const SoundBankDefinition * ptSoundBankDef);
};

enum {
    SOUNDBANK_COMMON_SNO_VERSION = 41,
    SOUNDBANK_XML_VERSION = 1,
    SOUNDBANK_PC_SNO_VERSION = 41,
    SOUNDBANK_CONSOLE_SNO_VERSION = 43,
    SOUNDBANK_AXE_SNO_VERSION = 42,
    SOUNDBANK_XBOXONE_SNO_VERSION = 48,
    SOUNDBANK_PS4_SNO_VERSION = 50,
    SOUNDBANK_NX64_SNO_VERSION = 45,
    SOUNDBANK_SAMPLE_XML_VERSION = 2,
};

// _GLOBAL__sub_I_SoundBankDefinition.cpp also runs the header statics of the includes above (cgplaceNull,
// the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354).
// The field names are "" in the binary.
static const TypeDescriptorField SoundSample_Fields[5] = {  // line 57
    TypeDescriptorField("", DT_INT, offsetof(SoundSample, dwHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SoundSample, tCompAndFormatUnion), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(SoundSample, dwDecompressedSize), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SoundSample, dwSampleSize), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SoundSample), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SoundSample("SoundSample", SOUNDBANK_SAMPLE_XML_VERSION, SoundSample_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 72
const XTypeDescriptor * DT_SOUNDSAMPLE = &TypeDescriptor_SoundSample;

static const TypeDescriptorField SoundBankDefinition_Fields[5] = {  // line 74
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(SoundBankDefinition, serSoundSamples), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SOUNDSAMPLE, offsetof(SoundBankDefinition, ptSoundSamples), 0x21,
                                     (int32)offsetof(SoundBankDefinition, serSoundSamples) - (int32)offsetof(SoundBankDefinition, ptSoundSamples)),
    VariableArrayCountTypeDescriptorField("", offsetof(SoundBankDefinition, nSoundSampleCount), 0x10000, DT_SOUNDSAMPLE,
                                          (int32)offsetof(SoundBankDefinition, serSoundSamples) - (int32)offsetof(SoundBankDefinition, nSoundSampleCount)),
    TypeDescriptorField("", DT_INT, offsetof(SoundBankDefinition, dwPad), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SoundBankDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SoundBankDefinition("SoundBank", SOUNDBANK_XML_VERSION, SoundBankDefinition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 81
const XTypeDescriptor * DT_SOUNDBANK = &TypeDescriptor_SoundBankDefinition;

static const char * arSoundCompressionExtensions[8] = {  // line 87
    ".smp",
    ".fsb",
    ".mp3",
    ".ogg",
    ".fsb",
    ".fsb",
    ".fsb",
    ".fsb",
};

SoundBankGroupDef * gptGroupDefSoundBank;  // line 99

// line 107
BOOL SoundSample::IsTemporaryVO() const {
    return tCompAndFormatUnion.tCompAndFormat.bIsTemporaryVO;
}

// line 125
SoundCompression SoundSample::GetCompression() const {
    return tCompAndFormatUnion.tCompAndFormat.eCompression;
}

// line 143
SoundFormat SoundSample::GetFormat() const {
    return tCompAndFormatUnion.tCompAndFormat.eFormat;
}

// line 156
SoundBankGroupDef::SoundBankGroupDef()
    : GroupDefinition("SoundBank", SOUNDBANK_SNO,
                      SNOVersions(SOUNDBANK_PC_SNO_VERSION, 44, SOUNDBANK_XBOXONE_SNO_VERSION, SOUNDBANK_PS4_SNO_VERSION, 46),
                      SNODATATYPE_TITLE, sizeof(SoundBankDefinition), DT_SOUNDBANK, 0x803, 0x100e) {
}

// line 184
uint32 SoundBankGroupDef::GetMaxCount() {
    return 128;
}

// line 196
void SoundBankGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(SoundBankDefinition));
}

// line 206
void SoundBankGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    SoundBankDefinition * pdef = (SoundBankDefinition *)lpDefinition;
    SoundPlatform::OnBankLoaded(pdef);
}

// line 218
static const char * sGetSoundCompressionExtension(const SoundCompression eCompression) {
    return arSoundCompressionExtensions[eCompression];
}

// line 299. Not in the binary as a function (inlined only), so inline.
inline void GetSampleFilename(char * szSampleFilename, const uint32 dwMaxFilenameLen, const char * szFolderName, const uint32 dwIndex,
                              const SoundCompression eCompression) {
    Blizzard::String::Format(szSampleFilename, dwMaxFilenameLen, "%s\\%04d%s", szFolderName, dwIndex,
                             sGetSoundCompressionExtension(eCompression));
}

// line 313. Not in the binary as a function (inlined only), so inline.
inline void GetSampleFilename(const char * szSnoName, char * szSampleFilename, const uint32 szSampleFilenameLen, const uint32 dwIndex,
                              const SoundCompression eCompression) {
    uint32 realLen = Blizzard::String::FindExtensionWithLeadingDot(szSnoName) - szSnoName;
    CRefString szRealPath(szSnoName, realLen);
    GetSampleFilename(szSampleFilename, szSampleFilenameLen, szRealPath.str(), dwIndex, eCompression);
}

// line 431
static BOOL sGetSampleFullname(const SoundBankDefinition * ptdefSoundBank, int32 nIndex, SNO tSno, char * szDest, int32 nDestLength) {
    if (ptdefSoundBank->tHeader.dwFlags & (1 << DH_IS_BAD_DATA_BIT))
        return FALSE;

    SoundSample * ptSoundSamples = ptdefSoundBank->ptSoundSamples;
    SNOName snoname(SOUNDBANK_SNO, tSno);
    char szPath[4096];
    SNOFileGetFilePath(snoname, szPath, sizeof(szPath));
    GetSampleFilename(szPath, szDest, nDestLength, nIndex, ptSoundSamples[nIndex].GetCompression());
    return TRUE;
}

// line 634
BOOL SoundBankGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 640, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// line 644
void SoundBankGroupDef::PurgeDefinitionAndSNO(const SNO sno) {
    GroupDefinition::PurgeDefinitionAndSNO(sno);
}

// line 655
void SoundBankGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer,
                             uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);

    SoundBankDefinition * pdef = (SoundBankDefinition *)lpDefinition;

    for (int32 i = 0; i < pdef->nSoundSampleCount; i++) {
        SoundSample & tSoundSample = pdef->ptSoundSamples[i];
        tSoundSample.dwSampleSize = 0;
    }
}

// line 682
void SoundBankGroupDef::Delete(void * lpDefinition) const {
    SoundBankDefinition * pdef = (SoundBankDefinition *)lpDefinition;
    SoundPlatform::OnBankUnloaded(pdef);

    GroupDefinition::Delete(lpDefinition);
}

// line 704
uint32 SoundBankGroupDef::GetContiguousReadSize(SNOElement * ptSNOElement) {
    SoundBankDefinition tDef;
    ReadDefinitionHeader(ptSNOElement, &tDef);

    uint32 dwSampleSize = tDef.serSoundSamples.dwSize;
    uint32 dwContiguousSize = sizeof(SoundBankDefinition) + dwSampleSize;
    return dwContiguousSize;
}

// line 861
void SoundFormatToCSB(const SoundFormat eFormat, uint32 & dwChannelCount, uint32 & dwFrequency, uint32 & dwBitRate) {
    switch (eFormat) {
    case SOUNDFORMAT_MONO_11KHz_8BIT:
        dwChannelCount = 1;
        dwFrequency = 11025;
        dwBitRate = 8;
        break;
    case SOUNDFORMAT_MONO_22KHz_8BIT:
        dwChannelCount = 1;
        dwFrequency = 22050;
        dwBitRate = 8;
        break;
    case SOUNDFORMAT_MONO_44KHz_8BIT:
        dwChannelCount = 1;
        dwFrequency = 44100;
        dwBitRate = 8;
        break;
    case SOUNDFORMAT_STEREO_11KHz_8BIT:
        dwChannelCount = 2;
        dwFrequency = 11025;
        dwBitRate = 8;
        break;
    case SOUNDFORMAT_STEREO_22KHz_8BIT:
        dwChannelCount = 2;
        dwFrequency = 22050;
        dwBitRate = 8;
        break;
    case SOUNDFORMAT_STEREO_44KHz_8BIT:
        dwChannelCount = 2;
        dwFrequency = 44100;
        dwBitRate = 8;
        break;
    case SOUNDFORMAT_MONO_11KHz_16BIT:
        dwChannelCount = 1;
        dwFrequency = 11025;
        dwBitRate = 16;
        break;
    case SOUNDFORMAT_MONO_22KHz_16BIT:
        dwChannelCount = 1;
        dwFrequency = 22050;
        dwBitRate = 16;
        break;
    case SOUNDFORMAT_MONO_44KHz_16BIT:
        dwChannelCount = 1;
        dwFrequency = 44100;
        dwBitRate = 16;
        break;
    case SOUNDFORMAT_STEREO_11KHz_16BIT:
        dwChannelCount = 2;
        dwFrequency = 11025;
        dwBitRate = 16;
        break;
    case SOUNDFORMAT_STEREO_22KHz_16BIT:
        dwChannelCount = 2;
        dwFrequency = 22050;
        dwBitRate = 16;
        break;
    case SOUNDFORMAT_STEREO_44KHz_16BIT:
        dwChannelCount = 2;
        dwFrequency = 44100;
        dwBitRate = 16;
        break;
    default:
        DisplayInternalError(NULL, "", 942, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

// line 975
BOOL SoundFormatIsMono(const SoundFormat eFormat) {
    switch (eFormat) {
    case SOUNDFORMAT_MONO_11KHz_8BIT:
    case SOUNDFORMAT_MONO_22KHz_8BIT:
    case SOUNDFORMAT_MONO_44KHz_8BIT:
    case SOUNDFORMAT_MONO_11KHz_16BIT:
    case SOUNDFORMAT_MONO_22KHz_16BIT:
    case SOUNDFORMAT_MONO_44KHz_16BIT:
        return TRUE;
    }

    return FALSE;
}

// line 1018. Not in the binary as a function (inlined only), so inline.
inline int32 SoundBankSampleIndexFromHash(const SoundBankDefinition * pdef, uint32 dwHash) {
    for (int32 i = 0; i < pdef->nSoundSampleCount; i++) {
        if (pdef->ptSoundSamples[i].dwHash == dwHash)
            return i;
    }

    return -1;
}

// line 1036
int32 SoundBankSampleIndexFromHash(const SNO snoSoundBank, uint32 dwHash) {
    ScopedSoundBankDefinitionLockR pdefSoundBank(snoSoundBank);
    int32 nResult = SoundBankSampleIndexFromHash(pdefSoundBank, dwHash);
    return nResult;
}

// line 1153
uint32 SoundSample::ComputeDuration() const {
    uint32 dwChannels;
    uint32 dwFreq;
    uint32 dwBitRate;
    SoundFormatToCSB(GetFormat(), dwChannels, dwFreq, dwBitRate);

    uint32 dwSizePerSample = (dwBitRate * dwChannels) / 8;
    uint32 dwSampleCount = dwSizePerSample ? dwDecompressedSize / dwSizePerSample : 0;
    uint32 dwDuration = dwFreq ? (dwSampleCount * 60) / dwFreq : 0;
    return dwDuration;
}

// line 1181
BOOL SoundBankGetSampleFullFilename(char * szDest, int32 nDestLength, SNO tSno, int32 nIndex) {
    if (tSno == SNO_NONE)
        return FALSE;

    szDest[0] = '\0';
    ScopedSoundBankDefinitionLockR pdefSoundBank(tSno);
    return sGetSampleFullname(pdefSoundBank, nIndex, tSno, szDest, nDestLength);
}
