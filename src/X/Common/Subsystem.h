// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Subsystem.h

#pragma once

#include "../Sigma/Unicode.h"

typedef void (*SubsystemInitFunc)();

typedef void (*SubsystemDisposeFunc)();

struct SubsystemSetup {  // size 0x18
    LPCSTR sSubSystemName;  // 0x00
    SubsystemInitFunc pfnInit;  // 0x08
    SubsystemDisposeFunc pfnDispose;  // 0x10
};
static_assert(sizeof(SubsystemSetup) == 0x18, "SubsystemSetup");

void SubsystemsInitialize(const SubsystemSetup * ptSubsystems, const int32 & nNumSubsystems);
void SubsystemsDispose(const SubsystemSetup * ptSubsystems, const int32 & nNumSubsystems);
