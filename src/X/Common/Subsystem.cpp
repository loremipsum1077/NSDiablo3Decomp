// Original file: X/Common/Subsystem.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"

#include "Subsystem.h"
#include "Shell/CmdLine.h"

void SubsystemsInitialize(const SubsystemSetup * ptSubsystemSetup, const int32 & nNumSubsystems)
{
    for (int32 i = 0; i < nNumSubsystems; i++) {
        const SubsystemSetup & tSubsystemSetup = ptSubsystemSetup[i];
        if (tSubsystemSetup.pfnInit) {
            // The original wrapped pfnInit in a command-line-flag-gated trace (sSubSystemName
            // exists for it) that is compiled out of this build. Only the opaque
            // CmdLineGetParams() call survives; its result is unused in the binary. The
            // gating bit is not recoverable from the DWARF, so the bare call is kept.
            CmdLineGetParams();
            tSubsystemSetup.pfnInit();
            CmdLineGetParams();
        }
    }
}

void SubsystemsDispose(const SubsystemSetup * ptSubsystemSetup, const int32 & nNumSubSystems)
{
    for (int32 i = nNumSubSystems; i > 0; i--) {
        const SubsystemSetup & tSubsystemSetup = ptSubsystemSetup[i - 1];
        if (tSubsystemSetup.pfnDispose)
            tSubsystemSetup.pfnDispose();
    }
}
