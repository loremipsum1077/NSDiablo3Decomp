#include "X/Sigma/ErrorManager/ErrorManager.h"
#include "../../../BlizzardCore/Include/Blizzard/Hardware.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Files/XFiles.h"
#include "../Memory/SigmaMemory.h"
#include "../Sigma.h"
#include "../buildver.h"
#include "ErrorManagerLogging.h"
#include "ErrorManagerPlatform.h"
#include <new>
#include <stdio.h>
#include <string.h>
























static LPCSTR sGetErrorString(const ErrorCode eErrorCode)

{
    switch (eErrorCode)
    {
    case ERROR_NONE: return "No Error";
    case ERROR_INTERNAL_FATAL:
    case ERROR_INTERNAL_FATAL_PT: return "Assertion";
    case ERROR_INTERNAL_NONFATAL: return "Warning";
    case ERROR_UNKNOWN: return "Fatal Error";
    case ERROR_UNKNOWN_NONFATAL: return "Unknown Nonfatal Error";
    case ERROR_OUT_OF_MEMORY: return "Out of Memory Error";
    case ERROR_FILE_CREATE: return "File Create Error";
    case ERROR_FILE_OPEN: return "File Open Error";
    case ERROR_FILE_READ: return "File Read Error";
    case ERROR_FILE_WRITE: return "File Write Error";
    case ERROR_FILE_COPY: return "File Copy Error";
    case ERROR_FILE_CORRUPTED: return "File Corrupt";
    case ERROR_DATA_MISSING: return "Data Not Found";
    default: return "<Unknown>";
    }
}






























































