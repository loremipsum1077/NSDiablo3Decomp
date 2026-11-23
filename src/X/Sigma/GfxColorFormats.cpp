#include "GfxColorFormats.h"

#include "ErrorManager/ErrorManager.h"
#include "Math/MathGeneral.h"
#include "RGBAColor.h"

uint32 TextureFormatSize(TextureFormat eTextureFormat) {
    switch (eTextureFormat) {
    case TEXTUREFORMAT_ARGB8888:
    case TEXTUREFORMAT_RGBA8888:
    case TEXTUREFORMAT_ARGB0888:
    case TEXTUREFORMAT_LIN_ARGB8888:
    case TEXTUREFORMAT_LIN_RGBA8888:
    case TEXTUREFORMAT_LIN_ARGB0888:
    case TEXTUREFORMAT_ABGR8888:
    case TEXTUREFORMAT_R32F:
    case TEXTUREFORMAT_D24X8:
    case TEXTUREFORMAT_DF24:
    case TEXTUREFORMAT_D32:
    case TEXTUREFORMAT_RAWZ:
    case TEXTUREFORMAT_INTZ:
    case TEXTUREFORMAT_DEPTHOPAQUE:
    case TEXTUREFORMAT_G16FR16F:
    case TEXTUREFORMAT_A2B10G10R10:
        return 4;

    case TEXTUREFORMAT_ARGB4444:
    case TEXTUREFORMAT_ARGB1555:
    case TEXTUREFORMAT_ARGB0555:
    case TEXTUREFORMAT_RGB565:
    case TEXTUREFORMAT_LIN_ARGB4444:
    case TEXTUREFORMAT_LIN_ARGB1555:
    case TEXTUREFORMAT_LIN_ARGB0555:
    case TEXTUREFORMAT_LIN_RGB565:
    case TEXTUREFORMAT_L16:
    case TEXTUREFORMAT_D16:
    case TEXTUREFORMAT_DF16:
    case TEXTUREFORMAT_R16F:
        return 2;

    case TEXTUREFORMAT_L8:
    case TEXTUREFORMAT_P8:
    case TEXTUREFORMAT_LIN_L8:
    case TEXTUREFORMAT_A8:
        return 1;

    case TEXTUREFORMAT_RGB888:
        return 3;

    case TEXTUREFORMAT_R16G16B16:
        return 6;

    case TEXTUREFORMAT_DXT1:
    case TEXTUREFORMAT_DXT1_1BITALPHA:
    case TEXTUREFORMAT_A16B16G16R16F:
    case TEXTUREFORMAT_A16R16G16B16:
    case TEXTUREFORMAT_G32FR32F:
    case TEXTUREFORMAT_A16B16G16R16:
        return 8;

    case TEXTUREFORMAT_DXT3:
    case TEXTUREFORMAT_DXT5:
    case TEXTUREFORMAT_A32B32G32R32F:
    case TEXTUREFORMAT_DXT5N:
        return 16;

    default:
        DisplayInternalError(NULL, "", 87, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

void RGBAColor::FromTexturePixel(const void* pColor, const TextureFormat eTextureFormat) {
    switch (eTextureFormat) {
    case TEXTUREFORMAT_ARGB8888:
    case TEXTUREFORMAT_LIN_ARGB8888: {
        Pixel32 p = *(const Pixel32*)pColor;
        Set((uint8)(p >> 16), (uint8)(p >> 8), (uint8)p, (uint8)(p >> 24));
        break;
    }
    case TEXTUREFORMAT_RGBA8888: {
        Pixel32 p = *(const Pixel32*)pColor;
        Set((uint8)(p >> 24), (uint8)(p >> 16), (uint8)(p >> 8), (uint8)p);
        break;
    }
    case TEXTUREFORMAT_ARGB0888: {
        Pixel32 p = *(const Pixel32*)pColor;
        Set((uint8)(p >> 16), (uint8)(p >> 8), (uint8)p, 0xFF);
        break;
    }
    case TEXTUREFORMAT_ARGB4444: {
        Pixel16 p = *(const Pixel16*)pColor;
        Set((p & 0x0F00) >> 4, p & 0x00F0, (p & 0x000F) << 4, (p & 0xF000) >> 8);
        break;
    }
    case TEXTUREFORMAT_ARGB1555: {
        Pixel16 p = *(const Pixel16*)pColor;
        Set((p & 0x7C00) >> 7, (p & 0x03E0) >> 2, (p & 0x001F) << 3, (p & 0x8000) >> 8);
        break;
    }
    case TEXTUREFORMAT_ARGB0555: {
        Pixel16 p = *(const Pixel16*)pColor;
        Set((p & 0x7C00) >> 7, (p & 0x03E0) >> 2, (p & 0x001F) << 3, 0xFF);
        break;
    }
    case TEXTUREFORMAT_RGB565: {
        Pixel16 p = *(const Pixel16*)pColor;
        Set((p & 0xF800) >> 8, (p & 0x07E0) >> 3, (p & 0x001F) << 3, 0xFF);
        break;
    }
    case TEXTUREFORMAT_L8:
    case TEXTUREFORMAT_LIN_L8: {
        Pixel8 p = *(const Pixel8*)pColor;
        Set(p, p, p, 0xFF);
        break;
    }
    case TEXTUREFORMAT_ABGR8888: {
        Pixel32 p = *(const Pixel32*)pColor;
        Set((uint8)p, (uint8)(p >> 8), (uint8)(p >> 16), (uint8)(p >> 24));
        break;
    }
    case TEXTUREFORMAT_RGB888: {
        const uint8* arPixels = (const uint8*)pColor;
        Set(arPixels[0], arPixels[1], arPixels[2], 0xFF);
        break;
    }
    case TEXTUREFORMAT_A8: {
        Pixel8 p = *(const Pixel8*)pColor;
        Set(0, 0, 0, p);
        break;
    }
    default:
        DisplayInternalError(NULL, "", 220, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

Pixel32 RGBAColor::ToTexturePixel(const TextureFormat eTextureFormat) const {
    Pixel32 pColor;
    switch (eTextureFormat) {
    case TEXTUREFORMAT_ARGB8888:
    case TEXTUREFORMAT_LIN_ARGB8888:
        pColor = (a << 24) | (r << 16) | (g << 8) | b;
        break;
    case TEXTUREFORMAT_RGBA8888:
    case TEXTUREFORMAT_LIN_RGBA8888:
        pColor = (r << 24) | (g << 16) | (b << 8) | a;
        break;
    case TEXTUREFORMAT_ARGB0888:
    case TEXTUREFORMAT_LIN_ARGB0888:
    case TEXTUREFORMAT_RGB888:
        pColor = (r << 16) | (g << 8) | b;
        break;
    case TEXTUREFORMAT_ARGB4444:
    case TEXTUREFORMAT_LIN_ARGB4444:
        pColor = ((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
        break;
    case TEXTUREFORMAT_ARGB1555:
    case TEXTUREFORMAT_LIN_ARGB1555:
        pColor = ((a >> 7) << 15) | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        break;
    case TEXTUREFORMAT_ARGB0555:
    case TEXTUREFORMAT_LIN_ARGB0555:
        pColor = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        break;
    case TEXTUREFORMAT_RGB565:
    case TEXTUREFORMAT_LIN_RGB565:
        pColor = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        break;
    case TEXTUREFORMAT_L8:
    case TEXTUREFORMAT_LIN_L8:
        pColor = r;
        break;
    case TEXTUREFORMAT_ABGR8888:
        pColor = (a << 24) | (b << 16) | (g << 8) | r;
        break;
    case TEXTUREFORMAT_A8:
        pColor = a;
        break;
    default:
        DisplayInternalError(NULL, "", 304, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return pColor;
}

void TextureGetMipDimensions(const uint32 dwWidth, const uint32 dwHeight, const int32 nMipLevel,
                             uint32& dwMipWidth, uint32& dwMipHeight) {
    dwMipWidth = dwWidth >> nMipLevel;
    dwMipHeight = dwHeight >> nMipLevel;
}

void TextureGetMipDimensions(const uint32 dwWidth, const uint32 dwHeight, const uint32 dwDepth,
                             const int32 nMipLevel, uint32& dwMipWidth, uint32& dwMipHeight,
                             uint32& dwMipDepth) {
    dwMipWidth = dwWidth >> nMipLevel;
    dwMipHeight = dwHeight >> nMipLevel;
    dwMipDepth = Math::Max<uint32>(dwDepth >> nMipLevel, 1);
}
