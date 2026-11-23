// Original file: X/Sigma/Files/XFilesCore.cpp
#include "XFiles.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"
#include "../../../Contrib/Contrib/PCRE/pcre.h"
#include "../DataStructures/XGrowableList.h"
#include "../ErrorManager/ErrorManager.h"
#include "../Math/MathGeneral.h"
#include "../Unicode.h"

#include <stdio.h>
#include <string.h>





















FileReference::FileReference()
{
}
// BlizzardCore functions whose headers do not declare them yet (signatures from the DWARF).
namespace Blizzard {
namespace File {
uint32 GetRootChars(const char * dirPath);
bool Create(const char * path, int mode, Blizzard::File::PreallocParms * parms);
bool Move(const char * oldFileName, const char * newFileName);
Blizzard::File::Existence Exists(const char * path);
int GetLastError();
int GetLastExternalError();
bool ProcessDirFast(const char * path, void * cookie, Blizzard::File::ProcessDirCallback callback, bool treatSymlinksAsFiles);
bool Write(Blizzard::File::Stream file, const void * buffer, int64 start, int32 amount, int extopt);
bool GetFileInfo(const char * path, Blizzard::File::FileInfo * info);
}  // namespace File
namespace String {
const char * FindFilename(const char * buf);
const char * FindExtension(const char * buf);
void ForceTrailingSeparator(char * buf, int bufMax, char preferredSlashType);
void JoinPath(char * path, int pathLen, const char * dir, const char * filename);
const char * MakeBackslashPath(const char * rawpath, char * fixedpath, int fixedpathMax);
}  // namespace String
namespace Process {
bool GetProcessDirectory(char * buf, int bufMax);
}  // namespace Process
}  // namespace Blizzard








void FileReferenceInit( FileReference & tFileRef, LPCSTR szPath )
{
    tFileRef.Clear();
    Blizzard::String::Copy( tFileRef.szPath, szPath, sizeof( tFileRef.szPath ) );
}






void FileReferenceAddDirectory( FileReference & tFileRef, LPCSTR szDirectory )
{
    Blizzard::String::ForceTrailingSeparator( tFileRef.szPath, sizeof( tFileRef.szPath ), 0 );
    Blizzard::String::Append( tFileRef.szPath, szDirectory, sizeof( tFileRef.szPath ) );
}






LPCSTR FileReferenceGetPath( const FileReference & tFileRef )
{
    return tFileRef.szPath;
}





int32 FileReferenceGetRefCount( const FileReference & tFileRef )
{
    return tFileRef.nRefcount;
}





void FileSplitPath( LPCSTR szPath, LPSTR pszDrive, LPSTR pszDirectory, LPSTR pszFilename, LPSTR pszExtension )
{
    LPCSTR directory = szPath + Math::Max( (int32)Blizzard::File::GetRootChars( szPath ) - 1, 0 );
    LPCSTR filename = Blizzard::String::FindFilename( directory );
    LPCSTR extension = Blizzard::String::FindExtension( filename );

    if ( pszDrive != NULL )
    {
        Blizzard::String::Copy( pszDrive, szPath, directory - szPath + 1 );
    }
    if ( pszDirectory != NULL )
    {
        Blizzard::String::Copy( pszDirectory, directory, filename - directory + 1 );
    }
    if ( pszFilename != NULL )
    {
        Blizzard::String::Copy( pszFilename, filename, extension - filename );
    }
    if ( pszExtension != NULL )
    {
        if ( *extension != 0 )
        {
            extension--;
        }
        strcpy( pszExtension, extension );
    }
}





























void FileGetProcessPath( LPSTR szProcessPath )
{
    Blizzard::Process::GetProcessDirectory( szProcessPath, 4096 );
}






BOOL FileDirectoryCreate( FileReference & tFileRef, const BOOL fCreateSubDirs )
{
    return Blizzard::File::CreateDirectory( tFileRef.szPath, fCreateSubDirs != FALSE );
}












































































































































































































BOOL FileCreate( FileReference & tFileRef )
{
    return Blizzard::File::Create( tFileRef.szPath, 0xc02, NULL );
}








































































































































BOOL FileDelete( FileReference & tFileRef )
{
    return Blizzard::File::Delete( tFileRef.szPath );
}
















BOOL FileRename( FileReference & tFileRef, LPCSTR szNewName )
{
    if ( Blizzard::File::Move( tFileRef.szPath, szNewName ) )
    {
        Blizzard::String::Copy( tFileRef.szPath, szNewName, sizeof( tFileRef.szPath ) );
        return TRUE;
    }
    return FALSE;
}

BOOL FileExists( const FileReference & tFileRef );












































































































































BOOL FileMove( FileReference & tFileRefSrc, FileReference & tFileRefDest )
{
    bool success = false;

    if ( FileExists( tFileRefSrc ) )
    {
        success = Blizzard::File::Move( tFileRefSrc.szPath, tFileRefDest.szPath );
    }
    return success;
}

