void ErrorManagerInitialize(
    LPCSTR szApplicationName,
    uint32 dwProjectID)
{
    CHAR szSystemString[512];

    SigmaGlobals & tSigmaGlobals = gtSigmaGlobals;
    tSigmaGlobals.ptEMGlobals = new (SigmaMemoryNew(sizeof(ErrorManagerGlobals), 4, NULL, TRUE)) ErrorManagerGlobals();
    ErrorManagerGlobals & tEMGlobals = *tSigmaGlobals.ptEMGlobals;

    SigmaMemoryClear(&tEMGlobals.tSystemInfo, sizeof(tEMGlobals.tSystemInfo));

    ErrorManagerLoggingInitialize();

    EMPInitialize();
    EMPInitializeSystemInfo();

    Blizzard::String::Copy(tEMGlobals.szApplicationName, szApplicationName, sizeof(tEMGlobals.szApplicationName));
    tEMGlobals.dwInspectorProjectID = dwProjectID;
    tEMGlobals.eErrorManagerBlizzardErrorCrashMode = EMBEM_SUMMARY_NO_CANCEL;
    tEMGlobals.eDumpType = EMDT_TINY;

    tEMGlobals.szHeroFile[0] = 0;


    EMPExceptionHandlerInit();

    switch (tEMGlobals.tSystemInfo.eOSType)
    {
    case OSTYPE_WINDOWS95:
        strcpy(szSystemString, "Windows 95");
        break;

    case OSTYPE_WINDOWS98:
        strcpy(szSystemString, "Windows 98");
        break;

    case OSTYPE_WINDOWSME:
        strcpy(szSystemString, "Windows ME");
        break;

    case OSTYPE_WINDOWSNT:
        strcpy(szSystemString, "Windows NT");
        break;

    case OSTYPE_WINDOWS2000:
        strcpy(szSystemString, "Windows 2000");
        break;

    case OSTYPE_WINDOWSXP:
        strcpy(szSystemString, "Windows XP");
        break;

    case OSTYPE_WINDOWSSERVER2003:
        strcpy(szSystemString, "Windows Server 2003");
        break;

    case OSTYPE_WINDOWSVISTA:
        strcpy(szSystemString, "Windows Vista");
        break;

    case OSTYPE_WINDOWSSERVER2008:
        strcpy(szSystemString, "Windows Server 2008");
        break;

    case OSTYPE_WINDOWS7:
        strcpy(szSystemString, "Windows 7");
        break;

    case OSTYPE_WINDOWSSERVER2008R2:
        strcpy(szSystemString, "Windows Server2008 R2");
        break;

    case OSTYPE_WINDOWS8:
        strcpy(szSystemString, "Windows 8");
        break;

    case OSTYPE_WINDOWSSERVER2012:
        strcpy(szSystemString, "Windows Server2012");
        break;

    case OSTYPE_WINDOWS8_1:
        strcpy(szSystemString, "Windows 8.1");
        break;

    case OSTYPE_WINDOWSSERVER2012R2:
        strcpy(szSystemString, "Windows Server2012 R2");
        break;

    case OSTYPE_WINDOWS10:
        strcpy(szSystemString, "Windows 10");
        break;

    case OSTYPE_WINDOWS10SERVER:
        strcpy(szSystemString, "Windows 10 Server");
        break;

    case OSTYPE_MACOSX:
        strcpy(szSystemString, "Mac OS X");
        break;













    case OSTYPE_LINUX:
        strcpy(szSystemString, "Linux");
        break;


    default:
        strcpy(szSystemString, "Unknown");
        break;
    }


    if (tEMGlobals.tSystemInfo.eOSType != OSTYPE_UNKNOWN && tEMGlobals.tSystemInfo.fOSIs64Bit)
    {
        strcat(szSystemString, "-64");
    }


    if (tEMGlobals.tSystemInfo.uServicePack)
    {
        CHAR temp[32];
        Blizzard::String::Format(temp, sizeof(temp), " SP%d", tEMGlobals.tSystemInfo.uServicePack);
        strcat(szSystemString, temp);
    }





    CHAR szDebugRelease[8] = "Release";





    CHAR szAssertsEnabled[14] = "No Assertions";





    Blizzard::String::Format(tEMGlobals.aszTemporary, sizeof(tEMGlobals.aszTemporary),
                             "%s %s (%s) running under %s (Version %d.%d.%d)\n", tEMGlobals.szApplicationName,
                             szDebugRelease, szAssertsEnabled, szSystemString, tEMGlobals.tSystemInfo.uMajorVersion,
                             tEMGlobals.tSystemInfo.uMinorVersion, tEMGlobals.tSystemInfo.uBuild);





    Blizzard::String::Format(tEMGlobals.aszTemporary, sizeof(tEMGlobals.aszTemporary), "%s (Version %d.%d.%d)", szSystemString, tEMGlobals.tSystemInfo.uMajorVersion, tEMGlobals.tSystemInfo.uMinorVersion, tEMGlobals.tSystemInfo.uBuild);

    ErrorManagerAddCrashMetadata("Operating_System", tEMGlobals.aszTemporary);






    sprintf(tEMGlobals.aszTemporary, "<BUILD>%s</BUILD>\n", BuildVersionCodeData());
















    Blizzard::Hardware::ProcessorFeatures cpuInfo;
    char cpuBrandString[64];
    char cpuVendorString[32];

    Blizzard::Hardware::ExtendedProcessorFeatures(cpuInfo);
    Blizzard::Hardware::GetProcessorVendorString(cpuVendorString, sizeof(cpuVendorString));
    Blizzard::Hardware::GetProcessorBrandString(cpuBrandString, sizeof(cpuBrandString));


    if (cpuVendorString[0])
        ErrorManagerAddCrashMetadata("CPU_Vendor", cpuVendorString);


    if (cpuBrandString[0])
    { ErrorManagerAddCrashMetadata("CPU_Details", cpuBrandString);
        sprintf(tEMGlobals.aszTemporary, "%s (Model %X, ExtModel %X, Family %X, ExtFamily %X, Stepping %X, Signature %X)", cpuBrandString, cpuInfo.model, cpuInfo.extmodel, cpuInfo.family, cpuInfo.extfamily, cpuInfo.stepping, cpuInfo.signature);
    }


    sprintf(tEMGlobals.aszTemporary, "%d", cpuInfo.packageCount);
    ErrorManagerAddCrashMetadata("CPU_Processors", tEMGlobals.aszTemporary);

    sprintf(tEMGlobals.aszTemporary, "%d", cpuInfo.numCores);
    ErrorManagerAddCrashMetadata("CPU_Cores", tEMGlobals.aszTemporary);

    sprintf(tEMGlobals.aszTemporary, "%d", cpuInfo.numHWThreads);
    ErrorManagerAddCrashMetadata("CPU_Threads", tEMGlobals.aszTemporary);



    float flTotalMegs = tEMGlobals.tSystemInfo.uTotalPhysicalMemory / (1024.0 * 1024.0);
    sprintf(tEMGlobals.aszTemporary, "%.2f MB", flTotalMegs);

    ErrorManagerAddCrashMetadata("System_Memory", tEMGlobals.aszTemporary);
















    CHAR szProcessPath[4096];
    FileGetProcessPath(szProcessPath);
    sprintf(tEMGlobals.aszTemporary, "Application Path: %s\n", szProcessPath);


    SigmaMemoryDumpOSStats();
}










