// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/IO/ConsoleIO.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console { namespace IO { class CPKArchiveReader; } }
namespace Console { namespace IO { struct FileRecord; } }

namespace Console {
namespace IO {
enum Results {
    IORESULT_SUCCESS = 0,
    IORESULT_CANCELED = 1,
    IORESULT_FILE_NOT_FOUND = 2,
    IORESULT_FILE_IO_ERROR = 3,
    IORESULT_WRONG_VERSION = 4,
    IORESULT_INVALID_HEADER = 5,
    IORESULT_COMPRESSION_ERROR = 6,
    IORESULT_CRC_VALIDATION_FAILED = 7,
    IORESULT_NOT_ENOUGH_SPACE = 8,
    IORESULT_FAILED = 9,
};

enum TaskPriorities {
    TASKPRIORITY_HIGHEST = 1000,
    TASKPRIORITY_HIGH = 750,
    TASKPRIORITY_MEDIUM = 500,
    TASKPRIORITY_LOW = 250,
    TASKPRIORITY_LOWEST = 0,
};

enum CPKArchiveSizes {
    CPK_READ_SECTOR_SIZE = 65536,
    CPK_COMP_SECTOR_SIZE = 16384,
    CPK_MAX_DECOMP_BUFFER_SIZE = 65536,
};

struct InternalBuffer {  // size 0x28
    uint8 * pBuffer;  // 0x00
    int32 nCapacity;  // 0x08
    int32 nSize;  // 0x0c
    int64 nOffset;  // 0x10
    Console::IO::CPKArchiveReader * ptLastArchive;  // 0x18
    Console::IO::FileRecord * ptOwnerRecord;  // 0x20
    BOOL Contains(Console::IO::FileRecord * ptRecord, int32 nFileOffset, uint8 *& pValidData, int32 & nValidDataSize);
};
static_assert(sizeof(InternalBuffer) == 0x28, "InternalBuffer");

class ReadBuffer {  // size 0x10010
public:
    ReadBuffer();
    uint8 * GetReadBuffer();
    int32 GetReadSize() const;
    int32 GetSector() const;
    Console::IO::CPKArchiveReader * GetArchive() const;
    BOOL IsValid() const;
    BOOL Contains(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nSector);
    void SetInvalid();
    void Copy(const Console::IO::ReadBuffer & readBuffer);
    void SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nSector, int32 nReadSize);
private:
    uint8 m_pBuffer[65536];  // 0x00
    Console::IO::CPKArchiveReader * m_ptArchiveReader;  // 0x10000
    int32 m_nSector;  // 0x10008
    int32 m_nReadSize;  // 0x1000c
};
static_assert(sizeof(ReadBuffer) == 0x10010, "ReadBuffer");

class DecompBuffer {  // size 0x10018
public:
    DecompBuffer();
    uint8 * GetDecompBuffer();
    int32 GetDecompSize() const;
    int32 GetSubOffset() const;
    int32 GetCompSector() const;
    Console::IO::CPKArchiveReader * GetArchive() const;
    BOOL IsValid() const;
    BOOL Contains(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nSubOffset);
    void SetInvalid();
    void Copy(const Console::IO::DecompBuffer & decompBuffer);
    void SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nSubOffset, int32 nDecompSize);
private:
    uint8 m_pBuffer[65536];  // 0x00
    Console::IO::CPKArchiveReader * m_ptArchiveReader;  // 0x10000
    int32 m_nCompSector;  // 0x10008
    int32 m_nSubOffset;  // 0x1000c
    int32 m_nDecompSize;  // 0x10010
};
static_assert(sizeof(DecompBuffer) == 0x10018, "DecompBuffer");

class CompBuffer {  // size 0x4010
public:
    CompBuffer();
    uint8 * GetCompBuffer();
    int32 GetCompSize() const;
    int32 GetCompSector() const;
    Console::IO::CPKArchiveReader * GetArchive() const;
    BOOL IsValid() const;
    BOOL Contains(Console::IO::CPKArchiveReader *, int32);
    void SetInvalid();
    void Copy(const Console::IO::CompBuffer &);
    void SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nCompSize);
private:
    uint8 m_pBuffer[16384];  // 0x00
    Console::IO::CPKArchiveReader * m_ptArchiveReader;  // 0x4000
    int32 m_nCompSector;  // 0x4008
    int32 m_nCompSize;  // 0x400c
};
static_assert(sizeof(CompBuffer) == 0x4010, "CompBuffer");

struct FileInfo {  // size 0x18
    uint64 dwHash;  // 0x00
    int32 nSize;  // 0x08
    int32 nLocationCount;  // 0x0c
    int32 nLocationIndex;  // 0x10
    int32 nLocationIndexOverride;  // 0x14
};
static_assert(sizeof(FileInfo) == 0x18, "FileInfo");

struct FileRecord {  // size 0x28
    Console::IO::FileInfo info;  // 0x00
    Console::IO::CPKArchiveReader * ptArchive;  // 0x18
    Console::IO::InternalBuffer * ptInternalBuffer;  // 0x20
};
static_assert(sizeof(FileRecord) == 0x28, "FileRecord");

