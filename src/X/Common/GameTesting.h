// Original file: X/Common/GameTesting.h
// The DWARF records only ScreenshotView for this header; the GameTesting
// functions below are defined in GameTesting.cpp (line numbers are its) and
// are declared here for their callers.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"
#include "Shell/CmdLine.h"

enum ScreenshotView {
    SV_INVALID = 0,
    SV_FRONT = 1,
    SV_BACK = 2,
    SV_GAME = 3,
};

namespace GameTesting {
void SetTestType(const TestType eTestType);  // line 465
void SetDumpFPSFequency(uint32 freq);  // line 646
void SetMinFPSToTriggerScreenshot(uint32 minFPS);  // line 688
void SetSampleTime(uint32 sampleTimeSecs);  // line 709
void SetWorldAdvanceWaitTime(uint32 waitTimeSecs);  // line 764
void SetFilter(LPCSTR szFilter);  // line 1104
void SetListPath(LPCSTR szListPath);  // line 1169
void SetEntryPoint(LPCSTR entryPoint);  // line 1229
}  // namespace GameTesting