void ErrorManagerSetDisplayErrorPrepCallback(PFNERRORPREPCALLBACK pfnErrorPrepCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnErrorPrepCallback = pfnErrorPrepCallback;
}





void ErrorManagerSetCrashReportCallback(PFNCRASHREPORTCALLBACK pfnCrashReportCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnCrashReportCallback = pfnCrashReportCallback;
}





void ErrorManagerSetEndOfCrashCallback(PFNENDOFCRASHCALLBACK pfnEndOfCrashCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnEndOfCrashCallback = pfnEndOfCrashCallback;
}







void ErrorManagerSetGameIDCallback(PFNGETGAMEID pfnGameIDCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnGameIDCallback = pfnGameIDCallback;
}








void ErrorManagerSetTerminateCallback(PFNTERMINATECALLBACK pfnTerminateCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnTerminateCallback = pfnTerminateCallback;
}






void ErrorManagerSetUITraceCallback(PFNUITRACECALLBACK pfnUITraceCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnUITraceCallback = pfnUITraceCallback;
}







void ErrorManagerSetRemoveAppTextEncodingCallback(PFNREMOVEAPPTEXTENCODINGCALLBACK pfnRemoveAppTextEncodingCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnRemoveAppTextEncodingCallback = pfnRemoveAppTextEncodingCallback;
}





void ErrorManagerSetTestingErrorCallback(PFNTESTINGERRORCALLBACK pfnTestingErrorCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnTestingErrorCallback = pfnTestingErrorCallback;
}


















void ErrorManagerSetCapsDepressedCallback(PFNCAPSDEPRESSEDCALLBACK pfnCapsDepressedCallback)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    tEMGlobals.pfnCapsDepressedCallback = pfnCapsDepressedCallback;
}














void ErrorManagerDispose()
{


    ErrorManagerSetDisplayErrorPrepCallback(NULL);
    ErrorManagerSetCrashReportCallback(NULL);
    ErrorManagerSetEndOfCrashCallback(NULL);
    ErrorManagerSetGameIDCallback(NULL);
    ErrorManagerSetTerminateCallback(NULL);
    ErrorManagerSetUITraceCallback(NULL);
    ErrorManagerSetRemoveAppTextEncodingCallback(NULL);
    ErrorManagerSetTestingErrorCallback(NULL);

    EMPExceptionHandlerDispose();
    EMPDispose();

    SigmaGlobals & tSigmaGlobals = gtSigmaGlobals;
    delete tSigmaGlobals.ptEMGlobals;
    tSigmaGlobals.ptEMGlobals = NULL;
}





void ErrorManagerGetDate(
    LPSTR szDate,
    uint32 nBufferSize)
{
    Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::Timestamp localTime = Blizzard::Time::LocalBias(utcTime);
    Blizzard::Time::TimeRec timeRec;
    Blizzard::Time::BreakTime(localTime, timeRec);

    Blizzard::String::Format(szDate, nBufferSize, "%02d.%02d.%02d %02d:%02d:%02d.%09d",
                             timeRec.year, timeRec.month, timeRec.day, timeRec.hour,
                             timeRec.minute, timeRec.second, timeRec.nanosecond);
}




static BOOL sErrorCodeIsFatal(const ErrorCode eErrorCode)

{
    switch (eErrorCode)
    {
    case ERROR_INTERNAL_NONFATAL:
    case ERROR_UNKNOWN_NONFATAL:
    case ERROR_FILE_OPEN:
    case ERROR_FILE_WRITE_NONFATAL:
    case ERROR_FILE_COPY:
        return FALSE;
    default:
        return TRUE;
    }
}







void DisplayInternalError(LPCSTR pszMessage, LPCSTR szFile, int32 nLine, ErrorCode eErrorCode)
{
    DisplayErrorMessage(eErrorCode, pszMessage);
}


















void ErrorMessageBox(LPCSTR szText,
                     LPCSTR szHeader)
{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    if (!tEMGlobals.fSuppressUserInteraction)
    {

        BOOL fOldErrorState = tEMGlobals.fSigmaCurrentlyHandlingError;
        tEMGlobals.fSigmaCurrentlyHandlingError = TRUE;
        EMPDisplayMessageBox(szHeader, szText);
        tEMGlobals.fSigmaCurrentlyHandlingError = fOldErrorState;
    }
}


























