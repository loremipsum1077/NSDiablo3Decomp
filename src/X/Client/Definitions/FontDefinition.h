// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/FontDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;

enum {
    FONT_MAX_CHARS = 4096,
    FONT_MAX_PAGES = 64,
    MAX_FONT_NAME = 64,
};

enum FontHintingMode {
    FHM_NATIVE_HINTING = 0,
    FHM_NO_HINTING = 1,
    FHM_AUTO_HINTING = 2,
};

enum FontHintingStyle {
    FHS_NORMAL = 0,
    FHS_LIGHT = 1,
    FHS_LCD = 2,
    FHS_LCD_VERTICAL = 3,
};

enum FontConsoleResolution {
    FCR_SD_BIT = 0,
    FCR_HD_BIT = 1,
    FCR_PAL_BIT = 2,
    FCR_1080P_BIT = 3,
    FCR_2160P_BIT = 4,
    FCR_900P_BIT = 5,
};

typedef int FontF26Dot6;

inline int32 FontF26Dot6_To_Int(const FontF26Dot6 val) {  // line 71
    // Font.cpp's inlined copies round through Math::FloatRoundToInt (line 74).
    return Math::FloatRoundToInt((float)val / 64.0f);
}

inline FontF26Dot6 Int_ToFontF26Dot6(const int32 val) {  // line 77
    return val << 6;
}

inline float FontF26Dot6_To_Float(const FontF26Dot6 val) {  // line 83
    return (float)val / 64.0f;
}

inline FontF26Dot6 Float_To_FontF26Dot6(const float val) {  // line 89
    return Math::FloatRoundToInt(val * 64.0f);
}

// Line 95 (parameters val1 / val2 on 96 / 97). The body is not in the DWARF; written from the
// inlined copies in Font.cpp (sign-extending 64-bit multiply, then an arithmetic shift by 6).
inline FontF26Dot6 FontF26Dot6Multiply(const FontF26Dot6 val1, const FontF26Dot6 val2) {
    return (FontF26Dot6)(((int64)val1 * val2) >> 6);
}

class FontChar {  // size 0x18
public:
    SNO snoTex;  // 0x00
    int32 nGlyphFrame;  // 0x04
    int16 nBearingXComp;  // 0x08
    int16 nBearingYComp;  // 0x0a
    int16 nHeightComp;  // 0x0c
    int16 nAdvanceComp;  // 0x0e
    int16 nTextureIndex;  // 0x10
    uint16 uChar;  // 0x12
    uint32 dwPad2;  // 0x14
};
static_assert(sizeof(FontChar) == 0x18, "FontChar");

struct FontConsoleKerning {  // size 0x10
    uint32 uPrevChar;  // 0x00
    uint32 uChar;  // 0x04
    int32 nKerningX;  // 0x08
    int32 nKerningY;  // 0x0c
};
static_assert(sizeof(FontConsoleKerning) == 0x10, "FontConsoleKerning");

struct SequentialCharInfo {  // size 0x8
    uint32 dwMinChar;  // 0x00
    uint32 dwMaxChar;  // 0x04
};
static_assert(sizeof(SequentialCharInfo) == 0x8, "SequentialCharInfo");

struct FontPointSizeData {  // size 0x58
    FontF26Dot6 nFontHeight;  // 0x00
    FontF26Dot6 nFontWidth;  // 0x04
    FontF26Dot6 nFontAscender;  // 0x08
    FontF26Dot6 nMaxAscender;  // 0x0c
    FontF26Dot6 nFontDescender;  // 0x10
    FontF26Dot6 nLineGap;  // 0x14
    FontF26Dot6 nAdvanceY;  // 0x18
    uint32 dwPad;  // 0x1c
    uint32 dwResolutionFlags;  // 0x20
    uint32 dwNumChars;  // 0x24
    union {  // size 0x8
        SequentialCharInfo tSequentialCharInfo;  // 0x00
        SerializeData serCharTable;  // 0x00
    };  // 0x28
    XPtr64<FontChar> ptCharTable;  // 0x30
    uint32 dwNumKernings;  // 0x38
    uint32 dwPad2;  // 0x3c
    SerializeData serKerningTable;  // 0x40
    XPtr64<FontConsoleKerning> ptKerningTable;  // 0x48
    FontF26Dot6 nIconHeight;  // 0x50
    float flPointSize;  // 0x54
    IVector2D GetKerning(const FontChar & tCharPrev, const FontChar & tChar) const;
};
static_assert(sizeof(FontPointSizeData) == 0x58, "FontPointSizeData");

struct FontConsoleTextureFrame {  // size 0x10
    float flU0;  // 0x00
    float flV0;  // 0x04
    float flU1;  // 0x08
    float flV1;  // 0x0c
};
static_assert(sizeof(FontConsoleTextureFrame) == 0x10, "FontConsoleTextureFrame");

