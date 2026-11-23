#pragma once

#include "SigmaTypes.h"
#include "Unicode.h"

struct ErrorManagerGlobals;
struct ErrorManagerPlatformGlobals;
struct GamePerfGlobals;
struct MemoryPools;
struct RefStringGlobals;

enum {
    KILO = 1024,
    MEG = 1048576,
    GIG = 1073741824,
    MACHINE_TICKS_PER_SECOND = 1000,
    NONE = -1,
};

enum Ternary {
    TERNARY_TRUE = 1,
    TERNARY_FALSE = 0,
    TERNARY_NA = -1,
};

struct SigmaGlobals {
    MemoryPools* ptMemoryPools;
    ErrorManagerGlobals* ptEMGlobals;
    ErrorManagerPlatformGlobals* ptEMPGlobals;
    GamePerfGlobals* ptGamePerfGlobals;
    RefStringGlobals* ptRefStringGlobals;
};

enum SigmaPlatform {
    SIGMAPLATFORM_UNKNOWN = -1,
    SIGMAPLATFORM_PC = 0,
    SIGMAPLATFORM_AXE = 1,
    SIGMAPLATFORM_XBOXONE = 2,
    SIGMAPLATFORM_PS4 = 3,
    SIGMAPLATFORM_NX64 = 4,
    NUM_SIGMAPLATFORMS = 5,
};

extern SigmaGlobals gtSigmaGlobals;
extern SigmaPlatform geSigmaPlatform;

// Lines 445 and 451. Bodies written from X/Common/Definitions/Definitions.cpp, where the console
// test compiles to one unsigned range check over XBOXONE..NX64.
inline SigmaPlatform SigmaPlatformGet() { return geSigmaPlatform; }
// Line 437. Body written from SNOFilesInitialize (SNOFiles.cpp:2016), where it is inlined.
inline SigmaPlatform SigmaPlatformSet(SigmaPlatform eSigmaPlatform) {
    geSigmaPlatform = eSigmaPlatform;
    return geSigmaPlatform;
}
inline BOOL SigmaPlatformIsConsole(SigmaPlatform eSigmaPlatform) {
    return eSigmaPlatform == SIGMAPLATFORM_XBOXONE || eSigmaPlatform == SIGMAPLATFORM_PS4 ||
           eSigmaPlatform == SIGMAPLATFORM_NX64;
}

void SigmaInit(LPCSTR szApplicationName, uint32 dwInspectorProjectID);
void SigmaDispose();

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(SigmaGlobals) == 0x28, "SigmaGlobals");
static_assert(offsetof(SigmaGlobals, ptMemoryPools) == 0x0, "SigmaGlobals");
static_assert(offsetof(SigmaGlobals, ptEMGlobals) == 0x8, "SigmaGlobals");
static_assert(offsetof(SigmaGlobals, ptEMPGlobals) == 0x10, "SigmaGlobals");
static_assert(offsetof(SigmaGlobals, ptGamePerfGlobals) == 0x18, "SigmaGlobals");
static_assert(offsetof(SigmaGlobals, ptRefStringGlobals) == 0x20, "SigmaGlobals");