// NOTE: instructions match but the jump table/block order differs from the original (the case blocks of the inlined sGetErrorString are laid out Assertion, Data Not Found, File Write, Fatal, Unknown Nonfatal, File Copy, File Corrupt, Warning, Out of Memory, File Create, File Read, File Open in the original; ours come out Assertion, File Read, Data Not Found, File Create, File Write, Unknown Nonfatal, File Open, Fatal, Out of Memory, File Copy, File Corrupt, Warning. The value-to-string mapping is equal; reordering the cases or rewriting the switch with a local and breaks does not change the layout).
void DisplayErrorMessage(ErrorCode eErrorCode,




                         LPCSTR pszMessage)
{





    BOOL fFatal = sErrorCodeIsFatal(eErrorCode);


    char szMessage2[1024];
    if (pszMessage)
    {
        Blizzard::String::Copy(szMessage2, pszMessage, sizeof(szMessage2));
    }
    else
    {
        strcpy(szMessage2, "<no message>");
    }

    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;


    if (tEMGlobals.fSigmaCurrentlyHandlingError)
    {
        if (fFatal)
        {



            EMPCrashForStack();
        }
        return;
    }

    tEMGlobals.fSigmaCurrentlyHandlingError = TRUE;




















    CHAR szMsgBoxHeader[256];
    Blizzard::String::Format(szMsgBoxHeader, sizeof(szMsgBoxHeader), "%s Error", tEMGlobals.szApplicationName);

    BOOL fErrorShouldBeExtractable = fFatal;

    CHAR szMsgBoxStr[2048];
    CHAR szMsgForFile[2048];
    CHAR szMsgForDebugString[2048];
    LPCSTR szErrorString = sGetErrorString(eErrorCode);

















    Blizzard::String::Format(szMsgForDebugString, sizeof(szMsgForDebugString), "%s\n\n", szMessage2);
    if (fErrorShouldBeExtractable)
    {
        Blizzard::String::Format(szMsgForFile, sizeof(szMsgForFile), "<ERROR-%d>\n%s\n</ERROR-%d>\n\n", tEMGlobals.nCrashNumber, szMessage2, tEMGlobals.nCrashNumber);
    }
    else
    {
        Blizzard::String::Copy(szMsgForFile, szMsgForDebugString, sizeof(szMsgForFile));
    }
    Blizzard::String::Format(szMsgBoxStr, sizeof(szMsgBoxStr), "%s\n\n%s", szErrorString, szMessage2);
















    if (eErrorCode == ERROR_INTERNAL_FATAL)
    {
        if (tEMGlobals.pfnAppDumpCallback)
        {
            tEMGlobals.pfnAppDumpCallback();
        }
    }


    if (tEMGlobals.pfnErrorPrepCallback)
    {
        tEMGlobals.pfnErrorPrepCallback();
    }







    if (!fFatal)
    {
        tEMGlobals.fSigmaCurrentlyHandlingError = FALSE;
    }
    else
    {








        EMPCrashForStack();



        if (eErrorCode >= EXTERNAL_ERROR_START)
        {

            ExitProgram(eErrorCode);
        }
    }
}





BOOL SigmaCurrentlyHandlingError()
{

    ErrorManagerGlobals * ptEMGlobals = gtSigmaGlobals.ptEMGlobals;
    return ptEMGlobals && ptEMGlobals->fSigmaCurrentlyHandlingError;
}
























void ErrorManagerSuppressUserInteraction(BOOL fSuppressUserInteraction)

{
    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;

    tEMGlobals.fSuppressUserInteraction = fSuppressUserInteraction;
}




BOOL ErrorManagerGetSuppressUserInteraction()
{

    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;

    return tEMGlobals.fSuppressUserInteraction;
}




























































void TraceHesitationStart()
{

    ErrorManagerGlobals & tEMGlobals = *gtSigmaGlobals.ptEMGlobals;

    if (tEMGlobals.dwTraceHesitationThreshhold)
    {



        tEMGlobals.tTraceHesitationStopwatch.Reset();
        tEMGlobals.flLastTraceTime = 0.0f;
    }
}







































BOOL TraceHesitationEnd()
{
















































    return FALSE;
}
















void ErrorManagerAddCrashMetadata(const char * pszKey,
                                  const char * pszValue)
{

    ErrorManagerGlobals * ptEMGlobals = gtSigmaGlobals.ptEMGlobals;
    if (ptEMGlobals)
    {
        ptEMGlobals->tErrorManagerBlizzardErrorData.AddMetadata(pszKey, pszValue);
    }
}


























































































char s_szSetReportingFolder[128];














const char * ErrorManagerGetReportingFolder()
{
    return s_szSetReportingFolder;
}

























































































ErrorManagerGetGameInfoCb s_fnGetGameInfo;
