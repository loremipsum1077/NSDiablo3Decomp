// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/OSInfo.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ErrorManager/ErrorManager.h"
#include "SigmaTypes.h"

struct OSInfo {  // size 0x228
    BOOL fInitialized;  // 0x00
    OS_Type eOSType;  // 0x04
    BOOL fOSIs64Bit;  // 0x08
    uint32 uMajorVersion;  // 0x0c
    uint32 uMinorVersion;  // 0x10
    uint32 uBuild;  // 0x14
    uint32 uServicePack;  // 0x18
    char szOSDescription[256];  // 0x1c
    char szOSLanguage[256];  // 0x11c
    uint64 uTotalPhysicalMemory;  // 0x220
};
static_assert(sizeof(OSInfo) == 0x228, "OSInfo");
