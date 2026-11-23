// Original file: X/Client/Definitions/FontDefinition.cpp

#include "FontDefinition.h"

// For the header statics the original initializes in this file's static initializer: blz::ignore
// (tuple.h) and cgplaceNull (WorldPlace.h). The GfxVB.h normals come in through GfxAPI.h.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/WorldPlace.h"
#include "TextureDefinition.h"
#include "../Gfx/GfxAPI.h"
#include "../Gfx/GfxInternal.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/SNO/SymbolicValue.cpp:5440 and :5449; no adopted header declares them yet.
extern const SymbolicEntry gFontHintingModes[4];
extern const SymbolicEntry gFontHintingStyles[5];

// Defined in X/Client/Gfx/Gfx.cpp:1854 (Gfx.h is not adopted).
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);

// Defined in X/Client/UserInterface/UIUtils.cpp:419 (UIUtils.h is not adopted).
float UIGetBaseResolutionScaleFrac();

enum {
    FONT_COMMON_SNO_VERSION = 50,
    FONT_XML_VERSION = 2,
    FONT_PC_SNO_VERSION = 50,
    FONT_CONSOLE_SNO_VERSION = 68,
    FONT_AXE_SNO_VERSION = 55,
    FONT_XBOXONE_SNO_VERSION = 71,
    FONT_PS4_SNO_VERSION = 72,
    FONT_NX64_SNO_VERSION = 72,
};