struct ReadAheadSuggestion {  // size 0x20
    Console::IO::CPKArchiveReader * ptLastArchive;  // 0x00
    int32 nLastSector;  // 0x08
    Console::IO::CPKArchiveReader * ptNextArchive;  // 0x10
    int32 nNextSector;  // 0x18
};
static_assert(sizeof(ReadAheadSuggestion) == 0x20, "ReadAheadSuggestion");

// ConsoleIO.h:71
inline ReadBuffer::ReadBuffer() {
    SetInvalid();
}

// Not in the DWARF's inline list (it inlines to no instructions), so its line is unknown.
inline uint8 * ReadBuffer::GetReadBuffer() {
    return m_pBuffer;
}

// ConsoleIO.h:81
inline int32 ReadBuffer::GetReadSize() const {
    return m_nReadSize;
}

// ConsoleIO.h:86
inline int32 ReadBuffer::GetSector() const {
    return m_nSector;
}

// ConsoleIO.h:91
inline Console::IO::CPKArchiveReader * ReadBuffer::GetArchive() const {
    return m_ptArchiveReader;
}

// ConsoleIO.h:96
inline BOOL ReadBuffer::IsValid() const {
    return m_ptArchiveReader != NULL;
}

// ConsoleIO.h:101
inline BOOL ReadBuffer::Contains(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nSector) {
    return m_ptArchiveReader == ptArchiveReader && m_nSector == nSector;
}

// ConsoleIO.h:108
inline void ReadBuffer::SetInvalid() {
    m_ptArchiveReader = NULL;
    m_nSector = -1;
    m_nReadSize = 0;
}

// ConsoleIO.h:119
inline void ReadBuffer::SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nSector, int32 nReadSize) {
    m_ptArchiveReader = ptArchiveReader;
    m_nSector = nSector;
    m_nReadSize = nReadSize;
}

// ConsoleIO.h:140
inline DecompBuffer::DecompBuffer() {
    SetInvalid();
}

// ConsoleIO.h:145
inline uint8 * DecompBuffer::GetDecompBuffer() {
    return m_pBuffer;
}

// ConsoleIO.h:150
inline int32 DecompBuffer::GetDecompSize() const {
    return m_nDecompSize;
}

// ConsoleIO.h:155
inline int32 DecompBuffer::GetSubOffset() const {
    return m_nSubOffset;
}

// ConsoleIO.h:160
inline int32 DecompBuffer::GetCompSector() const {
    return m_nCompSector;
}

// ConsoleIO.h:165
inline Console::IO::CPKArchiveReader * DecompBuffer::GetArchive() const {
    return m_ptArchiveReader;
}

// ConsoleIO.h:170
inline BOOL DecompBuffer::IsValid() const {
    return m_ptArchiveReader != NULL;
}

// ConsoleIO.h:175
inline BOOL DecompBuffer::Contains(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nSubOffset) {
    return m_ptArchiveReader == ptArchiveReader && m_nCompSector == nCompSector &&
           m_nSubOffset <= nSubOffset && nSubOffset < m_nSubOffset + m_nDecompSize;
}

// ConsoleIO.h:183
inline void DecompBuffer::SetInvalid() {
    m_ptArchiveReader = NULL;
    m_nSubOffset = -1;
    m_nCompSector = -1;
    m_nDecompSize = 0;
}

// ConsoleIO.h:195
inline void DecompBuffer::SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nSubOffset, int32 nDecompSize) {
    m_ptArchiveReader = ptArchiveReader;
    m_nCompSector = nCompSector;
    m_nSubOffset = nSubOffset;
    m_nDecompSize = nDecompSize;
}

// ConsoleIO.h:218
inline CompBuffer::CompBuffer() {
    SetInvalid();
}

// ConsoleIO.h:223
inline uint8 * CompBuffer::GetCompBuffer() {
    return m_pBuffer;
}

// ConsoleIO.h:228
inline int32 CompBuffer::GetCompSize() const {
    return m_nCompSize;
}

// ConsoleIO.h:233
inline int32 CompBuffer::GetCompSector() const {
    return m_nCompSector;
}

// ConsoleIO.h:238
inline Console::IO::CPKArchiveReader * CompBuffer::GetArchive() const {
    return m_ptArchiveReader;
}

// ConsoleIO.h:243
inline BOOL CompBuffer::IsValid() const {
    return m_ptArchiveReader != NULL;
}

// ConsoleIO.h:255
inline void CompBuffer::SetInvalid() {
    m_ptArchiveReader = NULL;
    m_nCompSector = -1;
    m_nCompSize = 0;
}

// ConsoleIO.h:266
inline void CompBuffer::SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nCompSize) {
    m_ptArchiveReader = ptArchiveReader;
    m_nCompSector = nCompSector;
    m_nCompSize = nCompSize;
}

