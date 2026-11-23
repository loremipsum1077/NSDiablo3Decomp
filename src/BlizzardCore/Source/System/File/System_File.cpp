// Original file: BlizzardCore/Source/System/File/System_File.cpp
// The original is a unity file that includes Dir.cpp, File.cpp, FileAPI.cpp, FileStacks.cpp,
// File_NX64.cpp, Filestack_Null.cpp, Filestack_Readonly.cpp and Preallocate.cpp (the DWARF
// decl_file of every function). Their bodies are written here, grouped by original file, each
// group in original line order. Only part of the file is written so far.
#include "StackedFile.h"
#include "FilePriv.h"
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Error.h"
#include "../../../Include/Blizzard/Thread.h"
#include "../../../Include/Blizzard/Using.h"
#include "../../../Include/Blizzard/Util.h"
#include "../../../Include/Blizzard/Sys/Allocator_STL.h"
#include <map>
#include <string>
#include <algorithm>
#include "../../../../NintendoSDK/Include/nn/fs/fs_Directory.h"
#include "../../../../NintendoSDK/Include/nn/fs/fs_FileSystem.h"
#include <new>
#include <stdio.h>
#include <string.h>

// ---- File_NX64.cpp (lines 22-46) ----
static bool g_bMountedROM;
static char g_szDataMount[4096];
static uint32_t g_uDataMountLength;
static char g_szCurrentDirectory[4096];

void InitializeFilesNX64(const char * mountName, const bool bIsROM)
{
    g_bMountedROM = bIsROM;
    Blizzard::String::Format(g_szDataMount, sizeof(g_szDataMount), "%s:", mountName);
    g_uDataMountLength = static_cast<uint32_t>(Blizzard::String::Length(g_szDataMount));
    Blizzard::String::Format(g_szCurrentDirectory, sizeof(g_szCurrentDirectory), "%s/", g_szDataMount);
}

bool IsROMPathNX64(const char * path)
{
    if (g_bMountedROM)
    {
        return Blizzard::String::Compare(g_szDataMount, path, g_uDataMountLength) == 0;
    }
    return false;
}

bool IsMountedROM()
{
    return g_bMountedROM;
}

// ---- FileAPI.cpp (lines 888-968): the per-thread error state ----
namespace System_File {
struct FileError {  // size 0x10
    FileError() : simpleError(0), externalError(0), errorStack(NULL) {}
    ~FileError() { Blizzard::Debug::FreeErrorStack(errorStack); }
    int simpleError;  // 0x00
    int externalError;  // 0x04
    Blizzard::Debug::ErrorStack errorStack;  // 0x08
};

Blizzard::Thread::LocalObject<FileError> s_error;

inline FileError & GetFileError()
{
    return s_error;
}
}  // namespace System_File

namespace Blizzard {
namespace File {
int GetLastError()
{
    System_File::FileError * last = &System_File::GetFileError();
    return last->simpleError;
}

inline void SetLastError(int err)
{
    System_File::FileError * last = &System_File::GetFileError();
    last->simpleError = err;
}

inline Debug::ErrorStack GetLastErrorStack()
{
    System_File::FileError * last = &System_File::GetFileError();
    return last->errorStack;
}

void SetLastErrorStack(Debug::ErrorStack err)
{
    System_File::FileError * last = &System_File::GetFileError();
    if (last->errorStack != err)
    {
        Debug::FreeErrorStack(last->errorStack);
    }
    last->errorStack = err;
}

inline void AddToLastErrorStack(int code, const char * str, int category)
{
    Debug::ErrorStack error = Debug::AddErrorToStack(GetLastErrorStack(), code, str, category);
    if (error != NULL)
    {
        SetLastErrorStack(error);
    }
}

int GetLastExternalError()
{
    System_File::FileError * last = &System_File::GetFileError();
    return last->externalError;
}
}  // namespace File
}  // namespace Blizzard

namespace System_File {
namespace Stacked {
// ---- File_NX64.cpp (lines 213-249, 638, 878): inlined into the File.cpp wrappers ----
inline bool SetWorkingDirectory(FileParms * parms)
{
    if (parms->path == NULL)
    {
        Blizzard::File::SetLastError(8);
        return false;
    }
    strncpy(g_szCurrentDirectory, parms->path, sizeof(g_szCurrentDirectory));
    g_szCurrentDirectory[sizeof(g_szCurrentDirectory) - 1] = '\0';
    return true;
}

inline bool GetWorkingDirectory(FileParms * parms)
{
    if (parms->cwdbuf == NULL || parms->cwdbuflen <= 0)
    {
        Blizzard::File::SetLastError(8);
        return false;
    }
    *parms->cwdbuf = '\0';
    strncpy(parms->cwdbuf, g_szCurrentDirectory, parms->cwdbuflen);
    return true;
}

inline const char * GetAbsolutePath(const char * szOriginalPath, char * szAbsolutePath, uint32 dwAbsolutePathSize)
{
    char szNativePath[4096];
    const char * szPath = Blizzard::String::MakeNativePath(szOriginalPath, szNativePath, sizeof(szNativePath));
    if (Blizzard::String::Find(szPath, ':') != NULL)
    {
        strncpy(szAbsolutePath, szPath, dwAbsolutePathSize);
    }
    else
    {
        snprintf(szAbsolutePath, dwAbsolutePathSize, "%s/%s", g_szCurrentDirectory, szPath);
    }
    return szAbsolutePath;
}

inline bool Close(Blizzard::File::Stream file)
{
    if (file->fileHandle.handle != NULL)
    {
        nn::fs::CloseFile(file->fileHandle);
    }
    return true;
}

inline bool Move(FileParms * parms)
{
    if (IsROMPathNX64(parms->path))
    {
        return false;
    }
    return nn::fs::RenameFile(parms->path, parms->newpath).IsSuccess();
}

inline bool Delete(FileParms * parms)
{
    if (IsROMPathNX64(parms->path))
    {
        return false;
    }
    return nn::fs::DeleteFile(parms->path).IsSuccess();
}

// File_NX64.cpp
bool ProcessDirFast(FileParms * parms);
bool Exists(FileParms * parms);
bool GetFileInfo(FileParms * parms);
bool GetRootChars(FileParms * parms);
bool MakeAbsolutePath(FileParms * parms);
bool CreateDirectory(FileParms * parms);
bool Open(FileParms * parms);
bool RemoveDirectory(FileParms * parms);
}  // namespace Stacked

class ReadImpl {
public:
    static bool Execute(Blizzard::File::Stream file, void * buffer, int64 start, int32 * amount);
};
class WriteImpl {
public:
    static bool Execute(Blizzard::File::Stream file, const void * buffer, int64 start, int32 * amount);
};

// ---- File.cpp (lines 111-490) ----
}  // namespace System_File

