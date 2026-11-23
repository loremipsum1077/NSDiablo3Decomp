// Original file: X/Client/Definitions/TextureDefinition.cpp

#include "TextureDefinition.h"

// For SNONAME_NONE (SNOName.h:142), cgplaceNull (WorldPlace.h:23) and the four RGBAColor
// normals (GfxVB.h:351-354), whose initializers the original runs in this file's static
// initializer. The DWARF does not record the include chain that reaches them.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
// blz::ignore reserves the byte at the base of the merged block the four normals sit in (see
// SurfaceDefinition.cpp).
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Game.h"
#include "../Gfx/GfxAPI.h"
#include "../Gfx/GfxInternal.h"
#include "../Prefetch.h"
#include "PreferencesDefinition.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/GfxColorFormats.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

#include <string.h>

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;
// Defined in X/Client/Gfx/GfxCommon.cpp:44; GfxCommon.h does not declare it yet.
extern const XTypeDescriptor * DT_TEXFRAME;

// Defined in X/Common/SNO/SNOFiles.cpp:5014 and :10409 and X/Common/SNO/SNOML_X.cpp:344 (only part
// of SNOFiles.h is adopted).
void SNOFileClose(SNOElement * ptSNOElement);
void sTraceBinary(const uint8 * pData, size_t nSize);
SNOElement * SNOMLFileOpenForRead(const SNOName & snoname);

// Defined in X/Client/UserInterface/UIUtils.cpp:335 (UIUtils.h does not declare it yet).
void ConvertUICToPixelsDoNotSnap(const Rect2D & rectUIC, Rect2D & rectPixels);

// Defined in X/Client/Gfx/Gfx.cpp (Gfx.h is only adopted for GfxVB.cpp); lines in the comments.
struct TextureInitialData;
BOOL GfxTextureFormatIsValid(const TextureFormat eTexFormat, const uint32 dwTexCreateFlags);  // :2944
BOOL GfxTextureCreate(TexData & tTexData, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight,
                      const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwMSAALevel,
                      const uint32 dwTargetERAMOffset, const int32 nExponentBias, const TextureInitialData * pInitialData,
                      const char * szTexDebugName);  // :2970
void GfxTextureDispose(TexData & tTexData);  // :3072
BOOL GfxCubeMapCreate(TexData & tTexData, const TextureFormat eTexFormat, const uint32 dwEdgeWidth, const uint32 dwMipMapLevels,
                      const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwTargetERAMOffset, const int32 nExponentBias,
                      const TextureInitialData * pTextureInitialData, const char * szTexDebugName);  // :3132
BOOL GfxVolumeTexCreate(TexData & tTexData, const TextureFormat eTexFormat, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwDepth,
                        const uint32 dwMipMapLevels, const ResourceUsage eUsage, const uint32 dwTexCreateFlags,
                        const TextureInitialData * pTextureInitialData, const char * szTexDebugName);  // :3188
void GfxTextureLock(const TexData & tTexData, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwPitch, const uint32 dwFlags);  // :3651
void GfxTextureUnlock(const TexData & tTexData, const uint32 dwMipMapLevel);  // :3664
void GfxCubeMapLock(const TexData & tTexData, const CubeMapFace eCubeMapFace, const uint32 dwMipMapLevel, void *& lpTexture,
                    uint32 & dwPitch, const uint32 dwFlags);  // :3674
void GfxCubeMapUnlock(const TexData & tTexData, const CubeMapFace eCubeMapFace, const uint32 dwMipMapLevel);  // :3688
void GfxVolumeTexLock(const TexData & tTexData, const uint32 dwMipMapLevel, void *& lpTexture, uint32 & dwRowPitch,
                      uint32 & dwSlicePitch, const uint32 dwGfxFlags);  // :3699
void GfxVolumeTexUnlock(const TexData & tTexData, const uint32 dwMipMapLevel);  // :3713
TextureFormat TextureFormatGetFallbackFormat(const TextureFormat eTextureFormat);  // :4000

enum {
    TEXTURE_COMMON_SNO_VERSION = 47,
    TEXTURE_XML_VERSION = 6,
    TEXTURE_PC_SNO_VERSION = 47,
    TEXTURE_CONSOLE_SNO_VERSION = 60,
    TEXTURE_PS4_SNO_VERSION = 66,
    TEXTURE_XBOXONE_SNO_VERSION = 61,
    TEXTURE_AXE_SNO_VERSION = 55,
    TEXTURE_NX64_SNO_VERSION = 65,
    MAX_TEXTURE_DEFS_IN_MEMORY = 512,
    MAX_TEXTURE_CACHE_SIZE = 125829120,
    MAX_TEXTURE_CACHE_SIZE_MIP_OFFSET = 83886080,
};

struct TextureParams {  // size 0x28
    TextureFormat eSupportedTextureFormat;  // 0x00
    int32 nMipLevelStart;  // 0x04
    int32 nMipLevelEnd;  // 0x08
    int32 nMipLevels;  // 0x0c
    BOOL fCompressed;  // 0x10
    uint32 dwWidth;  // 0x14
    uint32 dwHeight;  // 0x18
    uint32 dwPixelSize;  // 0x1c
    uint32 dwEstimatedSize;  // 0x20
    uint32 dwDepth;  // 0x24
};

struct DXTnColorBlock {  // size 0x8
    Pixel16 pColor0;  // 0x00
    Pixel16 pColor1;  // 0x02
    uint32 dwInterpBits;  // 0x04
};

struct ExplicitAlphaBlock {  // size 0x8
    uint16 wAlpha[4];  // 0x00
};

struct InterpAlphaBlock {  // size 0x8
    uint8 bAlpha0;  // 0x00
    uint8 bAlpha1;  // 0x01
    uint8 bInterpBits[6];  // 0x02
};

struct DXT3Block {  // size 0x10
    ExplicitAlphaBlock tAlphaBlock;  // 0x00
    DXTnColorBlock tColorBlock;  // 0x08
};

struct DXT5Block {  // size 0x10
    InterpAlphaBlock tAlphaBlock;  // 0x00
    DXTnColorBlock tColorBlock;  // 0x08
};

