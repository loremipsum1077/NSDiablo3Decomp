// X/Client/ImageUtils/BMPExport.cpp -- uncompressed 32-bit BMP writer.
#include "BMPHeader.h"

#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/GfxColorFormats.h"
#include "../../Sigma/Memory/SigmaMemory.h"

// Defined in X/Sigma/Files/XFilesCore.cpp; X/Sigma/Files/XFiles.h does not
// declare them in this tree yet.
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
BOOL FileCreate(FileReference & tFileRef);
BOOL FileOpen(FileReference & tFileRef, uint32 dwAccessFlags);
void FileClose(FileReference & tFileRef);
BOOL FileWrite(FileReference & tFileRef, const void * lpBuf, const uint32 dwOffset, const uint32 dwSize, ErrorCode eErrorCodeIfFails);

// 0x71003dce00 -- BMPExport.cpp:21.
void BMPExportTexture(LPCSTR szPath, Pixel32 * ptBuf, uint32 dwPitch, uint32 dwWidth, uint32 dwHeight)
{
    FileReference tRef;
    uint32 dwImageSize = dwHeight * dwPitch;

    BITMAPINFOHEADER bmih;
    SigmaMemoryClear(&bmih, sizeof(bmih));
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = dwWidth;
    bmih.biHeight = dwHeight;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = 0;
    bmih.biSizeImage = dwImageSize;
    bmih.biXPelsPerMeter = 2835;
    bmih.biYPelsPerMeter = 2835;

    BITMAPFILEHEADER bfh;
    ZeroMemory(&bfh, sizeof(bfh));
    bfh.bfType = 0x4d42;
    bfh.bfSize = dwImageSize + 0x36;
    bfh.bfOffBits = 0x36;

    FileReferenceInit(tRef, szPath);
    FileCreate(tRef);
    if (FileOpen(tRef, 2)) {
        if (FileWrite(tRef, &bfh, 0, 0xe, ERROR_FILE_WRITE)) {
            if (FileWrite(tRef, &bmih, 0xe, 0x28, ERROR_FILE_WRITE)) {
                uint32 dwFileOffset = 0x36;
                for (int32 i = dwHeight - 1; i >= 0; --i) {
                    if (!FileWrite(tRef, (uint8 *)ptBuf + i * dwPitch, dwFileOffset, dwPitch, ERROR_FILE_WRITE)) {
                        break;
                    }
                    dwFileOffset += dwPitch;
                }
            }
        }
        FileClose(tRef);
    }
}
