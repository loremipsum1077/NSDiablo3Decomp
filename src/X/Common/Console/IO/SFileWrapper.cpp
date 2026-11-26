// Original file: X/Common/Console/IO/SFileWrapper.cpp

#include "../../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../../BlizzardCore/Include/Blizzard/Mopaq.h"
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleIO.h"
#include "ConsoleIO_Archive.h"
#include "ConsoleIO_ArchiveReader.h"
#include "../../ThreadGlobals.h"

#include <string.h>

class HSFILERecord {  // size 0x1038
public:
    char szFilename[4096];  // 0x00
    uint64 dwHash;  // 0x1000
    int32 nFilePointer;  // 0x1008
    Console::IO::FileRecord * ptRecord;  // 0x1010
    Console::IO::FileInfo info;  // 0x1018
    Blizzard::File::Stream tOnDiskStream;  // 0x1030
};
static_assert(sizeof(HSFILERecord) == 0x1038, "HSFILERecord");







static Console::IO::Results snLastResult;
static char sszBasePath[4096];



bool SFileCloseFile(HSFILERecord * pFile)
{








    if (pFile->ptRecord != NULL)
    {
        pFile->ptRecord->ptArchive->CloseFileRecord(pFile->ptRecord);
    }


    delete pFile;

    return true;
}


bool SFileGetArchiveName(HSARCHIVE OpaqueArchive, char * pNameBuffer, unsigned int dwNameBufferSize)
{


    Console::IO::CPKArchiveReader * ptArchive = (Console::IO::CPKArchiveReader *)OpaqueArchive;

    const char * szArchiveName = ptArchive->GetArchiveName();
    if (dwNameBufferSize > strlen(szArchiveName))
    {

        strncpy(pNameBuffer, szArchiveName, dwNameBufferSize);
        return true;
    }
    return false;
}


bool SFileGetFileName(HSFILERecord * pFile, char * pNameBuffer, unsigned int dwNameBufferSize)
{


    if (pFile->dwHash != 0)
    {
        return Console::IO::GetFilename(pFile->dwHash, pNameBuffer, dwNameBufferSize) == TRUE;
    }
    else
    {
        const char * szFilename = pFile->szFilename;

        if (dwNameBufferSize > strlen(szFilename))
        {

            strncpy(pNameBuffer, szFilename, dwNameBufferSize);
            return true;
        }
        return false;
    }
}



























































Blizzard::Mopaq::ErrorCode SFileGetLastError()
{
    return Blizzard::Mopaq::SFILE_NOERR;
}
















bool SFileReadFilePriority(HSFILERecord * pFile, void * pBuffer, unsigned int dwBytesToRead, unsigned int * pdwBytesRead, Console::IO::TaskPriorities ePriority);




bool SFileReadFile(HSFILERecord * pFile, void * pBuffer, unsigned int dwBytesToRead, unsigned int * pdwBytesRead)
{




    ThreadGlobals * ptThreadGlobals = ThreadGlobals::TryToGet();
    BOOL fHighestPriority = ptThreadGlobals == NULL || ptThreadGlobals->m_nThreadIndex != THREADINDEX_ASYNCLOAD;
    return SFileReadFilePriority(pFile, pBuffer, dwBytesToRead, pdwBytesRead, fHighestPriority ? Console::IO::TASKPRIORITY_HIGHEST : Console::IO::TASKPRIORITY_HIGH);
}


bool SFileReadFilePriority(HSFILERecord * pFile, void * pBuffer, unsigned int dwBytesToRead, unsigned int * pdwBytesRead, Console::IO::TaskPriorities ePriority)
{





    int32 nBytesRead = dwBytesToRead;


















    if (Console::IO::ReadRecord(pFile->ptRecord, pBuffer, pFile->nFilePointer, dwBytesToRead, &nBytesRead, ePriority) != Console::IO::IORESULT_SUCCESS)
    {
        return false;
    }



    pFile->nFilePointer += nBytesRead;

    if (pdwBytesRead != NULL)
    {
        *pdwBytesRead = nBytesRead;
    }
    return true;
}


uint64 SFileSetFilePointer(HSFILERecord * pFile, int64 nOffset, unsigned int dwMoveMethod)
{



    switch (dwMoveMethod)
    {
    case 0:
        pFile->nFilePointer = nOffset;
        break;
    case 1:
        pFile->nFilePointer += nOffset;
        break;
    case 2:
        pFile->nFilePointer = pFile->info.nSize - nOffset;
        break;
    }
    return pFile->nFilePointer;
}



