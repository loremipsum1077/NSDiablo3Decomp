// Original file: X/Common/FileIO/FileIO.cpp
#include "FileIO.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/RefString.h"

// X/Common/Console/IO/SFileWrapper.cpp (its header is not in src yet; signatures from the DWARF).
bool SFileCloseFile(HSFILERecord * pFile);
Blizzard::Mopaq::ErrorCode SFileGetLastError();
bool SFileGetFileName(HSFILERecord * pFile, char * pNameBuffer, unsigned int dwNameBufferSize);
bool SFileReadFile(HSFILERecord * pFile, void * pBuffer, unsigned int dwBytesToRead, unsigned int * pdwBytesRead);
uint64 SFileSetFilePointer(HSFILERecord * pFile, int64 nOffset, unsigned int dwMoveMethod);
Blizzard::Mopaq::OpenFileResult SFileOpenFileEx(HSARCHIVE OpaqueArchive, uint64 dwHash, HSFILERecord ** ppFile, Console::IO::FileInfo * ptFileInfo);
Blizzard::Mopaq::OpenFileResult SFileOpenFileEx(HSARCHIVE OpaqueArchive, const char * szFilename, unsigned int dwFlags, HSFILERecord ** ppFile);
Blizzard::Mopaq::FileExistsResult SFileFileExists(HSARCHIVE OpaqueArchive, const char * szFilename, unsigned int, HSARCHIVE * pExistsInArchive);
Blizzard::IOOffset SFileGetFileStartingLocation(HSFILERecord * pFile);
int64 SFileGetFileSize(HSFILERecord * pFile);
int64 SFileGetFileCompressedSize(HSFILERecord * pFile);
Blizzard::Mopaq::OpenFileResult SFileOpenFile(HSFILERecord * pFile, HSFILERecord ** ppFile);
// X/Common/SNO/SNOFiles.cpp
void SNOFileGetFilePath(const SNOName & snoname, char * szFilePath, int32 nFilePathMaxSize);
void sTraceBinary(const uint8 * pData, size_t nSize);

// BlizzardCore/Source/System/String/Path.cpp:286; no adopted header declares it yet.
namespace Blizzard { namespace String { char * FindExtensionWithLeadingDot(char * buf); } }

using namespace Blizzard::Mopaq;

