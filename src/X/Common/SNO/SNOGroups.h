// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOGroups.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "SNOFiles.h"
#include "../../Sigma/Sigma.h"
#include "../../Sigma/Unicode.h"

class GroupDefinition;

enum {
    SNOGROUP_CAN_BE_COMPRESSED_BIT = 0,
    SNOGROUP_ALWAYS_LOADED_FOR_APP_PER_ACT = 1,
    SNOGROUP_ALWAYS_LOADED_FOR_APP_GLOBAL = 2,
    SNOGROUP_ALWAYS_LOADED_FOR_GAME_PER_ACT = 3,
    SNOGROUP_ALWAYS_LOADED_FOR_GAME_GLOBAL = 4,
    SNOGROUP_ALWAYS_LOADED_FOR_APP_PER_ACT_ON_HIGH_MEM_SYSTEM = 5,
    SNOGROUP_ALWAYS_LOADED_FOR_APP_GLOBAL_ON_HIGH_MEM_SYSTEM = 6,
    SNOGROUP_ALWAYS_LOADED_FOR_GAME_PER_ACT_ON_HIGH_MEM_SYSTEM = 7,
    SNOGROUP_ALWAYS_LOADED_FOR_GAME_GLOBAL_ON_HIGH_MEM_SYSTEM = 8,
    SNOGROUP_DUPLICATE_IN_CPKS = 9,
    SNOGROUP_ONLY_STORED_IN_COMMON_CPK = 10,
    SNOGROUP_STORE_CHILDS_AT_END_OF_CPK = 11,
    SNOGROUP_ENCRYPT_DATA_IN_CPK = 12,
    SNOGROUP_STORE_IN_CLIENT_COMMON_CPK = 13,
    SNOGROUP_STORE_IN_CORE_COMMON_CPK = 14,
    SNOGROUP_STORE_IN_SERVER_COMMON_CPK = 15,
    NUM_SNOGROUP_FLAGS = 16,
};

enum {
    AXE_BASE_SNO_VERSION = 2,
    NX64_BASE_SNO_VERSION = 1,
};

struct SNOVersions {  // size 0x14
    uint32 dwVersions[5];  // 0x00
    // Line 58. Callers (SNOGroups.cpp) show a zero loop at line 63, AXE gets 2 at line 66 and NX64
    // 1 at line 67, the same shape as SNOVersions(uint32) with 0.
    SNOVersions() {
        for (int32 i = 0; i < NUM_SIGMAPLATFORMS; i++)
            dwVersions[i] = 0;
        dwVersions[SIGMAPLATFORM_AXE] += AXE_BASE_SNO_VERSION;
        dwVersions[SIGMAPLATFORM_NX64] += NX64_BASE_SNO_VERSION;
    }
    // Line 70. The body spans lines 75-78; the folded constants in callers show every platform
    // gets dwGlobalVersion, AXE two more and NX64 one more. The exact statements are guessed.
    SNOVersions(uint32 dwGlobalVersion) {
        for (int32 i = 0; i < NUM_SIGMAPLATFORMS; i++)
            dwVersions[i] = dwGlobalVersion;
        dwVersions[SIGMAPLATFORM_AXE] += AXE_BASE_SNO_VERSION;
        dwVersions[SIGMAPLATFORM_NX64] += NX64_BASE_SNO_VERSION;
    }
    SNOVersions(uint32, bool);
    // Line 94, one store per line at 101-105. Callers end up with AXE two and NX64 one above the
    // *_SNO_VERSION enumerators of their .cpp; whether the base is added here or by the caller is
    // not recoverable, so it is left to the caller.
    SNOVersions(uint32 dwPCVersion, uint32 dwAXEVersion, uint32 dwXboxOneVersion, uint32 dwPS4Version, uint32 dwNX64Version) {
        dwVersions[SIGMAPLATFORM_PC] = dwPCVersion;
        dwVersions[SIGMAPLATFORM_AXE] = dwAXEVersion;
        dwVersions[SIGMAPLATFORM_XBOXONE] = dwXboxOneVersion;
        dwVersions[SIGMAPLATFORM_PS4] = dwPS4Version;
        dwVersions[SIGMAPLATFORM_NX64] = dwNX64Version;
    }
    SNOVersions(const SNOVersions &);
    // Line 117, body at 120. Callers only show dwVersions indexed by SigmaPlatformGet(); they pass
    // SIGMAPLATFORM_UNKNOWN, so the fallback to the running platform is assumed to be here.
    uint32 GetVersion(SigmaPlatform ePlatform) const {
        if (ePlatform == SIGMAPLATFORM_UNKNOWN)
            ePlatform = SigmaPlatformGet();
        return dwVersions[ePlatform];
    }
};
static_assert(sizeof(SNOVersions) == 0x14, "SNOVersions");

struct SNOGroupInfo {  // size 0x48
    CHAR szName[32];  // 0x00
    SNOGroup snoGroup;  // 0x20
    SNOVersions tVersions;  // 0x24
    uint32 dwFlags;  // 0x38
    SNODataType eSNODataType;  // 0x3c
    GroupDefinition * pdefGroup;  // 0x40
    // Line 126. Callers only show tVersions being constructed; no member is initialised here.
    SNOGroupInfo() {}
};
static_assert(sizeof(SNOGroupInfo) == 0x48, "SNOGroupInfo");


// Functions defined in this file (bodies are not in the DWARF):
//   line    58  SNOVersions::SNOVersions()  [inlined into 4 functions]
//   line    70  SNOVersions::SNOVersions(uint32 dwGlobalVersion)  [inlined into 19 functions]
//   line    84  SNOVersions::SNOVersions(uint32 dwGlobalVersion, bool)  [inlined into 2 functions]
//   line    94  SNOVersions::SNOVersions(uint32 dwPCVersion, uint32 dwAXEVersion, uint32 dwXboxOneVersion, uint32 dwPS4Version, uint32 dwNX64Version)  [inlined into 45 functions]
//   line   117  uint32 SNOVersions::GetVersion(SigmaPlatform ePlatform) const  [inlined into 15 functions]
//   line   126  SNOGroupInfo::SNOGroupInfo()  [inlined into 4 functions]

// SNOGroups.cpp:52 and :63 (X/Common/Game.cpp sFileSystemInitialize / sFileSystemDispose).
void SNOGroupsInitialize();
void SNOGroupsDispose();