BOOL FileExists( const FileReference & tFileRef )
{
    return Blizzard::File::Exists( tFileRef.szPath );
}






































































































































































BOOL FileOpen( FileReference & tFileRef, uint32 dwAccessFlags )
{
    if ( tFileRef.nRefcount == 0 )
    {
        uint32 perm = dwAccessFlags & 3;
        uint32 mode;
        bool success;
        switch ( perm )
        {
        case 1:
            mode = 0x1005;
            break;
        case 2:
            mode = 0x1002;
            break;
        case 3:
            mode = 0x1007;
            break;
        default:
            DisplayErrorMessage( ERROR_FILE_OPEN, "" );
            return FALSE;
        }
        tFileRef.dwAccessFlags = dwAccessFlags;
        success = Blizzard::File::Open( tFileRef.szPath, mode | ( ( dwAccessFlags & 4 ) << 4 ), tFileRef.tStream );
        if ( !success )
        {
            int nLastError = Blizzard::File::GetLastError();
            int nLastExternalError = Blizzard::File::GetLastExternalError();
            return FALSE;
        }
    }
    else if ( tFileRef.dwAccessFlags != dwAccessFlags )
    {
        DisplayErrorMessage( ERROR_FILE_OPEN, "" );
        return FALSE;
    }
























    Blizzard::Lock::Atomic::Increment( &tFileRef.nRefcount );
    return TRUE;
}



























































void FileClose( FileReference & tFileRef )
{

    int32 nRefcount = Blizzard::Lock::Atomic::Decrement( &tFileRef.nRefcount );
    if ( nRefcount == 0 )
    {
        bool success = Blizzard::File::Close( tFileRef.tStream );
        tFileRef.tStream = NULL;
    }
}



























































































































BOOL FileRead( FileReference & tFileRef, void * lpBuf, const uint32 dwOffset, const uint32 dwSize, const ErrorCode eErrorCodeIfFails )
{
    if ( Blizzard::File::Read( tFileRef.tStream, lpBuf, (int64)dwOffset, (int32)dwSize, 0 ) )
    {
        return TRUE;
    }
    if ( eErrorCodeIfFails != ERROR_NONE )
    {
        DisplayErrorMessage( eErrorCodeIfFails, "" );
    }
    return FALSE;
}














































BOOL FileWrite( FileReference & tFileRef, const void * lpBuf, const uint32 dwOffset, const uint32 dwSize, ErrorCode eErrorCodeIfFails )
{
    bool success;
    if ( dwOffset == (uint32)-1 )
    {
        success = Blizzard::File::Write( tFileRef.tStream, lpBuf, (int32)dwSize );
    }
    else
    {
        success = Blizzard::File::Write( tFileRef.tStream, lpBuf, (int64)dwOffset, (int32)dwSize, 0 );
    }
    if ( !success )
    {
        int nLastError = Blizzard::File::GetLastError();
        int nLastExternalError = Blizzard::File::GetLastExternalError();
        DisplayErrorMessage( eErrorCodeIfFails, "" );
        return FALSE;
    }
    return TRUE;
}








BOOL FileWriteSequential( FileReference & tFileRef, const void * lpBuf, uint32 & dwOffset, const uint32 dwSize )
{




    if ( !FileWrite( tFileRef, lpBuf, dwOffset, dwSize, ERROR_FILE_WRITE ) )
    {
        return FALSE;
    }
    dwOffset += dwSize;
    return TRUE;
}



















BOOL FileGetSize( const FileReference & tFileRef, uint32 & dwSize )
{
    Blizzard::File::FileInfo tInfo;
    bool success;
    if ( tFileRef.tStream != NULL )
    {
        success = Blizzard::File::GetFileInfo( tFileRef.tStream, &tInfo );
    }
    else
    {
        success = Blizzard::File::GetFileInfo( tFileRef.szPath, &tInfo );
    }
    if ( success )
    {
        dwSize = (uint32)tInfo.size;
    }
    return success;
}

class FileFindState {  // size 0xc8
public:
    FileFindState(const char * pattern, bool r, bool dirs);
    ~FileFindState();
    static void DoGlob(FileFindState * state, LPCSTR path);
    struct Path {  // size 0x1000
        char path[4096];  // 0x00
    };
    bool recursive;  // 0x00
    bool includeDirs;  // 0x01
    pcre * patternExpression;  // 0x08
    pcre_extra * patternExtra;  // 0x10
    XGrowableList<FileFindState::Path, const FileFindState::Path &> fileList;  // 0x18
    XGrowableList<FileFindState::Path, const FileFindState::Path &> dirList;  // 0x70
};
static_assert(sizeof(FileFindState) == 0xc8, "FileFindState");



















inline FileFindState::FileFindState( const char * pattern, bool r, bool dirs ) : recursive( r ), includeDirs( dirs )
{
    {
        const char * errorMessage;
        int32 errorOffset;
        patternExpression = pcre_compile( pattern, PCRE_CASELESS, &errorMessage, &errorOffset, NULL );
        patternExtra = pcre_study( patternExpression, 0, &errorMessage );
    }
}




