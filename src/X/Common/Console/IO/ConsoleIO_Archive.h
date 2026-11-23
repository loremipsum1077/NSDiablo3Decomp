// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/IO/ConsoleIO_Archive.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleIO.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console {
namespace IO {
class CPKArchive {  // size 0x1010
    // vtable pointer
public:
    struct Header {  // size 0x48
        uint32 dwMagicNumber;  // 0x00
        uint32 dwPackageVersion;  // 0x04
        int64 nDecompressedFileSize;  // 0x08
        uint32 dwFlags;  // 0x10
        int32 nFileCount;  // 0x14
        int32 nLocationCount;  // 0x18
        int32 nHeaderSector;  // 0x1c
        int32 nFileSizeBitCount;  // 0x20
        int32 nFileLocationCountBitCount;  // 0x24
        int32 nFileLocationIndexBitCount;  // 0x28
        int32 nLocationBitCount;  // 0x2c
        int32 nCompSectorToDecompOffsetBitCount;  // 0x30
        int32 nDecompSectorToCompSectorBitCount;  // 0x34
        uint32 dwCRC;  // 0x38
        int32 nReadSectorSize;  // 0x3c
        int32 nCompSectorSize;  // 0x40
    };
    CPKArchive(const char *);
    virtual ~CPKArchive();  // vtable slot 0
    BOOL IsCompressed() const;
    BOOL IsValid() const;
    Console::IO::Results GetResult() const;
    static uint64 GetFileHash(const char * szFilename);
    static uint64 GetSubFileHash(uint64 dwParentHash, const char * szFilename);
    const char * GetArchiveName() const;
protected:
    char m_szFilename[4096];  // 0x08
    uint32 m_dwFlags;  // 0x1008
    Console::IO::Results m_nResult;  // 0x100c
    static uint32 sHighestBit(uint64 dwValue);
};
static_assert(sizeof(CPKArchive) == 0x1010, "CPKArchive");

// ConsoleIO_Archive.h:63. The flag's name is not in the DWARF; the callers test bit 1.
inline BOOL CPKArchive::IsCompressed() const {
    return (m_dwFlags & (1 << 1)) != 0;
}

// ConsoleIO_Archive.h:68. The flag's name is not in the DWARF; the callers test bit 0.
inline BOOL CPKArchive::IsValid() const {
    return (m_dwFlags & (1 << 0)) != 0;
}

// ConsoleIO_Archive.h:73
inline Console::IO::Results CPKArchive::GetResult() const {
    return m_nResult;
}

// ConsoleIO_Archive.h:85
inline const char * CPKArchive::GetArchiveName() const {
    return m_szFilename;
}

}  // namespace IO
}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    63  BOOL Console::IO::CPKArchive::IsCompressed() const  [inlined into 4 functions]
//   line    68  BOOL Console::IO::CPKArchive::IsValid() const  [inlined into 2 functions]
//   line    73  Console::IO::Results Console::IO::CPKArchive::GetResult() const  [inlined into 2 functions]
//   line    85  const char * Console::IO::CPKArchive::GetArchiveName() const  [inlined into 3 functions]
