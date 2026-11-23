#include "SigmaTiming.h"

#include "ErrorManager/ErrorManager.h"
#include "Unicode.h"

enum ClockTestResult {
    ClockTestResult_Success = 0,
    ClockTestResult_QueryPerformanceFrequencyFailed = 1,
    ClockTestResult_QueryPerformanceFrequencyReturnedZero = 2,
    ClockTestResult_QueryPerformanceCounterWentBackwards = 3,
    ClockTestResult_QueryPerformanceCounterInaccurate = 4,
};

struct SigmaTimingGlobals {
    int64 lFrequency;
    int64 lGameClockTicksPerSecond;
    int64 lGameClockFrequency;
    int32 nGameClockTicksPerMillisecond;
    ClockTestResult eClockTestResult;
    TimingMethod eActiveTimingMethod;
};

static SigmaTimingGlobals sgSigmaTimingGlobals;

void SigmaTimingInitialize(int64 lGameClockTicksPerSecond) {
    sgSigmaTimingGlobals.eActiveTimingMethod = TimingMethodQPF;
    sgSigmaTimingGlobals.lFrequency = 1000000000;
    sgSigmaTimingGlobals.lGameClockTicksPerSecond = lGameClockTicksPerSecond;
    sgSigmaTimingGlobals.lGameClockFrequency = sgSigmaTimingGlobals.lFrequency / lGameClockTicksPerSecond;
    sgSigmaTimingGlobals.nGameClockTicksPerMillisecond = lGameClockTicksPerSecond / 1000;
}

TimingMethod SigmaTimingGetMethod() {
    return sgSigmaTimingGlobals.eActiveTimingMethod;
}

// LINKER-REMOVED: (DWARF: line 291, low_pc 0, 12 bytes). It keeps eActiveTimingMethod a
// full word that can hold any TimingMethod (docs/late_globalopt.md).
void SigmaTimingSetMethod(TimingMethod eTimingMethod) {
    sgSigmaTimingGlobals.eActiveTimingMethod = eTimingMethod;
}

// LINKER-REMOVED: (DWARF: line 298, low_pc 0, 100 bytes, frame pointer, returns int64).
// Its read of lFrequency is what keeps that global (as a byte) through the late GlobalOpt. The
// body is reconstructed on the pattern of SigmaGetGameClockTicks; it compiles to 100 bytes.
int64 SigmaGetClockFrequency() {
    switch (sgSigmaTimingGlobals.eActiveTimingMethod) {
    case TimingMethodQPF:
        return sgSigmaTimingGlobals.lFrequency;
    case TimingMethodTickCount:
        return 1000;
    default:
        DisplayInternalError(NULL, "", 307, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

uint32 SigmaGetGameClockTicks() {
    switch (sgSigmaTimingGlobals.eActiveTimingMethod) {
    case TimingMethodQPF:
        return SigmaTimingHighResTimer() / sgSigmaTimingGlobals.lGameClockFrequency;
    case TimingMethodTickCount:
        return GetMachineTicks() * sgSigmaTimingGlobals.nGameClockTicksPerMillisecond;
    default:
        DisplayInternalError(NULL, "", 338, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}