inline FileFindState::~FileFindState()
{
    if ( patternExtra != NULL )
    {
        pcre_free( patternExtra );
    }
    if ( patternExpression != NULL )
    {
        pcre_free( patternExpression );
    }
}
















void FileFindState::DoGlob( FileFindState * state, LPCSTR path )
{
    struct Internal
    {




        static bool GetItem( const Blizzard::File::ProcessDirParms & data )
        {
            FileFindState * state = (FileFindState *)data.cookie;
            FileFindState::Path * addedItem = NULL;
            if ( data.isDir && state->recursive )
            {
                addedItem = &state->dirList.GetAt( state->dirList.AddTail_Tracked( FileFindState::Path() ) );
            }
            else if ( !data.isDir || state->includeDirs )
            {
                int32 resultVector[3];
                int32 result = pcre_exec( state->patternExpression, state->patternExtra, data.filename, (int)Blizzard::String::Length( data.filename ), 0, 0, resultVector, 3 );
                if ( result >= 0 )
                {
                    addedItem = &state->fileList.GetAt( state->fileList.AddTail_Tracked( FileFindState::Path() ) );
                }
            }
            if ( addedItem != NULL )
            {
                Blizzard::String::JoinPath( addedItem->path, sizeof( addedItem->path ), data.pathRoot, data.filename );
                Blizzard::String::MakeBackslashPath( addedItem->path, addedItem->path, sizeof( addedItem->path ) );
            }
            return true;
        }
    };
    Blizzard::File::ProcessDirFast( path, state, Internal::GetItem, false );
}















void FileConvertDOSGlobToRegEx( char * szRegexPattern, size_t nRegexPatternLen, const char * szDosPattern )
{
    if ( strcmp( szDosPattern, "*.*" ) == 0 )
    {
        strcpy( szRegexPattern, ".*$" );
        return;
    }

    bool foundWildcard = false;
    char * szRegexPatternEnd = szRegexPattern;
    *szRegexPatternEnd++ = '^';
    for ( ;; )
    {
        char c = *szDosPattern++;
        switch ( c )
        {
        case 0:
            *szRegexPatternEnd++ = '$';
            *szRegexPatternEnd = 0;
            return;
        case '*':
            if ( !foundWildcard )
            {
                *szRegexPatternEnd++ = '.';
                *szRegexPatternEnd++ = '*';
                foundWildcard = true;
            }
            break;
        case '?':
            if ( !foundWildcard )
            {
                *szRegexPatternEnd++ = '.';
            }
            break;
        case '.':
            *szRegexPatternEnd++ = '\\';
            *szRegexPatternEnd++ = '.';
            foundWildcard = false;
            break;
        case '$':
        case '(':
        case ')':
        case '+':
        case '[':
        case '\\':
        case ']':
        case '^':
        case '{':
        case '}':
            if ( !foundWildcard )
            {
                *szRegexPatternEnd++ = '\\';
                *szRegexPatternEnd++ = c;
            }
            break;
        default:
            if ( !foundWildcard )
            {
                *szRegexPatternEnd++ = c;
            }
            break;
        }
    }
}





































void FileFindStart( void *& portableState, const FileReference & tBaseRef, LPCSTR szFilename, const BOOL fFindRecursive, const BOOL fIncludeDirs )
{
    char regexPattern[4096];
    FileConvertDOSGlobToRegEx( regexPattern, sizeof( regexPattern ), szFilename );








    FileFindState *& state = (FileFindState *&)portableState;

    state = new FileFindState( regexPattern, fFindRecursive != FALSE, fIncludeDirs == TRUE );


    FileFindState::DoGlob( state, tBaseRef.szPath );
}




BOOL FileFindNext( void * portableState, FileReference & tResult )
{
    FileFindState * state = (FileFindState *)portableState;





    for ( ;; )
    {
        if ( !state->fileList.IsEmpty() )
        {
            FileReferenceInit( tResult, state->fileList.GetHead().path );
            state->fileList.RemoveHead();
            return TRUE;
        }
        if ( state->dirList.IsEmpty() )
        {
            return FALSE;
        }
        FileFindState::DoGlob( state, state->dirList.GetHead().path );
        state->dirList.RemoveHead();
    }
}




void FileFindClose( void *& portableState )
{


    FileFindState *& state = (FileFindState *&)portableState;
    delete state;
}

bool IsROMPathNX64( const char * path );  // BlizzardCore/Source/System/File/File_NX64.cpp:35
namespace Blizzard { namespace String { const char * MakeNativePath( const char * rawpath, char * fixedpath, int fixedpathMax ); } }








FILE * fopen_ex( const char * filename, const char * mode )
{
    char szNativePath[4096];
    Blizzard::String::MakeNativePath( filename, szNativePath, sizeof( szNativePath ) );
    if ( Blizzard::String::Find( mode, 'w' ) != NULL && IsROMPathNX64( filename ) )
    {
        return NULL;
    }
    return fopen( szNativePath, mode );
}
