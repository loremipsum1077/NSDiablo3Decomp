// Original file: X/Sigma/ErrorManager/FileOutputStream.cpp

#include "FileOutputStream.h"
#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Files/XFiles.h"
#include "../Memory/SigmaMemory.h"
#include "../StringManip.h"
#include "ErrorManager.h"
#include <stdio.h>
#include <string.h>










FileOutputStream::FileOutputStream()
    : m_bInitialized(FALSE)
{





    m_eSeverityLevel = SLVL_INTERNAL;

    m_szFilename[0] = 0;
    m_szFilePath[0] = 0;
    m_tRingBufferLog.pMemoryRingBuffer = NULL; m_tRingBufferLog.dwCurrentLocation = 0; m_tRingBufferLog.dwRingBufferSize = 0;
}






FileOutputStream::~FileOutputStream()
{
}












































void FileOutputStream::SetFileName(LPCSTR pszFilePath)
{

    Blizzard::String::Copy(m_szFilename, pszFilePath, sizeof(m_szFilename));
}

























void FileOutputStream::Initialize(LPCSTR szLogPath)
{
    static const int32 s_nMaxLogCounter = 8;

















    char szRelativeDirectory[4096];
    Blizzard::String::JoinPath(szRelativeDirectory, sizeof(szRelativeDirectory), szLogPath, m_szFilename);


    Blizzard::File::MakeAbsolutePath(szRelativeDirectory, m_szFilePath, sizeof(m_szFilePath), false);
    {
        CHAR szDrive[4096];
        CHAR szDirectory[4096];
        CHAR szDriveAndDirectory[4096];

        FileSplitPath(m_szFilePath, szDrive, szDirectory, NULL, NULL);


        Blizzard::String::Copy(szDriveAndDirectory, szDrive, sizeof(szDriveAndDirectory));
        strcat_safe(szDriveAndDirectory, sizeof(szDriveAndDirectory), szDirectory);

        FileReference tRef;
        FileReferenceInit(tRef, szDriveAndDirectory);
        FileDirectoryCreate(tRef, TRUE);
    }


    if (Blizzard::File::Exists(m_szFilePath))
    {
        char szPath[128]; char szRenameTo[128]; char szDeleteNext[256]; char * szFilename = Blizzard::String::FindFilename(m_szFilePath);
        char * szExtension = Blizzard::String::FindExtensionWithLeadingDot(m_szFilePath);


        *szExtension = 0;


        size_t nLen = szFilename - m_szFilePath;

        SigmaMemoryCopy(szPath, m_szFilePath, nLen);
        szPath[nLen] = 0;





        const char * szFormatString = "%s_%s-%d.txt";
        uint32 nCounter = 0;
        Blizzard::String::Format(szRenameTo, sizeof(szRenameTo), szFormatString, szPath, szFilename, nCounter);
        nCounter++;
        while (Blizzard::File::Exists(szRenameTo))
        {
            Blizzard::String::Format(szRenameTo, sizeof(szRenameTo), szFormatString, szPath, szFilename, nCounter);
            nCounter++;
        }



        nCounter = nCounter % s_nMaxLogCounter;

        Blizzard::String::Format(szDeleteNext, sizeof(szDeleteNext), szFormatString, szPath, szFilename, nCounter);

        Blizzard::File::Delete(szDeleteNext);

        *szExtension = '.';

        Blizzard::File::Move(m_szFilePath, szRenameTo);
    }
















    FILE * fileDebugFile = fopen_ex(m_szFilePath, "w");
    if (fileDebugFile)
    {
        fclose(fileDebugFile);
    }
    else
    {

        m_szFilePath[0] = 0;
    }


    m_bInitialized = TRUE;
}




void FileOutputStream::Log(BOOL bAddTimestamp, LPCSTR szMessage, LPCSTR szPrependedLabel)

{


    Blizzard::Lock::ScopedMutex scopedMutex(m_criticalSection);

    if (!m_bInitialized || !m_szFilePath[0])
    {
        return;
    }


    FILE * fileDebugFile = fopen_ex(m_szFilePath, "a");
    if (!fileDebugFile)
    {
        return;
    }


    if (bAddTimestamp)
    {
        CHAR szDate[64];
        ErrorManagerGetDate(szDate, sizeof(szDate));






        fprintf(fileDebugFile, "%s\t", szDate);
    }









    fputs(szPrependedLabel, fileDebugFile);
    fputs(szMessage, fileDebugFile);







    fclose(fileDebugFile);
}





void FileOutputStream::LogToRingBuffer(BOOL bAddTimestamp, LPCSTR szMessage, LPCSTR szPrependedLabel)

{

    CHAR szDate[64];
    Blizzard::Lock::ScopedMutex scopedMutex(m_criticalSection);

    if (!m_bInitialized || !m_tRingBufferLog.pMemoryRingBuffer)
    {
        return;
    }


    if (bAddTimestamp)
    {
        ErrorManagerGetDate(szDate, sizeof(szDate));
        strcat(szDate, "\t");
    }
    else
    {
        szDate[0] = 0;
    }

    size_t dwNeededSize = strlen(szDate) + strlen(szPrependedLabel) + strlen(szMessage) + m_tRingBufferLog.dwCurrentLocation + 1;
    if (dwNeededSize >= m_tRingBufferLog.dwRingBufferSize)
    {
        m_tRingBufferLog.pMemoryRingBuffer[m_tRingBufferLog.dwCurrentLocation] = 0;
        m_tRingBufferLog.dwCurrentLocation = 0;
        m_tRingBufferLog.dwCurrentLocation += sprintf(m_tRingBufferLog.pMemoryRingBuffer, "%s\t", szDate);
        m_tRingBufferLog.dwCurrentLocation += sprintf(m_tRingBufferLog.pMemoryRingBuffer + m_tRingBufferLog.dwCurrentLocation, "%s", szPrependedLabel);
        m_tRingBufferLog.dwCurrentLocation += sprintf(m_tRingBufferLog.pMemoryRingBuffer + m_tRingBufferLog.dwCurrentLocation, "%s", szMessage);
        m_tRingBufferLog.pMemoryRingBuffer[m_tRingBufferLog.dwCurrentLocation] = 0;
}
    else
    {
        m_tRingBufferLog.dwCurrentLocation += sprintf(m_tRingBufferLog.pMemoryRingBuffer + m_tRingBufferLog.dwCurrentLocation, "%s\t", szDate);
        m_tRingBufferLog.dwCurrentLocation += sprintf(m_tRingBufferLog.pMemoryRingBuffer + m_tRingBufferLog.dwCurrentLocation, "%s", szPrependedLabel);
        m_tRingBufferLog.dwCurrentLocation += sprintf(m_tRingBufferLog.pMemoryRingBuffer + m_tRingBufferLog.dwCurrentLocation, "%s", szMessage);
        m_tRingBufferLog.pMemoryRingBuffer[m_tRingBufferLog.dwCurrentLocation] = 0;
}
}