static const TypeDescriptorField ImageFileID_Fields[2] = {  // line 80
    TypeDescriptorField("", DT_INT, offsetof(ImageFileID, nFrameIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ImageFileID), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ImageFileID("ImageFileID", 0, ImageFileID_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_IMAGE_FILE_ID = &TypeDescriptor_ImageFileID;  // line 84

static const TypeDescriptorField TextureDefinition_Fields[27] = {  // line 86
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, eTexFormat), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwWidth), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwHeight), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwFaceCount), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwDepth), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwPadding2), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwMipMapLevelMax), NULL, 0),
    FixedArrayTypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TextureDefinition, serTex), NULL, 0, 60, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwMipMapLevelMin), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TextureDefinition, serImageFileIDs), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_IMAGE_FILE_ID, offsetof(TextureDefinition, ptImageFileIDs), 0x21,
                                     (int32)offsetof(TextureDefinition, serImageFileIDs) - (int32)offsetof(TextureDefinition, ptImageFileIDs)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TextureDefinition, serFrame), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_TEXFRAME, offsetof(TextureDefinition, ptFrame), 0x21,
                                     (int32)offsetof(TextureDefinition, serFrame) - (int32)offsetof(TextureDefinition, ptFrame)),
    VariableArrayCountTypeDescriptorField("", offsetof(TextureDefinition, dwFrameCount), 0x10000, DT_TEXFRAME,
                                          (int32)offsetof(TextureDefinition, serFrame) - (int32)offsetof(TextureDefinition, dwFrameCount)),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(TextureDefinition, pHotspot), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, bCubemap), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwImportFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwMachineTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwConsoleMipOffset), NULL, 0x700001),
    TypeDescriptorField("", DT_FLOAT, offsetof(TextureDefinition, flConsoleLightmapVScale), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwVolumeXSlices), (const void *)1, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwVolumeYSlices), (const void *)1, 0x700001),
    TypeDescriptorField("", DT_INT64, offsetof(TextureDefinition, prectFrame), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, tTexData), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, eUsage), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TextureDefinition, dwLastFrameChecked), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TextureDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TextureDefinition("Textures", TEXTURE_XML_VERSION, TextureDefinition_Fields, 27, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_TEXTURE = &TypeDescriptor_TextureDefinition;  // line 124

TextureGroupDef * gptGroupDefTexture;  // line 266

static void sTextureDefinitionForceLoad(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, BOOL fCheckLoaded, void * ptSNOData,
                                        uint32 dwSNODataDeltaOffset);
static void sCachePurgeFunc(TexData & tTexData);
static void sReplaceTextureWithMipMapLevelColor(uint32 dwLevel, void * ptBuf, uint32 dwSize, TextureFormat eTexFormat);
static TextureFormat sFindClosestSupportedTextureFormat(const TextureFormat eDesiredFormat, const uint32 dwTexCreateFlags);
static void sLoadTexture(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset);
static void sLoadCubeMap(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset);
static void sLoadVolumeTexture(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset);
static void sLoadAllMipLevels(SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset, const TextureDefinition * pdefTexture);
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex);

// line 275
TextureGroupDef::TextureGroupDef()
    : GroupDefinition("Textures", TEXTURE_SNO,
                      SNOVersions(TEXTURE_PC_SNO_VERSION, 57, TEXTURE_XBOXONE_SNO_VERSION, TEXTURE_PS4_SNO_VERSION, 66),
                      SNODATATYPE_TITLE, sizeof(TextureDefinition), DT_TEXTURE, 0x2001, 0x9c1a) {
}

// line 303
uint32 TextureGroupDef::GetMaxCount() {
    return MAX_TEXTURE_DEFS_IN_MEMORY;
}

// line 353
void TextureGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(TextureDefinition));

    TextureDefinition * pdef = (TextureDefinition *)ptDefaultDef;
    pdef->eTexFormat = TEXTUREFORMAT_ARGB8888;
    pdef->dwWidth = 1024;
    pdef->dwHeight = 1024;
    pdef->dwMipMapLevelMin = 0;
    pdef->dwMipMapLevelMax = 0;
    pdef->dwDepth = 1;
    pdef->dwVolumeXSlices = 1;
    pdef->dwVolumeYSlices = 1;
}

// line 374
void TextureGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    TextureDefinition * pdef = (TextureDefinition *)lpDefinition;
    if (!(pdef->tHeader.dwFlags & (1 << DH_IS_EXTERNAL_BIT)))
        TextureComputeFramePixelCoords(pdef);

    BOOL bNoMipOffset = (pdef->dwImportFlags & 0x10) != 0;
    if (bNoMipOffset)
        pdef->dwRuntimeFlags |= (1 << TEXTURE_DEF_IGNORE_MIPOFFSET_BIT);
    else
        pdef->dwRuntimeFlags &= ~(1 << TEXTURE_DEF_IGNORE_MIPOFFSET_BIT);
}

// line 505
BOOL TextureGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 511, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// line 538
void TextureGroupDef::Delete(void * lpDefinition) const {
    TextureDefinition * ptDefinition = (TextureDefinition *)lpDefinition;

    SigmaMemoryFree(ptDefinition->prectFrame, NULL);
    ptDefinition->prectFrame = NULL;

    sCachePurgeFunc(ptDefinition->tTexData);

    GroupDefinition::Delete(lpDefinition);
}

