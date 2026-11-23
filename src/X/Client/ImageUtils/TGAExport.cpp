// X/Client/ImageUtils/TGAExport.cpp -- uncompressed-RGB TGA writers.
#include "TGAExport.h"

#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/GfxColorFormats.h"

// Defined in X/Sigma/Files/XFilesCore.cpp; X/Sigma/Files/XFiles.h does not
// declare them in this tree yet.
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
BOOL FileCreate(FileReference & tFileRef);
BOOL FileOpen(FileReference & tFileRef, uint32 dwAccessFlags);
void FileClose(FileReference & tFileRef);
BOOL FileWrite(FileReference & tFileRef, const void * lpBuf, const uint32 dwOffset, const uint32 dwSize, ErrorCode eErrorCodeIfFails);

// Defined in X/Client/ImageUtils/ImageUtils.cpp; its header is not in src yet.
void ImageBufferFlipVertical(void * lpBuf, uint32 dwPitch, uint32 dwHeight);

// 0x71001e2fb0 -- TGAExport.cpp:22.
void TGAExportTexture(LPCSTR szPath, const Pixel32 * ptBuf, const uint32 dwPitch, const uint32 dwWidth, const uint32 dwHeight)
{
    FileReference tRef;
    const uint32 dwImageSize = dwHeight * dwPitch;

    ImageBufferFlipVertical((void *)ptBuf, dwPitch, dwHeight);

    TGAHeader tHeader = {};
    tHeader.bImageType = TGAIMAGETYPE_UNCOMPRESSED_RGB;
    tHeader.wWidth = (uint16)dwWidth;
    tHeader.wHeight = (uint16)dwHeight;
    tHeader.bPixelDepth = 32;

    FileReferenceInit(tRef, szPath);
    FileCreate(tRef);
    if (FileOpen(tRef, 2)) {
        if (FileWrite(tRef, &tHeader, 0, 0x12, ERROR_FILE_WRITE)) {
            FileWrite(tRef, ptBuf, 0x12, dwImageSize, ERROR_FILE_WRITE);
        }
        FileClose(tRef);
    }

    ImageBufferFlipVertical((void *)ptBuf, dwPitch, dwHeight);
}

// 0x71001e30b0 -- TGAExport.cpp:76.
void TGAExportTexture(FileReference & tRef, const uint32 dwWidth, const uint32 dwHeight, const Pixel32 * ptTile, const uint32 dwTileX, const uint32 dwTileY, const uint32 dwTileWidth, const uint32 dwTileHeight)
{
    const uint32 dwTilePitch = dwTileWidth * 4;
    const uint32 dwPitch = dwWidth * 4;

    ImageBufferFlipVertical((void *)ptTile, dwTilePitch, dwTileHeight);

    TGAHeader tHeader = {};
    tHeader.bImageType = TGAIMAGETYPE_UNCOMPRESSED_RGB;
    tHeader.wWidth = (uint16)dwWidth;
    tHeader.wHeight = (uint16)dwHeight;
    tHeader.bPixelDepth = 32;

    FileCreate(tRef);
    if (FileOpen(tRef, 2)) {
        if (FileWrite(tRef, &tHeader, 0, 0x12, ERROR_FILE_WRITE)) {
            uint32 dwFileOffset = dwPitch * dwTileY * dwTileHeight + dwTileX * dwTileWidth * 4 + 0x12;
            for (uint32 i = 0; i != dwTileHeight; ++i) {
                if (!FileWrite(tRef, ptTile + i * dwTileWidth, dwFileOffset, dwTilePitch, ERROR_FILE_WRITE)) {
                    break;
                }
                dwFileOffset += dwPitch;
            }
        }
        FileClose(tRef);
    }

    ImageBufferFlipVertical((void *)ptTile, dwTilePitch, dwTileHeight);
}
