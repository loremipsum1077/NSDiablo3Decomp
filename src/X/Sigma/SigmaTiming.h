#pragma once

#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "SigmaTypes.h"

enum TimingMethod {
    TimingMethodNone = 0,
    TimingMethodQPF = 1,
    TimingMethodTickCount = 2,
};

void SigmaTimingInitialize(int64 lGameClockTicksPerSecond);
TimingMethod SigmaTimingGetMethod();
void SigmaTimingSetMethod(TimingMethod eTimingMethod);
uint32 SigmaGetGameClockTicks();

inline int64 SigmaTimingHighResTimer() {
    return Blizzard::Time::Nanoseconds();
}

inline uint32 GetMachineTicks() {
    return Blizzard::Time::Milliseconds();
}

namespace Blizzard { namespace Process { bool Sleep(Blizzard::Time::Millisecond milliseconds); } }  // BlizzardCore Process.h










































inline void Sleep(uint32 milli) {  // SigmaTiming.h:67
    Blizzard::Process::Sleep(milli);
}