namespace FileIO {

class File::FileImpl {  // size 0x10
public:
    XLocale m_eLocale;  // 0x00
    HSFILE m_hsFileHandle;  // 0x08
    FileImpl(XLocale eLocale);
    ~FileImpl();
    bool OpenCDPFile(const char * szFilename, FileIO::OpenFileFlags eFlags);
    bool OpenSNO(const SNOName & tSNOName, XLocale eLocale);
    bool OpenChildSNO(const SNOName & tSNOName, const char * szChildFile, XLocale eLocale);
    bool Open(const char * szFilename, XLocale eLocale);
    bool Open(const char * szDiskFilename, const char * szNonDiskFilename, XLocale eLocale);
    bool Open(const FileIO::File & tFile);
    bool OpenConsoleFile(HSARCHIVE ptArchive, uint64 dwHash, Console::IO::FileInfo * ptFileInfo);
    bool GetFileName(char * sBuffer, uint32 nBufferSize) const;
    int64 GetFileSize() const;
    int64 GetCompressedFileSize() const;
    uint64 SetFileCursor(int64 dwDistanceToMove, FileIO::FilePointerMovementMethod eMovementMethod);
    bool SimpleRead(void * pBuffer, uint64 dwBytesToRead, uint64 * pdwBytesRead);
    bool ReadFile(void * pBuffer, uint64 dwBytesToRead, uint64 * pdwBytesRead);
    bool TraceFile();
    bool Close();
    bool IsAlreadyOpen() const;
    int64 GetSNOFileStartingLocation() const;
};


































































































































































































































































inline File::FileImpl::FileImpl( XLocale eLocale )
    : m_eLocale( eLocale )
    , m_hsFileHandle( NULL )
{
}








inline File::FileImpl::~FileImpl()
{
    Close();
}

inline bool File::FileImpl::OpenCDPFile( const char * szFilename, FileIO::OpenFileFlags eFlags )
{
    OpenFileResult result = SFileOpenFileEx( 0, szFilename, eFlags, &m_hsFileHandle );
    return result != SFILEOPEN_FAIL;
}




inline bool File::FileImpl::OpenSNO( const SNOName & tSNOName, XLocale eLocale )
{
    char szFileName[4096];
    SNOFileGetFilePath( tSNOName, szFileName, sizeof( szFileName ) );
    {
        Blizzard::Mopaq::OpenFileFlags flags = 0;
        return OpenCDPFile( szFileName, flags );
    }
}































inline bool File::FileImpl::OpenChildSNO( const SNOName & tSNOName, const char * szChildFile, XLocale eLocale )
{
    char szPath[4096];
    SNOFileGetFilePath( tSNOName, szPath, sizeof( szPath ) );
    CRefString strFolder( szPath, (int32)( Blizzard::String::FindExtensionWithLeadingDot( szPath ) - szPath ) );
    char szFileName[4096];
    Blizzard::String::Format( szFileName, sizeof( szFileName ), "%s\\%s", strFolder.str(), szChildFile );
    {
        Blizzard::Mopaq::OpenFileFlags flags = 0;



































        return OpenCDPFile( szFileName, flags );
    }
}










































inline bool File::FileImpl::Open( const char * szFilename, XLocale eLocale )
{
    Blizzard::Mopaq::OpenFileFlags flags = 0;
    return OpenCDPFile( szFilename, flags );
}























inline bool File::FileImpl::Open( const char * szDiskFilename, const char * szNonDiskFilename, XLocale eLocale )
{
    {


























        bool bResult = OpenCDPFile( szNonDiskFilename, 0 );
        return bResult;
    }
}
inline bool File::FileImpl::Open( const FileIO::File & tFile )
{
    return SFileOpenFile( tFile.GetHSFILE(), &m_hsFileHandle ) == SFILEOPEN_MPQ;
}

















inline bool File::FileImpl::OpenConsoleFile( HSARCHIVE ptArchive, uint64 dwHash, Console::IO::FileInfo * ptFileInfo )
{
    return SFileOpenFileEx( ptArchive, dwHash, &m_hsFileHandle, ptFileInfo ) == SFILEOPEN_MPQ;
}
























inline bool File::FileImpl::GetFileName( char * sBuffer, uint32 nBufferSize ) const
{
    return SFileGetFileName( m_hsFileHandle, sBuffer, nBufferSize );
}













inline int64 File::FileImpl::GetFileSize() const
{
    return SFileGetFileSize( m_hsFileHandle );
}














inline int64 File::FileImpl::GetCompressedFileSize() const
{
    return SFileGetFileCompressedSize( m_hsFileHandle );
}































inline uint64 File::FileImpl::SetFileCursor( int64 dwDistanceToMove, FileIO::FilePointerMovementMethod eMovementMethod )
{
    return SFileSetFilePointer( m_hsFileHandle, dwDistanceToMove, eMovementMethod );
}































































































































inline bool File::FileImpl::SimpleRead( void * pBuffer, uint64 dwBytesToRead, uint64 * pdwBytesRead )
{
    uint32 nBytesRead;
    bool bResult = SFileReadFile( m_hsFileHandle, pBuffer, (uint32)dwBytesToRead, &nBytesRead );
    if ( pdwBytesRead != NULL )
    {
        *pdwBytesRead = nBytesRead;
    }
    return bResult;
}









































































































































inline bool File::FileImpl::ReadFile( void * pBuffer, uint64 dwBytesToRead, uint64 * pdwBytesRead )
{
    return SimpleRead( pBuffer, dwBytesToRead, pdwBytesRead );
}







































































inline bool File::FileImpl::TraceFile()
{
    unsigned char szBuffer[256];
    uint32 nLen = sizeof( szBuffer );
    SFileSetFilePointer( m_hsFileHandle, 0, FROM_BEGINNING );
    bool bReadSuccessful = SFileReadFile( m_hsFileHandle, szBuffer, sizeof( szBuffer ), &nLen );
    if ( bReadSuccessful )
    {
        sTraceBinary( szBuffer, nLen );
    }
    return true;
}






























inline bool File::FileImpl::Close()
{
    if ( m_hsFileHandle != NULL )
    {






















        XASSERT( SFileCloseFile( m_hsFileHandle ) );
        m_hsFileHandle = NULL;
    }
    return true;
}




inline bool File::FileImpl::IsAlreadyOpen() const
{
    return m_hsFileHandle != NULL;
}










inline int64 File::FileImpl::GetSNOFileStartingLocation() const
{
    return SFileGetFileStartingLocation( m_hsFileHandle );
}



















































File::File( XLocale eLocale )
    : m_eLocale( eLocale )
{
    m_Impl = new FileImpl( eLocale );
}


File::~File()
{
    delete m_Impl;
}







bool File::OpenSNO( const SNOName & tSNOName )
{
    return m_Impl->OpenSNO( tSNOName, m_eLocale );
}


bool File::OpenChildSNO( const SNOName & tSNOName, const char * szChildFile )
{



    return m_Impl->OpenChildSNO( tSNOName, szChildFile, m_eLocale );
}
bool File::Open( const char * szFilename )
{
    return m_Impl->Open( szFilename, m_eLocale );
}


bool File::Open( const char * szDiskFilename, const char * szNonDiskFilename )
{



    return m_Impl->Open( szDiskFilename, szNonDiskFilename, m_eLocale );
}
bool File::Open( const FileIO::File & tFile )
{
    return m_Impl->Open( tFile );
}



bool File::OpenConsoleFile( HSARCHIVE ptArchive, uint64 dwHash, Console::IO::FileInfo * ptFileInfo )
{
    return m_Impl->OpenConsoleFile( ptArchive, dwHash, ptFileInfo );
}





bool File::Close()
{
    return m_Impl->Close();
}

int64 File::GetFileSize() const
{
    return m_Impl->GetFileSize();
}

int64 File::GetCompressedFileSize() const
{
    return m_Impl->GetCompressedFileSize();
}






bool File::GetFileName( char * szBuffer, uint32 nBufferChar ) const
{
    return m_Impl->GetFileName( szBuffer, nBufferChar );
}



HSFILE File::GetHSFILE() const
{
    return m_Impl->m_hsFileHandle;
}






uint64 File::SetFileCursor( int64 dwDistanceToMove, FileIO::FilePointerMovementMethod eMethod )
{
    return m_Impl->SetFileCursor( dwDistanceToMove, eMethod );
}


















bool File::ReadFile( void * buffer, uint64 dwBytesToRead, uint64 * pdwBytesRead )
{
    return m_Impl->ReadFile( buffer, dwBytesToRead, pdwBytesRead );
}



















bool File::TraceFile()
{
    return m_Impl->TraceFile();
}

bool File::IsAlreadyOpen() const
{
    return m_Impl->IsAlreadyOpen();
}

int64 File::GetSNOFileStartingLocation() const
{
    return m_Impl->GetSNOFileStartingLocation();
}






















bool FileExists( const char * szFileName, XLocale eLocale )
{
    return SFileFileExists( 0, szFileName, 0, NULL ) != SFILEEXIST_FAIL;
}




















































































































































































































int32 GetLastErrorCode()
{
    return SFileGetLastError();
}

}  // namespace FileIO
