#include "ImageCompressAPI.h"
#include "../../Sigma/GfxColorFormats.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
// DWARF file-local helper signatures, definitions at lines 542 and 641.
static void sImageBufferConvertLinear(const void * lpSrc, void * lpDest, const uint32 dwSrcWidth, const uint32 dwSrcHeight, const uint32 dwSrcPitch, const TextureFormat eSrcTexFormat, const TextureFormat eDestTexFormat);
static void sImageBufferConvertNonlinear(const void * lpSrc, void * lpDest, const uint32 dwSrcWidth, const uint32 dwSrcHeight, const uint32 dwSrcPitch, const uint32 dwDestPitch, const TextureFormat eSrcTexFormat, const TextureFormat eDestTexFormat);





















































































void ImageBufferFlipVertical(void * lpBuf,
    uint32 dwPitch,
    uint32 dwHeight)

{
    uint8 * pbTempRow = (uint8 *)SigmaMemoryNew(dwPitch, 4, NULL, FALSE);
    uint8 * pbSrcRow = (uint8 *)lpBuf;
    uint8 * pbDestRow = pbSrcRow + (dwHeight - 1) * dwPitch;
    for (int32 i = 0; i < dwHeight / 2; ++i)
    {
        SigmaMemoryCopy(pbTempRow, pbSrcRow, dwPitch);
        SigmaMemoryCopy(pbSrcRow, pbDestRow, dwPitch);
        SigmaMemoryCopy(pbDestRow, pbTempRow, dwPitch);

        pbDestRow -= dwPitch;
        pbSrcRow += dwPitch;
    }

    SigmaMemoryFree(pbTempRow, NULL);
}




























































































































































































































































































































































void ImageBufferConvert(const void * lpSrc,
    void * lpDest,
    const uint32 dwSrcWidth,
    const uint32 dwSrcHeight,
    const uint32 dwSrcPitch,
    const TextureFormat eSrcTexFormat,
    const TextureFormat eDestTexFormat,
    uint32 dwDstPitch)
{
    if (dwDstPitch == (uint32)-1)
    {
        dwDstPitch = TextureFormatSize(eDestTexFormat) * dwSrcWidth;
    }


    if (TextureFormatIsCompressed(eDestTexFormat))
    {
        gptImageCompressAPI->ConvertToCompressed(lpSrc, lpDest, dwSrcWidth, dwSrcHeight, dwSrcPitch, eDestTexFormat);
    }
    else if (TextureFormatIsCompressed(eSrcTexFormat))
    {

        const uint32 kBlockSize = TextureFormatSize(eSrcTexFormat);
        const uint32 kBlockWidth = Math::Max(1u, dwSrcWidth / 4);
        const uint32 kBlockHeight = Math::Max(1u, dwSrcHeight / 4);
        const uint32 kCompactedRowSize = kBlockWidth * kBlockSize;






        if (kCompactedRowSize != dwSrcPitch)
        {





            uint8 * pSrcData = (uint8 *)SigmaMemoryNew(kCompactedRowSize * kBlockHeight, 4, NULL, FALSE);

            uint8 * pRead = (uint8 *)lpSrc;
            uint8 * pWrite = pSrcData;

            for (uint32 row = 0; row < kBlockHeight; ++row)
            {
                SigmaMemoryCopy(pWrite, pRead, kCompactedRowSize);

                pRead += dwSrcPitch;
                pWrite += kCompactedRowSize;
            }
            gptImageCompressAPI->ConvertToDecompressed(pSrcData, lpDest, dwSrcWidth, dwSrcHeight, eSrcTexFormat, eDestTexFormat);
            SigmaMemoryFree(pSrcData, NULL);
        }
        else
        {



            gptImageCompressAPI->ConvertToDecompressed(lpSrc, lpDest, dwSrcWidth, dwSrcHeight, eSrcTexFormat, eDestTexFormat);
        }
    }
    else
    {
        if (TextureFormatSize(eSrcTexFormat) * dwSrcWidth == dwSrcPitch && dwDstPitch == dwSrcPitch)
        {

            sImageBufferConvertLinear(lpSrc, lpDest, dwSrcWidth, dwSrcHeight, dwSrcPitch, eSrcTexFormat, eDestTexFormat);
        }
        else
        {
            sImageBufferConvertNonlinear(lpSrc, lpDest, dwSrcWidth, dwSrcHeight, dwSrcPitch, dwDstPitch, eSrcTexFormat, eDestTexFormat);
        }
    }
}