struct FontConsoleTexture {  // size 0x28
    uint32 dwResolutionFlags;  // 0x00
    TextureFormat eTexFormat;  // 0x04
    uint32 dwWidth;  // 0x08
    uint32 dwHeight;  // 0x0c
    SerializeData serTextureData;  // 0x10
    SNO tTexture;  // 0x18
    uint32 dwNumFrames;  // 0x1c
    SerializeData serFrameData;  // 0x20
};
static_assert(sizeof(FontConsoleTexture) == 0x28, "FontConsoleTexture");

struct FontConsolePointSize {  // size 0x4
    int32 nPointSize;  // 0x00
};
static_assert(sizeof(FontConsolePointSize) == 0x4, "FontConsolePointSize");

struct FontDefinition {  // size 0xd8
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    CHAR szFontFace[64];  // 0x10
    CHAR szOriginalFontFilename[64];  // 0x50
    BOOL bSmallCaps;  // 0x90
    FontHintingMode eHintingMode;  // 0x94
    FontHintingStyle eHintingStyle;  // 0x98
    int32 nSizeAdjustment;  // 0x9c
    float flLineGapAdjustment;  // 0xa0
    float flSpaceScalar;  // 0xa4
    SerializeData serTextureTable;  // 0xa8
    XPtr64<FontConsoleTexture> ptTextureTable;  // 0xb0
    uint32 dwNumTextures;  // 0xb8
    uint32 dwNumPointSizes;  // 0xbc
    SerializeData serPointSizeTable;  // 0xc0
    XPtr64<FontPointSizeData> ptPointSizeTable;  // 0xc8
    uint32 dwCustomDataOffset;  // 0xd0
    uint32 dwPad2;  // 0xd4
    FontDefinition() {}  // line 242
    const FontChar * LookupCharacter(FontPointSizeData * ptPointSize, uint32 uChar) const;
    FontPointSizeData * GetPointSizeData(int32 nFontSize) const;
    int32 GetNextSmallestFontSize(int32 nFontSize) const;
    float GetPointSizeForDisplayResolution(int32, uint32, uint32) const;
};
static_assert(sizeof(FontDefinition) == 0xd8, "FontDefinition");

class FontGroupDef : public GroupDefinition {  // size 0xa0, line 313
public:
    FontGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 318; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 321
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 323
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
    virtual uint32 GetContiguousReadSize(SNOElement * ptSNOElement);  // vtable slot 27
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(FontGroupDef) == 0xa0, "FontGroupDef");

extern FontGroupDef * gptGroupDefFont;  // FontDefinition.cpp:168

// Font.cpp's scoped locks leave no unlock call behind.
template <> inline void DefinitionUnlockR<FontDefinition>(const FontDefinition *) {}

typedef ScopedDefinitionLockR<FontDefinition> ScopedFontDefinitionLockR;

template <> inline const FontDefinition * DefinitionLockR<FontDefinition>(SNO sno) {  // line 377
    // LockRFast takes no reference, so the unlock does nothing (Font.cpp calls only LockRFast).
    return (const FontDefinition *)gptGroupDefFont->LockRFast(sno);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line    71  int32 FontF26Dot6_To_Int(const FontF26Dot6 val)  [inlined into 7 functions]
//   line    77  FontF26Dot6 Int_ToFontF26Dot6(const int32 val)  [inlined into 1 functions]
//   line    83  float FontF26Dot6_To_Float(const FontF26Dot6 val)  [inlined into 1 functions]
//   line    89  FontF26Dot6 Float_To_FontF26Dot6(const float val)  [inlined into 6 functions]
//   line    95  FontF26Dot6 FontF26Dot6Multiply(const FontF26Dot6 val1, const FontF26Dot6 val2)  [inlined into 3 functions]
//   line   242  FontDefinition::FontDefinition()  [inlined into 1 functions]
//   line   313  FontGroupDef::~FontGroupDef()  [0x71001acfe0]
//   line   318  void * FontGroupDef::MemAlloc(size_t nSize, uint32 dwAlign) const  [0x71001ad010 (+1 more)]
//   line   318  void FontGroupDef::MemFree(void *& ptr) const  [0x71001ad060 (+1 more)]
//   line   321  BOOL FontGroupDef::CanReferenceOtherSNOs() const  [0x71001ad0f0]
//   line   323  SNOPackType FontGroupDef::GetSNOPackDestination(const void *) const  [0x71001ad0e0]
//   line   377  const FontDefinition * DefinitionLockR<FontDefinition>(SNO sno)  [inlined into 7 functions]
