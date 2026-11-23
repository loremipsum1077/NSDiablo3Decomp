// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmSettings.h

#pragma once

#include "dmSerialize.h"

#include <stddef.h>

typedef void * dmAllocHook(dm_int32, dm_int32, const char *, dm_int32, const char *);

typedef void dmFreeHook(void *);

typedef void dmAssertHook(const char *, const char *, dm_int32, void *, void *);

typedef void dmWarningHook(const char *);

typedef void dmLogHook(const char *);

// Defined in Common/dmSettings.cpp. The DWARF does not say which header
// declared them; this one holds the hook types they use.
void * dmAllocFunction(dm_int32 size, dm_int32 alignment, const char * pFileName, dm_int32 lineNumber, const char * pFunctionName);
void dmFree(void * pMemory);
void dmAssertFunction(const char * pMessage, const char * pFileName, dm_int32 lineNumber, void * pUserA, void * pUserB);

extern bool dm_assertOnce;

// The DWARF keeps no macros; these two names are invented. Every allocation in
// the 2.6.2 binary passes 16 for the alignment and the caller's file, line and
// function name. Every assert tests the condition before dm_assertOnce, and
// its failure call is followed by a trap word (dmAssertFunction itself
// returns).
#define dmAlloc(size) dmAllocFunction((dm_int32)(size), 16, __FILE__, __LINE__, __FUNCTION__)
#define dmAssert(condition) \
    if (!(condition) && dm_assertOnce == false) \
    { \
        dmAssertFunction(#condition, __FILE__, __LINE__, NULL, NULL); \
        __builtin_trap(); \
    }

// Also defined in Common/dmSettings.cpp.
void dmSetAllocHook(dmAllocHook * pAllocHook, dmFreeHook * pFreeHook);
void dmSetAssertHook(dmAssertHook * pHook);
void dmSetWarningHook(dmWarningHook * pHook);
void dmSetLogHook(dmLogHook * pHook);
void dmWarn(const char * pFormat, ...);
void dmLog(const char * pFormat, ...);
