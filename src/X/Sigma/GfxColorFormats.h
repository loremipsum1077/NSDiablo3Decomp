#pragma once

#include "Sigma.h"

struct RGBAColor;
struct RGBAColorValue;

enum {
    TEXTUREFORMAT_UNKNOWN = -1,
    TEXTUREFORMAT_ARGB8888 = 0,
    TEXTUREFORMAT_RGBA8888 = 1,
    TEXTUREFORMAT_ARGB0888 = 2,
    TEXTUREFORMAT_ARGB4444 = 3,
    TEXTUREFORMAT_ARGB1555 = 4,
    TEXTUREFORMAT_ARGB0555 = 5,
    TEXTUREFORMAT_RGB565 = 6,
    TEXTUREFORMAT_L8 = 7,
    TEXTUREFORMAT_P8 = 8,
    TEXTUREFORMAT_DXT1 = 9,
    TEXTUREFORMAT_DXT1_1BITALPHA = 10,
    TEXTUREFORMAT_DXT3 = 11,
    TEXTUREFORMAT_DXT5 = 12,
    TEXTUREFORMAT_LIN_ARGB8888 = 13,
    TEXTUREFORMAT_LIN_RGBA8888 = 14,
    TEXTUREFORMAT_LIN_ARGB0888 = 15,
    TEXTUREFORMAT_LIN_ARGB4444 = 16,
    TEXTUREFORMAT_LIN_ARGB1555 = 17,
    TEXTUREFORMAT_LIN_ARGB0555 = 18,
    TEXTUREFORMAT_LIN_RGB565 = 19,
    TEXTUREFORMAT_LIN_L8 = 20,
    TEXTUREFORMAT_ABGR8888 = 21,
    TEXTUREFORMAT_RGB888 = 22,
    TEXTUREFORMAT_A8 = 23,
    TEXTUREFORMAT_R32F = 24,
    TEXTUREFORMAT_A16B16G16R16F = 25,
    TEXTUREFORMAT_A32B32G32R32F = 26,
    TEXTUREFORMAT_L16 = 27,
    TEXTUREFORMAT_R16G16B16 = 28,
    TEXTUREFORMAT_A16R16G16B16 = 29,
    TEXTUREFORMAT_D24X8 = 30,
    TEXTUREFORMAT_D16 = 31,
    TEXTUREFORMAT_DF24 = 32,
    TEXTUREFORMAT_DF16 = 33,
    TEXTUREFORMAT_D32 = 34,
    TEXTUREFORMAT_RAWZ = 35,
    TEXTUREFORMAT_INTZ = 36,
    TEXTUREFORMAT_DEPTHOPAQUE = 37,
    TEXTUREFORMAT_R16F = 38,
    TEXTUREFORMAT_G32FR32F = 39,
    TEXTUREFORMAT_G16FR16F = 40,
    TEXTUREFORMAT_A16B16G16R16 = 41,
    TEXTUREFORMAT_A2B10G10R10 = 42,
    TEXTUREFORMAT_DXT5N = 43,
    NUM_TEXTUREFORMATS = 44,
};

typedef int32 TextureFormat;

enum TextureUsage {
    TEXTUREUSAGE_NORMAL = 0,
    TEXTUREUSAGE_RENDERTARGET = 1,
    TEXTUREUSAGE_DEPTHTARGET = 2,
    NUM_TEXTUREUSAGES = 3,
};

uint32 TextureFormatSize(TextureFormat eTextureFormat);

// GfxColorFormats.inl:17. The body is not in the DWARF; the callers test a mask of the four DXT
// formats and DXT5N.
inline BOOL TextureFormatIsCompressed(TextureFormat eTextureFormat) {
    switch (eTextureFormat) {
        case TEXTUREFORMAT_DXT1:
        case TEXTUREFORMAT_DXT1_1BITALPHA:
        case TEXTUREFORMAT_DXT3:
        case TEXTUREFORMAT_DXT5:
        case TEXTUREFORMAT_DXT5N:
            return TRUE;
        default:
            return FALSE;
    }
}

// GfxColorFormats.inl:34. The callers test eTextureFormat - TEXTUREFORMAT_LIN_ARGB8888 < 8.
inline BOOL TextureFormatIsLinear(TextureFormat eTextureFormat) {
    return eTextureFormat >= TEXTUREFORMAT_LIN_ARGB8888 && eTextureFormat <= TEXTUREFORMAT_LIN_L8;
}
void TextureGetMipDimensions(const uint32 dwWidth, const uint32 dwHeight, const int32 nMipLevel,
                             uint32& dwMipWidth, uint32& dwMipHeight);
void TextureGetMipDimensions(const uint32 dwWidth, const uint32 dwHeight, const uint32 dwDepth,
                             const int32 nMipLevel, uint32& dwMipWidth, uint32& dwMipHeight,
                             uint32& dwMipDepth);

#include "RGBAColor.h"

// Line 110. Only the members GfxVB.cpp inlines are given bodies.
struct VertexColor {  // size 0x4
    static const uint32 redShiftVC = 0;
    static const uint32 greenShiftVC = 8;
    static const uint32 blueShiftVC = 16;
    static const uint32 alphaShiftVC = 24;
    VertexColor() {}
    VertexColor(const uint8 r, const uint8 g, const uint8 b, const uint8 a) { Set(r, g, b, a); }
    VertexColor(const RGBAColor& rgba) { Set(rgba); }  // line 151
    void Set(const uint8 r, const uint8 g, const uint8 b, const uint8 a) {  // line 163
        val = (r << redShiftVC) | (g << greenShiftVC) | (b << blueShiftVC) | (a << alphaShiftVC);
    }
    void Set(const RGBAColor& rgba) { Set(rgba.r, rgba.g, rgba.b, rgba.a); }  // line 172
    uint32 Get() const { return val; }
    // Lines 194, 200, 206 and 212; inlined into SubObjectGetSegmentVertexColor.
    uint8 Red() const { return (uint8)(val >> redShiftVC); }
    uint8 Green() const { return (uint8)(val >> greenShiftVC); }
    uint8 Blue() const { return (uint8)(val >> blueShiftVC); }
    uint8 Alpha() const { return (uint8)(val >> alphaShiftVC); }

private:
    uint32 val;  // 0x00
};
static_assert(sizeof(VertexColor) == 0x4, "VertexColor");
