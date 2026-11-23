#include "../Include/dmSettings.h"

#include <stdarg.h>
#include <stdio.h>
























static dmAllocHook * s_pAllocHook = NULL;
static dmFreeHook * s_pFreeHook = NULL;
static dmAssertHook * s_pAssertHook = NULL;
static dmWarningHook * s_pWarningHook = NULL;
static dmLogHook * s_pLogHook = NULL;

bool dm_assertOnce = false;


void dmSetAllocHook(dmAllocHook * pAllocHook, dmFreeHook * pFreeHook)
{
    if (s_pAllocHook == NULL && pAllocHook != NULL && pFreeHook != NULL)
    {
        s_pAllocHook = pAllocHook;
        s_pFreeHook = pFreeHook;
    }
}


void * dmAllocFunction(dm_int32 size, dm_int32 alignment, const char * pFileName, dm_int32 lineNumber, const char * pFunctionName)
{

    if (s_pAllocHook)
    {
        return s_pAllocHook(size, alignment, pFileName, lineNumber, pFunctionName);
    }

















    return NULL;
}


void dmFree(void * pMemory)
{

    if (s_pFreeHook)
    {
        s_pFreeHook(pMemory);
        return;
    }















}


void dmSetAssertHook(dmAssertHook * pHook)
{
    s_pAssertHook = pHook;
}


void dmSetWarningHook(dmWarningHook * pHook)
{
    s_pWarningHook = pHook;
}


void dmSetLogHook(dmLogHook * pHook)
{
    s_pLogHook = pHook;
}


void dmAssertFunction(const char * pMessage, const char * pFileName, dm_int32 lineNumber, void * pUserA, void * pUserB)
{
    dm_assertOnce = true;

    if (s_pAssertHook)
    {
        s_pAssertHook(pMessage, pFileName, lineNumber, pUserA, pUserB);
    }
    else
    {
        printf("Domino Assertion: %s, in file %s, line %d\n", pMessage, pFileName, lineNumber);
    }
}


void dmWarn(const char * pFormat, ...)
{
    static bool warned = false;



    if (warned)
    {
        return;
    }







    char buffer[256];
    va_list arg;
    va_start(arg, pFormat);



    vsnprintf(buffer, 256, pFormat, arg);
    va_end(arg);



    buffer[255] = 0;

    if (s_pWarningHook)
    {
        s_pWarningHook(buffer);
    }
    else
    {






        printf("%s", buffer);
    }
    warned = true;



}


void dmLog(const char * pFormat, ...)
{



    char buffer[256];
    va_list arg;
    va_start(arg, pFormat);



    vsnprintf(buffer, 256, pFormat, arg);
    va_end(arg);



    buffer[255] = 0;

    if (s_pLogHook)
    {
        s_pLogHook(buffer);
    }
    else
    {



        printf("%s", buffer);
    }

}