// line 566
void TextureGroupDef::PurgeAllDynamic() {
    TDataArrayIterator<void> tIt;
    TextureDefinition * ptDefinition;
    while (DefinitionDataArrayIterate(tIt, (void *&)ptDefinition)) {
        if (WasDynamicallyCreated(ptDefinition->tHeader.snoHandle))
            Purge(ptDefinition->tHeader.snoHandle, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
    }
}

// line 587
void TextureGroupDef::Dispose() {
    GroupDefinition::Dispose();
}

// line 628



static void sTextureDefinitionForceLoad(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, BOOL fCheckLoaded, void * ptSNOData,
                                        uint32 dwSNODataDeltaOffset) {
    if (pdefTexture->tTexData.htex)
        return;

    if (pdefTexture->dwFaceCount > 1)
        sLoadCubeMap(pdefTexture, ptSNOElement, ptSNOData, dwSNODataDeltaOffset);
    else if (pdefTexture->dwDepth > 1)
        sLoadVolumeTexture(pdefTexture, ptSNOElement, ptSNOData, dwSNODataDeltaOffset);
    else
        sLoadTexture(pdefTexture, ptSNOElement, ptSNOData, dwSNODataDeltaOffset);
}

// line 706
void TextureDefinitionSetTexture(const SNO sno, TexData & tTexData) {
    ScopedTextureDefinitionLockR pdefTexture(sno);

    if (!pdefTexture->tTexData.htex)
        sTextureDefinitionForceLoad(pdefTexture, NULL, FALSE, NULL, 0);

    if (pdefTexture->eTexFormat == TEXTUREFORMAT_DEPTHOPAQUE)
        tTexData.htex = NULL;
    else
        tTexData = pdefTexture->tTexData;
}

// line 822
static void sComputeTextureParams(const TextureDefinition * pdefTexture, const uint32 dwTexCreateFlags, TextureParams & tTexParams) {
    tTexParams.eSupportedTextureFormat = sFindClosestSupportedTextureFormat(pdefTexture->eTexFormat, dwTexCreateFlags);
    tTexParams.dwPixelSize = TextureFormatSize(tTexParams.eSupportedTextureFormat);
    tTexParams.fCompressed = TextureFormatIsCompressed(tTexParams.eSupportedTextureFormat);

    int32 nMipLevelOffset;
    if (pdefTexture->dwRuntimeFlags & (1 << TEXTURE_DEF_IGNORE_MIPOFFSET_BIT))
        nMipLevelOffset = 0;
    else
        nMipLevelOffset = Prefs::GetVideoPrefs().GetTextureMipOffset();

    int32 nMaxMipLevel = pdefTexture->dwMipMapLevelMax;
    if (tTexParams.fCompressed) {
        uint32 dwMipWidth = pdefTexture->dwWidth >> (nMaxMipLevel - pdefTexture->dwMipMapLevelMin);
        uint32 dwMipHeight = pdefTexture->dwHeight >> (nMaxMipLevel - pdefTexture->dwMipMapLevelMin);
        while (dwMipWidth < 4 || dwMipHeight < 4) {
            nMaxMipLevel--;
            dwMipWidth = pdefTexture->dwWidth >> (nMaxMipLevel - pdefTexture->dwMipMapLevelMin);
            dwMipHeight = pdefTexture->dwHeight >> (nMaxMipLevel - pdefTexture->dwMipMapLevelMin);
        }
    }

    nMaxMipLevel -= pdefTexture->dwMipMapLevelMin;
    tTexParams.nMipLevelEnd = nMaxMipLevel + 1;
    tTexParams.nMipLevelStart = Math::Min(nMipLevelOffset, nMaxMipLevel);
    tTexParams.nMipLevels = tTexParams.nMipLevelEnd - tTexParams.nMipLevelStart;

    TextureGetMipDimensions(pdefTexture->dwWidth, pdefTexture->dwHeight, pdefTexture->dwDepth, tTexParams.nMipLevelStart, tTexParams.dwWidth,
                            tTexParams.dwHeight, tTexParams.dwDepth);
    tTexParams.dwEstimatedSize = tTexParams.dwWidth * tTexParams.dwHeight * tTexParams.dwDepth * tTexParams.dwPixelSize;
    if (tTexParams.fCompressed)
        tTexParams.dwEstimatedSize /= 16;
}

// line 886
static SNO sTextureDefinitionCreate(LPCSTR szTexDebugName, const TextureFormat eDesiredTexFormat, TextureFormat & eSupportedTextureFormat,
                                    const uint32 dwFaceCount, const uint32 dwWidth, const uint32 dwHeight, const ResourceUsage eUsage,
                                    const uint32 dwTexCreateFlags, const uint32 dwMSAALevel, const uint32 dwTargetERAMOffset,
                                    const int32 nExponentBias) {
    eSupportedTextureFormat = sFindClosestSupportedTextureFormat(eDesiredTexFormat, dwTexCreateFlags);

    TextureDefinition defTexture;
    SigmaMemoryClear(&defTexture, sizeof(defTexture));
    defTexture.eTexFormat = eSupportedTextureFormat;
    defTexture.dwRuntimeFlags = 0;
    defTexture.dwWidth = dwWidth;
    defTexture.dwHeight = dwHeight;
    defTexture.dwMipMapLevelMin = 0;
    defTexture.dwMipMapLevelMax = 0;
    defTexture.dwFaceCount = dwFaceCount;
    defTexture.eUsage = eUsage;

    SNO snoResult = gptGroupDefTexture->New(&defTexture, FALSE, 0);
    if (snoResult != SNO_NONE) {
        const TextureDefinition * pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(snoResult, FALSE);
        if (defTexture.dwFaceCount == 1) {
            while (!GfxTextureCreate(const_cast<TexData &>(pdefTexture->tTexData), pdefTexture->eTexFormat, pdefTexture->dwWidth,
                                     pdefTexture->dwHeight, 1, pdefTexture->eUsage, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset,
                                     nExponentBias, NULL, szTexDebugName))
                gptGroupDefTexture->CacheManagerPurge(FALSE);
        } else {
            while (!GfxCubeMapCreate(const_cast<TexData &>(pdefTexture->tTexData), pdefTexture->eTexFormat, pdefTexture->dwWidth, 1,
                                     pdefTexture->eUsage, dwTexCreateFlags, dwTargetERAMOffset, nExponentBias, NULL, szTexDebugName))
                gptGroupDefTexture->CacheManagerPurge(FALSE);
        }

        if (dwTexCreateFlags & 1)
            const_cast<TextureDefinition *>(pdefTexture)->dwRuntimeFlags |= (1 << TEXTURE_DEF_UNMANAGED_BIT);

        gptGroupDefTexture->UnlockR((void *&)pdefTexture);
    }

    return snoResult;
}

// line 994
SNO TextureDefinitionCreateTexture(LPCSTR szTexDebugName, const TextureFormat eDesiredTexFormat, TextureFormat & eSupportedTextureFormat,
                                   const uint32 dwWidth, const uint32 dwHeight, const ResourceUsage eUsage, const uint32 dwTexCreateFlags,
                                   const uint32 dwMSAALevel, const uint32 dwTargetERAMOffset, const int32 nExponentBias) {
    return sTextureDefinitionCreate(szTexDebugName, eDesiredTexFormat, eSupportedTextureFormat, 1, dwWidth, dwHeight, eUsage, dwTexCreateFlags,
                                    dwMSAALevel, dwTargetERAMOffset, nExponentBias);
}

// Not in the binary. sTextureDefinitionCreate is out of line and keeps its dwFaceCount parameter
// and its cube-map branch, so the original had a second caller that the linker later discarded;
// lines 1000-1051 have room for it. The name and body are invented: a cube-map twin of
// TextureDefinitionCreateTexture.
SNO TextureDefinitionCreateCubeMap(LPCSTR szTexDebugName, const TextureFormat eDesiredTexFormat, TextureFormat & eSupportedTextureFormat,
                                   const uint32 dwEdgeWidth, const ResourceUsage eUsage, const uint32 dwTexCreateFlags,
                                   const uint32 dwTargetERAMOffset, const int32 nExponentBias) {
    return sTextureDefinitionCreate(szTexDebugName, eDesiredTexFormat, eSupportedTextureFormat, NUM_CUBEMAPFACES, dwEdgeWidth, dwEdgeWidth, eUsage,
                                    dwTexCreateFlags, 1, dwTargetERAMOffset, nExponentBias);
}

// line 1052
static void * sTextureDefinitionLock(const TextureDefinition * pdefTexture, const CubeMapFace eCubemapFace, const uint32 dwMipLevel,
                                     uint32 & dwPitch, uint32 & dwHeight, const uint32 dwFlags) {
    void * lpBuf;
    if (pdefTexture->dwFaceCount > 1)
        GfxCubeMapLock(pdefTexture->tTexData, eCubemapFace, dwMipLevel, lpBuf, dwPitch, dwFlags);
    else
        GfxTextureLock(pdefTexture->tTexData, dwMipLevel, lpBuf, dwPitch, dwFlags);

    dwHeight = pdefTexture->dwHeight >> dwMipLevel;
    return lpBuf;
}

// line 1081
void * TextureDefinitionLockTexture(const SNO sno, uint32 & dwPitch, uint32 & dwHeight, const uint32 dwMipLevel, const uint32 dwFlags) {
    ScopedTextureDefinitionLockR pdefTexture(sno);

    return sTextureDefinitionLock(pdefTexture, (CubeMapFace)-1, dwMipLevel, dwPitch, dwHeight, dwFlags);
}

// line 1217
static void sTextureDefinitionUnlock(const TextureDefinition * pdefTexture, void * lpBuf, const CubeMapFace eCubemapFace, const uint32 dwMipLevel) {
    if (pdefTexture->dwFaceCount > 1)
        GfxCubeMapUnlock(pdefTexture->tTexData, eCubemapFace, dwMipLevel);
    else
        GfxTextureUnlock(pdefTexture->tTexData, dwMipLevel);
}

// line 1241
void TextureDefinitionUnlockTexture(const SNO sno, void * lpBuf, const uint32 dwMipLevel) {
    ScopedTextureDefinitionLockR pdefTexture(sno);

    sTextureDefinitionUnlock(pdefTexture, lpBuf, (CubeMapFace)-1, dwMipLevel);
}

// line 1270
static void sColorFromOpaqueDXTnColorBlock(const DXTnColorBlock * ptBlock, const int32 nPixelNum, RGBAColor & rgbaColor) {
    RGBAColor rgbaColor0;
    RGBAColor rgbaColor1;
    uint32 dwCode = (ptBlock->dwInterpBits >> (nPixelNum * 2)) & 3;
    switch (dwCode) {
        case 0:
            rgbaColor.FromTexturePixel(&ptBlock->pColor0, TEXTUREFORMAT_RGB565);
            break;
        case 1:
            rgbaColor.FromTexturePixel(&ptBlock->pColor1, TEXTUREFORMAT_RGB565);
            break;
        case 2:
            rgbaColor0.FromTexturePixel(&ptBlock->pColor0, TEXTUREFORMAT_RGB565);
            rgbaColor1.FromTexturePixel(&ptBlock->pColor1, TEXTUREFORMAT_RGB565);
            rgbaColor.r = (2 * rgbaColor0.r + rgbaColor1.r + 1) / 3;
            rgbaColor.g = (2 * rgbaColor0.g + rgbaColor1.g + 1) / 3;
            rgbaColor.b = (2 * rgbaColor0.b + rgbaColor1.b + 1) / 3;
            rgbaColor.a = 255;
            break;
        case 3:
            rgbaColor0.FromTexturePixel(&ptBlock->pColor0, TEXTUREFORMAT_RGB565);
            rgbaColor1.FromTexturePixel(&ptBlock->pColor1, TEXTUREFORMAT_RGB565);
            rgbaColor.r = (rgbaColor0.r + 2 * rgbaColor1.r + 1) / 3;
            rgbaColor.g = (rgbaColor0.g + 2 * rgbaColor1.g + 1) / 3;
            rgbaColor.b = (rgbaColor0.b + 2 * rgbaColor1.b + 1) / 3;
            rgbaColor.a = 255;
            break;
    }
}

// line 1312
static void sColorFrom1BitAlphaDXTnColorBlock(const DXTnColorBlock * ptBlock, const int32 nPixelNum, RGBAColor & rgbaColor) {
    if (ptBlock->pColor0 > ptBlock->pColor1) {
        sColorFromOpaqueDXTnColorBlock(ptBlock, nPixelNum, rgbaColor);
        return;
    }

    RGBAColor rgbaColor0;
    RGBAColor rgbaColor1;
    uint32 dwCode = (ptBlock->dwInterpBits >> (nPixelNum * 2)) & 3;
    switch (dwCode) {
        case 0:
            rgbaColor.FromTexturePixel(&ptBlock->pColor0, TEXTUREFORMAT_RGB565);
            break;
        case 1:
            rgbaColor.FromTexturePixel(&ptBlock->pColor1, TEXTUREFORMAT_RGB565);
            break;
        case 2:
            rgbaColor0.FromTexturePixel(&ptBlock->pColor0, TEXTUREFORMAT_RGB565);
            rgbaColor1.FromTexturePixel(&ptBlock->pColor1, TEXTUREFORMAT_RGB565);
            rgbaColor.r = (rgbaColor0.r + rgbaColor1.r) / 2;
            rgbaColor.g = (rgbaColor0.g + rgbaColor1.g) / 2;
            rgbaColor.b = (rgbaColor0.b + rgbaColor1.b) / 2;
            rgbaColor.a = 255;
            break;
        case 3:
            rgbaColor.Set(255, 255, 255, 0);
            break;
    }
}

// line 1351
static void sAlphaFromExplicitAlphaBlock(const ExplicitAlphaBlock * ptBlock, const int32 nPixelX, const int32 nPixelY, RGBAColor & rgbaColor) {
    int32 nShift = nPixelX * 4;
    rgbaColor.a = ((ptBlock->wAlpha[nPixelY] & (0xf << nShift)) >> nShift) << 4;
}

// line 1368
static void sAlphaFromInterpAlphaBlock(const InterpAlphaBlock * ptBlock, const int32 nPixelNum, RGBAColor & rgbaColor) {
    uint64 dwInterpBits = *(const uint64 *)ptBlock;
    int32 nShift = nPixelNum * 3 + 16;
    uint32 dwCode = (uint32)(dwInterpBits >> nShift) & 7;

    if (ptBlock->bAlpha0 > ptBlock->bAlpha1) {
        switch (dwCode) {
            case 0: rgbaColor.a = ptBlock->bAlpha0; break;
            case 1: rgbaColor.a = ptBlock->bAlpha1; break;
            case 2: rgbaColor.a = (6 * ptBlock->bAlpha0 + 1 * ptBlock->bAlpha1 + 3) / 7; break;
            case 3: rgbaColor.a = (5 * ptBlock->bAlpha0 + 2 * ptBlock->bAlpha1 + 3) / 7; break;
            case 4: rgbaColor.a = (4 * ptBlock->bAlpha0 + 3 * ptBlock->bAlpha1 + 3) / 7; break;
            case 5: rgbaColor.a = (3 * ptBlock->bAlpha0 + 4 * ptBlock->bAlpha1 + 3) / 7; break;
            case 6: rgbaColor.a = (2 * ptBlock->bAlpha0 + 5 * ptBlock->bAlpha1 + 3) / 7; break;
            case 7: rgbaColor.a = (1 * ptBlock->bAlpha0 + 6 * ptBlock->bAlpha1 + 3) / 7; break;
        }
    } else {
        switch (dwCode) {
            case 0: rgbaColor.a = ptBlock->bAlpha0; break;
            case 1: rgbaColor.a = ptBlock->bAlpha1; break;
            case 2: rgbaColor.a = (4 * ptBlock->bAlpha0 + 1 * ptBlock->bAlpha1 + 2) / 5; break;
            case 3: rgbaColor.a = (3 * ptBlock->bAlpha0 + 2 * ptBlock->bAlpha1 + 2) / 5; break;
            case 4: rgbaColor.a = (2 * ptBlock->bAlpha0 + 3 * ptBlock->bAlpha1 + 2) / 5; break;
            case 5: rgbaColor.a = (1 * ptBlock->bAlpha0 + 4 * ptBlock->bAlpha1 + 2) / 5; break;
            case 6: rgbaColor.a = 0; break;
            case 7: rgbaColor.a = 255; break;
        }
    }
}

// line 1445
void sTextureDefinitionPixelLookup(const TextureDefinition * pdefTexture, const uint8 * ptr, const uint32 dwPitch, int32 u, int32 v,
                                   const int32 nMipLevelWidth, const int32 nMipLevelHeight, RGBAColor & rgbaColor) {
    u = Math::Pin(u, 0, nMipLevelWidth - 1);
    v = Math::Pin(v, 0, nMipLevelHeight - 1);

    if (!TextureFormatIsCompressed(pdefTexture->eTexFormat)) {
        int32 nBytesPerPixel = TextureFormatSize(pdefTexture->eTexFormat);
        Pixel32 * pPixel = (Pixel32 *)(ptr + v * dwPitch + u * nBytesPerPixel);
        rgbaColor.FromTexturePixel(pPixel, pdefTexture->eTexFormat);
    } else {
        int32 nBytesPerBlock = TextureFormatSize(pdefTexture->eTexFormat);
        int32 nBlockU = u / 4;
        int32 nBlockV = v / 4;
        int32 nPixelX = u & 3;
        int32 nPixelY = v & 3;
        int32 nPixelNum = nPixelY * 4 + nPixelX;

        switch (pdefTexture->eTexFormat) {
            case TEXTUREFORMAT_DXT5:
            case TEXTUREFORMAT_DXT5N: {
                DXT5Block * ptBlock = (DXT5Block *)(ptr + gptGFXAPI->GetTiledElementByteOffset(pdefTexture->tTexData.htex, nBlockU, nBlockV));
                sColorFromOpaqueDXTnColorBlock(&ptBlock->tColorBlock, nPixelNum, rgbaColor);
                sAlphaFromInterpAlphaBlock(&ptBlock->tAlphaBlock, nPixelNum, rgbaColor);
                break;
            }
            case TEXTUREFORMAT_DXT1: {
                DXTnColorBlock * ptBlock = (DXTnColorBlock *)(ptr + gptGFXAPI->GetTiledElementByteOffset(pdefTexture->tTexData.htex, nBlockU, nBlockV));
                sColorFromOpaqueDXTnColorBlock(ptBlock, nPixelNum, rgbaColor);
                break;
            }
            case TEXTUREFORMAT_DXT1_1BITALPHA: {
                DXTnColorBlock * ptBlock = (DXTnColorBlock *)(ptr + gptGFXAPI->GetTiledElementByteOffset(pdefTexture->tTexData.htex, nBlockU, nBlockV));
                sColorFrom1BitAlphaDXTnColorBlock(ptBlock, nPixelNum, rgbaColor);
                break;
            }
            case TEXTUREFORMAT_DXT3: {
                DXT3Block * ptBlock = (DXT3Block *)(ptr + gptGFXAPI->GetTiledElementByteOffset(pdefTexture->tTexData.htex, nBlockU, nBlockV));
                sColorFromOpaqueDXTnColorBlock(&ptBlock->tColorBlock, nPixelNum, rgbaColor);
                sAlphaFromExplicitAlphaBlock(&ptBlock->tAlphaBlock, nPixelX, nPixelY, rgbaColor);
                break;
            }
        }
    }
}

// line 1636
void TextureDefinitionPixelLookup(const SNO snoTexture, const float flU, const float flV, const BOOL fLinearFilter, RGBAColor & rgbaColor) {
    rgbaColor = crgbaWhite;

    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, snoTexture), 2))
        return;

    const TextureDefinition * pdefTexture = (const TextureDefinition *)gptGroupDefTexture->LockR(snoTexture, FALSE);
    if (!pdefTexture->tTexData.htex)
        sTextureDefinitionForceLoad(pdefTexture, NULL, FALSE, NULL, 0);

    float flAdjustedV = (pdefTexture->flConsoleLightmapVScale != 0.0f ? pdefTexture->flConsoleLightmapVScale : 1.0f) * flV;

    uint32 dwHeight;
    uint32 dwPitch = 0;
    const uint8 * ptr = (const uint8 *)TextureDefinitionLockTexture(snoTexture, dwPitch, dwHeight, 0, 2);

    int32 nMipLevelOffset;
    if (pdefTexture->dwRuntimeFlags & (1 << TEXTURE_DEF_IGNORE_MIPOFFSET_BIT))
        nMipLevelOffset = 0;
    else
        nMipLevelOffset = Prefs::GetVideoPrefs().GetTextureMipOffset();

    uint32 dwMipMapLevels = pdefTexture->dwMipMapLevelMax - pdefTexture->dwMipMapLevelMin;
    nMipLevelOffset = Math::Min(nMipLevelOffset, (int32)dwMipMapLevels);

    int32 nMipLevelWidth = pdefTexture->dwWidth >> nMipLevelOffset;
    int32 nMipLevelHeight = pdefTexture->dwHeight >> nMipLevelOffset;

    float fTexU = flU * nMipLevelWidth;
    float fTexV = flAdjustedV * nMipLevelHeight;
    fTexU = Math::PinWrap(fTexU, 0.0f, (float)nMipLevelWidth);
    fTexV = Math::PinWrap(fTexV, 0.0f, (float)nMipLevelHeight);
    int32 u = Math::FloatToInt(fTexU);
    int32 v = Math::FloatToInt(fTexV);

    if (fLinearFilter) {
        RGBAColor rgbaSamples[4];
        for (int32 i = 0; i < 4; i++) {
            int32 du = i & 1;
            int32 dv = i >> 1;
            sTextureDefinitionPixelLookup(pdefTexture, ptr, dwPitch, u + du, v + dv, nMipLevelWidth, nMipLevelHeight, rgbaSamples[i]);
        }

        float flFracU = Math::FloatFrac(fTexU);
        float flFracV = Math::FloatFrac(fTexV);
        Math::Lerp(rgbaSamples[0], rgbaSamples[1], flFracU, rgbaSamples[1]);
        Math::Lerp(rgbaSamples[2], rgbaSamples[3], flFracU, rgbaSamples[3]);
        Math::Lerp(rgbaSamples[1], rgbaSamples[3], flFracV, rgbaColor);
    } else {
        sTextureDefinitionPixelLookup(pdefTexture, ptr, dwPitch, u, v, nMipLevelWidth, nMipLevelHeight, rgbaColor);
    }

    TextureDefinitionUnlockTexture(snoTexture, (void *)ptr, 0);

    gptGroupDefTexture->UnlockR((void *&)pdefTexture);
}