static void sImageBufferConvertLinear(const void * lpSrc,
    void * lpDest,
    const uint32 dwSrcWidth,
    const uint32 dwSrcHeight,
    const uint32 dwSrcPitch,
    const TextureFormat eSrcTexFormat,
    const TextureFormat eDestTexFormat)
{

    uint32 dwPixCount = dwSrcWidth * dwSrcHeight;
    if (eSrcTexFormat == eDestTexFormat)
    {
        SigmaMemoryCopy(lpDest, lpSrc, dwSrcPitch * dwSrcHeight);
        return;
    }



    uint32 dwSrcSize = TextureFormatSize(eSrcTexFormat);
    uint32 dwDestSize = TextureFormatSize(eDestTexFormat);

    Pixel8 * ptSrc8 = (Pixel8 *)lpSrc;
    switch (dwDestSize)
    {

    case 4:
    {
        Pixel32 * ptDest32 = (Pixel32 *)lpDest;
        while (dwPixCount--)
        {
            RGBAColor rgbaSrc;
            rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
            *ptDest32++ = (Pixel32)rgbaSrc.ToTexturePixel(eDestTexFormat);
            ptSrc8 += dwSrcSize;
        }
        break;
    }



    case 3:
    {
        Pixel8 * pbDest24 = (Pixel8 *)lpDest;
        while (dwPixCount--)
        {
            RGBAColor rgbaSrc;
            rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
            Pixel32 p = rgbaSrc.ToTexturePixel(eDestTexFormat);
            pbDest24[0] = (Pixel8)(p >> 16);
            pbDest24[1] = (Pixel8)(p >> 8);
            pbDest24[2] = (Pixel8)p;
            pbDest24 += 3;
            ptSrc8 += dwSrcSize;
        }
        break;
    }


    case 2:
    {
        Pixel16 * ptDest16 = (Pixel16 *)lpDest;
        while (dwPixCount--)
        {
            RGBAColor rgbaSrc;
            rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
            *ptDest16++ = (Pixel16)rgbaSrc.ToTexturePixel(eDestTexFormat);
            ptSrc8 += dwSrcSize;
        }
        break;
    }



    case 1:
    {
        Pixel8 * ptDest8 = (Pixel8 *)lpDest;
        while (dwPixCount--)
        {
            RGBAColor rgbaSrc;
            rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
            *ptDest8++ = (Pixel8)rgbaSrc.ToTexturePixel(eDestTexFormat);
            ptSrc8 += dwSrcSize;
        }
        break;
    }


    default:
    {
        CRefString sTemp;
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    }
}




static void sImageBufferConvertNonlinear(const void * lpSrc,
    void * lpDest,
    const uint32 dwSrcWidth,
    const uint32 dwSrcHeight,
    const uint32 dwSrcPitch,
    const uint32 dwDestPitch,
    const TextureFormat eSrcTexFormat,
    const TextureFormat eDestTexFormat)
{
    if (eSrcTexFormat == eDestTexFormat)
    {


        uint32 dwSrcLineSize = TextureFormatSize(eSrcTexFormat) * dwSrcWidth;
        for (uint32 i = 0; i < dwSrcHeight; ++i)
            SigmaMemoryCopy((Pixel8 *)lpDest + dwDestPitch * i, (Pixel8 *)lpSrc + dwSrcPitch * i, dwSrcLineSize);
    }
    else
    {


        uint32 dwSrcSize = TextureFormatSize(eSrcTexFormat);
        uint32 dwDestSize = TextureFormatSize(eDestTexFormat);

        for (uint32 i = 0; i < dwSrcHeight; ++i)
        {
            Pixel8 * ptSrc8 = (Pixel8 *)lpSrc + dwSrcPitch * i;
            Pixel8 * ptDest8 = (Pixel8 *)lpDest + dwDestPitch * i;
            Pixel16 * ptDest16 = (Pixel16 *)ptDest8;
            Pixel8 * pbDest24 = ptDest8;
            Pixel32 * ptDest32 = (Pixel32 *)ptDest8;

            uint32 dwPixPerRow = dwSrcWidth;
            switch (dwDestSize)
            {

            case 4:
                while (dwPixPerRow--)
                {

                    RGBAColor rgbaSrc;
                    rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
                    *ptDest32++ = (Pixel32)rgbaSrc.ToTexturePixel(eDestTexFormat);
                    ptSrc8 += dwSrcSize;
                }
                break;




            case 3:
                while (dwPixPerRow--)
                {

                    RGBAColor rgbaSrc;
                    rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
                    Pixel32 p = rgbaSrc.ToTexturePixel(eDestTexFormat);
                    pbDest24[0] = (Pixel8)(p >> 16);
                    pbDest24[1] = (Pixel8)(p >> 8);
                    pbDest24[2] = (Pixel8)p;
                    pbDest24 += 3;
                    ptSrc8 += dwSrcSize;
                }
                break;



            case 2:
                while (dwPixPerRow--)
                {

                    RGBAColor rgbaSrc;
                    rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
                    *ptDest16++ = (Pixel16)rgbaSrc.ToTexturePixel(eDestTexFormat);
                    ptSrc8 += dwSrcSize;
                }
                break;




            case 1:
                while (dwPixPerRow--)
                {

                    RGBAColor rgbaSrc;
                    rgbaSrc.FromTexturePixel(ptSrc8, eSrcTexFormat);
                    *ptDest8++ = (Pixel8)rgbaSrc.ToTexturePixel(eDestTexFormat);
                    ptSrc8 += dwSrcSize;
                }
                break;



            default:
            {
                CRefString sTemp;
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
                ExitProgram(ERROR_INTERNAL_FATAL);
            }
            }
        }
    }
}

