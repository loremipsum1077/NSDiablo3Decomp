#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/GfxColorFormats.h"

enum {
    IMAGEFILEFORMAT_UNKNOWN = 0,
    IMAGEFILEFORMAT_BMP = 1,
    IMAGEFILEFORMAT_TGA = 2,
    IMAGEFILEFORMAT_TIFF = 3,
    IMAGEFILEFORMAT_JPG = 4,
    IMAGEFILEFORMAT_DDS = 5,
    IMAGEFILEFORMAT_PNG = 6,
};

typedef int32 ImageFileFormat;

LPCSTR ImageFileExtFromFileFormat(const ImageFileFormat eFileFormat);
void ImageFileExportTexture(const TextureFormat eTexFormat, const void * ptBuf, const uint32 dwPitch, const uint32 dwWidth, const uint32 dwHeight, LPCSTR szPath, const ImageFileFormat eFileFormat, const int32 nQuality);
