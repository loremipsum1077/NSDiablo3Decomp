#pragma once

#include "../SigmaTypes.h"
#include "../Unicode.h"

enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_INTERNAL_FATAL = 1,
    ERROR_INTERNAL_NONFATAL = 2,
    ERROR_INTERNAL_FATAL_PT = 64,
    EXTERNAL_ERROR_START = 128,
    ERROR_UNKNOWN = 128,
    ERROR_UNKNOWN_NONFATAL = 129,
    ERROR_OUT_OF_MEMORY = 130,
    ERROR_FILE_CREATE = 131,
    ERROR_FILE_OPEN = 132,
    ERROR_FILE_READ = 133,
    ERROR_FILE_WRITE = 134,
    ERROR_FILE_WRITE_NONFATAL = 135,
    ERROR_FILE_COPY = 136,
    ERROR_FILE_CORRUPTED = 137,
    ERROR_DATA_MISSING = 138,
};

enum OS_Type {
    OSTYPE_UNKNOWN = 0,
    OSTYPE_WINDOWS95 = 1,
    OSTYPE_WINDOWS98 = 2,
    OSTYPE_WINDOWSME = 3,
    OSTYPE_WINDOWSNT = 4,
    OSTYPE_WINDOWS2000 = 5,
    OSTYPE_WINDOWSXP = 6,
    OSTYPE_WINDOWSSERVER2003 = 7,
    OSTYPE_WINDOWSVISTA = 8,
    OSTYPE_WINDOWSSERVER2008 = 9,
    OSTYPE_WINDOWS7 = 10,
    OSTYPE_WINDOWSSERVER2008R2 = 11,
    OSTYPE_WINDOWS8 = 12,
    OSTYPE_WINDOWSSERVER2012 = 13,
    OSTYPE_WINDOWS8_1 = 14,
    OSTYPE_WINDOWSSERVER2012R2 = 15,
    OSTYPE_WINDOWS10 = 16,
    OSTYPE_WINDOWS10SERVER = 17,
    OSTYPE_MACOSX = 18,
    OSTYPE_XBOXONE = 19,
    OSTYPE_LINUX = 20,
    OSTYPE_PS4 = 21,
    OSTYPE_NX64 = 22,
};

enum ErrorManagerDumpType {
    EMDT_TINY = 0,
    EMDT_MEDIUM = 1,
    EMDT_FULL = 2,
};

typedef void (*PFNERRORPREPCALLBACK)();
typedef void (*PFNCRASHREPORTCALLBACK)();
typedef void (*PFNENDOFCRASHCALLBACK)();
typedef BOOL (*PFNGETGAMEID)(uint32&);
typedef void (*PFNTERMINATECALLBACK)(ErrorCode, uint32);
typedef void (*PFNUITRACECALLBACK)(LPCSTR, const uint32);
typedef void (*PFNREMOVEAPPTEXTENCODINGCALLBACK)(LPCSTR, char*, int32);
typedef void (*PFNTESTINGERRORCALLBACK)(LPCSTR);
typedef void (*PFNAPPDUMPCALLBACK)();
typedef BOOL (*PFNCAPSDEPRESSEDCALLBACK)();
typedef char* (*PFNCUSTOMDESCCALLBACK)();
typedef BOOL (*ErrorManagerGetGameInfoCb)(char*, uint32);

void ErrorManagerInitialize(LPCSTR szApplicationName, uint32 dwProjectID);
void ErrorManagerDispose();
void DisplayInternalError(LPCSTR pszMessage, LPCSTR szFile, int32 nLine, ErrorCode eErrorCode);
__attribute__((noreturn)) void ExitProgram(ErrorCode eCode);
void DisplayErrorMessage(ErrorCode eErrorCode, LPCSTR pszMessage);
BOOL SigmaCurrentlyHandlingError();
// ErrorManager.cpp:825, :1024 and the hesitation trace (X/Common/Game.cpp).
void ErrorManagerSuppressUserInteraction(BOOL fSuppressUserInteraction);
void ErrorManagerAddCrashMetadata(const char * pszKey, const char * pszValue);
void TraceHesitationStart();
BOOL TraceHesitationEnd();
// ErrorManager.cpp:531.
void ErrorManagerGetDate(LPSTR szDate, uint32 nBufferSize);
// ErrorManager.cpp:377-592, :836, :1139.
void ErrorManagerSetDisplayErrorPrepCallback(PFNERRORPREPCALLBACK pfnErrorPrepCallback);
void ErrorManagerSetCrashReportCallback(PFNCRASHREPORTCALLBACK pfnCrashReportCallback);
void ErrorManagerSetEndOfCrashCallback(PFNENDOFCRASHCALLBACK pfnEndOfCrashCallback);
void ErrorManagerSetGameIDCallback(PFNGETGAMEID pfnGameIDCallback);
void ErrorManagerSetRemoveAppTextEncodingCallback(PFNREMOVEAPPTEXTENCODINGCALLBACK pfnRemoveAppTextEncodingCallback);
void ErrorManagerSetTestingErrorCallback(PFNTESTINGERRORCALLBACK pfnTestingErrorCallback);
void ErrorManagerSetCapsDepressedCallback(PFNCAPSDEPRESSEDCALLBACK pfnCapsDepressedCallback);
void ErrorMessageBox(LPCSTR szText, LPCSTR szHeader);
BOOL ErrorManagerGetSuppressUserInteraction();
const char * ErrorManagerGetReportingFolder();