static const TypeDescriptorField FontConsoleTexture_Fields[9] = {  // line 60
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleTexture, dwResolutionFlags), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleTexture, eTexFormat), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleTexture, dwWidth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleTexture, dwHeight), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FontConsoleTexture, serTextureData), NULL, 0),
    TypeDescriptorField("", DT_ENUM, offsetof(FontConsoleTexture, tTexture), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleTexture, dwNumFrames), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FontConsoleTexture, serFrameData), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FontConsoleTexture), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FontConsoleTexture("FontConsoleTexture", 0, FontConsoleTexture_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 71
const XTypeDescriptor * DT_FONTCONSOLETEXTURE = &TypeDescriptor_FontConsoleTexture;

static const TypeDescriptorField FontChar_Fields[10] = {  // line 73
    TypeDescriptorField("", DT_ENUM, offsetof(FontChar, snoTex), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(FontChar, nGlyphFrame), NULL, 0),
    TypeDescriptorField("", DT_WORD, offsetof(FontChar, nBearingXComp), NULL, 0x700000),
    TypeDescriptorField("", DT_WORD, offsetof(FontChar, nBearingYComp), NULL, 0x700000),
    TypeDescriptorField("", DT_WORD, offsetof(FontChar, nHeightComp), NULL, 0x700000),
    TypeDescriptorField("", DT_WORD, offsetof(FontChar, nAdvanceComp), NULL, 0x700000),
    TypeDescriptorField("", DT_WORD, offsetof(FontChar, nTextureIndex), NULL, 0x700000),
    TypeDescriptorField("", DT_WORD, offsetof(FontChar, uChar), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(FontChar, dwPad2), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FontChar), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FontChar("FontChar", 0, FontChar_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 91
const XTypeDescriptor * DT_FONTCHAR = &TypeDescriptor_FontChar;

static const TypeDescriptorField FontConsoleKerning_Fields[5] = {  // line 93
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleKerning, uPrevChar), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleKerning, uChar), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleKerning, nKerningX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontConsoleKerning, nKerningY), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FontConsoleKerning), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FontConsoleKerning("FontConsoleKerning", 0, FontConsoleKerning_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 98
const XTypeDescriptor * DT_FONTCONSOLEKERNING = &TypeDescriptor_FontConsoleKerning;

static const TypeDescriptorField FontPointSizeData_Fields[19] = {  // line 100
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nFontHeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nFontWidth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nFontAscender), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nMaxAscender), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nFontDescender), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nLineGap), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nAdvanceY), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, dwPad), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, dwResolutionFlags), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, dwNumChars), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FontPointSizeData, serCharTable), NULL, 0x700000),
    TypeDescriptorField("", DT_INT64, offsetof(FontPointSizeData, ptCharTable), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, dwNumKernings), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, dwPad2), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FontPointSizeData, serKerningTable), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FONTCONSOLEKERNING, offsetof(FontPointSizeData, ptKerningTable), 0x700020,
                                     (int32)offsetof(FontPointSizeData, serKerningTable) - (int32)offsetof(FontPointSizeData, ptKerningTable)),
    TypeDescriptorField("", DT_INT, offsetof(FontPointSizeData, nIconHeight), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(FontPointSizeData, flPointSize), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FontPointSizeData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FontPointSizeData("FontPointSizeData", 0, FontPointSizeData_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 124
const XTypeDescriptor * DT_FONTPOINTSIZEDATA = &TypeDescriptor_FontPointSizeData;

static const TypeDescriptorField FontConsolePointSize_Fields[2] = {  // line 126
    TypeDescriptorField("", DT_INT, offsetof(FontConsolePointSize, nPointSize), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FontConsolePointSize), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FontConsolePointSize("FontConsolePointSize", 0, FontConsolePointSize_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 128
const XTypeDescriptor * DT_FONTCONSOLEPOINTSIZE = &TypeDescriptor_FontConsolePointSize;

static const TypeDescriptorField FontDefinition_Fields[18] = {  // line 130
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, dwFlags), NULL, 0x80001),
    CharArrayTypeDescriptorField("", offsetof(FontDefinition, szFontFace), "", 1, MAX_FONT_NAME, -1),
    CharArrayTypeDescriptorField("", offsetof(FontDefinition, szOriginalFontFilename), "", 1, MAX_FONT_NAME, -1),
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, bSmallCaps), NULL, 1),
    EnumTypeDescriptorField("", offsetof(FontDefinition, eHintingMode), NULL, gFontHintingModes),
    EnumTypeDescriptorField("", offsetof(FontDefinition, eHintingStyle), NULL, gFontHintingStyles),
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, nSizeAdjustment), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(FontDefinition, flLineGapAdjustment), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(FontDefinition, flSpaceScalar), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FontDefinition, serTextureTable), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FONTCONSOLETEXTURE, offsetof(FontDefinition, ptTextureTable), 0x700020,
                                     (int32)offsetof(FontDefinition, serTextureTable) - (int32)offsetof(FontDefinition, ptTextureTable)),
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, dwNumTextures), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, dwNumPointSizes), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FontDefinition, serPointSizeTable), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FONTPOINTSIZEDATA, offsetof(FontDefinition, ptPointSizeTable), 0x700020,
                                     (int32)offsetof(FontDefinition, serPointSizeTable) - (int32)offsetof(FontDefinition, ptPointSizeTable)),
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, dwCustomDataOffset), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(FontDefinition, dwPad2), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FontDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FontDefinition("Font", FONT_XML_VERSION, FontDefinition_Fields, 18, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 153
const XTypeDescriptor * DT_FONT = &TypeDescriptor_FontDefinition;

FontGroupDef * gptGroupDefFont;  // line 168
static BOOL gbFontPointSizeFallback = TRUE;  // line 171

// line 176
FontGroupDef::FontGroupDef()
    : GroupDefinition("Font", FONT_SNO,
                      SNOVersions(FONT_PC_SNO_VERSION, 57, FONT_XBOXONE_SNO_VERSION, FONT_PS4_SNO_VERSION, 73),
                      SNODATATYPE_TITLE, sizeof(FontDefinition), DT_FONT, 0x2011, 0x1b) {
}

// line 197
uint32 FontGroupDef::GetMaxCount() {
    return 64;
}

// line 395
BOOL FontGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 401, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

static void sFontConsoleDefinitionForceLoad(const FontDefinition * ptDefFont, SNOElement * ptSNOElement);

// line 410
void FontGroupDef::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer,
                        uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const {
    GroupDefinition::Read(tHeader, lpDefinition, ptSNOElement, fContiguousBuffer, dwContiguousSize, ptMemoryInterface);

    FontDefinition * ptFontDef = (FontDefinition *)lpDefinition;
    sFontConsoleDefinitionForceLoad(ptFontDef, ptSNOElement);

    for (uint32 dwPointSizeIndex = 0; dwPointSizeIndex < ptFontDef->dwNumPointSizes; dwPointSizeIndex++) {
        FontPointSizeData * ptPointSizeData = &ptFontDef->ptPointSizeTable[dwPointSizeIndex];
        const float cflGapUIC = ptFontDef->flLineGapAdjustment * 4.0f * UIGetBaseResolutionScaleFrac();
        ptPointSizeData->nLineGap = Float_To_FontF26Dot6(cflGapUIC);
        ptPointSizeData->nAdvanceY = ptPointSizeData->nFontAscender + ptPointSizeData->nLineGap - ptPointSizeData->nFontDescender;
    }
}

// line 452
uint32 FontGroupDef::GetContiguousReadSize(SNOElement * ptSNOElement) {
    FontDefinition tFontDef;
    SNOFileRead(ptSNOElement, 0, sizeof(FontDefinition), &tFontDef);

    uint32 dwContiguousSize = tFontDef.dwCustomDataOffset;
    return dwContiguousSize;
}

// line 476
static uint32 sFontConsoleGetResolutionFlags() {
    const uint32 dw2160PDisplayHeight = 2160;
    const uint32 dw1080PDisplayHeight = 1080;
    const uint32 dw900PDisplayHeight = 900;
    const uint32 dwHDDisplayHeight = 720;
    const uint32 dwPALDisplayHeight = 576;

    uint32 dwDisplayWidth;
    uint32 dwDisplayHeight;
    GfxGetCurrentDimensions(dwDisplayWidth, dwDisplayHeight);

    uint32 dwResolutionFlags;
    if (dwDisplayHeight >= dw2160PDisplayHeight)
        dwResolutionFlags = 1 << FCR_2160P_BIT;
    else if (dwDisplayHeight >= dw1080PDisplayHeight)
        dwResolutionFlags = 1 << FCR_1080P_BIT;
    else if (dwDisplayHeight >= dw900PDisplayHeight)
        dwResolutionFlags = 1 << FCR_900P_BIT;
    else if (dwDisplayHeight >= dwHDDisplayHeight)
        dwResolutionFlags = 1 << FCR_HD_BIT;
    else if (dwDisplayHeight == dwPALDisplayHeight)
        dwResolutionFlags = 1 << FCR_PAL_BIT;
    else
        dwResolutionFlags = 1 << FCR_SD_BIT;

    return dwResolutionFlags;
}

// line 525
static void sFontConsoleDefinitionForceLoad(const FontDefinition * ptDefFont, SNOElement * ptSNOElement) {
    if (gptGfxData == NULL)
        return;

    uint32 dwResolutionFlags = 0xFFFFFFFF;

    for (uint32 dwTextureIndex = 0; dwTextureIndex < ptDefFont->dwNumTextures; dwTextureIndex++) {
        FontConsoleTexture & tTexture = ptDefFont->ptTextureTable[dwTextureIndex];
        if (tTexture.tTexture == SNO_NONE && (tTexture.dwResolutionFlags & dwResolutionFlags)) {
            TextureFormat eTexFormat = tTexture.eTexFormat;
            CRefString sDebugTextureName;
            SNO snoFontTex = TextureDefinitionCreateTexture(sDebugTextureName.str(), eTexFormat, eTexFormat, tTexture.dwWidth, tTexture.dwHeight, 0, 0, 1, 0, 0);

            ScopedTextureDefinitionLockW pdefTexture(snoFontTex);
            gptGFXAPI->LoadAllMipLevels(ptSNOElement, NULL, 0, pdefTexture->tTexData.htex, tTexture.serTextureData);

            FontConsoleTextureFrame * ptFrames = (FontConsoleTextureFrame *)SigmaMemoryNew(tTexture.dwNumFrames * sizeof(FontConsoleTextureFrame), 4, NULL, FALSE);
            SNOFileRead(ptSNOElement, tTexture.serFrameData, ptFrames);

            pdefTexture->dwFrameCount = tTexture.dwNumFrames;
            pdefTexture->serFrame.dwSize = pdefTexture->dwFrameCount * sizeof(TexFrame);
            pdefTexture->ptFrame = (TexFrame *)gptGroupDefFont->MemAlloc(pdefTexture->serFrame.dwSize, 4);
            for (uint32 dwFrameIndex = 0; dwFrameIndex < tTexture.dwNumFrames; dwFrameIndex++) {
                pdefTexture->ptFrame[dwFrameIndex].hImageHandle = 0;
                pdefTexture->ptFrame[dwFrameIndex].flU0 = ptFrames[dwFrameIndex].flU0;
                pdefTexture->ptFrame[dwFrameIndex].flV0 = ptFrames[dwFrameIndex].flV0;
                pdefTexture->ptFrame[dwFrameIndex].flU1 = ptFrames[dwFrameIndex].flU1;
                pdefTexture->ptFrame[dwFrameIndex].flV1 = ptFrames[dwFrameIndex].flV1;
            }

            TextureComputeFramePixelCoords(pdefTexture);
            SigmaMemoryFree(ptFrames, NULL);
            tTexture.tTexture = snoFontTex;
        } else {
            tTexture.tTexture = SNO_NONE;
        }
    }

    for (uint32 dwPointSizeIndex = 0; dwPointSizeIndex < ptDefFont->dwNumPointSizes; dwPointSizeIndex++) {
        FontPointSizeData & tPointSizeData = ptDefFont->ptPointSizeTable[dwPointSizeIndex];
        if (!(tPointSizeData.dwResolutionFlags & dwResolutionFlags)) {
            tPointSizeData.ptCharTable = NULL;
            continue;
        }

        FontChar * ptCharTable = (FontChar *)gptGroupDefFont->MemAlloc(tPointSizeData.serCharTable.dwSize, 4);
        SNOFileRead(ptSNOElement, tPointSizeData.serCharTable, ptCharTable);
        tPointSizeData.ptCharTable = ptCharTable;

        for (uint32 dwCharIndex = 0; dwCharIndex < tPointSizeData.dwNumChars; dwCharIndex++) {
            if (tPointSizeData.ptCharTable[dwCharIndex].snoTex != INVALID_SNO) {
                // Invented local: the DWARF has none here, but without it the madd/ldr register
                // assignment differs (4 instructions).
                int32 nTextureIndex = tPointSizeData.ptCharTable[dwCharIndex].nTextureIndex;
                tPointSizeData.ptCharTable[dwCharIndex].snoTex = ptDefFont->ptTextureTable[nTextureIndex].tTexture;
            }
        }
    }
}

// line 620
static float sCalculateFontSize(int32 nReferenceSize) {
    return UIGetBaseResolutionScaleFrac() * (float)nReferenceSize;
}

// line 628
void FontGroupDef::GetDefaultData(void * ptDefaultDef) const {
    FontDefinition * pdefFont = (FontDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdefFont, sizeof(FontDefinition));
    pdefFont->eHintingMode = FHM_NATIVE_HINTING;
    pdefFont->eHintingStyle = FHS_NORMAL;
}

// line 664
void FontGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    FontDefinition * pdefFont = (FontDefinition *)lpDefinition;
    uint32 dwResolutionFlags = 0xFFFFFFFF;

    for (uint32 i = 0; i < pdefFont->dwNumPointSizes; i++) {
        FontPointSizeData & tPointSizeData = pdefFont->ptPointSizeTable[i];
        if (!(tPointSizeData.dwResolutionFlags & dwResolutionFlags))
            continue;

        uint32 dwMinChar = 0;
        uint32 dwMaxChar = 0;
        tPointSizeData.tSequentialCharInfo.dwMinChar = 0;
        tPointSizeData.tSequentialCharInfo.dwMaxChar = 0;
        if (tPointSizeData.dwNumChars > 2) {
            FontChar * ptCharTable = tPointSizeData.ptCharTable;
            dwMinChar = ptCharTable[1].uChar;
            dwMaxChar = dwMinChar;
            for (uint32 uChar = 2; uChar < tPointSizeData.dwNumChars; uChar++) {
                if (ptCharTable[uChar].uChar != dwMinChar + uChar - 1)
                    break;
                dwMaxChar = ptCharTable[uChar].uChar;
            }
            tPointSizeData.tSequentialCharInfo.dwMinChar = dwMinChar;
            tPointSizeData.tSequentialCharInfo.dwMaxChar = dwMaxChar;
        }
    }
}

