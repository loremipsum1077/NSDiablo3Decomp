// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Source/System/File/StackedFile.h

#pragma once

#include "../../../Include/Blizzard/File.h"
#include "../../../Include/Blizzard/Types.h"
#include "../../../Include/Blizzard/Lock.h"
#include "../../../Include/Blizzard/Memory.h"

namespace Blizzard { namespace File { class FileSystem; class FileSystemPtr; } }
namespace System_File { namespace Stacked { struct FileParms; } }

namespace System_File {
namespace Stacked {
typedef bool (*FileFunc)(const Blizzard::File::FileSystemPtr &, System_File::Stacked::FileParms *);

}  // namespace Stacked
}  // namespace System_File
namespace Blizzard {
namespace File {
class FileSystemPtr {  // size 0x8
    void Swap(Blizzard::File::FileSystem * ptr);
public:
    FileSystemPtr();
    FileSystemPtr(const Blizzard::File::FileSystemPtr &);
    FileSystemPtr(Blizzard::File::FileSystem *);
    ~FileSystemPtr();
    operator Blizzard::File::FileSystem *() const { return m_ref; }  // line 63
    Blizzard::File::FileSystem * operator->() { return m_ref; }  // line 64
    const Blizzard::File::FileSystem * operator->() const { return m_ref; }  // line 65
    Blizzard::File::FileSystem & operator*();
    Blizzard::File::FileSystemPtr & operator=(const Blizzard::File::FileSystemPtr & rhs) { Swap(rhs.m_ref); return *this; }  // line 68
    Blizzard::File::FileSystemPtr & operator=(Blizzard::File::FileSystem * ptr) { Swap(ptr); return *this; }  // line 69
    bool operator==(const Blizzard::File::FileSystem *);
    bool operator!=(const Blizzard::File::FileSystem * ptr) { return m_ref != ptr; }  // line 71
    bool operator==(const Blizzard::File::FileSystemPtr &);
    bool operator!=(const Blizzard::File::FileSystemPtr &);
    void Swap(Blizzard::File::FileSystemPtr & ptr);
private:
    Blizzard::File::FileSystem * m_ref;  // 0x00
};
static_assert(sizeof(FileSystemPtr) == 0x8, "FileSystemPtr");

class FileSystem {  // size 0x118
public:
    Blizzard::File::FileSystemPtr next;  // 0x00
    struct Functions {  // size 0x100
        System_File::Stacked::FileFunc checksparsefilesupport;  // 0x00
        System_File::Stacked::FileFunc cd;  // 0x08
        System_File::Stacked::FileFunc close;  // 0x10
        System_File::Stacked::FileFunc create;  // 0x18
        System_File::Stacked::FileFunc cwd;  // 0x20
        System_File::Stacked::FileFunc dirwalk;  // 0x28
        System_File::Stacked::FileFunc exists;  // 0x30
        System_File::Stacked::FileFunc flush;  // 0x38
        System_File::Stacked::FileFunc getfileinfo;  // 0x40
        System_File::Stacked::FileFunc getfreespace;  // 0x48
        System_File::Stacked::FileFunc getpos;  // 0x50
        System_File::Stacked::FileFunc getrootchars;  // 0x58
        System_File::Stacked::FileFunc isabspath;  // 0x60
        System_File::Stacked::FileFunc isreadonly;  // 0x68
        System_File::Stacked::FileFunc makeabspath;  // 0x70
        System_File::Stacked::FileFunc mkdir;  // 0x78
        System_File::Stacked::FileFunc move;  // 0x80
        System_File::Stacked::FileFunc copy;  // 0x88
        System_File::Stacked::FileFunc open;  // 0x90
        System_File::Stacked::FileFunc read;  // 0x98
        System_File::Stacked::FileFunc readp;  // 0xa0
        System_File::Stacked::FileFunc rmdir;  // 0xa8
        System_File::Stacked::FileFunc setcachemode;  // 0xb0
        System_File::Stacked::FileFunc seteof;  // 0xb8
        System_File::Stacked::FileFunc setfileinfo;  // 0xc0
        System_File::Stacked::FileFunc setpos;  // 0xc8
        System_File::Stacked::FileFunc setsparsefile;  // 0xd0
        System_File::Stacked::FileFunc setsparserange;  // 0xd8
        System_File::Stacked::FileFunc unlink;  // 0xe0
        System_File::Stacked::FileFunc write;  // 0xe8
        System_File::Stacked::FileFunc writep;  // 0xf0
        System_File::Stacked::FileFunc shutdown;  // 0xf8
    };
    int Acquire() { return Blizzard::Lock::Atomic::Increment(&active); }  // line 121
    // lines 122-129; body reconstructed from its copy inlined into FileSystemPtr::Swap.
    int Release()
    {
        int32 nRefCount = Blizzard::Lock::Atomic::Decrement(&active);
        if (nRefCount == 0)
        {
            next = NULL;
            Blizzard::Memory::RawFree(this);
        }
        return nRefCount;
    }
    static Blizzard::File::FileSystem * CreateFromTemplate(const Blizzard::File::FileSystem::Functions & mgrTemplate);
    Blizzard::File::FileSystem::Functions verbs;  // 0x08
    const Blizzard::File::FileSystem::Functions * original;  // 0x108
    int32 GetDebugRefCount();
private:
    FileSystem();
    FileSystem(const Blizzard::File::FileSystem &);
    Blizzard::File::FileSystem & operator=(const Blizzard::File::FileSystem &);
    Blizzard::File::FileSystem * operator&();
    volatile int32 active;  // 0x110
};
static_assert(sizeof(FileSystem) == 0x118, "FileSystem");

union FileInfoMask {  // size 0x4
    struct {  // size 0x4
        uint32 path : 1;  // 0x00
        uint32 size : 1;  // 0x00
        uint32 attributes : 1;  // 0x00
        uint32 createTime : 1;  // 0x00
        uint32 modTime : 1;  // 0x00
        uint32 accessTime : 1;  // 0x00
        uint32 existence : 1;  // 0x00
        uint32 regularFile : 1;  // 0x00
        uint32 pad : 24;  // 0x00
    } b;  // 0x00
    uint32 all;  // 0x00
};
static_assert(sizeof(FileInfoMask) == 0x4, "FileInfoMask");

}  // namespace File
}  // namespace Blizzard
namespace System_File {
namespace Stacked {
struct FileParms {  // size 0xd0
    System_File::Stacked::FileFunc Blizzard::File::FileSystem::Functions::* verb;  // 0x00
    const char * path;  // 0x08
    const char * newpath;  // 0x10
    Blizzard::File::Stream stream;  // 0x18
    Blizzard::File::FileInfo * info;  // 0x20
    Blizzard::File::FileInfo info_;  // 0x28
    Blizzard::File::FileInfoMask has;  // 0x68
    Blizzard::File::FileInfoMask want;  // 0x6c
    int mode;  // 0x70
    void * buffer;  // 0x78
    int32 amount;  // 0x80
    int64 pos;  // 0x88
    int64 length;  // 0x90
    int posMode;  // 0x98
    char * cwdbuf;  // 0xa0
    int cwdbuflen;  // 0xa8
    bool recursive;  // 0xac
    bool makeCanonical;  // 0xad
    void * cookie;  // 0xb0
    Blizzard::File::ProcessDirCallback dircallback;  // 0xb8
    bool sorted;  // 0xc0
    bool overwrite;  // 0xc1
    bool grantEveryone;  // 0xc2
    bool zerofillAll;  // 0xc3
    bool supportsSparseFiles;  // 0xc4
    int extopt;  // 0xc8
};
static_assert(sizeof(FileParms) == 0xd0, "FileParms");

}  // namespace Stacked
}  // namespace System_File

// Functions defined in this file (bodies are not in the DWARF):
//   line    63  Blizzard::File::FileSystemPtr::operator Blizzard::File::FileSystem *() const  [inlined into 23 functions]
//   line    64  Blizzard::File::FileSystem * Blizzard::File::FileSystemPtr::operator->()  [inline]
//   line    65  const Blizzard::File::FileSystem * Blizzard::File::FileSystemPtr::operator->() const  [inline]
//   line    68  Blizzard::File::FileSystemPtr & Blizzard::File::FileSystemPtr::operator=(const Blizzard::File::FileSystemPtr & rhs)  [inlined into 23 functions]
//   line    69  Blizzard::File::FileSystemPtr & Blizzard::File::FileSystemPtr::operator=(Blizzard::File::FileSystem * ptr)  [inlined into 25 functions]
//   line    71  bool Blizzard::File::FileSystemPtr::operator!=(const Blizzard::File::FileSystem * ptr)  [inlined into 22 functions]
//   line   121  int Blizzard::File::FileSystem::Acquire()  [inlined into 25 functions]
//   line   122  int Blizzard::File::FileSystem::Release()  [inlined into 25 functions]
