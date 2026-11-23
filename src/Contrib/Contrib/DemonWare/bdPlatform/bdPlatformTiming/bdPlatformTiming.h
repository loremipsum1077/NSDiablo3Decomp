// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformTiming/bdPlatformTiming.h

#pragma once

#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdPlatformTiming {  // size 0x1
public:
    static bdUInt64 getHiResTimeStamp();
    static bdFloat32 getElapsedTime(const bdUInt64 time1, const bdUInt64 time2);
    static void sleep(bdUInt timeInMs);
    static bdUInt32 getLoResTimeStamp();
    static bdUInt32 getLoResElapsedTime(const bdUInt32 time1, const bdUInt32 time2);
    static bdInt64 getPOSIXTimestamp();
private:
    bdPlatformTiming();
};
static_assert(sizeof(bdPlatformTiming) == 0x1, "bdPlatformTiming");