// line 744
void FontGroupDef::Delete(void * lpDefinition) const {
    FontDefinition * pdefFont = (FontDefinition *)lpDefinition;

    for (uint32 dwTextureIndex = 0; dwTextureIndex < pdefFont->dwNumTextures; dwTextureIndex++)
        gptGroupDefTexture->Purge(pdefFont->ptTextureTable[dwTextureIndex].tTexture, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);

    for (uint32 dwPointSizeIndex = 0; dwPointSizeIndex < pdefFont->dwNumPointSizes; dwPointSizeIndex++) {
        FontPointSizeData * ptPointSize = &pdefFont->ptPointSizeTable[dwPointSizeIndex];
        if (ptPointSize->ptCharTable) {
            void * ptCharTable = ptPointSize->ptCharTable;
            gptGroupDefFont->MemFree(ptCharTable);
            ptPointSize->ptCharTable = NULL;
        }
    }

    GroupDefinition::Delete(lpDefinition);
}

// line 820
IVector2D FontPointSizeData::GetKerning(const FontChar & tCharPrev, const FontChar & tChar) const {
    if (dwNumKernings) {
        uint32 uPrevChar = tCharPrev.uChar;
        uint32 uChar = tChar.uChar;
        int32 nMin = 0;
        int32 nMax = dwNumKernings;
        while (nMax > nMin) {
            int nMid = (nMin + nMax) / 2;
            if (ptKerningTable[nMid].uPrevChar < uPrevChar || (ptKerningTable[nMid].uPrevChar == uPrevChar && ptKerningTable[nMid].uChar < uChar))
                nMin = nMid + 1;
            else
                nMax = nMid;
        }

        if (nMax == nMin && ptKerningTable[nMin].uPrevChar == uPrevChar && ptKerningTable[nMin].uChar == uChar)
            return IVector2D(ptKerningTable[nMin].nKerningX, ptKerningTable[nMin].nKerningY);
    }

    return IVector2D(0, 0);
}

