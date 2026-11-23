// X/Client/ImageUtils/ImageCompressStubAPI.h -- the compressor every build
// without a texture compressor installs: each entry point is a fatal internal
// error.  The `__LINE__` each one bakes in is the 2.6.2 header's (26 / 37 /
// 49), so the bodies sit on those lines.
#pragma once

#include "ImageCompressAPI.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"






class ImageCompressStubAPI : public ImageCompressAPI {  // size 0x8
public:








    void ConvertToCompressed(const void *, void *, const uint32, const uint32, const uint32, const TextureFormat) {
        DisplayInternalError(0, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }








    void ConvertToDecompressed(const void *, void *, const uint32, const uint32, const TextureFormat, const TextureFormat) {
        DisplayInternalError(0, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }









    void ConvertAndMipMap(const void *, void **, const uint32, const uint32, const uint32, const TextureFormat, const ImageConvertParams &) {
        DisplayInternalError(0, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
};
static_assert(sizeof(ImageCompressStubAPI) == 0x8, "ImageCompressStubAPI");
