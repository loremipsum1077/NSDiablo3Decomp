// X/Client/ImageUtils/ImageCompressAPI.h -- the texture compressor interface;
// `gptImageCompressAPI` holds the one instance (ImageCompressAPI.cpp).  The
// 2.6.2 DWARF puts the class at line 21 and its inline destructor at line 24;
// the line numbers are kept.
#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/RGBAColor.h"

struct ImageConvertParams;










class ImageCompressAPI {  // size 0x8
public:
    // ImageCompressAPI.h:24 (main.elf 0x71003dd0c0, `ret`)
    virtual ~ImageCompressAPI() {}
    virtual void ConvertToCompressed(const void *, void *, const uint32, const uint32, const uint32, const TextureFormat) = 0;  // vtable slot 2
    virtual void ConvertToDecompressed(const void *, void *, const uint32, const uint32, const TextureFormat, const TextureFormat) = 0;  // vtable slot 3
    virtual void ConvertAndMipMap(const void *, void **, const uint32, const uint32, const uint32, const TextureFormat, const ImageConvertParams &) = 0;  // vtable slot 4
};
static_assert(sizeof(ImageCompressAPI) == 0x8, "ImageCompressAPI");

extern ImageCompressAPI * gptImageCompressAPI;

void ImageCompressAPIInitialize();
void ImageCompressAPIDispose();