// line 894
const FontChar * FontDefinition::LookupCharacter(FontPointSizeData * ptPointSize, uint32 uChar) const {
    static uint32 uHistory[5];
    static int32 nHistoryIndex;

    if (ptPointSize == NULL || ptPointSize->dwNumChars == 0)
        return NULL;

    int32 nMin = 0;
    int32 nMax = ptPointSize->dwNumChars;
    if (ptPointSize->tSequentialCharInfo.dwMinChar) {
        if (uChar >= ptPointSize->tSequentialCharInfo.dwMinChar && uChar <= ptPointSize->tSequentialCharInfo.dwMaxChar)
            return &ptPointSize->ptCharTable[uChar - ptPointSize->tSequentialCharInfo.dwMinChar + 1];
        if (ptPointSize->ptCharTable[0].uChar == uChar)
            return &ptPointSize->ptCharTable[0];
        nMin = ptPointSize->tSequentialCharInfo.dwMaxChar - ptPointSize->tSequentialCharInfo.dwMinChar + 2;
    }

    while (nMax > nMin) {
        int nMid = (nMin + nMax) / 2;
        if (ptPointSize->ptCharTable[nMid].uChar < uChar)
            nMin = nMid + 1;
        else
            nMax = nMid;
    }

    if (nMax == nMin && ptPointSize->ptCharTable[nMin].uChar == uChar)
        return &ptPointSize->ptCharTable[nMin];

    BOOL bFound = FALSE;
    for (int i = 4; i >= 0; i--) {
        if (uHistory[i] == uChar) {
            bFound = TRUE;
            break;
        }
    }
    if (!bFound) {
        uHistory[nHistoryIndex] = uChar;
        nHistoryIndex = (nHistoryIndex + 1) % 5;
    }

    return NULL;
}

