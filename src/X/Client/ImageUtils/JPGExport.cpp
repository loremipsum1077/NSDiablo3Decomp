#include "../../Sigma/GfxColorFormats.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Files/XFiles.h"
#include "../../../Contrib/Contrib/jpeg-compressor_v104_r1/jpge.h"
#include <stdio.h>
#include <string.h>
// DWARF ImageUtils.cpp:463; XFilesCore.cpp:523,543.
void ImageBufferConvert(const void * lpSrc, void * lpDest, const uint32 dwSrcWidth, const uint32 dwSrcHeight, const uint32 dwSrcPitch, const TextureFormat eSrcTexFormat, const TextureFormat eDestTexFormat, uint32 dwDstPitch);
BOOL FileDelete(FileReference & tFileRef);
BOOL FileRename(FileReference & tFileRef, LPCSTR szNewName);






















void JPGExportTexture(LPCSTR szPath,
    const Pixel32 * ptBuf,
    const uint32 dwPitch,
    const uint32 dwWidth,
    const uint32 dwHeight,
    const int32 nQuality)

{
























    // Inferred RGB allocation expression from original 32-bit multiply-by-three then shift-by-two.
    Pixel32 * ptBufNew = (Pixel32 *)SigmaMemoryNew(dwPitch * dwHeight * 3 / 4, 4, NULL, FALSE);
    ImageBufferConvert(ptBuf, ptBufNew, dwWidth, dwHeight, dwPitch, TEXTUREFORMAT_ARGB8888, TEXTUREFORMAT_RGB888, (uint32)-1);

    jpge::params params;
    params.m_quality = nQuality;

    jpge::compress_image_to_jpeg_file(szPath, dwWidth, dwHeight, 3, (const jpge::uint8 *)ptBufNew, params);




    SigmaMemoryFree(ptBufNew, NULL);


}



















































// NON_MATCHING: 948 vs 940 bytes; same dispatch and loop split as the original, but the loop-split flag has the opposite sense (ours tracks bAddedComment, original !bAddedComment; the column order "(...) && !bAddedComment" gives the right sense but merges the 0xdb/0xfe dispatch), plus a hoisted 0xdb constant and a shared fclose tail. Tried inner test != 0xdb / == 0xfe || == 0xe1 / reversed, three && orders, moving bAddedComment = TRUE.
void JPGSetComment(
    LPCSTR szPath,
    LPCSTR szComment)
{ // Inferred success flag becomes true only after closing the output; original then deletes/renames.
    BOOL bSuccess = FALSE;
    CRefString sTempPath = CRefString().Format("%s.tmp", szPath);
    FILE * pScreenshot = fopen(szPath, "rb");
    if (pScreenshot)
    {
        FILE * pNewScreenshot = fopen(sTempPath.str(), "wb");
        if (pNewScreenshot)
        {

            unsigned char marker = 0;
            unsigned char prevMarker = 0;
            BOOL bAddedComment = FALSE;
            while (!feof(pScreenshot))
            {
                marker = (unsigned char)getc(pScreenshot); // column 29: the getc after a cast
                if (prevMarker == 0xff && !bAddedComment && (marker == 0xdb || marker == 0xfe || marker == 0xe1)) // columns 20/28/47/84 suggest "(...) && !bAddedComment" last; that order compiles further from the original (see NON_MATCHING)
                {


                    uint32 prevLength = 0;
                    if (marker == 0xfe || marker == 0xe1)
                    {
                        prevLength = getc(pScreenshot) << 8;
                        prevLength += getc(pScreenshot);
                    }

                    unsigned short commentLength = (unsigned short)strlen(szComment);
                    // Original EXIF payload bytes, recovered as string data (not executable bytes).
                    const char * szExifHeader = "Exif\0\0MM\0\x2a\0\0\0\x08";
                    const int exitHeaderLength = 14;
                    const char * szExifOffset = "\0\x01\x87\x69\0\x04\0\0\0\x01\0\0\0\x1a\0\0\0\0";
                    const int exitOffsetLength = 18;
                    const char * szUserCommentPart1 = "\0\x01\x92\x86\0\x07";
                    const int userCommentLengthPart1 = 6;
                    const char * szUserCommentPart2 = "\0\0\0\x2c\0\0\0\0UNICODE\0";
                    const int userCommentLengthPart2 = 16;

                    unsigned short userCommentLength = commentLength * 2 + 8;
                    unsigned short exifLength = commentLength * 2 + exitHeaderLength + exitOffsetLength + userCommentLengthPart1 + userCommentLengthPart2 + 8;


                    putc(0xe1, pNewScreenshot);
                    putc((exifLength >> 8) & 0xff, pNewScreenshot);
                    putc(exifLength & 0xff, pNewScreenshot);
                    fwrite(szExifHeader, 1, exitHeaderLength, pNewScreenshot);
                    fwrite(szExifOffset, 1, exitOffsetLength, pNewScreenshot);
                    fwrite(szUserCommentPart1, 1, userCommentLengthPart1, pNewScreenshot);
                    putc(0, pNewScreenshot);
                    putc(0, pNewScreenshot);
                    putc((userCommentLength >> 8) & 0xff, pNewScreenshot);
                    putc(userCommentLength & 0xff, pNewScreenshot);
                    fwrite(szUserCommentPart2, 1, userCommentLengthPart2, pNewScreenshot);
                    for (int32 i = 0; i < commentLength; ++i)
                    {
                        putc(0, pNewScreenshot);
                        putc(szComment[i], pNewScreenshot);
                    }
                    putc(0, pNewScreenshot);
                    putc(0, pNewScreenshot);


                    putc(0xff, pNewScreenshot);
                    putc(0xfe, pNewScreenshot);
                    putc(((commentLength + 3) >> 8) & 0xff, pNewScreenshot);
                    putc((commentLength & 0xff) + 3, pNewScreenshot);
                    fwrite(szComment, 1, commentLength, pNewScreenshot);
                    fflush(pNewScreenshot);
                    putc(0, pNewScreenshot);
                    fseek(pScreenshot, (int32)prevLength, SEEK_CUR); // Signed extension in original.

                    if (marker == 0xdb) {
                        putc(0xff, pNewScreenshot);
                        putc(0xdb, pNewScreenshot);
                    }
                    bAddedComment = TRUE;
                    // Inferred: the insertion path skips the marker copy (putc at 216 sits at loop-body depth, column 5); prevMarker stays 0xff.
                    continue;
                }
                putc(marker, pNewScreenshot);
                prevMarker = marker;

            }
            fflush(pNewScreenshot);
            fclose(pNewScreenshot);
            bSuccess = TRUE;
        }
        fclose(pScreenshot);
    }

    if (bSuccess)
    {

        FileReference tRef;
        FileReferenceInit(tRef, szPath);
        FileDelete(tRef);

        FileReference tRefNewFile;
        FileReferenceInit(tRefNewFile, sTempPath.str());
        FileRename(tRefNewFile, szPath);
    }
}

