// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Files/XFiles.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"
#include <stdio.h>

enum {
    ACCESS_READ_BIT = 0,
    ACCESS_WRITE_BIT = 1,
    ACCESS_UNBUFFERED_BIT = 2,
};

enum {
    MAX_XFILENAME_LENGTH = 255,
};

struct FileReference {  // size 0x1010
    FileReference();
    FileReference(LPCSTR);
    void Clear();
    int32 nRefcount;  // 0x00
    uint32 dwAccessFlags;  // 0x04
    Blizzard::File::Stream tStream;  // 0x08
    char szPath[4096];  // 0x10
};
static_assert(sizeof(FileReference) == 0x1010, "FileReference");

inline void FileReference::Clear() {  // line 42
    nRefcount = 0;
    dwAccessFlags = 0;
    tStream = NULL;
    szPath[0] = 0;
}

typedef void (*FileDeleteCallback)(const FileReference &, void *);

struct CachedFileListEntry {  // size 0x1000
    CHAR szPath[4096];  // 0x00
};
static_assert(sizeof(CachedFileListEntry) == 0x1000, "CachedFileListEntry");

class CachedFileList {  // size 0x10
public:
    CachedFileList();
    void Load(LPCSTR, LPCSTR, const BOOL);
    void Close();
    int32 nCount;  // 0x00
    CachedFileListEntry * ptList;  // 0x08
private:
    void sBuildList(LPCSTR, LPCSTR);
    BOOL sWrite(FileReference &) const;
    BOOL sRead(FileReference &);
};
static_assert(sizeof(CachedFileList) == 0x10, "CachedFileList");


// Functions defined in this file (bodies are not in the DWARF):
//   line    42  void FileReference::Clear()  [inlined into 5 functions]
//   line   197  BOOL FileRead(FileReference & tFileRef, void * lpBuf, const SerializeData & ser)  [inline]

// X/Sigma/Files/XFilesCore.cpp:69, :109, :175 and :1578.
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
void FileSplitPath(LPCSTR szPath, LPSTR pszDrive, LPSTR pszDirectory, LPSTR pszFilename, LPSTR pszExtension);
BOOL FileDirectoryCreate(FileReference & tFileRef, const BOOL fCreateSubDirs);
FILE * fopen_ex(const char * filename, const char * mode);
void FileGetProcessPath(LPSTR szProcessPath);  // XFilesCore.cpp:165