// line 1739
static void sCachePurgeFunc(TexData & tTexData) {
    if (tTexData.htex) {
        if (gptGFXAPI)
            GfxTextureDispose(tTexData);
        tTexData.htex = NULL;
    }
}

// line 1757


static void sReplaceTextureWithMipMapLevelColor(uint32 dwLevel, void * ptBuf, uint32 dwSize, TextureFormat eTexFormat) {
    Pixel32 pColor;
    uint32 dwPixelSize;
    RGBAColor rgbaColor;

    switch (dwLevel) {
        case 0:
            rgbaColor = crgbaWhite;
            break;
        case 1:
            rgbaColor = crgbaRed;
            break;
        case 2:
            rgbaColor = crgbaOrange;
            break;
        case 3:
            rgbaColor = crgbaYellow;
            break;
        case 4:
            rgbaColor = crgbaGreen;
            break;
        case 5:
            rgbaColor.Set(0, 255, 255, 255);
            break;
        case 6:
            rgbaColor = crgbaBlue;
            break;
        default:
            rgbaColor = crgbaPurple;
            break;
    }

    if (TextureFormatIsCompressed(eTexFormat)) {
        DXTnColorBlock tColorBlock;
        tColorBlock.pColor0 = (Pixel16)rgbaColor.ToTexturePixel(TEXTUREFORMAT_RGB565);
        tColorBlock.pColor1 = tColorBlock.pColor0;
        tColorBlock.dwInterpBits = 0;

        switch (eTexFormat) {
            case TEXTUREFORMAT_DXT1:
            case TEXTUREFORMAT_DXT1_1BITALPHA: {
                int32 nBlocks = dwSize / sizeof(DXTnColorBlock);
                DXTnColorBlock * ptDest = (DXTnColorBlock *)ptBuf;
                for (int32 i = 0; i < nBlocks; i++)
                    ptDest[i] = tColorBlock;
                break;
            }
            case TEXTUREFORMAT_DXT3: {
                DXT3Block tBlock;
                tBlock.tAlphaBlock.wAlpha[0] = 0xffff;
                tBlock.tAlphaBlock.wAlpha[1] = 0xffff;
                tBlock.tAlphaBlock.wAlpha[2] = 0xffff;
                tBlock.tAlphaBlock.wAlpha[3] = 0xffff;
                tBlock.tColorBlock = tColorBlock;
                int32 nBlocks = dwSize / sizeof(DXT3Block);
                DXT3Block * ptDest = (DXT3Block *)ptBuf;
                for (int32 i = 0; i < nBlocks; i++)
                    ptDest[i] = tBlock;
                break;
            }
            case TEXTUREFORMAT_DXT5:
            case TEXTUREFORMAT_DXT5N: {
                DXT5Block tBlock;
                tBlock.tAlphaBlock.bAlpha0 = 255;
                tBlock.tAlphaBlock.bAlpha1 = 255;
                SigmaMemoryClear(tBlock.tAlphaBlock.bInterpBits, sizeof(tBlock.tAlphaBlock.bInterpBits));
                tBlock.tColorBlock = tColorBlock;
                int32 nBlocks = dwSize / sizeof(DXT5Block);
                DXT5Block * ptDest = (DXT5Block *)ptBuf;
                for (int32 i = 0; i < nBlocks; i++)
                    ptDest[i] = tBlock;
                break;
            }
        }
    } else {
        pColor = rgbaColor.ToTexturePixel(eTexFormat);
        dwPixelSize = TextureFormatSize(eTexFormat);
        switch (dwPixelSize) {
            case 4: {
                Pixel32 * pDest = (Pixel32 *)ptBuf;
                Pixel32 * pStop = (Pixel32 *)((uint8 *)ptBuf + dwSize);
                while (pDest < pStop)
                    *pDest++ = pColor;
                break;
            }
            case 2: {
                Pixel16 * pDest = (Pixel16 *)ptBuf;
                Pixel16 * pStop = (Pixel16 *)((uint8 *)ptBuf + dwSize);
                while (pDest < pStop)
                    *pDest++ = (Pixel16)pColor;
                break;
            }
            case 1: {
                Pixel8 * pDest = (Pixel8 *)ptBuf;
                Pixel8 * pStop = (Pixel8 *)((uint8 *)ptBuf + dwSize);
                while (pDest < pStop)
                    *pDest++ = (Pixel8)pColor;
                break;
            }
            default:
                DisplayInternalError(NULL, "", 1903, ERROR_INTERNAL_FATAL);
                ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
}

// line 1913
static TextureFormat sFindClosestSupportedTextureFormat(const TextureFormat eDesiredFormat, const uint32 dwTexCreateFlags) {
    TextureFormat eSupportedTextureFormat = eDesiredFormat;
    if (!GfxTextureFormatIsValid(eSupportedTextureFormat, dwTexCreateFlags)) {
        eSupportedTextureFormat = TextureFormatGetFallbackFormat(eSupportedTextureFormat);
        if (dwTexCreateFlags & 2) {
            while (eSupportedTextureFormat != TEXTUREFORMAT_ARGB8888) {
                if (GfxTextureFormatIsValid(eSupportedTextureFormat, dwTexCreateFlags))
                    return eSupportedTextureFormat;
                eSupportedTextureFormat = TextureFormatGetFallbackFormat(eSupportedTextureFormat);
            }
            DisplayInternalError(NULL, "", 1941, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        if (!GfxTextureFormatIsValid(eSupportedTextureFormat, dwTexCreateFlags)) {
            DisplayInternalError(NULL, "", 1952, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
    return eSupportedTextureFormat;
}

// line 2002
static void sLoadTexture(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset) {
    uint32 dwTexCreateFlags = 0;
    TextureParams tTexParams;
    sComputeTextureParams(pdefTexture, dwTexCreateFlags, tTexParams);

    const_cast<TextureDefinition *>(pdefTexture)->eUsage = RESOURCEUSAGE_IMMUTABLE;
    while (!GfxTextureCreate((TexData &)pdefTexture->tTexData, tTexParams.eSupportedTextureFormat, tTexParams.dwWidth, tTexParams.dwHeight, tTexParams.nMipLevels,
                             pdefTexture->eUsage, dwTexCreateFlags, 1, 0, 0, NULL, ""))
        gptGroupDefTexture->CacheManagerPurge(FALSE);

    SNOName snoname;
    gptGroupDefTexture->GetSNOName(pdefTexture, snoname);
    SNOElement * ptLocalSNOElement = NULL;
    if (!ptSNOElement) {
        ptLocalSNOElement = SNOMLFileOpenForRead(snoname);
        ptSNOElement = ptLocalSNOElement;
    }

    if (sgtAppGlobals.eDebugRenderMode == 5 || sgtAppGlobals.eDebugRenderMode == 6) {
        for (int32 nLevel = tTexParams.nMipLevelStart; nLevel < tTexParams.nMipLevelEnd; nLevel++) {
            uint32 dwMipWidth, dwMipHeight;
            TextureGetMipDimensions(pdefTexture->dwWidth, pdefTexture->dwHeight, nLevel, dwMipWidth, dwMipHeight);

            int32 nActualMipLevel = nLevel - tTexParams.nMipLevelStart;

            uint32 dwDestPitch;
            void * ptDest;
            GfxTextureLock(pdefTexture->tTexData, nActualMipLevel, ptDest, dwDestPitch, 0);

            uint32 dwSize = dwDestPitch * dwMipHeight;
            if (tTexParams.fCompressed)
                dwSize /= 4;
            sReplaceTextureWithMipMapLevelColor(nLevel, ptDest, dwSize, tTexParams.eSupportedTextureFormat);
            GfxTextureUnlock(pdefTexture->tTexData, nActualMipLevel);
        }
    } else {
        sLoadAllMipLevels(ptSNOElement, ptSNOData, dwSNODataDeltaOffset, pdefTexture);
    }

    if (ptLocalSNOElement)
        SNOFileClose(ptLocalSNOElement);
}

// line 2181
static void sLoadCubeMap(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset) {
    uint32 dwTexCreateFlags = 0;
    TextureParams tTexParams;
    sComputeTextureParams(pdefTexture, dwTexCreateFlags, tTexParams);

    const_cast<TextureDefinition *>(pdefTexture)->eUsage = RESOURCEUSAGE_IMMUTABLE;
    while (!GfxCubeMapCreate((TexData &)pdefTexture->tTexData, tTexParams.eSupportedTextureFormat, tTexParams.dwWidth, tTexParams.nMipLevels, pdefTexture->eUsage,
                             dwTexCreateFlags, 0, 0, NULL, ""))
        gptGroupDefTexture->CacheManagerPurge(FALSE);

    SNOName snoname;
    gptGroupDefTexture->GetSNOName(pdefTexture, snoname);
    SNOElement * ptLocalSNOElement = NULL;
    if (!ptSNOElement) {
        ptLocalSNOElement = SNOMLFileOpenForRead(snoname);
        ptSNOElement = ptLocalSNOElement;
    }

    if ((sgtAppGlobals.eDebugRenderMode == 5 || sgtAppGlobals.eDebugRenderMode == 6) && !tTexParams.fCompressed) {
        for (uint32 dwFaceIndex = 0; dwFaceIndex < NUM_CUBEMAPFACES; dwFaceIndex++) {
            for (int32 nLevel = tTexParams.nMipLevelStart; nLevel < tTexParams.nMipLevelEnd; nLevel++) {
                uint32 dwMipWidth, dwMipHeight;
                TextureGetMipDimensions(pdefTexture->dwWidth, pdefTexture->dwHeight, nLevel, dwMipWidth, dwMipHeight);

                int32 nActualMipLevel = nLevel - tTexParams.nMipLevelStart;

                uint32 dwDestPitch;
                void * ptDest;
                GfxCubeMapLock(pdefTexture->tTexData, (CubeMapFace)dwFaceIndex, nActualMipLevel, ptDest, dwDestPitch, 0);

                uint32 dwSize = dwDestPitch * dwMipHeight;
                if (tTexParams.fCompressed)
                    dwSize /= 4;
                sReplaceTextureWithMipMapLevelColor(nLevel, ptDest, dwSize, tTexParams.eSupportedTextureFormat);
                GfxCubeMapUnlock(pdefTexture->tTexData, (CubeMapFace)dwFaceIndex, nActualMipLevel);
            }
        }
    } else {
        sLoadAllMipLevels(ptSNOElement, ptSNOData, dwSNODataDeltaOffset, pdefTexture);
    }

    if (ptLocalSNOElement)
        SNOFileClose(ptLocalSNOElement);
}

// line 2355
static void sLoadVolumeTexture(const TextureDefinition * pdefTexture, SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset) {
    uint32 dwTexCreateFlags = 0;
    TextureParams tTexParams;
    sComputeTextureParams(pdefTexture, dwTexCreateFlags, tTexParams);

    const_cast<TextureDefinition *>(pdefTexture)->eUsage = RESOURCEUSAGE_IMMUTABLE;
    while (!GfxVolumeTexCreate((TexData &)pdefTexture->tTexData, tTexParams.eSupportedTextureFormat, tTexParams.dwWidth, tTexParams.dwHeight, tTexParams.dwDepth,
                               tTexParams.nMipLevels, pdefTexture->eUsage, dwTexCreateFlags, NULL, ""))
        gptGroupDefTexture->CacheManagerPurge(FALSE);

    SNOName snoname;
    gptGroupDefTexture->GetSNOName(pdefTexture, snoname);
    SNOElement * ptLocalSNOElement = NULL;
    if (!ptSNOElement) {
        ptLocalSNOElement = SNOMLFileOpenForRead(snoname);
        ptSNOElement = ptLocalSNOElement;
    }

    if ((sgtAppGlobals.eDebugRenderMode == 5 || sgtAppGlobals.eDebugRenderMode == 6) && !tTexParams.fCompressed) {
        for (int32 nLevel = tTexParams.nMipLevelStart; nLevel < tTexParams.nMipLevelEnd; nLevel++) {
            uint32 dwMipWidth, dwMipHeight, dwMipDepth;
            TextureGetMipDimensions(pdefTexture->dwWidth, pdefTexture->dwHeight, pdefTexture->dwDepth, nLevel, dwMipWidth, dwMipHeight, dwMipDepth);

            int32 nActualMipLevel = nLevel - tTexParams.nMipLevelStart;

            uint32 dwDestRowPitch;
            uint32 dwDestSlicePitch;
            void * ptDest;
            GfxVolumeTexLock(pdefTexture->tTexData, nActualMipLevel, ptDest, dwDestRowPitch, dwDestSlicePitch, 0);

            uint32 dwSize = dwDestSlicePitch * pdefTexture->dwDepth;
            sReplaceTextureWithMipMapLevelColor(nLevel, ptDest, dwSize, tTexParams.eSupportedTextureFormat);
            GfxVolumeTexUnlock(pdefTexture->tTexData, nActualMipLevel);
        }
    } else {
        sLoadAllMipLevels(ptSNOElement, ptSNOData, dwSNODataDeltaOffset, pdefTexture);
    }

    if (ptLocalSNOElement)
        SNOFileClose(ptLocalSNOElement);
}

// line 2542
static void sLoadAllMipLevels(SNOElement * ptSNOElement, void * ptSNOData, uint32 dwSNODataDeltaOffset, const TextureDefinition * pdefTexture) {
    gptGFXAPI->LoadAllMipLevels(ptSNOElement, ptSNOData, dwSNODataDeltaOffset, pdefTexture->tTexData.htex, pdefTexture->serTex[0][0]);
}

// line 2697
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex) {
    ScopedTextureDefinitionLockR pdefTexture(snoTexture);
    TextureGetFrameRectPixels(rectTexturePixels, pdefTexture, dwFrameIndex);
}

// line 2711
void TextureGetFrameRectUIScaledPixels(Rect2D & rectTexturePixels, const SNO snoTexture, const uint32 dwFrameIndex) {
    IRect2D rectTexturePixelsUIC;
    ScopedTextureDefinitionLockR pdefTexture(snoTexture);
    TextureGetFrameRectPixels(rectTexturePixelsUIC, pdefTexture, dwFrameIndex);
    // The DWARF puts rectUIC at function scope, but the original reuses its stack slot for the
    // unlock, so its lifetime ends before the lock's destructor runs.
    {
        Rect2D rectUIC;
        rectUIC.Set(rectTexturePixelsUIC);
        ConvertUICToPixelsDoNotSnap(rectUIC, rectTexturePixels);
    }
}

// line 2728
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex) {
    if (dwFrameIndex == (uint32)-1) {
        rectTexturePixels.Set(0, 0, pdefTexture->dwWidth, pdefTexture->dwHeight);
    } else {
        rectTexturePixels = pdefTexture->prectFrame[dwFrameIndex];
    }
}

// line 2752
void TextureGetFrameRectUVs(Rect2D & rectTextureUVs, const TextureDefinition * pdefTexture, const uint32 dwFrameIndex) {
    const TexFrame & tFrame = pdefTexture->ptFrame[dwFrameIndex];
    rectTextureUVs = Rect2D(tFrame.flU0, tFrame.flV0, tFrame.flU1, tFrame.flV1);
}

// line 2776
void TextureComputeFramePixelCoords(TextureDefinition * pdefTexture) {
    pdefTexture->prectFrame = NULL;

    if (!pdefTexture->dwFrameCount)
        return;

    if (pdefTexture->serFrame.dwSize != pdefTexture->dwFrameCount * sizeof(TexFrame)) {
        sTraceBinary((const uint8 *)pdefTexture, sizeof(TextureDefinition));
        DisplayInternalError(NULL, "", 2792, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    pdefTexture->prectFrame = (IRect2D *)SigmaMemoryNew(pdefTexture->dwFrameCount * sizeof(IRect2D), 4, NULL, FALSE);

    for (int32 i = 0; i < (int32)pdefTexture->dwFrameCount; i++) {
        TexFrame & tFrame = pdefTexture->ptFrame[i];
        pdefTexture->prectFrame[i].Set(Math::FloatToInt(tFrame.flU0 * pdefTexture->dwWidth), Math::FloatToInt(tFrame.flV0 * pdefTexture->dwHeight),
                                       Math::FloatToInt(tFrame.flU1 * pdefTexture->dwWidth),
                                       Math::FloatToInt(tFrame.flV1 * pdefTexture->dwHeight));
    }
}

// line 2963
void TextureGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer,
                           uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);

    TextureDefinition * pdef = (TextureDefinition *)lpDefinition;
    SigmaMemoryClear(&pdef->tTexData, sizeof(TexData));
    if (gptGfxData)
        sTextureDefinitionForceLoad(pdef, ptSNOElement, TRUE, NULL, 0);
}

// line 2994
uint32 TextureGroupDef::GetContiguousReadSize(SNOElement * ptSNOElement) {
    TextureDefinition tTextureDef;
    ReadDefinitionHeader(ptSNOElement, &tTextureDef);

    uint32 dwContiguousSize = tTextureDef.serTex[0][0].dwOffset;
    return dwContiguousSize;
}