Blizzard::Mopaq::OpenFileResult SFileOpenFileEx(HSARCHIVE OpaqueArchive, uint64 dwHash, HSFILERecord ** ppFile, Console::IO::FileInfo * ptFileInfo)
{




    Console::IO::CPKArchiveReader * ptArchive = (Console::IO::CPKArchiveReader *)OpaqueArchive;
    if (ptArchive == NULL)
    {
        ptArchive = Console::IO::FindArchiveContaining(dwHash, NULL);
    }
    if (ptArchive == NULL)
    {
        return Blizzard::Mopaq::SFILEOPEN_FAIL;
    }
    Console::IO::FileRecord * ptRecord;
    Console::IO::FileInfo info;
    if (ptFileInfo != NULL)
        info = *ptFileInfo;
    ptRecord = NULL;
    if (!ptArchive->OpenFileRecord(dwHash, &ptRecord, &info))
    {
        return Blizzard::Mopaq::SFILEOPEN_FAIL;
    }
    HSFILERecord * pRef = new HSFILERecord;
    pRef->szFilename[0] = 0;
    pRef->dwHash = dwHash;
    pRef->nFilePointer = 0;
    pRef->ptRecord = ptRecord;
    pRef->info = info;
    *ppFile = pRef;
    return Blizzard::Mopaq::SFILEOPEN_MPQ;
}









Blizzard::Mopaq::OpenFileResult SFileOpenFileEx(HSARCHIVE OpaqueArchive, const char * szFilename, unsigned int nFlags, HSFILERecord ** ppFile)
{





    Console::IO::CPKArchiveReader * ptArchive = (Console::IO::CPKArchiveReader *)OpaqueArchive;
    if (ptArchive == NULL)
    {
        ptArchive = Console::IO::FindArchiveContaining(szFilename, NULL);
    }

    if (ptArchive == NULL)
    {
        return Blizzard::Mopaq::SFILEOPEN_FAIL;
    }












































    Console::IO::FileRecord * ptRecord = NULL;
    if (!ptArchive->OpenFileRecord(szFilename, &ptRecord, NULL))
    {
        return Blizzard::Mopaq::SFILEOPEN_FAIL;
    }

    HSFILERecord * pRef = new HSFILERecord;
    Blizzard::String::Copy(pRef->szFilename, szFilename, sizeof(pRef->szFilename));
    pRef->dwHash = 0;
    pRef->nFilePointer = 0;



    pRef->ptRecord = ptRecord;
    pRef->info = ptRecord->info;
    *ppFile = pRef;
    return Blizzard::Mopaq::SFILEOPEN_MPQ;
}



Blizzard::Mopaq::FileExistsResult SFileFileExists(HSARCHIVE OpaqueArchive, uint64 dwHash, HSARCHIVE * pExistsInArchive)
{



    Console::IO::CPKArchiveReader * ptArchive = (Console::IO::CPKArchiveReader *)OpaqueArchive;

    Blizzard::Mopaq::FileExistsResult result = Blizzard::Mopaq::SFILEEXIST_FAIL;
    if (ptArchive != NULL)
    {
        Console::IO::FileInfo info;
        if (ptArchive->GetFileInfo(dwHash, info)) {
            if (pExistsInArchive)
                *pExistsInArchive = OpaqueArchive;
            result = Blizzard::Mopaq::SFILEEXIST_MPQ;
        } else {
            return Blizzard::Mopaq::SFILEEXIST_FAIL;
        }
    }
    else
    {



        ptArchive = Console::IO::FindArchiveContaining(dwHash, NULL);
        if (ptArchive) {
            if (pExistsInArchive)
                *pExistsInArchive = (HSARCHIVE)ptArchive;
            result = Blizzard::Mopaq::SFILEEXIST_MPQ;
        }
    }
    return result;
}




