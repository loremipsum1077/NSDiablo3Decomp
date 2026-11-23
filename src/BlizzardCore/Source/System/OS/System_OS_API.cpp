// Original file: BlizzardCore/Source/System/OS/System_OS_API.cpp
// The original is a unity file; its one function is from OS_API_NX64.cpp.
#include "../../../Include/Blizzard/Sys/OS_API.h"

#include <stddef.h>
#include <string.h>

namespace OS_API {

// GetOSInfo is OS_API_NX64.cpp:21 in the original unity file; laid on its DWARF lines.










void GetOSInfo(OSInfo * osInfo)
{
    if (!osInfo)    // line 23 col 9 (space-indented in the original)
        return;

    osInfo->versionMajor = 1;
    osInfo->versionMinor = 0;
    osInfo->versionRevision = 0;
    osInfo->servicePackMajor = 0;
    osInfo->servicePackMinor = 0;
    strcpy(osInfo->build, "0");
    osInfo->is64Bit = true;
    strcpy(osInfo->productName, "NX64");
}
}  // namespace OS_API