void InitializeForApp();
void DisposeForApp();
void Update();
void FreeInternalBuffer(Console::IO::InternalBuffer * ptInternalBuffer);
void InitializeForThread();
void DisposeForThread();
void SuggestAheadReadSector(const Console::IO::ReadAheadSuggestion & tReadAheadSuggestion, Console::IO::TaskPriorities nTaskPriority);
Console::IO::Results ReadRecord(Console::IO::FileRecord * ptRecord, void * pBuffer, int32 nFileOffset, int32 nBytesToRead, int32 * pnBytesRead, Console::IO::TaskPriorities nTaskPriority);
Console::IO::Results OpenArchive(const char * szFilename, int32 nArchivePriority, BOOL fIsHotfix, BOOL fValidateCRC, Console::IO::CPKArchiveReader ** pptArchive, Console::IO::TaskPriorities nTaskPriority);
Console::IO::Results OpenArchiveFromMemory(const char * szFilename, const uint8 * pArchiveData, int32 nArchiveSize, int32 nArchivePriority, BOOL fValidateCRC, Console::IO::CPKArchiveReader ** pptArchive, Console::IO::TaskPriorities nTaskPriority);
Console::IO::Results ChangeArchivePriority(Console::IO::CPKArchiveReader * ptArchive, int32 nArchivePriority);
Console::IO::Results CloseArchive(Console::IO::CPKArchiveReader * ptArchive, Console::IO::TaskPriorities nTaskPriority);
BOOL GetFilename(uint64 dwHash, char * szFilenameBuffer, int32 nFilenameBufferSize);
Console::IO::CPKArchiveReader * FindArchiveContaining(const char * szFilename, Console::IO::FileInfo * ptFileInfo);
uint64 GetLocaleOverrideHash(uint64 dwFileHash);
uint64 GetTextLocaleOverrideHash(uint64 dwFileHash);
Console::IO::CPKArchiveReader * FindArchiveContaining(uint64 dwHash, Console::IO::FileInfo * ptFileInfo);

}  // namespace IO
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    71  Console::IO::ReadBuffer::ReadBuffer()  [inlined into 1 functions]
//   line    81  int32 Console::IO::ReadBuffer::GetReadSize() const  [inlined into 5 functions]
//   line    86  int32 Console::IO::ReadBuffer::GetSector() const  [inlined into 1 functions]
//   line    91  Console::IO::CPKArchiveReader * Console::IO::ReadBuffer::GetArchive() const  [inlined into 1 functions]
//   line    96  BOOL Console::IO::ReadBuffer::IsValid() const  [inlined into 1 functions]
//   line   101  BOOL Console::IO::ReadBuffer::Contains(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nSector)  [inlined into 4 functions]
//   line   108  void Console::IO::ReadBuffer::SetInvalid()  [inlined into 5 functions]
//   line   119  void Console::IO::ReadBuffer::SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nSector, int32 nReadSize)  [inlined into 1 functions]
//   line   140  Console::IO::DecompBuffer::DecompBuffer()  [inlined into 1 functions]
//   line   145  uint8 * Console::IO::DecompBuffer::GetDecompBuffer()  [inlined into 1 functions]
//   line   150  int32 Console::IO::DecompBuffer::GetDecompSize() const  [inlined into 2 functions]
//   line   155  int32 Console::IO::DecompBuffer::GetSubOffset() const  [inlined into 2 functions]
//   line   160  int32 Console::IO::DecompBuffer::GetCompSector() const  [inlined into 1 functions]
//   line   165  Console::IO::CPKArchiveReader * Console::IO::DecompBuffer::GetArchive() const  [inlined into 2 functions]
//   line   170  BOOL Console::IO::DecompBuffer::IsValid() const  [inlined into 3 functions]
//   line   175  BOOL Console::IO::DecompBuffer::Contains(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nSubOffset)  [inlined into 1 functions]
//   line   183  void Console::IO::DecompBuffer::SetInvalid()  [inlined into 5 functions]
//   line   195  void Console::IO::DecompBuffer::SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nSubOffset, int32 nDecompSize)  [inlined into 2 functions]
//   line   218  Console::IO::CompBuffer::CompBuffer()  [inlined into 1 functions]
//   line   223  uint8 * Console::IO::CompBuffer::GetCompBuffer()  [inlined into 2 functions]
//   line   228  int32 Console::IO::CompBuffer::GetCompSize() const  [inlined into 1 functions]
//   line   233  int32 Console::IO::CompBuffer::GetCompSector() const  [inlined into 1 functions]
//   line   238  Console::IO::CPKArchiveReader * Console::IO::CompBuffer::GetArchive() const  [inlined into 2 functions]
//   line   243  BOOL Console::IO::CompBuffer::IsValid() const  [inlined into 2 functions]
//   line   255  void Console::IO::CompBuffer::SetInvalid()  [inlined into 4 functions]
//   line   266  void Console::IO::CompBuffer::SetValid(Console::IO::CPKArchiveReader * ptArchiveReader, int32 nCompSector, int32 nCompSize)  [inlined into 2 functions]
