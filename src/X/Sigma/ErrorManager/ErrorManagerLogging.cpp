// Original file: X/Sigma/ErrorManager/ErrorManagerLogging.cpp
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Using.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../DataStructures/XVar.h"
#include "../Memory/SigmaMemory.h"
#include "../Sigma.h"
#include "../SigmaTiming.h"
#include "ErrorManagerPlatform.h"
#include "FileOutputStream.h"
#include <new>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// BlizzardCore/Source/System/Process/Process.cpp:27.
namespace Blizzard { namespace Process { bool GetProcessDirectory(char * buf, int bufMax); } }

FileOutputStream * gptOutputStreams[NUM_OUTPUTSTREAMS];
static BOOL sbInitialized = FALSE;
static BOOL sbAppendTimestampToLogFilename = FALSE;
static char szLogPath[4096];

static XVarScope s_scopeErrorManager("ErrorManager", NULL);
static XVarScope s_scopeLogging(s_scopeErrorManager, "Logging");
XVarBool s_varLocalLoggingEnable(s_scopeLogging, "Local", true);


static SeverityLevel seDefaultLogSeverityLevel = SLVL_INFO;




typedef blz::vector<PFNLOGSINKCALLBACK> LogSinkArray;
LogSinkArray s_logSinks;

static void sLoggingDispose();













static FileOutputStream * sGetStream(OutputStreamType eStream)
{


    if (!gptOutputStreams[eStream])
    {



        FileOutputStream * ptStream = new (malloc(sizeof(FileOutputStream))) FileOutputStream;
        gptOutputStreams[eStream] = ptStream;
    }
    return gptOutputStreams[eStream];
}


void ErrorManagerLoggingInitialize()
{

    Blizzard::Using::Register("ErrorManager::Logging", sLoggingDispose);
    Blizzard::Using::StartUsing("Blizzard::Time");


    if (Blizzard::String::Length(szLogPath) == 0)
    {
        Blizzard::Process::GetProcessDirectory(szLogPath, sizeof(szLogPath));
    }
    sbInitialized = TRUE;
}



void ErrorManagerLoggingRegisterLogSink(PFNLOGSINKCALLBACK pfnLogSinkCallback)
{
    s_logSinks.push_back(pfnLogSinkCallback);
}




void ErrorManagerLoggingUnregisterLogSink(PFNLOGSINKCALLBACK pfnLogSinkCallback)

{
    LogSinkArray::iterator found = blz::find(s_logSinks.begin(), s_logSinks.end(), pfnLogSinkCallback);

    if (found != s_logSinks.end())
    {
        s_logSinks.erase(found);
    }
}



void ErrorManagerEnableLocalLogging(BOOL bEnabled)

{
    s_varLocalLoggingEnable.Set(bEnabled != FALSE, 3);
}



BOOL ErrorManagerIsLocalLoggingEnabled()
{
    return s_varLocalLoggingEnable.Get();
}




void ErrorManagerAppendTimestampToLogFilename(BOOL bEnabled)

{
    sbAppendTimestampToLogFilename = bEnabled;
}



void ErrorManagerSetDefaultStreamSeverityLevel(SeverityLevel eLevel)

{
    seDefaultLogSeverityLevel = eLevel;
}













SeverityLevel ErrorManagerGetDefaultStreamSeverityLevel()
{
    return seDefaultLogSeverityLevel;
}




SeverityLevel ErrorManagerGetStreamSeverityLevel(OutputStreamType eStream)

{
    FileOutputStream * ptStream = sGetStream(eStream);
    return ptStream->GetSeverityLevel() != SLVL_INTERNAL ? ptStream->GetSeverityLevel() : seDefaultLogSeverityLevel;
}



void ErrorManagerSetStreamSeverityLevel(OutputStreamType eStream,
                                        SeverityLevel eLevel)
{













    FileOutputStream * ptStream = sGetStream(eStream);
    ptStream->SetSeverityLevel(eLevel);
}







static void sErrorManagerAppendTimestampToLogFilename(char * pszFilename, uint32 uFilenameBufferLength)

{
    char szTempFilename[4096];
    char szExtension[32] = { 0 };
    char * pszExtension = Blizzard::String::FindExtensionWithLeadingDot(pszFilename);

    if (pszExtension)
    {
        Blizzard::String::Copy(szExtension, pszExtension, sizeof(szExtension));
        *pszExtension = 0;
    }

    Blizzard::Time::Timestamp tStartTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tStartTime, tTimeRecord);
    Blizzard::String::Format(szTempFilename, sizeof(szTempFilename), "%s_%d-%02d-%02d_%02d.%02d.%02d%s", pszFilename,
                             tTimeRecord.year, tTimeRecord.month, tTimeRecord.day, tTimeRecord.hour,
                             tTimeRecord.minute, tTimeRecord.second, szExtension);
    Blizzard::String::Copy(pszFilename, szTempFilename, uFilenameBufferLength);
}



