// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/ErrorManager/ErrorManagerPlatform.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "ErrorManager.h"
#include "ErrorManagerInspectorData.h"
#include "../OSInfo.h"
#include "../SigmaTypes.h"
#include "../Stopwatch.h"
#include "../Unicode.h"

struct ErrorManagerGlobals {  // size 0x17430
    PFNERRORPREPCALLBACK pfnErrorPrepCallback;  // 0x00
    PFNCRASHREPORTCALLBACK pfnCrashReportCallback;  // 0x08
    PFNENDOFCRASHCALLBACK pfnEndOfCrashCallback;  // 0x10
    PFNGETGAMEID pfnGameIDCallback;  // 0x18
    PFNTERMINATECALLBACK pfnTerminateCallback;  // 0x20
    PFNUITRACECALLBACK pfnUITraceCallback;  // 0x28
    PFNREMOVEAPPTEXTENCODINGCALLBACK pfnRemoveAppTextEncodingCallback;  // 0x30
    PFNTESTINGERRORCALLBACK pfnTestingErrorCallback;  // 0x38
    PFNAPPDUMPCALLBACK pfnAppDumpCallback;  // 0x40
    PFNCAPSDEPRESSEDCALLBACK pfnCapsDepressedCallback;  // 0x48
    CHAR aszTemporary[1024];  // 0x50
    OSInfo tSystemInfo;  // 0x450
    CHAR szApplicationName[4096];  // 0x678
    CHAR szHeroFile[4096];  // 0x1678
    blz::string szParentProcess;  // 0x2678
    BOOL fSigmaCurrentlyHandlingError;  // 0x26a0
    BOOL fSigmaCurrentlyInExceptionHandler;  // 0x26a4
    BOOL fSuppressUserInteraction;  // 0x26a8
    BOOL fSuppressStackCrawls;  // 0x26ac
    BOOL fSuppressModuleEnumeration;  // 0x26b0
    BOOL fSuppressTracingToScreen;  // 0x26b4
    uint32 dwInspectorProjectID;  // 0x26b8
    uint32 dwTraceHesitationThreshhold;  // 0x26bc
    Stopwatch tTraceHesitationStopwatch;  // 0x26c0
    float flLastTraceTime;  // 0x26c8
    HWND hwnd;  // 0x26cc
    int32 nCrashNumber;  // 0x26d0
    uint32 dwShouldTerminateToken;  // 0x26d4
    ErrorManagerBlizzardErrorMode eErrorManagerBlizzardErrorCrashMode;  // 0x26d8
    ErrorManagerBlizzardErrorData tErrorManagerBlizzardErrorData;  // 0x26e0
    ErrorManagerDumpType eDumpType;  // 0x17428
};
static_assert(sizeof(ErrorManagerGlobals) == 0x17430, "ErrorManagerGlobals");

// X/Sigma/ErrorManager/ErrorManagerNX64.cpp:37-158.
void EMPInitialize();
void EMPDispose();
void EMPDisplayMessageBox(LPCSTR szHeader, LPCSTR szMessage);
void EMPInitializeSystemInfo();
void EMPExceptionHandlerInit();
void EMPExceptionHandlerDispose();
void EMPCrashForStack();