namespace Blizzard {
namespace File {
typedef std::map<std::basic_string<char, std::char_traits<char>, Blizzard::Memory::AllocSTL<char> >, int> INODE_MAP;
typedef std::map<std::basic_string<char, std::char_traits<char>, Blizzard::Memory::AllocSTL<char> >, int> INODE_TRACKER;
}  // namespace File
}  // namespace Blizzard

// File.cpp:555
static Blizzard::File::INODE_MAP * s_Inode;
static Blizzard::File::INODE_TRACKER * s_InodeTracker;

namespace System_File {
struct UsingInode : public Blizzard::Using::ScopedUse {  // size 0x28
    UsingInode();
    ~UsingInode();
    static void ShutdownProc();
    Blizzard::Lock::RMutex inodeMutex;  // 0x08
};

// File.cpp:560
UsingInode::~UsingInode()
{
}

// File.cpp:565
UsingInode s_usingInode;

// File.cpp:568
inline UsingInode::UsingInode() : ScopedUse("Blizzard::Inode", ShutdownProc)
{
    Blizzard::Using::DependsOn("Blizzard::File", "Blizzard::Inode");
    Blizzard::Using::DependsOn("Blizzard::Inode", "Blizzard::Memory");
}

// File.cpp:575
void UsingInode::ShutdownProc()
{
    delete s_Inode;
    s_Inode = NULL;
    delete s_InodeTracker;
    s_InodeTracker = NULL;
}

// File.cpp:602. The linker removed this function (DW_AT_low_pc 0 in the DWARF), so nothing in the
// image checks its body. It is kept because it is the only writer of s_Inode and s_InodeTracker:
// without it both fold to NULL in ShutdownProc. The locals and the calls are the ones the DWARF
// lists; lines 612-613 and everything after the lookup are not reconstructed.
Blizzard::File::INODE GetInode(
    const std::basic_string<char, std::char_traits<char>, Blizzard::Memory::AllocSTL<char> > & path)
{
    static Blizzard::File::INODE inodeCounter;
    Blizzard::Lock::ScopedRMutex scopedMutex(s_usingInode.inodeMutex);
    if (s_Inode == NULL)
    {
        s_Inode = new Blizzard::File::INODE_MAP;
    }
    if (s_InodeTracker == NULL)
    {
        s_InodeTracker = new Blizzard::File::INODE_TRACKER;
    }
    char abspath[4096];
    Blizzard::File::MakeAbsolutePath(path.c_str(), abspath, sizeof(abspath), false);
    Blizzard::File::INODE fullpathInode = (*s_Inode)[abspath];
    if (fullpathInode == 0)
    {
        fullpathInode = ++inodeCounter;
        (*s_Inode)[abspath] = fullpathInode;
    }
    return fullpathInode;
}

bool SetWorkingDirectory(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::SetWorkingDirectory(parms);
}

bool Close(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::Close(parms->stream);
}

bool Create(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    Blizzard::File::SetLastError(9);
    return false;
}

bool GetWorkingDirectory(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::GetWorkingDirectory(parms);
}

bool Move(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::Move(parms);
}

bool Delete(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::Delete(parms);
}

bool CheckSparseFileSupport(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool ProcessDirFast(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::ProcessDirFast(parms);
}

bool Exists(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    Stacked::Exists(parms);
    return true;
}

bool Flush(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool GetFileInfo(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::GetFileInfo(parms);
}

bool GetFreeSpace(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool GetPos(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool GetRootChars(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    Stacked::GetRootChars(parms);
    return true;
}

bool IsAbsolutePath(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool IsReadOnly(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool MakeAbsolutePath(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    Stacked::MakeAbsolutePath(parms);
    return true;
}

bool CreateDirectory(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::CreateDirectory(parms);
}

bool Open(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::Open(parms);
}

bool Read(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return ReadImpl::Execute(parms->stream, parms->buffer, -1, &parms->amount);
}

bool ReadP(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return ReadImpl::Execute(parms->stream, parms->buffer, parms->pos, &parms->amount);
}

bool RemoveDirectory(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return Stacked::RemoveDirectory(parms);
}

bool SetCacheMode(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool SetEOF(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool SetSparseFile(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool SetSparseRange(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool SetAttributes(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool SetPos(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return false;
}

bool Write(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return WriteImpl::Execute(parms->stream, parms->buffer, -1, &parms->amount);
}

bool WriteP(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    return WriteImpl::Execute(parms->stream, parms->buffer, parms->pos, &parms->amount);
}

bool Shutdown(const Blizzard::File::FileSystemPtr &, Stacked::FileParms *)
{
    return true;
}

// ---- Filestack_Null.cpp (lines 27-317) ----
namespace Stacked {
bool null_checksparsefilesupport(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_cd(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_close(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_create(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_cwd(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_dirwalk(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_exists(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_flush(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_getfileinfo(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_getfreespace(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_getpos(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_getrootchars(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_isabspath(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_isreadonly(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_makeabspath(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_mkdir(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_move(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_copy(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_open(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_read(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_readp(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_rmdir(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_setcachemode(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_seteof(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_setfileinfo(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_setpos(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_setsparsefile(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_setsparserange(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_unlink(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_write(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_writep(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

bool null_shutdown(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return true;
}

Blizzard::File::FileSystem::Functions s_nullstack = {
    null_checksparsefilesupport,
    null_cd,
    null_close,
    null_create,
    null_cwd,
    null_dirwalk,
    null_exists,
    null_flush,
    null_getfileinfo,
    null_getfreespace,
    null_getpos,
    null_getrootchars,
    null_isabspath,
    null_isreadonly,
    null_makeabspath,
    null_mkdir,
    null_move,
    null_copy,
    null_open,
    null_read,
    null_readp,
    null_rmdir,
    null_setcachemode,
    null_seteof,
    null_setfileinfo,
    null_setpos,
    null_setsparsefile,
    null_setsparserange,
    null_unlink,
    null_write,
    null_writep,
    null_shutdown,
};
}  // namespace Stacked
}  // namespace System_File

// ---- FileStacks.cpp ----
namespace Blizzard {
namespace File {
// line 24
void FileSystemPtr::Swap(FileSystem * ptr)
{
    if (m_ref != ptr)
    {
        if (ptr)
        {
            ptr->Acquire();
        }
        if (m_ref)
        {
            m_ref->Release();
        }
        m_ref = ptr;
    }
}

// line 50
inline FileSystemPtr::FileSystemPtr(const FileSystemPtr & copy)
{
    if (copy.m_ref) copy.m_ref->Acquire();
    m_ref = copy.m_ref;
}

// line 56
inline FileSystemPtr::FileSystemPtr(FileSystem * ptr)
{
    if (ptr) ptr->Acquire();
    m_ref = ptr;
}

// line 63
inline FileSystemPtr::~FileSystemPtr()
{
    if (m_ref) m_ref->Release();
}

// line 69
inline FileSystem * FileSystem::CreateFromTemplate(const FileSystem::Functions & mgrTemplate)
{
    FileSystem * mgr = Blizzard::Memory::RawAlloc(sizeof(FileSystem));
    Blizzard::String::MemFill(&mgr->next, sizeof(mgr->next), 0);
    mgr->original = &mgrTemplate;
    mgr->active = 0;
    Blizzard::String::MemCopy(&mgr->verbs, &mgrTemplate, sizeof(mgr->verbs));
    return mgr;
}
}  // namespace File
}  // namespace Blizzard

namespace System_File {
bool SetWorkingDirectory(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
bool Close(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
bool Create(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
bool GetWorkingDirectory(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
bool Move(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
bool Copy(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
bool Delete(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms);
namespace Stacked {
bool file_init(const Blizzard::File::FileSystemPtr &, FileParms * parms);
bool Execute(const Blizzard::File::FileSystemPtr & mgr, FileParms * parms);

// line 108
Blizzard::File::FileSystem::Functions s_fileinit = {
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
    file_init,
};

// line 144
Blizzard::File::FileSystemPtr * s_manager;

// File.cpp:506
Blizzard::File::FileSystem::Functions s_basestack = {
    System_File::CheckSparseFileSupport,
    System_File::SetWorkingDirectory,
    System_File::Close,
    System_File::Create,
    System_File::GetWorkingDirectory,
    System_File::ProcessDirFast,
    System_File::Exists,
    System_File::Flush,
    System_File::GetFileInfo,
    System_File::GetFreeSpace,
    System_File::GetPos,
    System_File::GetRootChars,
    System_File::IsAbsolutePath,
    System_File::IsReadOnly,
    System_File::MakeAbsolutePath,
    System_File::CreateDirectory,
    System_File::Move,
    System_File::Copy,
    System_File::Open,
    System_File::Read,
    System_File::ReadP,
    System_File::RemoveDirectory,
    System_File::SetCacheMode,
    System_File::SetEOF,
    System_File::SetAttributes,
    System_File::SetPos,
    System_File::SetSparseFile,
    System_File::SetSparseRange,
    System_File::Delete,
    System_File::Write,
    System_File::WriteP,
    System_File::Shutdown,
};

// line 240
void Push(Blizzard::File::FileSystemPtr & stack, const Blizzard::File::FileSystem::Functions & mgrTemplate)
{
    Blizzard::File::FileSystemPtr mgr(Blizzard::File::FileSystem::CreateFromTemplate(mgrTemplate));
    mgr->next = stack;
    stack = mgr;
}

// line 398
inline Blizzard::File::FileSystemPtr & GetStack()
{
    if (s_manager == NULL)
    {
        s_manager = new Blizzard::File::FileSystemPtr(Blizzard::File::FileSystem::CreateFromTemplate(s_fileinit));
    }
    return *s_manager;
}

// line 411
bool file_init(const Blizzard::File::FileSystemPtr &, FileParms * parms)
{
    Blizzard::File::FileSystemPtr mgr(Blizzard::File::FileSystem::CreateFromTemplate(s_nullstack));
    Push(mgr, s_basestack);
    *s_manager = mgr;
    return Execute(mgr, parms);
}

// FileAPI.cpp:983
inline bool Execute(const Blizzard::File::FileSystemPtr & mgr, FileParms * parms)
{
    Blizzard::File::FileSystemPtr ptr(mgr);
    while (ptr != NULL)
    {
        FileFunc func = ptr->verbs.*(parms->verb);
        if (func)
        {
            return func(ptr, parms);
        }
        ptr = ptr->next;
    }
    return false;
}
}  // namespace Stacked
}  // namespace System_File

// FilePriv.h:105
namespace Blizzard {
namespace File {
template <bool B> inline FileSystemPtr GetFilesystem()
{
    return System_File::Stacked::GetStack();
}

// FileAPI.cpp:150
bool Delete(const char * path)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::unlink;
    parms.path = path;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:89
bool Close(Stream file)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::close;
    parms.stream = file;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:135
bool CreateDirectory(const char * dirPath, bool createSubDirs, bool grantEveryone)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::mkdir;
    parms.path = dirPath;
    parms.recursive = createSubDirs;
    parms.grantEveryone = grantEveryone;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:360
bool GetWorkingDirectory(char * buf, int bufMax)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::cwd;
    parms.cwdbuf = buf;
    parms.cwdbuflen = bufMax;
    *buf = '\0';
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:410
bool MakeAbsolutePath(const char * localPath, char * absolutePath, int absMax, bool makeCanonical)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::makeabspath;
    parms.path = localPath;
    parms.cwdbuf = absolutePath;
    parms.cwdbuflen = absMax;
    parms.makeCanonical = makeCanonical;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:425
bool Move(const char * oldFileName, const char * newFileName)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::move;
    parms.path = oldFileName;
    parms.newpath = newFileName;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:509
bool ProcessDirFast(const char * path, void * cookie, ProcessDirCallback callback, bool treatSymlinksAsFiles)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::dirwalk;
    parms.path = path;
    parms.cookie = cookie;
    parms.dircallback = callback;
    parms.sorted = false;
    parms.mode = treatSymlinksAsFiles;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:163
Existence Exists(const FileSystemPtr & mgr, const char * path)
{
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::exists;
    parms.path = path;
    parms.info = &parms.info_;
    if (path != NULL && System_File::Stacked::Execute(mgr, &parms))
    {
        return static_cast<Existence>(parms.info->existence);
    }
    return missing;
}

// FileAPI.cpp:178
Existence Exists(const char * path)
{
    return Exists(GetFilesystem<false>(), path);
}

// FileAPI.cpp:210
bool GetFileInfo(const char * path, FileInfo * info)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::getfileinfo;
    parms.path = path;
    parms.has.all = 0;
    parms.want.all = 0xffffffff;
    parms.stream = NULL;
    parms.info = &parms.info_;
    if (System_File::Stacked::Execute(mgr, &parms))
    {
        *info = *parms.info;
        return true;
    }
    return false;
}

// FileAPI.cpp:225
const FileInfo * GetFileInfo(Stream file)
{
    static FileInfo s_noinfo;
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::getfileinfo;
    parms.path = NULL;
    parms.stream = file;
    parms.info = NULL;
    parms.has.all = 0;
    parms.want.all = 0xffffffff;
    if (System_File::Stacked::Execute(mgr, &parms))
    {
        return parms.info;
    }
    return &s_noinfo;
}

// FileAPI.cpp:267
bool GetFileInfo(Stream file, FileInfo * info)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::getfileinfo;
    parms.path = NULL;
    parms.stream = file;
    parms.info = NULL;
    if (System_File::Stacked::Execute(mgr, &parms))
    {
        *info = *parms.info;
        return true;
    }
    return false;
}

// FileAPI.cpp:340
uint32 GetRootChars(const char * dirPath)
{
    uint32 numRootChars = 0;
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::getrootchars;
    parms.path = dirPath;
    parms.pos = 0;
    bool ok = System_File::Stacked::Execute(mgr, &parms);
    if (ok)
    {
        numRootChars = static_cast<uint32>(parms.pos);
    }
    return numRootChars;
}

// FileAPI.cpp:454
inline bool Open(const FileSystemPtr & mgr, const char * path, int mode, Stream & file)
{
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::open;
    parms.path = path;
    parms.stream = NULL;
    parms.mode = mode;
    bool ok = System_File::Stacked::Execute(mgr, &parms);
    if (ok)
    {
        file = parms.stream;
    }
    return ok;
}

// FileAPI.cpp:468
bool Open(const char * path, int mode, Stream & file)
{
    return Open(GetFilesystem<false>(), path, mode, file);
}

// FileAPI.cpp:537
bool Read(Stream file, void * buffer, int32 * amount)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::read;
    parms.stream = file;
    parms.buffer = buffer;
    parms.amount = *amount;
    parms.extopt = 0;
    *amount = 0;
    bool ok = System_File::Stacked::Execute(mgr, &parms);
    if (ok)
    {
        *amount = parms.amount;
    }
    return ok;
}

// FileAPI.cpp:559
bool Read(Stream file, void * buffer, int64 start, int32 * amount, int extopt)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::readp;
    parms.stream = file;
    parms.buffer = buffer;
    parms.pos = start;
    parms.amount = *amount;
    parms.extopt = extopt;
    *amount = 0;
    bool ok = System_File::Stacked::Execute(mgr, &parms);
    if (ok)
    {
        *amount = parms.amount;
    }
    return ok;
}

// FileAPI.cpp:776
bool Write(Stream file, const void * buffer, int32 * amount)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::write;
    parms.stream = file;
    parms.buffer = const_cast<void *>(buffer);
    parms.amount = *amount;
    parms.pos = -1;
    bool ok = System_File::Stacked::Execute(mgr, &parms);
    if (ok)
    {
        *amount = parms.amount;
    }
    return ok;
}

// FileAPI.cpp:798
bool Write(Stream file, const void * buffer, int64 start, int32 * amount, int extopt)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::writep;
    parms.stream = file;
    parms.buffer = const_cast<void *>(buffer);
    parms.pos = start;
    parms.amount = *amount;
    parms.extopt = extopt;
    bool ok = System_File::Stacked::Execute(mgr, &parms);
    if (ok)
    {
        *amount = parms.amount;
    }
    return ok;
}

// FileAPI.cpp:679
bool SetEOF(Stream file, int64 pos, int posMode)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::seteof;
    parms.stream = file;
    parms.pos = pos;
    parms.posMode = posMode;
    return System_File::Stacked::Execute(mgr, &parms);
}

// FileAPI.cpp:757
bool SetWorkingDirectory(const char * buf)
{
    FileSystemPtr mgr = GetFilesystem<false>();
    System_File::Stacked::FileParms parms;
    parms.verb = &FileSystem::Functions::cd;
    parms.path = buf;
    return System_File::Stacked::Execute(mgr, &parms);
}

}  // namespace File
}  // namespace Blizzard

// ---- FileAPI.cpp ----
namespace Blizzard {
namespace File {
bool CreateDirectory(const char * dirPath, bool createSubDirs, bool);
bool Read(Stream file, void * buffer, int32 * amount);
bool Write(Stream file, const void * buffer, int32 * amount);
bool Write(Stream file, const void * buffer, int64 start, int32 * amount, int extopt);

// line 125
bool CreateDirectory(const char * dirPath, bool createSubDirs)
{
    return CreateDirectory(dirPath, createSubDirs, false);
}

// line 531
bool Read(Stream file, void * buffer, int32 amount)
{
    int32 amountRead = amount;
    return Read(file, buffer, &amountRead) && amountRead == amount;
}

// line 553
bool Read(Stream file, void * buffer, int64 start, int32 amount, int extopt)
{
    int32 amountRead = amount;
    return Read(file, buffer, start, &amountRead, extopt) && amountRead == amount;
}

// line 770
bool Write(Stream file, const void * buffer, int32 amount)
{
    int32 amountWritten = amount;
    return Write(file, buffer, &amountWritten);
}

// line 792
bool Write(Stream file, const void * buffer, int64 start, int32 amount, int extopt)
{
    int32 amountWritten = amount;
    return Write(file, buffer, start, &amountWritten, extopt);
}
}  // namespace File
}  // namespace Blizzard

namespace Blizzard {
namespace File {
// FileAPI.cpp:963
inline void SetLastExternalError(int err)
{
    System_File::FileError * last = &System_File::GetFileError();
    last->externalError = err;
}
}  // namespace File
}  // namespace Blizzard

// ---- File_NX64.cpp (lines 55-185, 331-761) ----
namespace System_File {
bool ReadImpl::Execute(Blizzard::File::Stream file, void * buffer, int64 start, int32 * amount)
{
    if (file->fileHandle.handle == NULL)
    {
        Blizzard::File::SetLastError(8);
        return false;
    }
    if (*amount == 0)
    {
        return true;
    }
    int64_t fileSize = 0;
    const int64_t offset = start == -1 ? 0 : start;
    if (!file->infoValid)
    {
        nn::Result nn_result = nn::fs::GetFileSize(&fileSize, file->fileHandle);
    }
    else
    {
        fileSize = file->info.size;
    }
    const int64_t maxBytesToRead = fileSize - offset;
    size_t bytesRead = 0;
    const uint64 bytesToRead = *amount <= maxBytesToRead ? *amount : maxBytesToRead;
    const nn::Result result = nn::fs::ReadFile(&bytesRead, file->fileHandle, offset, buffer, bytesToRead);
    if (result.IsFailure())
    {
        Blizzard::File::SetLastExternalError(result.GetInnerValueForDebug());
        Blizzard::File::SetLastError(5);
        Blizzard::File::AddToLastErrorStack(
            5,
            Blizzard::String::QFormat("NX64 Read (0x%.8x) - %s %u %u", bytesRead, file->pathname, bytesToRead,
                                      bytesRead),
            1);
        return false;
    }
    const bool readComplete = bytesRead == static_cast<size_t>(*amount);
    *amount = static_cast<int32>(bytesRead);
    if (!readComplete)
    {
        Blizzard::File::SetLastError(7);
    }
    return true;
}

bool WriteImpl::Execute(Blizzard::File::Stream file, const void * buffer, int64 start, int32 * amount)
{
    if (file->fileHandle.handle == NULL)
    {
        Blizzard::File::SetLastError(8);
        return false;
    }
    if (*amount == 0)
    {
        return true;
    }
    file->infoValid = false;
    const uint64 bytesToWrite = *amount;
    const int64_t offset = start == -1 ? 0 : start;
    const nn::fs::WriteOption tOption = nn::fs::WriteOption::MakeValue(nn::fs::WriteOptionFlag_Flush);
    const nn::Result result = nn::fs::WriteFile(file->fileHandle, offset, buffer, bytesToWrite, tOption);
    if (result.IsFailure())
    {
        Blizzard::File::SetLastError(6);
        Blizzard::File::AddToLastErrorStack(6, Blizzard::String::QFormat("NX64 Write - %s", file->pathname), 1);
        return false;
    }
    return true;
}

static bool sIsMount(const char * path)
{
    return Blizzard::String::EndsWith(path, ":");
}

static bool sIsFile(const char * path)
{
    nn::fs::FileHandle handle;
    const nn::Result result = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Read);
    if (result.IsSuccess())
    {
        nn::fs::CloseFile(handle);
        return true;
    }
    return nn::fs::ResultTargetLocked::Includes(result);
}

static bool sIsDirectory(const char * path)
{
    nn::fs::DirectoryHandle handle;
    if (nn::fs::OpenDirectory(&handle, path, nn::fs::OpenDirectoryMode_All).IsSuccess())
    {
        nn::fs::CloseDirectory(handle);
        return true;
    }
    return false;
}

namespace Stacked {
static Blizzard::File::Existence GetExistsFromNativePath(const char * pathIn)
{
    if (sIsMount(pathIn))
    {
        return Blizzard::File::isDirectory;
    }
    if (sIsFile(pathIn))
    {
        return Blizzard::File::isFile;
    }
    if (sIsDirectory(pathIn))
    {
        return Blizzard::File::isDirectory;
    }
    return Blizzard::File::missing;
}

static void sUpdateFileInfo(Blizzard::File::Stream stream)
{
    if (stream->streaming == NULL || static_cast<Blizzard::Storage::StorageUnit *>(stream->streaming->storagenode) == NULL)
    {
        nn::fs::GetFileSize(&stream->info.size, stream->fileHandle);
        stream->info.sparseSize = stream->info.size;
    }
    stream->info.createTime = 0;
    stream->info.modTime = 0;
    stream->info.accessTime = 0;
    stream->info.existence = Blizzard::File::isFile;
    stream->info.attributes = 0;
    stream->info.path = stream->pathname;
    stream->info.regularFile = true;
    stream->infoValid = true;
}

bool Exists(FileParms * parms)
{
    char szAbsolutePath[4096];
    char szNativePath[4096];
    const char * szPath = Blizzard::String::MakeNativePath(parms->path, szNativePath, sizeof(szNativePath));
    if (Blizzard::String::Find(szPath, ':') != NULL)
    {
        strncpy(szAbsolutePath, szPath, sizeof(szAbsolutePath));
    }
    else
    {
        snprintf(szAbsolutePath, sizeof(szAbsolutePath), "%s/%s", g_szCurrentDirectory, szPath);
    }
    parms->info->existence = GetExistsFromNativePath(szAbsolutePath);
    return true;
}

bool ProcessDirFast(FileParms * parms)
{
    const char * szOriginalPath = parms->path;
    void * cookie = parms->cookie;
    Blizzard::File::ProcessDirCallback callback = parms->dircallback;
    char szNativePath[4096];
    char szAbsolutePath[4096];
    const char * szPath = Blizzard::String::MakeNativePath(szOriginalPath, szNativePath, sizeof(szNativePath));
    if (Blizzard::String::Find(szPath, ':') != NULL)
    {
        strncpy(szAbsolutePath, szPath, sizeof(szAbsolutePath));
    }
    else
    {
        snprintf(szAbsolutePath, sizeof(szAbsolutePath), "%s/%s", g_szCurrentDirectory, szPath);
    }
    nn::fs::DirectoryHandle handle;
    if (nn::fs::OpenDirectory(&handle, szAbsolutePath, nn::fs::OpenDirectoryMode_All).IsFailure())
    {
        goto fail;
    }
    {
        int64_t entryCount = 0;
        if (nn::fs::GetDirectoryEntryCount(&entryCount, handle).IsFailure())
        {
            nn::fs::CloseDirectory(handle);
            goto fail;
        }
        nn::fs::DirectoryEntry * entries = Blizzard::Memory::Allocate(
            static_cast<int>(entryCount) * sizeof(nn::fs::DirectoryEntry), 0, "", 0, NULL);
        int64_t entriesRead = 0;
        if (nn::fs::ReadDirectory(&entriesRead, entries, handle, entryCount).IsFailure())
        {
            Blizzard::Memory::Free(entries, 0, "", 0, NULL);
            nn::fs::CloseDirectory(handle);
            goto fail;
        }
        bool bResult = true;
        {
                Blizzard::File::ProcessDirParms dirParms;
                dirParms.pathRoot = szAbsolutePath;
                dirParms.cookie = cookie;
                for (int64_t i = 0; i < entriesRead; i++)
                {
                    dirParms.filename = entries[i].name;
                    dirParms.isDir = entries[i].directoryEntryType == nn::fs::DirectoryEntryType_Directory;
                    const bool bContinue = callback(dirParms);
                    bResult &= bContinue;
                    if (!bContinue)
                    {
                        break;
                    }
                }
        }
        Blizzard::Memory::Free(entries, 0, "", 0, NULL);
        nn::fs::CloseDirectory(handle);
        return bResult;
    }
fail:
    Blizzard::File::SetLastError(8);
    return false;
}

bool GetFileInfo(FileParms * parms)
{
    if (parms->path != NULL)
    {
        char szNativePath[4096];
        char szAbsolutePath[4096];
        Blizzard::String::MakeNativePath(parms->path, szNativePath, sizeof(szNativePath));
        if (Blizzard::String::Find(szNativePath, ':') != NULL)
        {
            strncpy(szAbsolutePath, szNativePath, sizeof(szAbsolutePath));
        }
        else
        {
            snprintf(szAbsolutePath, sizeof(szAbsolutePath), "%s/%s", g_szCurrentDirectory, szNativePath);
        }
        nn::fs::FileHandle handle;
        const bool bIsFile = nn::fs::OpenFile(&handle, szAbsolutePath, nn::fs::OpenMode_Read).IsSuccess();
        bool bExists = true;
        Blizzard::File::Existence existence;
        if (bIsFile)
        {
            nn::fs::GetFileSize(&parms->info->size, handle);
            existence = Blizzard::File::isFile;
        }
        else if (sIsDirectory(szAbsolutePath))
        {
            parms->info->size = 0;
            existence = Blizzard::File::isDirectory;
        }
        else
        {
            bExists = false;
        }
        if (bExists)
        {
            parms->info->createTime = 0;
            parms->info->modTime = 0;
            parms->info->accessTime = 0;
            parms->info->regularFile = bIsFile;
            parms->info->existence = existence;
            parms->info->path = NULL;
            if (bIsFile)
            {
                nn::fs::CloseFile(handle);
            }
            return true;
        }
    }
    else if (parms->stream != NULL)
    {
        sUpdateFileInfo(parms->stream);
        parms->info = &parms->stream->info;
        if (parms->info->path == NULL)
        {
            parms->info->path = parms->stream->pathname;
        }
        return parms->stream->infoValid;
    }
    Blizzard::File::SetLastError(8);
    return false;
}

bool GetRootChars(FileParms * parms)
{
    const char * dirPath = parms->path;
    Blizzard::Util::Buffer<256, char> path(Blizzard::String::Length(dirPath) + 1);
    if (path.Size() < 2)
    {
        parms->pos = 0;
        return true;
    }
    Blizzard::String::MakeUnivPath(dirPath, path, static_cast<int>(path.Size()));
    char * buf = path;
    if (buf[1] == ':')
    {
        parms->pos = buf[2] == '/' ? 3 : 2;
        return true;
    }
    if (buf[0] == '/' && buf[1] == '/')
    {
        const char * curr = buf + 2;
        for (int loop = 0; loop < 2; loop++)
        {
            curr = strchr(curr, '/');
            if (curr == NULL)
            {
                parms->pos = path.Size();
                return true;
            }
            curr++;
        }
        parms->pos = static_cast<uint32>(curr - buf);
        return true;
    }
    parms->pos = 0;
    return true;
}

bool CreateDirectory(FileParms * parms)
{
    char szAbsolutePath[4096];
    const char * szPath = parms->path;
    if (Blizzard::String::Find(szPath, ':') != NULL)
    {
        strncpy(szAbsolutePath, szPath, sizeof(szAbsolutePath));
    }
    else
    {
        snprintf(szAbsolutePath, sizeof(szAbsolutePath), "%s/%s", g_szCurrentDirectory, szPath);
    }
    if (IsROMPathNX64(szAbsolutePath))
    {
        return false;
    }
    if (!Blizzard::String::EndsWith(szAbsolutePath, "/"))
    {
        Blizzard::String::Append(szAbsolutePath, "/", sizeof(szAbsolutePath));
    }
    if (!parms->recursive)
    {
        return nn::fs::CreateDirectory(szAbsolutePath).IsSuccess();
    }
    const char * separator = Blizzard::String::FindPathSeparator(szAbsolutePath);
    bool bResult = separator != NULL;
    if (bResult)
    {
        separator = Blizzard::String::FindPathSeparator(separator + 1);
        while (separator != NULL)
        {
            char szPartialPath[4096];
            Blizzard::String::MemCopy(szPartialPath, szAbsolutePath, separator - szAbsolutePath);
            szPartialPath[separator - szAbsolutePath] = '\0';
            if (!sIsDirectory(szPartialPath) && nn::fs::CreateDirectory(szPartialPath).IsFailure())
            {
                bResult = false;
                break;
            }
            separator = Blizzard::String::FindPathSeparator(separator + 1);
        }
    }
    if (!bResult)
    {
        Blizzard::File::SetLastError(8);
    }
    return bResult;
}

bool MakeAbsolutePath(FileParms * parms)
{
    char szNativePath[4096];
    char szAbsolutePath[4096];
    Blizzard::String::MakeNativePath(parms->path, szNativePath, sizeof(szNativePath));
    if (Blizzard::String::Find(szNativePath, ':') != NULL)
    {
        strncpy(szAbsolutePath, szNativePath, sizeof(szAbsolutePath));
    }
    else
    {
        snprintf(szAbsolutePath, sizeof(szAbsolutePath), "%s/%s", g_szCurrentDirectory, szNativePath);
    }
    strncpy(parms->cwdbuf, szAbsolutePath, parms->cwdbuflen);
    Blizzard::String::MakeUnivPath(parms->cwdbuf, parms->cwdbuf, parms->cwdbuflen);
    if (parms->path[Blizzard::String::Length(parms->path) - 1] == '/')
    {
        Blizzard::String::ForceTrailingSeparator(parms->cwdbuf, parms->cwdbuflen, '/');
    }
    return true;
}

bool Open(FileParms * parms)
{
    const int mode = parms->mode;
    const bool bWrite = (mode & 10) != 0;
    const bool bRead = (mode & 5) != 0;
    char szNativePath[4096];
    char szAbsolutePath[4096];
    Blizzard::String::MakeNativePath(parms->path, szNativePath, sizeof(szNativePath));
    if (Blizzard::String::Find(szNativePath, ':') != NULL)
    {
        strncpy(szAbsolutePath, szNativePath, sizeof(szAbsolutePath));
    }
    else
    {
        snprintf(szAbsolutePath, sizeof(szAbsolutePath), "%s/%s", g_szCurrentDirectory, szNativePath);
    }
    if (IsMountedROM() && bWrite && IsROMPathNX64(szAbsolutePath))
    {
        return false;
    }
    int nnMode = bRead ? nn::fs::OpenMode_Read : 0;
    if (bWrite)
    {
        nnMode |= nn::fs::OpenMode_Write | nn::fs::OpenMode_AllowAppend;
    }
    if (mode & Blizzard::File::Mode::recreate)
    {
        nn::fs::DeleteFile(szAbsolutePath);
        nn::fs::CreateFile(szAbsolutePath, 0);
    }
    nn::fs::FileHandle handle;
    handle.handle = NULL;
    const nn::Result result = nn::fs::OpenFile(&handle, szAbsolutePath, nnMode);
    if (result.IsFailure())
    {
        Blizzard::File::SetLastExternalError(result.GetInnerValueForDebug());
        Blizzard::File::SetLastError(4);
        Blizzard::File::AddToLastErrorStack(
            4,
            Blizzard::String::QFormat("NX64 File Open, error: %u mode: %08x file: %s", result.GetInnerValueForDebug(),
                                      nnMode, szAbsolutePath),
            1);
        return false;
    }
    Blizzard::File::Stream stream = parms->stream;
    if (stream == NULL)
    {
        const int length = static_cast<int>(Blizzard::String::Length(szAbsolutePath));
        stream = Blizzard::Memory::Allocate(sizeof(Blizzard::File::StreamRecord) + length + 1,
                                            Blizzard::Memory::ZEROFILL, "", 0, NULL);
        stream->mode = nnMode;
        stream->pathname = reinterpret_cast<char *>(stream + 1);
        Blizzard::String::Copy(stream->pathname, szAbsolutePath, length + 1);
    }
    stream->fileHandle = handle;
    if (stream->streaming == NULL || static_cast<Blizzard::Storage::StorageUnit *>(stream->streaming->storagenode) == NULL)
    {
        nn::fs::GetFileSize(&stream->info.size, stream->fileHandle);
        stream->info.sparseSize = stream->info.size;
    }
    stream->info.createTime = 0;
    stream->info.modTime = 0;
    stream->info.accessTime = 0;
    stream->info.existence = Blizzard::File::isFile;
    stream->info.path = stream->pathname;
    stream->info.regularFile = true;
    stream->info.attributes = 0;
    stream->infoValid = true;
    parms->stream = stream;
    return true;
}

bool RemoveDirectory(FileParms * parms)
{
    char szAbsolutePath[4096];
    GetAbsolutePath(parms->path, szAbsolutePath, sizeof(szAbsolutePath));
    if (IsROMPathNX64(parms->path))
    {
        return false;
    }
    nn::Result result = parms->recursive ? nn::fs::DeleteDirectoryRecursively(szAbsolutePath)
                                         : nn::fs::DeleteDirectory(szAbsolutePath);
    return result.IsSuccess();
}
}  // namespace Stacked

// ---- FileAPI.cpp (lines 1016-1029) ----
void Shutdown();

class UsingFile : public Blizzard::Using::ScopedUse {  // size 0x8
public:
    UsingFile(const char * usingName, void (*shutdownProc)()) : ScopedUse(usingName, shutdownProc)
    {
        Blizzard::Using::DependsOn("Blizzard::File", "Blizzard::Memory");
        Blizzard::Using::DependsOn("Blizzard::File", "Blizzard::ThreadLocalStorage");
    }
};

UsingFile s_usingFile("Blizzard::File", Shutdown);

void Shutdown()
{
    {
        Blizzard::File::FileSystemPtr mgr = Blizzard::File::GetFilesystem<false>();
        Stacked::FileParms parms;
        parms.verb = &Blizzard::File::FileSystem::Functions::shutdown;
        Stacked::Execute(mgr, &parms);
    }
    Blizzard::File::FileSystemPtr * ptr = Stacked::s_manager;
    Stacked::s_manager = NULL;
    delete ptr;
}
}  // namespace System_File

namespace Blizzard {
namespace File {
// ---- Dir.cpp (line 44) ----
bool IsFile(const char * path)
{
    return Exists(path) == isFile;
}
}  // namespace File
}  // namespace Blizzard

namespace System_File {
bool PreallocateSlow(const char * path, int flags, Blizzard::File::PreallocParms * parms);
bool PreallocateSlow(Blizzard::File::Stream stream, Blizzard::File::PreallocParms * parms);
}  // namespace System_File

namespace Blizzard {
namespace File {
// ---- FileAPI.cpp (lines 102, 491) ----
inline bool Preallocate(const char * path, int mode, PreallocParms * parms)
{
    bool success = false;
    if (mode & 0x4000)
    {
        success = System_File::PreallocateSlow(path, mode, parms);
    }
    return success;
}

bool Create(const char * path, int mode, PreallocParms * parms)
{
    if (mode & 0x6000)
    {
        return Preallocate(path, mode, parms);
    }
    Stream file;
    if (Open(path, mode, file))
    {
        Close(file);
        if (parms != NULL)
        {
            parms->result = 2;
        }
        return true;
    }
    if (parms != NULL)
    {
        parms->result = 1;
    }
    return false;
}
}  // namespace File
}  // namespace Blizzard

// ---- Preallocate.cpp ----
namespace System_File {
bool PreallocateSlow(const char * path, int flags, Blizzard::File::PreallocParms * parms)
{
    Blizzard::File::Stream file;
    Blizzard::File::FileSystemPtr mgr = Blizzard::File::GetFilesystem<false>();
    bool result = Blizzard::File::Open(mgr, path, flags, file);
    if (result)
    {
        result = PreallocateSlow(file, parms);
        Blizzard::File::Close(file);
    }
    return result;
}

// NON_MATCHING: 8 of 150 instructions. The original zero-extends incrementBytes where it becomes
// the buffer size (mov w22, w10); this sign-extends it (sxtw x22, w9), which also swaps w9/w10 in
// the clamp above it. About 45 forms of the clamp were tried; clamping to 100000000 before the
// std::max gives the zero-extension but tests the two bounds in the wrong order.
bool PreallocateSlow(Blizzard::File::Stream stream, Blizzard::File::PreallocParms * parms)
{
    int incrementBytes = std::max(1000000, parms != NULL ? parms->interval : 0);
    if (incrementBytes > 100000000)
    {
        incrementBytes = 100000000;
    }
    int64 bytesWritten;
    Blizzard::Util::Buffer<0, char> zeroMemory(incrementBytes);
    Blizzard::String::MemFill(zeroMemory, incrementBytes, 0);
    bytesWritten = Blizzard::File::GetFileInfo(stream)->size;
    if (bytesWritten > parms->size)
    {
        Blizzard::File::SetEOF(stream, parms->size, 0);
    }
    else
    {
        while (parms->size > bytesWritten)
        {
            const int bytesInThisWrite = static_cast<int>(parms->size - bytesWritten > incrementBytes ? incrementBytes : parms->size - bytesWritten);
            if (!Blizzard::File::Write(stream, zeroMemory, bytesWritten, bytesInThisWrite, 0))
            {
                Blizzard::File::SetLastError(9);
                Blizzard::File::AddToLastErrorStack(
                    9, Blizzard::String::QFormat("PreallocateSlow - filewrite - %s", stream->pathname), 1);
                return false;
            }
            bytesWritten += bytesInThisWrite;
            if (parms->callback != NULL && !parms->callback(bytesWritten, parms->context))
            {
                parms->result = 5;
                Blizzard::File::SetLastError(9);
                Blizzard::File::AddToLastErrorStack(
                    9, Blizzard::String::QFormat("PreallocateSlow - canceled - %s", stream->pathname), 1);
                return false;
            }
        }
        if (parms->size != bytesWritten)
        {
            return false;
        }
    }
    parms->result = 3;
    return true;
}
}  // namespace System_File

// ---- File.cpp (line 290) ----
namespace System_File {
bool Copy(const Blizzard::File::FileSystemPtr &, Stacked::FileParms * parms)
{
    using namespace Blizzard;

    const char * oldFileName = parms->path;
    const char * newFileName = parms->newpath;
    bool overwrite = parms->overwrite;





    const int32 COPY_BLOCK_SIZE = 0x40000;


    char stdOld[1024];
    char stdNew[1024];
    String::MakeNativePath(oldFileName, stdOld, sizeof(stdOld));
    String::MakeNativePath(newFileName, stdNew, sizeof(stdNew));


    File::Stream srcFile;
    if (!File::Open(stdOld, 0x1005, srcFile))
    {


        File::SetLastError(2);
        return false;
    }


    File::Stream destFile;
    int mode = overwrite ? 0x402 : 0xc02;
    if (!File::Open(stdNew, mode, destFile))
    {
        File::Close(srcFile);
        File::SetLastError(4);
        return false;
    }

    int64 fileSize = File::GetFileInfo(srcFile)->size;
    int64 bytesWritten = 0;
    bool result = true;


    uint8 * buf;
    if (fileSize < COPY_BLOCK_SIZE)
    {
        buf = Memory::Allocate(static_cast<uint32>(fileSize));
    }
    else
    {
        buf = Memory::Allocate(COPY_BLOCK_SIZE);
    }
    while (bytesWritten < fileSize)
    {
        int64 amountLeft = fileSize - bytesWritten;
        int32 amount = amountLeft < COPY_BLOCK_SIZE ? static_cast<int32>(amountLeft) : COPY_BLOCK_SIZE;
        if (!File::Read(srcFile, buf, bytesWritten, &amount, 0) ||
            !File::Write(destFile, buf, bytesWritten, &amount, 0))
        {
            result = false;
            break;
        }
        bytesWritten += amount;
    }
    Memory::Free(buf);
    File::Close(srcFile);
    File::Close(destFile);
    if (!result)
    {
        File::Delete(stdNew);
    }
    return result;
}
}  // namespace System_File

// ---- Filestack_Readonly.cpp ----
namespace System_File {
namespace Stacked {
static bool ro_write(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

static bool ro_writep(const Blizzard::File::FileSystemPtr &, FileParms *)
{
    return false;
}

// line 40
static inline void FsSetDefaultFunction(Blizzard::File::FileSystem::Functions * fs, FileFunc f)
{
    for (FileFunc * func = &fs->checksparsefilesupport; func <= &fs->shutdown; func++)
    {
        *func = f;
    }
}

// line 53
static inline Blizzard::File::FileSystem::Functions FsInitializer(Blizzard::File::FileSystem::Functions * fs)
{
    FsSetDefaultFunction(fs, NULL);
    fs->write = ro_write;
    fs->writep = ro_writep;
    return *fs;
}

// line 62
Blizzard::File::FileSystem::Functions s_readonly_stack = FsInitializer(&s_readonly_stack);
}  // namespace Stacked
}  // namespace System_File
