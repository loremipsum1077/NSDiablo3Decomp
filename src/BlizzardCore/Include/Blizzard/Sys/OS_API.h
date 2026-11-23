// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Sys/OS_API.h

#pragma once

#include "../Types.h"

namespace OS_API {
struct OSInfo {  // size 0xb8
    uint32 versionMajor;  // 0x00
    uint32 versionMinor;  // 0x04
    uint32 versionRevision;  // 0x08
    uint32 servicePackMajor;  // 0x0c
    uint32 servicePackMinor;  // 0x10
    char build[32];  // 0x14
    char productName[128];  // 0x34
    bool is64Bit;  // 0xb4
};
static_assert(sizeof(OSInfo) == 0xb8, "OSInfo");

// OS_API_NX64.cpp:21
void GetOSInfo(OSInfo * osInfo);

}  // namespace OS_API
