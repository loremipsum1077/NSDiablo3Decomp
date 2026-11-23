// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/IO/ConsoleIO_ArchiveReader.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleIO_Archive.h"
#include "../../ThreadGlobals.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { namespace IO { struct FileInfo; } }
namespace Console { namespace IO { struct FileRecord; } }
namespace Console { namespace IO { class ReadBuffer; } }

namespace Console {
namespace IO {
class CPKArchiveReader : public Console::IO::CPKArchive {  // size 0x10c8
public:
    enum {
        CPK_COMP_READ_CHUNK_SIZE = 16384,
        CPK_FLAG_CLOSING = 6,
    };
    enum CPKArchiveTypes {
        CPK_ARCHIVE_TYPE_STANDARD = 1,
        CPK_ARCHIVE_TYPE_CACHE = 2,
    };
public:
    CPKArchiveReader(const char *, Console::IO::ReadBuffer &, BOOL, BOOL, BOOL, Console::IO::CPKArchiveReader::CPKArchiveTypes);
    CPKArchiveReader(const char *, const uint8 *, int64, BOOL);
    virtual ~CPKArchiveReader();  // vtable slot 0
    uint32 GetCRC() const;
    void FindBestReadSectorInfo(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector, Console::IO::FileRecord * ptRecord, int32 nOffset, int32 nSize, int32 & nReadSectorOut, int32 & nReadSectorCountOut, int32 & nOffsetInFirstSectorOut, int32 & nSizeInLastSectorOut);
    void FindBestReadAndCompSectorInfo(Console::IO::CPKArchiveReader * ptLastArchive, int32 nLastSector, Console::IO::FileRecord * ptRecord, int32 nOffset, int32 nSize, int32 & nReadSectorOut, int32 & nReadSectorCountOut, int32 & nOffsetInFirstSectorOut, int32 & nSizeInLastSectorOut, int32 & nCompSectorOut, int32 & nCompSectorCountOut, int32 & nCompSectorOffsetInFirstReadSectorOut);
    BOOL ReadSector(Console::IO::ReadBuffer & readBuffer, int32 nSector, BOOL * pfReadFromFileSystem, BOOL * pfShouldReadAhead, ThreadIndex nTaskCreator);
    BOOL DecompressSector(int32, uint8 *, int32, uint8 *, int32, int32);
    int64 GetLocation(int32 nLocationIndex) const;
    int64 GetDecompressedFileSize() const;
    void SetPriority(int32 nPriority);
    int32 GetPriority() const;
    Console::IO::CPKArchiveReader::CPKArchiveTypes GetArchiveType() const;
    BOOL GetFilename(int32 nFileIndex, char * szFilenameBuffer, int32 nFilenameBufferSize) const;
    BOOL GetFilename(uint64 dwFileHash, char * szFilenameBuffer, int32 nFilenameBufferSize) const;
    int32 GetFileCount() const;
    BOOL GetFileInfo(int32 nFileIndex, Console::IO::FileInfo & info) const;
    BOOL GetFileInfo(const char * szFilename, Console::IO::FileInfo & info) const;
    BOOL GetFileInfo(uint64 dwFileHash, Console::IO::FileInfo & info) const;
    BOOL OpenFileRecord(const char * szFilename, Console::IO::FileRecord ** pptRecord, Console::IO::FileInfo * ptInfo) const;
    void AddFileReference();
    void RemoveFileReference();
    BOOL OpenFileRecord(uint64 dwFileHash, Console::IO::FileRecord ** pptRecord, Console::IO::FileInfo * ptInfo) const;
    BOOL CloseFileRecord(Console::IO::FileRecord * ptRecord) const;
    int32 OffsetToCompSector(int64 nOffset, int32 & nOffsetInCompSector) const;
    int64 CompSectorToOffset(int32 nCompSector) const;
    int32 DecompSectorToCompSector(int32 nDecompSector) const;
    int32 CompSectorToReadSector(int32 nCompSector, int32 & nCompSectorOffsetInReadSector) const;
    int32 ReadSectorToCompSector(int32 nReadSector);
    int32 OffsetToReadSector(int64 nOffset, int32 & nOffsetInReadSector, BOOL fSkipHeader) const;
    int32 SizeToReadSectorCount(int32 nSize, int32 & nSizeInLastSector);
    int32 SizeToCompSectorCount(int32 nFirstCompSector, int32 nSize, int32 & nSizeInLastSector) const;
    int32 GetReadSectorSize();
    int32 GetCompSectorSize();
    int32 GetCompSectorPerReadSector();
    int32 GetHeaderReadSectorCount() const;
    int32 GetReadSectorCount() const;
    int32 GetCompSectorCount() const;
    int32 GetOpenedFileRecordCount() const;
    Blizzard::Time::Millisecond GetLastFileRecordAccessMilliseconds() const;
    BOOL IsClosing() const;
    void SetClosing(BOOL fClosing);
    int32 GetDecompressedSectorSize(int32 nSector) const;
    BOOL GetCompressedData(Console::IO::ReadBuffer &, const char *, void *&, int32 &);
    BOOL GetCompressedData(Console::IO::ReadBuffer & readBuffer, uint64 dwHash, void *& pCompressedData, int32 & nCompressedDataSize);
    BOOL GetDecompressedData(Console::IO::ReadBuffer &, const char *, void *&, int32 &);
    BOOL GetDecompressedData(Console::IO::ReadBuffer & readBuffer, uint64 dwHash, void *& pDecompressedData, int32 & nDecompressedDataSize);
    BOOL ExtractFile(Console::IO::ReadBuffer &, const char *, const char *);
    BOOL ExtractFile(Console::IO::ReadBuffer & readBuffer, uint64 dwHash, const char * szFilename);
    int64 GetArchiveFileSize() const;
    BOOL NeedsByteSwap() const;
protected:
    int64 m_nFileSize;  // 0x1010
    int64 m_nDecompressedFileSize;  // 0x1018
    uint32 m_dwCRC;  // 0x1020
    int64 m_nCurrentReadOffset;  // 0x1028
    int32 m_nReadSectorCount;  // 0x1030
    int32 m_nCompSectorCount;  // 0x1034
    int32 m_nReadSectorSize;  // 0x1038
    int32 m_nCompSectorSize;  // 0x103c
    uint8 * m_pDecompressedSectorToCompressedSector;  // 0x1040
    uint8 * m_pCompressedSectorToDecompressedOffset;  // 0x1048
    Blizzard::File::Stream m_tStream;  // 0x1050
    uint8 * m_pSortedFileInfo;  // 0x1058
    int32 m_nFileCount;  // 0x1060
    int32 m_nPriority;  // 0x1064
    uint8 * m_pLocations;  // 0x1068
    int32 m_nFileSizeBitCount;  // 0x1070
    int32 m_nFileLocationCountBitCount;  // 0x1074
    int32 m_nFileLocationIndexBitCount;  // 0x1078
    int32 m_nLocationBitCount;  // 0x107c
    int32 m_nCompSectorToDecompOffsetBitCount;  // 0x1080
    int32 m_nDecompSectorToCompSectorBitCount;  // 0x1084
    int32 m_nHeaderReadSectorCount;  // 0x1088
    int32 m_nHeaderSize;  // 0x108c
    int32 m_nFilenameArrayFileOffset;  // 0x1090
    int32 * m_pFilenameArrayOffsets;  // 0x1098
    char * m_szFilenames;  // 0x10a0
    volatile int32 m_nFileOpenCount;  // 0x10a8
    mutable Blizzard::Time::Millisecond m_nLastFileRecordAccessTime;  // 0x10ac
    Console::IO::CPKArchiveReader::CPKArchiveTypes m_eArchiveType;  // 0x10b0
    const uint8 * m_pArchiveData;  // 0x10b8
    BOOL m_fByteSwap;  // 0x10c0
    BOOL InitializeArchive(Console::IO::ReadBuffer & readBuffer, BOOL fLoadFilenamesInMemory, BOOL fSkipSize, BOOL fValidateCRC);
    BOOL InitializeArchiveFromMemory(BOOL fValidateCRC);
    BOOL Read(Console::IO::ReadBuffer & readBuffer, void * pBuffer, int32 nSize);
    int32 GetFileIndex(uint64 dwFileHash) const;
    int32 GetFileIndexInternal(uint64 dwFileHash) const;
    BOOL GetDataFromCompArchive(Console::IO::ReadBuffer &, uint64, void *&, int32 &);
    BOOL GetDataFromUncompArchive(Console::IO::ReadBuffer &, uint64, void *&, int32 &);
    BOOL GetDecompDataFromCompArchive(Console::IO::ReadBuffer &, uint64, void *&, int32 &);
    BOOL ExtractCompressedFile(Console::IO::ReadBuffer &, uint64, const char *);
    BOOL ExtractUncompressedFile(Console::IO::ReadBuffer &, uint64, const char *);
    BOOL IsIdentical(Console::IO::CPKArchiveReader *);
    void AssignTables(Console::IO::CPKArchiveReader *);
    void UnassignTables();
};
static_assert(sizeof(CPKArchiveReader) == 0x10c8, "CPKArchiveReader");

// ConsoleIO_ArchiveReader.h:110
inline void CPKArchiveReader::SetPriority(int32 nPriority) {
    m_nPriority = nPriority;
}

// ConsoleIO_ArchiveReader.h:116
inline int32 CPKArchiveReader::GetPriority() const {
    return m_nPriority;
}

// ConsoleIO_ArchiveReader.h:121
inline CPKArchiveReader::CPKArchiveTypes CPKArchiveReader::GetArchiveType() const {
    return m_eArchiveType;
}

// ConsoleIO_ArchiveReader.h:131
inline BOOL CPKArchiveReader::GetFilename(uint64 dwFileHash, char * szFilenameBuffer, int32 nFilenameBufferSize) const {
    int32 nIndex = GetFileIndex(dwFileHash);
    if (nIndex == -1)
        return FALSE;
    return GetFilename(nIndex, szFilenameBuffer, nFilenameBufferSize);
}

// ConsoleIO_ArchiveReader.h:145
inline int32 CPKArchiveReader::GetFileCount() const {
    return m_nFileCount;
}

// ConsoleIO_ArchiveReader.h:154
inline BOOL CPKArchiveReader::GetFileInfo(const char * szFilename, Console::IO::FileInfo & info) const {
    return GetFileInfo(Console::IO::CPKArchive::GetFileHash(szFilename), info);
}

// ConsoleIO_ArchiveReader.h:161
inline BOOL CPKArchiveReader::GetFileInfo(uint64 dwFileHash, Console::IO::FileInfo & info) const {
    int32 nIndex = GetFileIndex(dwFileHash);
    if (nIndex == -1)
        return FALSE;
    return GetFileInfo(nIndex, info);
}

// ConsoleIO_ArchiveReader.h:174
inline BOOL CPKArchiveReader::OpenFileRecord(const char * szFilename, Console::IO::FileRecord ** pptRecord, Console::IO::FileInfo * ptInfo) const {
    return OpenFileRecord(Console::IO::CPKArchive::GetFileHash(szFilename), pptRecord, ptInfo);
}

// ConsoleIO_ArchiveReader.h:204
inline int32 CPKArchiveReader::CompSectorToReadSector(int32 nCompSector, int32 & nCompSectorOffsetInReadSector) const {
    int32 nReadSector = (int32)(((int64)nCompSector * m_nCompSectorSize) / m_nReadSectorSize);
    nCompSectorOffsetInReadSector = nCompSector - (int32)(((int64)nReadSector * m_nReadSectorSize) / m_nCompSectorSize);
    return nReadSector + m_nHeaderReadSectorCount;
}

// ConsoleIO_ArchiveReader.h:217
inline int32 CPKArchiveReader::ReadSectorToCompSector(int32 nReadSector) {
    return (nReadSector - m_nHeaderReadSectorCount) * GetCompSectorPerReadSector();
}

// ConsoleIO_ArchiveReader.h:225
inline int32 CPKArchiveReader::OffsetToReadSector(int64 nOffset, int32 & nOffsetInReadSector, BOOL fSkipHeader) const {
    nOffsetInReadSector = nOffset % m_nReadSectorSize;
    if (fSkipHeader)
        return (int32)(nOffset / m_nReadSectorSize) + m_nHeaderReadSectorCount;
    return (int32)(nOffset / m_nReadSectorSize);
}

// ConsoleIO_ArchiveReader.h:247
inline int32 CPKArchiveReader::GetReadSectorSize() {
    return m_nReadSectorSize;
}

// ConsoleIO_ArchiveReader.h:252
inline int32 CPKArchiveReader::GetCompSectorSize() {
    return m_nCompSectorSize;
}

// ConsoleIO_ArchiveReader.h:257
inline int32 CPKArchiveReader::GetCompSectorPerReadSector() {
    return m_nReadSectorSize / m_nCompSectorSize;
}

// ConsoleIO_ArchiveReader.h:262
inline int32 CPKArchiveReader::GetHeaderReadSectorCount() const {
    return m_nHeaderReadSectorCount;
}

// ConsoleIO_ArchiveReader.h:267
inline int32 CPKArchiveReader::GetReadSectorCount() const {
    return m_nReadSectorCount;
}

// ConsoleIO_ArchiveReader.h:272
inline int32 CPKArchiveReader::GetCompSectorCount() const {
    return m_nCompSectorCount;
}

// ConsoleIO_ArchiveReader.h:277
inline int32 CPKArchiveReader::GetOpenedFileRecordCount() const {
    return m_nFileOpenCount;
}

// ConsoleIO_ArchiveReader.h:282
inline Blizzard::Time::Millisecond CPKArchiveReader::GetLastFileRecordAccessMilliseconds() const {
    return m_nLastFileRecordAccessTime;
}

// ConsoleIO_ArchiveReader.h:287
inline BOOL CPKArchiveReader::IsClosing() const {
    return (m_dwFlags & (1 << CPK_FLAG_CLOSING)) != 0;
}

// ConsoleIO_ArchiveReader.h:292
inline void CPKArchiveReader::SetClosing(BOOL fClosing) {
    if (fClosing)
        m_dwFlags |= (1 << CPK_FLAG_CLOSING);
    else
        m_dwFlags &= ~(1 << CPK_FLAG_CLOSING);
}

}  // namespace IO
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line   110  void Console::IO::CPKArchiveReader::SetPriority(int32 nPriority)  [inlined into 3 functions]
//   line   116  int32 Console::IO::CPKArchiveReader::GetPriority() const  [inlined into 1 functions]
//   line   121  Console::IO::CPKArchiveReader::CPKArchiveTypes Console::IO::CPKArchiveReader::GetArchiveType() const  [inlined into 2 functions]
//   line   131  BOOL Console::IO::CPKArchiveReader::GetFilename(uint64 dwFileHash, char * szFilenameBuffer, int32 nFilenameBufferSize) const  [inlined into 1 functions]
//   line   145  int32 Console::IO::CPKArchiveReader::GetFileCount() const  [inlined into 2 functions]
//   line   154  BOOL Console::IO::CPKArchiveReader::GetFileInfo(const char * szFilename, Console::IO::FileInfo & info) const  [inlined into 1 functions]
//   line   161  BOOL Console::IO::CPKArchiveReader::GetFileInfo(uint64 dwFileHash, Console::IO::FileInfo & info) const  [inlined into 3 functions]
//   line   174  BOOL Console::IO::CPKArchiveReader::OpenFileRecord(const char * szFilename, Console::IO::FileRecord ** pptRecord, Console::IO::FileInfo * ptInfo) const  [inlined into 2 functions]
//   line   204  int32 Console::IO::CPKArchiveReader::CompSectorToReadSector(int32 nCompSector, int32 & nCompSectorOffsetInReadSector) const  [inlined into 4 functions]
//   line   217  int32 Console::IO::CPKArchiveReader::ReadSectorToCompSector(int32 nReadSector)  [inlined into 1 functions]
//   line   225  int32 Console::IO::CPKArchiveReader::OffsetToReadSector(int64 nOffset, int32 & nOffsetInReadSector, BOOL fSkipHeader) const  [inlined into 4 functions]
//   line   247  int32 Console::IO::CPKArchiveReader::GetReadSectorSize()  [inlined into 3 functions]
//   line   252  int32 Console::IO::CPKArchiveReader::GetCompSectorSize()  [inlined into 2 functions]
//   line   257  int32 Console::IO::CPKArchiveReader::GetCompSectorPerReadSector()  [inlined into 2 functions]
//   line   262  int32 Console::IO::CPKArchiveReader::GetHeaderReadSectorCount() const  [inlined into 1 functions]
//   line   267  int32 Console::IO::CPKArchiveReader::GetReadSectorCount() const  [inlined into 3 functions]
//   line   272  int32 Console::IO::CPKArchiveReader::GetCompSectorCount() const  [inlined into 1 functions]
//   line   277  int32 Console::IO::CPKArchiveReader::GetOpenedFileRecordCount() const  [inlined into 2 functions]
//   line   282  Blizzard::Time::Millisecond Console::IO::CPKArchiveReader::GetLastFileRecordAccessMilliseconds() const  [inline]
//   line   287  BOOL Console::IO::CPKArchiveReader::IsClosing() const  [inlined into 2 functions]
//   line   292  void Console::IO::CPKArchiveReader::SetClosing(BOOL fClosing)  [inlined into 2 functions]