static void sLoggingDispose()
{
    sbInitialized = FALSE;

    for (int i = 0; i < NUM_OUTPUTSTREAMS; i++)
    {
        if (gptOutputStreams[i])
        {
            gptOutputStreams[i]->~FileOutputStream();
            free(gptOutputStreams[i]);
            gptOutputStreams[i] = NULL;
        }
    }
    Blizzard::Using::StopUsing("Blizzard::Time");
}




static BOOL sInitializeOutputStream(const OutputStreamType eStream)

{
    char szFilename[128];
    FileOutputStream * ptStream = sGetStream(eStream);
    Blizzard::String::Copy(szFilename, ptStream->GetFileName(), sizeof(szFilename));

    if (Blizzard::String::Length(szFilename) == 0)
    {

        switch (eStream)
        {
        case OUTPUTSTREAM_DEFAULT:
            Blizzard::String::Copy(szFilename, "Debug.txt", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_SYNC:
            Blizzard::String::Copy(szFilename, "Sync.txt", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_STREAMING:
            Blizzard::String::Copy(szFilename, "Streaming.log", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_BOOT:
            Blizzard::String::Copy(szFilename, "Boot.txt", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_BATTLENET:
            Blizzard::String::Copy(szFilename, "Battlenet.log", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_LOCKS:
            Blizzard::String::Copy(szFilename, "locks.log", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_LEADERBOARD:
            Blizzard::String::Copy(szFilename, "locks.log", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_NOTIFICATIONS:
            Blizzard::String::Copy(szFilename, "Notifications.log", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_RIFTSTATS:
            Blizzard::String::Copy(szFilename, "RiftStats.log", sizeof(szFilename));
            break;

        case OUTPUTSTREAM_TRADE:
            Blizzard::String::Copy(szFilename, "Trade.log", sizeof(szFilename));
            break;
        }
    }








    if (sbAppendTimestampToLogFilename)
    {
        sErrorManagerAppendTimestampToLogFilename(szFilename, sizeof(szFilename));
    }
    ptStream->SetFileName(szFilename);


    ptStream->Initialize(szLogPath);
    return TRUE;
}





void ErrorManagerSetLogStreamDirectory(LPCSTR pszDirectory)

{
    Blizzard::String::Copy(szLogPath, pszDirectory, sizeof(szLogPath));
}




void ErrorManagerGetLogStreamDirectory(char * pszOutDirectory,
                                       uint32 outDirectorySize)
{
    Blizzard::String::Copy(pszOutDirectory, szLogPath, outDirectorySize);
}





void ErrorManagerSetLogStreamFilename(const OutputStreamType eStream,
                                      LPCSTR pszFileName)
{
    FileOutputStream * ptStream = sGetStream(eStream);
    ptStream->SetFileName(pszFileName);
}




































const char * ErrorManagerGetLogStreamFilePath(OutputStreamType eStream)

{
    FileOutputStream * ptStream = sGetStream(eStream);
    return ptStream->GetFilePath();
}



void ErrorManagerLogToStream(




    const SeverityLevel eLevel,
    const OutputStreamType eStream,
    LPCSTR szMessage,
    BOOL fUseRingBuffer)
{
    if (!sbInitialized)
    {
        return;
    }

    FileOutputStream * ptStream = sGetStream(eStream);
    bool bLocalLoggingEnabled = s_varLocalLoggingEnable.Get();
    if (!ptStream->IsInitialized() && bLocalLoggingEnabled)
    {
        sInitializeOutputStream(eStream);
    }


    BOOL fAddTimestamp = (eStream == OUTPUTSTREAM_DEFAULT) && (eLevel != SLVL_CRASH_INFO);





















    CHAR szPrependedLabel[4];
    szPrependedLabel[0] = 0;



    if (bLocalLoggingEnabled)
    {
        if (fUseRingBuffer)
        {
            ptStream->LogToRingBuffer(fAddTimestamp, szMessage, szPrependedLabel);
        }
        else
        {
            ptStream->Log(fAddTimestamp, szMessage, szPrependedLabel);
        }
    }
}


































static void sPrependLabel(LPSTR buf,
                          LPCSTR szErrorMsg)
{
    size_t nErrMsgSize = strlen(szErrorMsg);
    SigmaMemoryMove(buf + nErrMsgSize, buf, 1024 - nErrMsgSize);
    memcpy(buf, szErrorMsg, nErrMsgSize);
    buf[1023] = 0;
}





static void sStringFromVarArgs(LPSTR pszBuf,
                               const uint32 dwBufferSize,
                               LPCSTR cpFmtStr,
                               va_list tArgList)
{









    vsnprintf(pszBuf, dwBufferSize, cpFmtStr, tArgList);







    pszBuf[dwBufferSize - 1] = 0;
}




static BOOL sTraceMsgIsIgnored(const uint32 dwFlags)

{
    ErrorManagerGlobals * ptEMGlobals = gtSigmaGlobals.ptEMGlobals;

    if ((dwFlags & LF_CAPSLOCK_ONLY) && ptEMGlobals)
    {
        if (ptEMGlobals->pfnCapsDepressedCallback &&
            !ptEMGlobals->pfnCapsDepressedCallback())
        {
            return TRUE;
        }
    }





    if (dwFlags & LF_LIMIT_OUTPUT)
    {
        static uint32 dwLastErrorTime = 0;
        static uint32 dwErrorCount = 0;
        uint32 dwTime = GetMachineTicks();
        if (dwTime <= dwLastErrorTime + 500)
        {
            dwErrorCount++;
            if (dwErrorCount > 5)
            {
                return TRUE;
            }
        }
        else
        {


            dwErrorCount = 0; dwLastErrorTime = dwTime;
        }
    }
    return FALSE;
}












BOOL TraceInternal_ShouldLog(const SeverityLevel eLevel,
                             const uint32 dwFlags,
                             const OutputStreamType eStream)
{
    if (sTraceMsgIsIgnored(dwFlags))
    {
        return FALSE;
    }



    return eLevel >= ErrorManagerGetStreamSeverityLevel(eStream);
}






void TraceInternal_Log(




    const SeverityLevel eLevel,
    const uint32 dwFlags,
    const OutputStreamType eStream,
    LPCSTR szFormat,
    ...)
{
    CHAR szMsg[1024]; va_list tArgList; va_start(tArgList, szFormat); sStringFromVarArgs(szMsg, sizeof(szMsg), szFormat, tArgList);

    ErrorManagerGlobals * ptEMGlobals = gtSigmaGlobals.ptEMGlobals;
    uint32 dwFlagsToUse = dwFlags;
    if (ptEMGlobals)
    {
        if (ptEMGlobals->fSuppressTracingToScreen)
        {
            dwFlagsToUse &= ~LF_ECHO_TO_SCREEN;
        }

        if (ptEMGlobals->pfnTestingErrorCallback)
        {
            dwFlagsToUse &= ~LF_ECHO_TO_SCREEN_AND_CONSOLE;
        }
    }
    LPCSTR cszDataError = "ERROR: ";
    LPCSTR cszDataWarning = "WARNING: ";

    if (eLevel >= SLVL_ERROR)
    {
        sPrependLabel(szMsg, cszDataError);
    }
    else if (eLevel == SLVL_WARNING)
    {
        sPrependLabel(szMsg, cszDataWarning);
    }
    char * bufpretty = szMsg;













    if (ptEMGlobals)
    {
        if (eLevel >= SLVL_ERROR && ptEMGlobals->pfnTestingErrorCallback && eStream == OUTPUTSTREAM_DEFAULT)
        {
                size_t nOffset = strlen(cszDataError);
                ptEMGlobals->pfnTestingErrorCallback(bufpretty + nOffset);
        }
    }








    if (dwFlagsToUse & LF_ECHO_TO_STREAM)
    {
        ErrorManagerLogToStream(eLevel, eStream, szMsg, FALSE);
    }








    if (dwFlagsToUse & LF_ECHO_TO_RING_BUFFER)
    {
        ErrorManagerLogToStream(eLevel, eStream, szMsg, TRUE);
    }









    if (ptEMGlobals && ptEMGlobals->pfnUITraceCallback)
    {
        ptEMGlobals->pfnUITraceCallback(szMsg, dwFlagsToUse);
    }
}

struct TraceBootData {  // size 0x10
    TraceBootData() {}
    Stopwatch tStopwatch;  // 0x00
    float flLastTraceBootTime;  // 0x08
};







BOOL gbTraceBooting = TRUE;


BOOL gbTraceBootInitialized = FALSE;
TraceBootData gtTraceBootData;































































void TraceBootStop()
{

    gbTraceBooting = FALSE;
}
