// Original file: X/Common/Console/IO/ConsoleIO_ArchiveReader.cpp

#include "ConsoleIO_ArchiveReader.h"

#include <stdlib.h>
#include <string.h>

#include "../../../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"
#include "../../../../BlizzardCore/Include/Blizzard/Util/Byteswap.h"
#include "../../../../BlizzardCore/Include/Blizzard/Util/Crypto.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/UnicodeText.h"
#include "ConsoleIO.h"

namespace Console {
namespace IO {

static void sReadBits(uint8 * pSourceBuffer, int32 nBitIndex, int32 nBitCount, uint32 & dwDestValue);
static void sReadBits(uint8 * pSourceBuffer, int32 nBitIndex, int32 nBitCount, uint64 & dwDestValue);

CPKArchiveReader::CPKArchiveReader(const char * szFilename, Console::IO::ReadBuffer & readBuffer, BOOL fLoadFilenamesInMemory, BOOL fIsHotfix, BOOL fIsValidateCRC, Console::IO::CPKArchiveReader::CPKArchiveTypes eArchiveType)
    : CPKArchive(szFilename),
      m_nFileSize(-1),
      m_nDecompressedFileSize(-1),
      m_dwCRC(0),
      m_nCurrentReadOffset(0),
      m_nReadSectorCount(-1),
      m_nCompSectorCount(-1),
      m_nReadSectorSize(CPK_READ_SECTOR_SIZE),
      m_nCompSectorSize(CPK_COMP_SECTOR_SIZE),
      m_pDecompressedSectorToCompressedSector(NULL),
      m_pCompressedSectorToDecompressedOffset(NULL),
      m_tStream(NULL),
      m_pSortedFileInfo(NULL),
      m_nFileCount(0),
      m_nPriority(0),
      m_pLocations(NULL),
      m_nFileSizeBitCount(0),
      m_nFileLocationCountBitCount(0),
      m_nFileLocationIndexBitCount(0),
      m_nLocationBitCount(0),
      m_nCompSectorToDecompOffsetBitCount(0),
      m_nDecompSectorToCompSectorBitCount(0),
      m_nHeaderReadSectorCount(0),
      m_nHeaderSize(0),
      m_nFilenameArrayFileOffset(-1),
      m_pFilenameArrayOffsets(NULL),
      m_szFilenames(NULL),
      m_nFileOpenCount(0),
      m_eArchiveType(eArchiveType),
      m_pArchiveData(NULL),
      m_fByteSwap(FALSE)
{
    m_nLastFileRecordAccessTime = Blizzard::Time::Milliseconds();
    {
        int mode = Blizzard::File::Perm::read | Blizzard::File::Perm::shRead | Blizzard::File::Mode::openExisting;
        if (!Blizzard::File::Open(m_szFilename, mode, m_tStream))
        {
            m_nResult = IORESULT_FILE_NOT_FOUND;
            return;
        }
    }
    if (InitializeArchive(readBuffer, fLoadFilenamesInMemory, FALSE, fIsValidateCRC))
    {
        m_nResult = IORESULT_SUCCESS;
        m_dwFlags |= 1;
    }
    else
    {
        Blizzard::File::Close(m_tStream);
        m_tStream = NULL;
    }
}

CPKArchiveReader::CPKArchiveReader(const char * szFilename, const uint8 * pArchiveData, int64 nArchiveSize, BOOL fValidateCRC)
    : CPKArchive(szFilename),
      m_nFileSize(nArchiveSize),
      m_nDecompressedFileSize(-1),
      m_dwCRC(0),
      m_nCurrentReadOffset(0),
      m_nReadSectorCount(-1),
      m_nCompSectorCount(-1),
      m_nReadSectorSize(CPK_READ_SECTOR_SIZE),
      m_nCompSectorSize(CPK_COMP_SECTOR_SIZE),
      m_pDecompressedSectorToCompressedSector(NULL),
      m_pCompressedSectorToDecompressedOffset(NULL),
      m_tStream(NULL),
      m_pSortedFileInfo(NULL),
      m_nFileCount(0),
      m_nPriority(0),
      m_pLocations(NULL),
      m_nFileSizeBitCount(0),
      m_nFileLocationCountBitCount(0),
      m_nFileLocationIndexBitCount(0),
      m_nLocationBitCount(0),
      m_nCompSectorToDecompOffsetBitCount(0),
      m_nDecompSectorToCompSectorBitCount(0),
      m_nHeaderReadSectorCount(0),
      m_nHeaderSize(0),
      m_nFilenameArrayFileOffset(-1),
      m_pFilenameArrayOffsets(NULL),
      m_szFilenames(NULL),
      m_nFileOpenCount(0),
      m_eArchiveType(CPK_ARCHIVE_TYPE_STANDARD),
      m_pArchiveData(pArchiveData),
      m_fByteSwap(FALSE)
{
    m_nLastFileRecordAccessTime = Blizzard::Time::Milliseconds();
    if (InitializeArchiveFromMemory(fValidateCRC))
    {
        m_nResult = IORESULT_SUCCESS;
        m_dwFlags |= 5;
    }
    else
    {
        Blizzard::File::Close(m_tStream);
        m_tStream = NULL;
    }
}

CPKArchiveReader::~CPKArchiveReader()
{
    if (m_dwFlags & 4)
    {
        if (m_fByteSwap)
            SigmaMemoryFree(m_pFilenameArrayOffsets, NULL);
        SigmaMemoryFree((void *)m_pArchiveData, NULL);
    }
    else
    {
        SigmaMemoryFree(m_pDecompressedSectorToCompressedSector, NULL);
        SigmaMemoryFree(m_pCompressedSectorToDecompressedOffset, NULL);
        SigmaMemoryFree(m_pLocations, NULL);
        SigmaMemoryFree(m_pFilenameArrayOffsets, NULL);
        SigmaMemoryFree(m_szFilenames, NULL);
        delete[] m_pSortedFileInfo;
        if (m_tStream != NULL)
        {
            Blizzard::File::Close(m_tStream);
            m_tStream = NULL;
        }
    }
}

BOOL CPKArchiveReader::InitializeArchive(Console::IO::ReadBuffer & readBuffer, BOOL fLoadFilenamesInMemory, BOOL fSkipSize, BOOL fValidateCRC)
{
    if (!fSkipSize)
    {
        Blizzard::File::FileInfo info;
        if (!Blizzard::File::GetFileInfo(m_tStream, &info))
        {
            m_nResult = IORESULT_FILE_IO_ERROR;
            return FALSE;
        }
        m_nFileSize = info.size;
    }
    m_nCurrentReadOffset = 0;

    Console::IO::CPKArchive::Header header;
    if (m_nFileSize == 0 || !Read(readBuffer, &header, sizeof(header)))
    {
        m_nResult = IORESULT_FILE_IO_ERROR;
        return FALSE;
    }

    BOOL fSwap = header.dwMagicNumber == 0xD4C3B2A1;
    m_fByteSwap = fSwap;
    if (fSwap)
    {
        Macro_Endian_SwizzleT<4>(&header.dwMagicNumber);
        Macro_Endian_SwizzleT<4>(&header.dwPackageVersion);
        Macro_Endian_SwizzleT<4>(&header.nFileCount);
        Macro_Endian_SwizzleT<8>(&header.nDecompressedFileSize);
        Macro_Endian_SwizzleT<4>(&header.dwFlags);
        Macro_Endian_SwizzleT<4>(&header.nHeaderSector);
        Macro_Endian_SwizzleT<4>(&header.nLocationCount);
        Macro_Endian_SwizzleT<4>(&header.nFileSizeBitCount);
        Macro_Endian_SwizzleT<4>(&header.nFileLocationCountBitCount);
        Macro_Endian_SwizzleT<4>(&header.nFileLocationIndexBitCount);
        Macro_Endian_SwizzleT<4>(&header.nLocationBitCount);
        Macro_Endian_SwizzleT<4>(&header.nCompSectorToDecompOffsetBitCount);
        Macro_Endian_SwizzleT<4>(&header.nDecompSectorToCompSectorBitCount);
        Macro_Endian_SwizzleT<4>(&header.dwCRC);
        Macro_Endian_SwizzleT<4>(&header.nReadSectorSize);
        Macro_Endian_SwizzleT<4>(&header.nCompSectorSize);
    }

    if (header.dwMagicNumber != 0xA1B2C3D4)
    {
        m_nResult = IORESULT_INVALID_HEADER;
        return FALSE;
    }

    if (header.dwPackageVersion == 6)
    {
        header.nReadSectorSize = CPK_READ_SECTOR_SIZE;
        header.nCompSectorSize = CPK_COMP_SECTOR_SIZE;
        m_nCurrentReadOffset -= sizeof(header.nReadSectorSize) + sizeof(header.nCompSectorSize);
    }
    else if (header.dwPackageVersion != 7)
    {
        m_nResult = IORESULT_WRONG_VERSION;
        return FALSE;
    }

    m_dwFlags = header.dwFlags;
    m_nFileCount = header.nFileCount;
    m_nFileSizeBitCount = header.nFileSizeBitCount;
    m_nFileLocationCountBitCount = header.nFileLocationCountBitCount;
    m_nFileLocationIndexBitCount = header.nFileLocationIndexBitCount;
    m_nLocationBitCount = header.nLocationBitCount;
    m_nCompSectorToDecompOffsetBitCount = header.nCompSectorToDecompOffsetBitCount;
    m_nDecompSectorToCompSectorBitCount = header.nDecompSectorToCompSectorBitCount;
    m_nDecompressedFileSize = header.nDecompressedFileSize;
    m_dwCRC = header.dwCRC;
    m_nReadSectorSize = header.nReadSectorSize;
    m_nCompSectorSize = header.nCompSectorSize;

    int32 nHeaderPadSize;
    if (header.dwFlags & 8)
    {
        m_nHeaderReadSectorCount = (header.nHeaderSector + header.nReadSectorSize - 1) / header.nReadSectorSize;
        m_nReadSectorCount = (m_nFileSize - header.nHeaderSector + header.nReadSectorSize - 1) / header.nReadSectorSize;
        m_nReadSectorCount += m_nHeaderReadSectorCount;
        m_nHeaderSize = header.nHeaderSector;
        nHeaderPadSize = 1;
    }
    else
    {
        m_nReadSectorCount = (m_nFileSize + header.nReadSectorSize - 1) / header.nReadSectorSize;
        m_nHeaderReadSectorCount = header.nHeaderSector;
        m_nHeaderSize = header.nHeaderSector * header.nReadSectorSize;
        nHeaderPadSize = header.nReadSectorSize;
    }
    m_nCompSectorCount = (m_nFileSize - header.nHeaderSector * nHeaderPadSize + header.nCompSectorSize - 1) / header.nCompSectorSize;
    int32 nDecompSectorCount = (header.nDecompressedFileSize + header.nCompSectorSize - 1) / header.nCompSectorSize;

    int32 nFileHashBitCount = 64;
    int32 nLocationBitCount = header.nLocationBitCount;
    int32 nCompSectorToDecompOffsetBitCount = header.nLocationBitCount;
    int32 nFileInfoBitCount = header.nFileSizeBitCount + header.nFileLocationCountBitCount + header.nFileLocationIndexBitCount + nFileHashBitCount;
    int32 nAllFileInfoSize = ((nFileInfoBitCount * header.nFileCount + 7) & ~7) / 8;

    int32 nAllLocationsSize = ((header.nLocationCount * nLocationBitCount + 7) & ~7) / 8;

    int32 nAllCompSectorToDecompOffsetSize = ((nCompSectorToDecompOffsetBitCount * m_nCompSectorCount + 7) & ~7) / 8;

    int32 nDecompSectorToCompSectorBitCount = sHighestBit(m_nCompSectorCount);
    int32 nAllDecompSectorToCompSectorSize = ((nDecompSectorToCompSectorBitCount * nDecompSectorCount + 7) & ~7) / 8;

    if (m_eArchiveType == CPK_ARCHIVE_TYPE_STANDARD)
    {
        m_pSortedFileInfo = new uint8[(size_t)nAllFileInfoSize];
        if (!Read(readBuffer, m_pSortedFileInfo, nAllFileInfoSize))
        {
            m_nResult = IORESULT_FILE_IO_ERROR;
            return FALSE;
        }

        m_pLocations = (uint8 *)SigmaMemoryNew(nAllLocationsSize, 4, NULL, FALSE);
        if (!Read(readBuffer, m_pLocations, nAllLocationsSize))
        {
            m_nResult = IORESULT_FILE_IO_ERROR;
            return FALSE;
        }

        if (IsCompressed())
        {
            m_pCompressedSectorToDecompressedOffset = (uint8 *)SigmaMemoryNew(nAllCompSectorToDecompOffsetSize, 4, NULL, FALSE);
            if (!Read(readBuffer, m_pCompressedSectorToDecompressedOffset, nAllCompSectorToDecompOffsetSize))
            {
                m_nResult = IORESULT_FILE_IO_ERROR;
                return FALSE;
            }

            m_pDecompressedSectorToCompressedSector = (uint8 *)SigmaMemoryNew(nAllDecompSectorToCompSectorSize, 4, NULL, FALSE);
            if (!Read(readBuffer, m_pDecompressedSectorToCompressedSector, nAllDecompSectorToCompSectorSize))
            {
                m_nResult = IORESULT_FILE_IO_ERROR;
                return FALSE;
            }
        }

        if (fLoadFilenamesInMemory)
        {
            m_pFilenameArrayOffsets = (int32 *)SigmaMemoryNew(header.nFileCount * sizeof(int32), 4, NULL, FALSE);
            if (!Read(readBuffer, m_pFilenameArrayOffsets, header.nFileCount * sizeof(int32)))
            {
                m_nResult = IORESULT_FILE_IO_ERROR;
                return FALSE;
            }
            if (fSwap)
            {
                for (int32 nFile = 0; nFile < header.nFileCount; nFile++)
                {
                    Macro_Endian_SwizzleT<4>(&m_pFilenameArrayOffsets[nFile]);
                }
            }

            int32 nFilenamesMaxSize = header.nHeaderSector * nHeaderPadSize - m_nCurrentReadOffset;
            m_szFilenames = (char *)SigmaMemoryNew(nFilenamesMaxSize, 4, NULL, FALSE);
            if (!Read(readBuffer, m_szFilenames, nFilenamesMaxSize))
            {
                m_nResult = IORESULT_FILE_IO_ERROR;
                return FALSE;
            }
        }
        else
        {
            m_nFilenameArrayFileOffset = m_nCurrentReadOffset;
        }
    }

    if (fValidateCRC)
    {
        uint32 dwCRC = 0;
        for (int32 i = m_nHeaderReadSectorCount; i < m_nReadSectorCount; i++)
        {
            ThreadIndex nTaskCreator;  // never initialized; the original leaves w5 as is
            if (!ReadSector(readBuffer, i, NULL, NULL, nTaskCreator))
            {
                m_nResult = IORESULT_FILE_IO_ERROR;
                return FALSE;
            }
            int32 nValidBytes = readBuffer.GetReadSize();
            for (int32 j = 0; j < nValidBytes; j += m_nCompSectorSize)
            {
                int32 nValidSubBytes = Math::Min(nValidBytes - j, m_nCompSectorSize);
                dwCRC = Blizzard::Util::CRC32(readBuffer.GetReadBuffer() + j, nValidSubBytes, dwCRC);
            }
        }
        if (dwCRC != m_dwCRC)
        {
            m_nResult = IORESULT_CRC_VALIDATION_FAILED;
            return FALSE;
        }
    }
    return TRUE;
}

// NON_MATCHING: register allocation only -- the header fields at 0x30/0x34 land in swapped
// registers, and the version-6 constants and the file-info madd operands come out in the other order.
// o02: on DWARF lines 447-629 (#line); 479/480 `else` + `if` (row 480 col 9 is a 4-indent condition), case bodies at
// 12-indent (494/498 cols), nHeaderPadSize = 1 placed before the 529 test (its value has no row of its own; inferred).
#line 447
BOOL CPKArchiveReader::InitializeArchiveFromMemory(
    BOOL fValidateCRC)
{
    m_nCurrentReadOffset = 0;


    Console::IO::CPKArchive::Header header = *(const Console::IO::CPKArchive::Header *)m_pArchiveData;
    m_nCurrentReadOffset += sizeof(header);

    BOOL fSwap = header.dwMagicNumber == 0xD4C3B2A1;
    m_fByteSwap = fSwap;


    if (fSwap)
    {
        Macro_Endian_SwizzleT<4>(&header.dwPackageVersion);
        Macro_Endian_SwizzleT<4>(&header.nFileCount);
        Macro_Endian_SwizzleT<8>(&header.nDecompressedFileSize);
        Macro_Endian_SwizzleT<4>(&header.dwFlags);
        Macro_Endian_SwizzleT<4>(&header.nHeaderSector);
        Macro_Endian_SwizzleT<4>(&header.nLocationCount);
        Macro_Endian_SwizzleT<4>(&header.nFileSizeBitCount);
        Macro_Endian_SwizzleT<4>(&header.nFileLocationCountBitCount);
        Macro_Endian_SwizzleT<4>(&header.nFileLocationIndexBitCount);
        Macro_Endian_SwizzleT<4>(&header.nLocationBitCount);
        Macro_Endian_SwizzleT<4>(&header.nCompSectorToDecompOffsetBitCount);
        Macro_Endian_SwizzleT<4>(&header.nDecompSectorToCompSectorBitCount);
        Macro_Endian_SwizzleT<4>(&header.dwCRC);
        Macro_Endian_SwizzleT<4>(&header.nReadSectorSize);
        Macro_Endian_SwizzleT<4>(&header.nCompSectorSize);
    }

    else
    if (header.dwMagicNumber != 0xA1B2C3D4)
    {
        m_nResult = IORESULT_INVALID_HEADER;
        return FALSE;
    }


    switch (header.dwPackageVersion)
    {
        case 7:
            break;
        case 6:
            header.nReadSectorSize = CPK_READ_SECTOR_SIZE;
            header.nCompSectorSize = CPK_COMP_SECTOR_SIZE;
            m_nCurrentReadOffset -= sizeof(header.nReadSectorSize) + sizeof(header.nCompSectorSize);
            break;

        default:
            m_nResult = IORESULT_WRONG_VERSION;
            return FALSE;
    }



    m_dwFlags = header.dwFlags;
    m_nFileCount = header.nFileCount;
    m_nFileSizeBitCount = header.nFileSizeBitCount;
    m_nFileLocationCountBitCount = header.nFileLocationCountBitCount;
    m_nFileLocationIndexBitCount = header.nFileLocationIndexBitCount;
    m_nLocationBitCount = header.nLocationBitCount;
    m_nCompSectorToDecompOffsetBitCount = header.nCompSectorToDecompOffsetBitCount;
    m_nDecompSectorToCompSectorBitCount = header.nDecompSectorToCompSectorBitCount;
    m_nDecompressedFileSize = header.nDecompressedFileSize;
    m_dwCRC = header.dwCRC;
    m_nReadSectorSize = header.nReadSectorSize;
    m_nCompSectorSize = header.nCompSectorSize;
    m_nReadSectorCount = (m_nFileSize + header.nReadSectorSize - 1) / header.nReadSectorSize;




    int32 nHeaderPadSize;
    if (header.dwFlags & 8)
    {
        m_nHeaderSize = header.nHeaderSector;
        m_nHeaderReadSectorCount = (header.nHeaderSector + header.nReadSectorSize - 1) / header.nReadSectorSize;
        nHeaderPadSize = 1;


        if (m_nHeaderReadSectorCount != header.nHeaderSector / header.nReadSectorSize)
        {
            m_nReadSectorCount++;
        }
    }
    else
    {
        m_nHeaderReadSectorCount = header.nHeaderSector;
        m_nHeaderSize = header.nReadSectorSize * header.nHeaderSector;
        nHeaderPadSize = header.nReadSectorSize;
    }

    m_nCompSectorCount = (m_nFileSize + header.nCompSectorSize - 1 - nHeaderPadSize * header.nHeaderSector) / header.nCompSectorSize;
    int32 nDecompSectorCount = (header.nDecompressedFileSize + header.nCompSectorSize - 1) / header.nCompSectorSize;


    int32 nFileHashBitCount = 64;
    int32 nFileInfoBitCount = header.nFileLocationIndexBitCount + header.nFileLocationCountBitCount + header.nFileSizeBitCount + nFileHashBitCount;
    int32 nAllFileInfoSize = ((nFileInfoBitCount * header.nFileCount + 7) & ~7) / 8;


    int32 nLocationBitCount = header.nLocationBitCount;
    int32 nAllLocationsSize = ((header.nLocationCount * nLocationBitCount + 7) & ~7) / 8;


    int32 nCompSectorToDecompOffsetBitCount = header.nLocationBitCount;
    int32 nAllCompSectorToDecompOffsetSize = ((nCompSectorToDecompOffsetBitCount * m_nCompSectorCount + 7) & ~7) / 8;


    int32 nDecompSectorToCompSectorBitCount = sHighestBit(m_nCompSectorCount);
    int32 nAllDecompSectorToCompSectorSize = ((nDecompSectorToCompSectorBitCount * nDecompSectorCount + 7) & ~7) / 8;


    if (m_eArchiveType == CPK_ARCHIVE_TYPE_STANDARD)
    {
        m_pSortedFileInfo = (uint8 *)m_pArchiveData + m_nCurrentReadOffset;
        m_nCurrentReadOffset += nAllFileInfoSize;

        m_pLocations = (uint8 *)m_pArchiveData + m_nCurrentReadOffset;
        m_nCurrentReadOffset += nAllLocationsSize;

        if (IsCompressed())
        {
            m_pCompressedSectorToDecompressedOffset = (uint8 *)m_pArchiveData + m_nCurrentReadOffset;
            m_nCurrentReadOffset += nAllCompSectorToDecompOffsetSize;

            m_pDecompressedSectorToCompressedSector = (uint8 *)m_pArchiveData + m_nCurrentReadOffset;
            m_nCurrentReadOffset += nAllDecompSectorToCompSectorSize;
        }

        if (fSwap)
        {
            m_pFilenameArrayOffsets = (int32 *)SigmaMemoryNew(header.nFileCount * sizeof(int32), 4, NULL, FALSE);
            SigmaMemoryCopy(m_pFilenameArrayOffsets, m_pArchiveData + m_nCurrentReadOffset, header.nFileCount * sizeof(int32));

            for (int32 nFile = 0; nFile < header.nFileCount; nFile++)
            {
                Macro_Endian_SwizzleT<4>(&m_pFilenameArrayOffsets[nFile]);
            }
        }
        else
        {

            m_pFilenameArrayOffsets = (int32 *)(m_pArchiveData + m_nCurrentReadOffset);
        }

        m_nCurrentReadOffset += header.nFileCount * sizeof(int32);
        m_szFilenames = (char *)(m_pArchiveData + m_nCurrentReadOffset);
    }

    if (fValidateCRC)
    {

        uint32 dwCRC = 0;

        int32 nOffset = 0;
        if (m_dwFlags & 8)
        {
            nOffset = m_nHeaderReadSectorCount * m_nReadSectorSize - m_nHeaderSize;
        }

        for (int32 i = m_nHeaderReadSectorCount; i < m_nReadSectorCount; i++)
        {
            int32 nValidBytes = m_nFileSize - (m_nReadSectorSize * i - nOffset) < m_nReadSectorSize ? m_nFileSize - (m_nReadSectorSize * i - nOffset) : m_nReadSectorSize;

            for (int32 j = 0; j < nValidBytes; j += m_nCompSectorSize)
            {
                int32 nValidSubBytes = Math::Min(nValidBytes - j, m_nCompSectorSize);
                dwCRC = Blizzard::Util::CRC32(m_pArchiveData + m_nReadSectorSize * i - nOffset + j, nValidSubBytes, dwCRC);
            }
        }

        if (dwCRC != m_dwCRC)
        {
            m_nResult = IORESULT_CRC_VALIDATION_FAILED;
            return FALSE;
        }
    }

    return TRUE;
}
#line 494
inline BOOL CPKArchiveReader::Read(Console::IO::ReadBuffer & readBuffer, void * pBuffer, int32 nSize)
{
    int32 nOffsetInReadSector;
    while (nSize != 0)
    {
        if (m_nCurrentReadOffset > m_nFileSize)
            return FALSE;

        {
            int32 nSector = OffsetToReadSector(m_nCurrentReadOffset, nOffsetInReadSector, FALSE);
            if (!readBuffer.Contains(this, nSector))
            {
                // The task creator passed here is never initialized; the original leaves w5 as is.
                ThreadIndex nTaskCreator;
                if (!ReadSector(readBuffer, nSector, NULL, NULL, nTaskCreator))
                    return FALSE;
            }

            int32 nValidSize = Math::Min(readBuffer.GetReadSize() - nOffsetInReadSector, nSize);
            SigmaMemoryCopy(pBuffer, readBuffer.GetReadBuffer() + nOffsetInReadSector, nValidSize);
            nSize -= nValidSize;
            m_nCurrentReadOffset += nValidSize;
            pBuffer = (uint8 *)pBuffer + nValidSize;
        }
    }
    return TRUE;
}

BOOL CPKArchiveReader::ReadSector(Console::IO::ReadBuffer & readBuffer, int32 nSector, BOOL * pfReadFromFileSystem, BOOL * pfShouldReadAhead, ThreadIndex nTaskCreator)
{
    // The original inlines IsClosing() here and discards the result (likely a compiled-out
    // check); only its load of m_dwFlags survives.
    IsClosing();

    int32 nBytesToRead = m_nReadSectorSize;
    int32 nOffset = 0;
    if ((m_dwFlags & 8) && nSector >= m_nHeaderReadSectorCount)
        nOffset = m_nHeaderReadSectorCount * nBytesToRead - m_nHeaderSize;

    if (m_dwFlags & 4)
    {
        nBytesToRead = Math::Min<long>(m_nFileSize - ((int64)nBytesToRead * nSector - nOffset), nBytesToRead);
        SigmaMemoryCopy(readBuffer.GetReadBuffer(), m_pArchiveData + (int64)m_nReadSectorSize * nSector - nOffset, nBytesToRead);
    }
    else
    {
        if (!Blizzard::File::Read(m_tStream, readBuffer.GetReadBuffer(), (int64)nBytesToRead * nSector - nOffset, &nBytesToRead, 0))
        {
            readBuffer.SetInvalid();
            m_nResult = IORESULT_FILE_IO_ERROR;
            return FALSE;
        }
        if (pfReadFromFileSystem != NULL)
        {
            *pfReadFromFileSystem = TRUE;
        }
        if (pfShouldReadAhead != NULL)
        {
            *pfShouldReadAhead = TRUE;
        }
    }

    if (nBytesToRead != 0)
    {
        readBuffer.SetValid(this, nSector, nBytesToRead);
        return TRUE;
    }
    return FALSE;
}

int64 CPKArchiveReader::GetLocation(int32 nLocationIndex) const
{
    uint64 nLocation;
    sReadBits(m_pLocations, nLocationIndex * m_nLocationBitCount, m_nLocationBitCount, nLocation);
    return nLocation;
}

static inline int32 sBinarySearch(uint64 dwHash, int32 nLow, int32 nHigh, uint8 * pSortedFileInfo, int32 nFileInfoBitCount)
{
    uint64 dwElemHash;
    int32 nMid;
    do
    {
        nMid = nLow + ((nHigh - nLow) >> 1);
        sReadBits(pSortedFileInfo, nMid * nFileInfoBitCount, sizeof(dwElemHash) * 8, dwElemHash);
        if (dwElemHash == dwHash)
            return nMid;
        if (dwElemHash < dwHash)
            nLow = nMid + 1;
        else
            nHigh = nMid - 1;
    } while (nHigh >= nLow);
    return -1;
}

BOOL CPKArchiveReader::GetFileInfo(int32 nFileIndex, Console::IO::FileInfo & info) const
{
    int32 nFileInfoBitCount = m_nFileSizeBitCount + m_nFileLocationCountBitCount + m_nFileLocationIndexBitCount + 64;
    uint64 dwHash;
    uint32 nSize;
    uint32 nLocationCount;
    uint32 nLocationIndex;
    sReadBits(m_pSortedFileInfo, nFileIndex * nFileInfoBitCount, sizeof(dwHash) * 8, dwHash);
    sReadBits(m_pSortedFileInfo, nFileIndex * nFileInfoBitCount + sizeof(dwHash) * 8, m_nFileSizeBitCount, nSize);
    sReadBits(m_pSortedFileInfo, nFileIndex * nFileInfoBitCount + sizeof(dwHash) * 8 + m_nFileSizeBitCount, m_nFileLocationCountBitCount, nLocationCount);
    sReadBits(m_pSortedFileInfo, nFileIndex * nFileInfoBitCount + sizeof(dwHash) * 8 + m_nFileSizeBitCount + m_nFileLocationCountBitCount, m_nFileLocationIndexBitCount, nLocationIndex);
    info.dwHash = dwHash;
    info.nSize = nSize;
    info.nLocationCount = nLocationCount;
    info.nLocationIndex = nLocationIndex;
    info.nLocationIndexOverride = -1;
    m_nLastFileRecordAccessTime = Blizzard::Time::Milliseconds();
    return TRUE;
}

int32 CPKArchiveReader::GetFileIndex(uint64 dwFileHash) const
{
    uint64 dwLocaleOverrideHash = GetTextLocaleOverrideHash(dwFileHash);
    int32 nIndex = GetFileIndexInternal(dwLocaleOverrideHash);
    if (nIndex != -1)
        return nIndex;

    if (UnicodeText::GetStringDisplayLocale() != UnicodeText::GetCurrentLocale())
    {
        uint64 dwLocaleOverrideHash = GetLocaleOverrideHash(dwFileHash);
        nIndex = GetFileIndexInternal(dwLocaleOverrideHash);
        if (nIndex != -1)
            return nIndex;
    }

    return GetFileIndexInternal(dwFileHash);
}

inline int32 CPKArchiveReader::GetFileIndexInternal(uint64 dwFileHash) const
{
    int32 nFileCount = m_nFileCount;
    int32 nFileInfoBitCount = m_nFileSizeBitCount + m_nFileLocationCountBitCount + m_nFileLocationIndexBitCount + 64;
    if (nFileCount == 0)
        return -1;
    return sBinarySearch(dwFileHash, 0, nFileCount - 1, m_pSortedFileInfo, nFileInfoBitCount);
}

BOOL CPKArchiveReader::GetFilename(int32 nFileIndex, char * szFilenameBuffer, int32 nFilenameBufferSize) const
{
    Blizzard::File::Stream tStream;
    int32 nFilenamesFileOffset;
    int32 nOffset;
    if (m_nFilenameArrayFileOffset != -1 && m_pFilenameArrayOffsets == NULL)
    {
        tStream = NULL;
        if (!Blizzard::File::Open(m_szFilename, Blizzard::File::Perm::read | Blizzard::File::Perm::shRead | Blizzard::File::Mode::openExisting, tStream))
            return FALSE;
        if (!Blizzard::File::Read(tStream, &nOffset, m_nFilenameArrayFileOffset + nFileIndex * sizeof(int32), sizeof(int32), 0))
            return FALSE;
        nFilenamesFileOffset = m_nFilenameArrayFileOffset + m_nFileCount * sizeof(int32);
        if (!Blizzard::File::Read(tStream, szFilenameBuffer, (int64)nFilenamesFileOffset + nOffset, nFilenameBufferSize, 0))
            return FALSE;
        Blizzard::File::Close(tStream);
        return TRUE;
    }
    if (m_pFilenameArrayOffsets != NULL)
    {
        int32 nOffset = m_pFilenameArrayOffsets[nFileIndex];
        strncpy(szFilenameBuffer, m_szFilenames + nOffset, nFilenameBufferSize);
        return TRUE;
    }
    return FALSE;
}

void CPKArchiveReader::AddFileReference()
{
    Blizzard::Lock::Atomic::Increment(&m_nFileOpenCount);
    m_nLastFileRecordAccessTime = Blizzard::Time::Milliseconds();
}

void CPKArchiveReader::RemoveFileReference()
{
    int32 nCount = Blizzard::Lock::Atomic::Decrement(&m_nFileOpenCount);
    (void)nCount;
    m_nLastFileRecordAccessTime = Blizzard::Time::Milliseconds();
}

BOOL CPKArchiveReader::OpenFileRecord(uint64 dwFileHash, Console::IO::FileRecord ** pptRecord, Console::IO::FileInfo * ptInfo) const
{
    Console::IO::FileRecord * ptRecord;
    Console::IO::FileInfo info;
    if (ptInfo != NULL && ptInfo->dwHash == dwFileHash)
    {
        info = *ptInfo;
    }
    else
    {
        if (!GetFileInfo(dwFileHash, info))
            return FALSE;
        if (ptInfo != NULL)
            *ptInfo = info;
    }
    ptRecord = new Console::IO::FileRecord;
    ptRecord->ptArchive = const_cast<CPKArchiveReader *>(this);
    ptRecord->ptInternalBuffer = NULL;
    ptRecord->info = info;
    *pptRecord = ptRecord;
    const_cast<CPKArchiveReader *>(this)->AddFileReference();
    return TRUE;
}

BOOL CPKArchiveReader::CloseFileRecord(Console::IO::FileRecord * ptRecord) const
{
    ptRecord->ptArchive->RemoveFileReference();
    delete ptRecord;
    return TRUE;
}

int32 CPKArchiveReader::OffsetToCompSector(int64 nOffset, int32 & nOffsetInCompSector) const
{
    int32 nIndex = nOffset / m_nCompSectorSize;
    int32 nCompressedSector = DecompSectorToCompSector(nIndex);
    while (nCompressedSector < m_nCompSectorCount - 1 && CompSectorToOffset(nCompressedSector + 1) <= nOffset)
        nCompressedSector++;
    int64 nDecompressedOffset = CompSectorToOffset(nCompressedSector);
    nOffsetInCompSector = nOffset - nDecompressedOffset;
    return nCompressedSector;
}

// The DWARF gives this no locals, so the value is read back into the parameter.
inline int32 CPKArchiveReader::DecompSectorToCompSector(int32 nDecompSector) const
{
    sReadBits(m_pDecompressedSectorToCompressedSector, nDecompSector * m_nDecompSectorToCompSectorBitCount, m_nDecompSectorToCompSectorBitCount, (uint32 &)nDecompSector);
    return nDecompSector;
}

int64 CPKArchiveReader::CompSectorToOffset(int32 nCompSector) const
{
    uint64 nDecompressedOffset;
    sReadBits(m_pCompressedSectorToDecompressedOffset, nCompSector * m_nCompSectorToDecompOffsetBitCount, m_nCompSectorToDecompOffsetBitCount, nDecompressedOffset);
    return nDecompressedOffset;
}

inline int32 CPKArchiveReader::SizeToReadSectorCount(int32 nSize, int32 & nSizeInLastSector)
{
    nSizeInLastSector = nSize % m_nReadSectorSize;
    if (nSizeInLastSector == 0)
        nSizeInLastSector = m_nReadSectorSize;
    return (nSize + m_nReadSectorSize - 1) / m_nReadSectorSize;
}

inline int32 CPKArchiveReader::SizeToCompSectorCount(int32 nFirstCompSector, int32 nSize, int32 & nSizeInLastSector) const
{
    int32 nSectorCount = 0;
    int32 nLastSectorSize = 0;
    while (nSize > 0)
    {
        nLastSectorSize = GetDecompressedSectorSize(nFirstCompSector + nSectorCount);
        nSize -= nLastSectorSize;
        nSectorCount++;
    }
    nSizeInLastSector = nLastSectorSize + nSize;
    return nSectorCount;
}

int32 CPKArchiveReader::GetDecompressedSectorSize(int32 nSector) const
{
    if (nSector == m_nCompSectorCount - 1)
        return m_nDecompressedFileSize - CompSectorToOffset(nSector);
    return CompSectorToOffset(nSector + 1) - CompSectorToOffset(nSector);
}

void CPKArchiveReader::FindBestReadSectorInfo(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector, Console::IO::FileRecord * ptRecord, int32 nOffset, int32 nSize, int32 & nReadSectorOut, int32 & nReadSectorCountOut, int32 & nOffsetInFirstSectorOut, int32 & nSizeInLastSectorOut)
{
    const Console::IO::FileInfo & info = ptRecord->info;
    int32 nValidSectorCount = GetReadSectorCount();

    if (info.nLocationIndexOverride != -1)
    {
        int32 nLocationIndex = info.nLocationIndexOverride;
        nReadSectorOut = OffsetToReadSector(GetLocation(nLocationIndex) + nOffset, nOffsetInFirstSectorOut, TRUE);
        nReadSectorCountOut = SizeToReadSectorCount(nOffsetInFirstSectorOut + nSize, nSizeInLastSectorOut);
        if (nReadSectorOut + nReadSectorCountOut <= nValidSectorCount)
            return;
    }

    if (ptLastArchive != this)
    {
        int32 nLocationIndex = info.nLocationIndex;
        int32 nLocationCount = info.nLocationCount;
        for (int32 nLocation = 0; nLocation < nLocationCount; nLocation++)
        {
            nReadSectorOut = OffsetToReadSector(GetLocation(nLocationIndex + nLocation) + nOffset, nOffsetInFirstSectorOut, TRUE);
            nReadSectorCountOut = SizeToReadSectorCount(nOffsetInFirstSectorOut + nSize, nSizeInLastSectorOut);
            if (nReadSectorOut + nReadSectorCountOut <= nValidSectorCount)
                break;
        }
        return;
    }

    // DWARF: the nBest* locals are function-scope declarations at 1768-1774, after the two early-out blocks.
    int32 nBestSectorOffset = 0x7FFFFFFF;
    int32 nBestOffsetInFirstSector = 0;
    int32 nBestReadSector = -1;
    int32 nBestSizeInLastSector = 0;
    int32 nBestReadSectorCount = 0;
    for (int32 nLocation = 0; nLocation < info.nLocationCount; nLocation++)
    {
        int32 nOffsetInFirstSector;
        int32 nReadSector = OffsetToReadSector(GetLocation(info.nLocationIndex + nLocation) + nOffset, nOffsetInFirstSector, TRUE);
        int32 nSizeInLastSector;
        int32 nReadSectorCount = SizeToReadSectorCount(nOffsetInFirstSector + nSize, nSizeInLastSector);
        if (nReadSector + nReadSectorCount <= nValidSectorCount)
        {
            int32 nSectorOffset = abs(nReadSector - nLastSector);
            if (nSectorOffset < nBestSectorOffset)
            {
                nBestSectorOffset = nSectorOffset;
                nBestOffsetInFirstSector = nOffsetInFirstSector;
                nBestReadSector = nReadSector;
                nBestSizeInLastSector = nSizeInLastSector;
                nBestReadSectorCount = nReadSectorCount;
            }
        }
    }
    nReadSectorOut = nBestReadSector;
    nOffsetInFirstSectorOut = nBestOffsetInFirstSector;
    nSizeInLastSectorOut = nBestSizeInLastSector;
    nReadSectorCountOut = nBestReadSectorCount;
}

// NON_MATCHING: register allocation only -- x24/x28 swap roles in the best-location loop.
// o02: on DWARF lines 1806-1915 (#line); rows 1887 (cols 13/30/44) read `if (nReadSectorCount + nReadSector > nValidSectorCount)`
// with its own continue, then 1892 (col 27) the separate nSectorOffset test.
#line 1806
void CPKArchiveReader::FindBestReadAndCompSectorInfo(
    Console::IO::CPKArchiveReader * ptLastArchive,
    int32 nLastSector,
    Console::IO::FileRecord * ptRecord,
    int32 nOffset,
    int32 nSize,
    int32 & nReadSectorOut,
    int32 & nReadSectorCountOut,
    int32 & nOffsetInFirstSectorOut,
    int32 & nSizeInLastSectorOut,
    int32 & nCompSectorOut,
    int32 & nCompSectorCountOut,
    int32 & nCompSectorOffsetInFirstReadSectorOut)
{
    const Console::IO::FileInfo & info = ptRecord->info;

    int32 nValidSectorCount = GetReadSectorCount();



    if (info.nLocationIndexOverride != -1)
    {
        int32 nLocationIndex = info.nLocationIndexOverride;
        nCompSectorOut = OffsetToCompSector(GetLocation(nLocationIndex) + nOffset, nOffsetInFirstSectorOut);
        nReadSectorOut = CompSectorToReadSector(nCompSectorOut, nCompSectorOffsetInFirstReadSectorOut);

        nCompSectorCountOut = SizeToCompSectorCount(nCompSectorOut, nOffsetInFirstSectorOut + nSize, nSizeInLastSectorOut);
        int32 dwUnused;
        nReadSectorCountOut = CompSectorToReadSector(nCompSectorOut + nCompSectorCountOut - 1, dwUnused) + 1 - nReadSectorOut;
        if (nReadSectorCountOut + nReadSectorOut <= nValidSectorCount)
        {
            return;
        }
    }



    if (ptLastArchive != this)
    {
        int32 nLocationIndex = info.nLocationIndex;
        int32 nLocationCount = info.nLocationCount;
        for (int32 nLocation = 0; nLocation < nLocationCount; nLocation++)
        {
            nCompSectorOut = OffsetToCompSector(GetLocation(nLocationIndex + nLocation) + nOffset, nOffsetInFirstSectorOut);
            nReadSectorOut = CompSectorToReadSector(nCompSectorOut, nCompSectorOffsetInFirstReadSectorOut);

            nCompSectorCountOut = SizeToCompSectorCount(nCompSectorOut, nOffsetInFirstSectorOut + nSize, nSizeInLastSectorOut);
            int32 dwUnused;
            nReadSectorCountOut = CompSectorToReadSector(nCompSectorOut + nCompSectorCountOut - 1, dwUnused) + 1 - nReadSectorOut;
            if (nReadSectorCountOut + nReadSectorOut <= nValidSectorCount)
            {
                break;
            }
        }

        return;
    }


    int32 nBestSectorOffset = 0x7FFFFFFF;


    int32 nBestOffsetInFirstSector = 0;
    int32 nBestCompSectorOffsetInFirstReadSector = 0;
    int32 nBestCompSector = -1;
    int32 nBestReadSector = -1;
    int32 nBestSizeInLastSector = 0;
    int32 nBestCompSectorCount = 0;
    int32 nBestReadSectorCount = 0;
    for (int32 nLocation = 0; nLocation < info.nLocationCount; nLocation++)
    {
        int32 nOffsetInFirstSector;
        int32 nCompSectorOffsetInFirstReadSector;
        int32 nCompSector = OffsetToCompSector(GetLocation(info.nLocationIndex + nLocation) + nOffset, nOffsetInFirstSector);
        int32 nReadSector = CompSectorToReadSector(nCompSector, nCompSectorOffsetInFirstReadSector);
        int32 nSizeInLastSector;
        int32 nCompSectorCount = SizeToCompSectorCount(nCompSector, nOffsetInFirstSector + nSize, nSizeInLastSector);
        int32 dwUnused;
        int32 nReadSectorCount = CompSectorToReadSector(nCompSector + nCompSectorCount - 1, dwUnused) - nReadSector + 1;

        int32 nSectorOffset = abs(nReadSector - nLastSector);
        if (nReadSectorCount + nReadSector > nValidSectorCount)
        {
            continue;
        }

        if (nSectorOffset < nBestSectorOffset)
        {
            nBestSectorOffset = nSectorOffset;
            nBestOffsetInFirstSector = nOffsetInFirstSector;
            nBestCompSectorOffsetInFirstReadSector = nCompSectorOffsetInFirstReadSector;
            nBestCompSector = nCompSector;
            nBestReadSector = nReadSector;
            nBestSizeInLastSector = nSizeInLastSector;
            nBestCompSectorCount = nCompSectorCount;
            nBestReadSectorCount = nReadSectorCount;
        }
    }

    nOffsetInFirstSectorOut = nBestOffsetInFirstSector;
    nCompSectorOffsetInFirstReadSectorOut = nBestCompSectorOffsetInFirstReadSector;
    nCompSectorOut = nBestCompSector;
    nReadSectorOut = nBestReadSector;

    nSizeInLastSectorOut = nBestSizeInLastSector;
    nCompSectorCountOut = nBestCompSectorCount;
    nReadSectorCountOut = nBestReadSectorCount;


}
#line 899
static void sReadBits(uint8 * pSourceBuffer, int32 nBitIndex, int32 nBitCount, uint32 & dwDestValue)
{
    uint32 dwUncompressedValue = 0;
    while (nBitCount > 0)
    {
        int32 nShift = Math::Min(nBitCount, 8);
        dwUncompressedValue <<= nShift;
        int32 nBitOffset = nBitIndex & 7;
        int32 nByteOffset = nBitIndex >> 3;
        int32 nValidBits = 8 - nBitOffset;
        nShift = pSourceBuffer[nByteOffset] & ((1 << nValidBits) - 1);
        if (nBitCount < nValidBits)
        {
            nShift >>= nValidBits - nBitCount;
        }
        dwUncompressedValue |= (uint8)nShift;
        nBitIndex += nValidBits;
        nBitCount -= nValidBits;
    }
    dwDestValue = dwUncompressedValue;
}

static void sReadBits(uint8 * pSourceBuffer, int32 nBitIndex, int32 nBitCount, uint64 & dwDestValue)
{
    uint64 dwUncompressedValue = 0;
    while (nBitCount > 0)
    {
        int32 nShift = Math::Min(nBitCount, 8);
        dwUncompressedValue <<= nShift;
        int32 nBitOffset = nBitIndex & 7;
        int32 nByteOffset = nBitIndex >> 3;
        int32 nValidBits = 8 - nBitOffset;
        nShift = pSourceBuffer[nByteOffset] & ((1 << nValidBits) - 1);
        if (nBitCount < nValidBits)
        {
            nShift >>= nValidBits - nBitCount;
        }
        dwUncompressedValue |= (uint8)nShift;
        nBitIndex += nValidBits;
        nBitCount -= nValidBits;
    }
    dwDestValue = dwUncompressedValue;
}

}  // namespace IO
}  // namespace Console
