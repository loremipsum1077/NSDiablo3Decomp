#include "ErrorManager.h"
#include "../SigmaTypes.h"

#include "ErrorManagerPlatform.h"
#include "../OSInfo.h"
#include "../Memory/SigmaMemory.h"
#include "../Sigma.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../NintendoSDK/Include/nn/os/os_DebugTypes.h"
#include "../../../NintendoSDK/Include/nn/settings/settings_Language.h"

#include <string.h>

// QueryMemoryInfo is declared in NintendoSDK/Include/nn/os/os_Debug.h and GetDesiredLanguage in
// NintendoSDK/Include/nn/oe/oe_Api.h in the original SDK; neither header is in the DWARF, so the
// minimal prototypes are declared here.
namespace nn {
namespace os {
void QueryMemoryInfo(MemoryInfo* pOutMemoryInfo);
}  // namespace os
namespace oe {
nn::settings::LanguageCode GetDesiredLanguage();
}  // namespace oe
}  // namespace nn

struct ErrorManagerPlatformGlobals {  // size 0x10
    BOOL fUnhandledExceptionCritSecInitialized;  // 0x00
    PFNCUSTOMDESCCALLBACK tCustomDescCallback;  // 0x08
};
static_assert(sizeof(ErrorManagerPlatformGlobals) == 0x10, "ErrorManagerPlatformGlobals");

void EMPInitialize() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;

    tSigmaGlobals.ptEMPGlobals =
        (ErrorManagerPlatformGlobals*)SigmaMemoryNew(sizeof(ErrorManagerPlatformGlobals), 4, NULL, TRUE);
}

void EMPDispose() {
    SigmaGlobals& tSigmaGlobals = gtSigmaGlobals;

    SigmaMemoryFree(tSigmaGlobals.ptEMPGlobals, NULL);
    tSigmaGlobals.ptEMPGlobals = NULL;
}

void EMPDisplayMessageBox(LPCSTR szHeader, LPCSTR szMessage) {
}

// EMPDisplayMessageBoxW(const utf16*, const utf16*) is in the original file (line 66) but has no
// address in the inventory, so it is not written here.

void EMPInitializeSystemInfo() {
    ErrorManagerGlobals& tEMGlobals = *gtSigmaGlobals.ptEMGlobals;

    OSInfo* ptSystemInfo = &tEMGlobals.tSystemInfo;
    ptSystemInfo->eOSType = OSTYPE_NX64;

    nn::os::MemoryInfo memoryInfo;
    nn::os::QueryMemoryInfo(&memoryInfo);
    ptSystemInfo->uTotalPhysicalMemory = memoryInfo.totalAvailableMemorySize;

    strcpy(ptSystemInfo->szOSDescription, "NX64");

    const nn::settings::LanguageCode language = nn::oe::GetDesiredLanguage();
    Blizzard::String::Copy(ptSystemInfo->szOSLanguage, language.string, 0x100);

    ptSystemInfo->fOSIs64Bit = 1;
    ptSystemInfo->fInitialized = 1;
}

void ErrorManagerSetCustomDescriptionCallback(PFNCUSTOMDESCCALLBACK pfnCustomDescCallback) {
    ErrorManagerPlatformGlobals& tEMPGlobals = *gtSigmaGlobals.ptEMPGlobals;

    tEMPGlobals.tCustomDescCallback = pfnCustomDescCallback;
}

void EMPExceptionHandlerInit() {
    ErrorManagerPlatformGlobals& tEMPGlobals = *gtSigmaGlobals.ptEMPGlobals;

    tEMPGlobals.fUnhandledExceptionCritSecInitialized = 1;
}

void EMPExceptionHandlerDispose() {
    ErrorManagerPlatformGlobals& tEMPGlobals = *gtSigmaGlobals.ptEMPGlobals;

    if (tEMPGlobals.fUnhandledExceptionCritSecInitialized != 0) {
        tEMPGlobals.fUnhandledExceptionCritSecInitialized = 0;
    }
}

// EMPOutputDebugString and ErrorManagerHandleRetryableGfxFailure are in the original file (lines
// 137 and 146) but have no address in the inventory, so they are not written here.

void EMPCrashForStack() {
    ErrorManagerGlobals& tEMGlobals = *gtSigmaGlobals.ptEMGlobals;
    (void)tEMGlobals;
}

void ExitProgram(ErrorCode eCode) {
    __builtin_trap();
}
