// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformTiming/bdPlatformTiming.cpp

#include "bdPlatformTiming.h"
#include <chrono>
#include <thread>















bdUInt64 bdPlatformTiming::getHiResTimeStamp()
{
    const std::chrono::nanoseconds timestamp = std::chrono::steady_clock::now().time_since_epoch();
    const long long usTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(timestamp).count();
    return usTimestamp;
}


bdFloat32 bdPlatformTiming::getElapsedTime(const bdUInt64 time1, const bdUInt64 time2)
{
    if (time2 < time1)
    {
        return 0.0f;
    }
    const bdUInt64 elapsedTime = time2 - time1;
    return static_cast<bdFloat32>(elapsedTime) / 1000000.0f;
}






void bdPlatformTiming::sleep(bdUInt timeInMs)
{
    if (timeInMs != 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(timeInMs));
}

bdUInt32 bdPlatformTiming::getLoResTimeStamp()
{
    const std::chrono::nanoseconds timestamp = std::chrono::steady_clock::now().time_since_epoch();
    const long long msTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp).count();
    return static_cast<bdUInt32>(msTimestamp);
}


bdUInt32 bdPlatformTiming::getLoResElapsedTime(const bdUInt32 time1, const bdUInt32 time2)
{
    bdUInt64 time2adjusted = time2;
    if (time2 < time1)
    {
        time2adjusted += 0xFFFFFFFF;
    }
    return static_cast<bdUInt32>((time2adjusted - time1) / 1000);
}
