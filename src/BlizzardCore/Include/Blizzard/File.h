// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/File.h

#pragma once

#include "Time.h"
#include "Types.h"

namespace Blizzard { namespace File { struct StreamRecord; } }

namespace Blizzard {
namespace File {
typedef Blizzard::File::StreamRecord * Stream;

}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace Error {
enum {
    fileExists = 1,
    fileNotFound = 2,
    fileCreateFailed = 3,
    fileOpenFailed = 4,
    readUnderflow = 5,
    writeUnderflow = 6,
    endOfFile = 7,
    paramErr = 8,
    genericErr = 9,
    rangeErr = 10,
    diskfullErr = 11,
    errnoErr = 100,
};

}  // namespace Error
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace Pos {
enum {
    fromStart = 0,
    fromCur = 1,
    fromEnd = 2,
};

}  // namespace Pos
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace Perm {
enum {
    read = 1,
    write = 2,
    shRead = 4,
    shWrite = 8,
    rw = 3,
    shrw = 12,
    any = 15,
};

}  // namespace Perm
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace Mode {
enum {
    sync = 0,
    async = 32,
    cache = 0,
    nocache = 64,
    temp = 128,
    preserve = 0,
    truncate = 256,
    append = 512,
    recreate = 1024,
    canexist = 0,
    cantexist = 2048,
    mustexist = 4096,
    preallocFast = 8192,
    preallocSlow = 16384,
    secure = 65536,
    openExisting = 4096,
    truncateExisting = 4352,
    appendExisting = 4608,
    createNew = 3072,
    openAlways = 512,
    createAlways = 1024,
    prealloc = 24576,
};

}  // namespace Mode
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace Attribute {
enum {
    readonly = 1,
    hidden = 2,
    system = 4,
    archive = 8,
    temporary = 16,
    normal = 32,
    directory = 64,
};

}  // namespace Attribute
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace CreateResult {
enum {
    createdFailed = 1,
    createdNormal = 2,
    createdPreallocSlow = 3,
    createdPreallocFast = 4,
    createdPreallocCancel = 5,
};

}  // namespace CreateResult
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
enum Existence {
    missing = 0,
    isFile = 1,
    isDirectory = 2,
};

struct FileInfo {  // size 0x40
    char * path;  // 0x00
    int64 size;  // 0x08
    int64 sparseSize;  // 0x10
    int32 attributes;  // 0x18
    Blizzard::Time::Timestamp createTime;  // 0x20
    Blizzard::Time::Timestamp modTime;  // 0x28
    Blizzard::Time::Timestamp accessTime;  // 0x30
    int existence;  // 0x38
    bool regularFile;  // 0x3c
};
static_assert(sizeof(FileInfo) == 0x40, "FileInfo");

typedef bool (*PreallocCallback)(const int64, void *);

struct PreallocParms {  // size 0x28
    PreallocParms();
    int64 size;  // 0x00
    int result;  // 0x08
    Blizzard::File::PreallocCallback callback;  // 0x10
    void * context;  // 0x18
    int interval;  // 0x20
    bool useSparse;  // 0x24
};
static_assert(sizeof(PreallocParms) == 0x28, "PreallocParms");

typedef int32 INODE;

}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
namespace InodeDefs {
enum {
    invalid = 0,
};

}  // namespace InodeDefs
}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
struct ProcessDirParms {  // size 0x20
    const char * pathRoot;  // 0x00
    const char * filename;  // 0x08
    void * cookie;  // 0x10
    bool isDir;  // 0x18
};
static_assert(sizeof(ProcessDirParms) == 0x20, "ProcessDirParms");

typedef bool (*ProcessDirCallback)(const Blizzard::File::ProcessDirParms &);

}  // namespace File
}  // namespace Blizzard
namespace Blizzard {
namespace File {
// The File functions have no DWARF of their own; the prototypes come from their mangled names
// and the parameter names are invented.
bool Open(const char * name, int mode, Blizzard::File::Stream & file);
bool Close(Blizzard::File::Stream file);
bool GetFileInfo(Blizzard::File::Stream file, Blizzard::File::FileInfo * info);
bool Read(Blizzard::File::Stream file, void * buffer, int64 offset, int32 * count, int flags);
bool Read(Blizzard::File::Stream file, void * buffer, int64 offset, int32 count, int flags);

// FileAPI overloads used by X/Client/Console/GamerProfileNX64.cpp (prototypes from the mangled names).
const Blizzard::File::FileInfo * GetFileInfo(Blizzard::File::Stream file);
bool Read(Blizzard::File::Stream file, void * buffer, int32 count);
bool Write(Blizzard::File::Stream file, const void * buffer, int32 count);
bool Delete(const char * name);
bool CreateDirectory(const char * name, bool recursive);
}  // namespace File
}  // namespace Blizzard

// BlizzardCore/Source/System/File/FileAPI.cpp:360 and :757 (X/Common/Game.cpp AppInitialize).
namespace Blizzard {
namespace File {
bool GetWorkingDirectory(char * buf, int bufMax);
// FileAPI.cpp:178, :410 and :425 (X/Sigma/ErrorManager/FileOutputStream.cpp).
Blizzard::File::Existence Exists(const char * path);
bool MakeAbsolutePath(const char * localPath, char * absolutePath, int absMax, bool makeCanonical);
bool Move(const char * oldFileName, const char * newFileName);
bool SetWorkingDirectory(const char * buf);
}  // namespace File
}  // namespace Blizzard
