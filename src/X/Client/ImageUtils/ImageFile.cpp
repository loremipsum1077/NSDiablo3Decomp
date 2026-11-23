// X/Client/ImageUtils/ImageFile.cpp -- screenshot file-format dispatch.
#include "ImageFile.h"

#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/GfxColorFormats.h"

// The export implementations live in their own translation units; their
// declarations belong to the BMPExport/TGAExport/JPGExport headers.
// X/Client/ImageUtils/BMPExport.h:21
void BMPExportTexture(LPCSTR szPath, Pixel32 * ptBuf, uint32 dwPitch, uint32 dwWidth, uint32 dwHeight);
// X/Client/ImageUtils/TGAExport.h:22
void TGAExportTexture(LPCSTR szPath, const Pixel32 * ptBuf, const uint32 dwPitch, const uint32 dwWidth, const uint32 dwHeight);
// X/Client/ImageUtils/JPGExport.h:34
void JPGExportTexture(LPCSTR szPath, const Pixel32 * ptBuf, const uint32 dwPitch, const uint32 dwWidth, const uint32 dwHeight, const int32 nQuality);

// 0x71001e2400 -- ImageFile.cpp:64.
LPCSTR ImageFileExtFromFileFormat(const ImageFileFormat eFileFormat)
{
    switch (eFileFormat) {
    case IMAGEFILEFORMAT_BMP:
        return "bmp";
    case IMAGEFILEFORMAT_TGA:
        return "tga";
    case IMAGEFILEFORMAT_TIFF:
        return "tif";
    case IMAGEFILEFORMAT_JPG:
        return "jpg";
    case IMAGEFILEFORMAT_DDS:
        return "dds";
    case IMAGEFILEFORMAT_PNG:
        return "png";
    }
    return "???";
}

// 0x71001e2430 -- ImageFile.cpp:94.  TIF is recognised but not written.
void ImageFileExportTexture(const TextureFormat eTexFormat, const void * ptBuf, const uint32 dwPitch, const uint32 dwWidth, const uint32 dwHeight, LPCSTR szPath, const ImageFileFormat eFileFormat, const int32 nQuality)
{
    switch (eFileFormat) {
    case IMAGEFILEFORMAT_BMP:
        BMPExportTexture(szPath, (Pixel32 *)ptBuf, dwPitch, dwWidth, dwHeight);
        break;
    case IMAGEFILEFORMAT_TGA:
        TGAExportTexture(szPath, (const Pixel32 *)ptBuf, dwPitch, dwWidth, dwHeight);
        break;
    case IMAGEFILEFORMAT_TIFF:
        break;
    case IMAGEFILEFORMAT_JPG:
        JPGExportTexture(szPath, (const Pixel32 *)ptBuf, dwPitch, dwWidth, dwHeight, nQuality);
        break;
    default:
        DisplayInternalError(NULL, "", 132, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}