Blizzard::Mopaq::FileExistsResult SFileFileExists(HSARCHIVE OpaqueArchive, const char * szFilename, unsigned int, HSARCHIVE * pExistsInArchive)
{


    Console::IO::CPKArchiveReader * ptArchive = (Console::IO::CPKArchiveReader *)OpaqueArchive;

    Blizzard::Mopaq::FileExistsResult result = Blizzard::Mopaq::SFILEEXIST_FAIL;
    if (ptArchive != NULL)
    {
        Console::IO::FileInfo info;
        if (ptArchive->GetFileInfo(szFilename, info)) {
            if (pExistsInArchive)
                *pExistsInArchive = OpaqueArchive;
            result = Blizzard::Mopaq::SFILEEXIST_MPQ;
        } else {
            return Blizzard::Mopaq::SFILEEXIST_FAIL;
        }
    }
    else
    {



        ptArchive = Console::IO::FindArchiveContaining(szFilename, NULL);
        if (ptArchive) {
            if (pExistsInArchive)
                *pExistsInArchive = (HSARCHIVE)ptArchive;
            result = Blizzard::Mopaq::SFILEEXIST_MPQ;
        }
    }
    return result;
}




































Blizzard::IOOffset SFileGetFileStartingLocation(HSFILERecord * pFile)
{
    return 0;
}



int64 SFileGetFileSize(HSFILERecord * pFile)
{
    return pFile->info.nSize;
}



int64 SFileGetFileCompressedSize(HSFILERecord * pFile)
{
    return 0;
}




bool SFileOpenArchive(const char * szFilename, int priority, unsigned int, HSARCHIVE * pOpaqueArchive)
{



    Console::IO::CPKArchiveReader * ptArchive = NULL;
    bool bRes = Console::IO::OpenArchive(szFilename, priority, FALSE, FALSE, &ptArchive, Console::IO::TASKPRIORITY_HIGHEST) == Console::IO::IORESULT_SUCCESS;
    *pOpaqueArchive = (HSARCHIVE)ptArchive;
    return bRes;
}



bool SFileCloseArchive(HSARCHIVE OpaqueArchive)
{
    Console::IO::CPKArchiveReader * ptArchiveReader = (Console::IO::CPKArchiveReader *)OpaqueArchive;
    return Console::IO::CloseArchive(ptArchiveReader, Console::IO::TASKPRIORITY_HIGHEST) == Console::IO::IORESULT_SUCCESS;
}



bool SFileGetArchiveFullName(HSARCHIVE OpaqueArchive, char * pNameBuffer, unsigned int dwNameBufferSize)
{




    return SFileGetArchiveName(OpaqueArchive, pNameBuffer, dwNameBufferSize);
}

bool SFileSetBasePath(const char * szBasePath)
{

    Blizzard::String::Copy(sszBasePath, szBasePath, sizeof(sszBasePath));

    uint32 dwLen = strlen(sszBasePath);
    if (sszBasePath[dwLen - 1] != '/' && sszBasePath[dwLen - 1] != '\\')
    {
        strcat(sszBasePath, "/");
    }

    return true;
}

Blizzard::Mopaq::OpenFileResult SFileOpenFile(HSFILERecord * pFile, HSFILERecord ** ppFile)
{


    if (pFile->dwHash != 0)
    {
        return SFileOpenFileEx(pFile->ptRecord != NULL ? (HSARCHIVE)pFile->ptRecord->ptArchive : 0, pFile->dwHash, ppFile, &pFile->info);
    }
    else
    {
        return SFileOpenFileEx(pFile->ptRecord != NULL ? (HSARCHIVE)pFile->ptRecord->ptArchive : 0, pFile->szFilename, 0, ppFile);
    }
}


bool SFileOverrideLocation(HSFILERecord * pFile, int64 nLocationOverride)
{


    if (nLocationOverride == -1 || pFile->ptRecord->info.nLocationCount == 1)
    {
        pFile->ptRecord->info.nLocationIndexOverride = -1;
        return true;
    }

    Console::IO::CPKArchiveReader * ptArchive = pFile->ptRecord->ptArchive;
    int32 nLocationIndex = pFile->info.nLocationIndex;
    int32 nLocationCount = pFile->info.nLocationCount;
    for (int32 nLocation = 0; nLocation < nLocationCount; nLocation++)
    {

        if (ptArchive->GetLocation(nLocationIndex + nLocation) == nLocationOverride)
        {
            pFile->ptRecord->info.nLocationIndexOverride = nLocationIndex + nLocation;
            return true;
        }
    }
    return false;
}

void SFileShutdown()
{
}