// line 1179
FontPointSizeData * FontDefinition::GetPointSizeData(int32 nFontSize) const {
    uint32 dwResolutionFlags = sFontConsoleGetResolutionFlags();
    float flPointSize = sCalculateFontSize(nFontSize + nSizeAdjustment);
    FontPointSizeData * ptPrevPointSizeData = NULL;

    for (uint32 dwPointSizeIndex = 0; dwPointSizeIndex < dwNumPointSizes; dwPointSizeIndex++) {
        FontPointSizeData * ptPointSizeData = &ptPointSizeTable[dwPointSizeIndex];
        if (ptPointSizeData->dwResolutionFlags & dwResolutionFlags) {
            if (ptPointSizeData->flPointSize <= flPointSize + 0.000001f && ptPointSizeData->flPointSize >= flPointSize - 0.000001f)
                return ptPointSizeData;

            ptPrevPointSizeData = ptPointSizeData;
            if (ptPointSizeData->flPointSize >= flPointSize + 0.000001f)
                break;
        }
    }

    return gbFontPointSizeFallback ? ptPrevPointSizeData : NULL;
}

// line 1233
int32 FontDefinition::GetNextSmallestFontSize(int32 nFontSize) const {
    uint32 dwResolutionFlags = sFontConsoleGetResolutionFlags();
    float flPointSize = sCalculateFontSize(nFontSize + nSizeAdjustment);

    for (uint32 dwPointSizeIndex = 0; dwPointSizeIndex < dwNumPointSizes; dwPointSizeIndex++) {
        if ((ptPointSizeTable[dwPointSizeIndex].dwResolutionFlags & dwResolutionFlags) &&
            ptPointSizeTable[dwPointSizeIndex].flPointSize < flPointSize + 0.000001f &&
            (dwPointSizeIndex + 1 >= dwNumPointSizes || ptPointSizeTable[dwPointSizeIndex + 1].flPointSize >= flPointSize - 0.000001f)) {
            int nSmallerSize = Math::FloatRoundToInt(ptPointSizeTable[dwPointSizeIndex].flPointSize / UIGetBaseResolutionScaleFrac());
            return nSmallerSize - nSizeAdjustment;
        }
    }

    return 0;
}

// line 1300. The linker stripped this function (its DWARF has low_pc 0, size 0x10), so its bytes
// cannot be checked; the body follows from its name and parameter. Its store keeps
// gbFontPointSizeFallback a real variable, which GetPointSizeData depends on.
void FontEnablePointSizeFallback(bool bEnable) {
    gbFontPointSizeFallback = bEnable;
}

// line 1305. Also stripped by the linker (size 0xc).
BOOL IsFontPointSizeFallbackEnabled() {
    return gbFontPointSizeFallback;
}
