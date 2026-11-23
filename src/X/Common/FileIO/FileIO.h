// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/FileIO/FileIO.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Mopaq.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Game.h"

namespace Console { namespace IO { struct FileInfo; } }
class HSFILERecord;
struct SNOName;

typedef HSFILERecord * HSFILE;

namespace FileIO {
typedef uint32 OpenFileFlags;

enum FilePointerMovementMethod {
    FROM_BEGINNING = 0,
    FROM_CURRENT = 1,
    FROM_END = 2,
};

class File {  // size 0x10
public:
    File(XLocale);
    ~File();
    bool OpenSNO(const SNOName & tSNOName);
    bool OpenChildSNO(const SNOName & tSNOName, const char * szChildFile);
    bool Open(const char * szFilename);
    bool Open(const char * szDiskFilename, const char * szNonDiskFilename);
    bool Open(const FileIO::File & tFile);
    bool OpenConsoleFile(HSARCHIVE ptArchive, uint64 dwHash, Console::IO::FileInfo * ptFileInfo);
    bool Close();
    int64 GetFileSize() const;
    int64 GetCompressedFileSize() const;
    bool GetFileName(char * szBuffer, uint32 nBufferChar) const;
    uint64 SetFileCursor(int64 dwDistanceToMove, FileIO::FilePointerMovementMethod eMethod);
    bool ReadFile(void * buffer, uint64 dwBytesToRead, uint64 * pdwBytesRead);
    HSFILE GetHSFILE() const;
    bool TraceFile();
    bool IsAlreadyOpen() const;
    int64 GetSNOFileStartingLocation() const;
private:
    class FileImpl;
    XLocale m_eLocale;  // 0x00
    FileImpl * m_Impl;  // 0x08
};
static_assert(sizeof(File) == 0x10, "File");

bool FileExists(const char * szFileName, XLocale eLocale);
int32 GetLastErrorCode();

}  // namespace FileIO
